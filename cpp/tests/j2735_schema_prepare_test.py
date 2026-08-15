#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import subprocess
import tempfile
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--script", type=Path, required=True)
    parser.add_argument("--schema", type=Path, required=True)
    return parser.parse_args()


def run(*args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(args, check=False, text=True, capture_output=True)


def main() -> int:
    args = parse_args()
    with tempfile.TemporaryDirectory(prefix="ipi-j2735-schema-") as directory:
        root = Path(directory)
        base = root / "licensed"
        output = root / "prepared"
        base.mkdir()
        (base / "MessageFrame.asn").write_text(
            "MessageFrame DEFINITIONS ::= BEGIN\n"
            "DSRCmsgID ::= INTEGER (0..32767)\n"
            "testMessage00 DSRCmsgID ::= 240\n"
            "END\n",
            encoding="utf-8",
        )
        (base / "REGION.asn").write_text(
            "REGION DEFINITIONS ::= BEGIN\n"
            "Reg-TestMessage00 DSRC.REG-EXT-ID-AND-TYPE ::= { ... }\n"
            "END\n",
            encoding="utf-8",
        )

        result = run(
            str(args.script),
            "--base-dir", str(base),
            "--output-dir", str(output),
            "--ipi-schema", str(args.schema),
            "--region-id", "201",
        )
        assert result.returncode == 0, result.stderr
        region = (output / "REGION.asn").read_text(encoding="utf-8")
        assert "IPI.IpiCooperativeService IDENTIFIED BY IPI.ipiRegionId" in region
        ipi = (output / "IPI.asn").read_text(encoding="utf-8")
        assert "ipiRegionId RegionId ::= 201" in ipi
        manifest = json.loads(
            (output / "ipi-j2735-schema-manifest.json").read_text(encoding="utf-8"))
        assert manifest["dsrc_message_id"] == 240
        assert manifest["region_id"] == 201
        assert manifest["encoding"] == "UPER"
        assert len(manifest["inputs"]) == 2
        assert len(manifest["outputs"]) == 3

        repeated = run(
            str(args.script),
            "--base-dir", str(base),
            "--output-dir", str(output),
            "--ipi-schema", str(args.schema),
        )
        assert repeated.returncode != 0
        assert "must not already contain files" in repeated.stderr

        invalid = run(
            str(args.script),
            "--base-dir", str(base),
            "--output-dir", str(root / "invalid"),
            "--ipi-schema", str(args.schema),
            "--region-id", "3",
        )
        assert invalid.returncode != 0
        assert "128..255" in invalid.stderr
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
