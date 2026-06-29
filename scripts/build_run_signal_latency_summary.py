#!/usr/bin/env python3
"""Join stationary run locations, signal survey values, and compact latency."""

import argparse
import csv
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_signal_strength_maps as signal_maps  # noqa: E402


RUN_DIRS = {
    "Run 1": "20260513_sunny_fintechparking_run_1",
    "Run 2": "20260514_sunny_after_rain_run_1",
    "Run 3": "20260515_sunny_run_1",
    "Run 4": "20260522_cloudy_run_1",
    "Run 5": "20260521_small_rain_run_1",
}


def percentile(values, pct):
    values = sorted(value for value in values if value is not None and value >= 0)
    if not values:
        return None
    index = max(0, min(len(values) - 1, math.ceil((pct / 100.0) * len(values)) - 1))
    return values[index]


def summarize_sender_csv(path):
    values = []
    attempts = 0
    accepted = 0
    with path.open(newline="") as handle:
        for row in csv.DictReader(handle):
            attempts += 1
            if str(row.get("accepted", "")).lower() != "true":
                continue
            accepted += 1
            try:
                values.append(float(row.get("rtt_ms", "")))
            except ValueError:
                pass
    return {
        "attempts": attempts,
        "accepted": accepted,
        "p50": percentile(values, 50),
        "p95": percentile(values, 95),
        "p99": percentile(values, 99),
    }


def find_sender_summary(run_dir, transport, payload_bytes):
    for name in [
        f"p5g-{transport}-service-payload-{payload_bytes}_sender.csv",
        f"p5g-{transport}-latency-payload-{payload_bytes}_sender.csv",
    ]:
        path = run_dir / name
        if path.exists():
            return summarize_sender_csv(path)
    return {"attempts": 0, "accepted": 0, "p50": None, "p95": None, "p99": None}


def format_float(value, digits=1):
    if value is None:
        return ""
    return f"{value:.{digits}f}"


def build_metric_interpolators(records):
    to_xy, _ = signal_maps.make_projection(records)
    interpolators = {}
    for metric in signal_maps.METRICS:
        samples = [
            {
                "x_m": record["x_m"],
                "y_m": record["y_m"],
                "value": record[metric["key"]],
                "id": record["id"],
            }
            for record in records
            if record[metric["key"]] is not None
        ]
        interpolators[metric["key"]] = {
            "samples": samples,
            "triangles": signal_maps.delaunay_triangles(samples),
        }
    return to_xy, interpolators


def signal_value_at(latitude, longitude, metric_key, to_xy, interpolators):
    x, y = to_xy(latitude, longitude)
    interpolator = interpolators[metric_key]
    for triangle in interpolator["triangles"]:
        value = signal_maps.barycentric_value(x, y, triangle, interpolator["samples"])
        if value is not None:
            return value, "interp", ""

    nearest = min(
        interpolator["samples"],
        key=lambda sample: math.hypot(x - sample["x_m"], y - sample["y_m"]),
    )
    return nearest["value"], "nearest", str(nearest["id"])


def build_summary(input_path, results_dir, output_path):
    records = signal_maps.load_measurements(input_path)
    signal_maps.enrich_coordinates(records)
    run_locations, _ = signal_maps.load_run_locations(records)
    to_xy, interpolators = build_metric_interpolators(records)

    fieldnames = [
        "run",
        "latitude",
        "longitude",
        "rsrp_dbm",
        "rsrq_db",
        "snr_db_reported",
        "signal_assignment",
        "nearest_signal_id",
        "mqtt_1k_p50_ms",
        "mqtt_1k_p95_ms",
        "tcp_1k_p50_ms",
        "tcp_1k_p95_ms",
        "mqtt_0b_p50_ms",
        "tcp_0b_p50_ms",
    ]

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for run in run_locations:
            dirname = RUN_DIRS[run["label"]]
            values = []
            assignments = []
            nearest_ids = []
            for key in ["rsrp_dbm", "rsrq_db", "snr_db"]:
                value, assignment, nearest_id = signal_value_at(
                    run["latitude"],
                    run["longitude"],
                    key,
                    to_xy,
                    interpolators,
                )
                values.append(value)
                assignments.append(assignment)
                if nearest_id:
                    nearest_ids.append(nearest_id)

            run_dir = results_dir / dirname
            mqtt_1k = find_sender_summary(run_dir, "mqtt", 1024)
            tcp_1k = find_sender_summary(run_dir, "tcp", 1024)
            mqtt_0b = find_sender_summary(run_dir, "mqtt", 0)
            tcp_0b = find_sender_summary(run_dir, "tcp", 0)

            writer.writerow(
                {
                    "run": run["label"],
                    "latitude": f'{run["latitude"]:.9f}',
                    "longitude": f'{run["longitude"]:.9f}',
                    "rsrp_dbm": format_float(values[0]),
                    "rsrq_db": format_float(values[1]),
                    "snr_db_reported": format_float(values[2]),
                    "signal_assignment": "+".join(sorted(set(assignments))),
                    "nearest_signal_id": ";".join(nearest_ids),
                    "mqtt_1k_p50_ms": format_float(mqtt_1k["p50"]),
                    "mqtt_1k_p95_ms": format_float(mqtt_1k["p95"]),
                    "tcp_1k_p50_ms": format_float(tcp_1k["p50"]),
                    "tcp_1k_p95_ms": format_float(tcp_1k["p95"]),
                    "mqtt_0b_p50_ms": format_float(mqtt_0b["p50"]),
                    "tcp_0b_p50_ms": format_float(tcp_0b["p50"]),
                }
            )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", default="results/real_5g/Signal.ods", help="Input signal survey ODS")
    parser.add_argument("--results-dir", default="results/real_5g", help="Real 5G results directory")
    parser.add_argument(
        "--output",
        default="results/real_5g/run_signal_latency_summary.csv",
        help="Output CSV path",
    )
    args = parser.parse_args()

    build_summary(Path(args.input), Path(args.results_dir), Path(args.output))
    print(args.output)


if __name__ == "__main__":
    main()
