#!/usr/bin/env python3
"""Regenerate IPI UPER vectors with two ASN.1 tools and a J2735 package."""

from __future__ import annotations

import argparse
import contextlib
import hashlib
import importlib
import importlib.util
import io
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def parse_args() -> argparse.Namespace:
    repository = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--schema", type=Path,
                        default=repository / "cpp" / "asn1" / "IPI.asn")
    parser.add_argument("--dsrc-stub", type=Path,
                        default=repository / "cpp" / "asn1" / "test" / "DSRC.asn")
    parser.add_argument("--vectors", type=Path,
                        default=repository / "cpp" / "tests" / "data" /
                                "j2735_ipi_conformance_vectors.json")
    parser.add_argument("--j2735-package", default="j2735_202409",
                        help="installed pycrate J2735 Python package")
    return parser.parse_args()


def decode_hex_fields(value: object) -> object:
    if isinstance(value, list):
        return [decode_hex_fields(item) for item in value]
    if not isinstance(value, dict):
        return value
    converted: dict[str, object] = {}
    for key, item in value.items():
        if key.endswith("Hex"):
            converted[key[:-3]] = bytes.fromhex(str(item))
        elif key == "servicePayload":
            payload = dict(item)
            choice = str(payload.pop("choice"))
            converted[key] = (choice, decode_hex_fields(payload))
        else:
            converted[key] = decode_hex_fields(item)
    return converted


def load_generated(path: Path):
    spec = importlib.util.spec_from_file_location("ipi_reference_schema", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load generated pycrate module: {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    args = parse_args()
    try:
        import asn1tools
    except ImportError as error:
        raise RuntimeError("asn1tools is required (validated version: 0.167.0)") from error
    compiler = shutil.which("pycrate_asn1compile.py")
    if compiler is None:
        raise RuntimeError("pycrate_asn1compile.py is required (validated pycrate: 0.7.11)")

    records = json.loads(args.vectors.read_text(encoding="utf-8"))
    schema_hash = hashlib.sha256(args.schema.read_bytes()).hexdigest()
    if schema_hash != records["schema_sha256"]:
        raise RuntimeError("IPI schema hash does not match the vector manifest")

    asn1tools_codec = asn1tools.compile_files(
        [str(args.dsrc_stub), str(args.schema)], codec="uper")
    j2735 = importlib.import_module(args.j2735_package)
    message_frame = getattr(j2735, "MessageFrame").MessageFrame

    with tempfile.TemporaryDirectory(prefix="ipi-j2735-reference-") as directory:
        generated_base = Path(directory) / "ipi_reference"
        result = subprocess.run(
            [compiler, "-i", str(args.dsrc_stub), str(args.schema),
             "-o", str(generated_base)],
            check=False,
            text=True,
            capture_output=True,
        )
        if result.returncode != 0:
            raise RuntimeError("pycrate schema compilation failed:\n" + result.stderr)
        pycrate_schema = load_generated(generated_base.with_suffix(".py"))
        pycrate_value = pycrate_schema.IPI.IpiCooperativeService

        for vector in records["vectors"]:
            name = vector["name"]
            value = decode_hex_fields(vector["value"])
            encoded = asn1tools_codec.encode("IpiCooperativeService", value)
            pycrate_value.set_val(value)
            independently_encoded = pycrate_value.to_uper()
            if encoded != independently_encoded:
                raise RuntimeError(f"ASN.1 encoders disagree for {name}")
            if encoded.hex() != vector["regional_uper_hex"]:
                raise RuntimeError(f"regional vector drift for {name}")

            message_frame.set_val({
                "messageId": records["message_id"],
                "value": (
                    records["message"],
                    {"regional": {
                        "regionId": records["region_id"],
                        "regExtValue": ("_unk_200", encoded),
                    }},
                ),
            })
            frame = message_frame.to_uper()
            if frame.hex() != vector["message_frame_uper_hex"]:
                raise RuntimeError(f"MessageFrame vector drift for {name}")
            # An unmodified J2735 package correctly preserves an unknown local
            # regional open type but announces the expected table miss on
            # stdout. Suppress that diagnostic and verify the raw value below.
            with contextlib.redirect_stdout(io.StringIO()):
                message_frame.from_uper(frame)
            decoded = message_frame.get_val()
            recovered = decoded["value"][1]["regional"]["regExtValue"][1]
            if recovered != encoded:
                raise RuntimeError(f"J2735 outer decoder changed regional bytes for {name}")
            print(f"{name}: regional={len(encoded)} bytes frame={len(frame)} bytes ok")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(2)
