#!/usr/bin/env python3
"""Build the comparison figures used by Section 5.

The constants below are transcribed from the repository result summaries named
in experiment_summary.md or computed from the retained sender CSVs with the
Section 4 issued-request deadline rule.  Keeping the inputs explicit makes the
paper build independent of private endpoint metadata and large raw collections.
"""

import csv
from pathlib import Path

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "figures"
OUT.mkdir(parents=True, exist_ok=True)

mpl.rcParams.update(
    {
        "font.family": "sans-serif",
        "font.size": 8.2,
        "axes.labelsize": 8.2,
        "axes.titlesize": 8.8,
        "legend.fontsize": 7.2,
        "xtick.labelsize": 7.2,
        "ytick.labelsize": 7.2,
        "pdf.fonttype": 42,
        "ps.fonttype": 42,
        "axes.spines.top": False,
        "axes.spines.right": False,
    }
)

PC5 = "#3377b4"
FIVEG = "#d55e00"
MQTT = "#0072b2"
TCP = "#d55e00"
UDP = "#009e73"
GRAY = "#666666"
TDD_BASELINE = "#2166ac"
TDD_ALTERNATIVE = "#b2182b"

REPO_ROOT = ROOT.parents[1]

TDD_AUG17 = (
    REPO_ROOT
    / "results/real_5g/20260817_airspan_tdd_profile_comparison_location_3"
    / "analysis"
)
TDD_AUG18 = (
    REPO_ROOT
    / "results/real_5g/20260818_airspan_tdd_40_40_20_location_3_directional_repeat"
    / "analysis"
)
TDD_AUG19_70 = (
    REPO_ROOT
    / "results/real_5g/20260819_airspan_tdd_70_20_10_new_device_location_3_directional_repeat"
    / "analysis"
)
TDD_AUG19_40 = (
    REPO_ROOT
    / "results/real_5g/20260819_airspan_tdd_40_40_20_new_device_location_3_directional_repeat"
    / "analysis"
)
DETECTOR_106_STREAM = (
    REPO_ROOT / "results/real_5g/20260702_detector_output_to_ipi_run_1"
)
DETECTOR_106_RAW_UDP = (
    REPO_ROOT / "results/real_5g/20260702_detector_output_to_ipi_udp_run_2"
)
DETECTOR_106_FRAGMENTED = (
    REPO_ROOT / "results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1"
)
DETECTOR_120_STREAM = (
    REPO_ROOT
    / "results/real_5g/20260706_detector_output_to_ipi_weak_signal_tcp_mqtt_run_1"
)
DETECTOR_120_FRAGMENTED = (
    REPO_ROOT
    / "results/real_5g/20260706_detector_output_to_ipi_weak_signal_udp_fragmented_run_1"
)


def save(fig: plt.Figure, name: str) -> None:
    fig.savefig(OUT / name, bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)


def read_csv_rows(path: Path) -> list[dict[str, str]]:
    with path.open(newline="") as handle:
        return list(csv.DictReader(handle))


def pc5_payload_and_route() -> None:
    payloads = [0, 256, 512, 1024, 2048]
    fields = [
        "P1 NLOS\n68 m",
        "P2\n113 m",
        "P3\n212 m",
        "P4\n377 m",
        "P5 no path\n469 m",
    ]
    availability = np.array(
        [
            [100.0, 99.7, 95.5, 0.0, 0.0],
            [99.9, 100.0, 100.0, 100.0, 99.8],
            [100.0, 100.0, 99.8, 97.9, 0.8],
            [99.9, 99.1, 96.9, 17.1, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0],
        ]
    ).T
    fig, axes = plt.subplots(
        1, 3, figsize=(7.15, 2.30), gridspec_kw={"width_ratios": [1.58, 1.0, 0.94]}
    )

    ax = axes[0]
    x = np.arange(len(payloads))
    point_colors = ["#a50f15", "#2166ac", "#4292c6", "#74a9cf", "#636363"]
    point_markers = ["o", "s", "^", "D", "X"]
    for index, (field, color, marker) in enumerate(
        zip(fields, point_colors, point_markers)
    ):
        ax.plot(
            x,
            availability[:, index],
            marker=marker,
            ms=3.7,
            lw=1.0,
            color=color,
            label=field.replace("\n", " "),
        )
    ax.axhline(90, color=GRAY, lw=0.7, ls="--")
    ax.text(4.05, 92, "90% reference", color=GRAY, fontsize=5.8, ha="right")
    ax.set_title("(a) Stationary response completion")
    payload_labels_kib = ["0", "0.25", "0.5", "1", "2"]
    ax.set_xticks(x, payload_labels_kib)
    ax.set_xlabel("Requested payload (KiB)")
    ax.set_ylabel("Issued requests completed (%)")
    ax.set_ylim(-4, 106)
    ax.grid(axis="y", color="#dddddd", lw=0.5)
    ax.legend(
        frameon=False,
        ncol=2,
        loc="lower left",
        fontsize=5.5,
        handlelength=1.5,
        columnspacing=0.6,
        labelspacing=0.25,
    )

    ax = axes[1]
    ref_p50 = [99.939, 99.980, 99.972, 99.966, 99.958]
    ref_p95 = [106.873, 106.917, 106.694, 106.878, 111.663]
    ref_p99 = [111.814, 111.691, 114.577, 112.515, 120.567]
    x = np.arange(len(payloads))
    ax.plot(x, ref_p50, "o-", color=PC5, label="p50")
    ax.plot(x, ref_p95, "s--", color="#6baed6", label="p95")
    ax.plot(x, ref_p99, "^:", color="#08306b", label="p99")
    for y, label in [(10, "10 ms"), (25, "25 ms"), (100, "100 ms")]:
        ax.axhline(y, color=GRAY, lw=0.7, ls="--")
        ax.text(4.05, y + 1.8, label, fontsize=6.4, color=GRAY, ha="right")
    ax.set_xticks(x, payload_labels_kib)
    ax.tick_params(axis="x", labelsize=6.2)
    ax.set_ylim(0, 130)
    ax.set_title("(b) 7-m reference RTT")
    ax.set_xlabel("Requested payload (KiB)")
    ax.set_ylabel("Response RTT (ms)")
    ax.legend(
        frameon=False,
        ncol=1,
        loc="center left",
        bbox_to_anchor=(0.02, 0.38),
        borderaxespad=0,
        handlelength=1.8,
        handletextpad=0.4,
    )
    ax.grid(axis="y", color="#dddddd", lw=0.5)

    ax = axes[2]
    route = np.arange(1, 5)
    route_avail = [76.3, 73.1, 59.1, 71.6]
    route_p95 = [108.421, 41.887, 43.923, 41.621]
    bars = ax.bar(route, route_avail, color=PC5, alpha=0.83, width=0.62, label="completion")
    ax.axhline(90, color=GRAY, lw=0.8, ls="--")
    ax.text(4.38, 91.5, "90% reference", color=GRAY, fontsize=6.3, ha="right")
    ax.set_ylim(0, 120)
    ax.set_xticks(route, [f"R{i}" for i in route])
    ax.set_ylabel("Completed (%)")
    ax.set_xlabel("Route collection")
    ax.set_title("(c) Route coverage and RTT")
    ax.grid(axis="y", color="#dddddd", lw=0.5)
    ax2 = ax.twinx()
    ax2.spines["right"].set_visible(True)
    ax2.plot(route, route_p95, "D-", color="#8c2d04", ms=4, lw=1.0, label="reply p95")
    ax2.set_ylim(0, 125)
    ax2.set_ylabel("Reply p95 RTT (ms)")
    for bar, value in zip(bars, route_avail):
        ax.text(bar.get_x() + bar.get_width() / 2, value + 2.0, f"{value:.1f}", ha="center", fontsize=6.5)

    fig.subplots_adjust(wspace=0.50, bottom=0.22)
    save(fig, "section5_pc5_payload_route.pdf")


def fiveg_deadline_envelope() -> None:
    groups = [
        "SPaT state updates",
        "Idle payloads <=4 KiB",
        "1 KiB with load/QoS",
        "Detector output, 4--58.6 KiB",
        "1 KiB, 1--100 clients",
        "8--64 KiB sweep",
        "128--512 KiB sweep",
        "1--2 MiB sweep",
    ]
    values = np.array(
        [
            [53.40, 100.00, 100.00, 100.00],
            [37.76, 96.29, 99.45, 100.00],
            [24.12, 93.47, 99.04, 100.00],
            [42.86, 99.32, 99.54, 99.54],
            [50.92, 99.51, 99.73, 99.79],
            [70.37, 99.66, 99.78, 99.78],
            [0.99, 88.36, 99.27, 100.00],
            [0.00, 0.00, 37.39, 100.00],
        ]
    )
    p50 = [58.748, 119.739, 139.163, 108.039, 97.832, 83.265, 227.193, 1146.068]
    p95 = [130.323, 413.725, 602.354, 245.879, 233.349, 199.793, 667.975, 3122.273]
    p99 = [140.479, 791.676, 990.812, 349.732, 298.077, 303.647, 946.023, 3579.058]

    fig, axes = plt.subplots(1, 2, figsize=(7.15, 2.65), gridspec_kw={"width_ratios": [1.38, 1.0]})
    ax = axes[0]
    im = ax.imshow(values, vmin=0, vmax=100, cmap="Oranges", aspect="auto")
    ax.set_xticks(range(4), ["100 ms", "500 ms", "1,000 ms", "Harness\ntimeout"])
    ax.set_yticks(range(len(groups)), groups)
    ax.set_title("(a) Requests completed by deadline\n(% of all issued requests)")
    for i in range(values.shape[0]):
        for j in range(values.shape[1]):
            v = values[i, j]
            ax.text(j, i, f"{v:.2f}%", ha="center", va="center", fontsize=6.2,
                    color="white" if v >= 67 else "black")
    cbar = fig.colorbar(im, ax=ax, fraction=0.046, pad=0.02)

    ax = axes[1]
    y = np.arange(len(groups))
    ax.scatter(p50, y, marker="o", color="#fdae6b", label="p50", zorder=3)
    ax.scatter(p95, y, marker="s", color=FIVEG, label="p95", zorder=3)
    ax.scatter(p99, y, marker="^", color="#7f2704", label="p99", zorder=3)
    for threshold in [100, 500, 1000]:
        ax.axvline(threshold, color=GRAY, ls="--", lw=0.7)
    ax.set_xscale("log")
    ax.set_xlim(40, 5000)
    ax.set_xticks([100, 500, 1000], ["100", "500", "1,000"])
    ax.tick_params(axis="x", labelsize=6.4)
    ax.set_yticks(y, ["" for _ in groups])
    ax.invert_yaxis()
    ax.set_xlabel("Completed-response RTT (ms, log scale)")
    ax.set_title("(b) Completed-response RTT percentiles")
    ax.grid(axis="x", which="both", color="#dddddd", lw=0.5)
    ax.legend(
        frameon=True,
        facecolor="white",
        edgecolor="none",
        framealpha=0.92,
        ncol=1,
        loc="upper right",
    )
    fig.subplots_adjust(wspace=0.23)
    save(fig, "section5_5g_deadline_envelope.pdf")


def completion_from_files(directory: Path, transport: str) -> dict[int, float]:
    values: dict[int, float] = {}
    for path in sorted(directory.glob(f"p5g-{transport}-detector-output-payload-*_sender.csv")):
        payload = int(path.stem.split("payload-")[1].split("_")[0])
        rows = read_csv_rows(path)
        completed = sum((row.get("accepted") or "").lower() == "true" for row in rows)
        values[payload] = 100.0 * completed / len(rows)
    return values


def protocol_completion() -> None:
    payloads = [0, 256, 1024, 1400, 4096, 19648, 22816, 23968, 25024, 60000]
    payload_labels = ["0", "0.25", "1", "1.37", "4",
                      "19.2", "22.3", "23.4", "24.4", "58.6"]
    transports = ["TCP", "MQTT", "raw UDP", "fragmented UDP"]

    conditions = [
        (
            "(a) Reported RSRP: -106 dBm",
            {
                "TCP": completion_from_files(DETECTOR_106_STREAM, "tcp"),
                "MQTT": completion_from_files(DETECTOR_106_STREAM, "mqtt"),
                "raw UDP": completion_from_files(DETECTOR_106_RAW_UDP, "udp"),
                "fragmented UDP": completion_from_files(DETECTOR_106_FRAGMENTED, "udp"),
            },
        ),
        (
            "(b) Reported RSRP: -120 dBm",
            {
                "TCP": completion_from_files(DETECTOR_120_STREAM, "tcp"),
                "MQTT": completion_from_files(DETECTOR_120_STREAM, "mqtt"),
                "raw UDP": {},
                "fragmented UDP": completion_from_files(DETECTOR_120_FRAGMENTED, "udp"),
            },
        ),
    ]

    fig, axes = plt.subplots(1, 2, figsize=(7.15, 2.48), sharey=True)
    transport_styles = {
        "TCP": (TCP, "o", "-"),
        "MQTT": (MQTT, "s", "--"),
        "raw UDP": (UDP, "^", "-."),
        "fragmented UDP": ("#6a3d9a", "D", ":"),
    }
    x = np.arange(len(payloads))
    for ax, (title, condition) in zip(axes, conditions):
        for transport in transports:
            values = np.array(
                [condition[transport].get(payload, np.nan) for payload in payloads],
                dtype=float,
            )
            if not np.any(np.isfinite(values)):
                continue
            color, marker, linestyle = transport_styles[transport]
            ax.plot(
                x,
                values,
                color=color,
                marker=marker,
                linestyle=linestyle,
                ms=3.8,
                lw=1.1,
                label=transport,
            )
        ax.set_title(title, loc="left")
        ax.set_xticks(x, payload_labels, rotation=32, ha="right")
        ax.set_xlabel("IPI workload payload (KiB)")
        ax.set_ylim(-4, 104)
        ax.set_xlim(-0.3, len(payloads) - 0.7)
        ax.grid(axis="y", color="#e8e8e8", lw=0.5)
    axes[0].set_ylabel("Issued requests with a complete response (%)")
    handles, labels = axes[0].get_legend_handles_labels()
    axes[0].legend(
        handles,
        labels,
        frameon=False,
        loc="lower left",
        ncol=2,
        fontsize=6.0,
        handlelength=1.7,
        columnspacing=0.8,
    )
    handles, labels = axes[1].get_legend_handles_labels()
    axes[1].legend(
        handles,
        labels,
        frameon=False,
        loc="lower left",
        ncol=2,
        fontsize=6.0,
        handlelength=1.7,
        columnspacing=0.8,
    )
    fig.subplots_adjust(left=0.09, right=0.99, top=0.91, bottom=0.28, wspace=0.16)
    save(fig, "section5_protocol_completion.pdf")


def stationary_signal_effects() -> None:
    """Compare detector-sized uplink results across stationary radio records."""
    rsrp = np.array([-120.0, -109.5, -106.0, -101.0])
    completion = {
        "MQTT": np.array([4.4, 99.4, 30.8, 99.7]),
        "TCP": np.array([0.0, 86.3, 18.4, 97.8]),
    }
    p95 = {
        "MQTT": np.array([162.9, 75.8, 183.7, 59.7]),
        "TCP": np.array([188.7, 107.0, 169.5, 93.6]),
    }
    styles = {
        "MQTT": (MQTT, "o", "-"),
        "TCP": (TCP, "s", "--"),
    }

    fig, axes = plt.subplots(2, 1, figsize=(3.33, 3.55), sharex=True)
    for transport, (color, marker, linestyle) in styles.items():
        axes[0].plot(
            rsrp,
            p95[transport],
            color=color,
            marker=marker,
            linestyle=linestyle,
            ms=4.0,
            lw=1.1,
            label=transport,
        )
        axes[1].plot(
            rsrp,
            completion[transport],
            color=color,
            marker=marker,
            linestyle=linestyle,
            ms=4.0,
            lw=1.1,
            label=transport,
        )

    axes[0].axhline(100, color=GRAY, ls=":", lw=0.8)
    axes[0].text(-101.1, 104, "100-ms deadline", color=GRAY, fontsize=6.0, ha="right")
    axes[0].set_ylabel("Response p95 RTT (ms)")
    axes[0].set_title("(a) Detector-sized response tail", loc="left")
    axes[0].set_ylim(40, 205)
    axes[0].legend(frameon=False, loc="upper right", ncol=2)

    axes[1].axhline(90, color=GRAY, ls=":", lw=0.8)
    axes[1].text(-101.1, 92, "90% reference", color=GRAY, fontsize=6.0, ha="right")
    axes[1].set_ylabel("Completed within 100 ms (%)")
    axes[1].set_xlabel("Reported RSRP at stationary condition (dBm)")
    axes[1].set_title("(b) All-attempt deadline completion", loc="left")
    axes[1].set_ylim(-4, 104)

    axes[1].set_xticks(rsrp, ["-120", "-109.5", "-106", "-101"])
    for ax in axes:
        ax.axvline(-105, color="#bbbbbb", lw=0.6, ls="--")
        ax.grid(axis="y", color="#e8e8e8", lw=0.5)
    fig.subplots_adjust(left=0.18, right=0.98, top=0.95, bottom=0.16, hspace=0.44)
    save(fig, "section5_signal_effects.pdf")


def downlink_payload_latency() -> None:
    """Show the established-profile downlink-heavy CAV object result."""
    comparison = read_csv_rows(TDD_AUG19_40 / "matched_tdd_comparison.csv")
    payloads = np.array([1.0, 10.0, 100.0, 500.0])
    fig, ax = plt.subplots(figsize=(3.33, 2.45))
    transport_styles = {
        "tcp": (TCP, "o", "-"),
        "mqtt": (MQTT, "s", "--"),
    }
    percentile_styles = {
        "p50": ("tdd_70_20_10_p50_ms", 0.55),
        "p95": ("tdd_70_20_10_p95_ms", 1.0),
    }
    for transport, (color, marker, linestyle) in transport_styles.items():
        rows = [
            row for row in comparison
            if row["direction"] == "downlink" and row["transport"] == transport
        ]
        for percentile, (field, alpha) in percentile_styles.items():
            values = np.array([float(row[field]) for row in rows])
            ax.plot(
                payloads,
                values,
                color=color,
                marker=marker,
                linestyle=linestyle if percentile == "p95" else ":",
                alpha=alpha,
                ms=3.8,
                lw=1.1,
                label=f"{transport.upper()} {percentile}",
            )
    ax.axhline(100, color=GRAY, ls="-.", lw=0.8)
    ax.text(470, 103, "100-ms deadline", fontsize=6.0, color=GRAY, ha="right")
    ax.set_xscale("log")
    ax.set_xticks(payloads, ["1", "10", "100", "500"])
    ax.get_xaxis().set_major_formatter(mpl.ticker.ScalarFormatter())
    ax.set_ylim(20, 110)
    ax.set_xlabel("Edge-to-vehicle application object (KiB)")
    ax.set_ylabel("Response RTT (ms)")
    ax.set_title("Downlink-heavy exchange under 70/20/10")
    ax.grid(axis="y", color="#e8e8e8", lw=0.5)
    ax.legend(
        frameon=False,
        loc="upper left",
        ncol=2,
        fontsize=5.8,
        handlelength=1.7,
        columnspacing=0.7,
    )
    fig.subplots_adjust(left=0.18, right=0.98, top=0.88, bottom=0.22)
    save(fig, "section5_downlink_latency.pdf")


def mixed_load_and_qos() -> None:
    fig, axes = plt.subplots(
        2,
        1,
        figsize=(3.33, 3.70),
        gridspec_kw={"height_ratios": [2.25, 0.75]},
    )

    ax = axes[0]
    conditions = [
        "idle\n-106 dBm",
        "+load\n-106 dBm",
        "idle\nA",
        "+load\nA",
        "idle\nB",
        "+load\nB",
    ]
    mqtt_a100 = [99.70, 40.15, 53.80, 37.93, 70.50, 46.77]
    mqtt_a200 = [99.95, 98.00, 89.20, 83.63, 98.40, 91.90]
    tcp_a100 = [0.75, 0.40, 0.00, 0.03, 0.10, 0.20]
    tcp_a200 = [97.90, 59.00, 46.70, 38.40, 99.00, 49.17]
    x = np.arange(len(conditions))
    ax.plot(x, mqtt_a100, "o:", color=MQTT, lw=1.0, label="MQTT, 100 ms")
    ax.plot(x, mqtt_a200, "o-", color=MQTT, lw=1.2, label="MQTT, 200 ms")
    ax.plot(x, tcp_a100, "s:", color=TCP, lw=1.0, label="TCP, 100 ms")
    ax.plot(x, tcp_a200, "s-", color=TCP, lw=1.2, label="TCP, 200 ms")
    ax.axhline(99, color="#999999", ls="--", lw=0.6)
    ax.set_ylim(-3, 103)
    ax.set_xticks(x, conditions, fontsize=5.7)
    for boundary in [1.5, 3.5]:
        ax.axvline(boundary, color="#dddddd", lw=0.6)
    ax.set_ylabel("Deadline completion (%)")
    ax.set_title("(a) Foreground with uplink load", fontsize=8.0)
    ax.legend(frameon=False, loc="lower left", ncol=2, fontsize=6.0)
    ax.grid(axis="y", color="#eeeeee", lw=0.5)

    ax = axes[1]
    ax.axis("off")
    ax.set_title("(b) Requested and observed packet marking", fontsize=8.0, loc="left")
    ax.text(0.02, 0.56, "Requested label", ha="left", va="center", weight="bold", transform=ax.transAxes)
    ax.text(0.98, 0.56, "default / 5qi-mapped", ha="right", va="center", transform=ax.transAxes)
    ax.text(0.02, 0.18, "Observed IPv4 TOS", ha="left", va="center", weight="bold", transform=ax.transAxes)
    ax.text(0.98, 0.18, "0x0 / 0x0", ha="right", va="center", color="#a50f15", transform=ax.transAxes)

    fig.subplots_adjust(left=0.16, right=0.98, top=0.94, bottom=0.08, hspace=0.72)
    save(fig, "section5_mixed_load_qos.pdf")


def concurrency_and_interruption() -> None:
    clients = np.array([1, 2, 5, 10, 20, 50, 100])
    good = {
        "MQTT": [40.551, 42.278, 45.454, 47.873, 53.753, 54.056, 81.850],
        "TCP": [143.829, 145.652, 145.869, 143.852, 145.751, 166.067, 264.070],
        "UDP": [40.542, 41.462, 41.752, 41.869, 43.796, 53.967, 113.391],
    }
    weak = {
        "MQTT": [91.782, 89.926, 112.105, 129.995, 175.844, 1117.707, 5135.984],
        "TCP": [215.653, 222.610, 219.582, 259.796, 288.742, 769.911, 2908.420],
        "UDP": [82.047, 88.844, 93.048, 104.357, 129.373, 169.870, 172.500],
    }
    # Deadline completion at 100 and 200 ms is recomputed from every retained sender
    # CSV in 20260702_multiclient_scalability_run_4 (minus 106 dBm) and
    # 20260706_multiclient_scalability_weak_signal_run_1 (weak-path).
    completion_rows = [
        "-106 MQTT", "-106 TCP", "-106 UDP",
        "-120 MQTT", "-120 TCP", "-120 UDP",
    ]
    a100 = np.array(
        [
            [100.000, 99.950, 99.940, 99.990, 98.920, 99.544, 97.595],
            [3.000, 2.800, 2.720, 3.140, 2.635, 0.412, 0.021],
            [99.900, 100.000, 99.920, 100.000, 99.980, 99.748, 91.574],
            [96.600, 96.450, 92.820, 87.000, 72.410, 3.802, 0.751],
            [0.400, 0.550, 0.540, 0.480, 0.255, 0.002, 0.007],
            [97.800, 96.800, 95.960, 93.700, 89.070, 57.402, 44.337],
        ]
    )
    a200 = np.array(
        [
            [100.000, 100.000, 99.940, 99.990, 99.785, 99.876, 99.867],
            [99.800, 99.800, 99.040, 99.910, 99.460, 99.524, 63.808],
            [100.000, 100.000, 100.000, 100.000, 99.995, 99.972, 99.965],
            [99.800, 99.550, 99.520, 99.340, 97.575, 70.872, 23.515],
            [93.200, 90.750, 91.240, 84.240, 79.580, 7.338, 3.972],
            [99.500, 99.900, 99.420, 99.650, 99.325, 96.704, 90.747],
        ]
    )
    colors = {"MQTT": MQTT, "TCP": TCP, "UDP": UDP}
    marks = {"MQTT": "o", "TCP": "s", "UDP": "^"}

    fig, axes = plt.subplots(2, 2, figsize=(7.15, 3.35),
                             gridspec_kw={"height_ratios": [1.1, 0.9]})
    ax = axes[0, 0]
    for name in ["MQTT", "TCP", "UDP"]:
        ax.plot(clients, good[name], marker=marks[name], color=colors[name], lw=1.0,
                label=name)
        ax.plot(clients, weak[name], marker=marks[name], color=colors[name], lw=1.0,
                ls="--", label="_nolegend_")
    for y in [100, 200, 500, 1000]:
        ax.axhline(y, color="#bbbbbb", ls=":" if y in (200, 1000) else "--", lw=0.6)
    ax.set_yscale("log")
    ax.set_xscale("log")
    ax.set_xticks(clients, [str(c) for c in clients])
    ax.get_xaxis().set_major_formatter(mpl.ticker.ScalarFormatter())
    ax.set_ylim(30, 8000)
    ax.set_xlabel("Application clients (count)")
    ax.set_ylabel("Response p95 RTT (ms, log scale)")
    ax.set_title("(a) Workload-response tail versus demand")
    ax.grid(which="both", color="#eeeeee", lw=0.5)
    ax.legend(
        frameon=False,
        ncol=1,
        loc="upper left",
        title="Solid: -106 dBm; dashed: -120 dBm",
        title_fontsize=6.0,
        borderaxespad=0.35,
        handlelength=1.7,
        labelspacing=0.25,
    )

    ax = axes[0, 1]
    labels = ["MQTT\nbroker", "MQTT\nreceiver", "TCP\nreceiver", "UDP\nreceiver"]
    timeout_completion = [95.5, 99.1, 95.3, 96.9]
    restart_a200 = [94.7, 97.7, 92.9, 96.9]
    gaps = [15.1007, 13.3674, 11.7147, 13.4635]
    x = np.arange(4)
    width = 0.31
    bars_200 = ax.bar(x - width / 2, restart_a200, color=[MQTT, MQTT, TCP, UDP], alpha=0.38,
                      width=width, label="within 200 ms")
    bars = ax.bar(x + width / 2, timeout_completion, color=[MQTT, MQTT, TCP, UDP], alpha=0.86,
                  width=width, label="before timeout")
    ax.set_ylim(90, 100.3)
    ax.set_xticks(x, labels)
    ax.set_ylabel("Issued requests completed (%)")
    ax.set_title("(b) Responder or broker restart", pad=23)
    ax.grid(axis="y", color="#eeeeee", lw=0.5)
    ax2 = ax.twinx()
    ax2.spines["right"].set_visible(True)
    gap_line, = ax2.plot(x, gaps, "D-", color="#54278f", lw=1.0, ms=4)
    ax2.set_ylim(0, 18)
    ax2.set_ylabel("Response gap (s)")
    ax.legend(
        [bars_200[0], bars[0], gap_line],
        ["within 200 ms", "before timeout", "response gap"],
        loc="lower center",
        bbox_to_anchor=(0.5, 1.005),
        ncol=3,
        frameon=False,
        fontsize=5.8,
        handlelength=1.2,
        columnspacing=0.7,
        borderaxespad=0,
    )
    for bar_group, values, alignment in (
        (bars_200, restart_a200, "right"),
        (bars, timeout_completion, "left"),
    ):
        for bar, value in zip(bar_group, values):
            ax.text(
                bar.get_x() + bar.get_width() / 2,
                value - 0.35,
                f"{value:g}",
                ha=alignment,
                va="top",
                fontsize=5.8,
            )

    def deadline_heatmap(ax: plt.Axes, values: np.ndarray, title: str) -> None:
        ax.imshow(values, vmin=0, vmax=100, cmap="Blues", aspect="auto")
        ax.set_xticks(range(len(clients)), [str(c) for c in clients])
        ax.tick_params(axis="x", labelsize=6.0)
        ax.set_yticks(range(len(completion_rows)), completion_rows)
        ax.set_xlabel("Application clients (count)")
        ax.set_title(title)
        for i in range(values.shape[0]):
            for j in range(values.shape[1]):
                value = values[i, j]
                if value < 0.1:
                    label = "<0.1%"
                elif value < 1:
                    label = f"{value:.1f}%"
                else:
                    label = f"{value:.0f}%"
                ax.text(j, i, label, ha="center", va="center", fontsize=5.7,
                        color="white" if value >= 67 else "black")

    deadline_heatmap(axes[1, 0], a100, "(c) Completed within 100 ms (%)")
    deadline_heatmap(axes[1, 1], a200, "(d) Completed within 200 ms (%)")

    fig.subplots_adjust(wspace=0.46, hspace=0.72, top=0.88, bottom=0.10)
    save(fig, "section5_concurrency_interruption.pdf")


def signal_survey() -> None:
    """Plot the separate-UE 16-point radio survey without interpolating it."""
    east = np.array([141.39, 49.70, 147.39, -160.25, -58.27, 168.82,
                     273.36, 83.98, -124.25, -17.14, 173.96, 237.37,
                     -70.27, 25.71, 3.43, 182.53])
    north = np.array([87.33, -50.85, -48.64, -223.29, -192.34, -177.97,
                      -191.23, -182.39, -388.00, -378.05, -341.57, -358.15,
                      -420.05, -390.21, -489.69, -476.43])
    distance = np.array([166.19, 71.10, 155.21, 274.84, 200.97, 245.30,
                         333.61, 200.80, 407.41, 378.44, 383.31, 429.67,
                         425.89, 391.05, 489.70, 510.19])
    rsrp = np.array([-121, -109, -110, -107, -91, -104, -110, -101,
                     -102, -100, -105, -107, -103, -106, -120, -114])
    snr = np.array([4.5, 13.0, 16.5, 19.0, 27.5, 21.0, 14.0, 19.0,
                    20.0, 22.0, 15.0, 17.5, 25.0, 14.5, 3.5, 11.0])
    point_id = np.arange(1, 17)

    fig, axes = plt.subplots(
        1, 3, figsize=(7.15, 2.35),
        gridspec_kw={"width_ratios": [1.28, 1.0, 1.0]},
    )
    ax = axes[0]
    points = ax.scatter(east, north, c=rsrp, cmap="viridis", vmin=-125,
                        vmax=-85, s=34, edgecolor="black", linewidth=0.45)
    ax.scatter([0], [0], marker="^", s=80, color="#111111", label="gNodeB")
    for idx, xval, yval in zip(point_id, east, north):
        ax.annotate(str(idx), (xval, yval), xytext=(3, 2),
                    textcoords="offset points", fontsize=5.8)
    ax.set_aspect("equal", adjustable="box")
    ax.set_xlim(-190, 305)
    ax.set_ylim(-535, 125)
    ax.set_xlabel("East of gNodeB (m)")
    ax.set_ylabel("North of gNodeB (m)")
    ax.set_title("(a) Spatial RSRP survey")
    ax.grid(color="#e8e8e8", lw=0.5)
    ax.legend(frameon=False, loc="lower left")
    cbar = fig.colorbar(points, ax=ax, fraction=0.047, pad=0.03)

    ax = axes[1]
    ax.scatter(distance, rsrp, c=rsrp, cmap="viridis", vmin=-125, vmax=-85,
               s=30, edgecolor="black", linewidth=0.4)
    for idx, xval, yval in zip(point_id, distance, rsrp):
        ax.annotate(str(idx), (xval, yval), xytext=(2, 2),
                    textcoords="offset points", fontsize=5.6)
    ax.set_xlim(40, 535)
    ax.set_ylim(-125, -85)
    ax.set_xlabel("Distance to gNodeB (m)")
    ax.set_ylabel("RSRP (dBm)")
    ax.set_title("(b) Received power")
    ax.grid(color="#e8e8e8", lw=0.5)

    ax = axes[2]
    ax.scatter(distance, snr, c=snr, cmap="cividis", vmin=0, vmax=30,
               s=30, edgecolor="black", linewidth=0.4)
    for idx, xval, yval in zip(point_id, distance, snr):
        ax.annotate(str(idx), (xval, yval), xytext=(2, 2),
                    textcoords="offset points", fontsize=5.6)
    ax.set_xlim(40, 535)
    ax.set_ylim(0, 31)
    ax.set_xlabel("Distance to gNodeB (m)")
    ax.set_ylabel("Reported SNR (dB)")
    ax.set_title("(c) Signal quality")
    ax.grid(color="#e8e8e8", lw=0.5)

    fig.subplots_adjust(wspace=0.62)
    save(fig, "section4_signal_survey.pdf")


def tdd_profile_results() -> None:
    """Compare August 19 same-device CAV bursts and directional streams."""
    fig, axes = plt.subplots(2, 2, figsize=(7.15, 4.65))

    comparison = read_csv_rows(TDD_AUG19_40 / "matched_tdd_comparison.csv")
    panels = [
        (axes[0, 0], "uplink", "(a) Vehicle-to-edge burst"),
        (axes[0, 1], "downlink", "(b) Edge-to-vehicle burst"),
    ]
    profile_styles = {
        "70/20/10": ("tdd_70_20_10_p95_ms", TDD_BASELINE),
        "40/40/20": ("tdd_40_40_20_p95_ms", TDD_ALTERNATIVE),
    }
    transport_styles = {
        "tcp": ("o", "-"),
        "mqtt": ("s", "--"),
    }
    for ax, direction, title in panels:
        for transport, (marker, linestyle) in transport_styles.items():
            rows = [
                row for row in comparison
                if row["direction"] == direction
                and row["transport"] == transport
            ]
            payloads = np.array([float(row["payload_kib"]) for row in rows])
            for profile, (field, color) in profile_styles.items():
                p95 = np.array([float(row[field]) for row in rows])
                ax.plot(
                    payloads,
                    p95,
                    marker=marker,
                    linestyle=linestyle,
                    ms=4.0,
                    lw=1.25,
                    color=color,
                    label=f"{profile} {transport.upper()}",
                )
        ax.axhline(100, color="#777777", ls=":", lw=0.75)
        ax.axhline(500, color="#333333", ls="-.", lw=0.75)
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_xticks(payloads, ["1", "10", "100", "500"])
        ax.set_yticks(
            [40, 100, 200, 500, 1000, 1500],
            ["40", "100", "200", "500", "1,000", "1,500"],
        )
        ax.set_xlim(0.7, 720)
        ax.set_ylim(35, 1600)
        payload_direction = (
            "Vehicle-to-edge payload (KiB)"
            if direction == "uplink"
            else "Edge-to-vehicle payload (KiB)"
        )
        ax.set_xlabel(payload_direction)
        ax.set_ylabel("Response p95 RTT (ms, log scale)")
        ax.set_title(title)
        ax.grid(which="both", color="#eeeeee", lw=0.55)
    handles, labels = axes[0, 0].get_legend_handles_labels()
    fig.legend(
        handles,
        labels,
        frameon=False,
        loc="upper center",
        bbox_to_anchor=(0.54, 0.995),
        ncol=4,
        fontsize=6.1,
        handlelength=2.2,
        columnspacing=1.15,
    )
    axes[0, 1].text(1.05, 105, "100 ms", fontsize=5.8, color="#555555")
    axes[0, 1].text(1.05, 515, "500 ms", fontsize=5.8, color="#333333")

    profile_sources = {
        "70/20/10": TDD_AUG19_70 / "bandwidth.csv",
        "40/40/20": TDD_AUG19_40 / "bandwidth.csv",
    }
    profile_colors = {
        "70/20/10": TDD_BASELINE,
        "40/40/20": TDD_ALTERNATIVE,
    }
    stream_panels = [
        (axes[1, 0], "upload", "(c) Sustained vehicle-to-edge transfer", 35.0),
        (axes[1, 1], "download", "(d) Sustained edge-to-vehicle transfer", 210.0),
    ]
    for ax, direction, title, ymax in stream_panels:
        x = np.arange(len(profile_sources), dtype=float)
        means = []
        ci_low = []
        ci_high = []
        for xpos, (profile, path) in zip(x, profile_sources.items()):
            rows = [
                row for row in read_csv_rows(path)
                if row["direction"] == direction and row["status"] == "complete"
            ]
            values = np.array([float(row["receiver_throughput_mbps"]) for row in rows])
            mean = float(np.mean(values))
            sem = float(np.std(values, ddof=1) / np.sqrt(len(values)))
            half_width = 4.30265273 * sem  # 95% t interval with n=3 (df=2)
            means.append(mean)
            ci_low.append(mean - half_width)
            ci_high.append(mean + half_width)
            jitter = np.linspace(-0.09, 0.09, len(values))
            ax.scatter(
                xpos + jitter,
                values,
                s=20,
                facecolor="white",
                edgecolor=profile_colors[profile],
                linewidth=0.9,
                zorder=2,
            )
        means_array = np.array(means)
        errors = np.vstack(
            (means_array - np.array(ci_low), np.array(ci_high) - means_array)
        )
        ax.errorbar(
            x,
            means_array,
            yerr=errors,
            fmt="D",
            markersize=4.2,
            color="black",
            markerfacecolor="black",
            capsize=3,
            lw=1.0,
            zorder=3,
        )
        for xpos, value, profile in zip(x, means_array, profile_sources):
            ax.text(
                xpos,
                value + ymax * 0.045,
                f"{value:.1f}",
                ha="center",
                va="bottom",
                fontsize=6.4,
                color=profile_colors[profile],
            )
        ax.set_xticks(x, list(profile_sources))
        ax.set_ylim(0, ymax)
        ax.set_ylabel("Application goodput (Mbit/s)")
        ax.set_title(title)
        ax.grid(axis="y", color="#eeeeee", lw=0.55)
    axes[1, 0].axhline(32, color="#555555", ls=":", lw=0.8)
    axes[1, 0].text(
        0.02,
        32.5,
        "32-Mbit/s video reference",
        fontsize=5.8,
        color="#555555",
        ha="left",
        va="bottom",
    )

    fig.subplots_adjust(left=0.085, right=0.99, top=0.89, bottom=0.095,
                        hspace=0.48, wspace=0.32)
    save(fig, "section5_tdd_profiles.pdf")


def directional_workloads() -> None:
    """Plot exact endpoint goodput and a matched weak-signal direction result."""
    fig, axes = plt.subplots(1, 3, figsize=(7.15, 2.42))

    summary = read_csv_rows(TDD_AUG17 / "throughput_summary.csv")
    repetitions = read_csv_rows(TDD_AUG17 / "throughput_repetitions.csv")
    directions = [
        (
            "upload_vehicle_to_d1",
            "upload_vehicle_to_d1_mbps",
            "(a) Exact vehicle-to-edge upload",
            14.0,
        ),
        (
            "download_d1_to_vehicle",
            "download_d1_to_vehicle_mbps",
            "(b) Exact edge-to-vehicle download",
            165.0,
        ),
    ]
    profiles = ["70/20/10", "40/40/20"]
    colors = [TDD_BASELINE, TDD_ALTERNATIVE]
    for ax, (direction, repetition_field, title, ymax) in zip(axes[:2], directions):
        rows = {
            row["tdd_profile"]: row
            for row in summary
            if row["direction"] == direction
        }
        means = np.array([float(rows[profile]["mean_mbps"]) for profile in profiles])
        lows = np.array([float(rows[profile]["ci95_low_mbps"]) for profile in profiles])
        highs = np.array([float(rows[profile]["ci95_high_mbps"]) for profile in profiles])
        errors = np.vstack((means - lows, highs - means))
        x = np.arange(len(profiles), dtype=float)
        for xpos, profile, color in zip(x, profiles, colors):
            values = np.array([
                float(row[repetition_field])
                for row in repetitions
                if row["tdd_profile"] == profile
            ])
            jitter = np.linspace(-0.11, 0.11, len(values))
            ax.scatter(
                xpos + jitter,
                values,
                s=16,
                facecolor="white",
                edgecolor=color,
                linewidth=0.75,
                zorder=2,
            )
        ax.errorbar(
            x,
            means,
            yerr=errors,
            fmt="D",
            markersize=4.0,
            color="black",
            markerfacecolor="black",
            capsize=3,
            lw=1.0,
            zorder=3,
        )
        for xpos, value, color in zip(x, means, colors):
            ax.text(
                xpos,
                ymax * 0.965,
                f"mean {value:.3f}",
                ha="center",
                va="top",
                fontsize=6.3,
                color=color,
                bbox={"facecolor": "white", "edgecolor": "none", "pad": 0.8},
            )
        ax.set_xticks(x, profiles)
        ax.set_ylim(0, ymax)
        ax.set_ylabel("Application goodput (Mbit/s)")
        ax.set_title(title)
        ax.grid(axis="y", color="#eeeeee", lw=0.55)

    uplink_rows = read_csv_rows(TDD_AUG18 / "uplink_rtt.csv")
    downlink_rows = read_csv_rows(TDD_AUG18 / "downlink_rtt.csv")
    uplink = next(
        row for row in uplink_rows
        if row["transport"] == "mqtt" and row["payload_kib"] == "500"
    )
    downlink = next(
        row for row in downlink_rows
        if row["transport"] == "mqtt" and row["payload_kib"] == "500"
    )
    ax = axes[2]
    x = np.array([0.0, 1.0])
    p50 = np.array([float(uplink["rtt_p50_ms"]), float(downlink["rtt_p50_ms"])])
    p95 = np.array([float(uplink["rtt_p95_ms"]), float(downlink["rtt_p95_ms"])])
    ax.scatter(x - 0.11, p50, marker="o", s=30, color="#f0f0f0", edgecolor="black",
               linewidth=0.6, label="p50", zorder=3)
    ax.scatter(x + 0.11, p95, marker="^", s=34, color=TDD_ALTERNATIVE,
               edgecolor="black", linewidth=0.5, label="p95", zorder=3)
    for xpos, value in zip(x - 0.11, p50):
        label = f"{value / 1000:.2f} s" if value >= 1000 else f"{value:.0f} ms"
        ax.text(xpos, value / 1.13, label, ha="center", va="top", fontsize=5.9)
    for xpos, value in zip(x + 0.11, p95):
        label = f"{value / 1000:.2f} s" if value >= 1000 else f"{value:.0f} ms"
        ax.text(xpos, value * 1.13, label, ha="center", va="bottom", fontsize=5.9)
    ax.axhline(500, color="#555555", ls=":", lw=0.8)
    ax.text(0.03, 535, "500-ms reference", fontsize=5.8, color="#555555")
    ax.set_yscale("log")
    ax.set_ylim(80, 14500)
    ax.set_xlim(-0.42, 1.42)
    ax.set_xticks(x, ["Vehicle payload\nto edge", "Edge payload\nto vehicle"])
    ax.set_ylabel("RTT (ms, log scale)")
    ax.set_title("(c) Matched 40/40/20, 500-KiB MQTT")
    ax.legend(frameon=False, loc="upper right", fontsize=6.0)
    ax.grid(axis="y", which="both", color="#eeeeee", lw=0.55)

    fig.subplots_adjust(left=0.075, right=0.99, top=0.89, bottom=0.25, wspace=0.44)
    save(fig, "section5_directional_workloads.pdf")


def cross_application_comparison() -> None:
    """Place measured response p95 and application deadlines on one time axis."""
    rows = [
        ("Signal warning — PC5", "PC5", 100.0, 106.873, None),
        ("Signal warning — 5G Uu", "5G Uu", 100.0, 34.1, None),
        (
            "Blind-spot warning — PC5 routes",
            "PC5",
            100.0,
            42.905,
            (41.621, 108.421),
        ),
        ("Emergency signal priority — PC5", "PC5", 10.0, 106.88, None),
        ("Emergency signal priority — 5G Uu", "5G Uu", 10.0, 44.49, None),
        ("Emergency maneuver — PC5", "PC5", 10.0, 111.66, None),
        ("Emergency maneuver — 5G Uu", "5G Uu", 10.0, 79.60, None),
        ("Remote recovery — 5G Uu", "5G Uu", 100.0, 175.0, None),
    ]
    fig, ax = plt.subplots(figsize=(7.15, 2.85))
    y = np.arange(len(rows))

    for index, (label, path, deadline, measured, interval) in enumerate(rows):
        within = measured <= deadline
        ax.hlines(
            index,
            min(deadline, measured),
            max(deadline, measured),
            color="#238b45" if within else "#cb181d",
            lw=1.8,
            alpha=0.75,
            zorder=1,
        )
        ax.scatter(
            deadline,
            index,
            marker="|",
            s=135,
            color="black",
            linewidth=1.6,
            zorder=3,
        )
        color = PC5 if path == "PC5" else FIVEG
        marker = "o" if path == "PC5" else "s"
        ax.scatter(
            measured,
            index,
            marker=marker,
            s=34,
            color=color,
            edgecolor="black",
            linewidth=0.4,
            zorder=4,
        )
        if interval is not None:
            ax.errorbar(
                measured,
                index,
                xerr=[[measured - interval[0]], [interval[1] - measured]],
                fmt="none",
                ecolor=color,
                capsize=2.5,
                lw=1.1,
                zorder=2,
            )
        ax.text(
            measured * (1.06 if measured < 145 else 0.95),
            index,
            f"{measured:.1f} ms",
            ha="left" if measured < 145 else "right",
            va="center",
            fontsize=6.0,
        )

    ax.scatter([], [], marker="|", s=135, color="black", linewidth=1.6, label="Application deadline")
    ax.scatter([], [], marker="o", s=34, color=PC5, edgecolor="black", linewidth=0.4, label="PC5 response p95")
    ax.scatter([], [], marker="s", s=34, color=FIVEG, edgecolor="black", linewidth=0.4, label="5G Uu response p95")
    ax.plot([], [], color="#238b45", lw=1.8, label="Within deadline")
    ax.plot([], [], color="#cb181d", lw=1.8, label="Exceeds deadline")

    ax.set_xscale("log")
    ax.set_xlim(6, 230)
    ax.set_xticks([10, 25, 50, 100, 200], ["10", "25", "50", "100", "200"])
    ax.set_ylim(len(rows) - 0.45, -0.55)
    ax.set_yticks(y, [row[0] for row in rows])
    ax.set_xlabel("Application deadline and measured response p95 RTT (ms, log scale)")
    ax.set_title("Do measured response tails meet application deadlines?")
    ax.grid(axis="x", which="both", color="#e8e8e8", lw=0.5)
    ax.legend(
        frameon=False,
        loc="upper center",
        bbox_to_anchor=(0.54, 1.02),
        ncol=5,
        fontsize=5.9,
        handlelength=1.5,
        columnspacing=0.8,
    )
    fig.subplots_adjust(left=0.36, right=0.99, top=0.84, bottom=0.17)
    save(fig, "section5_cross_application_comparison.pdf")


def main() -> None:
    pc5_payload_and_route()
    fiveg_deadline_envelope()
    protocol_completion()
    stationary_signal_effects()
    downlink_payload_latency()
    mixed_load_and_qos()
    concurrency_and_interruption()
    tdd_profile_results()
    directional_workloads()
    cross_application_comparison()


if __name__ == "__main__":
    main()
