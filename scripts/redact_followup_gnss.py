#!/usr/bin/env python3
"""Coarsen GNSS CSVs and remove repository-facing rosbag databases."""

from __future__ import annotations

import argparse
import csv
import filecmp
import os
import tempfile
from pathlib import Path


def matching_backup(path: Path, run_root: Path, raw_root: Path) -> Path:
    backup = raw_root / path.relative_to(run_root)
    if not backup.is_file():
        raise RuntimeError(f"missing raw backup for {path}: {backup}")
    if not filecmp.cmp(path, backup, shallow=False):
        raise RuntimeError(f"repository file no longer matches raw backup: {path}")
    return backup


def coarsen_csv(path: Path, decimals: int) -> int:
    mode = path.stat().st_mode
    fd, temporary_name = tempfile.mkstemp(prefix=f".{path.name}.", dir=path.parent)
    os.close(fd)
    temporary = Path(temporary_name)
    rows_with_position = 0
    try:
        with path.open("r", encoding="utf-8", newline="") as source, temporary.open(
            "w", encoding="utf-8", newline=""
        ) as destination:
            reader = csv.DictReader(source)
            if reader.fieldnames is None:
                raise RuntimeError(f"missing CSV header: {path}")
            required = {"latitude_deg", "longitude_deg"}
            if not required.issubset(reader.fieldnames):
                raise RuntimeError(f"missing GNSS coordinate columns: {path}")
            writer = csv.DictWriter(destination, fieldnames=reader.fieldnames, lineterminator="\n")
            writer.writeheader()
            for row in reader:
                if row["latitude_deg"] and row["longitude_deg"]:
                    row["latitude_deg"] = f"{float(row['latitude_deg']):.{decimals}f}"
                    row["longitude_deg"] = f"{float(row['longitude_deg']):.{decimals}f}"
                    rows_with_position += 1
                writer.writerow(row)
        os.chmod(temporary, mode)
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)
    return rows_with_position


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("run_root", type=Path)
    parser.add_argument("raw_backup_root", type=Path)
    parser.add_argument("--decimals", type=int, default=3)
    parser.add_argument("--drop-rosbag-databases", action="store_true")
    args = parser.parse_args()

    run_root = args.run_root.resolve()
    raw_root = args.raw_backup_root.resolve()
    if not run_root.is_dir() or not raw_root.is_dir():
        raise SystemExit("run root and raw backup root must both be directories")
    if run_root == raw_root or run_root == Path("/") or raw_root == Path("/"):
        raise SystemExit("refusing unsafe or identical roots")
    if args.decimals < 0 or args.decimals > 6:
        raise SystemExit("--decimals must be between 0 and 6")

    csv_paths = sorted(run_root.glob("application/**/gps/gps_samples.csv"))
    csv_paths.extend(sorted(run_root.glob("application/**/gps/gps_latest.csv")))
    database_paths = sorted(run_root.glob("application/**/gps/rosbag/*.db3"))
    if not csv_paths:
        raise SystemExit("no GNSS CSVs found")

    for path in [*csv_paths, *database_paths]:
        matching_backup(path, run_root, raw_root)

    position_rows = 0
    for path in csv_paths:
        position_rows += coarsen_csv(path, args.decimals)

    removed_databases = 0
    if args.drop_rosbag_databases:
        for path in database_paths:
            path.unlink()
            removed_databases += 1

    print(f"coarsened_csv_files={len(csv_paths)}")
    print(f"coarsened_position_rows={position_rows}")
    print(f"removed_rosbag_databases={removed_databases}")
    print(f"coordinate_decimals={args.decimals}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
