#!/usr/bin/env python3
"""Render a local OSM XML extract as a static PNG figure."""

from __future__ import annotations

import argparse
import csv
import math
import re
import xml.etree.ElementTree as ET
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import matplotlib.patheffects as pe  # noqa: E402
from matplotlib.lines import Line2D  # noqa: E402
from matplotlib.patches import Polygon  # noqa: E402


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_INPUT = ROOT / "map.osm"
DEFAULT_OUTPUT = ROOT / "paper" / "figs" / "fig-map-osm.png"
DEFAULT_OVERLAY_OUTPUT = ROOT / "paper" / "figs" / "fig-map-osm-overlays.png"
V2X_RSU = {
    "label": "V2X RSU",
    "latitude": 39.66711689714955,
    "longitude": -75.75772185598035,
}


def parse_osm(path: Path):
    tree = ET.parse(path)
    root = tree.getroot()

    bounds_elem = root.find("bounds")
    if bounds_elem is None:
        raise ValueError(f"{path} has no OSM bounds element")

    bounds = {
        "minlat": float(bounds_elem.attrib["minlat"]),
        "minlon": float(bounds_elem.attrib["minlon"]),
        "maxlat": float(bounds_elem.attrib["maxlat"]),
        "maxlon": float(bounds_elem.attrib["maxlon"]),
    }

    nodes = {}
    for node in root.findall("node"):
        node_id = node.attrib.get("id")
        if not node_id:
            continue
        nodes[node_id] = (float(node.attrib["lat"]), float(node.attrib["lon"]))

    ways = []
    for way in root.findall("way"):
        refs = [nd.attrib["ref"] for nd in way.findall("nd") if nd.attrib.get("ref") in nodes]
        if len(refs) < 2:
            continue
        tags = {tag.attrib["k"]: tag.attrib["v"] for tag in way.findall("tag")}
        coords = [nodes[ref] for ref in refs]
        ways.append({"coords": coords, "tags": tags, "closed": refs[0] == refs[-1]})

    return bounds, ways


def projector(bounds):
    lat0 = (bounds["minlat"] + bounds["maxlat"]) / 2.0
    lon0 = (bounds["minlon"] + bounds["maxlon"]) / 2.0
    meters_per_lat = 111_320.0
    meters_per_lon = 111_320.0 * math.cos(math.radians(lat0))

    def project(lat: float, lon: float):
        return (lon - lon0) * meters_per_lon, (lat - lat0) * meters_per_lat

    return project


def projected_bounds(bounds, project):
    min_x, min_y = project(bounds["minlat"], bounds["minlon"])
    max_x, max_y = project(bounds["maxlat"], bounds["maxlon"])
    return min_x, max_x, min_y, max_y


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
    for summary in sorted((ROOT / "results" / "mocar_v2x").glob("*exp_01_payload_sweep*/summary.md")):
        text = summary.read_text(errors="replace")
        lat_match = re.search(r"Mean GNSS latitude:\s*`?([-0-9.]+)`?", text)
        lon_match = re.search(r"Mean GNSS longitude:\s*`?([-0-9.]+)`?", text)
        if not lat_match or not lon_match:
            continue
        runs.append(
            {
                "label": f"V2X S{len(runs) + 1}",
                "latitude": float(lat_match.group(1)),
                "longitude": float(lon_match.group(1)),
            }
        )
    return runs


def load_overlay_data():
    signal_points = load_signal_points()
    return {
        "signal_points": signal_points,
        "fiveg_stationary": load_5g_stationary_runs(),
        "v2x_stationary": load_v2x_stationary_runs(),
        "fiveg_base": [point for point in signal_points if point["is_base_station"]],
        "v2x_rsu": [V2X_RSU],
    }


def expand_bounds_for_overlays(bounds, overlay_data):
    latitudes = [bounds["minlat"], bounds["maxlat"]]
    longitudes = [bounds["minlon"], bounds["maxlon"]]
    for records in overlay_data.values():
        for record in records:
            latitudes.append(record["latitude"])
            longitudes.append(record["longitude"])
    lat_pad = max(max(latitudes) - min(latitudes), 1e-6) * 0.04
    lon_pad = max(max(longitudes) - min(longitudes), 1e-6) * 0.04
    return {
        "minlat": min(latitudes) - lat_pad,
        "maxlat": max(latitudes) + lat_pad,
        "minlon": min(longitudes) - lon_pad,
        "maxlon": max(longitudes) + lon_pad,
    }


def is_polygon(way):
    tags = way["tags"]
    return way["closed"] and (
        "building" in tags
        or tags.get("amenity") == "parking"
        or tags.get("landuse") in {"grass", "meadow", "forest", "retail", "commercial", "residential"}
        or tags.get("leisure") in {"park", "pitch"}
        or tags.get("natural") in {"water", "wood"}
    )


def polygon_style(tags):
    if "building" in tags:
        return {"facecolor": "#d0cec8", "edgecolor": "#8c8983", "linewidth": 0.65, "zorder": 3}
    if tags.get("amenity") == "parking":
        return {"facecolor": "#e8e4da", "edgecolor": "#c7c1b3", "linewidth": 0.5, "zorder": 1}
    if tags.get("landuse") in {"grass", "meadow"} or tags.get("leisure") in {"park", "pitch"}:
        return {"facecolor": "#dfe9d4", "edgecolor": "#c2d0b7", "linewidth": 0.35, "zorder": 0.5}
    if tags.get("natural") == "water":
        return {"facecolor": "#d8e9f0", "edgecolor": "#9fbac7", "linewidth": 0.45, "zorder": 0.75}
    return {"facecolor": "#ece8df", "edgecolor": "#d1ccbf", "linewidth": 0.35, "zorder": 0.25}


def road_style(highway):
    major = {"motorway", "trunk", "primary", "secondary", "tertiary"}
    medium = {"residential", "unclassified", "living_street"}
    service = {"service", "road"}
    paths = {"footway", "path", "cycleway", "pedestrian", "steps", "track"}
    if highway in major:
        return {"casing": 4.2, "line": 3.0, "color": "#f7f1d0", "edge": "#b6a85e", "zorder": 8}
    if highway in medium:
        return {"casing": 3.2, "line": 2.1, "color": "#ffffff", "edge": "#b9b6ae", "zorder": 7}
    if highway in service:
        return {"casing": 2.6, "line": 1.6, "color": "#ffffff", "edge": "#c3c0b8", "zorder": 6}
    if highway in paths:
        return {"casing": 1.6, "line": 1.0, "color": "#f8f8f5", "edge": "#aaa69d", "zorder": 5, "dashes": (2.0, 2.0)}
    return {"casing": 2.2, "line": 1.3, "color": "#ffffff", "edge": "#c2beb5", "zorder": 6}


def render_osm(input_path: Path, output_path: Path, dpi: int = 300):
    bounds, ways = parse_osm(input_path)
    overlay_data = None
    project = projector(bounds)
    min_x, max_x, min_y, max_y = projected_bounds(bounds, project)

    width = max_x - min_x
    height = max_y - min_y
    pad = max(width, height) * 0.025
    aspect = width / height if height else 1.0
    fig_width = 7.2
    fig_height = max(4.0, fig_width / aspect)

    fig, ax = plt.subplots(figsize=(fig_width, fig_height), dpi=dpi)
    fig.patch.set_facecolor("white")
    ax.set_facecolor("#f5f2ea")

    # Draw polygons first so roads and paths remain legible.
    for way in ways:
        if not is_polygon(way):
            continue
        xy = [project(lat, lon) for lat, lon in way["coords"]]
        ax.add_patch(Polygon(xy, closed=True, **polygon_style(way["tags"])))

    # Draw road casings and road interiors separately.
    road_ways = [way for way in ways if "highway" in way["tags"]]
    for way in road_ways:
        xs, ys = zip(*(project(lat, lon) for lat, lon in way["coords"]))
        style = road_style(way["tags"]["highway"])
        ax.plot(
            xs,
            ys,
            color=style["edge"],
            linewidth=style["casing"],
            solid_capstyle="round",
            solid_joinstyle="round",
            zorder=style["zorder"],
        )
    for way in road_ways:
        xs, ys = zip(*(project(lat, lon) for lat, lon in way["coords"]))
        style = road_style(way["tags"]["highway"])
        kwargs = {}
        if "dashes" in style:
            kwargs["dashes"] = style["dashes"]
        ax.plot(
            xs,
            ys,
            color=style["color"],
            linewidth=style["line"],
            solid_capstyle="round",
            solid_joinstyle="round",
            zorder=style["zorder"] + 0.1,
            **kwargs,
        )

    ax.set_xlim(min_x - pad, max_x + pad)
    ax.set_ylim(min_y - pad, max_y + pad)
    ax.set_aspect("equal", adjustable="box")
    ax.axis("off")
    fig.subplots_adjust(left=0, right=1, top=1, bottom=0)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=dpi, bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)
    return output_path


def add_label(ax, x, y, text, dx=5, dy=5, fontsize=6.2, color="#222222", weight="normal", zorder=20):
    ax.annotate(
        text,
        (x, y),
        xytext=(dx, dy),
        textcoords="offset points",
        fontsize=fontsize,
        color=color,
        weight=weight,
        zorder=zorder,
        path_effects=[pe.withStroke(linewidth=2.2, foreground="white", alpha=0.92)],
    )


def draw_overlays(ax, overlay_data, project):
    signal_points = [
        point
        for point in overlay_data["signal_points"]
        if not point["is_base_station"] and point["rsrp_dbm"] is not None and point["snr_db"] is not None
    ]
    if signal_points:
        xs, ys = zip(*(project(point["latitude"], point["longitude"]) for point in signal_points))
        rsrp = [point["rsrp_dbm"] for point in signal_points]
        snr = [point["snr_db"] for point in signal_points]
        sizes = [42 + max(value, 0) * 4.0 for value in snr]
        scatter = ax.scatter(
            xs,
            ys,
            c=rsrp,
            s=sizes,
            cmap="RdYlGn",
            vmin=-122,
            vmax=-90,
            edgecolor="#202020",
            linewidth=0.45,
            alpha=0.92,
            zorder=30,
        )
        for point, x, y in zip(signal_points, xs, ys):
            add_label(ax, x, y, f"{point['snr_db']:.0f}", dx=4, dy=3, fontsize=5.8, color="#111111", zorder=31)
        cbar = ax.figure.colorbar(scatter, ax=ax, fraction=0.034, pad=0.01)
        cbar.set_label("5G RSRP (dBm)", fontsize=7)
        cbar.ax.tick_params(labelsize=6)

    for record in overlay_data["fiveg_base"]:
        x, y = project(record["latitude"], record["longitude"])
        ax.scatter([x], [y], marker="^", s=145, color="#111111", edgecolor="white", linewidth=0.8, zorder=45)
        add_label(ax, x, y, "5G BS", dx=6, dy=6, fontsize=7.2, weight="bold", zorder=46)

    for record in overlay_data["v2x_rsu"]:
        x, y = project(record["latitude"], record["longitude"])
        ax.scatter([x], [y], marker="*", s=205, color="#c51b29", edgecolor="white", linewidth=0.75, zorder=46)
        add_label(ax, x, y, "V2X RSU", dx=6, dy=-9, fontsize=7.2, weight="bold", zorder=47)

    if overlay_data["fiveg_stationary"]:
        xs, ys = zip(*(project(record["latitude"], record["longitude"]) for record in overlay_data["fiveg_stationary"]))
        ax.scatter(xs, ys, marker="s", s=62, color="#2166ac", edgecolor="white", linewidth=0.75, zorder=38)
        for record, x, y in zip(overlay_data["fiveg_stationary"], xs, ys):
            add_label(ax, x, y, record["label"], dx=5, dy=-8, fontsize=5.8, color="#123a61", zorder=39)

    if overlay_data["v2x_stationary"]:
        xs, ys = zip(*(project(record["latitude"], record["longitude"]) for record in overlay_data["v2x_stationary"]))
        ax.scatter(xs, ys, marker="D", s=58, color="#762a83", edgecolor="white", linewidth=0.75, zorder=39)
        for record, x, y in zip(overlay_data["v2x_stationary"], xs, ys):
            add_label(ax, x, y, record["label"], dx=5, dy=5, fontsize=5.8, color="#4a1654", zorder=40)

    legend_handles = [
        Line2D([0], [0], marker="o", color="none", markerfacecolor="#fdae61", markeredgecolor="#202020", markersize=6, label="5G signal survey; color=RSRP, label=SNR dB"),
        Line2D([0], [0], marker="^", color="none", markerfacecolor="#111111", markeredgecolor="white", markersize=7, label="5G base station"),
        Line2D([0], [0], marker="*", color="none", markerfacecolor="#c51b29", markeredgecolor="white", markersize=9, label="V2X RSU"),
        Line2D([0], [0], marker="s", color="none", markerfacecolor="#2166ac", markeredgecolor="white", markersize=6, label="5G stationary runs"),
        Line2D([0], [0], marker="D", color="none", markerfacecolor="#762a83", markeredgecolor="white", markersize=6, label="V2X stationary runs"),
    ]
    legend = ax.legend(
        handles=legend_handles,
        loc="lower left",
        bbox_to_anchor=(0.012, 0.012),
        frameon=True,
        framealpha=0.92,
        facecolor="white",
        edgecolor="#cccccc",
        fontsize=6.2,
    )
    legend.set_zorder(60)


def render_osm_with_overlays(input_path: Path, output_path: Path, dpi: int = 300):
    bounds, ways = parse_osm(input_path)
    overlay_data = load_overlay_data()
    bounds = expand_bounds_for_overlays(bounds, overlay_data)
    project = projector(bounds)
    min_x, max_x, min_y, max_y = projected_bounds(bounds, project)

    width = max_x - min_x
    height = max_y - min_y
    pad = max(width, height) * 0.025
    aspect = width / height if height else 1.0
    fig_width = 7.2
    fig_height = max(4.0, fig_width / aspect)

    fig, ax = plt.subplots(figsize=(fig_width, fig_height), dpi=dpi)
    fig.patch.set_facecolor("white")
    ax.set_facecolor("#f5f2ea")

    for way in ways:
        if not is_polygon(way):
            continue
        xy = [project(lat, lon) for lat, lon in way["coords"]]
        ax.add_patch(Polygon(xy, closed=True, **polygon_style(way["tags"])))

    road_ways = [way for way in ways if "highway" in way["tags"]]
    for way in road_ways:
        xs, ys = zip(*(project(lat, lon) for lat, lon in way["coords"]))
        style = road_style(way["tags"]["highway"])
        ax.plot(
            xs,
            ys,
            color=style["edge"],
            linewidth=style["casing"],
            solid_capstyle="round",
            solid_joinstyle="round",
            zorder=style["zorder"],
        )
    for way in road_ways:
        xs, ys = zip(*(project(lat, lon) for lat, lon in way["coords"]))
        style = road_style(way["tags"]["highway"])
        kwargs = {}
        if "dashes" in style:
            kwargs["dashes"] = style["dashes"]
        ax.plot(
            xs,
            ys,
            color=style["color"],
            linewidth=style["line"],
            solid_capstyle="round",
            solid_joinstyle="round",
            zorder=style["zorder"] + 0.1,
            **kwargs,
        )

    draw_overlays(ax, overlay_data, project)

    ax.set_xlim(min_x - pad, max_x + pad)
    ax.set_ylim(min_y - pad, max_y + pad)
    ax.set_aspect("equal", adjustable="box")
    ax.axis("off")
    fig.subplots_adjust(left=0, right=1, top=1, bottom=0)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=dpi, bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)
    return output_path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--dpi", type=int, default=300)
    parser.add_argument(
        "--overlay",
        action="store_true",
        help="draw 5G signal/SNR, 5G base station, V2X RSU, and stationary run overlays",
    )
    args = parser.parse_args()
    output = render_osm_with_overlays(args.input, args.output, args.dpi) if args.overlay else render_osm(args.input, args.output, args.dpi)
    print(output)


if __name__ == "__main__":
    main()
