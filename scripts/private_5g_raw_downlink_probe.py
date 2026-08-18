#!/usr/bin/env python3
"""Request/response downlink probe over TCP or MQTT.

The vehicle sends a compact request.  The edge returns an exact deterministic
application body.  RTT ends only after the complete body, length, sequence,
and CRC32 have been validated at the vehicle.
"""

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


REQUEST_MAGIC = b"E4DR"
RESPONSE_MAGIC = b"E4DS"
PROTOCOL_VERSION = 1
REQUEST_HEADER = struct.Struct("!4sB3xQQI")
RESPONSE_HEADER = struct.Struct("!4sBB2xQIIQ")
TCP_LENGTH = struct.Struct("!I")
CSV_FIELDS = (
    "emitter_role", "emit_time_ns", "run_id", "condition_id", "transport",
    "source_id", "sequence", "application_payload_bytes",
    "wire_request_bytes", "wire_response_bytes", "accepted",
    "client_send_wall_ns", "client_receive_wall_ns", "rtt_ms",
    "server_processing_ms", "payload_crc32", "detail",
)


def deterministic_payload(size: int) -> bytes:
    pattern = bytes(range(251))
    return (pattern * ((size + len(pattern) - 1) // len(pattern)))[:size]


@dataclass(frozen=True)
class Request:
    sequence: int
    client_send_wall_ns: int
    response_bytes: int


def make_request(sequence: int, send_wall_ns: int, response_bytes: int) -> bytes:
    return REQUEST_HEADER.pack(
        REQUEST_MAGIC, PROTOCOL_VERSION, sequence, send_wall_ns, response_bytes
    )


def parse_request(data: bytes, max_payload_bytes: int) -> Request:
    if len(data) != REQUEST_HEADER.size:
        raise ValueError("downlink request length mismatch")
    magic, version, sequence, send_wall_ns, response_bytes = REQUEST_HEADER.unpack(data)
    if magic != REQUEST_MAGIC or version != PROTOCOL_VERSION:
        raise ValueError("downlink request magic/version mismatch")
    if response_bytes > max_payload_bytes:
        raise ValueError("requested response exceeds configured maximum")
    return Request(sequence, send_wall_ns, response_bytes)


@dataclass(frozen=True)
class Response:
    accepted: bool
    sequence: int
    payload: bytes
    payload_crc32: int
    server_processing_ns: int


def make_response(request: Request) -> tuple[bytes, int]:
    started = time.monotonic_ns()
    payload = deterministic_payload(request.response_bytes)
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    processing_ns = time.monotonic_ns() - started
    header = RESPONSE_HEADER.pack(
        RESPONSE_MAGIC, PROTOCOL_VERSION, 1, request.sequence,
        len(payload), checksum, processing_ns,
    )
    return header + payload, processing_ns


def parse_response(data: bytes, max_payload_bytes: int) -> Response:
    if len(data) < RESPONSE_HEADER.size:
        raise ValueError("downlink response shorter than header")
    magic, version, accepted, sequence, payload_bytes, checksum, processing_ns = (
        RESPONSE_HEADER.unpack_from(data)
    )
    if magic != RESPONSE_MAGIC or version != PROTOCOL_VERSION:
        raise ValueError("downlink response magic/version mismatch")
    if payload_bytes > max_payload_bytes:
        raise ValueError("downlink response exceeds configured maximum")
    payload = data[RESPONSE_HEADER.size:]
    if len(payload) != payload_bytes:
        raise ValueError("downlink response payload length mismatch")
    if (zlib.crc32(payload) & 0xFFFFFFFF) != checksum:
        raise ValueError("downlink response payload checksum mismatch")
    return Response(bool(accepted), sequence, payload, checksum, processing_ns)


def validate_response(response: Response, sequence: int, expected_bytes: int,
                      expected_crc32: int) -> None:
    if not response.accepted:
        raise ValueError("edge rejected downlink request")
    if response.sequence != sequence:
        raise ValueError("downlink response sequence mismatch")
    if len(response.payload) != expected_bytes:
        raise ValueError("downlink response application length mismatch")
    if response.payload_crc32 != expected_crc32:
        raise ValueError("downlink response expected checksum mismatch")


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
    return data[offset:offset + size].decode("utf-8", "strict"), offset + size


class MqttClient:
    def __init__(self, host: str, port: int, client_id: str, timeout_s: float):
        self.sock = socket.create_connection((host, port), timeout=timeout_s)
        self.sock.settimeout(timeout_s)
        self.sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        body = mqtt_utf8("MQTT") + b"\x04\x02" + struct.pack("!H", 300) + mqtt_utf8(client_id)
        self.sock.sendall(mqtt_packet(0x10, body))
        packet_type, _, response = read_mqtt_packet(self.sock)
        if packet_type != 0x20 or len(response) != 2 or response[1] != 0:
            raise ConnectionError("MQTT CONNACK rejected")

    def subscribe(self, topic: str, packet_id: int = 1) -> None:
        self.sock.sendall(mqtt_packet(0x82, struct.pack("!H", packet_id) + mqtt_utf8(topic) + b"\x00"))
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
    return f"edge4av/{args.intersection_id}/raw-downlink/{args.source_id}/request"


def response_topic(args: argparse.Namespace) -> str:
    return f"edge4av/{args.intersection_id}/raw-downlink/{args.source_id}/response"


def csv_writer(stream: BinaryIO | object) -> csv.DictWriter:
    writer = csv.DictWriter(stream, fieldnames=CSV_FIELDS, lineterminator="\n")
    writer.writeheader()
    return writer


def base_row(args: argparse.Namespace, role: str, sequence: int) -> dict[str, object]:
    row: dict[str, object] = {field: "" for field in CSV_FIELDS}
    row.update(
        emitter_role=role,
        emit_time_ns=time.time_ns(),
        run_id=args.run_id,
        condition_id=args.condition_id,
        transport=args.transport,
        source_id=args.source_id,
        sequence=sequence,
    )
    return row


def connect_tcp(args: argparse.Namespace) -> socket.socket:
    sock = socket.create_connection((args.host, args.port), timeout=args.timeout_ms / 1000)
    sock.settimeout(args.timeout_ms / 1000)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    return sock


def complete_sender_row(args: argparse.Namespace, row: dict[str, object],
                        response: Response, finish_ns: int, start_ns: int,
                        wire_response_bytes: int) -> None:
    receive_wall_ns = time.time_ns()
    row.update(
        emit_time_ns=receive_wall_ns, accepted="true",
        application_payload_bytes=len(response.payload),
        wire_response_bytes=wire_response_bytes,
        client_receive_wall_ns=receive_wall_ns,
        rtt_ms=f"{(finish_ns - start_ns) / 1_000_000:.6f}",
        server_processing_ms=f"{response.server_processing_ns / 1_000_000:.6f}",
        payload_crc32=response.payload_crc32,
        detail="validated complete downlink response length, sequence, and CRC32",
    )


def run_tcp_sender(args: argparse.Namespace, writer: csv.DictWriter) -> None:
    expected_crc = zlib.crc32(deterministic_payload(args.payload_bytes)) & 0xFFFFFFFF
    sock: socket.socket | None = None
    for sequence in range(1, args.count + 1):
        row = base_row(args, "raw-downlink-requester", sequence)
        try:
            if sock is None:
                sock = connect_tcp(args)
            send_wall_ns = time.time_ns()
            request = make_request(sequence, send_wall_ns, args.payload_bytes)
            row.update(client_send_wall_ns=send_wall_ns, wire_request_bytes=TCP_LENGTH.size + len(request))
            start_ns = time.monotonic_ns()
            send_tcp_message(sock, request)
            data = receive_tcp_message(sock, RESPONSE_HEADER.size + args.max_payload_bytes)
            response = parse_response(data, args.max_payload_bytes)
            finish_ns = time.monotonic_ns()
            validate_response(response, sequence, args.payload_bytes, expected_crc)
            complete_sender_row(args, row, response, finish_ns, start_ns, TCP_LENGTH.size + len(data))
        except Exception as error:
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


def run_mqtt_sender(args: argparse.Namespace, writer: csv.DictWriter) -> None:
    expected_crc = zlib.crc32(deterministic_payload(args.payload_bytes)) & 0xFFFFFFFF
    client: MqttClient | None = None
    for sequence in range(1, args.count + 1):
        row = base_row(args, "raw-downlink-requester", sequence)
        try:
            if client is None:
                client = MqttClient(args.host, args.port, f"raw-downlink-requester-{args.source_id}-{time.time_ns()}", args.timeout_ms / 1000)
                client.subscribe(response_topic(args))
            send_wall_ns = time.time_ns()
            request = make_request(sequence, send_wall_ns, args.payload_bytes)
            packet = client.publish_packet(request_topic(args), request)
            row.update(client_send_wall_ns=send_wall_ns, wire_request_bytes=len(packet))
            start_ns = time.monotonic_ns()
            client.sock.sendall(packet)
            topic, data = client.receive_publish()
            response = parse_response(data, args.max_payload_bytes)
            finish_ns = time.monotonic_ns()
            if topic != response_topic(args):
                raise ValueError("unexpected MQTT downlink response topic")
            validate_response(response, sequence, args.payload_bytes, expected_crc)
            wire_response = len(client.publish_packet(response_topic(args), data))
            complete_sender_row(args, row, response, finish_ns, start_ns, wire_response)
        except Exception as error:
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


def receiver_row(args: argparse.Namespace, request: Request, checksum: int,
                 processing_ns: int, wire_response_bytes: int) -> dict[str, object]:
    row = base_row(args, "raw-downlink-responder", request.sequence)
    row.update(
        application_payload_bytes=request.response_bytes,
        wire_request_bytes=REQUEST_HEADER.size,
        wire_response_bytes=wire_response_bytes, accepted="true",
        client_send_wall_ns=request.client_send_wall_ns,
        server_processing_ms=f"{processing_ns / 1_000_000:.6f}",
        payload_crc32=checksum,
        detail="served exact downlink response body with length and CRC32",
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
                    data = receive_tcp_message(conn, REQUEST_HEADER.size)
                except ConnectionError:
                    break
                request = parse_request(data, args.max_payload_bytes)
                response, processing_ns = make_response(request)
                send_tcp_message(conn, response)
                checksum = zlib.crc32(response[RESPONSE_HEADER.size:]) & 0xFFFFFFFF
                writer.writerow(receiver_row(args, request, checksum, processing_ns, TCP_LENGTH.size + len(response)))
                sys.stdout.flush()


def run_mqtt_receiver(args: argparse.Namespace, writer: csv.DictWriter) -> None:
    client = MqttClient(args.host, args.port, f"raw-downlink-responder-{args.source_id}-{time.time_ns()}", 86400)
    client.subscribe(request_topic(args))
    try:
        while True:
            topic, data = client.receive_publish()
            if topic != request_topic(args):
                continue
            request = parse_request(data, args.max_payload_bytes)
            response, processing_ns = make_response(request)
            packet = client.publish_packet(response_topic(args), response)
            client.sock.sendall(packet)
            checksum = zlib.crc32(response[RESPONSE_HEADER.size:]) & 0xFFFFFFFF
            writer.writerow(receiver_row(args, request, checksum, processing_ns, len(packet)))
            sys.stdout.flush()
    finally:
        client.close()


def self_test() -> None:
    for size in (0, 1, 1024, 102400, 512000):
        request = parse_request(make_request(7, 123, size), 512000)
        response_data, _ = make_response(request)
        response = parse_response(response_data, 512000)
        expected_crc = zlib.crc32(deterministic_payload(size)) & 0xFFFFFFFF
        validate_response(response, 7, size, expected_crc)
    for value in (0, 127, 128, 16384, 512000, 268435455):
        assert 1 <= len(mqtt_remaining_length(value)) <= 4
    print("self_test=passed")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--role", choices=("sender", "receiver"))
    parser.add_argument("--transport", choices=("tcp", "mqtt"), default="tcp")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--bind", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=37666)
    parser.add_argument("--count", type=int, default=10)
    parser.add_argument("--interval-ms", type=int, default=200)
    parser.add_argument("--timeout-ms", type=int, default=60000)
    parser.add_argument("--payload-bytes", type=int, default=1024)
    parser.add_argument("--max-payload-bytes", type=int, default=1024 * 1024)
    parser.add_argument("--run-id", default="raw-downlink-run")
    parser.add_argument("--condition-id", default="raw-downlink-condition")
    parser.add_argument("--intersection-id", default="intersection-1")
    parser.add_argument("--source-id", default="vehicle-1")
    args = parser.parse_args()
    if not args.self_test and args.role is None:
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
