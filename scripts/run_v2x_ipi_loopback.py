#!/usr/bin/env python3
"""Run local IPI loopback probes with V2X dataset-derived payload sizes."""

from __future__ import annotations

import argparse
import csv
import json
import math
import os
import re
import socket
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MANIFEST = REPO_ROOT / "results" / "v2x_benchmarks" / "latest" / "v2x_ipi_payload_manifest.json"
DEFAULT_RESULTS_ROOT = REPO_ROOT / "results" / "v2x_benchmarks"
SENDER = REPO_ROOT / "cpp" / "build" / "example_private_5g_latency_sender"
RECEIVER = REPO_ROOT / "cpp" / "build" / "example_private_5g_latency_receiver"


def run(cmd: list[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    print("+ " + " ".join(cmd), flush=True)
    return subprocess.run(cmd, cwd=cwd, check=True, text=True)


def capture(cmd: list[str]) -> str:
    try:
        return subprocess.check_output(cmd, text=True, stderr=subprocess.STDOUT).strip()
    except (subprocess.CalledProcessError, FileNotFoundError) as exc:
        return str(exc)


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def wait_for_port(port: int, process: subprocess.Popen[str], timeout_s: float = 5.0) -> None:
    deadline = time.time() + timeout_s
    while time.time() < deadline:
        if process.poll() is not None:
            raise RuntimeError(f"receiver exited early with code {process.returncode}")
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.2):
                return
        except OSError:
            time.sleep(0.05)
    raise TimeoutError(f"receiver did not listen on port {port}")


def percentile(values: list[float], pct: float) -> float | None:
    if not values:
        return None
    ordered = sorted(values)
    rank = math.ceil(pct * len(ordered)) - 1
    return ordered[max(0, min(rank, len(ordered) - 1))]


def parse_sender_csv(path: Path) -> dict:
    rows = []
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            rows.append(row)
    rtts = [float(row["rtt_ms"]) for row in rows if row.get("accepted") == "true" and row.get("rtt_ms")]
    attempts = len(rows)
    accepted = sum(1 for row in rows if row.get("accepted") == "true")
    return {
        "attempts": attempts,
        "accepted": accepted,
        "success_rate_pct": round((accepted / attempts) * 100.0, 3) if attempts else 0.0,
        "rtt_ms_p50": percentile(rtts, 0.50),
        "rtt_ms_p95": percentile(rtts, 0.95),
        "rtt_ms_p99": percentile(rtts, 0.99),
        "rtt_ms_count": len(rtts),
    }


def load_payload_sizes(manifest_path: Path) -> list[int]:
    with manifest_path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    sizes = manifest.get("representative_ipi_payload_bytes", [])
    return sorted({int(size) for size in sizes if int(size) >= 0})


def slugify(value: str) -> str:
    slug = re.sub(r"[^A-Za-z0-9]+", "-", value).strip("-").lower()
    return slug or "unknown"


def load_payload_sets(
    manifest_path: Path,
    group_by_dataset: bool,
    dataset_filters: set[str],
) -> list[dict]:
    with manifest_path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)

    if not group_by_dataset:
        sizes = manifest.get("representative_ipi_payload_bytes", [])
        return [
            {
                "benchmark": "all",
                "dataset": "all",
                "label": "all-datasets",
                "payload_sizes": sorted({int(size) for size in sizes if int(size) >= 0}),
            }
        ]

    payload_sets = []
    for item in manifest.get("dataset_family_payloads", []):
        benchmark = item.get("benchmark", "unknown")
        dataset = item.get("dataset", "unknown")
        label = f"{benchmark}/{dataset}"
        filter_keys = {slugify(benchmark), slugify(dataset), slugify(label)}
        if dataset_filters and not (dataset_filters & filter_keys):
            continue
        sizes = item.get("representative_ipi_payload_bytes", [])
        payload_sets.append(
            {
                "benchmark": benchmark,
                "dataset": dataset,
                "label": label,
                "payload_sizes": sorted({int(size) for size in sizes if int(size) >= 0}),
            }
        )

    if not payload_sets:
        raise SystemExit("No matching dataset payload sets found in manifest.")
    return payload_sets


def build_cpp() -> None:
    run(["cmake", "-S", "cpp", "-B", "cpp/build", "-DIPI_ENABLE_TESTS=ON"], cwd=REPO_ROOT)
    run(["cmake", "--build", "cpp/build", "--target", "example_private_5g_latency_receiver", "example_private_5g_latency_sender"], cwd=REPO_ROOT)


def write_readme(output_dir: Path, summary: dict) -> None:
    lines = [
        "# V2X IPI Loopback Benchmark",
        "",
        f"Run ID: `{summary['run_id']}`",
        f"Generated UTC: `{summary['generated_utc']}`",
        "",
        "This run uses dataset-derived payload sizes with the local TCP IPI probe pair.",
        "It measures IPI message validation and loopback transport behavior, not detector accuracy.",
        "",
        "## Environment",
        "",
        "```text",
        summary["environment"].get("nvidia_smi_before", ""),
        "```",
        "",
        "## Conditions",
        "",
        "| benchmark | dataset | payload_bytes | attempts | accepted | success_rate_pct | p50_ms | p95_ms | p99_ms |",
        "| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]
    for item in summary["conditions"]:
        lines.append(
            f"| {item.get('benchmark', '')} | {item.get('dataset', '')} | "
            f"{item['payload_bytes']} | {item['attempts']} | {item['accepted']} | "
            f"{item['success_rate_pct']} | {format_ms(item['rtt_ms_p50'])} | "
            f"{format_ms(item['rtt_ms_p95'])} | {format_ms(item['rtt_ms_p99'])} |"
        )
    lines.extend(["", "Raw CSVs and stderr logs are in this directory."])
    (output_dir / "README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def format_ms(value: float | None) -> str:
    return "" if value is None else f"{value:.3f}"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--results-root", type=Path, default=DEFAULT_RESULTS_ROOT)
    parser.add_argument("--payload-sizes", default="", help="Comma-separated override.")
    parser.add_argument("--group-by-dataset", action="store_true")
    parser.add_argument(
        "--dataset-families",
        default="",
        help="Comma-separated slugs or names to include when using --group-by-dataset.",
    )
    parser.add_argument("--count", type=int, default=30)
    parser.add_argument("--interval-ms", type=int, default=0)
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--run-id", default="")
    args = parser.parse_args()

    if args.build:
        build_cpp()
    if not SENDER.exists() or not RECEIVER.exists():
        raise SystemExit("IPI sender/receiver binaries are missing. Rerun with --build.")

    if args.payload_sizes:
        payload_sets = [
            {
                "benchmark": "manual",
                "dataset": "manual",
                "label": "manual",
                "payload_sizes": sorted(
                    {int(item) for item in args.payload_sizes.split(",") if item.strip()}
                ),
            }
        ]
    else:
        dataset_filters = {
            slugify(item) for item in args.dataset_families.split(",") if item.strip()
        }
        payload_sets = load_payload_sets(args.manifest, args.group_by_dataset, dataset_filters)

    run_id = args.run_id or "v2x-ipi-loopback-" + datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    output_dir = args.results_root.resolve() / run_id
    output_dir.mkdir(parents=True, exist_ok=True)

    env = os.environ.copy()
    env["CUDA_VISIBLE_DEVICES"] = env.get("CUDA_VISIBLE_DEVICES", "0,1,2,3")

    summary = {
        "run_id": run_id,
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "manifest": str(args.manifest.resolve()),
        "count_per_condition": args.count,
        "interval_ms": args.interval_ms,
        "cuda_visible_devices": env["CUDA_VISIBLE_DEVICES"],
        "payload_sets": [
            {
                "benchmark": item["benchmark"],
                "dataset": item["dataset"],
                "label": item["label"],
                "payload_sizes": item["payload_sizes"],
            }
            for item in payload_sets
        ],
        "environment": {
            "python": sys.version,
            "nvidia_smi_before": capture(
                [
                    "nvidia-smi",
                    "--query-gpu=index,name,memory.total,memory.used,utilization.gpu",
                    "--format=csv,noheader",
                ]
            ),
            "gpu_processes_before": capture(
                [
                    "nvidia-smi",
                    "--query-compute-apps=gpu_uuid,pid,process_name,used_memory",
                    "--format=csv,noheader",
                ]
            ),
        },
        "conditions": [],
    }

    for payload_set in payload_sets:
        scope_slug = slugify(payload_set["label"])
        condition_label = f"{payload_set['label']}-dataset-derived-ipi-loopback"
        for payload_bytes in payload_set["payload_sizes"]:
            port = free_port()
            condition_id = f"v2x-ipi-loopback-{scope_slug}-payload-{payload_bytes}"
            receiver_stdout = output_dir / f"{condition_id}_receiver.csv"
            receiver_stderr = output_dir / f"{condition_id}_receiver.stderr"
            sender_stdout = output_dir / f"{condition_id}_sender.csv"
            sender_stderr = output_dir / f"{condition_id}_sender.stderr"

            receiver_cmd = [
                str(RECEIVER),
                "--transport",
                "tcp",
                "--port",
                str(port),
                "--run-id",
                run_id,
                "--condition-id",
                condition_id,
                "--condition-label",
                condition_label,
                "--rsu-id",
                "local-rsu",
                "--network-load-level",
                "loopback",
                "--qos-profile",
                "host-default",
                "--mobility-state",
                "dataset-replay",
                "--clock-sync-state",
                "same-host",
                "--csv",
            ]
            sender_cmd = [
                str(SENDER),
                "--transport",
                "tcp",
                "--host",
                "127.0.0.1",
                "--port",
                str(port),
                "--count",
                str(args.count),
                "--interval-ms",
                str(args.interval_ms),
                "--message",
                "service",
                "--payload-bytes",
                str(payload_bytes),
                "--run-id",
                run_id,
                "--condition-id",
                condition_id,
                "--condition-label",
                condition_label,
                "--request-id",
                condition_id,
                "--av-id",
                "dataset-agent",
                "--obu-id",
                "dataset-agent",
                "--rsu-id",
                "local-rsu",
                "--network-load-level",
                "loopback",
                "--qos-profile",
                "host-default",
                "--mobility-state",
                "dataset-replay",
                "--clock-sync-state",
                "same-host",
                "--csv",
            ]

            with receiver_stdout.open("w", encoding="utf-8") as r_out, receiver_stderr.open("w", encoding="utf-8") as r_err:
                receiver = subprocess.Popen(receiver_cmd, stdout=r_out, stderr=r_err, text=True, env=env)
                try:
                    wait_for_port(port, receiver)
                    with sender_stdout.open("w", encoding="utf-8") as s_out, sender_stderr.open("w", encoding="utf-8") as s_err:
                        subprocess.run(sender_cmd, stdout=s_out, stderr=s_err, check=True, text=True, env=env, timeout=120)
                finally:
                    if receiver.poll() is None:
                        receiver.terminate()
                        try:
                            receiver.wait(timeout=5)
                        except subprocess.TimeoutExpired:
                            receiver.kill()
                            receiver.wait(timeout=5)

            condition_summary = parse_sender_csv(sender_stdout)
            condition_summary.update(
                {
                    "benchmark": payload_set["benchmark"],
                    "dataset": payload_set["dataset"],
                    "dataset_label": payload_set["label"],
                    "payload_bytes": payload_bytes,
                    "condition_id": condition_id,
                    "sender_csv": str(sender_stdout),
                    "receiver_csv": str(receiver_stdout),
                    "sender_stderr": str(sender_stderr),
                    "receiver_stderr": str(receiver_stderr),
                }
            )
            summary["conditions"].append(condition_summary)
            print(json.dumps(condition_summary, sort_keys=True))

    summary["environment"]["nvidia_smi_after"] = capture(
        [
            "nvidia-smi",
            "--query-gpu=index,name,memory.total,memory.used,utilization.gpu",
            "--format=csv,noheader",
        ]
    )

    with (output_dir / "summary.json").open("w", encoding="utf-8") as handle:
        json.dump(summary, handle, indent=2, sort_keys=True)
        handle.write("\n")

    with (output_dir / "summary.csv").open("w", encoding="utf-8", newline="") as handle:
        fieldnames = [
            "condition_id",
            "benchmark",
            "dataset",
            "dataset_label",
            "payload_bytes",
            "attempts",
            "accepted",
            "success_rate_pct",
            "rtt_ms_p50",
            "rtt_ms_p95",
            "rtt_ms_p99",
            "rtt_ms_count",
        ]
        writer = csv.DictWriter(handle, fieldnames=fieldnames, lineterminator="\n")
        writer.writeheader()
        for item in summary["conditions"]:
            writer.writerow({key: item.get(key) for key in fieldnames})

    write_readme(output_dir, summary)
    print(f"Wrote {output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
