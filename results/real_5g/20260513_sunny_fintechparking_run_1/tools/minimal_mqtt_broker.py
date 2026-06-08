#!/usr/bin/env python3
"""Minimal MQTT 3.1.1 QoS0 broker for isolated IPI latency experiments."""

import argparse
import select
import socket
import struct
import time


def encode_remaining_length(value):
    out = bytearray()
    while True:
        encoded = value % 128
        value //= 128
        if value:
            encoded |= 128
        out.append(encoded)
        if not value:
            return bytes(out)


def make_packet(packet_type, body=b""):
    return bytes([packet_type]) + encode_remaining_length(len(body)) + body


def read_remaining_length(sock):
    multiplier = 1
    value = 0
    while True:
        b = sock.recv(1)
        if not b:
            return None
        encoded = b[0]
        value += (encoded & 127) * multiplier
        if not encoded & 128:
            return value
        multiplier *= 128
        if multiplier > 128**4:
            raise RuntimeError("malformed remaining length")


def read_exact(sock, size):
    chunks = bytearray()
    while len(chunks) < size:
        chunk = sock.recv(size - len(chunks))
        if not chunk:
            return None
        chunks.extend(chunk)
    return bytes(chunks)


def read_utf8(buf, offset):
    if offset + 2 > len(buf):
        raise RuntimeError("short mqtt string")
    size = struct.unpack("!H", buf[offset : offset + 2])[0]
    offset += 2
    if offset + size > len(buf):
        raise RuntimeError("mqtt string exceeds packet")
    return buf[offset : offset + size].decode("utf-8", "replace"), offset + size


def write_utf8(value):
    data = value.encode("utf-8")
    return struct.pack("!H", len(data)) + data


class Client:
    def __init__(self, sock, addr):
        self.sock = sock
        self.addr = addr
        self.subscriptions = set()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=1883)
    args = parser.parse_args()

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((args.host, args.port))
    server.listen(16)
    clients = {}
    print(f"minimal mqtt broker listening on {args.host}:{args.port}", flush=True)

    try:
        while True:
            readable, _, _ = select.select([server] + [c.sock for c in clients.values()], [], [], 1.0)
            for sock in readable:
                if sock is server:
                    conn, addr = server.accept()
                    conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                    clients[conn] = Client(conn, addr)
                    print(f"{time.time():.6f} accept {addr}", flush=True)
                    continue

                client = clients.get(sock)
                try:
                    header = sock.recv(1)
                    if not header:
                        raise ConnectionError("closed")
                    packet_type = header[0] & 0xF0
                    flags = header[0] & 0x0F
                    remaining = read_remaining_length(sock)
                    if remaining is None:
                        raise ConnectionError("closed")
                    body = read_exact(sock, remaining)
                    if body is None:
                        raise ConnectionError("closed")

                    if packet_type == 0x10:
                        sock.sendall(make_packet(0x20, b"\x00\x00"))
                    elif packet_type == 0x80:
                        packet_id = body[:2]
                        offset = 2
                        granted = bytearray()
                        while offset < len(body):
                            topic, offset = read_utf8(body, offset)
                            if offset >= len(body):
                                break
                            offset += 1
                            client.subscriptions.add(topic)
                            granted.append(0)
                            print(f"{time.time():.6f} subscribe {client.addr} {topic}", flush=True)
                        sock.sendall(make_packet(0x90, packet_id + bytes(granted or b"\x00")))
                    elif packet_type == 0x30:
                        topic, offset = read_utf8(body, 0)
                        payload = body[offset:]
                        forwarded = 0
                        frame = make_packet(0x30 | flags, write_utf8(topic) + payload)
                        for peer in list(clients.values()):
                            if topic in peer.subscriptions:
                                peer.sock.sendall(frame)
                                forwarded += 1
                        print(
                            f"{time.time():.6f} publish topic={topic} bytes={len(payload)} forwarded={forwarded}",
                            flush=True,
                        )
                    elif packet_type == 0xC0:
                        sock.sendall(make_packet(0xD0))
                    elif packet_type == 0xE0:
                        raise ConnectionError("disconnect")
                except Exception as exc:
                    if client is not None:
                        print(f"{time.time():.6f} close {client.addr} {exc}", flush=True)
                    clients.pop(sock, None)
                    try:
                        sock.close()
                    except OSError:
                        pass
    finally:
        for client in clients.values():
            client.sock.close()
        server.close()


if __name__ == "__main__":
    main()
