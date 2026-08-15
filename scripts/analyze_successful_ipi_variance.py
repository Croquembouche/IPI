#!/usr/bin/env python3
"""Analyze RTT variance conditional on a successful IPI attempt.

The analysis reuses the condition inventory from analyze_cav_limiting_factors.py
and returns to the retained sender records for individual successful RTTs.
Conditions with at least 20 finite successful RTTs are retained, matching the
stability threshold used by the condition-level PCA.

Each condition receives equal weight.  Because RTT spans multiple orders of
magnitude, the primary variance decomposition uses log10 RTT.  It separates
the mean within-condition variance from the variance of condition means.  Raw
millisecond-squared variance, coefficient of variation, and p95/p50 dispersion
are also retained so the result can be audited without relying on one scale.
"""

from __future__ import annotations

import csv
import importlib.util
import json
import math
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "paper" / "analysis" / "limiting_factors"
TRUE = {"true", "1", "yes", "accepted", "ok"}


def load_limiting_factor_module():
    path = ROOT / "scripts" / "analyze_cav_limiting_factors.py"
    spec = importlib.util.spec_from_file_location("ipi_limiting_factors", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot load {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def source_files_for(row):
    if row["family"] == "airspan_followup":
        sample_id = row["condition"].lower()
        condition_name, repetition_name, transport = sample_id.split("-")
        sender_dir = (
            ROOT
            / "results"
            / "real_5g"
            / row["run"]
            / "application"
            / condition_name
            / f"rep_{repetition_name.removeprefix('rep')}"
            / transport
            / "senders"
        )
        return sorted(sender_dir.glob(f"{sample_id}_veh-*_sender.csv"))

    if row["path"] == "PC5 direct":
        run_dir = ROOT / "results" / "mocar_v2x" / row["run"]
        if row["family"] in ("pc5_stationary_reference", "pc5_stationary"):
            return [
                run_dir
                / "remote_obu"
                / "obu"
                / f"payload_{int(row['payload_bytes'])}.csv"
            ]
        if row["family"] == "pc5_mobility":
            return sorted((run_dir / "remote_obu" / "obu").glob("*.csv"))

    return [ROOT / path for path in row["source_files"].split(";") if path]


def read_successful_rtts(files, condition_id=None):
    values = []
    for file in files:
        if not file.exists() or file.stat().st_size == 0:
            continue
        with file.open(newline="", errors="replace") as handle:
            for row in csv.DictReader(handle):
                observed_condition = row.get("condition_id")
                if condition_id and observed_condition and observed_condition != condition_id:
                    continue
                accepted = str(
                    row.get("accepted", row.get("success", ""))
                ).strip().lower()
                try:
                    rtt = float(row.get("rtt_ms", ""))
                except (TypeError, ValueError):
                    continue
                if accepted in TRUE and math.isfinite(rtt) and rtt > 0:
                    values.append(rtt)
    return values


def percentile_summary(values):
    array = np.asarray(values, dtype=float)
    percentiles = np.percentile(array, [0, 25, 50, 75, 90, 95, 100])
    return {
        name: float(value)
        for name, value in zip(
            ("minimum", "p25", "median", "p75", "p90", "p95", "maximum"),
            percentiles,
        )
    }


def equal_condition_decomposition(rows, prefix):
    within = float(np.mean([row[f"{prefix}_population_variance"] for row in rows]))
    between = float(np.var([row[f"{prefix}_mean"] for row in rows]))
    total = within + between
    return {
        "within_condition_variance": within,
        "between_condition_variance": between,
        "total_variance": total,
        "within_condition_fraction": within / total,
        "between_condition_fraction": between / total,
    }


def path_summary(rows):
    return {
        "conditions": len(rows),
        "successful_rtt_records": int(sum(row["successful_rtt_records"] for row in rows)),
        "coefficient_of_variation": percentile_summary(
            [row["rtt_coefficient_of_variation"] for row in rows]
        ),
        "p95_p50_ratio": percentile_summary([row["rtt_p95_p50_ratio"] for row in rows]),
        "standard_deviation_ms": percentile_summary([row["rtt_stddev_ms"] for row in rows]),
        "raw_rtt_equal_condition_decomposition": equal_condition_decomposition(rows, "rtt"),
        "log10_rtt_equal_condition_decomposition": equal_condition_decomposition(rows, "log10_rtt"),
    }


def collect_condition_statistics():
    limiting_factors = load_limiting_factor_module()
    conditions = (
        limiting_factors.collect_regular_5g()
        + limiting_factors.collect_airspan()
        + limiting_factors.collect_pc5()
    )
    conditions.sort(key=lambda row: (row["path"], row["family"], row["run"], row["condition"]))

    statistics = []
    mismatches = []
    missing_sources = []
    for condition in conditions:
        files = [file for file in source_files_for(condition) if file.exists()]
        if not files:
            missing_sources.append(
                {"run": condition["run"], "condition": condition["condition"]}
            )
            continue

        filter_condition = condition["condition"] if condition["path"] == "5G Uu" else None
        if condition["family"] == "airspan_followup":
            filter_condition = filter_condition.lower()
        rtts = read_successful_rtts(files, filter_condition)
        if len(rtts) != condition["accepted"]:
            mismatches.append(
                {
                    "family": condition["family"],
                    "run": condition["run"],
                    "condition": condition["condition"],
                    "accepted_rows": int(condition["accepted"]),
                    "finite_successful_rtt_rows": len(rtts),
                }
            )
        if len(rtts) < 20:
            continue

        values = np.asarray(rtts, dtype=float)
        log_values = np.log10(values)
        p50, p95 = np.percentile(values, [50, 95])
        statistics.append(
            {
                "path": condition["path"],
                "family": condition["family"],
                "run": condition["run"],
                "condition": condition["condition"],
                "transport": condition["transport"],
                "context": condition["context"],
                "payload_bytes": condition["payload_bytes"],
                "clients": condition["clients"],
                "attempts": condition["attempts"],
                "accepted": condition["accepted"],
                "successful_rtt_records": len(values),
                "rtt_mean": float(values.mean()),
                "rtt_sample_variance": float(values.var(ddof=1)),
                "rtt_population_variance": float(values.var()),
                "rtt_stddev_ms": float(values.std(ddof=1)),
                "rtt_coefficient_of_variation": float(values.std(ddof=1) / values.mean()),
                "rtt_p95_p50_ratio": float(p95 / p50),
                "log10_rtt_mean": float(log_values.mean()),
                "log10_rtt_sample_variance": float(log_values.var(ddof=1)),
                "log10_rtt_population_variance": float(log_values.var()),
                "source_files": ";".join(
                    str(file.relative_to(ROOT)) for file in files
                ),
            }
        )
    return conditions, statistics, missing_sources, mismatches


def write_csv(path, rows):
    fields = list(rows[0]) if rows else []
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    conditions, statistics, missing_sources, mismatches = collect_condition_statistics()
    if not statistics:
        raise RuntimeError("No successful RTT records were available for analysis")

    all_summary = path_summary(statistics)
    by_path = {
        path: path_summary([row for row in statistics if row["path"] == path])
        for path in sorted({row["path"] for row in statistics})
    }
    by_family = {
        family: path_summary([row for row in statistics if row["family"] == family])
        for family in sorted({row["family"] for row in statistics})
    }
    summary = {
        "definition": (
            "RTT variance conditional on an accepted IPI response with a finite positive sender-side RTT"
        ),
        "analysis_unit": (
            "one experimental condition, weighted equally; individual RTT records are used within each condition"
        ),
        "minimum_successful_rtts_per_condition": 20,
        "condition_inventory": len(conditions),
        "eligible_conditions": len(statistics),
        "successful_rtt_records": int(
            sum(row["successful_rtt_records"] for row in statistics)
        ),
        "accepted_rows_in_eligible_conditions": int(
            sum(row["accepted"] for row in statistics)
        ),
        "missing_source_conditions": missing_sources,
        "accepted_rtt_count_mismatches": mismatches,
        "all_conditions": all_summary,
        "by_path": by_path,
        "by_family": by_family,
        "interpretation_boundary": (
            "The result conditions on successful responses. Complete and near-complete outages are excluded by the 20-response threshold and must be assessed with accepted delivery and deadline availability."
        ),
    }

    write_csv(OUT / "successful_attempt_variance.csv", statistics)
    (OUT / "successful_attempt_variance_summary.json").write_text(
        json.dumps(summary, indent=2) + "\n"
    )

    overall = summary["all_conditions"]
    log_decomposition = overall["log10_rtt_equal_condition_decomposition"]
    five_g = summary["by_path"]["5G Uu"]
    pc5 = summary["by_path"]["PC5 direct"]
    report = f"""# Successful IPI Attempt RTT Variance

## Result

The analysis includes {summary['successful_rtt_records']:,} finite RTT records from successful IPI attempts in {summary['eligible_conditions']} experimental conditions. Each condition has at least 20 successful responses and receives equal weight.

Because RTT spans multiple orders of magnitude, the primary variance decomposition uses log10 RTT. Differences between condition means explain {100 * log_decomposition['between_condition_fraction']:.1f}% of the successful-attempt variance, while attempt-to-attempt variation within a fixed condition explains {100 * log_decomposition['within_condition_fraction']:.1f}%. The successful-attempt latency regime is therefore set mainly by the experimental condition rather than by ordinary jitter within one condition.

Across conditions, the median coefficient of variation is {overall['coefficient_of_variation']['median']:.3f}, and the median p95/p50 ratio is {overall['p95_p50_ratio']['median']:.3f}. The 95th-percentile condition has a p95/p50 ratio of {overall['p95_p50_ratio']['p95']:.3f}, and the maximum is {overall['p95_p50_ratio']['maximum']:.3f}, so a successful response can still have a highly variable completion time.

The median p95/p50 ratio is {five_g['p95_p50_ratio']['median']:.3f} across {five_g['conditions']} private-5G conditions and {pc5['p95_p50_ratio']['median']:.3f} across {pc5['conditions']} direct-PC5 conditions. This comparison is conditional on success: PC5 conditions with fewer than 20 replies, including complete and near-complete outages, are not part of the variance calculation.

## Method boundary

The result does not replace accepted delivery or deadline availability. It answers a separate question: when an IPI attempt succeeds, how predictable is its RTT? Raw millisecond-squared variance is retained in the CSV and JSON, but log10 RTT is used for the primary decomposition because a small number of seconds-scale tails otherwise dominate the scale.
"""
    (OUT / "successful_attempt_variance.md").write_text(report)

    print(
        json.dumps(
            {
                "eligible_conditions": summary["eligible_conditions"],
                "successful_rtt_records": summary["successful_rtt_records"],
                "between_condition_fraction_log10": log_decomposition[
                    "between_condition_fraction"
                ],
                "within_condition_fraction_log10": log_decomposition[
                    "within_condition_fraction"
                ],
                "median_coefficient_of_variation": overall[
                    "coefficient_of_variation"
                ]["median"],
                "median_p95_p50_ratio": overall["p95_p50_ratio"]["median"],
                "output": str(OUT),
            },
            indent=2,
        )
    )


if __name__ == "__main__":
    main()
