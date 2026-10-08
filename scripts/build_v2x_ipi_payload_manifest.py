#!/usr/bin/env python3
"""Build an IPI payload manifest from downloaded V2X dataset files."""

from __future__ import annotations

import argparse
import json
import math
import statistics
import time
from collections import Counter, defaultdict
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_DATA_ROOT = REPO_ROOT / "benchmarks" / "v2x" / "data"
DEFAULT_OUTPUT = REPO_ROOT / "results" / "v2x_benchmarks" / "latest" / "v2x_ipi_payload_manifest.json"
DEFAULT_BENCHMARK_PAYLOAD_CAP_BYTES = 2 * 1024 * 1024
DEFAULT_UU_RECEIVER_SAFETY_LIMIT_BYTES = 16 * 1024 * 1024

POINT_CLOUD_EXTENSIONS = {".pcd", ".bin", ".npy", ".npz"}
IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".png"}
ANNOTATION_EXTENSIONS = {".json", ".yaml", ".yml", ".txt", ".pkl"}
IGNORED_DIR_PARTS = {"manifests", "__pycache__"}
IGNORED_FILENAMES = {"SOURCE.txt"}
DATASET_FAMILIES = {
    "DAIR-V2X-C-Example": {
        "benchmark": "OpenDAIR-V2X",
        "dataset": "DAIR-V2X-C-Example",
    },
    "V2X-Seq-SPD-Example": {
        "benchmark": "OpenDAIR-V2X",
        "dataset": "V2X-Seq-SPD-Example",
    },
    "V2X-Seq-TFD-Example": {
        "benchmark": "OpenDAIR-V2X",
        "dataset": "V2X-Seq-TFD-Example",
    },
    "TruckV2X-test-Town1_2": {
        "benchmark": "TruckV2X",
        "dataset": "TruckV2X-test-Town1_2",
    },
    "V2X-Radar-V-ImageSets": {
        "benchmark": "V2X-Radar",
        "dataset": "V2X-Radar-V-ImageSets",
    },
    "V2X-Radar-C-validate": {
        "benchmark": "V2X-Radar",
        "dataset": "V2X-Radar-C-validate",
    },
}


def classify(path: Path) -> str | None:
    suffix = path.suffix.lower()
    if suffix in POINT_CLOUD_EXTENSIONS:
        return "point_cloud"
    if suffix in IMAGE_EXTENSIONS:
        return "image"
    if suffix in ANNOTATION_EXTENSIONS:
        return "annotation"
    return None


def percentile(values: list[int], pct: float) -> int:
    if not values:
        return 0
    ordered = sorted(values)
    rank = math.ceil(pct * len(ordered)) - 1
    return ordered[max(0, min(rank, len(ordered) - 1))]


def summarize_sizes(values: list[int]) -> dict:
    if not values:
        return {
            "count": 0,
            "min_bytes": 0,
            "p50_bytes": 0,
            "p95_bytes": 0,
            "p99_bytes": 0,
            "max_bytes": 0,
            "mean_bytes": 0,
        }
    return {
        "count": len(values),
        "min_bytes": min(values),
        "p50_bytes": percentile(values, 0.50),
        "p95_bytes": percentile(values, 0.95),
        "p99_bytes": percentile(values, 0.99),
        "max_bytes": max(values),
        "mean_bytes": round(statistics.fmean(values), 2),
    }


def iter_dataset_files(root: Path):
    for path in root.rglob("*"):
        if not path.is_file():
            continue
        if path.name in IGNORED_FILENAMES:
            continue
        if any(part in IGNORED_DIR_PARTS for part in path.parts):
            continue
        category = classify(path)
        if category is None:
            continue
        yield path, category, path.stat().st_size


def detect_dataset_family(rel_path: Path) -> dict:
    parts = rel_path.parts
    if len(parts) >= 2 and parts[0] == "extracted":
        family = DATASET_FAMILIES.get(parts[1])
        if family:
            return family
        return {
            "benchmark": "unknown",
            "dataset": parts[1],
        }
    if len(parts) >= 2 and parts[0] == "full":
        return {
            "benchmark": "manual-entrypoint",
            "dataset": parts[1],
        }
    return {
        "benchmark": "unknown",
        "dataset": "unclassified",
    }


def representative_payloads(sizes: list[int], payload_cap_bytes: int) -> list[int]:
    if not sizes:
        return [0, 256, 1024, 4096, 16_384, payload_cap_bytes]
    candidates = {
        0,
        256,
        1024,
        4096,
        min(percentile(sizes, 0.50), payload_cap_bytes),
        min(percentile(sizes, 0.95), payload_cap_bytes),
        min(percentile(sizes, 0.99), payload_cap_bytes),
        min(max(sizes), payload_cap_bytes),
    }
    return sorted(int(value) for value in candidates if value >= 0)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data-root", type=Path, default=DEFAULT_DATA_ROOT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--max-files", type=int, default=0, help="Optional scan cap for quick smoke tests.")
    parser.add_argument(
        "--benchmark-payload-cap-bytes",
        type=int,
        default=DEFAULT_BENCHMARK_PAYLOAD_CAP_BYTES,
        help=(
            "Largest object selected for one benchmark operation. This is a workload "
            "selection cap, not the IPIO wire-format limit."
        ),
    )
    parser.add_argument("--allow-empty", action="store_true")
    args = parser.parse_args()

    if args.benchmark_payload_cap_bytes <= 0:
        raise SystemExit("--benchmark-payload-cap-bytes must be positive")
    if args.benchmark_payload_cap_bytes >= DEFAULT_UU_RECEIVER_SAFETY_LIMIT_BYTES:
        raise SystemExit(
            "--benchmark-payload-cap-bytes must leave room below the 16-MiB "
            "Uu probe packet safety limit"
        )

    root = args.data_root.resolve()
    if not root.exists():
        raise SystemExit(f"Data root does not exist: {root}")

    records = []
    by_category: dict[str, list[int]] = defaultdict(list)
    by_benchmark: dict[str, list[int]] = defaultdict(list)
    by_dataset: dict[tuple[str, str], list[int]] = defaultdict(list)
    by_dataset_category: dict[tuple[str, str], dict[str, list[int]]] = defaultdict(
        lambda: defaultdict(list)
    )
    extensions: Counter[str] = Counter()
    for index, (path, category, size) in enumerate(iter_dataset_files(root), start=1):
        chunk_count = max(1, math.ceil(size / args.benchmark_payload_cap_bytes))
        ipi_payload_bytes = min(size, args.benchmark_payload_cap_bytes)
        rel_path = path.relative_to(root)
        family = detect_dataset_family(rel_path)
        benchmark = family["benchmark"]
        dataset = family["dataset"]
        records.append(
            {
                "path": str(rel_path),
                "benchmark": benchmark,
                "dataset": dataset,
                "category": category,
                "extension": path.suffix.lower(),
                "raw_bytes": size,
                "ipi_payload_bytes_per_message": ipi_payload_bytes,
                "ipi_chunk_count": chunk_count,
                "ipi_transfer_payload_bytes": chunk_count * ipi_payload_bytes
                if size >= args.benchmark_payload_cap_bytes
                else size,
            }
        )
        by_category[category].append(size)
        by_benchmark[benchmark].append(size)
        by_dataset[(benchmark, dataset)].append(size)
        by_dataset_category[(benchmark, dataset)][category].append(size)
        extensions[path.suffix.lower()] += 1
        if args.max_files and index >= args.max_files:
            break

    if not records and not args.allow_empty:
        raise SystemExit(
            "No V2X dataset files found. Run scripts/download_v2x_benchmarks.py first "
            "or pass --allow-empty for a default payload manifest."
        )

    all_sizes = [record["raw_bytes"] for record in records]
    dataset_family_payloads = []
    for (benchmark, dataset), sizes in sorted(by_dataset.items()):
        category_summary = {
            category: summarize_sizes(category_sizes)
            for category, category_sizes in sorted(by_dataset_category[(benchmark, dataset)].items())
        }
        dataset_family_payloads.append(
            {
                "benchmark": benchmark,
                "dataset": dataset,
                "file_count": len(sizes),
                "size_summary": summarize_sizes(sizes),
                "category_size_summary": category_summary,
                "representative_ipi_payload_bytes": representative_payloads(
                    sizes, args.benchmark_payload_cap_bytes
                ),
            }
        )
    manifest = {
        "generated_unix_s": int(time.time()),
        "data_root": str(root),
        "ipi_operation_encoding": "IPIO-v2-section32",
        "ipi_benchmark_payload_cap_bytes": args.benchmark_payload_cap_bytes,
        "ipi_uu_receiver_safety_limit_bytes": DEFAULT_UU_RECEIVER_SAFETY_LIMIT_BYTES,
        "file_count": len(records),
        "extension_counts": dict(sorted(extensions.items())),
        "overall_size_summary": summarize_sizes(all_sizes),
        "benchmark_family_size_summary": {
            benchmark: summarize_sizes(sizes) for benchmark, sizes in sorted(by_benchmark.items())
        },
        "dataset_family_payloads": dataset_family_payloads,
        "category_size_summary": {
            category: summarize_sizes(sizes) for category, sizes in sorted(by_category.items())
        },
        "representative_ipi_payload_bytes": representative_payloads(
            all_sizes, args.benchmark_payload_cap_bytes
        ),
        "records": records,
        "notes": [
            "The benchmark cap selects objects through the evaluated 2-MiB Uu envelope by default; it is not the uint32 IPIO wire limit.",
            "The Uu probe receiver separately enforces a 16-MiB packet safety limit.",
            "ipi_payload_bytes_per_message is suitable for the current IPI loopback sender.",
            "ipi_chunk_count estimates how many benchmark operations a larger raw artifact would require at the selected cap.",
        ],
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8") as handle:
        json.dump(manifest, handle, indent=2, sort_keys=True)
        handle.write("\n")
    print(f"Wrote {args.output}")
    print(json.dumps({k: manifest[k] for k in ("file_count", "overall_size_summary", "representative_ipi_payload_bytes")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
