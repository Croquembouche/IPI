#!/usr/bin/env python3
"""Download V2X benchmark repositories and public example archives."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import tarfile
import time
import zipfile
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ROOT = REPO_ROOT / "benchmarks" / "v2x"
DEFAULT_SOURCES = DEFAULT_ROOT / "sources.json"
DEFAULT_RESULTS_MANIFEST = REPO_ROOT / "results" / "v2x_benchmarks" / "latest" / "download_manifest.json"


def run(cmd: list[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    print("+ " + " ".join(cmd), flush=True)
    return subprocess.run(cmd, cwd=cwd, check=True, text=True)


def load_sources(path: Path) -> dict:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def maybe_install_gdown() -> None:
    try:
        subprocess.run(
            [sys.executable, "-m", "gdown", "--version"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=True,
        )
        return
    except subprocess.CalledProcessError:
        pass

    run([sys.executable, "-m", "pip", "install", "--user", "gdown"])


def clone_repo(name: str, url: str, repos_dir: Path) -> dict:
    target = repos_dir / repo_target_name(name)
    if target.exists():
        revision = subprocess.check_output(
            ["git", "-C", str(target), "rev-parse", "HEAD"], text=True
        ).strip()
        return {
            "name": name,
            "url": url,
            "path": str(target),
            "status": "already_present",
            "revision": revision,
        }

    run(["git", "clone", "--depth", "1", url, str(target)])
    revision = subprocess.check_output(
        ["git", "-C", str(target), "rev-parse", "HEAD"], text=True
    ).strip()
    return {
        "name": name,
        "url": url,
        "path": str(target),
        "status": "cloned",
        "revision": revision,
    }


def repo_target_name(name: str) -> str:
    if name == "opencood":
        return "OpenCOOD"
    if name == "dair_v2x":
        return "DAIR-V2X"
    if name == "v2v4real":
        return "V2V4Real"
    if name == "v2x_real":
        return "V2X-Real"
    if name == "v2x_radar":
        return "V2X-Radar"
    return name


def download_drive_file(example: dict, archives_dir: Path, install_missing: bool) -> dict:
    output = archives_dir / example["filename"]
    if output.exists() and output.stat().st_size > 0:
        return {
            "name": example["name"],
            "url": example["url"],
            "path": str(output),
            "bytes": output.stat().st_size,
            "status": "already_present",
        }

    if install_missing:
        maybe_install_gdown()

    url_or_id = example.get("drive_file_id") or example["url"]
    cmd = [sys.executable, "-m", "gdown", url_or_id, "-O", str(output)]
    try:
        run(cmd)
    except (subprocess.CalledProcessError, FileNotFoundError) as exc:
        return {
            "name": example["name"],
            "url": example["url"],
            "path": str(output),
            "status": "failed",
            "error": str(exc),
            "hint": "Install gdown or rerun with --install-missing.",
        }

    return {
        "name": example["name"],
        "url": example["url"],
        "path": str(output),
        "bytes": output.stat().st_size,
        "status": "downloaded",
    }


def archive_stem(archive: Path) -> str:
    name = archive.name
    for suffix in (".tar.gz", ".tar.bz2", ".tar.xz", ".tgz", ".zip"):
        if name.endswith(suffix):
            return name[: -len(suffix)]
    return archive.stem


def extract_archive(archive: Path, extract_dir: Path) -> dict:
    target = extract_dir / archive_stem(archive)
    if target.exists() and any(target.iterdir()):
        return {"archive": str(archive), "path": str(target), "status": "already_present"}

    target.mkdir(parents=True, exist_ok=True)
    if zipfile.is_zipfile(archive):
        with zipfile.ZipFile(archive) as zf:
            zf.extractall(target)
        return {"archive": str(archive), "path": str(target), "status": "extracted"}

    if tarfile.is_tarfile(archive):
        with tarfile.open(archive) as tf:
            tf.extractall(target)
        return {"archive": str(archive), "path": str(target), "status": "extracted"}

    return {
        "archive": str(archive),
        "path": str(target),
        "status": "skipped",
        "reason": "unsupported archive format",
    }


def download_http_artifact(name: str, artifact: dict, archives_dir: Path) -> dict:
    output = archives_dir / artifact["filename"]
    expected_bytes = int(artifact.get("expected_bytes") or 0)
    if output.exists() and output.stat().st_size > 0:
        status = "already_present"
        if expected_bytes and output.stat().st_size != expected_bytes:
            status = "present_size_mismatch"
        return {
            "name": name,
            "dataset": artifact.get("dataset", ""),
            "benchmark": artifact.get("benchmark", ""),
            "repo_id": artifact.get("repo_id", ""),
            "source_path": artifact.get("path", ""),
            "url": artifact["url"],
            "path": str(output),
            "bytes": output.stat().st_size,
            "expected_bytes": expected_bytes,
            "status": status,
        }

    cmd = ["curl", "-L", "--fail", "--continue-at", "-", "-o", str(output), artifact["url"]]
    try:
        run(cmd)
    except (subprocess.CalledProcessError, FileNotFoundError) as exc:
        return {
            "name": name,
            "dataset": artifact.get("dataset", ""),
            "benchmark": artifact.get("benchmark", ""),
            "repo_id": artifact.get("repo_id", ""),
            "source_path": artifact.get("path", ""),
            "url": artifact["url"],
            "path": str(output),
            "expected_bytes": expected_bytes,
            "status": "failed",
            "error": str(exc),
        }

    status = "downloaded"
    if expected_bytes and output.stat().st_size != expected_bytes:
        status = "downloaded_size_mismatch"
    return {
        "name": name,
        "dataset": artifact.get("dataset", ""),
        "benchmark": artifact.get("benchmark", ""),
        "repo_id": artifact.get("repo_id", ""),
        "source_path": artifact.get("path", ""),
        "url": artifact["url"],
        "path": str(output),
        "bytes": output.stat().st_size,
        "expected_bytes": expected_bytes,
        "status": status,
    }


def safe_path_component(value: str) -> str:
    return "".join(ch if ch.isalnum() or ch in "._-" else "_" for ch in value).strip("_")


def download_model_artifact(name: str, artifact: dict, checkpoints_dir: Path, install_missing: bool) -> dict:
    benchmark = safe_path_component(artifact.get("benchmark", "models")) or "models"
    subdir = artifact.get("subdir") or name
    target_dir = checkpoints_dir / benchmark / safe_path_component(subdir)
    target_dir.mkdir(parents=True, exist_ok=True)
    filename = artifact.get("filename") or f"{name}.pth"
    output = target_dir / filename

    if output.exists() and output.stat().st_size > 0:
        return {
            "name": name,
            "dataset": artifact.get("dataset", ""),
            "benchmark": artifact.get("benchmark", ""),
            "method": artifact.get("method", ""),
            "modality": artifact.get("modality", ""),
            "config": artifact.get("config", ""),
            "url": artifact["url"],
            "path": str(output),
            "bytes": output.stat().st_size,
            "status": "already_present",
        }

    if install_missing:
        maybe_install_gdown()

    url_or_id = artifact.get("drive_file_id") or artifact["url"]
    cmd = [sys.executable, "-m", "gdown", url_or_id, "-O", str(output)]
    try:
        run(cmd)
    except (subprocess.CalledProcessError, FileNotFoundError) as exc:
        return {
            "name": name,
            "dataset": artifact.get("dataset", ""),
            "benchmark": artifact.get("benchmark", ""),
            "method": artifact.get("method", ""),
            "modality": artifact.get("modality", ""),
            "config": artifact.get("config", ""),
            "url": artifact["url"],
            "path": str(output),
            "status": "failed",
            "error": str(exc),
            "hint": "Install gdown or rerun with --install-missing.",
        }

    return {
        "name": name,
        "dataset": artifact.get("dataset", ""),
        "benchmark": artifact.get("benchmark", ""),
        "method": artifact.get("method", ""),
        "modality": artifact.get("modality", ""),
        "config": artifact.get("config", ""),
        "url": artifact["url"],
        "path": str(output),
        "bytes": output.stat().st_size,
        "status": "downloaded",
    }


def record_dataset_entrypoint(name: str, entry: dict, full_dir: Path) -> dict:
    dataset_dir = full_dir / name
    dataset_dir.mkdir(parents=True, exist_ok=True)
    readme = dataset_dir / "SOURCE.txt"
    lines = [
        f"dataset={entry.get('dataset', name)}",
        f"benchmark={entry.get('benchmark', '')}",
        f"download_kind={entry.get('download_kind', '')}",
        f"url={entry.get('url', '')}",
    ]
    if entry.get("box_root"):
        lines.append(f"box_root={entry['box_root']}")
    if entry.get("drive_folder_id"):
        lines.append(f"drive_folder_id={entry['drive_folder_id']}")
    lines.append(f"description={entry.get('description', '')}")
    readme.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return {
        "name": name,
        "dataset": entry.get("dataset", name),
        "benchmark": entry.get("benchmark", ""),
        "download_kind": entry.get("download_kind", ""),
        "url": entry.get("url", ""),
        "path": str(dataset_dir),
        "status": "entrypoint_recorded",
    }


def download_drive_folder(name: str, entry: dict, full_dir: Path, install_missing: bool) -> dict:
    if entry.get("download_kind") != "google_drive_folder":
        recorded = record_dataset_entrypoint(name, entry, full_dir)
        recorded["status"] = "manual_download_required"
        return recorded

    if install_missing:
        maybe_install_gdown()

    target = full_dir / name
    target.mkdir(parents=True, exist_ok=True)
    url = entry["url"]
    cmd = [
        sys.executable,
        "-m",
        "gdown",
        "--folder",
        "--remaining-ok",
        url,
        "-O",
        str(target),
    ]
    try:
        run(cmd)
    except (subprocess.CalledProcessError, FileNotFoundError) as exc:
        result = record_dataset_entrypoint(name, entry, full_dir)
        result["status"] = "failed"
        result["error"] = str(exc)
        return result

    file_count = sum(1 for path in target.rglob("*") if path.is_file())
    byte_count = sum(path.stat().st_size for path in target.rglob("*") if path.is_file())
    return {
        "name": name,
        "dataset": entry.get("dataset", name),
        "benchmark": entry.get("benchmark", ""),
        "download_kind": entry.get("download_kind", ""),
        "url": url,
        "path": str(target),
        "status": "downloaded",
        "file_count": file_count,
        "bytes": byte_count,
    }


def selected_names(value: str, available: set[str]) -> list[str]:
    if value == "all":
        return sorted(available)
    names = [item.strip() for item in value.split(",") if item.strip()]
    unknown = sorted(set(names) - available)
    if unknown:
        raise SystemExit(f"Unknown selection(s): {', '.join(unknown)}")
    return names


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=DEFAULT_ROOT)
    parser.add_argument("--sources", type=Path, default=DEFAULT_SOURCES)
    parser.add_argument("--results-manifest", type=Path, default=DEFAULT_RESULTS_MANIFEST)
    parser.add_argument("--repos", default="all")
    parser.add_argument("--dataset-examples", default="all")
    parser.add_argument("--full-datasets", default="opv2v,v2xset,dair_v2x,v2v4real,v2x_real")
    parser.add_argument("--hf-artifacts", default="truckv2x_town1_2_test,v2x_radar_v_imagesets")
    parser.add_argument("--model-artifacts", default="v2x_radar_c_radar_late")
    parser.add_argument("--skip-repos", action="store_true")
    parser.add_argument("--skip-datasets", action="store_true")
    parser.add_argument("--skip-hf-artifacts", action="store_true")
    parser.add_argument("--skip-model-artifacts", action="store_true")
    parser.add_argument("--download-full-datasets", action="store_true")
    parser.add_argument("--install-missing", action="store_true")
    parser.add_argument("--extract", action="store_true")
    args = parser.parse_args()

    sources = load_sources(args.sources)
    root = args.root.resolve()
    repos_dir = root / "repos"
    data_dir = root / "data"
    archives_dir = data_dir / "archives"
    extracted_dir = data_dir / "extracted"
    full_dir = data_dir / "full"
    checkpoints_dir = data_dir / "checkpoints"
    manifest_dir = data_dir / "manifests"
    for path in (repos_dir, archives_dir, extracted_dir, full_dir, checkpoints_dir, manifest_dir):
        path.mkdir(parents=True, exist_ok=True)

    manifest: dict = {
        "generated_unix_s": int(time.time()),
        "root": str(root),
        "sources_file": str(args.sources.resolve()),
        "repositories": [],
        "dataset_examples": [],
        "huggingface_artifacts": [],
        "model_artifacts": [],
        "full_dataset_entrypoints": [],
        "full_dataset_downloads": [],
        "extractions": [],
    }

    if not args.skip_repos:
        repo_sources = sources["benchmark_repositories"]
        for name in selected_names(args.repos, set(repo_sources)):
            repo = repo_sources[name]
            manifest["repositories"].append(clone_repo(name, repo["url"], repos_dir))

    if not args.skip_datasets:
        example_sources = sources["dataset_examples"]
        for name in selected_names(args.dataset_examples, set(example_sources)):
            example = dict(example_sources[name])
            example["name"] = name
            result = download_drive_file(example, archives_dir, args.install_missing)
            manifest["dataset_examples"].append(result)
            if args.extract and result["status"] in {"downloaded", "already_present"}:
                manifest["extractions"].append(extract_archive(Path(result["path"]), extracted_dir))

    if not args.skip_hf_artifacts:
        artifact_sources = sources.get("huggingface_artifacts", {})
        for name in selected_names(args.hf_artifacts, set(artifact_sources)):
            artifact = artifact_sources[name]
            result = download_http_artifact(name, artifact, archives_dir)
            manifest["huggingface_artifacts"].append(result)
            if args.extract and result["status"] in {"downloaded", "already_present"}:
                manifest["extractions"].append(extract_archive(Path(result["path"]), extracted_dir))

    if not args.skip_model_artifacts:
        model_sources = sources.get("model_artifacts", {})
        for name in selected_names(args.model_artifacts, set(model_sources)):
            artifact = model_sources[name]
            result = download_model_artifact(name, artifact, checkpoints_dir, args.install_missing)
            manifest["model_artifacts"].append(result)

    full_sources = sources.get("full_dataset_entrypoints", {})
    for name in selected_names(args.full_datasets, set(full_sources)):
        entry = full_sources[name]
        manifest["full_dataset_entrypoints"].append(record_dataset_entrypoint(name, entry, full_dir))
        if args.download_full_datasets:
            manifest["full_dataset_downloads"].append(
                download_drive_folder(name, entry, full_dir, args.install_missing)
            )

    output = manifest_dir / "download_manifest.json"
    with output.open("w", encoding="utf-8") as handle:
        json.dump(manifest, handle, indent=2, sort_keys=True)
        handle.write("\n")
    args.results_manifest.parent.mkdir(parents=True, exist_ok=True)
    with args.results_manifest.open("w", encoding="utf-8") as handle:
        json.dump(manifest, handle, indent=2, sort_keys=True)
        handle.write("\n")
    print(f"Wrote {output}")
    print(f"Wrote {args.results_manifest}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
