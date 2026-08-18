#!/usr/bin/env python3
"""Measure an exact validated TCP bulk transfer in either direction."""

from __future__ import annotations

import argparse
import json
import signal
import socket
import struct
import sys
import time
import zlib


REQUEST_MAGIC = b"E4BT"
ACK_MAGIC = b"E4BA"
VERSION = 1
DIRECTION = {"upload": 1, "download": 2}
REQUEST = struct.Struct("!4sBBHQ")
ACK = struct.Struct("!4sBBHQQI")


def payload_block(size: int) -> bytes:
    pattern = bytes(range(251))
    return (pattern * ((size + len(pattern) - 1) // len(pattern)))[:size]


def recv_exact(sock: socket.socket, size: int) -> bytes:
    data = bytearray()
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            raise ConnectionError("peer closed before complete control message")
        data.extend(chunk)
    return bytes(data)


def send_payload(sock: socket.socket, total_bytes: int, block_bytes: int) -> tuple[int, int]:
    block = payload_block(block_bytes)
    remaining = total_bytes
    checksum = 0
    started_ns = time.monotonic_ns()
    while remaining:
        chunk = block if remaining >= len(block) else block[:remaining]
        sock.sendall(chunk)
        checksum = zlib.crc32(chunk, checksum)
        remaining -= len(chunk)
    elapsed_ns = time.monotonic_ns() - started_ns
    return elapsed_ns, checksum & 0xFFFFFFFF


def receive_payload(sock: socket.socket, total_bytes: int, block_bytes: int) -> tuple[int, int]:
    remaining = total_bytes
    checksum = 0
    started_ns = None
    while remaining:
        chunk = sock.recv(min(block_bytes, remaining))
        if not chunk:
            raise ConnectionError("peer closed before exact payload was received")
        if started_ns is None:
            started_ns = time.monotonic_ns()
        checksum = zlib.crc32(chunk, checksum)
        remaining -= len(chunk)
    if started_ns is None:
        started_ns = time.monotonic_ns()
    return time.monotonic_ns() - started_ns, checksum & 0xFFFFFFFF


def make_ack(total_bytes: int, elapsed_ns: int, checksum: int) -> bytes:
    return ACK.pack(ACK_MAGIC, VERSION, 1, 0, total_bytes, elapsed_ns, checksum)


def parse_ack(data: bytes, expected_bytes: int, expected_crc: int) -> int:
    magic, version, accepted, _reserved, total_bytes, elapsed_ns, checksum = ACK.unpack(data)
    if magic != ACK_MAGIC or version != VERSION or not accepted:
        raise ValueError("invalid or rejected transfer acknowledgment")
    if total_bytes != expected_bytes:
        raise ValueError("acknowledged byte count mismatch")
    if checksum != expected_crc:
        raise ValueError("acknowledged CRC32 mismatch")
    return elapsed_ns


def throughput_mbps(total_bytes: int, elapsed_ns: int) -> float:
    return total_bytes * 8.0 * 1000.0 / max(elapsed_ns, 1)


def result(args: argparse.Namespace, role: str, local_elapsed_ns: int,
           peer_elapsed_ns: int, completion_elapsed_ns: int, checksum: int) -> dict:
    return {
        "schema": "edge4av-exact-tcp-bulk-throughput-v1",
        "emit_time_ns": time.time_ns(),
        "role": role,
        "direction": args.direction,
        "total_bytes": args.total_bytes,
        "block_bytes": args.block_bytes,
        "local_data_elapsed_s": local_elapsed_ns / 1_000_000_000,
        "local_data_throughput_mbps": throughput_mbps(args.total_bytes, local_elapsed_ns),
        "peer_data_elapsed_s": peer_elapsed_ns / 1_000_000_000,
        "peer_data_throughput_mbps": throughput_mbps(args.total_bytes, peer_elapsed_ns),
        "request_to_completion_s": completion_elapsed_ns / 1_000_000_000,
        "payload_crc32": checksum,
        "validated_exact_bytes": True,
    }


def handle_server(args: argparse.Namespace, conn: socket.socket) -> dict:
    request_data = recv_exact(conn, REQUEST.size)
    magic, version, direction, _reserved, total_bytes = REQUEST.unpack(request_data)
    if magic != REQUEST_MAGIC or version != VERSION:
        raise ValueError("request magic/version mismatch")
    if direction != DIRECTION[args.direction] or total_bytes != args.total_bytes:
        raise ValueError("request direction/byte count mismatch")
    completion_start = time.monotonic_ns()
    if args.direction == "upload":
        local_elapsed, checksum = receive_payload(conn, total_bytes, args.block_bytes)
        conn.sendall(make_ack(total_bytes, local_elapsed, checksum))
        peer_elapsed = 0
    else:
        local_elapsed, checksum = send_payload(conn, total_bytes, args.block_bytes)
        ack = recv_exact(conn, ACK.size)
        peer_elapsed = parse_ack(ack, total_bytes, checksum)
    completion_elapsed = time.monotonic_ns() - completion_start
    return result(args, "server", local_elapsed, peer_elapsed, completion_elapsed, checksum)


def run_server(args: argparse.Namespace) -> dict:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((args.bind, args.port))
        server.listen(1)
        conn, peer = server.accept()
        with conn:
            conn.settimeout(args.timeout_s)
            output = handle_server(args, conn)
            output["peer_address"] = peer[0]
            return output


def run_client(args: argparse.Namespace) -> dict:
    with socket.create_connection((args.host, args.port), timeout=args.timeout_s) as sock:
        sock.settimeout(args.timeout_s)
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        request = REQUEST.pack(
            REQUEST_MAGIC, VERSION, DIRECTION[args.direction], 0, args.total_bytes
        )
        completion_start = time.monotonic_ns()
        sock.sendall(request)
        if args.direction == "upload":
            local_elapsed, checksum = send_payload(sock, args.total_bytes, args.block_bytes)
            ack = recv_exact(sock, ACK.size)
            peer_elapsed = parse_ack(ack, args.total_bytes, checksum)
        else:
            local_elapsed, checksum = receive_payload(sock, args.total_bytes, args.block_bytes)
            sock.sendall(make_ack(args.total_bytes, local_elapsed, checksum))
            peer_elapsed = 0
        completion_elapsed = time.monotonic_ns() - completion_start
        return result(args, "client", local_elapsed, peer_elapsed, completion_elapsed, checksum)


def self_test() -> None:
    block = payload_block(65536)
    checksum = zlib.crc32(block) & 0xFFFFFFFF
    elapsed = parse_ack(make_ack(len(block), 1234, checksum), len(block), checksum)
    assert elapsed == 1234
    assert abs(throughput_mbps(1_000_000, 1_000_000_000) - 8.0) < 1e-12
    print("self_test=passed")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--role", choices=("server", "client"))
    parser.add_argument("--direction", choices=("upload", "download"), default="upload")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--bind", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=39050)
    parser.add_argument("--total-bytes", type=int, default=50 * 1024 * 1024)
    parser.add_argument("--block-bytes", type=int, default=64 * 1024)
    parser.add_argument("--timeout-s", type=float, default=300.0)
    args = parser.parse_args()
    if not args.self_test and args.role is None:
        parser.error("--role is required unless --self-test is used")
    if args.port <= 0 or args.total_bytes <= 0 or args.block_bytes <= 0 or args.timeout_s <= 0:
        parser.error("port, byte counts, and timeout must be positive")
    return args


def main() -> int:
    signal.signal(signal.SIGPIPE, signal.SIG_IGN)
    args = parse_args()
    if args.self_test:
        self_test()
        return 0
    output = run_server(args) if args.role == "server" else run_client(args)
    json.dump(output, sys.stdout, indent=2, sort_keys=True)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
