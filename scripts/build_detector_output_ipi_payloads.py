#!/usr/bin/env python3
"""Estimate IPI payload conditions from detector output CSV files.

The V2X-Radar detector benchmark records per-sample prediction and ground-truth
counts. This script turns those counts into representative cooperative-service
payload sizes so detector-output transport can be evaluated separately from raw
sensor-artifact transfer.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
from datetime import datetime, timezone
from pathlib import Path


DEFAULT_INPUT = Path("results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/per_sample.csv")
DEFAULT_OUTPUT = Path("results/v2x_benchmarks/latest/detector_output_ipi_payloads.json")


def percentile(values: list[int], pct: float) -> int:
    if not values:
        return 0
    ordered = sorted(values)
    rank = math.ceil(pct * len(ordered)) - 1
    return ordered[max(0, min(rank, len(ordered) - 1))]


def parse_rows(path: Path, bytes_per_box: int, base_bytes: int) -> list[dict]:
    rows = []
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            if row.get("status") != "ok":
                continue
            pred_boxes = int(float(row.get("pred_boxes", "0") or 0))
            gt_boxes = int(float(row.get("gt_boxes", "0") or 0))
            payload_bytes = base_bytes + (pred_boxes * bytes_per_box)
            rows.append(
                {
                    "sample_idx": int(row.get("sample_idx", 0)),
                    "gpu_index": row.get("gpu_index", ""),
                    "elapsed_ms": float(row.get("elapsed_ms", "0") or 0),
                    "pred_boxes": pred_boxes,
                    "gt_boxes": gt_boxes,
                    "estimated_payload_bytes": payload_bytes,
                }
            )
    return rows


def representative_payloads(payloads: list[int], max_ipi_payload: int) -> list[int]:
    candidates = {
        0,
        256,
        1024,
        4096,
        percentile(payloads, 0.50),
        percentile(payloads, 0.95),
        percentile(payloads, 0.99),
        max(payloads) if payloads else 0,
        max_ipi_payload,
    }
    return sorted({max(0, min(max_ipi_payload, int(value))) for value in candidates})


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--bytes-per-box", type=int, default=96)
    parser.add_argument("--base-bytes", type=int, default=256)
    parser.add_argument("--max-ipi-payload-bytes", type=int, default=60000)
    args = parser.parse_args()

    rows = parse_rows(args.input, args.bytes_per_box, args.base_bytes)
    payloads = [row["estimated_payload_bytes"] for row in rows]
    summary = {
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "source_csv": str(args.input),
        "bytes_per_box": args.bytes_per_box,
        "base_bytes": args.base_bytes,
        "max_ipi_payload_bytes": args.max_ipi_payload_bytes,
        "sample_count": len(rows),
        "payload_size_summary": {
            "min": min(payloads) if payloads else 0,
            "p50": percentile(payloads, 0.50),
            "p95": percentile(payloads, 0.95),
            "p99": percentile(payloads, 0.99),
            "max": max(payloads) if payloads else 0,
        },
        "representative_ipi_payload_bytes": representative_payloads(payloads, args.max_ipi_payload_bytes),
        "records": rows,
        "notes": [
            "Payload sizes are estimates for detector-output objects, not raw sensor payloads.",
            "Use representative_ipi_payload_bytes with scripts/run_v2x_ipi_loopback.py or the private-5G sender.",
        ],
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(args.output)
    print(json.dumps({k: summary[k] for k in ("sample_count", "payload_size_summary", "representative_ipi_payload_bytes")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
