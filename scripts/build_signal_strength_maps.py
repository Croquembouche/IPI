#!/usr/bin/env python3
"""Build interpolated signal-strength maps from the real 5G ODS sheet."""

import argparse
import base64
import csv
import gzip
import math
import re
import statistics
import urllib.request
import zipfile
from html import escape
from pathlib import Path
from xml.etree import ElementTree as ET


TABLE_NS = "urn:oasis:names:tc:opendocument:xmlns:table:1.0"
OSM_TILE_URL = "https://tile.openstreetmap.org/{z}/{x}/{y}.png"
TILE_SIZE = 256
OSM_LABEL_SCALE = 3.2

RUN_LOCATION_SOURCES = [
    {
        "label": "Run 1",
        "source": "results/real_5g/20260513_sunny_fintechparking_run_1/gps/gps_samples.csv",
        "description": "Run 1, sunny FinTech parking",
    },
    {
        "label": "Run 2",
        "source": "results/real_5g/20260514_sunny_after_rain_run_1/gps/gps_samples.csv",
        "description": "Run 2, sunny after rain",
    },
    {
        "label": "Run 3",
        "source": "results/real_5g/20260515_sunny_run_1/gps/gps_samples.csv",
        "description": "Run 3, sunny continuation",
    },
    {
        "label": "Run 4",
        "source": "results/real_5g/20260522_cloudy_run_1/gps_20260522/gps/gps_samples.csv",
        "description": "Run 4, cloudy",
    },
    {
        "label": "Run 5",
        "source": "results/real_5g/20260521_small_rain_run_1/gps_20260521/gps/gps_samples.csv",
        "description": "Run 5, small rain",
    },
]

EXCLUDED_GNSS_SOURCES = []

METRICS = [
    {
        "key": "rsrp_dbm",
        "label": "RSRP",
        "unit": "dBm",
        "domain": (-125.0, -85.0),
        "description": "Reference signal received power",
    },
    {
        "key": "rsrq_db",
        "label": "RSRQ",
        "unit": "dB",
        "domain": (-20.0, -8.0),
        "description": "Reference signal received quality",
    },
    {
        "key": "snr_db",
        "label": "SNR",
        "unit": "dB",
        "domain": (0.0, 30.0),
        "description": "Signal-to-noise ratio reported in dB",
    },
]


def ods_rows(path, max_columns=64):
    with zipfile.ZipFile(path) as archive:
        content = archive.read("content.xml")
    root = ET.fromstring(content)
    row_tag = f"{{{TABLE_NS}}}table-row"
    cell_tag = f"{{{TABLE_NS}}}table-cell"
    covered_cell_tag = f"{{{TABLE_NS}}}covered-table-cell"
    repeat_columns = f"{{{TABLE_NS}}}number-columns-repeated"
    repeat_rows = f"{{{TABLE_NS}}}number-rows-repeated"

    for table in root.iter(f"{{{TABLE_NS}}}table"):
        for row in table.iter(row_tag):
            row_values = []
            for cell in row:
                if cell.tag not in {cell_tag, covered_cell_tag}:
                    continue
                repeated = int(cell.attrib.get(repeat_columns, "1"))
                text = "".join(cell.itertext()).strip()
                if repeated > max_columns - len(row_values):
                    repeated = max(0, max_columns - len(row_values))
                row_values.extend([text] * repeated)
            repeated_rows = int(row.attrib.get(repeat_rows, "1"))
            if any(value.strip() for value in row_values):
                yield row_values[:max_columns]
            elif repeated_rows <= 10:
                for _ in range(repeated_rows):
                    yield row_values[:max_columns]


def parse_float(value):
    value = str(value).strip()
    if not value or value == "-":
        return None
    try:
        return float(value)
    except ValueError:
        return None


def parse_range_midpoint(value):
    value = str(value).strip()
    if not value or value == "-":
        return None
    match = re.fullmatch(r"(-?\d+(?:\.\d+)?)\s*-\s*(-?\d+(?:\.\d+)?)", value)
    if match:
        low = float(match.group(1))
        high = float(match.group(2))
        return (low + high) / 2.0
    return parse_float(value)


def parse_location(value):
    parts = [part.strip() for part in str(value).split(",")]
    if len(parts) != 2:
        raise ValueError(f"Invalid location cell: {value!r}")
    return float(parts[0]), float(parts[1])


def load_measurements(path):
    rows = list(ods_rows(path))
    header_row = None
    for row in rows:
        normalized = [cell.strip().lower() for cell in row]
        if "id" in normalized and "location" in normalized:
            header_row = row
            break
    if header_row is None:
        raise ValueError(f"No ID/Location header row found in {path}")

    columns = {cell.strip().lower(): idx for idx, cell in enumerate(header_row) if cell.strip()}
    required = ["id", "location", "rsrp", "rsrq", "snr"]
    missing = [name for name in required if name not in columns]
    if missing:
        raise ValueError(f"Missing required columns in {path}: {', '.join(missing)}")

    records = []
    seen_header = False
    for row in rows:
        if row is header_row:
            seen_header = True
            continue
        if not seen_header:
            continue
        if len(row) <= columns["id"] or not row[columns["id"]].strip():
            continue
        try:
            point_id = int(float(row[columns["id"]]))
        except ValueError:
            continue
        latitude, longitude = parse_location(row[columns["location"]])
        snr_raw = row[columns["snr"]].strip()
        records.append(
            {
                "id": point_id,
                "latitude": latitude,
                "longitude": longitude,
                "is_base_station": point_id == 0,
                "rsrp_dbm": parse_float(row[columns["rsrp"]]),
                "rsrq_db": parse_float(row[columns["rsrq"]]),
                "snr_db": parse_range_midpoint(snr_raw),
                "snr_raw": snr_raw,
            }
        )

    if not records:
        raise ValueError(f"No signal measurement records found in {path}")
    if not any(record["is_base_station"] for record in records):
        raise ValueError("No base-station record found; expected ID 0")
    return sorted(records, key=lambda record: record["id"])


def make_projection(records):
    lat0 = statistics.mean(record["latitude"] for record in records)
    lon0 = statistics.mean(record["longitude"] for record in records)
    meters_per_lon = 111320.0 * math.cos(math.radians(lat0))
    meters_per_lat = 110540.0

    def to_xy(latitude, longitude):
        return (longitude - lon0) * meters_per_lon, (latitude - lat0) * meters_per_lat

    def to_latlon(x, y):
        return y / meters_per_lat + lat0, x / meters_per_lon + lon0

    return to_xy, to_latlon


def enrich_coordinates(records):
    to_xy, to_latlon = make_projection(records)
    base = next(record for record in records if record["is_base_station"])
    base_x, base_y = to_xy(base["latitude"], base["longitude"])
    for record in records:
        x, y = to_xy(record["latitude"], record["longitude"])
        record["x_m"] = x
        record["y_m"] = y
        record["east_m_from_base"] = x - base_x
        record["north_m_from_base"] = y - base_y
        record["distance_to_base_m"] = math.hypot(record["east_m_from_base"], record["north_m_from_base"])
    return to_latlon


def convex_hull(points):
    points = sorted(set(points))
    if len(points) <= 1:
        return points

    def cross(origin, a, b):
        return (a[0] - origin[0]) * (b[1] - origin[1]) - (a[1] - origin[1]) * (b[0] - origin[0])

    lower = []
    for point in points:
        while len(lower) >= 2 and cross(lower[-2], lower[-1], point) <= 0:
            lower.pop()
        lower.append(point)

    upper = []
    for point in reversed(points):
        while len(upper) >= 2 and cross(upper[-2], upper[-1], point) <= 0:
            upper.pop()
        upper.append(point)

    return lower[:-1] + upper[:-1]


def point_on_segment(point, a, b, tolerance=1e-6):
    px, py = point
    ax, ay = a
    bx, by = b
    cross = (px - ax) * (by - ay) - (py - ay) * (bx - ax)
    if abs(cross) > tolerance:
        return False
    dot = (px - ax) * (px - bx) + (py - ay) * (py - by)
    return dot <= tolerance


def point_in_polygon(point, polygon):
    if len(polygon) < 3:
        return True
    for idx, start in enumerate(polygon):
        end = polygon[(idx + 1) % len(polygon)]
        if point_on_segment(point, start, end):
            return True
    x, y = point
    inside = False
    j = len(polygon) - 1
    for i in range(len(polygon)):
        xi, yi = polygon[i]
        xj, yj = polygon[j]
        intersects = (yi > y) != (yj > y) and x < (xj - xi) * (y - yi) / (yj - yi) + xi
        if intersects:
            inside = not inside
        j = i
    return inside


def plot_extent(records):
    xs = [record["x_m"] for record in records]
    ys = [record["y_m"] for record in records]
    x_span = max(xs) - min(xs)
    y_span = max(ys) - min(ys)
    pad = max(20.0, 0.10 * max(x_span, y_span))
    return min(xs) - pad, max(xs) + pad, min(ys) - pad, max(ys) + pad


def circumcircle_contains(point, triangle, points):
    px, py = point
    ax, ay = points[triangle[0]]
    bx, by = points[triangle[1]]
    cx, cy = points[triangle[2]]

    ax -= px
    ay -= py
    bx -= px
    by -= py
    cx -= px
    cy -= py

    determinant = (
        (ax * ax + ay * ay) * (bx * cy - cx * by)
        - (bx * bx + by * by) * (ax * cy - cx * ay)
        + (cx * cx + cy * cy) * (ax * by - bx * ay)
    )
    orientation = (points[triangle[1]][0] - points[triangle[0]][0]) * (
        points[triangle[2]][1] - points[triangle[0]][1]
    ) - (points[triangle[1]][1] - points[triangle[0]][1]) * (points[triangle[2]][0] - points[triangle[0]][0])
    return determinant > 1e-9 if orientation > 0 else determinant < -1e-9


def delaunay_triangles(samples):
    if len(samples) < 3:
        return []

    points = [(sample["x_m"], sample["y_m"]) for sample in samples]
    min_x = min(point[0] for point in points)
    max_x = max(point[0] for point in points)
    min_y = min(point[1] for point in points)
    max_y = max(point[1] for point in points)
    span = max(max_x - min_x, max_y - min_y, 1.0)
    mid_x = (min_x + max_x) / 2.0
    mid_y = (min_y + max_y) / 2.0
    super_points = [
        (mid_x - 20 * span, mid_y - span),
        (mid_x, mid_y + 20 * span),
        (mid_x + 20 * span, mid_y - span),
    ]
    points.extend(super_points)
    super_triangle = (len(points) - 3, len(points) - 2, len(points) - 1)
    super_indices = set(super_triangle)
    triangles = [super_triangle]

    for point_index in range(len(samples)):
        point = points[point_index]
        bad_triangles = [triangle for triangle in triangles if circumcircle_contains(point, triangle, points)]
        edge_counts = {}
        for triangle in bad_triangles:
            for edge in (
                tuple(sorted((triangle[0], triangle[1]))),
                tuple(sorted((triangle[1], triangle[2]))),
                tuple(sorted((triangle[2], triangle[0]))),
            ):
                edge_counts[edge] = edge_counts.get(edge, 0) + 1
        boundary_edges = [edge for edge, count in edge_counts.items() if count == 1]
        triangles = [triangle for triangle in triangles if triangle not in bad_triangles]
        for edge in boundary_edges:
            triangles.append((edge[0], edge[1], point_index))

    return [triangle for triangle in triangles if not any(index in super_indices for index in triangle)]


def barycentric_value(x, y, triangle, samples, tolerance=1e-9):
    a, b, c = [samples[index] for index in triangle]
    x1, y1, v1 = a["x_m"], a["y_m"], a["value"]
    x2, y2, v2 = b["x_m"], b["y_m"], b["value"]
    x3, y3, v3 = c["x_m"], c["y_m"], c["value"]
    denominator = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3)
    if abs(denominator) < tolerance:
        return None
    w1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denominator
    w2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denominator
    w3 = 1.0 - w1 - w2
    if w1 >= -1e-6 and w2 >= -1e-6 and w3 >= -1e-6:
        return w1 * v1 + w2 * v2 + w3 * v3
    return None


def interpolate_cells(records, metric_key, extent, hull, grid_size):
    min_x, max_x, min_y, max_y = extent
    dx = (max_x - min_x) / grid_size
    dy = (max_y - min_y) / grid_size
    samples = [
        {"x_m": record["x_m"], "y_m": record["y_m"], "value": record[metric_key]}
        for record in records
        if record[metric_key] is not None
    ]
    triangles = delaunay_triangles(samples)
    cells = []
    for row in range(grid_size):
        y0 = min_y + row * dy
        y1 = y0 + dy
        cy = (y0 + y1) / 2.0
        for col in range(grid_size):
            x0 = min_x + col * dx
            x1 = x0 + dx
            cx = (x0 + x1) / 2.0
            if not point_in_polygon((cx, cy), hull):
                continue
            value = None
            for triangle in triangles:
                value = barycentric_value(cx, cy, triangle, samples)
                if value is not None:
                    break
            if value is None:
                continue
            cells.append((x0, y0, x1, y1, value))
    return cells


def color_for_value(value, domain):
    low, high = domain
    if high <= low:
        t = 0.5
    else:
        t = (value - low) / (high - low)
    t = max(0.0, min(1.0, t))
    stops = [
        (0.0, (165, 0, 38)),
        (0.35, (244, 109, 67)),
        (0.55, (255, 255, 191)),
        (0.75, (102, 189, 99)),
        (1.0, (43, 131, 186)),
    ]
    for (left_t, left_rgb), (right_t, right_rgb) in zip(stops, stops[1:]):
        if left_t <= t <= right_t:
            local = (t - left_t) / (right_t - left_t)
            rgb = tuple(round(a + (b - a) * local) for a, b in zip(left_rgb, right_rgb))
            return f"rgb({rgb[0]},{rgb[1]},{rgb[2]})"
    rgb = stops[-1][1]
    return f"rgb({rgb[0]},{rgb[1]},{rgb[2]})"


class PanelGeometry:
    def __init__(self, extent, left=72, top=58, plot_width=400, plot_height=400, right=102, bottom=72):
        self.extent = extent
        self.left = left
        self.top = top
        self.plot_width = plot_width
        self.plot_height = plot_height
        self.right = right
        self.bottom = bottom
        self.width = left + plot_width + right
        self.height = top + plot_height + bottom

    def sx(self, x):
        min_x, max_x, _, _ = self.extent
        return self.left + (x - min_x) / (max_x - min_x) * self.plot_width

    def sy(self, y):
        _, _, min_y, max_y = self.extent
        return self.top + self.plot_height - (y - min_y) / (max_y - min_y) * self.plot_height


def svg_text(x, y, text, size=12, weight="400", anchor="start", extra=""):
    safe_text = escape(str(text))
    return (
        f'<text x="{x:.2f}" y="{y:.2f}" font-size="{size}" font-weight="{weight}" '
        f'text-anchor="{anchor}" {extra}>{safe_text}</text>'
    )


def render_ticks(elements, geometry, to_latlon):
    min_x, max_x, min_y, max_y = geometry.extent
    for idx in range(5):
        fraction = idx / 4.0
        x = min_x + (max_x - min_x) * fraction
        y = min_y + (max_y - min_y) * fraction
        sx = geometry.sx(x)
        sy = geometry.sy(y)
        latitude, longitude = to_latlon(x, min_y)
        elements.append(
            f'<line x1="{sx:.2f}" y1="{geometry.top:.2f}" x2="{sx:.2f}" '
            f'y2="{geometry.top + geometry.plot_height:.2f}" stroke="#d6d6d6" stroke-width="0.6"/>'
        )
        elements.append(
            f'<line x1="{sx:.2f}" y1="{geometry.top + geometry.plot_height:.2f}" x2="{sx:.2f}" '
            f'y2="{geometry.top + geometry.plot_height + 5:.2f}" stroke="#1f2933" stroke-width="0.8"/>'
        )
        elements.append(svg_text(sx, geometry.top + geometry.plot_height + 20, f"{longitude:.5f}", 9, anchor="middle"))

        latitude, longitude = to_latlon(min_x, y)
        elements.append(
            f'<line x1="{geometry.left:.2f}" y1="{sy:.2f}" x2="{geometry.left + geometry.plot_width:.2f}" '
            f'y2="{sy:.2f}" stroke="#d6d6d6" stroke-width="0.6"/>'
        )
        elements.append(
            f'<line x1="{geometry.left - 5:.2f}" y1="{sy:.2f}" x2="{geometry.left:.2f}" '
            f'y2="{sy:.2f}" stroke="#1f2933" stroke-width="0.8"/>'
        )
        elements.append(svg_text(geometry.left - 9, sy + 3, f"{latitude:.5f}", 9, anchor="end"))

    elements.append(svg_text(geometry.left + geometry.plot_width / 2, geometry.height - 18, "Longitude", 11, anchor="middle"))
    elements.append(
        svg_text(
            18,
            geometry.top + geometry.plot_height / 2,
            "Latitude",
            11,
            anchor="middle",
            extra=f'transform="rotate(-90 18 {geometry.top + geometry.plot_height / 2:.2f})"',
        )
    )


def render_scale_bar(elements, geometry):
    min_x, max_x, _, _ = geometry.extent
    span = max_x - min_x
    candidates = [25, 50, 100, 150, 200, 250, 500]
    length = min(candidates, key=lambda value: abs(value - span / 4.0))
    x0 = geometry.left + geometry.plot_width - geometry.plot_width * length / span - 18
    x1 = geometry.left + geometry.plot_width - 18
    y = geometry.top + geometry.plot_height - 18
    elements.append(f'<line x1="{x0:.2f}" y1="{y:.2f}" x2="{x1:.2f}" y2="{y:.2f}" stroke="#111827" stroke-width="2"/>')
    elements.append(f'<line x1="{x0:.2f}" y1="{y - 4:.2f}" x2="{x0:.2f}" y2="{y + 4:.2f}" stroke="#111827" stroke-width="1.2"/>')
    elements.append(f'<line x1="{x1:.2f}" y1="{y - 4:.2f}" x2="{x1:.2f}" y2="{y + 4:.2f}" stroke="#111827" stroke-width="1.2"/>')
    elements.append(svg_text((x0 + x1) / 2, y - 7, f"{length} m", 10, anchor="middle"))


def render_colorbar(elements, geometry, domain, unit, scale=1.0):
    low, high = domain
    bar_x = geometry.left + geometry.plot_width + 30 * scale
    bar_y = geometry.top + 52 * scale
    bar_w = 14 * scale
    bar_h = 230 * scale
    steps = 90
    for idx in range(steps):
        fraction = idx / steps
        value = high - (high - low) * fraction
        y = bar_y + fraction * bar_h
        elements.append(
            f'<rect x="{bar_x:.2f}" y="{y:.2f}" width="{bar_w}" height="{bar_h / steps + 0.4:.2f}" '
            f'fill="{color_for_value(value, domain)}" stroke="none"/>'
        )
    elements.append(
        f'<rect x="{bar_x:.2f}" y="{bar_y:.2f}" width="{bar_w}" height="{bar_h}" '
        f'fill="none" stroke="#111827" stroke-width="{0.8 * scale:.2f}"/>'
    )
    elements.append(svg_text(bar_x + bar_w + 8 * scale, bar_y + 4 * scale, f"{high:g}", 9 * scale, "700"))
    elements.append(svg_text(bar_x + bar_w + 8 * scale, bar_y + bar_h + 3 * scale, f"{low:g}", 9 * scale, "700"))
    elements.append(svg_text(bar_x + bar_w / 2, bar_y - 10 * scale, unit, 10 * scale, "700", anchor="middle"))
    elements.append(svg_text(bar_x + bar_w / 2, bar_y + bar_h + 18 * scale, "low", 9 * scale, anchor="middle"))
    elements.append(svg_text(bar_x + bar_w / 2, bar_y - 24 * scale, "high", 9 * scale, anchor="middle"))


def render_points(elements, records, geometry):
    for record in records:
        sx = geometry.sx(record["x_m"])
        sy = geometry.sy(record["y_m"])
        if record["is_base_station"]:
            points = [(sx, sy - 10), (sx - 9, sy + 8), (sx + 9, sy + 8)]
            point_text = " ".join(f"{x:.2f},{y:.2f}" for x, y in points)
            elements.append(f'<polygon points="{point_text}" fill="#111827" stroke="#ffffff" stroke-width="1.4"/>')
            elements.append(svg_text(sx + 11, sy - 9, "BS 0", 10, "700"))
        else:
            elements.append(f'<circle cx="{sx:.2f}" cy="{sy:.2f}" r="4.2" fill="#ffffff" stroke="#111827" stroke-width="1.1"/>')
            elements.append(svg_text(sx + 5.5, sy - 5.5, record["id"], 8, "700"))


def render_panel(metric, records, cells, extent, to_latlon, geometry=None):
    if geometry is None:
        geometry = PanelGeometry(extent)
    elements = []
    elements.append(f'<rect x="0" y="0" width="{geometry.width}" height="{geometry.height}" fill="#ffffff"/>')
    elements.append(svg_text(geometry.left + geometry.plot_width / 2, 24, f'{metric["label"]} ({metric["unit"]})', 16, "700", "middle"))
    elements.append(svg_text(geometry.left + geometry.plot_width / 2, 43, metric["description"], 10, "400", "middle"))
    elements.append(
        f'<rect x="{geometry.left:.2f}" y="{geometry.top:.2f}" width="{geometry.plot_width}" '
        f'height="{geometry.plot_height}" fill="#f7f7f7" stroke="#111827" stroke-width="1.0"/>'
    )

    for x0, y0, x1, y1, value in cells:
        sx0 = geometry.sx(x0)
        sx1 = geometry.sx(x1)
        sy0 = geometry.sy(y1)
        sy1 = geometry.sy(y0)
        elements.append(
            f'<rect x="{sx0:.2f}" y="{sy0:.2f}" width="{sx1 - sx0 + 0.25:.2f}" '
            f'height="{sy1 - sy0 + 0.25:.2f}" fill="{color_for_value(value, metric["domain"])}" '
            f'opacity="0.90" stroke="none"/>'
        )

    render_ticks(elements, geometry, to_latlon)
    elements.append(
        f'<rect x="{geometry.left:.2f}" y="{geometry.top:.2f}" width="{geometry.plot_width}" '
        f'height="{geometry.plot_height}" fill="none" stroke="#111827" stroke-width="1.0"/>'
    )
    render_scale_bar(elements, geometry)
    render_colorbar(elements, geometry, metric["domain"], metric["unit"])
    render_points(elements, records, geometry)
    elements.append(svg_text(geometry.left + 8, geometry.top + 15, "Triangulated linear interpolation; points show measurements", 9, anchor="start"))
    return elements, geometry


def latlon_to_global_pixel(latitude, longitude, zoom):
    latitude = max(min(latitude, 85.05112878), -85.05112878)
    sin_lat = math.sin(math.radians(latitude))
    scale = TILE_SIZE * (2**zoom)
    x = (longitude + 180.0) / 360.0 * scale
    y = (0.5 - math.log((1.0 + sin_lat) / (1.0 - sin_lat)) / (4.0 * math.pi)) * scale
    return x, y


class OSMPanelGeometry:
    def __init__(self, pixel_bounds, zoom, left=70, top=118, right=235, bottom=105):
        self.pixel_bounds = pixel_bounds
        self.zoom = zoom
        self.left = left
        self.top = top
        self.right = right
        self.bottom = bottom
        min_px, max_px, min_py, max_py = pixel_bounds
        self.plot_width = max_px - min_px
        self.plot_height = max_py - min_py
        self.width = left + self.plot_width + right
        self.height = top + self.plot_height + bottom

    def screen_from_pixel(self, px, py):
        min_px, _, min_py, _ = self.pixel_bounds
        return self.left + px - min_px, self.top + py - min_py

    def screen_from_latlon(self, latitude, longitude):
        return self.screen_from_pixel(*latlon_to_global_pixel(latitude, longitude, self.zoom))


def osm_pixel_bounds(records, zoom, pad_px=120):
    pixels = [latlon_to_global_pixel(record["latitude"], record["longitude"], zoom) for record in records]
    xs = [point[0] for point in pixels]
    ys = [point[1] for point in pixels]
    return min(xs) - pad_px, max(xs) + pad_px, min(ys) - pad_px, max(ys) + pad_px


def fetch_osm_tile(z, x, y, cache_dir):
    tile_path = cache_dir / str(z) / str(x) / f"{y}.png"
    if tile_path.exists() and tile_path.stat().st_size > 0:
        return tile_path

    tile_path.parent.mkdir(parents=True, exist_ok=True)
    request = urllib.request.Request(
        OSM_TILE_URL.format(z=z, x=x, y=y),
        headers={
            "User-Agent": "Edge4AV-signal-map-generator/1.0 (research figure generation)",
            "Accept": "image/png",
        },
    )
    with urllib.request.urlopen(request, timeout=30) as response:
        tile_path.write_bytes(response.read())
    return tile_path


def tile_data_uri(tile_path):
    encoded = base64.b64encode(tile_path.read_bytes()).decode("ascii")
    return f"data:image/png;base64,{encoded}"


def render_osm_tiles(elements, geometry, cache_dir):
    min_px, max_px, min_py, max_py = geometry.pixel_bounds
    min_tile_x = math.floor(min_px / TILE_SIZE)
    max_tile_x = math.floor((max_px - 1) / TILE_SIZE)
    min_tile_y = math.floor(min_py / TILE_SIZE)
    max_tile_y = math.floor((max_py - 1) / TILE_SIZE)

    elements.append(
        f'<rect x="{geometry.left:.2f}" y="{geometry.top:.2f}" width="{geometry.plot_width:.2f}" '
        f'height="{geometry.plot_height:.2f}" fill="#e9ecef" stroke="none"/>'
    )
    for tile_x in range(min_tile_x, max_tile_x + 1):
        for tile_y in range(min_tile_y, max_tile_y + 1):
            tile_path = fetch_osm_tile(geometry.zoom, tile_x, tile_y, cache_dir)
            screen_x, screen_y = geometry.screen_from_pixel(tile_x * TILE_SIZE, tile_y * TILE_SIZE)
            elements.append(
                f'<image x="{screen_x:.2f}" y="{screen_y:.2f}" width="{TILE_SIZE}" height="{TILE_SIZE}" '
                f'href="{tile_data_uri(tile_path)}"/>'
            )
    elements.append(
        f'<rect x="{geometry.left:.2f}" y="{geometry.top:.2f}" width="{geometry.plot_width:.2f}" '
        f'height="{geometry.plot_height:.2f}" fill="none" stroke="#111827" stroke-width="1.0"/>'
    )


def render_osm_heatmap(elements, cells, metric, geometry, to_latlon):
    for x0, y0, x1, y1, value in cells:
        lat_top, lon_left = to_latlon(x0, y1)
        lat_bottom, lon_right = to_latlon(x1, y0)
        px0, py0 = latlon_to_global_pixel(lat_top, lon_left, geometry.zoom)
        px1, py1 = latlon_to_global_pixel(lat_bottom, lon_right, geometry.zoom)
        sx0, sy0 = geometry.screen_from_pixel(px0, py0)
        sx1, sy1 = geometry.screen_from_pixel(px1, py1)
        elements.append(
            f'<rect x="{sx0:.2f}" y="{sy0:.2f}" width="{sx1 - sx0 + 0.35:.2f}" '
            f'height="{sy1 - sy0 + 0.35:.2f}" fill="{color_for_value(value, metric["domain"])}" '
            f'opacity="0.56" stroke="none"/>'
        )


def render_osm_points(elements, records, geometry):
    for record in records:
        sx, sy = geometry.screen_from_latlon(record["latitude"], record["longitude"])
        if record["is_base_station"]:
            points = [(sx, sy - 30), (sx - 28, sy + 23), (sx + 28, sy + 23)]
            point_text = " ".join(f"{x:.2f},{y:.2f}" for x, y in points)
            elements.append(f'<polygon points="{point_text}" fill="#111827" stroke="#ffffff" stroke-width="5.0"/>')
            elements.append(svg_text(sx + 36, sy - 24, "BS 0", 32, "700"))
        else:
            elements.append(f'<circle cx="{sx:.2f}" cy="{sy:.2f}" r="16.0" fill="#ffffff" stroke="#111827" stroke-width="3.2"/>')
            elements.append(svg_text(sx, sy + 7.0, record["id"], 21, "700", anchor="middle"))


def render_run_markers(elements, run_locations, geometry):
    colors = ["#5b21b6", "#b45309", "#047857", "#be123c"]
    offsets = [(34, -34), (34, 56), (34, -34), (34, 56)]
    for idx, run in enumerate(run_locations):
        sx, sy = geometry.screen_from_latlon(run["latitude"], run["longitude"])
        color = colors[idx % len(colors)]
        label = re.sub(r"[^0-9]", "", run["label"]) or str(idx + 1)
        elements.append(f'<circle cx="{sx:.2f}" cy="{sy:.2f}" r="30.0" fill="{color}" stroke="#ffffff" stroke-width="6.0"/>')
        elements.append(svg_text(sx, sy + 10.5, label, 30, "700", anchor="middle", extra='style="fill:#ffffff"'))
        dx, dy = offsets[idx % len(offsets)]
        text_x = sx + dx
        text_y = sy + dy
        text = run["label"]
        box_w = 128
        box_h = 46
        elements.append(
            f'<rect x="{text_x - 4:.2f}" y="{text_y - 13:.2f}" width="{box_w}" height="{box_h}" '
            f'rx="6" fill="#ffffff" fill-opacity="0.92" stroke="{color}" stroke-width="3.0"/>'
        )
        elements.append(svg_text(text_x + box_w / 2 - 4, text_y + 18, text, 30, "700", anchor="middle"))


def render_run_legend(elements, run_locations, geometry, excluded_locations=None):
    outside_runs = [
        item for item in (excluded_locations or [])
        if item.get("label", "").lower().startswith("run")
    ]
    if not run_locations and not outside_runs:
        return
    colors = ["#5b21b6", "#b45309", "#047857", "#be123c"]
    x = geometry.left + geometry.plot_width - 330
    y = geometry.top + 44
    line_height = 45
    height = 52 + line_height * len(run_locations)
    if outside_runs:
        height += 48 + line_height * len(outside_runs)
    elements.append(
        f'<rect x="{x - 18:.2f}" y="{y - 38:.2f}" width="310" height="{height}" '
        f'rx="8" fill="#ffffff" fill-opacity="0.88" stroke="#111827" stroke-width="2.0"/>'
    )
    elements.append(svg_text(x, y, "Signal-mapped runs", 30, "700"))
    for idx, run in enumerate(run_locations):
        row_y = y + 16 + idx * line_height
        color = colors[idx % len(colors)]
        label = re.sub(r"[^0-9]", "", run["label"]) or str(idx + 1)
        elements.append(f'<circle cx="{x + 17:.2f}" cy="{row_y - 9:.2f}" r="16.0" fill="{color}" stroke="#ffffff" stroke-width="3.0"/>')
        elements.append(svg_text(x + 17, row_y, label, 18, "700", anchor="middle", extra='style="fill:#ffffff"'))
        elements.append(svg_text(x + 44, row_y, run["label"], 27, "700"))
    if outside_runs:
        start_y = y + 36 + len(run_locations) * line_height
        elements.append(svg_text(x, start_y, "Outside footprint", 25, "700"))
        for idx, run in enumerate(outside_runs):
            row_y = start_y + 35 + idx * line_height
            label = re.sub(r"[^0-9]", "", run["label"]) or str(idx + 1)
            elements.append(f'<circle cx="{x + 17:.2f}" cy="{row_y - 9:.2f}" r="16.0" fill="#6b7280" stroke="#ffffff" stroke-width="3.0"/>')
            elements.append(svg_text(x + 17, row_y, label, 18, "700", anchor="middle", extra='style="fill:#ffffff"'))
            elements.append(svg_text(x + 44, row_y, f'{run["label"]} off map', 25, "700"))


def render_osm_scale_bar(elements, geometry, records):
    center_lat = statistics.mean(record["latitude"] for record in records)
    meters_per_pixel = 156543.03392 * math.cos(math.radians(center_lat)) / (2**geometry.zoom)
    target_m = geometry.plot_width * meters_per_pixel / 4.0
    candidates = [25, 50, 100, 150, 200, 250, 500]
    length_m = min(candidates, key=lambda value: abs(value - target_m))
    length_px = length_m / meters_per_pixel
    x1 = geometry.left + geometry.plot_width - 18
    x0 = x1 - length_px
    y = geometry.top + geometry.plot_height - 18
    elements.append(f'<line x1="{x0:.2f}" y1="{y:.2f}" x2="{x1:.2f}" y2="{y:.2f}" stroke="#111827" stroke-width="6.0"/>')
    elements.append(f'<line x1="{x0:.2f}" y1="{y - 12:.2f}" x2="{x0:.2f}" y2="{y + 12:.2f}" stroke="#111827" stroke-width="4.0"/>')
    elements.append(f'<line x1="{x1:.2f}" y1="{y - 12:.2f}" x2="{x1:.2f}" y2="{y + 12:.2f}" stroke="#111827" stroke-width="4.0"/>')
    elements.append(svg_text((x0 + x1) / 2, y - 20, f"{length_m} m", 28, "700", anchor="middle"))


def render_osm_panel(metric, records, cells, to_latlon, geometry, cache_dir, run_locations=None, excluded_locations=None):
    elements = []
    elements.append(f'<rect x="0" y="0" width="{geometry.width:.2f}" height="{geometry.height:.2f}" fill="#ffffff"/>')
    subtitle = "OpenStreetMap snapshot with interpolated signal overlay"
    if run_locations:
        subtitle = "OpenStreetMap signal overlay with signal-mapped run locations"
    render_osm_tiles(elements, geometry, cache_dir)
    render_osm_heatmap(elements, cells, metric, geometry, to_latlon)
    render_osm_points(elements, records, geometry)
    if run_locations:
        render_run_markers(elements, run_locations, geometry)
        render_run_legend(elements, run_locations, geometry, excluded_locations)
    render_osm_scale_bar(elements, geometry, records)
    render_colorbar(elements, geometry, metric["domain"], metric["unit"], scale=OSM_LABEL_SCALE)
    elements.append(svg_text(geometry.left + 16, geometry.top + 36, "Triangulated linear interpolation", 26, "700"))
    elements.append(svg_text(geometry.left + 16, geometry.top + geometry.plot_height - 18, "(C) OpenStreetMap contributors", 23, "700"))
    elements.append(f'<rect x="0" y="0" width="{geometry.width:.2f}" height="{geometry.top:.2f}" fill="#ffffff"/>')
    elements.append(svg_text(geometry.left + geometry.plot_width / 2, 48, f'{metric["label"]} ({metric["unit"]})', 48, "700", "middle"))
    elements.append(svg_text(geometry.left + geometry.plot_width / 2, 88, subtitle, 28, "400", "middle"))
    return elements


def write_svg(path, width, height, elements):
    payload = "\n".join(elements)
    path.write_text(
        "\n".join(
            [
                '<?xml version="1.0" encoding="UTF-8"?>',
                f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
                '<style>text { font-family: Arial, Helvetica, sans-serif; fill: #111827; } '
                '.panel-label { font-weight: 700; }</style>',
                payload,
                "</svg>",
                "",
            ]
        ),
        encoding="utf-8",
    )


def write_parsed_csv(path, records):
    fieldnames = [
        "id",
        "latitude",
        "longitude",
        "east_m_from_base",
        "north_m_from_base",
        "distance_to_base_m",
        "is_base_station",
        "rsrp_dbm",
        "rsrq_db",
        "snr_db",
        "snr_raw",
    ]
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for record in records:
            writer.writerow(
                {
                    "id": record["id"],
                    "latitude": f'{record["latitude"]:.8f}',
                    "longitude": f'{record["longitude"]:.8f}',
                    "east_m_from_base": f'{record["east_m_from_base"]:.2f}',
                    "north_m_from_base": f'{record["north_m_from_base"]:.2f}',
                    "distance_to_base_m": f'{record["distance_to_base_m"]:.2f}',
                    "is_base_station": str(record["is_base_station"]).lower(),
                    "rsrp_dbm": "" if record["rsrp_dbm"] is None else f'{record["rsrp_dbm"]:.2f}',
                    "rsrq_db": "" if record["rsrq_db"] is None else f'{record["rsrq_db"]:.2f}',
                    "snr_db": "" if record["snr_db"] is None else f'{record["snr_db"]:.2f}',
                    "snr_raw": record["snr_raw"],
                }
            )


def read_gnss_points(path):
    path = Path(path)
    opener = gzip.open if path.suffix == ".gz" else open
    points = []
    with opener(path, "rt", newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            try:
                latitude = float(row.get("latitude_deg", ""))
                longitude = float(row.get("longitude_deg", ""))
            except ValueError:
                continue
            if not math.isfinite(latitude) or not math.isfinite(longitude):
                continue
            if abs(latitude) < 1e-9 and abs(longitude) < 1e-9:
                continue
            points.append((latitude, longitude, row))
    return points


def signal_bbox(records, pad_deg=0.01):
    return (
        min(record["latitude"] for record in records) - pad_deg,
        max(record["latitude"] for record in records) + pad_deg,
        min(record["longitude"] for record in records) - pad_deg,
        max(record["longitude"] for record in records) + pad_deg,
    )


def inside_bbox(latitude, longitude, bbox):
    min_lat, max_lat, min_lon, max_lon = bbox
    return min_lat <= latitude <= max_lat and min_lon <= longitude <= max_lon


def median_gnss_location(source):
    points = read_gnss_points(source)
    if not points:
        return None
    latitudes = [point[0] for point in points]
    longitudes = [point[1] for point in points]
    return {
        "latitude": statistics.median(latitudes),
        "longitude": statistics.median(longitudes),
        "sample_count": len(points),
        "lat_min": min(latitudes),
        "lat_max": max(latitudes),
        "lon_min": min(longitudes),
        "lon_max": max(longitudes),
    }


def load_run_locations(records):
    bbox = signal_bbox(records)
    run_locations = []
    excluded = []
    for source in RUN_LOCATION_SOURCES:
        stats = median_gnss_location(source["source"])
        if stats is None:
            excluded.append({**source, "reason": "no valid GNSS samples"})
            continue
        item = {**source, **stats}
        if inside_bbox(item["latitude"], item["longitude"], bbox):
            run_locations.append(item)
        else:
            excluded.append({**item, "reason": "median GNSS location falls outside the signal-map footprint"})

    for source in EXCLUDED_GNSS_SOURCES:
        stats = median_gnss_location(source["source"])
        if stats is None:
            excluded.append({**source, "sample_count": 0})
        else:
            excluded.append({**source, **stats})
    return run_locations, excluded


def write_run_locations_csv(path, run_locations, excluded):
    fieldnames = [
        "label",
        "description",
        "latitude",
        "longitude",
        "sample_count",
        "lat_min",
        "lat_max",
        "lon_min",
        "lon_max",
        "included",
        "reason",
        "source",
    ]
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for item in run_locations:
            writer.writerow(
                {
                    "label": item["label"],
                    "description": item.get("description", ""),
                    "latitude": f'{item["latitude"]:.9f}',
                    "longitude": f'{item["longitude"]:.9f}',
                    "sample_count": item.get("sample_count", ""),
                    "lat_min": f'{item["lat_min"]:.9f}',
                    "lat_max": f'{item["lat_max"]:.9f}',
                    "lon_min": f'{item["lon_min"]:.9f}',
                    "lon_max": f'{item["lon_max"]:.9f}',
                    "included": "true",
                    "reason": "",
                    "source": item["source"],
                }
            )
        for item in excluded:
            writer.writerow(
                {
                    "label": item["label"],
                    "description": item.get("description", ""),
                    "latitude": "" if "latitude" not in item else f'{item["latitude"]:.9f}',
                    "longitude": "" if "longitude" not in item else f'{item["longitude"]:.9f}',
                    "sample_count": item.get("sample_count", ""),
                    "lat_min": "" if "lat_min" not in item else f'{item["lat_min"]:.9f}',
                    "lat_max": "" if "lat_max" not in item else f'{item["lat_max"]:.9f}',
                    "lon_min": "" if "lon_min" not in item else f'{item["lon_min"]:.9f}',
                    "lon_max": "" if "lon_max" not in item else f'{item["lon_max"]:.9f}',
                    "included": "false",
                    "reason": item.get("reason", ""),
                    "source": item["source"],
                }
            )


def metric_stats(records, metric_key):
    values = [record[metric_key] for record in records if record[metric_key] is not None]
    return {
        "count": len(values),
        "min": min(values),
        "max": max(values),
        "mean": statistics.mean(values),
    }


def write_summary(path, records, run_locations, excluded_locations, generated_files, grid_size, osm_enabled, osm_zoom):
    lines = [
        "# Signal Strength Map Summary",
        "",
        "Generated from `results/real_5g/Signal.ods`.",
        "",
        "Assumptions:",
        "",
        "- ID `0` is the base station and is plotted as a marker, but it is not used as a measured signal sample because its RSRP, RSRQ, and SNR cells are empty.",
        "- SNR ranges are represented by their midpoint before interpolation.",
        "- SNR is a dimensionless ratio; the survey reports its logarithmic value in dB.",
        f"- Interpolation uses Delaunay triangulation with barycentric linear interpolation on a `{grid_size} x {grid_size}` grid.",
        "- Interpolated cells outside the triangulated measurement area are masked.",
    ]
    if osm_enabled:
        lines.extend(
            [
                f"- OpenStreetMap Standard tiles are cached locally and embedded in the OSM overlay SVGs at zoom `{osm_zoom}`.",
                "- OpenStreetMap data attribution: `(C) OpenStreetMap contributors`.",
            ]
        )
    lines.extend(["", "Generated files:", ""])
    for output in generated_files:
        lines.append(f"- `{output}`")
    lines.extend(
        [
            "",
            "Measurement summary:",
            "",
            "| Metric | Samples | Min | Mean | Max | Color scale |",
            "|---|---:|---:|---:|---:|---|",
        ]
    )
    for metric in METRICS:
        stats = metric_stats(records, metric["key"])
        low, high = metric["domain"]
        lines.append(
            f'| {metric["label"]} ({metric["unit"]}) | {stats["count"]} | '
            f'{stats["min"]:.2f} | {stats["mean"]:.2f} | {stats["max"]:.2f} | '
            f'{low:g} to {high:g} {metric["unit"]} |'
        )
    base = next(record for record in records if record["is_base_station"])
    lines.extend(
        [
            "",
            f'Base station: ID `0` at `{base["latitude"]:.8f}, {base["longitude"]:.8f}`.',
            "",
            "Signal-mapped run-location markers:",
            "",
            "| Label | Latitude | Longitude | GNSS samples | Source |",
            "|---|---:|---:|---:|---|",
        ]
    )
    for run in run_locations:
        lines.append(
            f'| {run["label"]} | {run["latitude"]:.9f} | {run["longitude"]:.9f} | '
            f'{run["sample_count"]} | `{run["source"]}` |'
        )
    if excluded_locations:
        lines.extend(["", "GNSS captures outside the plotted signal footprint:", ""])
        for item in excluded_locations:
            location = ""
            if "latitude" in item and "longitude" in item:
                location = f' at `{item["latitude"]:.9f}, {item["longitude"]:.9f}`'
            lines.append(f'- `{item["label"]}`{location}: {item.get("reason", "not plotted")}.')
    lines.append("")
    path.write_text("\n".join(lines), encoding="utf-8")


def build_maps(input_path, output_dir, grid_size, osm_zoom, skip_osm):
    records = load_measurements(input_path)
    to_latlon = enrich_coordinates(records)
    run_locations, excluded_locations = load_run_locations(records)
    extent = plot_extent(records)
    hull = convex_hull([(record["x_m"], record["y_m"]) for record in records])
    output_dir.mkdir(parents=True, exist_ok=True)

    parsed_csv = output_dir / "signal_measurements_parsed.csv"
    write_parsed_csv(parsed_csv, records)
    run_csv = output_dir / "run_locations_parsed.csv"
    write_run_locations_csv(run_csv, run_locations, excluded_locations)

    generated_files = [parsed_csv, run_csv]
    panel_payloads = []
    for metric in METRICS:
        cells = interpolate_cells(records, metric["key"], extent, hull, grid_size)
        elements, geometry = render_panel(metric, records, cells, extent, to_latlon)
        individual_path = output_dir / f'signal_map_{metric["label"].lower()}.svg'
        write_svg(individual_path, geometry.width, geometry.height, elements)
        generated_files.append(individual_path)
        panel_payloads.append((metric, cells))

    combined_geometry = PanelGeometry(extent, left=64, top=58, plot_width=330, plot_height=330, right=92, bottom=68)
    combined_width = combined_geometry.width * len(METRICS)
    combined_height = combined_geometry.height
    combined_elements = [f'<rect x="0" y="0" width="{combined_width}" height="{combined_height}" fill="#ffffff"/>']
    for idx, (metric, cells) in enumerate(panel_payloads):
        elements, _ = render_panel(metric, records, cells, extent, to_latlon, combined_geometry)
        offset_x = idx * combined_geometry.width
        combined_elements.append(f'<g transform="translate({offset_x},0)">')
        combined_elements.extend(elements[1:])
        combined_elements.append("</g>")
    combined_path = output_dir / "signal_strength_maps.svg"
    write_svg(combined_path, combined_width, combined_height, combined_elements)
    generated_files.append(combined_path)

    if not skip_osm:
        tile_cache_dir = output_dir / "osm_tiles"
        osm_bounds_records = records + [
            {"latitude": run["latitude"], "longitude": run["longitude"]}
            for run in run_locations
        ]
        osm_geometry = OSMPanelGeometry(osm_pixel_bounds(osm_bounds_records, osm_zoom), osm_zoom)
        osm_combined_width = osm_geometry.width * len(METRICS)
        osm_combined_height = osm_geometry.height
        osm_combined_elements = [
            f'<rect x="0" y="0" width="{osm_combined_width:.2f}" height="{osm_combined_height:.2f}" fill="#ffffff"/>'
        ]
        run_combined_elements = [
            f'<rect x="0" y="0" width="{osm_combined_width:.2f}" height="{osm_combined_height:.2f}" fill="#ffffff"/>'
        ]
        for idx, (metric, cells) in enumerate(panel_payloads):
            elements = render_osm_panel(metric, records, cells, to_latlon, osm_geometry, tile_cache_dir)
            osm_path = output_dir / f'signal_osm_{metric["label"].lower()}.svg'
            write_svg(osm_path, osm_geometry.width, osm_geometry.height, elements)
            generated_files.append(osm_path)

            offset_x = idx * osm_geometry.width
            osm_combined_elements.append(f'<g transform="translate({offset_x:.2f},0)">')
            osm_combined_elements.extend(elements[1:])
            osm_combined_elements.append("</g>")

            run_elements = render_osm_panel(
                metric,
                records,
                cells,
                to_latlon,
                osm_geometry,
                tile_cache_dir,
                run_locations,
                excluded_locations,
            )
            run_path = output_dir / f'signal_osm_runs_{metric["label"].lower()}.svg'
            write_svg(run_path, osm_geometry.width, osm_geometry.height, run_elements)
            generated_files.append(run_path)

            run_combined_elements.append(f'<g transform="translate({offset_x:.2f},0)">')
            run_combined_elements.extend(run_elements[1:])
            run_combined_elements.append("</g>")
        osm_combined_path = output_dir / "signal_osm_overlay_maps.svg"
        write_svg(osm_combined_path, osm_combined_width, osm_combined_height, osm_combined_elements)
        generated_files.append(osm_combined_path)
        run_combined_path = output_dir / "signal_osm_runs_overlay_maps.svg"
        write_svg(run_combined_path, osm_combined_width, osm_combined_height, run_combined_elements)
        generated_files.append(run_combined_path)

    summary_path = output_dir / "signal_maps_summary.md"
    write_summary(summary_path, records, run_locations, excluded_locations, generated_files, grid_size, not skip_osm, osm_zoom)
    generated_files.append(summary_path)
    return generated_files


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", default="results/real_5g/Signal.ods", help="Input ODS file")
    parser.add_argument("--output-dir", default="results/real_5g", help="Directory for generated maps")
    parser.add_argument("--grid-size", type=int, default=100, help="Interpolation grid resolution per axis")
    parser.add_argument("--osm-zoom", type=int, default=18, help="OpenStreetMap tile zoom for overlay outputs")
    parser.add_argument("--skip-osm", action="store_true", help="Skip OpenStreetMap overlay outputs")
    args = parser.parse_args()

    generated = build_maps(Path(args.input), Path(args.output_dir), args.grid_size, args.osm_zoom, args.skip_osm)
    for path in generated:
        print(path)


if __name__ == "__main__":
    main()
