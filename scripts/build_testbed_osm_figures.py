#!/usr/bin/env python3
"""Build compact OSM figures for the paper testbed section."""

import csv
import math
import re
import urllib.request
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402
from PIL import Image  # noqa: E402


TILE_SIZE = 256
OSM_TILE_URL = "https://tile.openstreetmap.org/{z}/{x}/{y}.png"
ROOT = Path(__file__).resolve().parents[1]
FIG_DIR = ROOT / "paper" / "figs"
TILE_CACHE = ROOT / "results" / "real_5g" / "osm_tiles"
ZOOM = 18

V2X_RSU = {
    "label": "V2X RSU",
    "latitude": 39.66711689714955,
    "longitude": -75.75772185598035,
}

BUILDING_REGIONS = [
    {
        "label": "B1",
        "lat_min": 39.66713,
        "lat_max": 39.66763,
        "lon_min": -75.75803,
        "lon_max": -75.75735,
    },
    {
        "label": "B2",
        "lat_min": 39.66353,
        "lat_max": 39.66386,
        "lon_min": -75.75892,
        "lon_max": -75.75848,
    },
    {
        "label": "B3",
        "lat_min": 39.66345,
        "lat_max": 39.66383,
        "lon_min": -75.75705,
        "lon_max": -75.75630,
    },
    {
        "label": "B4",
        "lat_min": 39.66354,
        "lat_max": 39.66395,
        "lon_min": -75.75618,
        "lon_max": -75.75536,
    },
]


def latlon_to_global_pixel(latitude, longitude, zoom=ZOOM):
    latitude = max(min(latitude, 85.05112878), -85.05112878)
    sin_lat = math.sin(math.radians(latitude))
    scale = TILE_SIZE * (2**zoom)
    x = (longitude + 180.0) / 360.0 * scale
    y = (0.5 - math.log((1.0 + sin_lat) / (1.0 - sin_lat)) / (4.0 * math.pi)) * scale
    return x, y


def global_pixel_to_latlon(x, y, zoom=ZOOM):
    scale = TILE_SIZE * (2**zoom)
    lon = x / scale * 360.0 - 180.0
    n = math.pi - 2.0 * math.pi * y / scale
    lat = math.degrees(math.atan(math.sinh(n)))
    return lat, lon


def fetch_tile(z, x, y):
    path = TILE_CACHE / str(z) / str(x) / f"{y}.png"
    if path.exists() and path.stat().st_size > 0:
        return path
    path.parent.mkdir(parents=True, exist_ok=True)
    request = urllib.request.Request(
        OSM_TILE_URL.format(z=z, x=x, y=y),
        headers={
            "User-Agent": "Edge4AV-testbed-figure-generator/1.0",
            "Accept": "image/png",
        },
    )
    with urllib.request.urlopen(request, timeout=30) as response:
        path.write_bytes(response.read())
    return path


def read_csv(path):
    with path.open(newline="") as handle:
        return list(csv.DictReader(handle))


def load_5g_runs():
    rows = read_csv(ROOT / "results" / "real_5g" / "run_locations_parsed.csv")
    runs = []
    for row in rows:
        if row.get("included", "").lower() != "true":
            continue
        runs.append(
            {
                "label": row["label"].replace("Run ", "5G "),
                "latitude": float(row["latitude"]),
                "longitude": float(row["longitude"]),
            }
        )
    return runs


def load_signal_points():
    rows = read_csv(ROOT / "results" / "real_5g" / "signal_measurements_parsed.csv")
    points = []
    for row in rows:
        points.append(
            {
                "id": row["id"],
                "latitude": float(row["latitude"]),
                "longitude": float(row["longitude"]),
                "rsrp_dbm": float(row["rsrp_dbm"]) if row["rsrp_dbm"] else None,
                "is_base_station": row["is_base_station"].lower() == "true",
            }
        )
    return points


def load_v2x_stationary_runs():
    runs = []
    for summary in sorted((ROOT / "results" / "mocar_v2x").glob("*exp_01_payload_sweep*/summary.md")):
        text = summary.read_text(errors="replace")
        lat_match = re.search(r"Mean GNSS latitude:\s*`?([-0-9.]+)`?", text)
        lon_match = re.search(r"Mean GNSS longitude:\s*`?([-0-9.]+)`?", text)
        if not lat_match or not lon_match:
            latest = summary.parent / "gps" / "gps_latest.csv"
            if not latest.exists():
                continue
            rows = read_csv(latest)
            if not rows:
                continue
            lat = float(rows[-1]["latitude_deg"])
            lon = float(rows[-1]["longitude_deg"])
        else:
            lat = float(lat_match.group(1))
            lon = float(lon_match.group(1))
        runs.append(
            {
                "label": f"V2X {len(runs) + 1}",
                "latitude": lat,
                "longitude": lon,
                "source": summary.parent.name,
            }
        )
    return runs


def load_mobility_points():
    files = [
        ROOT
        / "results"
        / "mocar_v2x"
        / "20260704_exp_02_radio_distance_mobility_182104"
        / "v2x_gnss_by_send_time.csv",
        ROOT
        / "results"
        / "mocar_v2x"
        / "20260704_exp_02_radio_distance_mobility_183232"
        / "v2x_gnss_by_send_time.csv",
    ]
    points = []
    for idx, path in enumerate(files, start=3):
        for row in read_csv(path):
            lat = row.get("send_latitude_deg") or row.get("latitude_deg") or row.get("latitude")
            lon = row.get("send_longitude_deg") or row.get("longitude_deg") or row.get("longitude")
            if not lat or not lon:
                continue
            accepted = (row.get("success") or row.get("accepted") or "").lower() == "true"
            points.append(
                {
                    "run": f"Run {idx}",
                    "latitude": float(lat),
                    "longitude": float(lon),
                    "accepted": accepted,
                }
            )
    return points


def make_bounds(records, pad_px=100):
    pixels = [latlon_to_global_pixel(record["latitude"], record["longitude"]) for record in records]
    xs = [point[0] for point in pixels]
    ys = [point[1] for point in pixels]
    return min(xs) - pad_px, max(xs) + pad_px, min(ys) - pad_px, max(ys) + pad_px


def osm_mosaic(bounds):
    min_x, max_x, min_y, max_y = bounds
    min_tile_x = math.floor(min_x / TILE_SIZE)
    max_tile_x = math.floor((max_x - 1) / TILE_SIZE)
    min_tile_y = math.floor(min_y / TILE_SIZE)
    max_tile_y = math.floor((max_y - 1) / TILE_SIZE)
    mosaic_width = (max_tile_x - min_tile_x + 1) * TILE_SIZE
    mosaic_height = (max_tile_y - min_tile_y + 1) * TILE_SIZE
    mosaic = Image.new("RGB", (mosaic_width, mosaic_height), (233, 236, 239))
    for tile_x in range(min_tile_x, max_tile_x + 1):
        for tile_y in range(min_tile_y, max_tile_y + 1):
            img = Image.open(fetch_tile(ZOOM, tile_x, tile_y)).convert("RGB")
            paste_x = (tile_x - min_tile_x) * TILE_SIZE
            paste_y = (tile_y - min_tile_y) * TILE_SIZE
            mosaic.paste(img, (paste_x, paste_y))
    extent = (
        min_tile_x * TILE_SIZE,
        (max_tile_x + 1) * TILE_SIZE,
        (max_tile_y + 1) * TILE_SIZE,
        min_tile_y * TILE_SIZE,
    )
    return np.asarray(mosaic), extent


def draw_osm(ax, bounds):
    min_x, max_x, min_y, max_y = bounds
    image, extent = osm_mosaic(bounds)
    ax.imshow(image, extent=extent, zorder=0)
    ax.set_xlim(min_x, max_x)
    ax.set_ylim(max_y, min_y)
    ax.set_aspect("equal")
    ax.axis("off")
    return image, extent


def xy(record):
    return latlon_to_global_pixel(record["latitude"], record["longitude"])


def draw_label(ax, x, y, text, dx=6, dy=-6, size=5.6):
    ax.annotate(
        text,
        xy=(x, y),
        xytext=(dx, dy),
        textcoords="offset points",
        fontsize=size,
        weight="bold",
        color="#111827",
        bbox={"boxstyle": "round,pad=0.12", "fc": "white", "ec": "#6b7280", "lw": 0.45, "alpha": 0.88},
        zorder=8,
    )


def draw_building_outlines(ax, image, extent):
    arr = image.astype(np.int16)
    red = arr[:, :, 0]
    green = arr[:, :, 1]
    blue = arr[:, :, 2]
    base_mask = (
        (red >= 150)
        & (red <= 230)
        & (green >= 135)
        & (green <= 220)
        & (blue >= 120)
        & (blue <= 210)
        & ((red - blue) >= 8)
        & ((red - green) >= -5)
        & ((red - green) <= 28)
        & ((green - blue) >= 2)
        & ((green - blue) <= 35)
    )
    height, width = base_mask.shape
    x0, x1, y1, y0 = extent
    xs = np.linspace(x0, x1, width)
    ys = np.linspace(y0, y1, height)
    xx, yy = np.meshgrid(xs, ys)
    region_mask = np.zeros_like(base_mask, dtype=bool)
    for region in BUILDING_REGIONS:
        left, top = latlon_to_global_pixel(region["lat_max"], region["lon_min"])
        right, bottom = latlon_to_global_pixel(region["lat_min"], region["lon_max"])
        region_mask |= (
            (xx >= min(left, right))
            & (xx <= max(left, right))
            & (yy >= min(top, bottom))
            & (yy <= max(top, bottom))
        )
    mask = base_mask & region_mask
    visited = np.zeros_like(mask, dtype=bool)
    cleaned = np.zeros_like(mask, dtype=bool)
    height, width = mask.shape
    candidates = np.argwhere(mask)
    for start_y, start_x in candidates:
        if visited[start_y, start_x] or not mask[start_y, start_x]:
            continue
        stack = [(int(start_y), int(start_x))]
        visited[start_y, start_x] = True
        component = []
        while stack:
            cy, cx = stack.pop()
            component.append((cy, cx))
            for ny in (cy - 1, cy, cy + 1):
                for nx in (cx - 1, cx, cx + 1):
                    if ny == cy and nx == cx:
                        continue
                    if ny < 0 or nx < 0 or ny >= height or nx >= width:
                        continue
                    if visited[ny, nx] or not mask[ny, nx]:
                        continue
                    visited[ny, nx] = True
                    stack.append((ny, nx))
        if len(component) < 85:
            continue
        ys_comp = [point[0] for point in component]
        xs_comp = [point[1] for point in component]
        if (max(xs_comp) - min(xs_comp) < 8) or (max(ys_comp) - min(ys_comp) < 8):
            continue
        for cy, cx in component:
            cleaned[cy, cx] = True

    if cleaned.max() > 0:
        ax.contour(
            xs,
            ys,
            cleaned.astype(float),
            levels=[0.5],
            colors=["#f59e0b"],
            linewidths=1.2,
            zorder=4,
        )


def draw_scale_bar(ax, bounds, length_m=100):
    min_x, max_x, min_y, max_y = bounds
    center_lat, _ = global_pixel_to_latlon((min_x + max_x) / 2, (min_y + max_y) / 2)
    meters_per_pixel = 156543.03392 * math.cos(math.radians(center_lat)) / (2**ZOOM)
    length_px = length_m / meters_per_pixel
    x0 = min_x + 24
    y0 = max_y - 28
    ax.plot([x0, x0 + length_px], [y0, y0], color="#111827", lw=1.2, zorder=9)
    ax.plot([x0, x0], [y0 - 4, y0 + 4], color="#111827", lw=1.0, zorder=9)
    ax.plot([x0 + length_px, x0 + length_px], [y0 - 4, y0 + 4], color="#111827", lw=1.0, zorder=9)
    ax.text(x0, y0 - 7, f"{length_m} m", fontsize=6.5, color="#111827", zorder=9)


def draw_rsrp_overlay(ax, bounds, signal_points):
    samples = [point for point in signal_points if point["rsrp_dbm"] is not None]
    min_x, max_x, min_y, max_y = bounds
    width = 140
    height = max(80, int(width * (max_y - min_y) / (max_x - min_x)))
    xs = np.linspace(min_x, max_x, width)
    ys = np.linspace(min_y, max_y, height)
    sample_xy = np.array([xy(point) for point in samples], dtype=float)
    sample_values = np.array([point["rsrp_dbm"] for point in samples], dtype=float)
    grid = np.empty((height, width), dtype=float)
    for row, y_val in enumerate(ys):
        dx = sample_xy[:, 0][None, :] - xs[:, None]
        dy = sample_xy[:, 1][None, :] - y_val
        dist2 = dx * dx + dy * dy
        weights = 1.0 / np.maximum(dist2, 1.0)
        grid[row, :] = (weights * sample_values).sum(axis=1) / weights.sum(axis=1)
    image = ax.imshow(
        grid,
        extent=(min_x, max_x, max_y, min_y),
        cmap="RdYlGn",
        vmin=-121,
        vmax=-91,
        alpha=0.25,
        zorder=1,
    )
    return image


def build_stationary_overlay():
    signal_points = load_signal_points()
    fiveg_runs = load_5g_runs()
    v2x_runs = load_v2x_stationary_runs()
    records = signal_points + fiveg_runs + v2x_runs + [V2X_RSU]
    for box in BUILDING_REGIONS:
        records.append({"latitude": box["lat_min"], "longitude": box["lon_min"]})
        records.append({"latitude": box["lat_max"], "longitude": box["lon_max"]})
    bounds = make_bounds(records, pad_px=88)

    fig, ax = plt.subplots(figsize=(3.35, 3.25), dpi=320)
    image, extent = draw_osm(ax, bounds)
    image = draw_rsrp_overlay(ax, bounds, signal_points)
    osm_image, osm_extent = osm_mosaic(bounds)
    draw_building_outlines(ax, osm_image, osm_extent)

    base = next(point for point in signal_points if point["is_base_station"])
    bx, by = xy(base)
    ax.scatter([bx], [by], marker="^", s=38, c="#111827", edgecolors="white", linewidths=0.65, zorder=7)

    for run in fiveg_runs:
        x, y = xy(run)
        ax.scatter([x], [y], marker="o", s=31, c="#2563eb", edgecolors="white", linewidths=0.65, zorder=7)
    for idx, run in enumerate(fiveg_runs, start=1):
        x, y = xy(run)
        ax.text(x, y + 2.5, str(idx), fontsize=4.8, weight="bold", ha="center", va="center", color="white", zorder=8)

    for run in v2x_runs:
        x, y = xy(run)
        ax.scatter([x], [y], marker="D", s=28, c="#dc2626", edgecolors="white", linewidths=0.65, zorder=7)

    rx, ry = xy(V2X_RSU)
    ax.scatter([rx], [ry], marker="s", s=35, c="#000000", edgecolors="white", linewidths=0.65, zorder=8)

    draw_scale_bar(ax, bounds, 150)
    ax.text(
        bounds[1] - 2,
        bounds[3] - 4,
        "(C) OpenStreetMap contributors",
        ha="right",
        va="bottom",
        fontsize=5.5,
        color="#111827",
        zorder=9,
    )
    handles = [
        plt.Line2D([0], [0], marker="^", color="none", markerfacecolor="#111827", markeredgecolor="white", markersize=6, label="5G base station"),
        plt.Line2D([0], [0], marker="o", color="none", markerfacecolor="#2563eb", markeredgecolor="white", markersize=6, label="5G stationary runs"),
        plt.Line2D([0], [0], marker="s", color="none", markerfacecolor="#000000", markeredgecolor="white", markersize=6, label="fixed V2X RSU"),
        plt.Line2D([0], [0], marker="D", color="none", markerfacecolor="#dc2626", markeredgecolor="white", markersize=5.5, label="V2X stationary OBU"),
        plt.Line2D([0], [0], color="#f59e0b", lw=1.4, label="building outline"),
    ]
    ax.legend(handles=handles, loc="upper right", fontsize=4.7, framealpha=0.9, borderpad=0.25, handlelength=1.0, labelspacing=0.25)
    cbar = fig.colorbar(image, ax=ax, fraction=0.033, pad=0.012)
    cbar.set_label("RSRP (dBm)", fontsize=5.4)
    cbar.ax.tick_params(labelsize=5.0, length=2)
    FIG_DIR.mkdir(parents=True, exist_ok=True)
    fig.savefig(FIG_DIR / "fig-testbed-stationary-overlay.png", bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)


def build_empty_stationary_osm():
    signal_points = load_signal_points()
    fiveg_runs = load_5g_runs()
    v2x_runs = load_v2x_stationary_runs()
    records = signal_points + fiveg_runs + v2x_runs + [V2X_RSU]
    bounds = make_bounds(records, pad_px=88)

    fig, ax = plt.subplots(figsize=(3.35, 3.25), dpi=320)
    draw_osm(ax, bounds)
    ax.text(
        bounds[1] - 2,
        bounds[3] - 4,
        "(C) OpenStreetMap contributors",
        ha="right",
        va="bottom",
        fontsize=5.5,
        color="#111827",
        zorder=9,
    )
    FIG_DIR.mkdir(parents=True, exist_ok=True)
    fig.savefig(FIG_DIR / "fig-empty-osm-stationary.png", bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)


def build_empty_mobility_osm():
    mobility = load_mobility_points()
    records = mobility + [V2X_RSU]
    bounds = make_bounds(records, pad_px=78)

    fig, ax = plt.subplots(figsize=(3.35, 3.85), dpi=320)
    draw_osm(ax, bounds)
    ax.text(
        bounds[1] - 2,
        bounds[3] - 4,
        "(C) OpenStreetMap contributors",
        ha="right",
        va="bottom",
        fontsize=5.5,
        color="#111827",
        zorder=9,
    )
    FIG_DIR.mkdir(parents=True, exist_ok=True)
    fig.savefig(FIG_DIR / "fig-empty-osm-mobility.png", bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)


def build_mobility_overlay():
    mobility = load_mobility_points()
    records = mobility + [V2X_RSU]
    for box in BUILDING_REGIONS:
        records.append({"latitude": box["lat_min"], "longitude": box["lon_min"]})
        records.append({"latitude": box["lat_max"], "longitude": box["lon_max"]})
    bounds = make_bounds(records, pad_px=78)

    fig, ax = plt.subplots(figsize=(3.35, 3.85), dpi=320)
    image, extent = draw_osm(ax, bounds)
    draw_building_outlines(ax, image, extent)

    success_colors = {"Run 3": "#2563eb", "Run 4": "#059669"}
    for run in ["Run 3", "Run 4"]:
        pts = [point for point in mobility if point["run"] == run and point["accepted"]]
        xs, ys = zip(*(xy(point) for point in pts))
        ax.scatter(xs, ys, s=3.2, c=success_colors[run], marker="o", linewidths=0, alpha=0.95, label=f"{run} success", zorder=6)
    pts = [point for point in mobility if not point["accepted"]]
    xs, ys = zip(*(xy(point) for point in pts))
    ax.scatter(xs, ys, s=5.8, c="#dc2626", marker="x", linewidths=0.45, alpha=0.9, label="timeout", zorder=7)

    rx, ry = xy(V2X_RSU)
    ax.scatter([rx], [ry], marker="s", s=34, c="#000000", edgecolors="white", linewidths=0.65, zorder=8)
    draw_scale_bar(ax, bounds, 100)
    ax.text(
        bounds[1] - 2,
        bounds[3] - 4,
        "(C) OpenStreetMap contributors",
        ha="right",
        va="bottom",
        fontsize=5.5,
        color="#111827",
        zorder=9,
    )
    handles, labels = ax.get_legend_handles_labels()
    handles.append(plt.Line2D([0], [0], marker="s", color="none", markerfacecolor="#000000", markeredgecolor="white", markersize=6, label="fixed V2X RSU"))
    handles.append(plt.Line2D([0], [0], color="#f59e0b", lw=1.4, label="building outline"))
    ax.legend(handles=handles, loc="upper right", fontsize=4.7, framealpha=0.9, borderpad=0.25, handlelength=1.0, labelspacing=0.25)
    fig.savefig(FIG_DIR / "fig-v2x-mobility-routes.png", bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)


def main():
    build_stationary_overlay()
    build_mobility_overlay()
    build_empty_stationary_osm()
    build_empty_mobility_osm()
    print(FIG_DIR / "fig-testbed-stationary-overlay.png")
    print(FIG_DIR / "fig-v2x-mobility-routes.png")
    print(FIG_DIR / "fig-empty-osm-stationary.png")
    print(FIG_DIR / "fig-empty-osm-mobility.png")


if __name__ == "__main__":
    main()
