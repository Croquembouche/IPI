#!/usr/bin/env python3
"""Remove deployment-specific identifiers from an Airspan follow-up result tree.

The raw result tree must be preserved separately before this script is run.
Repository-facing host telemetry keeps resource counters but drops process
command lines, which can contain credentials, endpoints, and absolute paths.
"""

from __future__ import annotations

import argparse
import ipaddress
import json
import os
import re
import tempfile
from pathlib import Path
from typing import Any


PRIVATE_IPV4_RE = re.compile(r"(?<![\d.])(?:\d{1,3}\.){3}\d{1,3}(?![\d.])")
MAC_RE = re.compile(
    r"(?<![0-9a-f])(?:[0-9a-f]{2}:){5}[0-9a-f]{2}(?![0-9a-f])",
    re.IGNORECASE,
)


def replace_private_ipv4(match: re.Match[str]) -> str:
    value = match.group(0)
    try:
        address = ipaddress.IPv4Address(value)
    except ipaddress.AddressValueError:
        return value
    if address.is_private or address.is_loopback or address.is_link_local:
        return "<private-ip>"
    return value


def sanitize_text(value: str, replacements: list[tuple[str, str]]) -> str:
    for original, replacement in replacements:
        value = value.replace(original, replacement)
    value = PRIVATE_IPV4_RE.sub(replace_private_ipv4, value)
    return MAC_RE.sub("<mac-redacted>", value)


def sanitize_value(value: Any, replacements: list[tuple[str, str]]) -> Any:
    if isinstance(value, str):
        return sanitize_text(value, replacements)
    if isinstance(value, list):
        return [sanitize_value(item, replacements) for item in value]
    if isinstance(value, dict):
        return {
            key: sanitize_value(item, replacements)
            for key, item in value.items()
        }
    return value


def atomic_write(path: Path, text: str) -> None:
    mode = path.stat().st_mode
    fd, temporary_name = tempfile.mkstemp(prefix=f".{path.name}.", dir=path.parent)
    os.close(fd)
    temporary = Path(temporary_name)
    try:
        temporary.write_text(text, encoding="utf-8")
        os.chmod(temporary, mode)
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


def telemetry_role(path: Path) -> str:
    if path.name == "d1.jsonl" or "edge" in path.parts:
        return "d1"
    return "car"


def sanitize_telemetry(
    path: Path, replacements: list[tuple[str, str]]
) -> tuple[int, int]:
    role = telemetry_role(path)
    output: list[str] = []
    rows = 0
    removed_cmdlines = 0
    with path.open("r", encoding="utf-8") as handle:
        for line_number, line in enumerate(handle, start=1):
            if not line.strip():
                continue
            try:
                sample = json.loads(line)
            except json.JSONDecodeError as error:
                raise RuntimeError(f"invalid telemetry JSONL {path}:{line_number}: {error}") from error
            sample["hostname"] = role
            for process in sample.get("processes", []):
                if "cmdline" in process:
                    del process["cmdline"]
                    removed_cmdlines += 1
            sample = sanitize_value(sample, replacements)
            output.append(json.dumps(sample, separators=(",", ":"), sort_keys=False))
            rows += 1
    atomic_write(path, "\n".join(output) + ("\n" if output else ""))
    return rows, removed_cmdlines


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("run_root", type=Path)
    parser.add_argument("--repo-root", required=True)
    parser.add_argument("--edge-deploy-root", required=True)
    parser.add_argument("--edge-run-root", required=True)
    parser.add_argument("--edge-host", required=True)
    parser.add_argument("--gnss-receiver", required=True)
    parser.add_argument("--local-hostname", required=True)
    parser.add_argument("--edge-hostname", required=True)
    args = parser.parse_args()

    run_root = args.run_root.resolve()
    if not run_root.is_dir() or run_root == Path("/"):
        raise SystemExit("run root must be an existing non-root directory")

    replacements = sorted(
        [
            (args.edge_deploy_root, "<edge-deploy-root>"),
            (args.edge_run_root, "<edge-run-root>"),
            (args.repo_root, "<repo-root>"),
            (args.edge_hostname, "d1"),
            (args.local_hostname, "car"),
            (args.gnss_receiver, "<gnss-receiver>"),
            (args.edge_host, "<edge-host>"),
        ],
        key=lambda item: len(item[0]),
        reverse=True,
    )

    telemetry_files = sorted(run_root.glob("application/**/host_telemetry/*.jsonl"))
    if not telemetry_files:
        raise SystemExit("no host telemetry JSONL files found")

    telemetry_rows = 0
    removed_cmdlines = 0
    for path in telemetry_files:
        rows, removed = sanitize_telemetry(path, replacements)
        telemetry_rows += rows
        removed_cmdlines += removed

    telemetry_set = set(telemetry_files)
    sanitized_text_files = 0
    skipped_binary_files = 0
    for path in sorted(run_root.rglob("*")):
        if not path.is_file() or path in telemetry_set or path.name == "SHA256SUMS":
            continue
        data = path.read_bytes()
        if b"\x00" in data:
            skipped_binary_files += 1
            continue
        try:
            original = data.decode("utf-8")
        except UnicodeDecodeError:
            skipped_binary_files += 1
            continue
        sanitized = sanitize_text(original, replacements)
        if sanitized != original:
            atomic_write(path, sanitized)
            sanitized_text_files += 1

    print(f"telemetry_files={len(telemetry_files)}")
    print(f"telemetry_rows={telemetry_rows}")
    print(f"removed_process_cmdlines={removed_cmdlines}")
    print(f"sanitized_other_text_files={sanitized_text_files}")
    print(f"skipped_binary_files={skipped_binary_files}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
