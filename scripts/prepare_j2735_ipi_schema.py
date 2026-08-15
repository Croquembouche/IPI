#!/usr/bin/env python3
"""Prepare a licensed SAE J2735 ASN bundle with the IPI regional extension.

The script never downloads or redistributes SAE modules. It copies a caller-
supplied licensed module directory to a new output directory, adds IPI.asn, and
binds IPI.IpiCooperativeService to Reg-TestMessage00.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import sys
from pathlib import Path


MESSAGE_ID = 240
TEST_MESSAGE = "TestMessage00"
REGIONAL_SET = "Reg-TestMessage00"
OBJECT_ENTRY = "{ IPI.IpiCooperativeService IDENTIFIED BY IPI.ipiRegionId }"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base-dir", type=Path, required=True,
                        help="directory containing the licensed J2735 ASN modules")
    parser.add_argument("--output-dir", type=Path, required=True,
                        help="new directory that will receive the prepared bundle")
    parser.add_argument("--ipi-schema", type=Path,
                        default=Path(__file__).resolve().parents[1] / "cpp" / "asn1" / "IPI.asn")
    parser.add_argument("--region-id", type=int, default=200)
    parser.add_argument("--standard-version", default="J2735ASN_202309")
    return parser.parse_args()


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def asn_files(root: Path) -> list[Path]:
    return sorted(path for path in root.rglob("*")
                  if path.is_file() and path.suffix.lower() in {".asn", ".asn1"})


def find_matching_brace(text: str, opening: int) -> int:
    depth = 0
    in_comment = False
    index = opening
    while index < len(text):
        if in_comment:
            if text[index] == "\n":
                in_comment = False
            index += 1
            continue
        if text.startswith("--", index):
            in_comment = True
            index += 2
            continue
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return index
        index += 1
    raise ValueError(f"unterminated object set {REGIONAL_SET}")


def patch_regional_set(text: str) -> str:
    declaration = re.search(
        rf"\b{re.escape(REGIONAL_SET)}\b\s+[^\n]*?::=\s*\{{",
        text,
        flags=re.MULTILINE,
    )
    if declaration is None:
        raise ValueError(f"{REGIONAL_SET} declaration not found")
    opening = text.find("{", declaration.start())
    closing = find_matching_brace(text, opening)
    body = text[opening + 1:closing]
    if "IPI.IpiCooperativeService" in body:
        raise ValueError("IPI regional object-set entry already exists")
    extension = re.search(r"\.\.\.", body)
    if extension is None:
        raise ValueError(f"{REGIONAL_SET} has no extension marker")

    root = body[:extension.start()].rstrip()
    suffix = body[extension.end():].lstrip()
    while root.endswith(","):
        root = root[:-1].rstrip()

    if root:
        replacement = (
            "\n" + root.strip() + " |\n"
            f"    {OBJECT_ENTRY} ,\n"
            "    ..."
        )
    else:
        replacement = f"\n    {OBJECT_ENTRY} ,\n    ..."
    if suffix:
        replacement += " " + suffix
    replacement += "\n"
    return text[:opening + 1] + replacement + text[closing:]


def render_ipi_schema(source: Path, region_id: int) -> str:
    text = source.read_text(encoding="utf-8")
    rendered, count = re.subn(
        r"\bipiRegionId\s+RegionId\s+::=\s+\d+",
        f"ipiRegionId RegionId ::= {region_id}",
        text,
        count=1,
    )
    if count != 1:
        raise ValueError("IPI schema does not contain one ipiRegionId assignment")
    return rendered


def verify_message_id(files: list[Path]) -> None:
    pattern = re.compile(r"\btestMessage00(?:-D)?\b\s+DSRCmsgID\s*::=\s*240\b")
    for path in files:
        if pattern.search(path.read_text(encoding="utf-8", errors="strict")):
            return
    raise ValueError("licensed modules do not bind TestMessage00 to DSRCmsgID 240")


def main() -> int:
    args = parse_args()
    base = args.base_dir.resolve()
    output = args.output_dir.resolve()
    schema = args.ipi_schema.resolve()

    if not 128 <= args.region_id <= 255:
        raise ValueError("--region-id must be in the local J2735 range 128..255")
    if not base.is_dir():
        raise ValueError(f"base ASN directory does not exist: {base}")
    if not schema.is_file():
        raise ValueError(f"IPI schema does not exist: {schema}")
    if output == base or base in output.parents:
        raise ValueError("output directory must be outside the licensed source directory")
    if output.exists() and any(output.iterdir()):
        raise ValueError("output directory must not already contain files")

    source_files = asn_files(base)
    if not source_files:
        raise ValueError("no .asn or .asn1 modules found in --base-dir")
    verify_message_id(source_files)

    regional_matches: list[Path] = []
    for path in source_files:
        if re.search(rf"\b{re.escape(REGIONAL_SET)}\b\s+[^\n]*?::=\s*\{{",
                     path.read_text(encoding="utf-8"), flags=re.MULTILINE):
            regional_matches.append(path)
    if len(regional_matches) != 1:
        raise ValueError(
            f"expected one {REGIONAL_SET} definition, found {len(regional_matches)}")

    output.mkdir(parents=True, exist_ok=True)
    for source in source_files:
        destination = output / source.relative_to(base)
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)

    regional_output = output / regional_matches[0].relative_to(base)
    regional_output.write_text(
        patch_regional_set(regional_output.read_text(encoding="utf-8")),
        encoding="utf-8",
    )
    ipi_output = output / "IPI.asn"
    ipi_output.write_text(render_ipi_schema(schema, args.region_id), encoding="utf-8")

    output_files = asn_files(output)
    manifest = {
        "standard_version": args.standard_version,
        "message": TEST_MESSAGE,
        "dsrc_message_id": MESSAGE_ID,
        "region_id": args.region_id,
        "encoding": "UPER",
        "regional_object_set": REGIONAL_SET,
        "regional_type": "IPI.IpiCooperativeService",
        "inputs": [
            {
                "path": str(path.relative_to(base)),
                "sha256": sha256(path),
            }
            for path in source_files
        ],
        "outputs": [
            {
                "path": str(path.relative_to(output)),
                "sha256": sha256(path),
            }
            for path in output_files
        ],
    }
    manifest_path = output / "ipi-j2735-schema-manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n",
                             encoding="utf-8")
    print(manifest_path)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        sys.exit(2)
