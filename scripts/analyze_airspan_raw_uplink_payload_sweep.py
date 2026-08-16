#!/usr/bin/env python3
"""Validate and summarize the Airspan raw-byte uplink payload sweep.

The analyzer independently checks every sender and edge receiver row.  It
reports complete application-ACK RTT, exact accepted application bytes, and
per-condition host/interface health.  Cross-host wall clocks are never used
to derive latency.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import statistics
import subprocess
import zlib
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


TRANSPORTS = ("tcp", "mqtt")
PAYLOADS = (1024, 10240, 102400, 1048576, 2097152)
DEADLINES_MS = (100, 500, 1000)
EXPECTED_COUNT = 1000
EXPECTED_SENDER_DETAIL = "validated compact application acknowledgment"
EXPECTED_RECEIVER_DETAIL = "validated raw payload length and CRC32"


def percentile(values: list[float], fraction: float) -> float | None:
    if not values:
        return None
    ordered = sorted(values)
    return ordered[max(0, math.ceil(fraction * len(ordered)) - 1)]


def deterministic_crc32(size: int) -> int:
    pattern = bytes(range(251))
    payload = (pattern * ((size + len(pattern) - 1) // len(pattern)))[:size]
    return zlib.crc32(payload) & 0xFFFFFFFF


def load_csv(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        return list(csv.DictReader(handle))


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def format_number(value: Any, digits: int = 6) -> str:
    if value is None:
        return ""
    if isinstance(value, float):
        result = f"{value:.{digits}f}".rstrip("0").rstrip(".")
        return "0" if result == "-0" else result
    return str(value)


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def write_csv(path: Path, rows: list[dict[str, Any]], fields: list[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow({field: format_number(row.get(field)) for field in fields})


def git_commit(repo_root: Path) -> str:
    result = subprocess.run(
        ["git", "rev-parse", "HEAD"],
        cwd=repo_root,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


def telemetry_summary(path: Path, host_role: str) -> tuple[dict[str, Any], list[str]]:
    errors: list[str] = []
    samples: list[dict[str, Any]] = []
    if not path.is_file():
        return {"host_role": host_role, "telemetry_rows": 0}, [f"missing telemetry: {path}"]
    with path.open("r", encoding="utf-8") as handle:
        for line_number, line in enumerate(handle, start=1):
            if not line.strip():
                continue
            try:
                samples.append(json.loads(line))
            except json.JSONDecodeError as error:
                errors.append(f"invalid telemetry JSON at {path}:{line_number}: {error}")
    result: dict[str, Any] = {
        "host_role": host_role,
        "telemetry_rows": len(samples),
        "telemetry_elapsed_s": None,
        "cpu_busy_pct": None,
        "memory_used_pct_median": None,
        "memory_used_pct_max": None,
        "interface": None,
        "network_tx_bytes_delta": None,
        "network_rx_bytes_delta": None,
        "network_tx_mbps": None,
        "network_rx_mbps": None,
        "network_tx_errors_delta": None,
        "network_rx_errors_delta": None,
        "network_tx_drops_delta": None,
        "network_rx_drops_delta": None,
    }
    if len(samples) < 2:
        errors.append(f"telemetry has fewer than two rows: {path}")
        return result, errors

    first, last = samples[0], samples[-1]
    elapsed_s = (int(last["wall_time_epoch_ns"]) - int(first["wall_time_epoch_ns"])) / 1e9
    result["telemetry_elapsed_s"] = elapsed_s
    if elapsed_s <= 0:
        errors.append(f"telemetry has non-positive elapsed time: {path}")

    first_cpu = first.get("cpu_times_ticks", {}).get("cpu")
    last_cpu = last.get("cpu_times_ticks", {}).get("cpu")
    if first_cpu and last_cpu and len(first_cpu) == len(last_cpu):
        deltas = [int(end) - int(start) for start, end in zip(first_cpu, last_cpu)]
        total = sum(deltas)
        idle = deltas[3] + (deltas[4] if len(deltas) > 4 else 0)
        if total > 0:
            result["cpu_busy_pct"] = 100.0 * (total - idle) / total

    memory_used: list[float] = []
    for sample in samples:
        memory = sample.get("memory_kib", {})
        total = memory.get("MemTotal")
        available = memory.get("MemAvailable")
        if total and available is not None:
            memory_used.append(100.0 * (int(total) - int(available)) / int(total))
    if memory_used:
        result["memory_used_pct_median"] = statistics.median(memory_used)
        result["memory_used_pct_max"] = max(memory_used)

    first_network = first.get("network", {})
    last_network = last.get("network", {})
    common_interfaces = sorted(set(first_network) & set(last_network))
    if len(common_interfaces) != 1:
        errors.append(
            f"expected exactly one common telemetry interface at {path}, got {common_interfaces}"
        )
        return result, errors
    interface = common_interfaces[0]
    result["interface"] = interface
    start = first_network[interface]
    finish = last_network[interface]
    mappings = {
        "network_tx_bytes_delta": "tx_bytes",
        "network_rx_bytes_delta": "rx_bytes",
        "network_tx_errors_delta": "tx_errors",
        "network_rx_errors_delta": "rx_errors",
        "network_tx_drops_delta": "tx_drops",
        "network_rx_drops_delta": "rx_drops",
    }
    for output, source in mappings.items():
        delta = int(finish[source]) - int(start[source])
        result[output] = delta
        if delta < 0:
            errors.append(f"negative {source} counter delta at {path}")
    if elapsed_s > 0:
        result["network_tx_mbps"] = result["network_tx_bytes_delta"] * 8.0 / elapsed_s / 1e6
        result["network_rx_mbps"] = result["network_rx_bytes_delta"] * 8.0 / elapsed_s / 1e6
    return result, errors


def analyze_condition(
    run_root: Path,
    transport: str,
    payload: int,
    operator_context: dict[str, Any],
    serving_cell_correction: dict[str, Any] | None,
    tdd_profile_correction: dict[str, Any] | None,
) -> tuple[dict[str, Any], list[dict[str, Any]], list[str]]:
    condition_dir = run_root / "application" / transport / f"payload_{payload}"
    condition_name = f"{transport}-payload-{payload}"
    errors: list[str] = []
    required = {
        "complete marker": condition_dir / "complete.marker",
        "sender CSV": condition_dir / "sender.csv",
        "receiver CSV": condition_dir / "edge" / "logs" / "receiver.csv",
        "manifest": condition_dir / "run_manifest.json",
        "condition validation": condition_dir / "validation_summary.json",
        "sender status": condition_dir / "sender.exit_status",
    }
    for label, path in required.items():
        if not path.is_file():
            errors.append(f"{condition_name}: missing {label}: {path}")
    if errors:
        return {
            "condition": condition_name,
            "transport": transport,
            "application_payload_bytes": payload,
        }, [], errors

    manifest = load_json(required["manifest"])
    stored_validation = load_json(required["condition validation"])
    sender = load_csv(required["sender CSV"])
    receiver = load_csv(required["receiver CSV"])
    accepted_rows = [row for row in sender if row.get("accepted", "").lower() == "true"]
    failed_rows = [row for row in sender if row.get("accepted", "").lower() == "false"]
    expected_condition_id = f"uplink-{transport}-payload-{payload}"
    expected_crc = deterministic_crc32(payload)

    radio_context = operator_context["radio"]
    manifest_expected = {
        "status": "complete",
        "transport": transport,
        "condition_id": expected_condition_id,
        "application_payload_bytes": payload,
        "count": EXPECTED_COUNT,
        "interval_ms": 200,
        "timing_metric": "monotonic_complete_application_ack_rtt",
        "payload_protocol": "exact raw application body with length and CRC32 validation",
        "rsrp_dbm": operator_context["radio"]["rsrp_dbm"],
        "rsrq_db": operator_context["radio"]["rsrq_db"],
    }
    if tdd_profile_correction is None:
        manifest_expected["tdd_profile"] = operator_context["radio"]["tdd_profile"]
    else:
        previous_tdd_profile = tdd_profile_correction.get("previous_record", {}).get(
            "tdd_profile"
        )
        if manifest.get("tdd_profile") != previous_tdd_profile:
            errors.append(
                f"{condition_name}: acquisition manifest tdd_profile="
                f"{manifest.get('tdd_profile')!r}, expected correction "
                f"previous_record={previous_tdd_profile!r}"
            )
    # Acquisition-time manifests are immutable evidence.  When a later
    # serving-cell correction exists, validate the corrected context through
    # that explicit record instead of requiring the stale manifest value to
    # match it.
    if serving_cell_correction is None:
        manifest_expected["airspan_cell"] = radio_context["serving_cell"]
    else:
        previous_serving_cell = serving_cell_correction.get("previous_record", {}).get(
            "serving_cell"
        )
        for key in ("airspan_cell", "serving_cell"):
            if key in manifest and manifest[key] != previous_serving_cell:
                errors.append(
                    f"{condition_name}: acquisition manifest {key}={manifest[key]!r}, "
                    f"expected correction previous_record={previous_serving_cell!r}"
                )
    cell_states = radio_context.get("cell_administrative_states")
    has_post_acquisition_cell_correction = (
        run_root / "cell_administrative_state_correction.json"
    ).is_file()
    if cell_states and not has_post_acquisition_cell_correction:
        manifest_expected.update(
            {
                "cell_administrative_lock_definition": "locked means not broadcasting",
                "cell_1_administrative_state": cell_states["cell_1"]["state"],
                "cell_1_broadcasting": cell_states["cell_1"]["broadcasting"],
                "cell_2_administrative_state": cell_states["cell_2"]["state"],
                "cell_2_broadcasting": cell_states["cell_2"]["broadcasting"],
            }
        )
        if serving_cell_correction is None:
            manifest_expected["serving_cell"] = radio_context["serving_cell"]
    for key, expected in manifest_expected.items():
        if manifest.get(key) != expected:
            errors.append(
                f"{condition_name}: manifest {key}={manifest.get(key)!r}, expected {expected!r}"
            )
    if required["sender status"].read_text(encoding="utf-8").strip() != "0":
        errors.append(f"{condition_name}: sender exit status is not zero")

    if len(sender) != EXPECTED_COUNT:
        errors.append(f"{condition_name}: sender rows {len(sender)} != {EXPECTED_COUNT}")
    if len(receiver) < len(accepted_rows) or len(receiver) > EXPECTED_COUNT:
        errors.append(
            f"{condition_name}: receiver rows {len(receiver)} outside accepted..attempt range "
            f"{len(accepted_rows)}..{EXPECTED_COUNT}"
        )

    expected_sequences = list(range(1, EXPECTED_COUNT + 1))
    try:
        sender_sequences = [int(row["sequence"]) for row in sender]
        receiver_sequences = [int(row["sequence"]) for row in receiver]
        if sender_sequences != expected_sequences:
            errors.append(f"{condition_name}: sender sequence is not exactly 1..1000")
        if len(set(receiver_sequences)) != len(receiver_sequences):
            errors.append(f"{condition_name}: receiver sequence contains duplicates")
        if any(sequence not in expected_sequences for sequence in receiver_sequences):
            errors.append(f"{condition_name}: receiver sequence is outside 1..1000")

        sender_checks = (
            ("accepted state", lambda row: row["accepted"].lower() in ("true", "false")),
            ("transport", lambda row: row["transport"] == transport),
            ("condition_id", lambda row: row["condition_id"] == expected_condition_id),
            ("payload length", lambda row: int(row["application_payload_bytes"]) == payload),
            ("payload CRC32", lambda row: int(row["payload_crc32"]) == expected_crc),
        )
        receiver_checks = (
            ("accepted", lambda row: row["accepted"].lower() == "true"),
            ("transport", lambda row: row["transport"] == transport),
            ("condition_id", lambda row: row["condition_id"] == expected_condition_id),
            ("payload length", lambda row: int(row["application_payload_bytes"]) == payload),
            ("payload CRC32", lambda row: int(row["payload_crc32"]) == expected_crc),
            ("validation detail", lambda row: row["detail"] == EXPECTED_RECEIVER_DETAIL),
        )
        for label, check in sender_checks:
            if not all(check(row) for row in sender):
                errors.append(f"{condition_name}: sender {label} validation failed")
        if not all(row["detail"] == EXPECTED_SENDER_DETAIL for row in accepted_rows):
            errors.append(f"{condition_name}: accepted sender ACK detail validation failed")
        if not all(float(row["rtt_ms"]) >= 0 for row in accepted_rows):
            errors.append(f"{condition_name}: accepted sender RTT validation failed")
        if not all(row.get("detail", "").strip() for row in failed_rows):
            errors.append(f"{condition_name}: failed sender row lacks failure detail")
        for label, check in receiver_checks:
            if not all(check(row) for row in receiver):
                errors.append(f"{condition_name}: receiver {label} validation failed")
        receiver_by_sequence = {int(row["sequence"]): row for row in receiver}
        for sender_row in accepted_rows:
            receiver_row = receiver_by_sequence.get(int(sender_row["sequence"]))
            if receiver_row is None:
                errors.append(
                    f"{condition_name}: accepted sender sequence {sender_row['sequence']} lacks receiver row"
                )
                continue
            for field in (
                "sequence",
                "application_payload_bytes",
                "payload_crc32",
                "client_send_wall_ns",
                "server_processing_ms",
            ):
                if sender_row[field] != receiver_row[field]:
                    errors.append(
                        f"{condition_name}: sender/receiver {field} mismatch at sequence {sender_row['sequence']}"
                    )
                    break
    except (KeyError, TypeError, ValueError) as error:
        errors.append(f"{condition_name}: malformed sender/receiver row: {error}")

    try:
        rtts = [float(row["rtt_ms"]) for row in accepted_rows]
        server_processing = [float(row["server_processing_ms"]) for row in accepted_rows]
        wire_sizes = [int(row["wire_request_bytes"]) for row in accepted_rows]
        first_send_ns = min(int(row["client_send_wall_ns"]) for row in sender)
        final_receive_ns = max(int(row["client_receive_wall_ns"]) for row in sender)
        active_elapsed_s = (final_receive_ns - first_send_ns) / 1e9
    except (KeyError, TypeError, ValueError) as error:
        errors.append(f"{condition_name}: cannot derive sender metrics: {error}")
        rtts, server_processing, wire_sizes, active_elapsed_s = [], [], [], 0.0
    if active_elapsed_s <= 0:
        errors.append(f"{condition_name}: non-positive active application span")

    summary: dict[str, Any] = {
        "condition": condition_name,
        "condition_id": expected_condition_id,
        "transport": transport,
        "payload_label": {
            1024: "1 KiB",
            10240: "10 KiB",
            102400: "100 KiB",
            1048576: "1,024 KiB",
            2097152: "2,048 KiB",
        }[payload],
        "application_payload_bytes": payload,
        "tdd_profile": radio_context.get("tdd_profile"),
        "rsrp_dbm": manifest.get("rsrp_dbm"),
        "rsrq_db": manifest.get("rsrq_db"),
        "radio_state": manifest.get("radio_state"),
        "serving_cell": radio_context.get("serving_cell"),
        "serving_cell_state": radio_context.get("serving_cell_state"),
        "cell_1_administrative_state": (
            cell_states.get("cell_1", {}).get("state") if cell_states else None
        ),
        "cell_1_broadcasting": (
            cell_states.get("cell_1", {}).get("broadcasting") if cell_states else None
        ),
        "cell_2_administrative_state": (
            cell_states.get("cell_2", {}).get("state") if cell_states else None
        ),
        "cell_2_broadcasting": (
            cell_states.get("cell_2", {}).get("broadcasting") if cell_states else None
        ),
        "expected_payload_crc32": expected_crc,
        "attempts": len(sender),
        "accepted": len(accepted_rows),
        "failed": len(sender) - len(accepted_rows),
        "receiver_rows": len(receiver),
        "success_rate_pct": 100.0 * len(accepted_rows) / len(sender) if sender else 0.0,
        "rtt_count": len(rtts),
        "rtt_p50_ms": percentile(rtts, 0.50),
        "rtt_p95_ms": percentile(rtts, 0.95),
        "rtt_p99_ms": percentile(rtts, 0.99),
        "rtt_max_ms": max(rtts) if rtts else None,
        "server_processing_p50_ms": percentile(server_processing, 0.50),
        "server_processing_p95_ms": percentile(server_processing, 0.95),
        "server_processing_p99_ms": percentile(server_processing, 0.99),
        "server_processing_max_ms": max(server_processing) if server_processing else None,
        "wire_request_bytes_min": min(wire_sizes) if wire_sizes else None,
        "wire_request_bytes_max": max(wire_sizes) if wire_sizes else None,
        "active_application_elapsed_s": active_elapsed_s,
        "accepted_application_bytes": len(accepted_rows) * payload,
        "application_goodput_mbps": (
            len(accepted_rows) * payload * 8.0 / active_elapsed_s / 1e6
            if active_elapsed_s > 0
            else None
        ),
        "workload_started_at": manifest.get("workload_started_at"),
        "workload_finished_at": manifest.get("workload_finished_at"),
    }
    for deadline in DEADLINES_MS:
        hits = sum(rtt <= deadline for rtt in rtts)
        summary[f"deadline_hits_{deadline}ms"] = hits
        summary[f"deadline_misses_{deadline}ms"] = len(sender) - hits
        summary[f"deadline_hit_rate_{deadline}ms_pct"] = (
            100.0 * hits / len(sender) if sender else 0.0
        )

    stored_expected = {
        "status": "complete",
        "transport": transport,
        "application_payload_bytes": payload,
        "attempts": summary["attempts"],
        "accepted": summary["accepted"],
        "failed": summary["failed"],
        "receiver_rows": summary["receiver_rows"],
        "rtt_count": summary["rtt_count"],
    }
    for key, expected in stored_expected.items():
        if stored_validation.get(key) != expected:
            errors.append(
                f"{condition_name}: stored validation {key}={stored_validation.get(key)!r}, expected {expected!r}"
            )
    for deadline in DEADLINES_MS:
        key = f"deadline_misses_{deadline}ms"
        if stored_validation.get(key) != summary[key]:
            errors.append(
                f"{condition_name}: stored validation {key}={stored_validation.get(key)!r}, expected {summary[key]!r}"
            )

    host_rows: list[dict[str, Any]] = []
    telemetry_paths = (
        ("car", condition_dir / "host_telemetry" / "car.jsonl"),
        ("edge", condition_dir / "edge" / "host_telemetry" / "d1.jsonl"),
    )
    for host_role, path in telemetry_paths:
        telemetry, telemetry_errors = telemetry_summary(path, host_role)
        telemetry.update(
            {
                "condition": condition_name,
                "transport": transport,
                "application_payload_bytes": payload,
            }
        )
        host_rows.append(telemetry)
        errors.extend(f"{condition_name}: {error}" for error in telemetry_errors)
    car = next(row for row in host_rows if row["host_role"] == "car")
    edge = next(row for row in host_rows if row["host_role"] == "edge")
    summary.update(
        {
            "car_interface_tx_mbps": car.get("network_tx_mbps"),
            "edge_interface_rx_mbps": edge.get("network_rx_mbps"),
            "car_cpu_busy_pct": car.get("cpu_busy_pct"),
            "edge_cpu_busy_pct": edge.get("cpu_busy_pct"),
            "car_memory_used_pct_max": car.get("memory_used_pct_max"),
            "edge_memory_used_pct_max": edge.get("memory_used_pct_max"),
            "car_interface_error_drop_delta": sum(
                car.get(key) or 0
                for key in (
                    "network_tx_errors_delta",
                    "network_rx_errors_delta",
                    "network_tx_drops_delta",
                    "network_rx_drops_delta",
                )
            ),
            "edge_interface_error_drop_delta": sum(
                edge.get(key) or 0
                for key in (
                    "network_tx_errors_delta",
                    "network_rx_errors_delta",
                    "network_tx_drops_delta",
                    "network_rx_drops_delta",
                )
            ),
        }
    )
    summary["condition_validation_passed"] = not errors
    return summary, host_rows, errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("run_root", type=Path)
    parser.add_argument(
        "--transports",
        nargs="+",
        choices=TRANSPORTS,
        default=list(TRANSPORTS),
        help="Transport subset to analyze (default: tcp mqtt)",
    )
    parser.add_argument(
        "--payloads",
        nargs="+",
        type=int,
        choices=PAYLOADS,
        default=list(PAYLOADS),
        help="Payload-byte subset to analyze (default: the full payload matrix)",
    )
    args = parser.parse_args()

    selected_transports = tuple(dict.fromkeys(args.transports))
    selected_payloads = tuple(dict.fromkeys(args.payloads))
    expected_conditions = len(selected_transports) * len(selected_payloads)
    expected_attempts = expected_conditions * EXPECTED_COUNT

    run_root = args.run_root.resolve()
    if not run_root.is_dir() or run_root == Path("/"):
        raise SystemExit("run root must be an existing non-root directory")
    context_path = run_root / "operator_context.json"
    if not context_path.is_file():
        raise SystemExit(f"missing operator context: {context_path}")
    operator_context = load_json(context_path)
    serving_cell_correction_path = run_root / "serving_cell_selection_correction.json"
    serving_cell_correction = (
        load_json(serving_cell_correction_path)
        if serving_cell_correction_path.is_file()
        else None
    )
    tdd_profile_correction_path = run_root / "tdd_profile_correction.json"
    tdd_profile_correction = (
        load_json(tdd_profile_correction_path)
        if tdd_profile_correction_path.is_file()
        else None
    )
    correction_errors: list[str] = []
    if serving_cell_correction is not None:
        corrected = serving_cell_correction.get("corrected_serving_context", {})
        if (
            serving_cell_correction.get("schema")
            != "edge4av-airspan-serving-cell-selection-correction-v1"
        ):
            correction_errors.append("serving-cell correction schema is invalid")
        if serving_cell_correction.get("run_name") != operator_context.get("run_name"):
            correction_errors.append("serving-cell correction run_name does not match operator context")
        if serving_cell_correction.get("applies_to_all_conditions") is not True:
            correction_errors.append("serving-cell correction does not apply to all conditions")
        if corrected.get("serving_cell") != operator_context.get("radio", {}).get("serving_cell"):
            correction_errors.append("corrected serving cell does not match operator context")
        if corrected.get("handoff_state") != operator_context.get("radio", {}).get("handoff_state"):
            correction_errors.append("corrected handoff state does not match operator context")
        corrected_cell = corrected.get("serving_cell")
        corrected_admin = (
            operator_context.get("radio", {})
            .get("cell_administrative_states", {})
            .get(f"cell_{corrected_cell}", {})
        )
        if corrected_admin.get("broadcasting") is not True:
            correction_errors.append("corrected serving cell is not recorded as broadcasting")
    if tdd_profile_correction is not None:
        corrected_tdd = tdd_profile_correction.get("corrected_tdd_context", {})
        if (
            tdd_profile_correction.get("schema")
            != "edge4av-airspan-tdd-profile-correction-v1"
        ):
            correction_errors.append("TDD-profile correction schema is invalid")
        if tdd_profile_correction.get("run_name") != operator_context.get("run_name"):
            correction_errors.append("TDD-profile correction run_name does not match operator context")
        if tdd_profile_correction.get("applies_to_all_conditions") is not True:
            correction_errors.append("TDD-profile correction does not apply to all conditions")
        if corrected_tdd.get("tdd_profile") != operator_context.get("radio", {}).get(
            "tdd_profile"
        ):
            correction_errors.append("corrected TDD profile does not match operator context")
    correction_path = run_root / "measurement_boundary_correction.json"
    if not correction_path.is_file():
        raise SystemExit(f"missing measurement correction: {correction_path}")
    measurement_correction = load_json(correction_path)
    anomalies_path = run_root / "known_anomalies.json"
    if not anomalies_path.is_file():
        raise SystemExit(f"missing known anomalies record: {anomalies_path}")
    known_anomalies = load_json(anomalies_path).get("anomalies", [])
    edge_verification_path = run_root / "edge_artifact_verification.json"
    if not edge_verification_path.is_file():
        raise SystemExit(f"missing edge artifact verification: {edge_verification_path}")
    edge_artifact_verification = load_json(edge_verification_path)
    analysis_dir = run_root / "analysis"
    repo_root = Path(__file__).resolve().parents[1]

    conditions: list[dict[str, Any]] = []
    host_rows: list[dict[str, Any]] = []
    errors: list[str] = list(correction_errors)
    if not edge_artifact_verification.get("all_remote_files_match_local_copies"):
        errors.append("remote edge artifacts do not all match their local fetched copies")
    for transport in selected_transports:
        for payload in selected_payloads:
            condition, hosts, condition_errors = analyze_condition(
                run_root,
                transport,
                payload,
                operator_context,
                serving_cell_correction,
                tdd_profile_correction,
            )
            conditions.append(condition)
            host_rows.extend(hosts)
            errors.extend(condition_errors)

    complete_conditions = sum(
        row.get("attempts") == EXPECTED_COUNT and row.get("condition_validation_passed") is True
        for row in conditions
    )
    attempts = sum(int(row.get("attempts", 0)) for row in conditions)
    accepted = sum(int(row.get("accepted", 0)) for row in conditions)
    failed = sum(int(row.get("failed", 0)) for row in conditions)
    total_application_bytes = sum(int(row.get("accepted_application_bytes", 0)) for row in conditions)
    total_active_s = sum(float(row.get("active_application_elapsed_s", 0)) for row in conditions)
    matrix_passed = complete_conditions == expected_conditions and not errors
    radio = operator_context["radio"]
    per_condition_radio_context_validated = all(
        row.get("tdd_profile") == radio["tdd_profile"]
        and row.get("rsrp_dbm") == radio["rsrp_dbm"]
        and row.get("rsrq_db") == radio["rsrq_db"]
        for row in conditions
    )
    cell_administrative_context = radio.get("cell_administrative_states")
    per_condition_cell_context_validated = bool(cell_administrative_context) and all(
        row.get("serving_cell") == radio["serving_cell"]
        and row.get("cell_1_administrative_state")
        == cell_administrative_context["cell_1"]["state"]
        and row.get("cell_1_broadcasting")
        == cell_administrative_context["cell_1"]["broadcasting"]
        and row.get("cell_2_administrative_state")
        == cell_administrative_context["cell_2"]["state"]
        and row.get("cell_2_broadcasting")
        == cell_administrative_context["cell_2"]["broadcasting"]
        for row in conditions
    )

    overall = {
        "conditions": len(conditions),
        "complete_conditions": complete_conditions,
        "attempts": attempts,
        "accepted": accepted,
        "failed": failed,
        "success_rate_pct": 100.0 * accepted / attempts if attempts else 0.0,
        "accepted_application_bytes": total_application_bytes,
        "sum_active_application_elapsed_s": total_active_s,
        "aggregate_application_goodput_mbps": (
            total_application_bytes * 8.0 / total_active_s / 1e6 if total_active_s > 0 else None
        ),
    }
    for deadline in DEADLINES_MS:
        hits = sum(int(row.get(f"deadline_hits_{deadline}ms", 0)) for row in conditions)
        overall[f"deadline_hits_{deadline}ms"] = hits
        overall[f"deadline_misses_{deadline}ms"] = attempts - hits
        overall[f"deadline_hit_rate_{deadline}ms_pct"] = (
            100.0 * hits / attempts if attempts else 0.0
        )

    run_manifest = {
        "schema": "edge4av-airspan-raw-uplink-run-v1",
        "status": "complete" if matrix_passed else "invalid",
        "run_name": operator_context["run_name"],
        "run_id": operator_context["run_id"],
        "analyzed_at_utc": datetime.now(timezone.utc).isoformat(),
        "git_commit_at_analysis": git_commit(repo_root),
        "transports": list(selected_transports),
        "application_payload_bytes": list(selected_payloads),
        "attempts_per_condition": EXPECTED_COUNT,
        "application_direction": "uplink-oriented",
        "timing_metric": "monotonic complete application-acknowledgment receipt-and-parse RTT",
        "measurement_boundary_correction": measurement_correction,
        "known_anomalies": known_anomalies,
        "edge_artifact_verification": edge_artifact_verification,
        "one_way_metrics_valid": False,
        "application_matrix_passed": matrix_passed,
        "operator_context": operator_context,
        "overall": overall,
    }
    if serving_cell_correction is not None:
        run_manifest["serving_cell_selection_correction"] = serving_cell_correction
    if tdd_profile_correction is not None:
        run_manifest["tdd_profile_correction"] = tdd_profile_correction
    validation = {
        "schema": "edge4av-airspan-raw-uplink-validation-v1",
        "application_matrix_passed": matrix_passed,
        "expected_conditions": expected_conditions,
        "complete_conditions": complete_conditions,
        "expected_attempts": expected_attempts,
        "attempts": attempts,
        "accepted": accepted,
        "failed": failed,
        "expected_receiver_rows_if_all_attempts_are_accepted": expected_attempts,
        "receiver_rows": sum(int(row.get("receiver_rows", 0)) for row in conditions),
        "payload_length_and_crc32_validated": matrix_passed,
        "sender_edge_sequence_correlation_validated": matrix_passed,
        "host_telemetry_files": len(host_rows),
        "validation_errors": errors,
        "one_way_metrics_excluded": True,
        "measurement_boundary_correction_applied": True,
        "serving_cell_selection_correction_applied": serving_cell_correction is not None,
        "tdd_profile_correction_applied": tdd_profile_correction is not None,
        "known_anomalies": known_anomalies,
        "remote_edge_artifacts_checksum_verified": edge_artifact_verification.get(
            "all_remote_files_match_local_copies", False
        ),
        "remote_edge_files_verified": edge_artifact_verification.get("matched_files", 0),
        "per_condition_radio_context_validated": per_condition_radio_context_validated,
        "per_condition_cell_context_validated": per_condition_cell_context_validated,
        "cell_administrative_context": cell_administrative_context,
        "serving_cell_context": {
            "serving_cell": radio["serving_cell"],
            "serving_cell_state": radio.get("serving_cell_state"),
            "handoff_state": radio.get("handoff_state"),
        },
        "radio_context": {
            "tdd_profile": radio["tdd_profile"],
            "rsrp_dbm": radio["rsrp_dbm"],
            "rsrq_db": radio["rsrq_db"],
            "state": "operator-reported",
        },
        "paper_facing_tdd_comparison_passed": False,
        "paper_facing_blockers": [
            "This is one location and one operator-reported TDD profile, not a matched TDD comparison.",
            "Timestamp-aligned Airspan configuration/ACP and MG52 radio exports are not stored with this block.",
        ],
    }

    condition_fields = [
        "condition",
        "condition_validation_passed",
        "transport",
        "payload_label",
        "application_payload_bytes",
        "tdd_profile",
        "rsrp_dbm",
        "rsrq_db",
        "radio_state",
        "serving_cell",
        "serving_cell_state",
        "cell_1_administrative_state",
        "cell_1_broadcasting",
        "cell_2_administrative_state",
        "cell_2_broadcasting",
        "expected_payload_crc32",
        "attempts",
        "accepted",
        "failed",
        "receiver_rows",
        "success_rate_pct",
        "rtt_count",
        "rtt_p50_ms",
        "rtt_p95_ms",
        "rtt_p99_ms",
        "rtt_max_ms",
        "server_processing_p50_ms",
        "server_processing_p95_ms",
        "server_processing_p99_ms",
        "server_processing_max_ms",
        "deadline_hits_100ms",
        "deadline_misses_100ms",
        "deadline_hit_rate_100ms_pct",
        "deadline_hits_500ms",
        "deadline_misses_500ms",
        "deadline_hit_rate_500ms_pct",
        "deadline_hits_1000ms",
        "deadline_misses_1000ms",
        "deadline_hit_rate_1000ms_pct",
        "wire_request_bytes_min",
        "wire_request_bytes_max",
        "active_application_elapsed_s",
        "accepted_application_bytes",
        "application_goodput_mbps",
        "car_interface_tx_mbps",
        "edge_interface_rx_mbps",
        "car_cpu_busy_pct",
        "edge_cpu_busy_pct",
        "car_memory_used_pct_max",
        "edge_memory_used_pct_max",
        "car_interface_error_drop_delta",
        "edge_interface_error_drop_delta",
        "workload_started_at",
        "workload_finished_at",
    ]
    host_fields = [
        "condition",
        "transport",
        "application_payload_bytes",
        "host_role",
        "telemetry_rows",
        "telemetry_elapsed_s",
        "cpu_busy_pct",
        "memory_used_pct_median",
        "memory_used_pct_max",
        "interface",
        "network_tx_bytes_delta",
        "network_rx_bytes_delta",
        "network_tx_mbps",
        "network_rx_mbps",
        "network_tx_errors_delta",
        "network_rx_errors_delta",
        "network_tx_drops_delta",
        "network_rx_drops_delta",
    ]
    write_csv(analysis_dir / "application_summary.csv", conditions, condition_fields)
    write_csv(analysis_dir / "host_telemetry_summary.csv", host_rows, host_fields)
    write_json(run_root / "run_manifest.json", run_manifest)
    write_json(run_root / "validation_summary.json", validation)
    write_json(
        analysis_dir / "summary.json",
        {
            "schema": "edge4av-airspan-raw-uplink-analysis-v1",
            "run_manifest": run_manifest,
            "validation": validation,
            "conditions": conditions,
        },
    )

    location = operator_context["location"]
    if location.get("fresh_gps_capture_for_this_run") is False:
        location_context_line = (
            f"- Referenced location evidence: same-day stationary `{location['location_id']}` "
            f"capture; repository-safe coordinate derivative "
            f"`{location['repository_coordinate_0_001deg']}`. "
            "Continuity with this application run was not independently reverified."
        )
    else:
        location_context_line = (
            f"- Location: stationary `{location['location_id']}`; repository-safe "
            f"coordinate derivative `{location['repository_coordinate_0_001deg']}`."
        )
    if cell_administrative_context:
        cell_1 = cell_administrative_context["cell_1"]
        cell_2 = cell_administrative_context["cell_2"]
        cell_context_line = (
            f"- Airspan administrative state: Cell 1 `{cell_1['state']}` "
            f"({'broadcasting' if cell_1['broadcasting'] else 'not broadcasting'}); "
            f"Cell 2 `{cell_2['state']}` "
            f"({'broadcasting' if cell_2['broadcasting'] else 'not broadcasting'}). "
            f"Serving cell recorded as Cell {radio['serving_cell']}; its evidence state is listed below."
        )
    else:
        cell_context_line = (
            f"- Serving cell: operator-reported Cell {radio['serving_cell']}; "
            "Airspan administrative/broadcast states were not recorded."
        )
    lines = [
        "# Private 5G Raw-Byte Uplink Payload Sweep",
        "",
        f"- Application matrix: `{'passed' if matrix_passed else 'failed'}` ({complete_conditions}/{expected_conditions} conditions).",
        f"- Attempts: `{attempts}`; accepted: `{accepted}`; failed: `{failed}`; success: `{overall['success_rate_pct']:.6f}%`.",
        "- Request: exact raw application body; response: compact correlated application acknowledgment after edge length/CRC32 validation.",
        "- RTT: sender monotonic time immediately before framed transmission through receipt and structural parsing of the complete application acknowledgment.",
        f"- Recorded radio context: `{radio['tdd_profile']}`, RSRP `{radio['rsrp_dbm']} dBm`, RSRQ `{radio['rsrq_db']} dB`.",
        cell_context_line,
        location_context_line,
        "- One-way latency is excluded because endpoint clocks were unsynchronized.",
        "",
        "## Per-Condition Results",
        "",
        "| transport | payload | TDD | RSRP dBm | RSRQ dB | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | miss @100 ms | miss @500 ms | miss @1000 ms | app goodput Mbps |",
        "| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]
    for row in conditions:
        lines.append(
            f"| {row['transport'].upper()} | {row.get('payload_label', '')} | "
            f"{row.get('tdd_profile', '')} | {row.get('rsrp_dbm', '')} | {row.get('rsrq_db', '')} | "
            f"{row.get('accepted', 0)}/{row.get('attempts', 0)} | "
            f"{format_number(row.get('rtt_p50_ms'), 3)} | {format_number(row.get('rtt_p95_ms'), 3)} | "
            f"{format_number(row.get('rtt_p99_ms'), 3)} | {format_number(row.get('rtt_max_ms'), 3)} | "
            f"{row.get('deadline_misses_100ms', '')} | {row.get('deadline_misses_500ms', '')} | "
            f"{row.get('deadline_misses_1000ms', '')} | {format_number(row.get('application_goodput_mbps'), 3)} |"
        )
    lines.extend(
        [
            "",
            "## Evidence Qualifications",
            "",
            "- Payload labels are application-body bytes, not individual IP packets. TCP can segment one application object across many packets; MQTT adds its own framing over TCP.",
            "- Application goodput is accepted payload bytes divided by the sender span from the first request transmission to the final application acknowledgment. Interface rates also include framing, acknowledgments, telemetry, and incidental interface traffic.",
            "- Topic/sequence/accepted-state/payload-length/CRC32 comparisons occur immediately after the RTT timer. Every accepted row passed them, but their local CPU time is not included in `rtt_ms`.",
            "- TDD, RSRP, and RSRQ are copied into every per-condition manifest and application-summary row, and the analyzer rejects a condition if those values differ from the run-level operator context.",
            "- The per-condition telemetry captures are process-scoped in time, but cross-host telemetry timestamps are not used for latency because the clocks were unsynchronized.",
            f"- The referenced one-minute ROS 2 bag and exact coordinates remain in excluded raw location evidence `{location['gps_raw_run_name']}`; only the stationarity summary and 0.001-degree derivative are repository-facing.",
            f"- Evidence states: TDD `{radio.get('tdd_profile_state', 'not-recorded')}`; "
            f"serving context `{radio.get('serving_cell_state', 'not-recorded')}`; "
            f"RSRP `{radio.get('rsrp_state', radio.get('radio_measurement_state', 'not-recorded'))}`; "
            f"RSRQ `{radio.get('rsrq_state', radio.get('radio_measurement_state', 'not-recorded'))}`. "
            "Timestamp-aligned Airspan/ACP and MG52 exports are still required for artifact verification.",
            "- This payload sweep covers one location and one profile. It cannot estimate a causal TDD effect and does not replace the planned two-location `40/40/20` versus `70/20/10` directional matrix.",
            "",
        ]
    )
    if serving_cell_correction is not None:
        lines.insert(
            -1,
            "- The acquisition-time condition manifests retain their original serving-cell "
            "entry. `serving_cell_selection_correction.json` supersedes that entry for "
            "serving-cell interpretation; application measurements are unchanged.",
        )
    if tdd_profile_correction is not None:
        lines.insert(
            -1,
            "- The acquisition-time condition manifests retain their original TDD "
            "entry. `tdd_profile_correction.json` supersedes that entry for "
            "radio-context interpretation; application measurements are unchanged.",
        )
    if known_anomalies:
        lines.insert(
            -1,
            f"- Known acquisition anomalies are preserved in `known_anomalies.json` ({len(known_anomalies)} record(s)); no row was silently filtered.",
        )
    (analysis_dir / "summary.md").write_text("\n".join(lines), encoding="utf-8")

    print(json.dumps(validation, indent=2, sort_keys=True))
    return 0 if matrix_passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
