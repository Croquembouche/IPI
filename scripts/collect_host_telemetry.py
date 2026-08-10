#!/usr/bin/env python3
"""Collect low-overhead host telemetry from Linux /proc as JSON Lines."""

from __future__ import annotations

import argparse
import json
import os
import re
import signal
import socket
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Iterable


STOP_REQUESTED = False


def request_stop(_signum: int, _frame: object) -> None:
    global STOP_REQUESTED
    STOP_REQUESTED = True


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Sample CPU, memory, network, and selected process counters."
    )
    parser.add_argument("--output", required=True, help="Destination JSONL path")
    parser.add_argument("--interval-s", type=float, default=1.0)
    parser.add_argument(
        "--duration-s",
        type=float,
        default=0.0,
        help="Stop after this duration; zero means run until signaled",
    )
    parser.add_argument(
        "--interface",
        action="append",
        default=[],
        help="Network interface to retain; repeat as needed (default: all)",
    )
    parser.add_argument(
        "--process-match",
        action="append",
        default=[],
        help="Regex matched against process command lines; repeat as needed",
    )
    args = parser.parse_args()
    if args.interval_s <= 0:
        parser.error("--interval-s must be positive")
    if args.duration_s < 0:
        parser.error("--duration-s cannot be negative")
    return args


def read_cpu_times() -> dict[str, list[int]]:
    result: dict[str, list[int]] = {}
    with open("/proc/stat", "r", encoding="utf-8") as handle:
        for line in handle:
            fields = line.split()
            if not fields or not fields[0].startswith("cpu"):
                continue
            if fields[0] != "cpu" and not fields[0][3:].isdigit():
                continue
            result[fields[0]] = [int(value) for value in fields[1:]]
    return result


def read_named_integers(path: str) -> dict[str, int]:
    result: dict[str, int] = {}
    with open(path, "r", encoding="utf-8") as handle:
        for line in handle:
            fields = line.split()
            if len(fields) < 2:
                continue
            key = fields[0].rstrip(":")
            try:
                result[key] = int(fields[1])
            except ValueError:
                continue
    return result


def read_meminfo() -> dict[str, int]:
    values = read_named_integers("/proc/meminfo")
    retained = (
        "MemTotal",
        "MemFree",
        "MemAvailable",
        "Buffers",
        "Cached",
        "SwapTotal",
        "SwapFree",
        "Dirty",
        "Writeback",
    )
    return {key: values[key] for key in retained if key in values}


def read_vmstat() -> dict[str, int]:
    values = read_named_integers("/proc/vmstat")
    retained = (
        "pgpgin",
        "pgpgout",
        "pswpin",
        "pswpout",
        "pgfault",
        "pgmajfault",
    )
    return {key: values[key] for key in retained if key in values}


def read_netdev(interfaces: set[str]) -> dict[str, dict[str, int]]:
    field_names = (
        "rx_bytes",
        "rx_packets",
        "rx_errors",
        "rx_drops",
        "rx_fifo",
        "rx_frame",
        "rx_compressed",
        "rx_multicast",
        "tx_bytes",
        "tx_packets",
        "tx_errors",
        "tx_drops",
        "tx_fifo",
        "tx_collisions",
        "tx_carrier",
        "tx_compressed",
    )
    result: dict[str, dict[str, int]] = {}
    with open("/proc/net/dev", "r", encoding="utf-8") as handle:
        for line in handle:
            if ":" not in line:
                continue
            name, raw_values = line.split(":", 1)
            name = name.strip()
            if interfaces and name not in interfaces:
                continue
            values = [int(value) for value in raw_values.split()]
            if len(values) == len(field_names):
                result[name] = dict(zip(field_names, values))
    return result


def read_process_io(pid: str) -> dict[str, int]:
    try:
        values = read_named_integers(f"/proc/{pid}/io")
    except (FileNotFoundError, PermissionError, ProcessLookupError):
        return {}
    retained = ("rchar", "wchar", "read_bytes", "write_bytes", "cancelled_write_bytes")
    return {key: values[key] for key in retained if key in values}


def matching_processes(patterns: Iterable[re.Pattern[str]]) -> list[dict[str, Any]]:
    compiled = list(patterns)
    if not compiled:
        return []

    result: list[dict[str, Any]] = []
    for entry in os.scandir("/proc"):
        if not entry.name.isdigit():
            continue
        pid = entry.name
        try:
            raw_cmdline = Path(f"/proc/{pid}/cmdline").read_bytes()
            cmdline = raw_cmdline.replace(b"\0", b" ").decode("utf-8", errors="replace").strip()
            if not cmdline or not any(pattern.search(cmdline) for pattern in compiled):
                continue

            raw_stat = Path(f"/proc/{pid}/stat").read_text(encoding="utf-8")
            closing_paren = raw_stat.rfind(")")
            if closing_paren < 0:
                continue
            comm = raw_stat[raw_stat.find("(") + 1 : closing_paren]
            fields = raw_stat[closing_paren + 2 :].split()
            result.append(
                {
                    "pid": int(pid),
                    "comm": comm,
                    "cmdline": cmdline,
                    "state": fields[0],
                    "utime_ticks": int(fields[11]),
                    "stime_ticks": int(fields[12]),
                    "starttime_ticks": int(fields[19]),
                    "rss_pages": int(fields[21]),
                    "io": read_process_io(pid),
                }
            )
        except (FileNotFoundError, PermissionError, ProcessLookupError, ValueError, IndexError):
            continue
    result.sort(key=lambda item: item["pid"])
    return result


def collect_sample(
    hostname: str,
    interfaces: set[str],
    process_patterns: list[re.Pattern[str]],
) -> dict[str, Any]:
    try:
        load_average = list(os.getloadavg())
    except OSError:
        load_average = []
    return {
        "schema": "edge4av-host-telemetry-v1",
        "hostname": hostname,
        "wall_time_utc": datetime.now(timezone.utc).isoformat(timespec="microseconds"),
        "wall_time_epoch_ns": time.time_ns(),
        "monotonic_ns": time.monotonic_ns(),
        "clock_ticks_per_second": os.sysconf("SC_CLK_TCK"),
        "page_size_bytes": os.sysconf("SC_PAGE_SIZE"),
        "load_average": load_average,
        "cpu_times_ticks": read_cpu_times(),
        "memory_kib": read_meminfo(),
        "vmstat": read_vmstat(),
        "network": read_netdev(interfaces),
        "processes": matching_processes(process_patterns),
    }


def main() -> int:
    args = parse_args()
    signal.signal(signal.SIGINT, request_stop)
    signal.signal(signal.SIGTERM, request_stop)

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    interfaces = set(args.interface)
    process_patterns = [re.compile(pattern) for pattern in args.process_match]
    hostname = socket.gethostname()
    start_monotonic = time.monotonic()
    next_sample = start_monotonic

    with output_path.open("w", encoding="utf-8", buffering=1) as output:
        while not STOP_REQUESTED:
            now = time.monotonic()
            if args.duration_s and now - start_monotonic >= args.duration_s:
                break
            if now < next_sample:
                time.sleep(min(next_sample - now, 0.1))
                continue
            sample = collect_sample(hostname, interfaces, process_patterns)
            output.write(json.dumps(sample, separators=(",", ":")) + "\n")
            next_sample += args.interval_s
            if next_sample < time.monotonic() - args.interval_s:
                next_sample = time.monotonic()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
