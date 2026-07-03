#!/usr/bin/env python3
"""Generate TCP or UDP background load for Edge4AV experiments.

The tool is intentionally dependency-free so it can run on the private-5G
vehicle and infrastructure nodes even when iperf3 is unavailable.
"""

from __future__ import annotations

import argparse
import csv
import os
import socket
import sys
import time
from pathlib import Path


def now_ns() -> int:
    return time.time_ns()


def write_row(writer: csv.DictWriter, row: dict) -> None:
    writer.writerow(row)
    sys.stdout.flush()


def make_writer() -> csv.DictWriter:
    fields = [
        "emit_time_ns",
        "role",
        "protocol",
        "host",
        "port",
        "duration_s",
        "target_mbps",
        "bytes",
        "packets",
        "elapsed_s",
        "throughput_mbps",
        "detail",
    ]
    writer = csv.DictWriter(sys.stdout, fieldnames=fields)
    writer.writeheader()
    return writer


def tcp_server(args: argparse.Namespace) -> int:
    writer = make_writer()
    total_bytes = 0
    total_packets = 0
    start = time.monotonic()
    deadline = start + args.duration_s if args.duration_s > 0 else None

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((args.bind, args.port))
        server.listen(args.backlog)
        server.settimeout(0.5)
        while deadline is None or time.monotonic() < deadline:
            try:
                conn, peer = server.accept()
            except socket.timeout:
                continue
            with conn:
                conn.settimeout(0.5)
                while deadline is None or time.monotonic() < deadline:
                    try:
                        chunk = conn.recv(args.packet_bytes)
                    except socket.timeout:
                        continue
                    if not chunk:
                        break
                    total_bytes += len(chunk)
                    total_packets += 1

    elapsed = max(time.monotonic() - start, 1e-9)
    write_row(writer, summary_row(args, "server", total_bytes, total_packets, elapsed, "tcp server complete"))
    return 0


def udp_server(args: argparse.Namespace) -> int:
    writer = make_writer()
    total_bytes = 0
    total_packets = 0
    start = time.monotonic()
    deadline = start + args.duration_s if args.duration_s > 0 else None

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind((args.bind, args.port))
        sock.settimeout(0.5)
        while deadline is None or time.monotonic() < deadline:
            try:
                chunk, _peer = sock.recvfrom(args.packet_bytes)
            except socket.timeout:
                continue
            total_bytes += len(chunk)
            total_packets += 1

    elapsed = max(time.monotonic() - start, 1e-9)
    write_row(writer, summary_row(args, "server", total_bytes, total_packets, elapsed, "udp server complete"))
    return 0


def rate_sleep(start: float, sent_bytes: int, target_mbps: float) -> None:
    if target_mbps <= 0:
        return
    target_elapsed = (sent_bytes * 8.0) / (target_mbps * 1_000_000.0)
    actual_elapsed = time.monotonic() - start
    delay = target_elapsed - actual_elapsed
    if delay > 0:
        time.sleep(min(delay, 0.1))


def tcp_client(args: argparse.Namespace) -> int:
    writer = make_writer()
    payload = os.urandom(args.packet_bytes)
    total_bytes = 0
    total_packets = 0
    start = time.monotonic()
    deadline = start + args.duration_s

    with socket.create_connection((args.host, args.port), timeout=args.connect_timeout_s) as sock:
        while time.monotonic() < deadline:
            sock.sendall(payload)
            total_bytes += len(payload)
            total_packets += 1
            rate_sleep(start, total_bytes, args.target_mbps)

    elapsed = max(time.monotonic() - start, 1e-9)
    write_row(writer, summary_row(args, "client", total_bytes, total_packets, elapsed, "tcp client complete"))
    return 0


def udp_client(args: argparse.Namespace) -> int:
    writer = make_writer()
    payload = os.urandom(args.packet_bytes)
    total_bytes = 0
    total_packets = 0
    start = time.monotonic()
    deadline = start + args.duration_s

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        while time.monotonic() < deadline:
            sock.sendto(payload, (args.host, args.port))
            total_bytes += len(payload)
            total_packets += 1
            rate_sleep(start, total_bytes, args.target_mbps)

    elapsed = max(time.monotonic() - start, 1e-9)
    write_row(writer, summary_row(args, "client", total_bytes, total_packets, elapsed, "udp client complete"))
    return 0


def summary_row(args: argparse.Namespace, role: str, total_bytes: int, total_packets: int, elapsed: float, detail: str) -> dict:
    host = args.host if role == "client" else args.bind
    throughput = (total_bytes * 8.0) / elapsed / 1_000_000.0
    return {
        "emit_time_ns": now_ns(),
        "role": role,
        "protocol": args.protocol,
        "host": host,
        "port": args.port,
        "duration_s": args.duration_s,
        "target_mbps": args.target_mbps,
        "bytes": total_bytes,
        "packets": total_packets,
        "elapsed_s": f"{elapsed:.6f}",
        "throughput_mbps": f"{throughput:.3f}",
        "detail": detail,
    }


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--role", choices=["server", "client"], required=True)
    parser.add_argument("--protocol", choices=["tcp", "udp"], default="tcp")
    parser.add_argument("--bind", default="0.0.0.0")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=39000)
    parser.add_argument("--duration-s", type=float, default=60.0)
    parser.add_argument("--target-mbps", type=float, default=0.0, help="0 means send as fast as possible.")
    parser.add_argument("--packet-bytes", type=int, default=1200)
    parser.add_argument("--backlog", type=int, default=16)
    parser.add_argument("--connect-timeout-s", type=float, default=10.0)
    args = parser.parse_args()

    if args.packet_bytes <= 0:
        raise SystemExit("--packet-bytes must be positive")
    if args.duration_s <= 0 and args.role == "client":
        raise SystemExit("--duration-s must be positive for client mode")
    return args


def main() -> int:
    args = parse_args()
    if args.role == "server" and args.protocol == "tcp":
        return tcp_server(args)
    if args.role == "server" and args.protocol == "udp":
        return udp_server(args)
    if args.role == "client" and args.protocol == "tcp":
        return tcp_client(args)
    return udp_client(args)


if __name__ == "__main__":
    raise SystemExit(main())
