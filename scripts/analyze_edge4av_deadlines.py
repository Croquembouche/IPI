#!/usr/bin/env python3
"""Compute deadline-hit statistics from Edge4AV experiment CSV logs."""

from __future__ import annotations

import argparse
import csv
import json
import math
from pathlib import Path


def percentile(values: list[float], pct: float) -> float | None:
    if not values:
        return None
    ordered = sorted(values)
    rank = math.ceil(pct * len(ordered)) - 1
    return ordered[max(0, min(rank, len(ordered) - 1))]


def read_rows(paths: list[Path]) -> list[dict]:
    rows = []
    for path in paths:
        with path.open("r", encoding="utf-8", newline="") as handle:
            reader = csv.DictReader(handle)
            rows.extend(row for row in reader if row.get("rtt_ms"))
    return rows


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", nargs="+", type=Path)
    parser.add_argument("--deadline-ms", type=float, required=True)
    parser.add_argument("--output", type=Path, default=Path("deadline_summary.json"))
    args = parser.parse_args()

    rows = read_rows(args.csv)
    accepted_rows = [row for row in rows if row.get("accepted") == "true"]
    rtts = [float(row["rtt_ms"]) for row in accepted_rows]
    deadline_hits = [value for value in rtts if value <= args.deadline_ms]

    summary = {
        "deadline_ms": args.deadline_ms,
        "input_csvs": [str(path) for path in args.csv],
        "attempts": len(rows),
        "accepted": len(accepted_rows),
        "deadline_hits": len(deadline_hits),
        "accepted_success_rate_pct": round(100.0 * len(accepted_rows) / len(rows), 3) if rows else 0.0,
        "deadline_hit_rate_pct": round(100.0 * len(deadline_hits) / len(rows), 3) if rows else 0.0,
        "deadline_hit_rate_of_accepted_pct": round(100.0 * len(deadline_hits) / len(accepted_rows), 3)
        if accepted_rows
        else 0.0,
        "rtt_ms": {
            "p50": percentile(rtts, 0.50),
            "p95": percentile(rtts, 0.95),
            "p99": percentile(rtts, 0.99),
            "max": max(rtts) if rtts else None,
        },
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
