#!/usr/bin/env python3
"""Build radio-conditioned latency relationship artifacts for the paper."""

import argparse
import csv
import json
import math
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402


FEATURES = [
    ("rsrp_dbm", "RSRP", "dBm"),
    ("rsrq_db", "RSRQ", "dB"),
    ("snr_db_reported", "SNR", "dB"),
]

LATENCY_FIELDS = [
    "mqtt_1k_p50_ms",
    "mqtt_1k_p95_ms",
    "tcp_1k_p50_ms",
    "tcp_1k_p95_ms",
]


def parse_float(row, key):
    value = row.get(key, "")
    if value == "":
        return None
    return float(value)


def load_rows(path):
    with path.open(newline="") as handle:
        rows = []
        for row in csv.DictReader(handle):
            parsed = {"run": row["run"]}
            for key, _, _ in FEATURES:
                parsed[key] = parse_float(row, key)
            for key in LATENCY_FIELDS:
                parsed[key] = parse_float(row, key)
            rows.append(parsed)
    return rows


def feature_ranges(rows):
    ranges = {}
    for key, label, unit in FEATURES:
        values = [row[key] for row in rows if row[key] is not None]
        ranges[key] = {
            "label": label,
            "unit": unit,
            "min": min(values),
            "max": max(values),
        }
    return ranges


def normalized_feature(value, low, high):
    if math.isclose(low, high):
        return 0.5
    return (value - low) / (high - low)


def normalized_vector(values, ranges):
    vector = []
    for key, _, _ in FEATURES:
        span = ranges[key]
        vector.append(normalized_feature(values[key], span["min"], span["max"]))
    return vector


def radio_quality_index(row, ranges):
    return sum(normalized_vector(row, ranges)) / len(FEATURES)


def euclidean(a, b):
    return math.sqrt(sum((left - right) ** 2 for left, right in zip(a, b)))


def estimate_latency(rows, ranges, radio_tuple, power=2.0, epsilon=1e-6):
    query = {
        "rsrp_dbm": radio_tuple[0],
        "rsrq_db": radio_tuple[1],
        "snr_db_reported": radio_tuple[2],
    }
    query_vector = normalized_vector(query, ranges)
    weighted = {key: 0.0 for key in LATENCY_FIELDS}
    total_weight = 0.0

    for row in rows:
        row_vector = normalized_vector(row, ranges)
        distance = euclidean(query_vector, row_vector)
        weight = 1.0 / ((distance + epsilon) ** power)
        total_weight += weight
        for key in LATENCY_FIELDS:
            weighted[key] += weight * row[key]

    return {key: weighted[key] / total_weight for key in LATENCY_FIELDS}


def write_relationship_csv(rows, ranges, path):
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "run",
        "radio_quality_index",
        "rsrp_dbm",
        "rsrq_db",
        "snr_db_reported",
        *LATENCY_FIELDS,
    ]
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for row in sorted(rows, key=lambda item: radio_quality_index(item, ranges)):
            writer.writerow(
                {
                    "run": row["run"],
                    "radio_quality_index": f"{radio_quality_index(row, ranges):.3f}",
                    "rsrp_dbm": f'{row["rsrp_dbm"]:.1f}',
                    "rsrq_db": f'{row["rsrq_db"]:.1f}',
                    "snr_db_reported": f'{row["snr_db_reported"]:.1f}',
                    "mqtt_1k_p50_ms": f'{row["mqtt_1k_p50_ms"]:.1f}',
                    "mqtt_1k_p95_ms": f'{row["mqtt_1k_p95_ms"]:.1f}',
                    "tcp_1k_p50_ms": f'{row["tcp_1k_p50_ms"]:.1f}',
                    "tcp_1k_p95_ms": f'{row["tcp_1k_p95_ms"]:.1f}',
                }
            )


def write_model_json(rows, ranges, path):
    model = {
        "features": [
            {
                "name": key,
                "label": label,
                "unit": unit,
                "higher_is_better": True,
                "normalization_min": ranges[key]["min"],
                "normalization_max": ranges[key]["max"],
            }
            for key, label, unit in FEATURES
        ],
        "method": "inverse-distance weighted interpolation over normalized RSRP, RSRQ, and SNR",
        "distance": "Euclidean distance in min-max normalized radio-feature space",
        "weight": "1 / (distance + 1e-6)^2",
        "scope": "Interpolation within the measured stationary private-5G radio envelope; not a validated causal radio model.",
        "latency_fields": LATENCY_FIELDS,
        "training_samples": [
            {
                "run": row["run"],
                "radio_quality_index": round(radio_quality_index(row, ranges), 3),
                "rsrp_dbm": row["rsrp_dbm"],
                "rsrq_db": row["rsrq_db"],
                "snr_db_reported": row["snr_db_reported"],
                **{key: row[key] for key in LATENCY_FIELDS},
            }
            for row in sorted(rows, key=lambda item: radio_quality_index(item, ranges))
        ],
    }
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(model, indent=2) + "\n", encoding="utf-8")


def plot_relationship(rows, ranges, path):
    ordered = sorted(rows, key=lambda item: radio_quality_index(item, ranges))
    x_values = [radio_quality_index(row, ranges) for row in ordered]
    labels = [row["run"].replace("Run ", "R") for row in ordered]

    plt.rcParams.update(
        {
            "font.family": "DejaVu Sans",
            "font.size": 8,
            "axes.labelsize": 8,
            "axes.titlesize": 9,
            "legend.fontsize": 7,
            "xtick.labelsize": 7,
            "ytick.labelsize": 7,
        }
    )
    fig, axes = plt.subplots(1, 2, figsize=(7.15, 2.7), sharex=True)
    panels = [
        ("p50 RTT", "mqtt_1k_p50_ms", "tcp_1k_p50_ms"),
        ("p95 RTT", "mqtt_1k_p95_ms", "tcp_1k_p95_ms"),
    ]
    styles = {
        "MQTT": {"color": "#1f77b4", "marker": "o"},
        "TCP": {"color": "#d55e00", "marker": "s"},
    }

    for ax, (title, mqtt_key, tcp_key) in zip(axes, panels):
        for transport, key in [("MQTT", mqtt_key), ("TCP", tcp_key)]:
            y_values = [row[key] for row in ordered]
            ax.plot(
                x_values,
                y_values,
                linewidth=1.6,
                markersize=4.5,
                label=transport,
                **styles[transport],
            )
        ax.set_title(title)
        ax.set_yscale("log")
        ax.grid(True, which="major", color="#d8dee4", linewidth=0.6)
        ax.grid(True, which="minor", color="#eef1f4", linewidth=0.4)
        ax.set_xlabel("Radio quality index")
        ax.set_ylabel("1 KiB RTT (ms)")
        ax.set_xlim(-0.05, 1.05)
        ax.set_xticks([0.0, 0.25, 0.5, 0.75, 1.0])
        ax.legend(loc="upper right", frameon=False)

    label_offsets = {
        "R3": (5, -13),
        "R2": (-18, -13),
        "R1": (4, 7),
        "R4": (-20, 7),
        "R5": (6, 7),
    }
    for x_value, label, row in zip(x_values, labels, ordered):
        y_value = row["mqtt_1k_p50_ms"]
        axes[0].annotate(
            label,
            xy=(x_value, y_value),
            xytext=label_offsets.get(label, (5, 7)),
            textcoords="offset points",
            fontsize=7,
        )

    fig.text(
        0.5,
        0.01,
        "Index is the mean of min-max normalized RSRP, RSRQ, and SNR over the signal-mapped runs; higher is better.",
        ha="center",
        va="bottom",
        fontsize=7,
    )
    fig.tight_layout(rect=(0, 0.08, 1, 1))
    path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(path, dpi=300)
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--input",
        default="results/real_5g/run_signal_latency_summary.csv",
        help="Run-level signal/latency summary CSV",
    )
    parser.add_argument(
        "--relationship-csv",
        default="results/real_5g/radio_latency_relationship.csv",
        help="Output CSV with normalized radio-quality relationship",
    )
    parser.add_argument(
        "--model-json",
        default="results/real_5g/radio_latency_estimator_model.json",
        help="Output JSON with estimator metadata",
    )
    parser.add_argument(
        "--figure",
        default="paper/figs/fig-radio-latency-relationship.png",
        help="Output paper figure",
    )
    parser.add_argument(
        "--estimate",
        nargs=3,
        type=float,
        metavar=("RSRP_DBM", "RSRQ_DB", "SNR_DB"),
        help="Optionally print estimated compact 1 KiB latency for a radio tuple",
    )
    args = parser.parse_args()

    rows = load_rows(Path(args.input))
    ranges = feature_ranges(rows)
    write_relationship_csv(rows, ranges, Path(args.relationship_csv))
    write_model_json(rows, ranges, Path(args.model_json))
    plot_relationship(rows, ranges, Path(args.figure))

    if args.estimate:
        estimates = estimate_latency(rows, ranges, args.estimate)
        print(json.dumps(estimates, indent=2))
    else:
        print(args.relationship_csv)
        print(args.model_json)
        print(args.figure)


if __name__ == "__main__":
    main()
