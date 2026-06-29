#!/usr/bin/env python3
"""Run a lightweight four-GPU benchmark over staged V2X dataset artifacts."""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import math
import os
import subprocess
import sys
import time
from collections import defaultdict
from datetime import datetime, timezone
from multiprocessing import get_context
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MANIFEST = REPO_ROOT / "results" / "v2x_benchmarks" / "latest" / "v2x_ipi_payload_manifest.json"
DEFAULT_RESULTS_ROOT = REPO_ROOT / "results" / "v2x_benchmarks"


def capture(cmd: list[str]) -> str:
    try:
        return subprocess.check_output(cmd, text=True, stderr=subprocess.STDOUT).strip()
    except (FileNotFoundError, subprocess.CalledProcessError) as exc:
        return str(exc)


def load_manifest(path: Path) -> dict:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def select_records(manifest: dict, files_per_dataset: int, categories: set[str]) -> list[dict]:
    grouped: dict[tuple[str, str], dict[str, list[dict]]] = defaultdict(lambda: defaultdict(list))
    for record in manifest.get("records", []):
        if categories and record.get("category") not in categories:
            continue
        key = (record.get("benchmark", "unknown"), record.get("dataset", "unknown"))
        grouped[key][record.get("category", "unknown")].append(record)

    selected = []
    for key in sorted(grouped):
        per_category = grouped[key]
        category_names = sorted(per_category)
        if not category_names:
            continue
        quota = max(1, math.ceil(files_per_dataset / len(category_names)))
        dataset_records = []
        for category in category_names:
            records = sorted(
                per_category[category],
                key=lambda item: (-int(item.get("raw_bytes", 0)), item.get("path", "")),
            )
            dataset_records.extend(records[:quota])
        selected.extend(dataset_records[:files_per_dataset])
    return selected


def shard_records(records: list[dict], workers: int) -> list[list[dict]]:
    shards = [[] for _ in range(workers)]
    for index, record in enumerate(records):
        shards[index % workers].append(record)
    return shards


def worker_main(
    local_rank: int,
    records: list[dict],
    data_root: str,
    max_bytes: int,
    queue,
) -> None:
    import torch

    torch.cuda.set_device(local_rank)
    device = torch.device("cuda", local_rank)
    gpu_name = torch.cuda.get_device_name(device)
    rows = []
    failures = []
    started = time.perf_counter()
    total_bytes = 0

    for record in records:
        path = Path(data_root) / record["path"]
        read_started = time.perf_counter()
        try:
            payload = path.read_bytes()[:max_bytes]
            read_elapsed_ms = (time.perf_counter() - read_started) * 1000.0
            if payload:
                cpu_tensor = torch.frombuffer(bytearray(payload), dtype=torch.uint8)
            else:
                cpu_tensor = torch.zeros(1, dtype=torch.uint8)
            gpu_started = time.perf_counter()
            tensor = cpu_tensor.to(device=device, non_blocking=False).float()
            # Deterministic, cheap GPU work that exercises copy, reduction, and elementwise paths.
            scaled = (tensor * 0.00392156862745098).sin().square()
            checksum_value = float(scaled.mean().item())
            torch.cuda.synchronize(device)
            gpu_elapsed_ms = (time.perf_counter() - gpu_started) * 1000.0
            total_bytes += len(payload)
            rows.append(
                {
                    "gpu_index": local_rank,
                    "gpu_name": gpu_name,
                    "benchmark": record.get("benchmark", ""),
                    "dataset": record.get("dataset", ""),
                    "category": record.get("category", ""),
                    "path": record.get("path", ""),
                    "raw_bytes": int(record.get("raw_bytes", 0)),
                    "processed_bytes": len(payload),
                    "read_ms": round(read_elapsed_ms, 3),
                    "gpu_ms": round(gpu_elapsed_ms, 3),
                    "checksum": round(checksum_value, 8),
                }
            )
        except Exception as exc:  # noqa: BLE001 - preserve per-file failure evidence.
            failures.append(
                {
                    "gpu_index": local_rank,
                    "gpu_name": gpu_name,
                    "benchmark": record.get("benchmark", ""),
                    "dataset": record.get("dataset", ""),
                    "category": record.get("category", ""),
                    "path": record.get("path", ""),
                    "error": repr(exc),
                }
            )

    elapsed_s = time.perf_counter() - started
    queue.put(
        {
            "worker": {
                "gpu_index": local_rank,
                "gpu_name": gpu_name,
                "records": len(records),
                "successes": len(rows),
                "failures": len(failures),
                "processed_bytes": total_bytes,
                "elapsed_s": round(elapsed_s, 6),
                "throughput_mb_s": round(total_bytes / 1_000_000.0 / elapsed_s, 3)
                if elapsed_s > 0
                else 0.0,
                "max_memory_allocated_bytes": int(torch.cuda.max_memory_allocated(device)),
            },
            "rows": rows,
            "failures": failures,
        }
    )


def summarize(rows: list[dict], failures: list[dict], workers: list[dict], elapsed_s: float) -> dict:
    by_dataset: dict[tuple[str, str], dict] = {}
    for row in rows:
        key = (row["benchmark"], row["dataset"])
        item = by_dataset.setdefault(
            key,
            {
                "benchmark": row["benchmark"],
                "dataset": row["dataset"],
                "files": 0,
                "processed_bytes": 0,
                "gpu_ms_total": 0.0,
                "categories": defaultdict(int),
            },
        )
        item["files"] += 1
        item["processed_bytes"] += int(row["processed_bytes"])
        item["gpu_ms_total"] += float(row["gpu_ms"])
        item["categories"][row["category"]] += 1

    dataset_summaries = []
    for item in by_dataset.values():
        dataset_summaries.append(
            {
                "benchmark": item["benchmark"],
                "dataset": item["dataset"],
                "files": item["files"],
                "processed_bytes": item["processed_bytes"],
                "gpu_ms_total": round(item["gpu_ms_total"], 3),
                "categories": dict(sorted(item["categories"].items())),
            }
        )

    return {
        "files": len(rows) + len(failures),
        "successes": len(rows),
        "failures": len(failures),
        "processed_bytes": sum(int(row["processed_bytes"]) for row in rows),
        "elapsed_s": round(elapsed_s, 6),
        "throughput_mb_s": round(
            sum(int(row["processed_bytes"]) for row in rows) / 1_000_000.0 / elapsed_s, 3
        )
        if elapsed_s > 0
        else 0.0,
        "workers": workers,
        "datasets": sorted(dataset_summaries, key=lambda item: (item["benchmark"], item["dataset"])),
    }


def write_readme(output_dir: Path, summary: dict) -> None:
    lines = [
        "# V2X Four-GPU Dataset Benchmark",
        "",
        f"Run ID: `{summary['run_id']}`",
        f"Generated UTC: `{summary['generated_utc']}`",
        "",
        "This benchmark replays staged V2X dataset artifacts through a small CUDA",
        "tensor workload on all visible GPUs. It is a dataset/GPU plumbing and",
        "payload-processing benchmark, not detector AP/IoU.",
        "",
        "## Result",
        "",
        f"- files: `{summary['aggregate']['files']}`",
        f"- successes: `{summary['aggregate']['successes']}`",
        f"- failures: `{summary['aggregate']['failures']}`",
        f"- processed bytes: `{summary['aggregate']['processed_bytes']}`",
        f"- wall time seconds: `{summary['aggregate']['elapsed_s']}`",
        f"- throughput MB/s: `{summary['aggregate']['throughput_mb_s']}`",
        "",
        "## Workers",
        "",
        "| gpu | name | files | failures | processed_bytes | throughput_mb_s | max_mem_bytes |",
        "| ---: | --- | ---: | ---: | ---: | ---: | ---: |",
    ]
    for worker in summary["aggregate"]["workers"]:
        lines.append(
            f"| {worker['gpu_index']} | {worker['gpu_name']} | {worker['successes']} | "
            f"{worker['failures']} | {worker['processed_bytes']} | "
            f"{worker['throughput_mb_s']} | {worker['max_memory_allocated_bytes']} |"
        )
    lines.extend(["", "## Datasets", ""])
    lines.extend(
        [
            "| benchmark | dataset | files | processed_bytes | gpu_ms_total | categories |",
            "| --- | --- | ---: | ---: | ---: | --- |",
        ]
    )
    for item in summary["aggregate"]["datasets"]:
        lines.append(
            f"| {item['benchmark']} | {item['dataset']} | {item['files']} | "
            f"{item['processed_bytes']} | {item['gpu_ms_total']} | "
            f"{json.dumps(item['categories'], sort_keys=True)} |"
        )
    lines.extend(["", "Raw per-file timings are in `per_file.csv`; failures are in `failures.csv`."])
    (output_dir / "README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--results-root", type=Path, default=DEFAULT_RESULTS_ROOT)
    parser.add_argument("--run-id", default="")
    parser.add_argument("--gpus", default=os.environ.get("CUDA_VISIBLE_DEVICES", "0,1,2,3"))
    parser.add_argument("--files-per-dataset", type=int, default=48)
    parser.add_argument("--max-bytes-per-file", type=int, default=1_048_576)
    parser.add_argument("--categories", default="annotation,image,point_cloud")
    args = parser.parse_args()

    manifest = load_manifest(args.manifest)
    data_root = Path(manifest["data_root"]).resolve()
    categories = {item.strip() for item in args.categories.split(",") if item.strip()}
    gpu_ids = [item.strip() for item in args.gpus.split(",") if item.strip()]
    if not gpu_ids:
        raise SystemExit("No GPUs selected.")

    selected = select_records(manifest, args.files_per_dataset, categories)
    if not selected:
        raise SystemExit("No manifest records selected.")

    run_id = args.run_id or "v2x-gpu-dataset-benchmark-" + datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    output_dir = args.results_root.resolve() / run_id
    output_dir.mkdir(parents=True, exist_ok=True)

    env_gpu_ids = ",".join(gpu_ids)
    os.environ["CUDA_VISIBLE_DEVICES"] = env_gpu_ids
    ctx = get_context("spawn")
    queue = ctx.Queue()
    shards = shard_records(selected, len(gpu_ids))

    started = time.perf_counter()
    processes = []
    for local_rank, shard in enumerate(shards):
        process = ctx.Process(
            target=worker_main,
            args=(local_rank, shard, str(data_root), args.max_bytes_per_file, queue),
        )
        process.start()
        processes.append(process)

    payloads = [queue.get() for _ in processes]
    for process in processes:
        process.join()
    bad_exits = [process.exitcode for process in processes if process.exitcode != 0]
    if bad_exits:
        raise SystemExit(f"GPU worker failures: {bad_exits}")

    elapsed_s = time.perf_counter() - started
    rows = [row for payload in payloads for row in payload["rows"]]
    failures = [failure for payload in payloads for failure in payload["failures"]]
    workers = [payload["worker"] for payload in payloads]
    rows.sort(key=lambda item: (item["benchmark"], item["dataset"], item["category"], item["path"]))
    failures.sort(key=lambda item: (item["benchmark"], item["dataset"], item["category"], item["path"]))
    workers.sort(key=lambda item: item["gpu_index"])

    summary = {
        "run_id": run_id,
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "manifest": str(args.manifest.resolve()),
        "data_root": str(data_root),
        "cuda_visible_devices": env_gpu_ids,
        "files_per_dataset": args.files_per_dataset,
        "max_bytes_per_file": args.max_bytes_per_file,
        "categories": sorted(categories),
        "environment": {
            "python": sys.version,
            "nvidia_smi_before_or_after": capture(
                [
                    "nvidia-smi",
                    "--query-gpu=index,name,memory.total,memory.used,utilization.gpu",
                    "--format=csv,noheader",
                ]
            ),
            "gpu_processes": capture(
                [
                    "nvidia-smi",
                    "--query-compute-apps=gpu_uuid,pid,process_name,used_memory",
                    "--format=csv,noheader",
                ]
            ),
        },
        "selected_digest": hashlib.sha256(
            "\n".join(record["path"] for record in selected).encode("utf-8")
        ).hexdigest(),
        "aggregate": summarize(rows, failures, workers, elapsed_s),
    }

    with (output_dir / "summary.json").open("w", encoding="utf-8") as handle:
        json.dump(summary, handle, indent=2, sort_keys=True)
        handle.write("\n")

    with (output_dir / "per_file.csv").open("w", encoding="utf-8", newline="") as handle:
        fieldnames = [
            "gpu_index",
            "gpu_name",
            "benchmark",
            "dataset",
            "category",
            "path",
            "raw_bytes",
            "processed_bytes",
            "read_ms",
            "gpu_ms",
            "checksum",
        ]
        writer = csv.DictWriter(handle, fieldnames=fieldnames, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)

    with (output_dir / "failures.csv").open("w", encoding="utf-8", newline="") as handle:
        fieldnames = ["gpu_index", "gpu_name", "benchmark", "dataset", "category", "path", "error"]
        writer = csv.DictWriter(handle, fieldnames=fieldnames, lineterminator="\n")
        writer.writeheader()
        writer.writerows(failures)

    write_readme(output_dir, summary)
    print(json.dumps(summary["aggregate"], indent=2, sort_keys=True))
    print(f"Wrote {output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
