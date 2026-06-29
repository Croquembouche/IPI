#!/usr/bin/env python3
"""Build paper figures for the real private-5G latency runs."""

import csv
import math
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.lines import Line2D  # noqa: E402


RESULTS_DIR = Path("results/real_5g")
FIG_DIR = Path("paper/figs")

RUNS = {
    "Run 1": RESULTS_DIR / "20260513_sunny_fintechparking_run_1",
    "Run 2": RESULTS_DIR / "20260514_sunny_after_rain_run_1",
    "Run 3": RESULTS_DIR / "20260515_sunny_run_1",
    "Run 4": RESULTS_DIR / "20260522_cloudy_run_1",
    "Run 5": RESULTS_DIR / "20260521_small_rain_run_1",
}

RUN_COLORS = {
    "Run 1": "#0072B2",
    "Run 2": "#E69F00",
    "Run 3": "#009E73",
    "Run 4": "#CC79A7",
    "Run 5": "#D55E00",
}

COLORS = {
    "mqtt": "#009e73",
    "tcp": "#0072b2",
}

TRANSPORT_STYLES = {
    "mqtt": "-",
    "tcp": "--",
}

MARKERS = {
    "mqtt": "o",
    "tcp": "s",
}

RUN_CONTEXT = {
    "Run 1": "RSRP -106.0 dBm, RSRQ -10.1 dB, SNR 14.5 dB",
    "Run 2": "RSRP -107.4 dBm, RSRQ -10.1 dB, SNR 14.2 dB",
    "Run 3": "RSRP -120.0 dBm, RSRQ -13.0 dB, SNR 3.5 dB",
    "Run 4": "signal-mapped: RSRP -102.6 dBm, RSRQ -10.0 dB, SNR 19.7 dB",
    "Run 5": "signal-mapped: RSRP -108.7 dBm, RSRQ -11.6 dB, SNR 12.7 dB",
}


def percentile(values, pct):
    values = sorted(value for value in values if value is not None and value >= 0)
    if not values:
        return None
    index = max(0, min(len(values) - 1, math.ceil((pct / 100.0) * len(values)) - 1))
    return values[index]


def load_rtt(path):
    values = []
    with path.open(newline="") as handle:
        for row in csv.DictReader(handle):
            if str(row.get("accepted", "")).lower() != "true":
                continue
            try:
                values.append(float(row["rtt_ms"]))
            except (KeyError, ValueError):
                pass
    return values


def sender_path(run_dir, transport, payload):
    candidates = [
        run_dir / f"p5g-{transport}-service-payload-{payload}_sender.csv",
        run_dir / f"p5g-{transport}-latency-payload-{payload}_sender.csv",
    ]
    for path in candidates:
        if path.exists():
            return path
    return None


def metric(run_dir, transport, payload, pct):
    path = sender_path(run_dir, transport, payload)
    if path is None:
        return None
    return percentile(load_rtt(path), pct)


def condition_metrics(run_dir, transport, payload):
    path = sender_path(run_dir, transport, payload)
    if path is None:
        return None
    values = load_rtt(path)
    if not values:
        return None
    return {
        "p50": percentile(values, 50),
        "p95": percentile(values, 95),
        "accepted": len(values),
    }


def payload_label(payload):
    if payload == 0:
        return "0 B"
    if payload < 1024:
        return f"{payload} B"
    if payload < 1024 * 1024:
        return f"{payload // 1024} KiB"
    return f"{payload // (1024 * 1024)} MiB"


def large_payload_sender_path(run_dir, transport, payload):
    if transport == "mqtt":
        candidates = [
            run_dir / f"p5g-{transport}-latency-payload-{payload}_sender.csv",
            run_dir / f"p5g-{transport}-service-payload-{payload}_sender.csv",
        ]
    else:
        candidates = [
            run_dir / f"p5g-{transport}-service-payload-{payload}_sender.csv",
            run_dir / f"p5g-{transport}-latency-payload-{payload}_sender.csv",
        ]
    for path in candidates:
        if path.exists():
            return path
    raise FileNotFoundError(f"No sender CSV for {run_dir}, {transport}, {payload}")


def large_payload_metric(run_dir, transport, payload, pct):
    return percentile(load_rtt(large_payload_sender_path(run_dir, transport, payload)), pct)


def annotate_points(ax, x_values, y_values, color, offsets=None):
    offsets = offsets or {}
    for idx, (x_value, y_value) in enumerate(zip(x_values, y_values)):
        dx, dy = offsets.get(idx, (0, 10))
        ax.annotate(
            f"{y_value:.0f}",
            xy=(x_value, y_value),
            xytext=(dx, dy),
            textcoords="offset points",
            ha="center",
            fontsize=8,
            color=color,
            bbox={"boxstyle": "round,pad=0.2", "fc": "white", "ec": "none", "alpha": 0.85},
        )


def configure_latency_axis(ax):
    ax.set_yscale("log")
    ax.grid(True, which="major", axis="y", color="#c7d0d8", linewidth=0.7)
    ax.grid(True, which="minor", axis="y", color="#edf1f4", linewidth=0.4)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)


def build_clean_run_scaling():
    payloads = [1024, 262144, 1048576, 2097152]
    x_values = list(range(len(payloads)))
    fig, ax = plt.subplots(figsize=(9.4, 4.75))

    deadline_lines = [
        (100, "100 ms"),
        (500, "500 ms"),
        (2000, "2 s"),
    ]
    for y_value, label in deadline_lines:
        ax.axhline(y_value, color="#9aa8b2", linewidth=0.9, linestyle="--", zorder=0)
        ax.text(
            len(payloads) - 0.52,
            y_value,
            label,
            ha="right",
            va="bottom",
            fontsize=8,
            color="#4f5f67",
        )

    run_jitter = {
        "Run 1": -0.055,
        "Run 2": -0.025,
        "Run 3": 0.0,
        "Run 4": 0.025,
        "Run 5": 0.055,
    }
    transport_offset = {"mqtt": -0.16, "tcp": 0.16}
    all_values = []
    grouped_p50 = {(transport, idx): [] for transport in ["mqtt", "tcp"] for idx in x_values}

    for run_label, run_dir in RUNS.items():
        for transport in ["mqtt", "tcp"]:
            for idx, payload in enumerate(payloads):
                values = condition_metrics(run_dir, transport, payload)
                if values is None:
                    continue
                p50 = values["p50"]
                p95 = values["p95"]
                all_values.extend([p50, p95])
                grouped_p50[(transport, idx)].append(p50)
                xpos = idx + transport_offset[transport] + run_jitter[run_label]
                ax.vlines(
                    xpos,
                    p50,
                    p95,
                    color=COLORS[transport],
                    alpha=0.45,
                    linewidth=1.1,
                    zorder=2,
                )
                filled = values["accepted"] >= 1000
                ax.scatter(
                    xpos,
                    p50,
                    s=38,
                    marker=MARKERS[transport],
                    facecolors=COLORS[transport] if filled else "white",
                    edgecolors="white" if filled else COLORS[transport],
                    linewidths=0.8 if filled else 1.4,
                    zorder=3,
                )

    for transport in ["mqtt", "tcp"]:
        for idx in x_values:
            values = grouped_p50[(transport, idx)]
            if not values:
                continue
            center = idx + transport_offset[transport]
            median_value = percentile(values, 50)
            ax.hlines(
                median_value,
                center - 0.08,
                center + 0.08,
                color="#263238",
                linewidth=1.8,
                zorder=4,
            )

    ax.annotate(
        "weak-radio Run 3",
        xy=(1 + transport_offset["mqtt"] + run_jitter["Run 3"], metric(RUNS["Run 3"], "mqtt", 262144, 50)),
        xytext=(0.72, 28000),
        arrowprops={"arrowstyle": "->", "color": "#5f6b73", "linewidth": 0.9},
        fontsize=8.5,
        color="#263238",
        bbox={"boxstyle": "round,pad=0.25", "fc": "white", "ec": "#aab7c2", "alpha": 0.94},
    )

    configure_latency_axis(ax)
    ax.set_ylim(20, max(60000, max(all_values) * 1.12))
    ax.set_xticks(x_values)
    ax.set_xticklabels([payload_label(payload) for payload in payloads])
    ax.set_xlabel("IPI payload size")
    ax.set_ylabel("RTT (ms, log scale)")
    ax.set_title("Private-5G application RTT envelope by payload size", fontsize=12, fontweight="bold")

    handles = [
        Line2D(
            [0],
            [0],
            color=COLORS[transport],
            linestyle="",
            marker=MARKERS[transport],
            markerfacecolor=COLORS[transport],
            markeredgecolor="white",
            markersize=6,
            label=f"{transport.upper()} path p50",
        )
        for transport in ["mqtt", "tcp"]
    ]
    handles.extend(
        [
            Line2D([0], [0], color="#6f7f89", linewidth=1.2, label="p50-p95 range"),
            Line2D(
                [0],
                [0],
                color="#263238",
                linewidth=1.8,
                label="median p50 across runs",
            ),
            Line2D(
                [0],
                [0],
                color=COLORS["mqtt"],
                linestyle="",
                marker=MARKERS["mqtt"],
                markerfacecolor="white",
                markeredgecolor=COLORS["mqtt"],
                markersize=6,
                label="partial condition",
            ),
        ]
    )
    ax.legend(
        handles=handles,
        loc="upper left",
        ncol=5,
        frameon=False,
        fontsize=8.5,
        bbox_to_anchor=(0.0, 1.02),
    )

    fig.subplots_adjust(left=0.08, right=0.985, top=0.88, bottom=0.14)
    FIG_DIR.mkdir(parents=True, exist_ok=True)
    fig.savefig(FIG_DIR / "fig-clean-run-scaling.png", dpi=300)
    plt.close(fig)


def main():
    plt.rcParams.update(
        {
            "font.family": "DejaVu Sans",
            "font.size": 10,
            "axes.labelsize": 11,
            "xtick.labelsize": 10,
            "ytick.labelsize": 10,
            "legend.fontsize": 10,
        }
    )
    build_clean_run_scaling()
    print(FIG_DIR / "fig-clean-run-scaling.png")


if __name__ == "__main__":
    main()
