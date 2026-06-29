#!/usr/bin/env python3
"""Run a bounded V2X-Radar/OpenCOOD detector benchmark on staged data."""

from __future__ import annotations

import argparse
import csv
import json
import os
import subprocess
import sys
import time
from collections import defaultdict
from datetime import datetime, timezone
from multiprocessing import get_context
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OPENCOOD_ROOT = REPO_ROOT / "benchmarks" / "v2x" / "repos" / "V2X-Radar" / "CodeBase" / "OpenCOOD"
DEFAULT_CONFIG = (
    DEFAULT_OPENCOOD_ROOT
    / "opencood"
    / "hypes_yaml"
    / "v2x-radar"
    / "radar_only"
    / "collab_radaronly_radarpillarnet_late.yaml"
)
DEFAULT_DATA = REPO_ROOT / "benchmarks" / "v2x" / "data" / "v2x-radar" / "v2x-radar-c" / "validate"
DEFAULT_CKPT = (
    REPO_ROOT
    / "benchmarks"
    / "v2x"
    / "data"
    / "checkpoints"
    / "V2X-Radar"
    / "radar_late"
    / "v2x_radar_c_radar_late.pth"
)
DEFAULT_RESULTS_ROOT = REPO_ROOT / "results" / "v2x_benchmarks"
IOU_THRESHOLDS = (0.3, 0.5, 0.7)
DISTANCE_RANGES = ((0, 30), (30, 50), (50, 100))


def capture(cmd: list[str]) -> str:
    try:
        return subprocess.check_output(cmd, text=True, stderr=subprocess.STDOUT).strip()
    except (FileNotFoundError, subprocess.CalledProcessError) as exc:
        return str(exc)


def install_mmcv_runner_compat() -> None:
    """Provide the tiny mmcv.runner API surface this fork imports."""
    import types
    import torch

    if "mmcv.runner" in sys.modules:
        return

    runner = types.ModuleType("mmcv.runner")

    class BaseModule(torch.nn.Module):
        def __init__(self, init_cfg=None):
            super().__init__()
            self.init_cfg = init_cfg

    def auto_fp16(*_args, **_kwargs):
        def decorator(func):
            return func

        return decorator

    runner.BaseModule = BaseModule
    runner.auto_fp16 = auto_fp16
    sys.modules["mmcv.runner"] = runner


def make_result_stat() -> dict:
    stat: dict = {"overall": {}}
    for iou in IOU_THRESHOLDS:
        stat["overall"][iou] = {"tp": [], "fp": [], "score": [], "gt": 0}
    for distance_range in DISTANCE_RANGES:
        key = str(distance_range)
        stat[key] = {}
        for iou in IOU_THRESHOLDS:
            stat[key][iou] = {"tp": [], "fp": [], "score": [], "gt": 0}
    return stat


def merge_result_stats(items: list[dict]) -> dict:
    merged = make_result_stat()
    for item in items:
        for range_key, by_iou in item.items():
            for iou, values in by_iou.items():
                target = merged[range_key][float(iou)]
                target["tp"].extend(values["tp"])
                target["fp"].extend(values["fp"])
                target["score"].extend(values["score"])
                target["gt"] += int(values["gt"])
    return merged


def voc_ap(rec: list[float], prec: list[float]) -> float:
    rec = [0.0] + rec + [1.0]
    prec = [0.0] + prec + [0.0]
    for index in range(len(prec) - 2, -1, -1):
        prec[index] = max(prec[index], prec[index + 1])
    ap = 0.0
    for index in range(1, len(rec)):
        if rec[index] != rec[index - 1]:
            ap += (rec[index] - rec[index - 1]) * prec[index]
    return ap


def calculate_ap(stat: dict) -> dict:
    output = {}
    for range_key, by_iou in stat.items():
        output[range_key] = {}
        for iou, values in by_iou.items():
            gt_total = int(values["gt"])
            if gt_total <= 0:
                output[range_key][str(iou)] = {
                    "ap": None,
                    "gt": 0,
                    "detections": len(values["score"]),
                }
                continue

            order = sorted(range(len(values["score"])), key=lambda idx: -float(values["score"][idx]))
            fp = [int(values["fp"][idx]) for idx in order]
            tp = [int(values["tp"][idx]) for idx in order]
            fp_cumsum = []
            tp_cumsum = []
            fp_running = 0
            tp_running = 0
            for fp_item, tp_item in zip(fp, tp):
                fp_running += fp_item
                tp_running += tp_item
                fp_cumsum.append(fp_running)
                tp_cumsum.append(tp_running)
            recall = [value / gt_total for value in tp_cumsum]
            precision = [
                tp_value / max(tp_value + fp_value, 1)
                for tp_value, fp_value in zip(tp_cumsum, fp_cumsum)
            ]
            output[range_key][str(iou)] = {
                "ap": round(voc_ap(recall, precision), 6),
                "gt": gt_total,
                "detections": len(values["score"]),
                "tp_final": tp_cumsum[-1] if tp_cumsum else 0,
                "fp_final": fp_cumsum[-1] if fp_cumsum else 0,
                "recall_final": round(recall[-1], 6) if recall else 0.0,
                "precision_final": round(precision[-1], 6) if precision else 0.0,
            }
    return output


def json_ready_stat(stat: dict) -> dict:
    return {
        range_key: {
            str(iou): {
                "tp": values["tp"],
                "fp": values["fp"],
                "score": [float(item) for item in values["score"]],
                "gt": int(values["gt"]),
            }
            for iou, values in by_iou.items()
        }
        for range_key, by_iou in stat.items()
    }


def select_sample_indices(dataset_length: int, max_samples: int, mode: str) -> list[int]:
    if max_samples <= 0 or max_samples >= dataset_length:
        return list(range(dataset_length))
    if mode == "first":
        return list(range(max_samples))
    if max_samples == 1:
        return [0]
    indices = []
    for index in range(max_samples):
        value = round(index * (dataset_length - 1) / (max_samples - 1))
        indices.append(int(value))
    return sorted(dict.fromkeys(indices))


def shard_indices(indices: list[int], workers: int) -> list[list[int]]:
    shards = [[] for _ in range(workers)]
    for index, sample_idx in enumerate(indices):
        shards[index % workers].append(sample_idx)
    return shards


def worker_main(
    local_rank: int,
    sample_indices: list[int],
    opencood_root: str,
    config_path: str,
    data_path: str,
    ckpt_path: str,
    fusion_method: str,
    queue,
) -> None:
    started = time.perf_counter()
    try:
        import torch

        install_mmcv_runner_compat()
        root = Path(opencood_root).resolve()
        sys.path.insert(0, str(root))
        os.chdir(root)

        from opencood.data_utils.datasets import build_dataset
        from opencood.hypes_yaml.yaml_utils import load_yaml
        from opencood.tools import test_utils, train_utils
        from opencood.utils import eval_utils_modify

        torch.cuda.set_device(local_rank)
        device = torch.device("cuda", local_rank)
        gpu_name = torch.cuda.get_device_name(device)

        hypes = load_yaml(config_path)
        hypes["root_dir"] = data_path
        hypes["validate_dir"] = data_path
        hypes["test_dir"] = data_path
        hypes["fusion"]["fusion_method"] = fusion_method

        model = train_utils.create_model(hypes)
        state_dict = torch.load(ckpt_path, map_location="cpu")
        load_result = model.load_state_dict(state_dict, strict=False)
        model.to(device)
        model.eval()

        dataset = build_dataset(hypes, visualize=True, train=False)
        result_stat = make_result_stat()
        rows = []
        failures = []

        for sample_idx in sample_indices:
            sample_started = time.perf_counter()
            try:
                item = dataset[sample_idx]
                if item is None:
                    rows.append(
                        {
                            "gpu_index": local_rank,
                            "sample_idx": sample_idx,
                            "status": "skipped_none",
                            "elapsed_ms": round((time.perf_counter() - sample_started) * 1000.0, 3),
                        }
                    )
                    continue
                batch = dataset.collate_batch_test([item])
                if batch is None:
                    rows.append(
                        {
                            "gpu_index": local_rank,
                            "sample_idx": sample_idx,
                            "status": "skipped_collate_none",
                            "elapsed_ms": round((time.perf_counter() - sample_started) * 1000.0, 3),
                        }
                    )
                    continue
                batch = train_utils.to_device(batch, device)
                with torch.no_grad():
                    if fusion_method == "late":
                        infer_result, _output_dict = test_utils.inference_late_fusion(batch, model, dataset)
                    elif fusion_method == "no":
                        infer_result, _output_dict = test_utils.inference_no_fusion(batch, model, dataset)
                    elif fusion_method == "single":
                        infer_result, _output_dict = test_utils.inference_no_fusion(
                            batch, model, dataset, single_gt=True
                        )
                    else:
                        infer_result, _output_dict = test_utils.inference_early_fusion(batch, model, dataset)

                pred_box_tensor = infer_result["pred_box_tensor"]
                pred_score = infer_result["pred_score"]
                gt_box_tensor = infer_result["gt_box_tensor"]
                for iou_thresh in IOU_THRESHOLDS:
                    eval_utils_modify.caluclate_tp_fp(
                        pred_box_tensor,
                        pred_score,
                        gt_box_tensor,
                        result_stat,
                        iou_thresh,
                        DISTANCE_RANGES,
                    )
                torch.cuda.synchronize(device)
                elapsed_ms = round((time.perf_counter() - sample_started) * 1000.0, 3)
                rows.append(
                    {
                        "gpu_index": local_rank,
                        "gpu_name": gpu_name,
                        "sample_idx": sample_idx,
                        "status": "ok",
                        "elapsed_ms": elapsed_ms,
                        "pred_boxes": 0 if pred_box_tensor is None else int(pred_box_tensor.shape[0]),
                        "gt_boxes": 0 if gt_box_tensor is None else int(gt_box_tensor.shape[0]),
                        "score_min": None
                        if pred_score is None or pred_score.numel() == 0
                        else round(float(pred_score.min().item()), 6),
                        "score_max": None
                        if pred_score is None or pred_score.numel() == 0
                        else round(float(pred_score.max().item()), 6),
                    }
                )
            except Exception as exc:  # noqa: BLE001 - preserve per-sample failure evidence.
                failures.append(
                    {
                        "gpu_index": local_rank,
                        "gpu_name": gpu_name,
                        "sample_idx": sample_idx,
                        "error": repr(exc),
                    }
                )

        elapsed_s = time.perf_counter() - started
        queue.put(
            {
                "worker": {
                    "gpu_index": local_rank,
                    "gpu_name": gpu_name,
                    "assigned_samples": len(sample_indices),
                    "successful_samples": sum(1 for row in rows if row.get("status") == "ok"),
                    "failures": len(failures),
                    "elapsed_s": round(elapsed_s, 6),
                    "max_memory_allocated_bytes": int(torch.cuda.max_memory_allocated(device)),
                    "missing_keys": list(load_result.missing_keys),
                    "unexpected_keys": list(load_result.unexpected_keys),
                },
                "rows": rows,
                "failures": failures,
                "result_stat": json_ready_stat(result_stat),
            }
        )
    except Exception as exc:  # noqa: BLE001 - report worker-level failures.
        queue.put(
            {
                "worker": {
                    "gpu_index": local_rank,
                    "assigned_samples": len(sample_indices),
                    "successful_samples": 0,
                    "failures": len(sample_indices),
                    "elapsed_s": round(time.perf_counter() - started, 6),
                    "fatal_error": repr(exc),
                },
                "rows": [],
                "failures": [
                    {
                        "gpu_index": local_rank,
                        "sample_idx": sample_idx,
                        "error": repr(exc),
                    }
                    for sample_idx in sample_indices
                ],
                "result_stat": json_ready_stat(make_result_stat()),
            }
        )


def write_csv(path: Path, rows: list[dict], fieldnames: list[str]) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames, lineterminator="\n")
        writer.writeheader()
        for row in rows:
            writer.writerow({field: row.get(field, "") for field in fieldnames})


def write_readme(path: Path, summary: dict) -> None:
    aggregate = summary["aggregate"]
    ap = summary["ap"]["overall"]
    lines = [
        "# V2X-Radar Detector Benchmark",
        "",
        f"Run ID: `{summary['run_id']}`",
        f"Generated UTC: `{summary['generated_utc']}`",
        "",
        "This run loads the official V2X-Radar radar-only late-fusion checkpoint",
        "and evaluates a bounded subset of the staged cooperative validation split",
        "across all selected GPUs. It is a partial detector benchmark unless",
        "`max_samples` covers the full validation split.",
        "",
        "## Result",
        "",
        f"- samples requested: `{aggregate['samples_requested']}`",
        f"- successful samples: `{aggregate['successful_samples']}`",
        f"- failed samples: `{aggregate['failed_samples']}`",
        f"- elapsed seconds: `{aggregate['elapsed_s']}`",
        f"- AP overall IoU 0.3: `{ap['0.3']['ap']}`",
        f"- AP overall IoU 0.5: `{ap['0.5']['ap']}`",
        f"- AP overall IoU 0.7: `{ap['0.7']['ap']}`",
        "",
        "## Workers",
        "",
        "| gpu | name | assigned | successful | failures | elapsed_s | max_mem_bytes |",
        "| ---: | --- | ---: | ---: | ---: | ---: | ---: |",
    ]
    for worker in aggregate["workers"]:
        lines.append(
            f"| {worker.get('gpu_index')} | {worker.get('gpu_name', '')} | "
            f"{worker.get('assigned_samples')} | {worker.get('successful_samples')} | "
            f"{worker.get('failures')} | {worker.get('elapsed_s')} | "
            f"{worker.get('max_memory_allocated_bytes', '')} |"
        )
    lines.extend(
        [
            "",
            "Raw per-sample detections are in `per_sample.csv`; per-sample failures",
            "are in `failures.csv`; complete AP/stat details are in `summary.json`.",
        ]
    )
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--opencood-root", type=Path, default=DEFAULT_OPENCOOD_ROOT)
    parser.add_argument("--config", type=Path, default=DEFAULT_CONFIG)
    parser.add_argument("--data", type=Path, default=DEFAULT_DATA)
    parser.add_argument("--checkpoint", type=Path, default=DEFAULT_CKPT)
    parser.add_argument("--results-root", type=Path, default=DEFAULT_RESULTS_ROOT)
    parser.add_argument("--run-id", default="")
    parser.add_argument("--gpus", default=os.environ.get("CUDA_VISIBLE_DEVICES", "0,1,2,3"))
    parser.add_argument("--dataset-length", type=int, default=922)
    parser.add_argument("--max-samples", type=int, default=32)
    parser.add_argument("--sample-mode", choices=("spread", "first"), default="spread")
    parser.add_argument("--fusion-method", default="late", choices=("late", "no", "single", "early", "intermediate"))
    args = parser.parse_args()

    gpu_ids = [item.strip() for item in args.gpus.split(",") if item.strip()]
    if not gpu_ids:
        raise SystemExit("No GPUs selected.")
    for required in (args.opencood_root, args.config, args.data, args.checkpoint):
        if not required.exists():
            raise SystemExit(f"Required path does not exist: {required}")

    os.environ["CUDA_VISIBLE_DEVICES"] = ",".join(gpu_ids)
    sample_indices = select_sample_indices(args.dataset_length, args.max_samples, args.sample_mode)
    shards = shard_indices(sample_indices, len(gpu_ids))

    run_id = args.run_id or "v2x-radar-detector-benchmark-" + datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    output_dir = args.results_root.resolve() / run_id
    output_dir.mkdir(parents=True, exist_ok=True)

    started = time.perf_counter()
    ctx = get_context("spawn")
    queue = ctx.Queue()
    processes = []
    for local_rank, shard in enumerate(shards):
        process = ctx.Process(
            target=worker_main,
            args=(
                local_rank,
                shard,
                str(args.opencood_root.resolve()),
                str(args.config.resolve()),
                str(args.data),
                str(args.checkpoint.resolve()),
                args.fusion_method,
                queue,
            ),
        )
        process.start()
        processes.append(process)

    results = [queue.get() for _ in processes]
    for process in processes:
        process.join()
    elapsed_s = time.perf_counter() - started

    workers = [result["worker"] for result in results]
    rows = [row for result in results for row in result["rows"]]
    failures = [failure for result in results for failure in result["failures"]]
    result_stat = merge_result_stats([result["result_stat"] for result in results])
    ap = calculate_ap(result_stat)

    summary = {
        "run_id": run_id,
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "benchmark": "V2X-Radar",
        "dataset": "V2X-Radar-C validate",
        "framework": "V2X-Radar OpenCOOD fork",
        "opencood_root": str(args.opencood_root.resolve()),
        "config": str(args.config.resolve()),
        "data": str(args.data),
        "checkpoint": str(args.checkpoint.resolve()),
        "checkpoint_bytes": args.checkpoint.stat().st_size,
        "cuda_visible_devices": os.environ["CUDA_VISIBLE_DEVICES"],
        "dataset_length_assumed": args.dataset_length,
        "max_samples": args.max_samples,
        "sample_mode": args.sample_mode,
        "sample_indices": sample_indices,
        "fusion_method": args.fusion_method,
        "environment": {
            "python": sys.version,
            "nvidia_smi_before_or_after": capture(
                [
                    "nvidia-smi",
                    "--query-gpu=index,name,memory.used,memory.total",
                    "--format=csv,noheader",
                ]
            ),
        },
        "aggregate": {
            "samples_requested": len(sample_indices),
            "successful_samples": sum(1 for row in rows if row.get("status") == "ok"),
            "failed_samples": len(failures),
            "elapsed_s": round(elapsed_s, 6),
            "workers": workers,
        },
        "ap": ap,
        "result_stat": json_ready_stat(result_stat),
    }

    (output_dir / "summary.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    write_csv(
        output_dir / "per_sample.csv",
        sorted(rows, key=lambda item: (int(item.get("gpu_index", -1)), int(item.get("sample_idx", -1)))),
        [
            "gpu_index",
            "gpu_name",
            "sample_idx",
            "status",
            "elapsed_ms",
            "pred_boxes",
            "gt_boxes",
            "score_min",
            "score_max",
        ],
    )
    write_csv(
        output_dir / "failures.csv",
        sorted(failures, key=lambda item: (int(item.get("gpu_index", -1)), int(item.get("sample_idx", -1)))),
        ["gpu_index", "gpu_name", "sample_idx", "error"],
    )
    write_readme(output_dir / "README.md", summary)

    print(json.dumps(summary["aggregate"], indent=2, sort_keys=True))
    print(json.dumps(summary["ap"]["overall"], indent=2, sort_keys=True))
    if failures:
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
