#!/usr/bin/env python3
"""Overlay experiment markers on an existing OSM-derived SVG map."""

from __future__ import annotations

import argparse
import csv
import math
import re
import xml.etree.ElementTree as ET
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_INPUT_SVG = ROOT / "paper" / "figs" / "map_buildings_orange_hatched.svg"
DEFAULT_OUTPUT_SVG = ROOT / "paper" / "figs" / "map_buildings_orange_hatched_overlays.svg"
DEFAULT_OSM = ROOT / "map.osm"

V2X_RSU = {
    "label": "V2X RSU",
    "latitude": 39.66711689714955,
    "longitude": -75.75772185598035,
}


def read_csv(path: Path):
    with path.open(newline="") as handle:
        return list(csv.DictReader(handle))


def load_signal_points():
    points = []
    for row in read_csv(ROOT / "results" / "real_5g" / "signal_measurements_parsed.csv"):
        points.append(
            {
                "label": row["id"],
                "latitude": float(row["latitude"]),
                "longitude": float(row["longitude"]),
                "is_base_station": row["is_base_station"].lower() == "true",
                "rsrp_dbm": float(row["rsrp_dbm"]) if row["rsrp_dbm"] else None,
                "snr_db": float(row["snr_db"]) if row["snr_db"] else None,
            }
        )
    return points


def load_5g_stationary_runs():
    runs = []
    for row in read_csv(ROOT / "results" / "real_5g" / "run_locations_parsed.csv"):
        if row.get("included", "").lower() != "true":
            continue
        runs.append(
            {
                "label": row["label"].replace("Run ", "5G S"),
                "latitude": float(row["latitude"]),
                "longitude": float(row["longitude"]),
            }
        )
    return runs


def load_v2x_stationary_runs():
    runs = []
    summaries = sorted(
        (ROOT / "results" / "mocar_v2x").glob("*exp_01_payload_sweep*/summary.md")
    )
    point_summaries = [
        summary for summary in summaries if summary.parent.name.startswith("20260704_")
    ]
    for summary in summaries:
        text = summary.read_text(errors="replace")
        lat_match = re.search(r"Mean GNSS latitude:\s*`?([-0-9.]+)`?", text)
        lon_match = re.search(r"Mean GNSS longitude:\s*`?([-0-9.]+)`?", text)
        if not lat_match or not lon_match:
            continue
        if summary in point_summaries:
            marker = str(point_summaries.index(summary) + 1)
            label = f"V2X Point {marker}"
        else:
            marker = "R"
            label = "V2X reference location"
        runs.append(
            {
                "label": label,
                "marker": marker,
                "latitude": float(lat_match.group(1)),
                "longitude": float(lon_match.group(1)),
            }
        )
    return runs


def parse_osm_nodes_ways(osm_path: Path):
    root = ET.parse(osm_path).getroot()
    nodes = {
        node.attrib["id"]: (float(node.attrib["lat"]), float(node.attrib["lon"]))
        for node in root.findall("node")
        if "id" in node.attrib and "lat" in node.attrib and "lon" in node.attrib
    }
    ways = {}
    for way in root.findall("way"):
        way_id = way.attrib.get("id")
        if not way_id:
            continue
        refs = [nd.attrib["ref"] for nd in way.findall("nd") if nd.attrib.get("ref") in nodes]
        if refs:
            ways[way_id] = refs
    return nodes, ways


def extract_svg_way_points(svg_text: str):
    pattern = re.compile(r'<path id="way-(\d+)" d="([^"]+)"')
    result = {}
    for way_id, d_attr in pattern.findall(svg_text):
        nums = [float(value) for value in re.findall(r"[-+]?(?:\d+\.\d+|\d+)", d_attr)]
        result[way_id] = list(zip(nums[0::2], nums[1::2]))
    return result


def fit_svg_transform(svg_text: str, osm_path: Path):
    nodes, ways = parse_osm_nodes_ways(osm_path)
    svg_way_points = extract_svg_way_points(svg_text)
    rows = []
    for way_id, refs in ways.items():
        points = svg_way_points.get(way_id)
        if not points or len(points) != len(refs):
            continue
        for (x, y), ref in zip(points, refs):
            lat, lon = nodes[ref]
            rows.append((lat, lon, x, y))

    if len(rows) < 6:
        raise RuntimeError("Could not match enough OSM way points to fit SVG transform")

    a = np.array([[lon, lat, 1.0] for lat, lon, _, _ in rows])
    bx = np.array([x for _, _, x, _ in rows])
    by = np.array([y for _, _, _, y in rows])
    cx = np.linalg.lstsq(a, bx, rcond=None)[0]
    cy = np.linalg.lstsq(a, by, rcond=None)[0]

    def project(lat: float, lon: float):
        return float(cx @ np.array([lon, lat, 1.0])), float(cy @ np.array([lon, lat, 1.0]))

    residual_x = bx - a @ cx
    residual_y = by - a @ cy
    error = {
        "pairs": len(rows),
        "rmse_x": float(np.sqrt(np.mean(residual_x * residual_x))),
        "rmse_y": float(np.sqrt(np.mean(residual_y * residual_y))),
        "max_abs_x": float(np.max(np.abs(residual_x))),
        "max_abs_y": float(np.max(np.abs(residual_y))),
    }
    return project, error


def color_for_rsrp(rsrp_dbm: float):
    # Piecewise red-yellow-green palette matching the PNG figure.
    stops = [
        (-122.0, (197, 27, 53)),
        (-112.0, (253, 174, 97)),
        (-104.0, (255, 255, 191)),
        (-96.0, (166, 217, 106)),
        (-90.0, (0, 104, 55)),
    ]
    value = max(stops[0][0], min(stops[-1][0], rsrp_dbm))
    for (lo, c0), (hi, c1) in zip(stops, stops[1:]):
        if lo <= value <= hi:
            t = (value - lo) / (hi - lo)
            rgb = tuple(round(c0[i] + (c1[i] - c0[i]) * t) for i in range(3))
            return "#{:02x}{:02x}{:02x}".format(*rgb)
    return "#fdae61"


def label_svg(x, y, text, dx=8, dy=-8, size=16, fill="#222222", weight="600"):
    # White stroke keeps labels readable on roads/building hatches.
    return (
        f'<text x="{x + dx:.2f}" y="{y + dy:.2f}" font-family="Arial, sans-serif" '
        f'font-size="{size}" font-weight="{weight}" fill="{fill}" '
        f'stroke="white" stroke-width="4" paint-order="stroke">{escape_xml(text)}</text>'
    )


def signal_label_offset(x, y):
    dx = 13
    dy = -13
    if y < 35:
        dy = 32
    elif y > 1610:
        dy = -26
    if x < 35:
        dx = 16
    elif x > 1000:
        dx = -68
    return dx, dy


def escape_xml(text: str):
    return (
        text.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


def marker_group(project):
    signal_points = load_signal_points()
    fiveg_base = [point for point in signal_points if point["is_base_station"]]
    signal_measurements = [
        point
        for point in signal_points
        if not point["is_base_station"] and point["rsrp_dbm"] is not None and point["snr_db"] is not None
    ]
    fiveg_stationary = load_5g_stationary_runs()
    v2x_stationary = load_v2x_stationary_runs()

    out = [
        '<g id="experiment-overlays" font-family="Arial, sans-serif">',
        '  <title>5G signal, SNR, base-station, V2X RSU, and stationary run overlays</title>',
        '  <desc>5G signal survey markers encode RSRP by color and SNR by adjacent numeric label. Other markers show fixed infrastructure and stationary data-collection locations.</desc>',
    ]

    for point in signal_measurements:
        x, y = project(point["latitude"], point["longitude"])
        radius = 16.0 + max(point["snr_db"], 0.0) * 0.38
        color = color_for_rsrp(point["rsrp_dbm"])
        out.append(
            f'  <circle cx="{x:.2f}" cy="{y:.2f}" r="{radius:.2f}" fill="{color}" '
            f'stroke="#202020" stroke-width="2.0" opacity="0.94"><title>5G signal point {escape_xml(point["label"])}: '
            f'RSRP {point["rsrp_dbm"]:.0f} dBm, SNR {point["snr_db"]:.1f} dB</title></circle>'
        )
        dx, dy = signal_label_offset(x, y)
        out.append(label_svg(x, y, f'{point["snr_db"]:.0f}', dx=dx, dy=dy, size=22, fill="#111111", weight="700"))

    for point in fiveg_base:
        x, y = project(point["latitude"], point["longitude"])
        out.append(
            f'  <path d="M {x:.2f} {y - 30:.2f} L {x - 26:.2f} {y + 23:.2f} L {x + 26:.2f} {y + 23:.2f} Z" '
            f'fill="#111111" stroke="white" stroke-width="3.4"><title>5G base station</title></path>'
        )
        out.append(label_svg(x, y, "5G BS", dx=28, dy=-16, size=28, fill="#111111", weight="700"))

    x, y = project(V2X_RSU["latitude"], V2X_RSU["longitude"])
    out.append(
        f'  <path d="M {x:.2f} {y - 30:.2f} L {x + 8.83:.2f} {y - 9.50:.2f} L {x + 30:.2f} {y - 9.27:.2f} '
        f'L {x + 14.27:.2f} {y + 3.70:.2f} L {x + 18.53:.2f} {y + 24.27:.2f} L {x:.2f} {y + 13.33:.2f} '
        f'L {x - 18.53:.2f} {y + 24.27:.2f} L {x - 14.27:.2f} {y + 3.70:.2f} L {x - 30:.2f} {y - 9.27:.2f} '
        f'L {x - 8.83:.2f} {y - 9.50:.2f} Z" fill="#c51b29" stroke="white" stroke-width="3.4"><title>V2X RSU</title></path>'
    )
    out.append(label_svg(x, y, "V2X RSU", dx=24, dy=34, size=28, fill="#222222", weight="700"))

    for point in fiveg_stationary:
        x, y = project(point["latitude"], point["longitude"])
        number = re.search(r"(\d+)$", point["label"])
        marker_text = number.group(1) if number else point["label"]
        out.append(
            f'  <rect x="{x - 20:.2f}" y="{y - 20:.2f}" width="40" height="40" rx="4" ry="4" fill="#2166ac" '
            f'stroke="white" stroke-width="4"><title>{escape_xml(point["label"])} stationary 5G run</title></rect>'
        )
        out.append(
            f'  <text x="{x:.2f}" y="{y + 8:.2f}" font-family="Arial, sans-serif" font-size="28" '
            f'font-weight="800" fill="white" text-anchor="middle">{escape_xml(marker_text)}</text>'
        )

    for point in v2x_stationary:
        x, y = project(point["latitude"], point["longitude"])
        marker_text = point["marker"]
        out.append(
            f'  <path d="M {x:.2f} {y - 25:.2f} L {x + 25:.2f} {y:.2f} L {x:.2f} {y + 25:.2f} L {x - 25:.2f} {y:.2f} Z" '
            f'fill="#762a83" stroke="white" stroke-width="4"><title>{escape_xml(point["label"])} stationary V2X run</title></path>'
        )
        out.append(
            f'  <text x="{x:.2f}" y="{y + 8:.2f}" font-family="Arial, sans-serif" font-size="27" '
            f'font-weight="800" fill="white" text-anchor="middle">{escape_xml(marker_text)}</text>'
        )

    out.extend(
        [
            '  <g id="experiment-overlay-legend" transform="translate(780 90)">',
            '    <rect x="0" y="0" width="310" height="340" fill="white" fill-opacity="0.88" stroke="#cccccc" stroke-width="2.0"/>',
            '    <circle cx="32" cy="40" r="14" fill="#fdae61" stroke="#202020" stroke-width="1.8"/>',
            '    <text x="70" y="48" font-size="21" fill="#111111">5G sample</text>',
            '    <path d="M 32 68 L 14 105 L 50 105 Z" fill="#111111" stroke="white" stroke-width="2.4"/>',
            '    <text x="70" y="100" font-size="21" fill="#111111">5G base</text>',
            '    <path d="M 32 124 L 40 142 L 60 142 L 45 154 L 51 174 L 32 164 L 13 174 L 19 154 L 4 142 L 24 142 Z" fill="#c51b29" stroke="white" stroke-width="2.4"/>',
            '    <text x="70" y="158" font-size="21" fill="#111111">V2X RSU</text>',
            '    <rect x="13" y="190" width="38" height="38" rx="4" ry="4" fill="#2166ac" stroke="white" stroke-width="2.4"/>',
            '    <text x="32" y="218" font-size="25" font-weight="800" text-anchor="middle" fill="white">1</text>',
            '    <text x="70" y="218" font-size="21" fill="#111111">5G stationary</text>',
            '    <path d="M 32 240 L 54 262 L 32 284 L 10 262 Z" fill="#762a83" stroke="white" stroke-width="2.4"/>',
            '    <text x="32" y="270" font-size="24" font-weight="800" text-anchor="middle" fill="white">1</text>',
            '    <text x="70" y="270" font-size="21" fill="#111111">V2X ref./point</text>',
            '    <rect x="13" y="300" width="38" height="25" fill="url(#orange-building-hatch)" stroke="#ff8c00" stroke-width="3.0"/>',
            '    <text x="70" y="321" font-size="21" fill="#111111">Buildings</text>',
            '  </g>',
            "</g>",
        ]
    )
    return "\n".join(out)


def overlay_svg(input_svg: Path, output_svg: Path, osm_path: Path):
    svg_text = input_svg.read_text()
    project, error = fit_svg_transform(svg_text, osm_path)
    overlay = marker_group(project)
    if "</svg>" not in svg_text:
        raise RuntimeError(f"{input_svg} does not look like an SVG file")
    svg_text = re.sub(
        r'<svg([^>]*)width="1000" height="1666" viewBox="0 0 1000 1666"',
        r'<svg\1width="1040" height="1410" viewBox="80 -40 1040 1410"',
        svg_text,
        count=1,
    )
    svg_text = re.sub(
        r"(<svg[^>]*>)",
        r'\1\n  <rect x="80" y="-40" width="1040" height="1410" fill="white"/>',
        svg_text,
        count=1,
    )
    result = svg_text.replace("</svg>", overlay + "\n</svg>")
    output_svg.write_text(result)
    return error


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input-svg", type=Path, default=DEFAULT_INPUT_SVG)
    parser.add_argument("--output-svg", type=Path, default=DEFAULT_OUTPUT_SVG)
    parser.add_argument("--osm", type=Path, default=DEFAULT_OSM)
    args = parser.parse_args()
    error = overlay_svg(args.input_svg, args.output_svg, args.osm)
    print(args.output_svg)
    print(
        "fit pairs={pairs} rmse=({rmse_x:.4f},{rmse_y:.4f}) max=({max_abs_x:.4f},{max_abs_y:.4f})".format(
            **error
        )
    )


if __name__ == "__main__":
    main()
