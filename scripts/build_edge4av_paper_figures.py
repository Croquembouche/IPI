#!/usr/bin/env python3
"""Build reproducible paper figures from stored Edge4AV result summaries."""

from __future__ import annotations

import csv
import re
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.lines import Line2D  # noqa: E402
from matplotlib.patches import FancyArrowPatch, FancyBboxPatch  # noqa: E402


ROOT = Path(__file__).resolve().parents[1]
FIG_DIR = ROOT / "paper_attempt3" / "figs"
DEADLINE_SOURCE = (
    ROOT
    / "results"
    / "real_5g"
    / "20260702_end_to_end_deadline_analysis_run_1"
    / "summary.md"
)
MULTICLIENT_SOURCES = {
    "good-signal": ROOT / "results" / "real_5g" / "20260702_multiclient_scalability_run_4" / "summary.md",
    "weak-signal": ROOT
    / "results"
    / "real_5g"
    / "20260706_multiclient_scalability_weak_signal_run_1"
    / "summary.md",
}
FAILURE_SOURCE = ROOT / "results" / "real_5g" / "20260703_failure_fallback_run_1" / "summary.md"
MOCAR_CLEAN_SOURCES = [
    ROOT / "results" / "mocar_v2x" / "20260703_exp_01_payload_sweep_0_2kb_final" / "payload_summary.csv",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_01_payload_sweep_0_2kb_144149" / "payload_summary.csv",
]
MOCAR_DEGRADED_SOURCES = [
    ROOT / "results" / "mocar_v2x" / "20260703_exp_01_payload_sweep_0_2kb_171209" / "payload_summary.csv",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_01_payload_sweep_0_2kb_133527" / "payload_summary.csv",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_01_payload_sweep_0_2kb_151351" / "payload_summary.csv",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_01_payload_sweep_0_2kb_155120" / "payload_summary.csv",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_01_payload_sweep_0_2kb_164129" / "payload_summary.csv",
]
MOCAR_MOVING_SOURCES = [
    ROOT / "results" / "mocar_v2x" / "20260704_exp_02_radio_distance_mobility_173816" / "summary.md",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_02_radio_distance_mobility_175820" / "summary.md",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_02_radio_distance_mobility_182104" / "summary.md",
    ROOT / "results" / "mocar_v2x" / "20260704_exp_02_radio_distance_mobility_183232" / "summary.md",
]
MOCAR_SETUP_SOURCE = ROOT / "results" / "mocar_v2x" / "20260703_setup_test" / "summary.md"

TRANSPORT_COLOR = {
    "TCP": "#0072B2",
    "UDP": "#D55E00",
    "MQTT": "#009E73",
}
TRANSPORT_MARKER = {
    "TCP": "s",
    "UDP": "^",
    "MQTT": "o",
}


def parse_number(value: str) -> float:
    cleaned = value.strip().replace(",", "").replace("%", "")
    return float(cleaned)


def table_after_heading(path: Path, heading: str) -> list[dict[str, str]]:
    lines = path.read_text(encoding="utf-8").splitlines()
    start = None
    for index, line in enumerate(lines):
        if line.strip() == heading:
            start = index + 1
            break
    if start is None:
        raise ValueError(f"Heading {heading!r} not found in {path}")

    table_lines: list[str] = []
    in_table = False
    for line in lines[start:]:
        stripped = line.strip()
        if stripped.startswith("## ") and in_table:
            break
        if stripped.startswith("|"):
            table_lines.append(stripped)
            in_table = True
        elif in_table and stripped:
            break

    if len(table_lines) < 3:
        raise ValueError(f"No Markdown table found after {heading!r} in {path}")

    headers = [cell.strip() for cell in table_lines[0].strip("|").split("|")]
    rows: list[dict[str, str]] = []
    for line in table_lines[2:]:
        cells = [cell.strip() for cell in line.strip("|").split("|")]
        if len(cells) != len(headers):
            raise ValueError(f"Malformed table row in {path}: {line}")
        rows.append(dict(zip(headers, cells)))
    return rows


def load_deadline_rows() -> list[dict[str, float | str]]:
    rows = []
    for row in table_after_heading(DEADLINE_SOURCE, "## Service-Envelope Summary"):
        rows.append(
            {
                "group": row["Evidence group"],
                "attempts": parse_number(row["Attempts"]),
                "success_pct": parse_number(row["Success %"]),
                "p50_ms": parse_number(row["p50 RTT ms"]),
                "p95_ms": parse_number(row["p95 RTT ms"]),
                "p99_ms": parse_number(row["p99 RTT ms"]),
                "miss_100_pct": parse_number(row["Miss @100 ms"]),
                "miss_500_pct": parse_number(row["Miss @500 ms"]),
                "miss_1000_pct": parse_number(row["Miss @1000 ms"]),
            }
        )
    return rows


def load_multiclient_rows() -> list[dict[str, float | str]]:
    all_rows = []
    for condition, path in MULTICLIENT_SOURCES.items():
        for row in table_after_heading(path, "## Results"):
            all_rows.append(
                {
                    "condition": condition,
                    "transport": row["Transport"],
                    "clients": int(parse_number(row["Clients"])),
                    "attempts": int(parse_number(row["Attempts"])),
                    "success_pct": parse_number(row["Success %"]),
                    "p50_ms": parse_number(row["p50 ms"]),
                    "p95_ms": parse_number(row["p95 ms"]),
                    "p99_ms": parse_number(row["p99 ms"]),
                }
            )
    return all_rows


def load_failure_rows() -> list[dict[str, float | str]]:
    rows = []
    for row in table_after_heading(FAILURE_SOURCE, "## Results"):
        rows.append(
            {
                "failure_mode": row["Failure mode"],
                "transport": row["Transport"],
                "attempts": int(parse_number(row["Attempts"])),
                "accepted": int(parse_number(row["Accepted"])),
                "failed": int(parse_number(row["Failed"])),
                "success_pct": parse_number(row["Success %"]),
                "p95_ms": parse_number(row["p95 ms"]),
                "success_gap_ms": parse_number(row["Success gap ms"]),
            }
        )
    return rows


def relative_sources(paths: list[Path]) -> str:
    return "; ".join(str(path.relative_to(ROOT)) for path in paths)


def load_payload_summary_rows(path: Path) -> list[dict[str, float | str]]:
    rows = []
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            attempts = row.get("analysis attempts") or row.get("attempts_for_analysis") or row.get("rows")
            successes = row.get("analysis success") or row.get("success_for_analysis") or row.get("success")
            p95 = row.get("p95 RTT ms") or row.get("p95_rtt_ms") or ""
            rows.append(
                {
                    "attempts": int(parse_number(str(attempts))),
                    "successes": int(parse_number(str(successes))),
                    "p95_ms": parse_number(p95) if str(p95).strip() else "",
                }
            )
    return rows


def aggregate_payload_summaries(paths: list[Path], condition: str, latency_metric: str) -> dict[str, float | str]:
    attempts = 0
    successes = 0
    p95_values = []
    for path in paths:
        for row in load_payload_summary_rows(path):
            attempts += int(row["attempts"])
            successes += int(row["successes"])
            if row["p95_ms"] != "":
                p95_values.append(float(row["p95_ms"]))

    return {
        "condition": condition,
        "attempts": attempts,
        "successes": successes,
        "success_pct": 100.0 * successes / attempts,
        "latency_metric": latency_metric,
        "latency_low_ms": min(p95_values) if p95_values and latency_metric else "",
        "latency_high_ms": max(p95_values) if p95_values and latency_metric else "",
        "source_paths": relative_sources(paths),
    }


def markdown_scalar(path: Path, labels: list[str]) -> float:
    text = path.read_text(encoding="utf-8")
    for label in labels:
        pattern = rf"^- {re.escape(label)}: `?([^`\n]+)`?"
        match = re.search(pattern, text, flags=re.MULTILINE)
        if match:
            return parse_number(match.group(1))
    raise ValueError(f"None of {labels!r} found in {path}")


def load_mocar_rows() -> list[dict[str, float | str]]:
    rows = [
        aggregate_payload_summaries(
            MOCAR_CLEAN_SOURCES,
            "Clean stationary 0-2 KiB",
            "successful-reply p95 RTT range",
        ),
        aggregate_payload_summaries(
            MOCAR_DEGRADED_SOURCES,
            "Degraded stationary sweeps",
            "",
        ),
    ]

    moving_attempts = 0
    moving_successes = 0
    moving_p95 = []
    for path in MOCAR_MOVING_SOURCES:
        moving_attempts += int(markdown_scalar(path, ["Rows", "Rows collected before operator stop"]))
        moving_successes += int(markdown_scalar(path, ["Success"]))
        moving_p95.append(markdown_scalar(path, ["p95 RTT ms"]))
    rows.append(
        {
            "condition": "Moving 256 B route runs",
            "attempts": moving_attempts,
            "successes": moving_successes,
            "success_pct": 100.0 * moving_successes / moving_attempts,
            "latency_metric": "successful-reply p95 RTT range",
            "latency_low_ms": min(moving_p95),
            "latency_high_ms": max(moving_p95),
            "source_paths": relative_sources(MOCAR_MOVING_SOURCES),
        }
    )

    setup_text = MOCAR_SETUP_SOURCE.read_text(encoding="utf-8")
    setup_averages = [parse_number(value) for value in re.findall(r"average RTT `([0-9.]+) ms`", setup_text)]
    if len(setup_averages) != 2:
        raise ValueError(f"Expected two setup RTT averages in {MOCAR_SETUP_SOURCE}")
    rows.append(
        {
            "condition": "Bidirectional custom RTT setup",
            "attempts": 20,
            "successes": 20,
            "success_pct": 100.0,
            "latency_metric": "directional average RTT range",
            "latency_low_ms": min(setup_averages),
            "latency_high_ms": max(setup_averages),
            "source_paths": str(MOCAR_SETUP_SOURCE.relative_to(ROOT)),
        }
    )

    return rows


def write_csv(path: Path, rows: list[dict[str, float | str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)


def add_box(ax, xy, width, height, title, lines, facecolor, edgecolor="#2f3b45"):
    box = FancyBboxPatch(
        xy,
        width,
        height,
        boxstyle="round,pad=0.018,rounding_size=0.018",
        linewidth=0.9,
        edgecolor=edgecolor,
        facecolor=facecolor,
    )
    ax.add_patch(box)
    x, y = xy
    ax.text(x + width / 2, y + height - 0.055, title, ha="center", va="top", fontsize=8.2, fontweight="bold")
    ax.text(
        x + 0.025,
        y + height - 0.115,
        "\n".join(lines),
        ha="left",
        va="top",
        fontsize=7.2,
        linespacing=1.15,
    )
    return box


def add_arrow(ax, start, end, label, color="#4b6777", curve=0.0, label_offset=0.025):
    arrow = FancyArrowPatch(
        start,
        end,
        arrowstyle="-|>",
        mutation_scale=10,
        linewidth=1.2,
        color=color,
        connectionstyle=f"arc3,rad={curve}",
    )
    ax.add_patch(arrow)
    mid_x = (start[0] + end[0]) / 2
    mid_y = (start[1] + end[1]) / 2
    va = "bottom" if label_offset >= 0 else "top"
    ax.text(mid_x, mid_y + label_offset, label, ha="center", va=va, fontsize=7.0, color=color)


def plot_measured_paths() -> None:
    fig, ax = plt.subplots(figsize=(7.05, 3.15))
    ax.set_axis_off()
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)

    add_box(
        ax,
        (0.03, 0.58),
        0.22,
        0.30,
        "Vehicle / OBU",
        ["service client", "Mocar OBU", "sender CSVs", "GNSS capture"],
        "#eef6fb",
    )
    add_box(
        ax,
        (0.38, 0.58),
        0.24,
        0.30,
        "Intersection / RSU",
        ["IPI SPaT bridge", "Mocar RSU", "custom RTT RX"],
        "#edf8f0",
    )
    add_box(
        ax,
        (0.74, 0.58),
        0.22,
        0.30,
        "Private-5G Edge",
        ["TCP receiver", "MQTT broker", "UDP sensitivity", "failure restart"],
        "#fff4e6",
    )
    add_box(
        ax,
        (0.20, 0.12),
        0.25,
        0.30,
        "Workload Sources",
        ["V2X-Radar detector", "OpenDAIR / TruckV2X", "payload manifest", "GPU / loopback checks"],
        "#f5f0ff",
    )
    add_box(
        ax,
        (0.56, 0.12),
        0.25,
        0.30,
        "Service Types",
        ["shared state", "addressed assistance", "detector summary", "bulk / fallback"],
        "#f7f7f7",
    )

    add_arrow(ax, (0.38, 0.75), (0.25, 0.75), "V2X broadcast")
    add_arrow(ax, (0.25, 0.67), (0.38, 0.67), "custom RTT", curve=-0.08, label_offset=-0.025)
    add_arrow(ax, (0.25, 0.55), (0.74, 0.55), "private-5G session", label_offset=-0.025)
    add_arrow(ax, (0.45, 0.31), (0.56, 0.31), "payload classes")
    add_arrow(ax, (0.68, 0.42), (0.78, 0.58), "replay")
    add_arrow(ax, (0.74, 0.72), (0.61, 0.72), "edge reply", curve=0.08)
    add_arrow(ax, (0.68, 0.58), (0.68, 0.42), "deadline analysis")

    ax.text(
        0.5,
        0.02,
        "Each path writes stored sender/receiver logs, summaries, figure data, and claim scope.",
        ha="center",
        va="bottom",
        fontsize=7.4,
        color="#39464f",
    )

    fig.subplots_adjust(left=0.01, right=0.99, top=0.98, bottom=0.02)
    fig.savefig(FIG_DIR / "edge4av_measured_paths.pdf")
    fig.savefig(FIG_DIR / "edge4av_measured_paths.png", dpi=300)
    plt.close(fig)


def plot_mocar_reliability(rows: list[dict[str, float | str]]) -> None:
    labels = ["clean\nstatic", "degraded\nstatic", "moving\nroute", "setup\nRTT"]
    x = list(range(len(rows)))
    success_pct = [float(row["success_pct"]) for row in rows]

    fig, (ax_success, ax_latency) = plt.subplots(
        2,
        1,
        figsize=(3.45, 3.35),
        sharex=True,
        gridspec_kw={"height_ratios": [1.0, 0.95], "hspace": 0.38},
    )
    bar_colors = ["#0072B2", "#0072B2", "#0072B2", "#6b7280"]
    bars = ax_success.bar(x, success_pct, color=bar_colors, width=0.62)
    ax_success.set_ylabel("successful replies (%)")
    ax_success.set_ylim(0, 105)
    ax_success.set_xticks(x)
    ax_success.grid(True, axis="y", color="#d7dee3", linewidth=0.7)
    ax_success.spines["top"].set_visible(False)
    ax_success.spines["right"].set_visible(False)

    for bar, pct in zip(bars, success_pct):
        if pct == 100.0:
            pct_label = "100%"
        elif pct >= 99.5:
            pct_label = f"{pct:.2f}%"
        else:
            pct_label = f"{pct:.1f}%"
        ax_success.text(
            bar.get_x() + bar.get_width() / 2,
            min(pct + 3.0, 101.5),
            pct_label,
            ha="center",
            va="bottom",
            fontsize=7.2,
            color="#1f2a33",
        )
    ax_success.text(
        x[-1],
        48,
        "setup\ncheck",
        ha="center",
        va="center",
        fontsize=6.1,
        color="white",
        fontweight="bold",
    )

    ax_latency.axhline(100, color="#94a3ad", linestyle="--", linewidth=0.85)
    for idx, row in enumerate(rows):
        if row["latency_low_ms"] == "":
            continue
        low = float(row["latency_low_ms"])
        high = float(row["latency_high_ms"])
        mid = (low + high) / 2.0
        ax_latency.errorbar(
            idx,
            mid,
            yerr=[[mid - low], [high - mid]],
            color="#D55E00",
            marker="D",
            markersize=4.5,
            linewidth=1.3,
            capsize=3.5,
            zorder=3,
        )
    ax_latency.set_ylabel("RTT metric (ms)")
    ax_latency.set_xticks(x)
    ax_latency.set_xticklabels(labels)
    ax_latency.set_ylim(0, 130)
    ax_latency.spines["top"].set_visible(False)
    ax_latency.spines["right"].set_visible(False)
    ax_latency.grid(True, axis="y", color="#d7dee3", linewidth=0.7)
    ax_success.set_title("Delivery", fontsize=9.3, fontweight="bold", pad=2)
    ax_latency.set_title("Successful-reply RTT", fontsize=9.3, fontweight="bold", pad=2)

    handles = [
        Line2D([0], [0], color="#D55E00", marker="D", linewidth=1.3, label="RTT range"),
        Line2D([0], [0], color="#94a3ad", linestyle="--", linewidth=0.85, label="100 ms"),
    ]
    ax_latency.legend(handles=handles, loc="lower left", frameon=False, fontsize=6.8)
    fig.subplots_adjust(left=0.15, right=0.98, top=0.94, bottom=0.17)
    fig.savefig(FIG_DIR / "edge4av_mocar_reliability.pdf")
    fig.savefig(FIG_DIR / "edge4av_mocar_reliability.png", dpi=300)
    plt.close(fig)


def short_group(label: str) -> str:
    replacements = {
        "SPaT/state mirror": "SPaT mirror",
        "Compact service <=4 KiB, excluding E11": "Compact <=4 KiB",
        "E06 load/QoS 1 KiB": "QoS label only, 1 KiB",
        "E09 detector-output replay": "Detector replay",
        "E11 multiclient 1 KiB": "Multiclient 1 KiB",
        "Mid payload 8-64 KiB": "Mid 8-64 KiB",
        "Map/perception 128-512 KiB": "Map/perception",
        "Bulk >=1 MiB": "Bulk >=1 MiB",
    }
    return replacements.get(label, label)


def plot_deadline_envelope(rows: list[dict[str, float | str]]) -> None:
    rows = list(rows)
    labels = [short_group(str(row["group"])) for row in rows]
    y = list(range(len(rows)))

    fig, (ax_rtt, ax_miss) = plt.subplots(
        1,
        2,
        figsize=(7.05, 3.55),
        gridspec_kw={"width_ratios": [1.05, 1.0], "wspace": 0.18},
    )

    for threshold in [100, 500, 1000]:
        ax_rtt.axvline(threshold, color="#94a3ad", linewidth=0.8, linestyle="--", zorder=0)

    for idx, row in enumerate(rows):
        p50 = float(row["p50_ms"])
        p95 = float(row["p95_ms"])
        p99 = float(row["p99_ms"])
        ax_rtt.hlines(idx, p50, p99, color="#9fb3bf", linewidth=2.0, zorder=1)
        ax_rtt.plot(p50, idx, "o", color="#0072B2", markersize=4.8, zorder=2)
        ax_rtt.plot(p95, idx, "D", color="#D55E00", markersize=4.3, zorder=3)
        ax_rtt.plot(p99, idx, "s", color="#6f42c1", markersize=4.1, zorder=2)

    ax_rtt.set_xscale("log")
    ax_rtt.set_xlim(20, 7000)
    ax_rtt.set_yticks(y)
    ax_rtt.set_yticklabels(labels)
    ax_rtt.invert_yaxis()
    ax_rtt.set_xlabel("RTT over accepted replies (ms, log scale)")
    ax_rtt.set_title("Latency envelope", fontsize=10, fontweight="bold")
    ax_rtt.grid(True, axis="x", which="major", color="#d7dee3", linewidth=0.7)
    ax_rtt.grid(True, axis="x", which="minor", color="#edf1f4", linewidth=0.4)
    ax_rtt.spines["top"].set_visible(False)
    ax_rtt.spines["right"].set_visible(False)

    bar_height = 0.22
    offsets = [-bar_height, 0, bar_height]
    miss_specs = [
        ("miss_100_pct", "miss @100 ms", "#b2182b"),
        ("miss_500_pct", "miss @500 ms", "#ef8a62"),
        ("miss_1000_pct", "miss @1000 ms", "#67a9cf"),
    ]
    for offset, (key, label, color) in zip(offsets, miss_specs):
        ax_miss.barh(
            [idx + offset for idx in y],
            [float(row[key]) for row in rows],
            height=bar_height * 0.9,
            color=color,
            label=label,
        )
    ax_miss.set_xlim(0, 100)
    ax_miss.set_yticks([])
    ax_miss.invert_yaxis()
    ax_miss.set_xlabel("deadline miss rate (% of attempts)")
    ax_miss.set_title("Deadline misses", fontsize=10, fontweight="bold")
    ax_miss.grid(True, axis="x", color="#d7dee3", linewidth=0.7)
    ax_miss.spines["top"].set_visible(False)
    ax_miss.spines["right"].set_visible(False)
    ax_miss.legend(loc="upper right", frameon=False, fontsize=7.2)

    handles = [
        Line2D([0], [0], marker="o", color="none", markerfacecolor="#0072B2", label="p50", markersize=5),
        Line2D([0], [0], marker="D", color="none", markerfacecolor="#D55E00", label="p95", markersize=4.8),
        Line2D([0], [0], marker="s", color="none", markerfacecolor="#6f42c1", label="p99", markersize=4.7),
        Line2D([0], [0], color="#9fb3bf", linewidth=2, label="p50-p99 span"),
        Line2D([0], [0], color="#94a3ad", linestyle="--", linewidth=0.8, label="100/500/1000 ms"),
    ]
    ax_rtt.legend(handles=handles, loc="upper right", frameon=False, fontsize=7.2)

    fig.subplots_adjust(left=0.205, right=0.985, top=0.88, bottom=0.15)
    fig.savefig(FIG_DIR / "edge4av_deadline_envelope.pdf")
    fig.savefig(FIG_DIR / "edge4av_deadline_envelope.png", dpi=300)
    plt.close(fig)


def plot_multiclient(rows: list[dict[str, float | str]]) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(7.05, 3.35), sharey=True)
    condition_titles = {
        "good-signal": "Baseline stationary run",
        "weak-signal": "Weak signal stationary run",
    }

    for ax, condition in zip(axes, ["good-signal", "weak-signal"]):
        condition_rows = [row for row in rows if row["condition"] == condition]
        for transport in ["TCP", "UDP", "MQTT"]:
            transport_rows = sorted(
                [row for row in condition_rows if row["transport"] == transport],
                key=lambda row: int(row["clients"]),
            )
            clients = [int(row["clients"]) for row in transport_rows]
            p95 = [float(row["p95_ms"]) for row in transport_rows]
            success = [float(row["success_pct"]) for row in transport_rows]
            ax.plot(
                clients,
                p95,
                color=TRANSPORT_COLOR[transport],
                marker=TRANSPORT_MARKER[transport],
                linewidth=1.6,
                markersize=4.6,
                label=f"{transport} p95",
            )

        for threshold, label in [(100, "100 ms"), (500, "500 ms"), (1000, "1000 ms")]:
            ax.axhline(threshold, color="#94a3ad", linewidth=0.75, linestyle="--", zorder=0)
            ax.text(1.05, threshold * 1.05, label, fontsize=7, color="#4b5963")

        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_xticks([1, 2, 5, 10, 20, 50, 100])
        ax.get_xaxis().set_major_formatter(plt.ScalarFormatter())
        ax.set_xlim(0.85, 120)
        ax.set_ylim(25, 8000)
        ax.set_title(condition_titles[condition], fontsize=10, fontweight="bold")
        ax.set_xlabel("simultaneous clients")
        ax.grid(True, axis="y", which="major", color="#d7dee3", linewidth=0.7)
        ax.grid(True, axis="y", which="minor", color="#edf1f4", linewidth=0.4)
        ax.spines["top"].set_visible(False)
        ax.spines["right"].set_visible(False)

    axes[0].set_ylabel("p95 RTT (ms, log scale)")
    axes[1].legend(loc="upper left", frameon=False, fontsize=7.7)
    fig.subplots_adjust(left=0.08, right=0.985, top=0.89, bottom=0.15, wspace=0.08)
    fig.savefig(FIG_DIR / "edge4av_multiclient_context.pdf")
    fig.savefig(FIG_DIR / "edge4av_multiclient_context.png", dpi=300)
    plt.close(fig)


def plot_failure_recovery(rows: list[dict[str, float | str]]) -> None:
    labels = []
    for row in rows:
        mode = "recv" if str(row["failure_mode"]).startswith("Receiver") else "broker"
        labels.append(f"{row['transport']}\n{mode}")

    x = list(range(len(rows)))
    gaps_s = [float(row["success_gap_ms"]) / 1000.0 for row in rows]
    success_pct = [float(row["success_pct"]) for row in rows]

    fig, ax_gap = plt.subplots(figsize=(3.45, 2.25))
    bars = ax_gap.bar(x, gaps_s, color="#b2182b", width=0.62)
    ax_gap.axhline(0.5, color="#94a3ad", linestyle="--", linewidth=0.75)
    ax_gap.axhline(1.0, color="#94a3ad", linestyle=":", linewidth=0.75)
    ax_gap.set_ylabel("inter-success gap (s)")
    ax_gap.set_ylim(0, max(gaps_s) * 1.28)
    ax_gap.set_xticks(x)
    ax_gap.set_xticklabels(labels)
    ax_gap.grid(True, axis="y", color="#d7dee3", linewidth=0.7)
    ax_gap.spines["top"].set_visible(False)
    ax_gap.spines["right"].set_visible(False)

    for bar, gap, pct in zip(bars, gaps_s, success_pct):
        ax_gap.text(
            bar.get_x() + bar.get_width() / 2,
            gap + 0.35,
            f"{gap:.1f}s\n{pct:.1f}%",
            ha="center",
            va="bottom",
            fontsize=7.3,
            color="#5b1a1a",
        )

    handles = [
        Line2D([0], [0], color="#b2182b", linewidth=6, label="maximum gap"),
        Line2D([0], [0], color="#94a3ad", linestyle="--", linewidth=0.75, label="500 ms / 1 s"),
    ]
    ax_gap.legend(handles=handles, loc="upper left", frameon=False, fontsize=6.9)
    fig.subplots_adjust(left=0.16, right=0.98, top=0.98, bottom=0.20)
    fig.savefig(FIG_DIR / "edge4av_failure_recovery.pdf")
    fig.savefig(FIG_DIR / "edge4av_failure_recovery.png", dpi=300)
    plt.close(fig)


def main() -> int:
    plt.rcParams.update(
        {
            "font.family": "DejaVu Sans",
            "font.size": 8.5,
            "axes.labelsize": 8.8,
            "xtick.labelsize": 8.0,
            "ytick.labelsize": 8.0,
            "legend.fontsize": 7.8,
            "pdf.fonttype": 42,
            "ps.fonttype": 42,
        }
    )
    FIG_DIR.mkdir(parents=True, exist_ok=True)

    deadline_rows = load_deadline_rows()
    multiclient_rows = load_multiclient_rows()
    failure_rows = load_failure_rows()
    mocar_rows = load_mocar_rows()
    write_csv(FIG_DIR / "edge4av_deadline_envelope_data.csv", deadline_rows)
    write_csv(FIG_DIR / "edge4av_multiclient_context_data.csv", multiclient_rows)
    write_csv(FIG_DIR / "edge4av_failure_recovery_data.csv", failure_rows)
    write_csv(FIG_DIR / "edge4av_mocar_reliability_data.csv", mocar_rows)
    plot_measured_paths()
    plot_mocar_reliability(mocar_rows)
    plot_deadline_envelope(deadline_rows)
    plot_multiclient(multiclient_rows)
    plot_failure_recovery(failure_rows)

    for path in [
        FIG_DIR / "edge4av_measured_paths.pdf",
        FIG_DIR / "edge4av_mocar_reliability.pdf",
        FIG_DIR / "edge4av_deadline_envelope.pdf",
        FIG_DIR / "edge4av_multiclient_context.pdf",
        FIG_DIR / "edge4av_failure_recovery.pdf",
        FIG_DIR / "edge4av_mocar_reliability_data.csv",
        FIG_DIR / "edge4av_deadline_envelope_data.csv",
        FIG_DIR / "edge4av_multiclient_context_data.csv",
        FIG_DIR / "edge4av_failure_recovery_data.csv",
    ]:
        print(path.relative_to(ROOT))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
