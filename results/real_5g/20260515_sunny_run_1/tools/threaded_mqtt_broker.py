#!/usr/bin/env python3
"""Threaded MQTT 3.1.1 QoS0 broker for large IPI latency probes."""

import argparse
import queue
import socket
import struct
import threading
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


class Broker:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.clients = set()
        self.lock = threading.Lock()

    def add_client(self, client):
        with self.lock:
            self.clients.add(client)

    def remove_client(self, client):
        with self.lock:
            self.clients.discard(client)

    def subscribers(self, topic):
        with self.lock:
            return [client for client in self.clients if topic in client.subscriptions]

    def serve(self):
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((self.host, self.port))
        server.listen(32)
        print(f"threaded mqtt broker listening on {self.host}:{self.port}", flush=True)
        try:
            while True:
                sock, addr = server.accept()
                sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                client = Client(self, sock, addr)
                self.add_client(client)
                client.start()
        finally:
            server.close()


class Client:
    def __init__(self, broker, sock, addr):
        self.broker = broker
        self.sock = sock
        self.addr = addr
        self.subscriptions = set()
        self.outbox = queue.Queue()
        self.closed = threading.Event()

    def start(self):
        print(f"{time.time():.6f} accept {self.addr}", flush=True)
        threading.Thread(target=self.writer, daemon=True).start()
        threading.Thread(target=self.reader, daemon=True).start()

    def send(self, packet):
        if not self.closed.is_set():
            self.outbox.put(packet)

    def close(self, reason):
        if self.closed.is_set():
            return
        self.closed.set()
        self.broker.remove_client(self)
        try:
            self.sock.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass
        try:
            self.sock.close()
        except OSError:
            pass
        print(f"{time.time():.6f} close {self.addr} {reason}", flush=True)

    def writer(self):
        try:
            while not self.closed.is_set():
                packet = self.outbox.get()
                self.sock.sendall(packet)
        except Exception as exc:
            self.close(f"writer {exc}")

    def reader(self):
        try:
            while not self.closed.is_set():
                header = self.sock.recv(1)
                if not header:
                    self.close("closed")
                    return
                packet_type = header[0] & 0xF0
                flags = header[0] & 0x0F
                remaining = read_remaining_length(self.sock)
                if remaining is None:
                    self.close("closed")
                    return
                body = read_exact(self.sock, remaining)
                if body is None:
                    self.close("closed")
                    return

                if packet_type == 0x10:
                    self.send(make_packet(0x20, b"\x00\x00"))
                elif packet_type == 0x80:
                    packet_id = body[:2]
                    offset = 2
                    granted = bytearray()
                    while offset < len(body):
                        topic, offset = read_utf8(body, offset)
                        if offset >= len(body):
                            break
                        offset += 1
                        self.subscriptions.add(topic)
                        granted.append(0)
                        print(f"{time.time():.6f} subscribe {self.addr} {topic}", flush=True)
                    self.send(make_packet(0x90, packet_id + bytes(granted or b"\x00")))
                elif packet_type == 0x30:
                    topic, offset = read_utf8(body, 0)
                    payload = body[offset:]
                    frame = make_packet(0x30 | flags, write_utf8(topic) + payload)
                    peers = self.broker.subscribers(topic)
                    for peer in peers:
                        peer.send(frame)
                    print(
                        f"{time.time():.6f} publish topic={topic} bytes={len(payload)} forwarded={len(peers)}",
                        flush=True,
                    )
                elif packet_type == 0xC0:
                    self.send(make_packet(0xD0))
                elif packet_type == 0xE0:
                    self.close("disconnect")
                    return
        except Exception as exc:
            self.close(f"reader {exc}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=1883)
    args = parser.parse_args()
    Broker(args.host, args.port).serve()


if __name__ == "__main__":
    main()
