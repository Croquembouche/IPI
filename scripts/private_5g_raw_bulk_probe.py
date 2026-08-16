#!/usr/bin/env python3
"""Raw-byte uplink probe over TCP or MQTT with a compact application ACK."""

from __future__ import annotations

import argparse
import csv
import signal
import socket
import struct
import sys
import time
import zlib
from dataclasses import dataclass
from typing import BinaryIO


REQUEST_MAGIC = b"E4UR"
ACK_MAGIC = b"E4UA"
PROTOCOL_VERSION = 1
REQUEST_HEADER = struct.Struct("!4sB3xQQII")
ACK_HEADER = struct.Struct("!4sBB2xQIIQ")
TCP_LENGTH = struct.Struct("!I")
CSV_FIELDS = (
    "emitter_role",
    "emit_time_ns",
    "run_id",
    "condition_id",
    "transport",
    "source_id",
    "sequence",
    "application_payload_bytes",
    "wire_request_bytes",
    "accepted",
    "client_send_wall_ns",
    "client_receive_wall_ns",
    "rtt_ms",
    "server_processing_ms",
    "payload_crc32",
    "detail",
)


def deterministic_payload(size: int) -> bytes:
    pattern = bytes(range(251))
    return (pattern * ((size + len(pattern) - 1) // len(pattern)))[:size]


def make_request(sequence: int, send_wall_ns: int, payload: bytes) -> bytes:
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    return REQUEST_HEADER.pack(
        REQUEST_MAGIC,
        PROTOCOL_VERSION,
        sequence,
        send_wall_ns,
        len(payload),
        checksum,
    ) + payload


@dataclass(frozen=True)
class Request:
    sequence: int
    client_send_wall_ns: int
    payload: bytes
    payload_crc32: int


def parse_request(data: bytes, max_payload_bytes: int) -> Request:
    if len(data) < REQUEST_HEADER.size:
        raise ValueError("request shorter than header")
    magic, version, sequence, send_wall_ns, payload_bytes, checksum = REQUEST_HEADER.unpack_from(data)
    if magic != REQUEST_MAGIC or version != PROTOCOL_VERSION:
        raise ValueError("request magic/version mismatch")
    if payload_bytes > max_payload_bytes:
        raise ValueError("request payload exceeds configured maximum")
    payload = data[REQUEST_HEADER.size :]
    if len(payload) != payload_bytes:
        raise ValueError("request payload length mismatch")
    actual_checksum = zlib.crc32(payload) & 0xFFFFFFFF
    if actual_checksum != checksum:
        raise ValueError("request payload checksum mismatch")
    return Request(sequence, send_wall_ns, payload, checksum)


@dataclass(frozen=True)
class Ack:
    accepted: bool
    sequence: int
    payload_bytes: int
    payload_crc32: int
    server_processing_ns: int


def make_ack(request: Request, accepted: bool, server_processing_ns: int) -> bytes:
    return ACK_HEADER.pack(
        ACK_MAGIC,
        PROTOCOL_VERSION,
        1 if accepted else 0,
        request.sequence,
        len(request.payload),
        request.payload_crc32,
        server_processing_ns,
    )


def parse_ack(data: bytes) -> Ack:
    if len(data) != ACK_HEADER.size:
        raise ValueError("application acknowledgment length mismatch")
    magic, version, accepted, sequence, payload_bytes, checksum, processing_ns = ACK_HEADER.unpack(data)
    if magic != ACK_MAGIC or version != PROTOCOL_VERSION:
        raise ValueError("application acknowledgment magic/version mismatch")
    return Ack(bool(accepted), sequence, payload_bytes, checksum, processing_ns)


def recv_exact(sock: socket.socket, size: int) -> bytes:
    chunks = bytearray()
    while len(chunks) < size:
        chunk = sock.recv(size - len(chunks))
        if not chunk:
            raise ConnectionError("peer closed before complete message")
        chunks.extend(chunk)
    return bytes(chunks)


def send_tcp_message(sock: socket.socket, message: bytes) -> None:
    sock.sendall(TCP_LENGTH.pack(len(message)))
    sock.sendall(message)


def receive_tcp_message(sock: socket.socket, maximum: int) -> bytes:
    (size,) = TCP_LENGTH.unpack(recv_exact(sock, TCP_LENGTH.size))
    if size == 0 or size > maximum:
        raise ValueError("invalid framed TCP message length")
    return recv_exact(sock, size)


def mqtt_utf8(value: str) -> bytes:
    encoded = value.encode("utf-8")
    if len(encoded) > 65535:
        raise ValueError("MQTT string exceeds 65535 bytes")
    return struct.pack("!H", len(encoded)) + encoded


def mqtt_remaining_length(value: int) -> bytes:
    if value < 0 or value > 268435455:
        raise ValueError("MQTT packet exceeds protocol maximum")
    encoded = bytearray()
    while True:
        byte = value % 128
        value //= 128
        if value:
            byte |= 0x80
        encoded.append(byte)
        if not value:
            return bytes(encoded)


def mqtt_packet(packet_type: int, body: bytes = b"") -> bytes:
    return bytes([packet_type]) + mqtt_remaining_length(len(body)) + body


def read_mqtt_packet(sock: socket.socket) -> tuple[int, int, bytes]:
    first = recv_exact(sock, 1)[0]
    multiplier = 1
    remaining = 0
    for _ in range(4):
        value = recv_exact(sock, 1)[0]
        remaining += (value & 0x7F) * multiplier
        if not value & 0x80:
            return first & 0xF0, first & 0x0F, recv_exact(sock, remaining)
        multiplier *= 128
    raise ValueError("malformed MQTT remaining length")


def read_mqtt_utf8(data: bytes, offset: int = 0) -> tuple[str, int]:
    if offset + 2 > len(data):
        raise ValueError("short MQTT string")
    (size,) = struct.unpack_from("!H", data, offset)
    offset += 2
    if offset + size > len(data):
        raise ValueError("MQTT string exceeds packet")
    return data[offset : offset + size].decode("utf-8", "strict"), offset + size


class MqttClient:
    def __init__(self, host: str, port: int, client_id: str, timeout_s: float):
        self.sock = socket.create_connection((host, port), timeout=timeout_s)
        self.sock.settimeout(timeout_s)
        self.sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        connect_body = mqtt_utf8("MQTT") + b"\x04\x02" + struct.pack("!H", 300) + mqtt_utf8(client_id)
        self.sock.sendall(mqtt_packet(0x10, connect_body))
        packet_type, _, body = read_mqtt_packet(self.sock)
        if packet_type != 0x20 or len(body) != 2 or body[1] != 0:
            raise ConnectionError("MQTT CONNACK rejected")

    def subscribe(self, topic: str, packet_id: int = 1) -> None:
        body = struct.pack("!H", packet_id) + mqtt_utf8(topic) + b"\x00"
        self.sock.sendall(mqtt_packet(0x82, body))
        packet_type, _, response = read_mqtt_packet(self.sock)
        if packet_type != 0x90 or len(response) < 3 or response[:2] != struct.pack("!H", packet_id):
            raise ConnectionError("invalid MQTT SUBACK")

    @staticmethod
    def publish_packet(topic: str, payload: bytes) -> bytes:
        return mqtt_packet(0x30, mqtt_utf8(topic) + payload)

    def receive_publish(self) -> tuple[str, bytes]:
        while True:
            packet_type, _, body = read_mqtt_packet(self.sock)
            if packet_type == 0x30:
                topic, offset = read_mqtt_utf8(body)
                return topic, body[offset:]
            if packet_type == 0xC0:
                self.sock.sendall(mqtt_packet(0xD0))

    def close(self) -> None:
        try:
            self.sock.sendall(mqtt_packet(0xE0))
        except OSError:
            pass
        self.sock.close()


def request_topic(args: argparse.Namespace) -> str:
    return f"edge4av/{args.intersection_id}/raw-uplink/{args.source_id}/request"


def ack_topic(args: argparse.Namespace) -> str:
    return f"edge4av/{args.intersection_id}/raw-uplink/{args.source_id}/ack"


def csv_writer(stream: BinaryIO | object) -> csv.DictWriter:
    writer = csv.DictWriter(stream, fieldnames=CSV_FIELDS, lineterminator="\n")
    writer.writeheader()
    return writer


def base_row(args: argparse.Namespace, role: str, sequence: int) -> dict[str, object]:
    return {
        "emitter_role": role,
        "emit_time_ns": time.time_ns(),
        "run_id": args.run_id,
        "condition_id": args.condition_id,
        "transport": args.transport,
        "source_id": args.source_id,
        "sequence": sequence,
        "application_payload_bytes": "",
        "wire_request_bytes": "",
        "accepted": "",
        "client_send_wall_ns": "",
        "client_receive_wall_ns": "",
        "rtt_ms": "",
        "server_processing_ms": "",
        "payload_crc32": "",
        "detail": "",
    }


def validate_ack(ack: Ack, sequence: int, payload: bytes, checksum: int) -> None:
    if ack.sequence != sequence:
        raise ValueError("application acknowledgment sequence mismatch")
    if ack.payload_bytes != len(payload):
        raise ValueError("application acknowledgment payload length mismatch")
    if ack.payload_crc32 != checksum:
        raise ValueError("application acknowledgment checksum mismatch")
    if not ack.accepted:
        raise ValueError("application receiver rejected payload")


def connect_tcp(args: argparse.Namespace) -> socket.socket:
    sock = socket.create_connection((args.host, args.port), timeout=args.timeout_ms / 1000)
    sock.settimeout(args.timeout_ms / 1000)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    return sock


def run_tcp_sender(args: argparse.Namespace, writer: csv.DictWriter) -> None:
    payload = deterministic_payload(args.payload_bytes)
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    sock: socket.socket | None = None
    for sequence in range(1, args.count + 1):
        row = base_row(args, "raw-uplink-sender", sequence)
        row.update(application_payload_bytes=len(payload), payload_crc32=checksum)
        try:
            if sock is None:
                sock = connect_tcp(args)
            send_wall_ns = time.time_ns()
            request = make_request(sequence, send_wall_ns, payload)
            row.update(client_send_wall_ns=send_wall_ns, wire_request_bytes=TCP_LENGTH.size + len(request))
            start_ns = time.monotonic_ns()
            send_tcp_message(sock, request)
            ack_data = receive_tcp_message(sock, ACK_HEADER.size)
            ack = parse_ack(ack_data)
            finish_ns = time.monotonic_ns()
            receive_wall_ns = time.time_ns()
            validate_ack(ack, sequence, payload, checksum)
            row.update(
                emit_time_ns=receive_wall_ns,
                accepted="true",
                client_receive_wall_ns=receive_wall_ns,
                rtt_ms=f"{(finish_ns - start_ns) / 1_000_000:.6f}",
                server_processing_ms=f"{ack.server_processing_ns / 1_000_000:.6f}",
                detail="validated compact application acknowledgment",
            )
        except Exception as error:  # preserve exactly one row per declared attempt
            row.update(accepted="false", client_receive_wall_ns=time.time_ns(), detail=str(error))
            if sock is not None:
                sock.close()
                sock = None
        writer.writerow(row)
        sys.stdout.flush()
        if sequence != args.count:
            time.sleep(args.interval_ms / 1000)
    if sock is not None:
        sock.close()


def mqtt_sender_client(args: argparse.Namespace) -> MqttClient:
    client = MqttClient(
        args.host,
        args.port,
        f"raw-uplink-sender-{args.source_id}-{time.time_ns()}",
        args.timeout_ms / 1000,
    )
    client.subscribe(ack_topic(args))
    return client


def run_mqtt_sender(args: argparse.Namespace, writer: csv.DictWriter) -> None:
    payload = deterministic_payload(args.payload_bytes)
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    client: MqttClient | None = None
    for sequence in range(1, args.count + 1):
        row = base_row(args, "raw-uplink-sender", sequence)
        row.update(application_payload_bytes=len(payload), payload_crc32=checksum)
        try:
            if client is None:
                client = mqtt_sender_client(args)
            send_wall_ns = time.time_ns()
            request = make_request(sequence, send_wall_ns, payload)
            packet = client.publish_packet(request_topic(args), request)
            row.update(client_send_wall_ns=send_wall_ns, wire_request_bytes=len(packet))
            start_ns = time.monotonic_ns()
            client.sock.sendall(packet)
            topic, ack_data = client.receive_publish()
            ack = parse_ack(ack_data)
            finish_ns = time.monotonic_ns()
            receive_wall_ns = time.time_ns()
            if topic != ack_topic(args):
                raise ValueError("unexpected MQTT acknowledgment topic")
            validate_ack(ack, sequence, payload, checksum)
            row.update(
                emit_time_ns=receive_wall_ns,
                accepted="true",
                client_receive_wall_ns=receive_wall_ns,
                rtt_ms=f"{(finish_ns - start_ns) / 1_000_000:.6f}",
                server_processing_ms=f"{ack.server_processing_ns / 1_000_000:.6f}",
                detail="validated compact application acknowledgment",
            )
        except Exception as error:  # preserve exactly one row per declared attempt
            row.update(accepted="false", client_receive_wall_ns=time.time_ns(), detail=str(error))
            if client is not None:
                client.close()
                client = None
        writer.writerow(row)
        sys.stdout.flush()
        if sequence != args.count:
            time.sleep(args.interval_ms / 1000)
    if client is not None:
        client.close()


def receiver_row(args: argparse.Namespace, request: Request, processing_ns: int) -> dict[str, object]:
    row = base_row(args, "raw-uplink-receiver", request.sequence)
    row.update(
        application_payload_bytes=len(request.payload),
        wire_request_bytes=REQUEST_HEADER.size + len(request.payload),
        accepted="true",
        client_send_wall_ns=request.client_send_wall_ns,
        server_processing_ms=f"{processing_ns / 1_000_000:.6f}",
        payload_crc32=request.payload_crc32,
        detail="validated raw payload length and CRC32",
    )
    return row


def run_tcp_receiver(args: argparse.Namespace, writer: csv.DictWriter) -> None:
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((args.bind, args.port))
    server.listen(16)
    while True:
        conn, _ = server.accept()
        conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        with conn:
            while True:
                try:
                    data = receive_tcp_message(conn, REQUEST_HEADER.size + args.max_payload_bytes)
                except ConnectionError:
                    break
                started = time.monotonic_ns()
                request = parse_request(data, args.max_payload_bytes)
                processing_ns = time.monotonic_ns() - started
                send_tcp_message(conn, make_ack(request, True, processing_ns))
                writer.writerow(receiver_row(args, request, processing_ns))
                sys.stdout.flush()


def run_mqtt_receiver(args: argparse.Namespace, writer: csv.DictWriter) -> None:
    client = MqttClient(
        args.host,
        args.port,
        f"raw-uplink-receiver-{args.source_id}-{time.time_ns()}",
        86400,
    )
    client.subscribe(request_topic(args))
    try:
        while True:
            topic, data = client.receive_publish()
            if topic != request_topic(args):
                continue
            started = time.monotonic_ns()
            request = parse_request(data, args.max_payload_bytes)
            processing_ns = time.monotonic_ns() - started
            client.sock.sendall(client.publish_packet(ack_topic(args), make_ack(request, True, processing_ns)))
            writer.writerow(receiver_row(args, request, processing_ns))
            sys.stdout.flush()
    finally:
        client.close()


def self_test() -> None:
    for size in (0, 1, 1024, 102400, 2097152):
        payload = deterministic_payload(size)
        request = parse_request(make_request(7, 123, payload), 2097152)
        assert request.sequence == 7 and request.payload == payload
        ack = parse_ack(make_ack(request, True, 456))
        validate_ack(ack, 7, payload, zlib.crc32(payload) & 0xFFFFFFFF)
    for value in (0, 127, 128, 16384, 2097152, 268435455):
        encoded = mqtt_remaining_length(value)
        assert 1 <= len(encoded) <= 4
    print("self_test=passed")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--role", choices=("sender", "receiver"))
    parser.add_argument("--transport", choices=("tcp", "mqtt"), default="tcp")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--bind", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=36666)
    parser.add_argument("--count", type=int, default=10)
    parser.add_argument("--interval-ms", type=int, default=1000)
    parser.add_argument("--timeout-ms", type=int, default=60000)
    parser.add_argument("--payload-bytes", type=int, default=1024)
    parser.add_argument("--max-payload-bytes", type=int, default=128 * 1024 * 1024)
    parser.add_argument("--run-id", default="raw-uplink-run")
    parser.add_argument("--condition-id", default="raw-uplink-condition")
    parser.add_argument("--intersection-id", default="intersection-1")
    parser.add_argument("--source-id", default="vehicle-1")
    args = parser.parse_args()
    if args.self_test:
        return args
    if args.role is None:
        parser.error("--role is required unless --self-test is used")
    for name in ("port", "count", "timeout_ms", "max_payload_bytes"):
        if getattr(args, name) <= 0:
            parser.error(f"--{name.replace('_', '-')} must be positive")
    if args.interval_ms < 0 or args.payload_bytes < 0:
        parser.error("interval and payload size cannot be negative")
    if args.payload_bytes > args.max_payload_bytes:
        parser.error("payload size exceeds configured maximum")
    return args


def main() -> int:
    signal.signal(signal.SIGPIPE, signal.SIG_IGN)
    args = parse_args()
    if args.self_test:
        self_test()
        return 0
    writer = csv_writer(sys.stdout)
    if args.role == "sender" and args.transport == "tcp":
        run_tcp_sender(args, writer)
    elif args.role == "sender":
        run_mqtt_sender(args, writer)
    elif args.transport == "tcp":
        run_tcp_receiver(args, writer)
    else:
        run_mqtt_receiver(args, writer)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
