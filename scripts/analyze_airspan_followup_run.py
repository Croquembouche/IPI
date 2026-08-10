#!/usr/bin/env python3
"""Build validated application, deadline, GNSS, load, and fairness summaries.

The analyzer treats every sender row as an attempt. An unaccepted row or an
accepted row whose RTT exceeds a deadline is a deadline miss. RTT percentiles
use accepted rows with a recorded RTT only.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import re
import statistics
import subprocess
from collections import defaultdict
from datetime import datetime
from pathlib import Path
from typing import Any, Iterable


DEADLINES_MS = (100, 120, 400, 500, 1000, 5000)
TRANSPORTS = ("tcp", "mqtt", "udp")
REPETITIONS = (1, 2)


def percentile(values: list[float], fraction: float) -> float | None:
    if not values:
        return None
    ordered = sorted(values)
    index = max(0, min(math.ceil(fraction * len(ordered)) - 1, len(ordered) - 1))
    return ordered[index]


def parse_time(value: str) -> datetime:
    return datetime.fromisoformat(value)


def iso_to_ns(value: str) -> int:
    return int(parse_time(value).timestamp() * 1_000_000_000)


def as_float(value: str | None) -> float | None:
    if value in (None, ""):
        return None
    return float(value)


def format_number(value: Any, digits: int = 9) -> str:
    if value is None:
        return ""
    if isinstance(value, float):
        formatted = f"{value:.{digits}f}".rstrip("0").rstrip(".")
        return "0" if formatted == "-0" else formatted
    return str(value)


def jain_fairness(values: Iterable[float]) -> float | None:
    items = list(values)
    if not items:
        return None
    denominator = len(items) * sum(value * value for value in items)
    if denominator == 0:
        return None
    return (sum(items) ** 2) / denominator


def base_stats() -> dict[str, Any]:
    return {
        "attempts": 0,
        "accepted": 0,
        "failed": 0,
        "rtts": [],
        "failure_details": {},
        "deadline_hits": {deadline: 0 for deadline in DEADLINES_MS},
    }


def consume_row(stats: dict[str, Any], row: dict[str, str]) -> None:
    stats["attempts"] += 1
    accepted = row.get("accepted", "").lower() == "true"
    rtt = as_float(row.get("rtt_ms"))
    if accepted:
        stats["accepted"] += 1
        if rtt is not None:
            stats["rtts"].append(rtt)
            for deadline in DEADLINES_MS:
                if rtt <= deadline:
                    stats["deadline_hits"][deadline] += 1
    else:
        stats["failed"] += 1
        detail = row.get("detail", "").strip() or "unspecified"
        stats["failure_details"][detail] = stats["failure_details"].get(detail, 0) + 1


def finalize_stats(stats: dict[str, Any]) -> dict[str, Any]:
    attempts = stats["attempts"]
    rtts = stats["rtts"]
    result = {
        "attempts": attempts,
        "accepted": stats["accepted"],
        "failed": stats["failed"],
        "success_rate_pct": 100.0 * stats["accepted"] / attempts if attempts else 0.0,
        "rtt_count": len(rtts),
        "rtt_p50_ms": percentile(rtts, 0.50),
        "rtt_p95_ms": percentile(rtts, 0.95),
        "rtt_p99_ms": percentile(rtts, 0.99),
        "rtt_max_ms": max(rtts) if rtts else None,
        "failure_detail_counts": dict(sorted(stats["failure_details"].items())),
    }
    for deadline in DEADLINES_MS:
        hits = stats["deadline_hits"][deadline]
        result[f"deadline_hits_{deadline}ms"] = hits
        result[f"deadline_hit_rate_{deadline}ms_pct"] = (
            100.0 * hits / attempts if attempts else 0.0
        )
    return result


def merge_stats(target: dict[str, Any], source: dict[str, Any]) -> None:
    for key in ("attempts", "accepted", "failed"):
        target[key] += source[key]
    target["rtts"].extend(source["rtts"])
    for detail, count in source["failure_details"].items():
        target["failure_details"][detail] = target["failure_details"].get(detail, 0) + count
    for deadline in DEADLINES_MS:
        target["deadline_hits"][deadline] += source["deadline_hits"][deadline]


def summarize_senders(paths: list[Path]) -> tuple[dict[str, Any], list[dict[str, Any]], dict[str, Any]]:
    aggregate = base_stats()
    per_client: list[dict[str, Any]] = []
    for path in paths:
        client = base_stats()
        with path.open("r", encoding="utf-8", newline="") as handle:
            for row in csv.DictReader(handle):
                consume_row(aggregate, row)
                consume_row(client, row)
        client_id = path.stem.removesuffix("_sender").rsplit("_", 1)[-1]
        per_client.append({"client_id": client_id, **finalize_stats(client)})
    return finalize_stats(aggregate), per_client, aggregate


def gnss_summary(path: Path, start_ns: int, finish_ns: int) -> dict[str, Any]:
    result = {
        "gnss_rows_total": 0,
        "gnss_rows_in_workload": 0,
        "gnss_position_rows_in_workload": 0,
        "gnss_latitude_median_deg": None,
        "gnss_longitude_median_deg": None,
        "gnss_latitude_min_deg": None,
        "gnss_latitude_max_deg": None,
        "gnss_longitude_min_deg": None,
        "gnss_longitude_max_deg": None,
    }
    if not path.is_file():
        return result
    latitudes: list[float] = []
    longitudes: list[float] = []
    with path.open("r", encoding="utf-8", newline="") as handle:
        for row in csv.DictReader(handle):
            result["gnss_rows_total"] += 1
            timestamp = int(row["system_time_ns"])
            if timestamp < start_ns or timestamp > finish_ns:
                continue
            result["gnss_rows_in_workload"] += 1
            latitude = as_float(row.get("latitude_deg"))
            longitude = as_float(row.get("longitude_deg"))
            if latitude is None or longitude is None:
                continue
            latitudes.append(latitude)
            longitudes.append(longitude)
    result["gnss_position_rows_in_workload"] = len(latitudes)
    if latitudes:
        result.update(
            {
                "gnss_latitude_median_deg": statistics.median(latitudes),
                "gnss_longitude_median_deg": statistics.median(longitudes),
                "gnss_latitude_min_deg": min(latitudes),
                "gnss_latitude_max_deg": max(latitudes),
                "gnss_longitude_min_deg": min(longitudes),
                "gnss_longitude_max_deg": max(longitudes),
            }
        )
    return result


def interface_summary(path: Path, start_ns: int, finish_ns: int) -> dict[str, Any]:
    result = {
        "host_telemetry_rows_in_workload": 0,
        "interface_elapsed_s": None,
        "interface_tx_bytes": None,
        "interface_rx_bytes": None,
        "interface_tx_mbps": None,
        "interface_tx_errors_delta": None,
        "interface_tx_drops_delta": None,
        "interface_rx_errors_delta": None,
        "interface_rx_drops_delta": None,
    }
    if not path.is_file():
        return result
    first: dict[str, Any] | None = None
    last: dict[str, Any] | None = None
    with path.open("r", encoding="utf-8") as handle:
        for line in handle:
            sample = json.loads(line)
            timestamp = int(sample["wall_time_epoch_ns"])
            if timestamp < start_ns or timestamp > finish_ns:
                continue
            interface = sample.get("network", {}).get("eno2")
            if interface is None:
                continue
            point = {"timestamp": timestamp, **interface}
            if first is None:
                first = point
            last = point
            result["host_telemetry_rows_in_workload"] += 1
    if first is None or last is None or last["timestamp"] <= first["timestamp"]:
        return result
    elapsed = (last["timestamp"] - first["timestamp"]) / 1_000_000_000
    tx_bytes = last["tx_bytes"] - first["tx_bytes"]
    rx_bytes = last["rx_bytes"] - first["rx_bytes"]
    result.update(
        {
            "interface_elapsed_s": elapsed,
            "interface_tx_bytes": tx_bytes,
            "interface_rx_bytes": rx_bytes,
            "interface_tx_mbps": tx_bytes * 8.0 / elapsed / 1_000_000,
            "interface_tx_errors_delta": last["tx_errors"] - first["tx_errors"],
            "interface_tx_drops_delta": last["tx_drops"] - first["tx_drops"],
            "interface_rx_errors_delta": last["rx_errors"] - first["rx_errors"],
            "interface_rx_drops_delta": last["rx_drops"] - first["rx_drops"],
        }
    )
    return result


def load_report(path: Path) -> dict[str, Any]:
    result = {
        "load_report_present": False,
        "load_target_mbps": None,
        "load_report_mbps": None,
        "load_report_bytes": None,
        "load_report_elapsed_s": None,
    }
    if not path.is_file() or path.stat().st_size == 0:
        return result
    with path.open("r", encoding="utf-8", newline="") as handle:
        rows = list(csv.DictReader(handle))
    if len(rows) != 1:
        return result
    row = rows[0]
    result.update(
        {
            "load_report_present": True,
            "load_target_mbps": as_float(row.get("target_mbps")),
            "load_report_mbps": as_float(row.get("throughput_mbps")),
            "load_report_bytes": int(row["bytes"]),
            "load_report_elapsed_s": as_float(row.get("elapsed_s")),
        }
    )
    return result


def write_csv(path: Path, rows: list[dict[str, Any]], fields: list[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow({field: format_number(row.get(field)) for field in fields})


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def git_commit(repo_root: Path) -> str:
    result = subprocess.run(
        ["git", "rev-parse", "HEAD"],
        cwd=repo_root,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("run_root", type=Path)
    parser.add_argument("--conditions", nargs="+")
    args = parser.parse_args()

    run_root = args.run_root.resolve()
    repo_root = Path(__file__).resolve().parents[1]
    analysis_dir = run_root / "analysis"
    operator_context = json.loads((run_root / "operator_context.json").read_text(encoding="utf-8"))
    corrected_signal_class = operator_context["location"]["signal_class"]
    physical_placement = operator_context["location"]["physical_placement"]
    coordinate_precision_deg = operator_context["location"].get(
        "repository_coordinate_precision_deg"
    )
    classification_correction = operator_context.get("classification_correction")
    conditions = (
        args.conditions
        or operator_context.get("collection_plan", {}).get("conditions")
        or operator_context["location"].get("applies_to_conditions")
        or ["C1", "C2", "C3", "C4"]
    )
    gnss_status = operator_context["location"].get("automatic_gnss_status", "unknown")
    gnss_initial_outage = "timed_out" in gnss_status or "unavailable" in gnss_status
    recovered_match = re.search(r"recovered_at_(.+)$", gnss_status)
    gnss_recovered_at = recovered_match.group(1) if recovered_match else None

    samples: list[dict[str, Any]] = []
    gnss_rows: list[dict[str, Any]] = []
    c4_clients: list[dict[str, Any]] = []
    fairness_rows: list[dict[str, Any]] = []
    group_stats: dict[tuple[str, str], dict[str, Any]] = defaultdict(base_stats)
    overall_stats = base_stats()
    missing_samples: list[str] = []
    validation_errors: list[str] = []
    raw_signal_classes: set[str] = set()
    raw_physical_placements: set[str] = set()

    for condition in conditions:
        condition_lower = condition.lower()
        for repetition in REPETITIONS:
            for transport in TRANSPORTS:
                transport_dir = run_root / "application" / condition_lower / f"rep_{repetition}" / transport
                sample_id = f"{condition}-rep{repetition}-{transport}"
                manifest_path = transport_dir / "run_manifest.json"
                validation_path = transport_dir / "validation_summary.json"
                if not manifest_path.is_file() or not validation_path.is_file():
                    missing_samples.append(sample_id)
                    continue
                manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
                validation = json.loads(validation_path.read_text(encoding="utf-8"))
                if manifest.get("signal_class"):
                    raw_signal_classes.add(manifest["signal_class"])
                raw_placement = manifest.get("physical_placement") or manifest.get(
                    "signal_placement"
                )
                if raw_placement:
                    raw_physical_placements.add(raw_placement)
                sender_paths = sorted((transport_dir / "senders").glob("*_sender.csv"))
                stats, clients, raw_stats = summarize_senders(sender_paths)
                merge_stats(group_stats[(condition, transport)], raw_stats)
                merge_stats(overall_stats, raw_stats)

                expected_rows = manifest["clients"] * manifest["count_per_client"]
                exact_files = len(sender_paths) == manifest["clients"]
                exact_rows = stats["attempts"] == expected_rows
                complete_files = validation.get("complete_files") == manifest["clients"]
                started = parse_time(manifest["workload_started_at"])
                finished = parse_time(manifest["workload_finished_at"])
                bin_start = parse_time(manifest["acp_bin_start"])
                bin_end = parse_time(manifest["acp_bin_end"])
                in_bin = bin_start <= started <= finished <= bin_end
                if not (exact_files and exact_rows and complete_files and in_bin):
                    validation_errors.append(sample_id)

                start_ns = iso_to_ns(manifest["workload_started_at"])
                finish_ns = iso_to_ns(manifest["workload_finished_at"])
                gnss = gnss_summary(transport_dir / "gps" / "gps_samples.csv", start_ns, finish_ns)
                interface = interface_summary(
                    transport_dir / "host_telemetry" / "car.jsonl", start_ns, finish_ns
                )
                load = load_report(transport_dir / "load_client.csv")
                sample = {
                    "sample_id": sample_id,
                    "condition": condition,
                    "repetition": repetition,
                    "transport": transport,
                    "corrected_signal_class": corrected_signal_class,
                    "physical_placement": physical_placement,
                    "payload_bytes": manifest["payload_bytes"],
                    "clients": manifest["clients"],
                    "offered_load_mbps": manifest["load_mbps"],
                    "count_per_client": manifest["count_per_client"],
                    "interval_ms": manifest["interval_ms"],
                    "acp_bin_start": manifest["acp_bin_start"],
                    "acp_bin_end": manifest["acp_bin_end"],
                    "workload_started_at": manifest["workload_started_at"],
                    "workload_finished_at": manifest["workload_finished_at"],
                    "application_sample_valid": exact_files and exact_rows and complete_files and in_bin,
                    **stats,
                    **gnss,
                    **interface,
                    **load,
                }
                samples.append(sample)
                gnss_rows.append(
                    {
                        key: sample[key]
                        for key in (
                            "sample_id",
                            "condition",
                            "repetition",
                            "transport",
                            "workload_started_at",
                            "workload_finished_at",
                            "gnss_rows_total",
                            "gnss_rows_in_workload",
                            "gnss_position_rows_in_workload",
                            "gnss_latitude_median_deg",
                            "gnss_longitude_median_deg",
                            "gnss_latitude_min_deg",
                            "gnss_latitude_max_deg",
                            "gnss_longitude_min_deg",
                            "gnss_longitude_max_deg",
                        )
                    }
                )

                if condition == "C4":
                    for client in clients:
                        c4_clients.append(
                            {
                                "sample_id": sample_id,
                                "repetition": repetition,
                                "transport": transport,
                                **client,
                            }
                        )
                    accepted_counts = [client["accepted"] for client in clients]
                    success_rates = [client["success_rate_pct"] for client in clients]
                    p50_values = [
                        client["rtt_p50_ms"] for client in clients if client["rtt_p50_ms"] is not None
                    ]
                    fairness_rows.append(
                        {
                            "sample_id": sample_id,
                            "repetition": repetition,
                            "transport": transport,
                            "clients": len(clients),
                            "attempts": stats["attempts"],
                            "accepted": stats["accepted"],
                            "failed": stats["failed"],
                            "success_rate_pct": stats["success_rate_pct"],
                            "jain_accepted_count": jain_fairness(accepted_counts),
                            "client_success_rate_min_pct": min(success_rates),
                            "client_success_rate_median_pct": statistics.median(success_rates),
                            "client_success_rate_max_pct": max(success_rates),
                            "client_rtt_p50_min_ms": min(p50_values),
                            "client_rtt_p50_median_ms": statistics.median(p50_values),
                            "client_rtt_p50_max_ms": max(p50_values),
                        }
                    )

    grouped: list[dict[str, Any]] = []
    for condition in conditions:
        for transport in TRANSPORTS:
            grouped.append(
                {
                    "condition": condition,
                    "transport": transport,
                    "corrected_signal_class": corrected_signal_class,
                    **finalize_stats(group_stats[(condition, transport)]),
                }
            )

    invalid_attempts: list[dict[str, Any]] = []
    for path in sorted((run_root / "application").glob("c*/rep_*/*attempt*")):
        validation_path = path / "validation_summary.json"
        manifest_path = path / "run_manifest.json"
        validation = json.loads(validation_path.read_text(encoding="utf-8")) if validation_path.is_file() else {}
        manifest = json.loads(manifest_path.read_text(encoding="utf-8")) if manifest_path.is_file() else {}
        reason = "wrapper_transition_missing_load_report" if "recorder_transition" in path.name else "incomplete_timeout"
        invalid_attempts.append(
            {
                "path": str(path.relative_to(run_root)),
                "condition": manifest.get("condition"),
                "repetition": manifest.get("repetition"),
                "transport": manifest.get("transport"),
                "attempts": validation.get("rows"),
                "accepted": validation.get("accepted"),
                "failed": validation.get("failed"),
                "reason": reason,
                "included_in_summary": False,
            }
        )

    overall = finalize_stats(overall_stats)
    gnss_full_samples = sum(row["gnss_position_rows_in_workload"] > 0 for row in samples)
    sample_count = len(samples)
    expected_sample_count = len(conditions) * len(REPETITIONS) * len(TRANSPORTS)
    application_matrix_passed = (
        sample_count == expected_sample_count and not missing_samples and not validation_errors
    )

    application_fields = [
        "sample_id", "condition", "repetition", "transport", "corrected_signal_class",
        "physical_placement", "payload_bytes", "clients", "offered_load_mbps",
        "count_per_client", "interval_ms", "acp_bin_start", "acp_bin_end",
        "workload_started_at", "workload_finished_at", "application_sample_valid",
        "attempts", "accepted", "failed", "success_rate_pct", "rtt_count",
        "rtt_p50_ms", "rtt_p95_ms", "rtt_p99_ms", "rtt_max_ms",
    ]
    for deadline in DEADLINES_MS:
        application_fields.extend(
            [f"deadline_hits_{deadline}ms", f"deadline_hit_rate_{deadline}ms_pct"]
        )
    application_fields.extend(
        [
            "load_report_present", "load_target_mbps", "load_report_mbps",
            "load_report_bytes", "load_report_elapsed_s", "interface_tx_mbps",
            "interface_tx_errors_delta", "interface_tx_drops_delta",
            "interface_rx_errors_delta", "interface_rx_drops_delta",
            "gnss_rows_in_workload", "gnss_position_rows_in_workload",
            "gnss_latitude_median_deg", "gnss_longitude_median_deg",
        ]
    )
    grouped_fields = [
        "condition", "transport", "corrected_signal_class", "attempts", "accepted",
        "failed", "success_rate_pct", "rtt_count", "rtt_p50_ms", "rtt_p95_ms",
        "rtt_p99_ms", "rtt_max_ms",
    ]
    for deadline in DEADLINES_MS:
        grouped_fields.extend(
            [f"deadline_hits_{deadline}ms", f"deadline_hit_rate_{deadline}ms_pct"]
        )
    client_fields = [
        "sample_id", "repetition", "transport", "client_id", "attempts", "accepted",
        "failed", "success_rate_pct", "rtt_count", "rtt_p50_ms", "rtt_p95_ms",
        "rtt_p99_ms", "rtt_max_ms",
    ]
    for deadline in DEADLINES_MS:
        client_fields.extend(
            [f"deadline_hits_{deadline}ms", f"deadline_hit_rate_{deadline}ms_pct"]
        )
    fairness_fields = [
        "sample_id", "repetition", "transport", "clients", "attempts", "accepted",
        "failed", "success_rate_pct", "jain_accepted_count",
        "client_success_rate_min_pct", "client_success_rate_median_pct",
        "client_success_rate_max_pct", "client_rtt_p50_min_ms",
        "client_rtt_p50_median_ms", "client_rtt_p50_max_ms",
    ]
    gnss_fields = list(gnss_rows[0]) if gnss_rows else []
    invalid_fields = list(invalid_attempts[0]) if invalid_attempts else ["path"]

    write_csv(analysis_dir / "application_summary.csv", samples, application_fields)
    write_csv(analysis_dir / "condition_transport_summary.csv", grouped, grouped_fields)
    write_csv(analysis_dir / "c4_per_client_summary.csv", c4_clients, client_fields)
    write_csv(analysis_dir / "c4_fairness_summary.csv", fairness_rows, fairness_fields)
    write_csv(analysis_dir / "gnss_summary.csv", gnss_rows, gnss_fields)
    write_csv(analysis_dir / "invalid_attempts.csv", invalid_attempts, invalid_fields)

    metadata_correction_applied = (
        raw_signal_classes != {corrected_signal_class}
        or raw_physical_placements != {physical_placement}
    )
    if metadata_correction_applied:
        metadata_correction = {
            "schema": "edge4av-airspan-metadata-correction-v1",
            "scope": "all application samples, preserved attempts, run manifests, and matrix schedule entries in this run",
            "raw_planned_fields": {
                "signal_class": sorted(raw_signal_classes),
                "physical_or_signal_placement": sorted(raw_physical_placements),
            },
            "operator_corrected_fields": {
                "signal_class": corrected_signal_class,
                "physical_placement": physical_placement,
            },
            "reason": (
                classification_correction.get("reason")
                if classification_correction
                else "The run-level operator context supersedes inconsistent per-transport collection-planning fields."
            ),
            "rule": "Use the operator-corrected fields for every analysis and comparison; retain raw manifest fields only as collection-history provenance.",
        }
        if classification_correction:
            metadata_correction["classification_evidence"] = classification_correction
        write_json(run_root / "metadata_corrections.json", metadata_correction)

    run_manifest = {
        "schema": "edge4av-airspan-followup-run-v1",
        "run_name": run_root.name,
        "git_commit": git_commit(repo_root),
        "collection_started_at": min(sample["acp_bin_start"] for sample in samples),
        "collection_finished_at": max(sample["workload_finished_at"] for sample in samples),
        "conditions": conditions,
        "repetitions": list(REPETITIONS),
        "transports": list(TRANSPORTS),
        "corrected_signal_class": corrected_signal_class,
        "physical_placement": physical_placement,
        "operator_reported_location": operator_context["location"],
        "gnss_coordinates_repository_precision_deg": coordinate_precision_deg,
        "timing_metric": "request_response_rtt",
        "clock_sync_state": "unsynced",
        "one_way_metrics_valid": False,
        "application_samples": sample_count,
        "application_attempts": overall["attempts"],
        "application_accepted": overall["accepted"],
        "application_failed": overall["failed"],
        "application_success_rate_pct": overall["success_rate_pct"],
        "preserved_invalid_attempts": len(invalid_attempts),
        "acp_mg52_state": "operator_verified_export_pending",
        "paper_facing_state": "blocked_pending_acp_mg52_alignment",
    }
    write_json(run_root / "run_manifest.json", run_manifest)

    validation_summary = {
        "schema": "edge4av-airspan-followup-validation-v1",
        "application_matrix_passed": application_matrix_passed,
        "expected_application_samples": expected_sample_count,
        "valid_application_samples": sample_count - len(validation_errors),
        "missing_samples": missing_samples,
        "validation_errors": validation_errors,
        "attempts": overall["attempts"],
        "accepted": overall["accepted"],
        "failed": overall["failed"],
        "success_rate_pct": overall["success_rate_pct"],
        "gnss_samples_with_in_window_positions": gnss_full_samples,
        "gnss_samples_without_in_window_positions": sample_count - gnss_full_samples,
        "gnss_initial_outage_recorded": gnss_initial_outage,
        "gnss_recovered_at": gnss_recovered_at,
        "gnss_coordinates_repository_precision_deg": coordinate_precision_deg,
        "preserved_invalid_attempts": len(invalid_attempts),
        "one_way_metrics_excluded": True,
        "metadata_correction_applied": metadata_correction_applied,
        "acp_mg52_artifacts_present": False,
        "paper_facing_passed": False,
        "paper_facing_blockers": [
            "ACP Cell 2 and MG52 radio exports have not yet been aligned to the declared bins.",
            (
                "The operator corrected this block from strong to medium/typical after inspecting matching RSRP values, but the supporting MG52 artifact has not yet been imported."
                if classification_correction
                else f"The operator-reported {corrected_signal_class} signal class still requires radio-metric confirmation."
            ),
        ],
        "known_anomalies": [],
    }
    if gnss_initial_outage:
        validation_summary["known_anomalies"].append(
            f"Automatic GNSS was initially unavailable and later recovered at {gnss_recovered_at}; the operator-reported coordinate covers the fixed location."
        )
    if "C3" in conditions:
        c3_rep1_tcp = next(
            (sample for sample in samples if sample["sample_id"] == "C3-rep1-tcp"), None
        )
        if c3_rep1_tcp and not c3_rep1_tcp["load_report_present"]:
            validation_summary["known_anomalies"].append(
                "C3 repetition 1 TCP has no end-of-run load CSV; interface counters provide its achieved-uplink estimate."
            )
    if invalid_attempts:
        validation_summary["known_anomalies"].append(
            f"{len(invalid_attempts)} non-primary attempts are preserved and excluded from valid-repeat summaries."
        )
    write_json(run_root / "validation_summary.json", validation_summary)

    summary_json = {
        "schema": "edge4av-airspan-followup-analysis-v1",
        "run_manifest": run_manifest,
        "validation": validation_summary,
        "overall": overall,
        "condition_transport": grouped,
        "c4_fairness": fairness_rows,
        "invalid_attempts": invalid_attempts,
        "files": {
            "application_summary": "analysis/application_summary.csv",
            "condition_transport_summary": "analysis/condition_transport_summary.csv",
            "c4_per_client_summary": "analysis/c4_per_client_summary.csv",
            "c4_fairness_summary": "analysis/c4_fairness_summary.csv",
            "gnss_summary": "analysis/gnss_summary.csv",
            "invalid_attempts": "analysis/invalid_attempts.csv",
        },
    }
    write_json(analysis_dir / "summary.json", summary_json)

    signal_label = {
        "medium_typical_deployment": "Medium/Typical",
    }.get(corrected_signal_class, corrected_signal_class.replace("_", " ").title())
    signal_qualification = (
        "operator-corrected from a reported RSRP comparison; MG52 artifact import pending"
        if classification_correction
        else "operator-reported pending MG52 radio-export validation"
    )
    lines = [
        f"# Airspan Follow-Up: {signal_label} Location",
        "",
        f"- Application matrix: `{'passed' if application_matrix_passed else 'failed'}` ({sample_count}/{expected_sample_count} samples).",
        f"- Attempts: `{overall['attempts']}`; accepted: `{overall['accepted']}`; failed: `{overall['failed']}`; success: `{overall['success_rate_pct']:.3f}%`.",
        f"- Signal class: `{corrected_signal_class}` ({signal_qualification}).",
        f"- Physical placement: `{physical_placement}`.",
        "- Timing metric: request/response RTT; one-way fields are excluded because endpoint clocks were unsynchronized.",
        f"- Repository GNSS precision: `{coordinate_precision_deg}` degree; exact coordinates and rosbags remain only in the excluded raw backup.",
        "- Paper-facing status: pending ACP Cell 2 and MG52 radio-export alignment.",
        "",
        "## Condition/Transport Summary",
        "",
        "| condition | transport | attempts | accepted | success % | p50 ms | p95 ms | p99 ms | hit @100 ms % | hit @500 ms % |",
        "| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]
    for row in grouped:
        lines.append(
            f"| {row['condition']} | {row['transport'].upper()} | {row['attempts']} | {row['accepted']} | "
            f"{row['success_rate_pct']:.3f} | {format_number(row['rtt_p50_ms'], 3)} | "
            f"{format_number(row['rtt_p95_ms'], 3)} | {format_number(row['rtt_p99_ms'], 3)} | "
            f"{row['deadline_hit_rate_100ms_pct']:.3f} | {row['deadline_hit_rate_500ms_pct']:.3f} |"
        )
    failed_samples = [sample for sample in samples if sample["failed"]]
    if failed_samples:
        lines.extend(
            [
                "",
                "## Failure Observations",
                "",
                "| sample | failed/attempts | recorded detail |",
                "| --- | ---: | --- |",
            ]
        )
        for sample in failed_samples:
            details = "; ".join(
                f"{detail}: {count}"
                for detail, count in sample["failure_detail_counts"].items()
            ).replace("|", "\\|")
            lines.append(
                f"| {sample['sample_id']} | {sample['failed']}/{sample['attempts']} | {details} |"
            )
    if fairness_rows:
        lines.extend(
            [
                "",
                "## C4 Fairness",
                "",
                "| repetition | transport | accepted/attempts | success % | Jain accepted-count fairness | client success min-max % |",
                "| ---: | --- | ---: | ---: | ---: | ---: |",
            ]
        )
        for row in fairness_rows:
            lines.append(
                f"| {row['repetition']} | {row['transport'].upper()} | {row['accepted']}/{row['attempts']} | "
                f"{row['success_rate_pct']:.3f} | {row['jain_accepted_count']:.6f} | "
                f"{row['client_success_rate_min_pct']:.3f}-{row['client_success_rate_max_pct']:.3f} |"
            )
    lines.extend(["", "## Evidence Qualifications", ""])
    if metadata_correction_applied:
        lines.append(
            "- `metadata_corrections.json` supersedes inconsistent per-transport planning fields with the run-level operator context."
        )
    if gnss_initial_outage:
        lines.append(
            f"- Automatic GNSS initially failed and recovered at {gnss_recovered_at}; earlier samples use the operator-reported fixed location."
        )
    else:
        lines.append(
            f"- Automatic GNSS produced in-window positions for {gnss_full_samples}/{sample_count} valid samples."
        )
    lines.extend(
        [
            "- Repository-facing coordinates are rounded to 0.001 degree, and serialized rosbags are retained only in the excluded raw backup.",
            "- Deadline rates use every attempt as the denominator. Timeouts and other unaccepted rows are deadline misses.",
        ]
    )
    if invalid_attempts:
        lines.append(
            f"- {len(invalid_attempts)} incomplete or transition attempts remain preserved but are excluded from valid-repeat summaries."
        )
    lines.extend(
        [
            "- ACP Cell 2 and MG52 exports are still required before signal-related interpretation or paper-facing use.",
            "",
        ]
    )
    (analysis_dir / "summary.md").write_text("\n".join(lines), encoding="utf-8")

    print(json.dumps(validation_summary, indent=2, sort_keys=True))
    return 0 if application_matrix_passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
