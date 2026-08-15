#!/usr/bin/env python3
"""Build the Section 5 RTT comparison figure from stored result artifacts."""

from __future__ import annotations

import csv
import glob
from dataclasses import dataclass
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import numpy as np


ROOT = Path(__file__).resolve().parents[1]
FIG_DIR = ROOT / "paper" / "figs"
CSV_OUT = FIG_DIR / "fig-rtt-comparison.csv"
PDF_OUT = FIG_DIR / "fig-rtt-comparison.pdf"
PNG_OUT = FIG_DIR / "fig-rtt-comparison.png"


TOKENS = {
    "surface": "#FCFCFD",
    "panel": "#FFFFFF",
    "ink": "#1F2430",
    "muted": "#6F768A",
    "grid": "#E6E8F0",
    "axis": "#D7DBE7",
}

COLORS = {
    "V2X": {"fill": "#A3BEFA", "edge": "#2E4780", "marker": "o"},
    "5G MQTT": {"fill": "#F0986E", "edge": "#804126", "marker": "s"},
    "5G TCP": {"fill": "#A3D576", "edge": "#386411", "marker": "D"},
    "5G UDP": {"fill": "#FFE15B", "edge": "#736422", "marker": "^"},
}


@dataclass(frozen=True)
class SeriesSpec:
    label: str
    family: str
    paths: tuple[str, ...]
    success_column: str
    success_value: str
    rtt_column: str = "rtt_ms"


SPECS = (
    SeriesSpec(
        "V2X fixed, 0 B",
        "V2X",
        ("results/mocar_v2x/20260703_exp_01_payload_sweep_0_2kb_final/remote_obu/obu/payload_0.csv",),
        "success",
        "true",
    ),
    SeriesSpec(
        "V2X fixed, 2 KiB",
        "V2X",
        ("results/mocar_v2x/20260703_exp_01_payload_sweep_0_2kb_final/remote_obu/obu/payload_2048.csv",),
        "success",
        "true",
    ),
    SeriesSpec(
        "V2X weak, 1 KiB",
        "V2X",
        ("results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_155120/remote_obu/obu/payload_1024.csv",),
        "success",
        "true",
    ),
    SeriesSpec(
        "V2X mobile, 256 B",
        "V2X",
        ("results/mocar_v2x/20260704_exp_02_radio_distance_mobility_175820/remote_obu/obu/mocar-exp02-mobility-route-los-moving-run2-payload-256.csv",),
        "success",
        "true",
    ),
    SeriesSpec(
        "V2X mobile, 256 B, 500 ms timeout",
        "V2X",
        ("results/mocar_v2x/20260704_exp_02_radio_distance_mobility_182104/remote_obu/obu/mocar-exp02-mobility-route-los-moving-run3-payload-256.csv",),
        "success",
        "true",
    ),
    SeriesSpec(
        "5G MQTT idle, 1 KiB",
        "5G MQTT",
        ("results/real_5g/20260701_load_qos_run_1/p5g-mqtt-loadqos-payload-1024-idle_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G TCP idle, 1 KiB",
        "5G TCP",
        ("results/real_5g/20260701_load_qos_run_1/p5g-tcp-loadqos-payload-1024-idle_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G MQTT detector, 60 KiB",
        "5G MQTT",
        ("results/real_5g/20260702_detector_output_to_ipi_run_1/p5g-mqtt-detector-output-payload-60000_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G TCP detector, 60 KiB",
        "5G TCP",
        ("results/real_5g/20260702_detector_output_to_ipi_run_1/p5g-tcp-detector-output-payload-60000_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G MQTT bulk, 2 MiB",
        "5G MQTT",
        ("results/real_5g/20260513_sunny_fintechparking_run_1/p5g-mqtt-latency-payload-2097152_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G TCP bulk, 2 MiB",
        "5G TCP",
        ("results/real_5g/20260513_sunny_fintechparking_run_1/p5g-tcp-service-payload-2097152_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G MQTT load, 1 KiB",
        "5G MQTT",
        ("results/real_5g/20260701_load_qos_run_1/p5g-mqtt-loadqos-payload-1024-uplink-25mbps_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G TCP load, 1 KiB",
        "5G TCP",
        ("results/real_5g/20260701_load_qos_run_1/p5g-tcp-loadqos-payload-1024-uplink-25mbps_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G MQTT weak signal, 1 KiB",
        "5G MQTT",
        ("results/real_5g/20260701_load_qos_weak_signal_run_1/p5g-mqtt-weak-signal-default-payload-1024-idle_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G TCP weak signal, 1 KiB",
        "5G TCP",
        ("results/real_5g/20260701_load_qos_weak_signal_run_1/p5g-tcp-weak-signal-default-payload-1024-idle_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G MQTT 100 clients, 1 KiB",
        "5G MQTT",
        ("results/real_5g/20260706_multiclient_scalability_weak_signal_run_1/p5g-scale-mqtt-payload-1024-clients-100_veh-*_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G TCP 100 clients, 1 KiB",
        "5G TCP",
        ("results/real_5g/20260706_multiclient_scalability_weak_signal_run_1/p5g-scale-tcp-payload-1024-clients-100_veh-*_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G UDP 100 clients, 1 KiB",
        "5G UDP",
        ("results/real_5g/20260706_multiclient_scalability_weak_signal_run_1/p5g-scale-udp-payload-1024-clients-100_veh-*_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G MQTT broker restart, 1 KiB",
        "5G MQTT",
        ("results/real_5g/20260703_failure_fallback_run_1/p5g-failure-broker-restart-mqtt-payload-1024_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G TCP receiver restart, 1 KiB",
        "5G TCP",
        ("results/real_5g/20260703_failure_fallback_run_1/p5g-failure-receiver-restart-tcp-payload-1024_sender.csv",),
        "accepted",
        "true",
    ),
    SeriesSpec(
        "5G UDP receiver restart, 1 KiB",
        "5G UDP",
        ("results/real_5g/20260703_failure_fallback_run_1/p5g-failure-receiver-restart-udp-payload-1024_sender.csv",),
        "accepted",
        "true",
    ),
)


def expand_paths(patterns: tuple[str, ...]) -> list[Path]:
    paths: list[Path] = []
    for pattern in patterns:
        matches = sorted(ROOT.glob(pattern)) if "*" in pattern else [ROOT / pattern]
        paths.extend(matches)
    missing = [str(path) for path in paths if not path.exists()]
    if missing:
        raise FileNotFoundError(f"Missing input files: {missing}")
    if not paths:
        raise FileNotFoundError(f"No files matched: {patterns}")
    return paths


def read_series(spec: SeriesSpec) -> dict[str, object]:
    attempts = 0
    responses = 0
    rtts: list[float] = []
    paths = expand_paths(spec.paths)

    for path in paths:
        with path.open(newline="") as fh:
            reader = csv.DictReader(fh)
            for row in reader:
                attempts += 1
                if row.get(spec.success_column, "").strip().lower() == spec.success_value:
                    try:
                        rtt = float(row.get(spec.rtt_column, ""))
                    except ValueError:
                        continue
                    if rtt > 0:
                        responses += 1
                        rtts.append(rtt)

    if not rtts:
        raise ValueError(f"No successful RTT samples for {spec.label}")

    values = np.array(rtts, dtype=float)
    p5, p50, p95, p99 = np.percentile(values, [5, 50, 95, 99])
    return {
        "label": spec.label,
        "family": spec.family,
        "attempts": attempts,
        "responses": responses,
        "response_rate": responses / attempts if attempts else 0.0,
        "p5_rtt_ms": p5,
        "p50_rtt_ms": p50,
        "p95_rtt_ms": p95,
        "p99_rtt_ms": p99,
        "source_files": ";".join(str(path.relative_to(ROOT)) for path in paths),
    }


def write_csv(rows: list[dict[str, object]]) -> None:
    fieldnames = [
        "label",
        "family",
        "attempts",
        "responses",
        "response_rate",
        "p5_rtt_ms",
        "p50_rtt_ms",
        "p95_rtt_ms",
        "p99_rtt_ms",
        "source_files",
    ]
    with CSV_OUT.open("w", newline="") as fh:
        writer = csv.DictWriter(fh, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def format_ms(value: float) -> str:
    if value >= 1000:
        return f"{value / 1000:.1f} s"
    if value >= 100:
        return f"{value:.0f} ms"
    return f"{value:.1f} ms"


def plot(rows: list[dict[str, object]]) -> None:
    plt.rcParams.update(
        {
            "font.family": "sans-serif",
            "font.sans-serif": ["DejaVu Sans", "Arial", "sans-serif"],
            "font.size": 8.5,
            "axes.facecolor": TOKENS["panel"],
            "figure.facecolor": TOKENS["surface"],
            "axes.edgecolor": TOKENS["axis"],
            "axes.labelcolor": TOKENS["ink"],
            "xtick.color": TOKENS["muted"],
            "ytick.color": TOKENS["ink"],
            "savefig.facecolor": "white",
        }
    )

    plot_rows = list(reversed(rows))
    fig, ax = plt.subplots(figsize=(7.15, 5.7))
    y_positions = np.arange(len(plot_rows))

    ax.set_xscale("log")
    ax.set_xlim(10, 9000)
    ax.set_ylim(-0.8, len(plot_rows) - 0.2)
    ax.grid(True, axis="x", which="major", color=TOKENS["grid"], linewidth=0.8)
    ax.grid(True, axis="x", which="minor", color=TOKENS["grid"], linewidth=0.35, alpha=0.45)
    ax.grid(False, axis="y")

    for x, text in [(100, "100 ms"), (500, "500 ms"), (1000, "1 s")]:
        ax.axvline(x, color=TOKENS["muted"], linestyle=":", linewidth=0.9, zorder=0)
        ax.text(x, len(plot_rows) - 0.1, text, rotation=90, ha="right", va="top",
                fontsize=7.3, color=TOKENS["muted"])

    for i, row in enumerate(plot_rows):
        style = COLORS[str(row["family"])]
        p5 = float(row["p5_rtt_ms"])
        p50 = float(row["p50_rtt_ms"])
        p95 = float(row["p95_rtt_ms"])
        rate = float(row["response_rate"]) * 100.0

        ax.hlines(i, p5, p95, color=style["edge"], linewidth=1.4, alpha=0.9)
        ax.scatter(
            [p50],
            [i],
            s=28,
            marker=style["marker"],
            facecolors=style["fill"],
            edgecolors=style["edge"],
            linewidths=0.8,
            zorder=3,
        )
        label_x = min(p95 * 1.12, 8200)
        ax.text(label_x, i, f"{rate:.0f}%", va="center", ha="left",
                fontsize=7.2, color=TOKENS["muted"])

    ax.set_yticks(y_positions)
    ax.set_yticklabels([str(row["label"]) for row in plot_rows])
    ax.set_xlabel("Sender-side RTT in milliseconds, log scale")
    ax.xaxis.set_major_formatter(mticker.FuncFormatter(lambda value, _: format_ms(value)))
    ax.tick_params(axis="both", length=0)
    for spine in ["top", "right"]:
        ax.spines[spine].set_visible(False)
    ax.spines["left"].set_color(TOKENS["axis"])
    ax.spines["bottom"].set_color(TOKENS["axis"])

    handles = []
    labels = []
    for family, style in COLORS.items():
        handle = ax.scatter([], [], s=30, marker=style["marker"],
                            facecolors=style["fill"], edgecolors=style["edge"],
                            linewidths=0.8)
        handles.append(handle)
        labels.append(family)
    ax.legend(handles, labels, loc="lower left", bbox_to_anchor=(0, 1.01),
              frameon=False, ncol=4, columnspacing=1.3, handletextpad=0.35,
              borderaxespad=0)

    fig.text(
        0.135,
        0.982,
        "RTT comparison across measured V2X and 5G conditions",
        ha="left",
        va="top",
        fontsize=10.5,
        fontweight="semibold",
        color=TOKENS["ink"],
    )
    fig.text(
        0.135,
        0.947,
        "Line spans p5 to p95 successful-response RTT; marker is p50; right labels show response rate.",
        ha="left",
        va="top",
        fontsize=8.0,
        color=TOKENS["muted"],
    )
    fig.subplots_adjust(left=0.36, right=0.965, top=0.88, bottom=0.085)

    FIG_DIR.mkdir(parents=True, exist_ok=True)
    fig.savefig(PDF_OUT)
    fig.savefig(PNG_OUT, dpi=300)
    plt.close(fig)


def main() -> None:
    rows = [read_series(spec) for spec in SPECS]
    write_csv(rows)
    plot(rows)
    print(f"Wrote {CSV_OUT.relative_to(ROOT)}")
    print(f"Wrote {PDF_OUT.relative_to(ROOT)}")
    print(f"Wrote {PNG_OUT.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
