#!/usr/bin/env python3
"""Overlay latency-derived V2X mobility signal strength on an OSM-derived SVG map."""

from __future__ import annotations

import argparse
import csv
import math
import re
from collections import defaultdict
from pathlib import Path

import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.tri as mtri  # noqa: E402

from overlay_experiment_markers_on_svg import V2X_RSU, escape_xml, fit_svg_transform, label_svg


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_INPUT_SVG = ROOT / "paper" / "figs" / "map_buildings_orange_hatched.svg"
DEFAULT_OUTPUT_SVG = ROOT / "paper" / "figs" / "map_buildings_orange_hatched_v2x_mobility_signal.svg"
DEFAULT_OSM = ROOT / "map.osm"
DEFAULT_POINTS_CSV = ROOT / "paper" / "figs" / "map_buildings_orange_hatched_v2x_mobility_signal_points.csv"
MOBILITY_JOIN_GLOB = "20260704_exp_02_radio_distance_mobility_*/v2x_gnss_by_send_time.csv"
STATIONARY_RUNS = (
    ("1", "20260704_exp_01_payload_sweep_0_2kb_133527"),
    ("2", "20260704_exp_01_payload_sweep_0_2kb_144149"),
    ("3", "20260704_exp_01_payload_sweep_0_2kb_151351"),
    ("4", "20260704_exp_01_payload_sweep_0_2kb_155120"),
    ("5", "20260704_exp_01_payload_sweep_0_2kb_164129"),
)


def read_joined_mobility_rows():
    rows = []
    for path in sorted((ROOT / "results" / "mocar_v2x").glob(MOBILITY_JOIN_GLOB)):
        run_id = path.parent.name.rsplit("_", 1)[-1]
        with path.open(newline="") as handle:
            for row in csv.DictReader(handle):
                if not row.get("send_latitude_deg") or not row.get("send_longitude_deg"):
                    continue
                success = row.get("success", "").lower() == "true"
                rtt_ms = float(row["rtt_ms"]) if success and row.get("rtt_ms") else None
                rows.append(
                    {
                        "run_id": run_id,
                        "sequence": int(row["sequence"]),
                        "success": success,
                        "rtt_ms": rtt_ms,
                        "timeout_ms": float(row["timeout_ms"]) if row.get("timeout_ms") else None,
                        "latitude": float(row["send_latitude_deg"]),
                        "longitude": float(row["send_longitude_deg"]),
                        "gnss_delta_ms": float(row["gnss_delta_ms"]) if row.get("gnss_delta_ms") else None,
                    }
                )
    if not rows:
        raise RuntimeError("No joined V2X mobility/GNSS rows found")
    return rows


def read_stationary_runs():
    runs = []
    for label, run_id in STATIONARY_RUNS:
        summary_path = ROOT / "results" / "mocar_v2x" / run_id / "summary.md"
        summary = summary_path.read_text(errors="replace")
        lat_match = re.search(r"Mean GNSS latitude:\s*`?([-0-9.]+)`?", summary)
        lon_match = re.search(r"Mean GNSS longitude:\s*`?([-0-9.]+)`?", summary)
        if not lat_match or not lon_match:
            raise RuntimeError(f"Missing mean GNSS location in {summary_path}")
        runs.append(
            {
                "label": label,
                "latitude": float(lat_match.group(1)),
                "longitude": float(lon_match.group(1)),
            }
        )
    return runs


def latency_strength(row):
    if not row["success"]:
        return 0.0
    # The mobility runs have successful-packet medians near 28-29 ms and p95
    # around 42-44 ms. Map low RTT to strong link quality, but keep high-latency
    # successful packets separate from packet failures.
    low_ms = 20.0
    weak_ms = 60.0
    rtt = row["rtt_ms"]
    if rtt <= low_ms:
        return 1.0
    if rtt >= weak_ms:
        return 0.10
    return 1.0 - 0.90 * ((rtt - low_ms) / (weak_ms - low_ms))


def quality_color(value):
    stops = [
        (0.00, (197, 27, 53)),
        (0.35, (253, 174, 97)),
        (0.60, (255, 255, 191)),
        (0.78, (166, 217, 106)),
        (1.00, (0, 104, 55)),
    ]
    value = max(0.0, min(1.0, value))
    for (lo, c0), (hi, c1) in zip(stops, stops[1:]):
        if lo <= value <= hi:
            t = (value - lo) / (hi - lo)
            rgb = tuple(round(c0[i] + (c1[i] - c0[i]) * t) for i in range(3))
            return "#{:02x}{:02x}{:02x}".format(*rgb)
    return "#c51b35"


def aggregate_points(rows, project, grid_size=12.0):
    buckets = defaultdict(list)
    for row in rows:
        x, y = project(row["latitude"], row["longitude"])
        row = {**row, "x": x, "y": y, "strength": latency_strength(row)}
        key = (round(x / grid_size), round(y / grid_size), row["run_id"])
        buckets[key].append(row)

    points = []
    for records in buckets.values():
        successes = [record for record in records if record["success"]]
        failures = [record for record in records if not record["success"]]
        rtts = sorted(record["rtt_ms"] for record in successes if record["rtt_ms"] is not None)
        strength = sum(record["strength"] for record in records) / len(records)
        points.append(
            {
                "x": sum(record["x"] for record in records) / len(records),
                "y": sum(record["y"] for record in records) / len(records),
                "latitude": sum(record["latitude"] for record in records) / len(records),
                "longitude": sum(record["longitude"] for record in records) / len(records),
                "run_id": records[0]["run_id"],
                "samples": len(records),
                "successes": len(successes),
                "failures": len(failures),
                "failure_rate": len(failures) / len(records),
                "mean_strength": strength,
                "median_rtt_ms": rtts[len(rtts) // 2] if rtts else None,
            }
        )
    return sorted(points, key=lambda point: (point["run_id"], point["y"], point["x"]))


def convex_hull(points):
    unique = sorted({(point["x"], point["y"]) for point in points})
    if len(unique) <= 1:
        return unique

    def cross(origin, a, b):
        return (a[0] - origin[0]) * (b[1] - origin[1]) - (a[1] - origin[1]) * (b[0] - origin[0])

    lower = []
    for point in unique:
        while len(lower) >= 2 and cross(lower[-2], lower[-1], point) <= 0:
            lower.pop()
        lower.append(point)

    upper = []
    for point in reversed(unique):
        while len(upper) >= 2 and cross(upper[-2], upper[-1], point) <= 0:
            upper.pop()
        upper.append(point)

    return lower[:-1] + upper[:-1]


def point_in_polygon(x, y, polygon):
    inside = False
    j = len(polygon) - 1
    for i, point_i in enumerate(polygon):
        xi, yi = point_i
        xj, yj = polygon[j]
        crosses = (yi > y) != (yj > y)
        if crosses:
            x_at_y = (xj - xi) * (y - yi) / (yj - yi + 1e-12) + xi
            if x < x_at_y:
                inside = not inside
        j = i
    return inside


def interpolate_heatmap_cells(points, cell_size=12.0):
    xs = np.array([point["x"] for point in points], dtype=float)
    ys = np.array([point["y"] for point in points], dtype=float)
    values = np.array([point["mean_strength"] for point in points], dtype=float)
    weights = np.array([max(point["samples"], 1) for point in points], dtype=float)
    hull = convex_hull(points)
    min_x = float(xs.min() - cell_size)
    max_x = float(xs.max() + cell_size)
    min_y = float(ys.min() - cell_size)
    max_y = float(ys.max() + cell_size)

    triangulation = mtri.Triangulation(xs, ys)
    interpolator = mtri.LinearTriInterpolator(triangulation, values)
    cells = []
    y = min_y
    while y <= max_y:
        x = min_x
        while x <= max_x:
            cx = x + cell_size / 2.0
            cy = y + cell_size / 2.0
            # Fill the triangulated mobility envelope, including the center of
            # the route loop, while keeping cells outside the measured envelope
            # unpainted.
            if point_in_polygon(cx, cy, hull):
                interpolated = interpolator(cx, cy)
                if np.ma.is_masked(interpolated):
                    x += cell_size
                    continue
                distances = np.hypot(xs - cx, ys - cy)
                nearest = float(distances.min())
                support = int(weights[np.argsort(distances)[:12]].sum())
                cells.append(
                    {
                        "x": x,
                        "y": y,
                        "width": cell_size * 1.04,
                        "height": cell_size * 1.04,
                        "value": float(interpolated),
                        "nearest_distance": nearest,
                        "support": support,
                    }
                )
            x += cell_size
        y += cell_size
    return cells


def write_points_csv(points, output_path):
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", newline="") as handle:
        fieldnames = [
            "run_id",
            "latitude",
            "longitude",
            "samples",
            "successes",
            "failures",
            "failure_rate",
            "mean_strength",
            "median_rtt_ms",
        ]
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for point in points:
            writer.writerow(
                {
                    "run_id": point["run_id"],
                    "latitude": f'{point["latitude"]:.9f}',
                    "longitude": f'{point["longitude"]:.9f}',
                    "samples": point["samples"],
                    "successes": point["successes"],
                    "failures": point["failures"],
                    "failure_rate": f'{point["failure_rate"]:.3f}',
                    "mean_strength": f'{point["mean_strength"]:.3f}',
                    "median_rtt_ms": "" if point["median_rtt_ms"] is None else f'{point["median_rtt_ms"]:.3f}',
                }
            )


def mobility_overlay_group(points, cells, stationary_runs, project):
    x, y = project(V2X_RSU["latitude"], V2X_RSU["longitude"])

    out = [
        '<g id="v2x-mobility-signal-strength-overlay" font-family="Arial, sans-serif">',
        "  <title>Latency-derived V2X mobility signal-strength overlay</title>",
        "  <desc>Moving-run packets joined to GNSS. Color encodes a latency-derived signal-strength score: low RTT is stronger; failed packets contribute zero.</desc>",
    ]

    out.append('  <g id="v2x-interpolated-heatmap-cells">')
    for cell in cells:
        color = quality_color(cell["value"])
        title = (
            f'interpolated strength {cell["value"]:.2f}; '
            f'nearest mobility sample {cell["nearest_distance"]:.1f} SVG units; '
            f'local packet support {cell["support"]}'
        )
        out.append(
            f'    <rect x="{cell["x"]:.2f}" y="{cell["y"]:.2f}" width="{cell["width"]:.2f}" '
            f'height="{cell["height"]:.2f}" fill="{color}" opacity="0.56" stroke="none">'
            f"<title>{escape_xml(title)}</title></rect>"
        )
    out.append("  </g>")

    out.append('  <g id="v2x-stationary-run-markers">')
    for run in stationary_runs:
        run_x, run_y = project(run["latitude"], run["longitude"])
        out.append(
            f'    <path d="M {run_x:.2f} {run_y - 27:.2f} L {run_x + 27:.2f} {run_y:.2f} '
            f'L {run_x:.2f} {run_y + 27:.2f} L {run_x - 27:.2f} {run_y:.2f} Z" '
            f'fill="#762a83" stroke="white" stroke-width="5.0">'
            f'<title>V2X stationary run {run["label"]}</title></path>'
        )
        out.append(
            f'    <text x="{run_x:.2f}" y="{run_y + 9:.2f}" font-size="29" font-weight="800" '
            f'fill="white" text-anchor="middle">{run["label"]}</text>'
        )
    out.append("  </g>")

    out.append(
        f'  <polygon points="{x:.2f},{y - 30:.2f} {x - 28:.2f},{y + 23:.2f} {x + 28:.2f},{y + 23:.2f}" '
        f'fill="#111827" stroke="#ffffff" stroke-width="5.0"><title>V2X RSU</title></polygon>'
    )
    out.append(label_svg(x, y, "RSU 0", dx=42, dy=10, size=36, fill="#111827", weight="700"))

    bar_x = 1008
    bar_y = 250
    bar_w = 42
    bar_h = 710
    steps = 88
    out.extend(
        [
            f'  <g id="v2x-signal-strength-key">',
            f'    <rect x="{bar_x - 18}" y="{bar_y - 72}" width="170" height="{bar_h + 154}" '
            f'fill="white" fill-opacity="0.88" stroke="#111827" stroke-width="2.0"/>',
        ]
    )
    for idx in range(steps):
        value = 1.0 - idx / (steps - 1)
        y0 = bar_y + idx * (bar_h / steps)
        out.append(
            f'    <rect x="{bar_x}" y="{y0:.2f}" width="{bar_w}" height="{bar_h / steps + 0.5:.2f}" '
            f'fill="{quality_color(value)}" stroke="none"/>'
        )
    out.extend(
        [
            f'    <rect x="{bar_x}" y="{bar_y}" width="{bar_w}" height="{bar_h}" fill="none" '
            f'stroke="#111827" stroke-width="2.6"/>',
            f'    <text x="{bar_x + bar_w + 26}" y="{bar_y + 16}" font-size="34" font-weight="700" '
            f'text-anchor="start" fill="#111111">1.0</text>',
            f'    <text x="{bar_x + bar_w + 26}" y="{bar_y + bar_h + 10}" font-size="34" font-weight="700" '
            f'text-anchor="start" fill="#111111">0.0</text>',
            f'    <text x="{bar_x + bar_w / 2}" y="{bar_y - 61}" font-size="34" '
            f'text-anchor="middle" fill="#111111">strong</text>',
            f'    <text x="{bar_x + bar_w / 2}" y="{bar_y - 29}" font-size="38" font-weight="700" '
            f'text-anchor="middle" fill="#111111">V2X</text>',
            f'    <text x="{bar_x + bar_w / 2}" y="{bar_y + 7}" font-size="38" font-weight="700" '
            f'text-anchor="middle" fill="#111111">signal</text>',
            f'    <text x="{bar_x + bar_w / 2}" y="{bar_y + bar_h + 48}" font-size="34" '
            f'text-anchor="middle" fill="#111111">weak</text>',
            "  </g>",
            '  <g id="v2x-stationary-key" transform="translate(88 1158)">',
            '    <rect x="0" y="0" width="390" height="58" fill="white" fill-opacity="0.88" stroke="#111827" stroke-width="2.0"/>',
            '    <path d="M 43 8 L 64 29 L 43 50 L 22 29 Z" fill="#762a83" stroke="white" stroke-width="3.0"/>',
            '    <text x="43" y="39" font-size="25" font-weight="800" text-anchor="middle" fill="white">1</text>',
            '    <text x="84" y="42" font-size="34" font-weight="700" fill="#111111">stationary runs</text>',
            "  </g>",
            '  <g id="v2x-building-key" transform="translate(88 1230)">',
            '    <rect x="0" y="0" width="390" height="58" fill="white" fill-opacity="0.88" stroke="#111827" stroke-width="2.0"/>',
            '    <rect x="20" y="14" width="46" height="30" fill="url(#orange-building-hatch)" stroke="#ff8c00" stroke-width="3.0"/>',
            '    <text x="84" y="42" font-size="34" font-weight="700" fill="#111111">building footprints</text>',
            "  </g>",
            "</g>",
        ]
    )
    return "\n".join(out)


def overlay_svg(input_svg: Path, output_svg: Path, osm_path: Path, points_csv: Path):
    svg_text = input_svg.read_text()
    project, error = fit_svg_transform(svg_text, osm_path)
    rows = read_joined_mobility_rows()
    stationary_runs = read_stationary_runs()
    points = aggregate_points(rows, project)
    cells = interpolate_heatmap_cells(points)
    write_points_csv(points, points_csv)
    overlay = mobility_overlay_group(points, cells, stationary_runs, project)
    if "</svg>" not in svg_text:
        raise RuntimeError(f"{input_svg} does not look like an SVG file")
    svg_text = re.sub(
        r'<svg([^>]*)width="1000" height="1666" viewBox="0 0 1000 1666"',
        r'<svg\1width="1100" height="1410" viewBox="80 -40 1100 1410"',
        svg_text,
        count=1,
    )
    svg_text = re.sub(
        r"(<svg[^>]*>)",
        r'\1\n  <rect x="80" y="-40" width="1100" height="1410" fill="white"/>',
        svg_text,
        count=1,
    )
    output_svg.write_text(svg_text.replace("</svg>", overlay + "\n</svg>"))
    return error, rows, points, cells


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input-svg", type=Path, default=DEFAULT_INPUT_SVG)
    parser.add_argument("--output-svg", type=Path, default=DEFAULT_OUTPUT_SVG)
    parser.add_argument("--osm", type=Path, default=DEFAULT_OSM)
    parser.add_argument("--points-csv", type=Path, default=DEFAULT_POINTS_CSV)
    args = parser.parse_args()
    error, rows, points, cells = overlay_svg(args.input_svg, args.output_svg, args.osm, args.points_csv)
    successes = sum(1 for row in rows if row["success"])
    print(args.output_svg)
    print(args.points_csv)
    print(
        "fit pairs={pairs} rmse=({rmse_x:.4f},{rmse_y:.4f}) max=({max_abs_x:.4f},{max_abs_y:.4f})".format(
            **error
        )
    )
    print(f"joined packets={len(rows)} successes={successes} failures={len(rows) - successes} bins={len(points)} cells={len(cells)}")


if __name__ == "__main__":
    main()
