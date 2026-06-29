# V2X Dataset Benchmarks For IPI

This directory is the reproducible entry point for testing IPI with public V2X
cooperative-perception assets. The target benchmark set is intentionally broader
than one dataset:

- DAIR-V2X / OpenDAIR-V2X
- OPV2V / OpenCOOD
- V2XSet / OpenCOOD
- V2V4Real
- TruckV2X
- V2X-Radar
- V2X-Real as an additional modern real-world V2X target

The current repository's executable IPI path measures message encoding and
transport behavior, not detector accuracy. The workflow below therefore keeps
three layers separate:

1. Download public benchmark code and example datasets.
2. Convert available V2X dataset artifacts into IPI-safe payload conditions and
   run the existing IPI loopback latency probe.
3. Run dataset/framework GPU smokes and detector AP/IoU benchmarks before
   making perception-performance claims.

A full V2X-Radar radar-only late-fusion detector validation run is recorded
under `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/`.
Additional detector targets such as OPV2V, V2XSet, V2V4Real, and V2X-Real
remain optional follow-up runs once their full data/checkpoint pairs are staged.

The current staged artifacts cover at least three distinct benchmark families:
OpenDAIR-V2X examples, TruckV2X, and V2X-Radar. The latest manifest also
separates DAIR-V2X-C, V2X-Seq-SPD, V2X-Seq-TFD, TruckV2X, V2X-Radar-C, and
V2X-Radar-V ImageSets groups.

## Layout

- `sources.json` records the official source URLs and known example-file IDs.
- `data/` is ignored by Git and stores downloaded dataset archives/extractions.
- `repos/` is ignored by Git and stores benchmark code checkouts such as
  OpenCOOD and DAIR-V2X.
- `results/v2x_benchmarks/` stores generated IPI manifests, command logs, CSVs,
  summaries, and environment snapshots.

## Download Benchmark Assets

```bash
python3 scripts/download_v2x_benchmarks.py \
  --root benchmarks/v2x \
  --repos all \
  --dataset-examples all \
  --install-missing \
  --extract
```

This clones the public benchmark repositories and downloads the official
DAIR-V2X/V2X-Seq example archives plus selected Hugging Face-hosted V2X
artifacts listed in `sources.json`.

To download only the larger V2X-Radar cooperative validation split used by the
OpenCOOD smoke:

```bash
python3 scripts/download_v2x_benchmarks.py \
  --root benchmarks/v2x \
  --skip-repos \
  --skip-datasets \
  --hf-artifacts v2x_radar_c_validate \
  --extract
```

Full OPV2V, V2XSet, DAIR-V2X, V2V4Real, and V2X-Real data sources are recorded
in the same download manifest. To attempt bulk Google Drive folder downloads for
the datasets with public folder IDs, run:

```bash
python3 scripts/download_v2x_benchmarks.py \
  --root benchmarks/v2x \
  --skip-repos \
  --skip-datasets \
  --download-full-datasets \
  --full-datasets opv2v,v2xset,dair_v2x \
  --install-missing
```

V2V4Real and V2X-Real full assets are published as Box/web downloads; the script
records their official URLs in the manifest, and downloaded files should be
placed under `benchmarks/v2x/data/full/<dataset>/`.

## Build A Dataset-Derived IPI Manifest

```bash
python3 scripts/build_v2x_ipi_payload_manifest.py \
  --data-root benchmarks/v2x/data \
  --output results/v2x_benchmarks/latest/v2x_ipi_payload_manifest.json
```

IPI's current `IPI-CooperativeService` offload payload section is limited to
65,535 bytes because optional sections use a 16-bit length field. The manifest
therefore records both the raw artifact size and the number of IPI chunks needed
when an image, point cloud, or annotation exceeds the safe per-message payload.

## Run The Local IPI Payload Benchmark

```bash
python3 scripts/run_v2x_ipi_loopback.py \
  --manifest results/v2x_benchmarks/latest/v2x_ipi_payload_manifest.json \
  --build \
  --count 50
```

This starts the repository's `example_private_5g_latency_receiver`, runs
`example_private_5g_latency_sender` for representative dataset-derived payload
sizes, and writes CSV plus summary files under a timestamped result directory.
The run is CPU/network-loopback by design; GPU detector benchmarking is handled
by the benchmark framework step below.

To keep results separated by benchmark/dataset family, run:

```bash
python3 scripts/run_v2x_ipi_loopback.py \
  --manifest results/v2x_benchmarks/latest/v2x_ipi_payload_manifest.json \
  --group-by-dataset \
  --count 20
```

The grouped run writes benchmark and dataset labels into `summary.json`,
`summary.csv`, and the per-run README so DAIR-V2X/V2X-Seq, TruckV2X, and
V2X-Radar payload behavior can be compared directly.

## Run The Four-GPU Dataset Artifact Benchmark

```bash
CUDA_VISIBLE_DEVICES=0,1,2,3 \
conda run -n mmdet3d python scripts/run_v2x_gpu_dataset_benchmark.py \
  --manifest results/v2x_benchmarks/latest/v2x_ipi_payload_manifest.json \
  --files-per-dataset 48 \
  --max-bytes-per-file 1048576
```

This uses all four visible GPUs for a lightweight, deterministic tensor workload
over selected staged annotation, image, and point-cloud artifacts. It validates
dataset/GPU plumbing and records per-file timings; it is not detector AP/IoU.

## V2X-Radar OpenCOOD Dataset Smoke

The V2X-Radar OpenCOOD fork expects the dataset path to contain a literal
`v2x-radar` component. Keep the ignored symlink below when running the smoke
against the extracted validation split:

```bash
mkdir -p benchmarks/v2x/data/v2x-radar/v2x-radar-c
ln -sfn ../../extracted/V2X-Radar-C-validate/validate \
  benchmarks/v2x/data/v2x-radar/v2x-radar-c/validate
```

The latest recorded smoke is under
`results/v2x_benchmarks/v2x-radar-opencood-smoke-20260629T180847Z/`. It builds
the V2X-Radar dataset with the radar-only late-fusion config and retrieves a
real validation sample with `visualize=True`.

## Run The V2X-Radar Detector Benchmark

The official V2X-Radar radar-only late-fusion checkpoint is listed in
`sources.json` and is staged under:

```text
benchmarks/v2x/data/checkpoints/V2X-Radar/radar_late/v2x_radar_c_radar_late.pth
```

Run the full cooperative validation split on all four GPUs:

```bash
CUDA_VISIBLE_DEVICES=0,1,2,3 \
conda run -n mmdet3d python scripts/run_v2x_radar_detector_benchmark.py \
  --max-samples 0 \
  --sample-mode spread \
  --gpus 0,1,2,3
```

The latest full run evaluated `922/922` samples with zero failures and recorded
overall AP `0.130266` at IoU `0.5` and AP `0.046459` at IoU `0.7`. It used all
four RTX 2080 Ti GPUs and loaded the checkpoint with no missing or unexpected
keys.

## OpenCOOD Four-GPU Benchmark Command

After installing OpenCOOD dependencies and staging full OPV2V or V2XSet data,
run training with all four GPUs using the OpenCOOD distributed command:

```bash
cd benchmarks/v2x/repos/OpenCOOD
CUDA_VISIBLE_DEVICES=0,1,2,3 \
python -m torch.distributed.launch --nproc_per_node=4 --use_env \
  opencood/tools/train.py --hypes_yaml <config.yaml>
```

For evaluation from a checkpoint:

```bash
cd benchmarks/v2x/repos/OpenCOOD
CUDA_VISIBLE_DEVICES=0,1,2,3 \
python opencood/tools/inference.py \
  --model_dir <checkpoint_dir> \
  --fusion_method <early|late|intermediate>
```

Record the exact config, checkpoint, dataset split, command, GPU state, and
OpenCOOD AP/IoU outputs under `results/v2x_benchmarks/`.

For V2V4Real, use its repository-specific environment and keep the same result
recording discipline:

```bash
cd benchmarks/v2x/repos/V2V4Real
CUDA_VISIBLE_DEVICES=0,1,2,3 \
python -m torch.distributed.launch --nproc_per_node=4 --use_env \
  opencood/tools/train.py --hypes_yaml <v2v4real_config.yaml>
```
