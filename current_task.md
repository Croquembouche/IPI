# Current Task

Last updated: 2026-06-29 18:43 UTC

## Task

Download and stage V2X datasets and benchmark code so IPI can be implemented
and tested against public V2X benchmark assets. Use at least three different
V2X benchmark/dataset families, preserve all recorded results, and use the
available four RTX 2080 Ti GPUs for GPU benchmark runs when those runs are
ready.

## Status

Completed for the requested local scope.

The "at least three" benchmark/dataset requirement is currently satisfied by
staged and recorded evidence for:

- OpenDAIR-V2X: `DAIR-V2X-C-Example`, `V2X-Seq-SPD-Example`,
  `V2X-Seq-TFD-Example`
- TruckV2X: `TruckV2X-test-Town1_2`
- V2X-Radar: `V2X-Radar-C-validate`, `V2X-Radar-V-ImageSets`

Additional benchmark code and full-dataset entrypoints are staged for OPV2V,
V2XSet, V2V4Real, and V2X-Real. Current completed runs cover IPI payload
transport across all staged dataset groups, four-GPU dataset artifact
processing across all staged dataset groups, OpenCOOD/V2X-Radar dataset
integration, and a full four-GPU V2X-Radar detector AP validation run with the
official radar-only late-fusion checkpoint.

## Staged Assets

Tracked source/docs/scripts:

- `benchmarks/v2x/README.md`
- `benchmarks/v2x/sources.json`
- `scripts/download_v2x_benchmarks.py`
- `scripts/build_v2x_ipi_payload_manifest.py`
- `scripts/run_v2x_ipi_loopback.py`
- `scripts/run_v2x_gpu_dataset_benchmark.py`
- `scripts/run_v2x_radar_detector_benchmark.py`

Ignored benchmark repositories cloned under `benchmarks/v2x/repos/`:

- `DAIR-V2X` at commit `c885c54af0c34bc515fa9ca8b5e8fda76a15462c`
- `OpenCOOD` at commit `31ba16025da27ffe4e336f011290dfbc66f9a1f1`
- `V2V4Real` at commit `5a821e13753bafc611f95c47bc1a306acdcb0f7c`
- `V2X-Real` at commit `2b8acd5c15b22fdb47326e30b8805e172d9b1259`
- `V2X-Radar` at commit `1624751e31260c68d544e60599079d2104ee8a73`

Ignored downloaded/extracted dataset artifacts under `benchmarks/v2x/data/`:

- `DAIR-V2X-C-Example.zip` (`264,885,691` bytes)
- `V2X-Seq-SPD-Example.zip` (`585,329,481` bytes)
- `V2X-Seq-TFD-Example.zip` (`285,827,698` bytes)
- `TruckV2X-test-Town1_2.zip` (`1,159,933,259` bytes)
- `V2X-Radar-V-ImageSets.zip` (`48,903` bytes)
- `V2X-Radar-C-validate.tar.gz` (`5,745,353,647` bytes; extracted validation
  split is about `9.2G`)
- official V2X-Radar radar-only late-fusion checkpoint
  `v2x_radar_c_radar_late.pth` (`55,260,533` bytes; SHA-256
  `906bf37936690bbb8c37be8227e4f66f0cdff6a31534004929bc1f15845d6667`)

The local ignored symlink
`benchmarks/v2x/data/v2x-radar/v2x-radar-c/validate` points to the extracted
V2X-Radar cooperative validation split. Keep this path shape because the
V2X-Radar OpenCOOD fork branches on a literal `v2x-radar` path component.

## Recorded Evidence

Latest source/download manifest:

- `results/v2x_benchmarks/latest/download_manifest.json`
- now includes the official V2X-Radar model artifact entry
  `v2x_radar_c_radar_late`

Latest dataset-derived IPI payload manifest:

- `results/v2x_benchmarks/latest/v2x_ipi_payload_manifest.json`
- scanned files: `22,431`
- benchmark families in staged data: `OpenDAIR-V2X`, `TruckV2X`, `V2X-Radar`
- representative pooled IPI payload sizes: `0`, `256`, `1024`, `4096`,
  `60000` bytes
- overall raw artifact size summary: p50 `134,797` bytes, p95 `4,032,174`
  bytes, max `47,104,225` bytes

Latest grouped IPI TCP loopback run:

- directory: `results/v2x_benchmarks/v2x-ipi-loopback-20260629T175556Z/`
- conditions: `34`
- probes: `680/680` accepted
- success rate: `100%` for every dataset group and payload condition
- groups: `DAIR-V2X-C-Example`, `V2X-Seq-SPD-Example`,
  `V2X-Seq-TFD-Example`, `TruckV2X-test-Town1_2`,
  `V2X-Radar-C-validate`, `V2X-Radar-V-ImageSets`

Earlier loopback runs are preserved under:

- `results/v2x_benchmarks/v2x-ipi-loopback-20260629T170448Z/`
- `results/v2x_benchmarks/v2x-ipi-loopback-20260629T173201Z/`
- `results/v2x_benchmarks/v2x-ipi-loopback-20260629T173649Z/`

Latest four-GPU dataset artifact benchmark:

- directory:
  `results/v2x_benchmarks/v2x-gpu-dataset-benchmark-20260629T175939Z/`
- command used all four GPUs via `CUDA_VISIBLE_DEVICES=0,1,2,3`
- files processed: `224`
- successes/failures: `224/0`
- processed bytes: `128,904,577`
- wall time: `14.770803` seconds
- selected files covered all six staged dataset groups listed above
- each RTX 2080 Ti worker processed `56` files

Latest V2X-Radar OpenCOOD dataset smoke:

- directory:
  `results/v2x_benchmarks/v2x-radar-opencood-smoke-20260629T180847Z/`
- config:
  `opencood/hypes_yaml/v2x-radar/radar_only/collab_radaronly_radarpillarnet_late.yaml`
- dataset construction: passed
- dataset length: `922` validation samples across `5` scenarios
- first sample retrieval with `visualize=True`: passed
- first sample contains lidar, radar, and target-box arrays:
  `origin_lidar (42238, 4)`, `origin_radar (10008, 4)`,
  `object_bbx_center (150, 7)`, `object_bbx_mask (150,)`

Latest full V2X-Radar detector benchmark:

- directory:
  `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/`
- script: `scripts/run_v2x_radar_detector_benchmark.py`
- command used all four GPUs via `CUDA_VISIBLE_DEVICES=0,1,2,3`
- config:
  `opencood/hypes_yaml/v2x-radar/radar_only/collab_radaronly_radarpillarnet_late.yaml`
- checkpoint:
  `benchmarks/v2x/data/checkpoints/V2X-Radar/radar_late/v2x_radar_c_radar_late.pth`
- checkpoint load: no missing keys, no unexpected keys on every worker
- validation samples: `922/922`
- sample failures: `0`
- wall time: `828.747236` seconds
- detections: `186,880`
- ground-truth boxes: `7,948`
- overall AP@IoU `0.3`: `0.192143`
- overall AP@IoU `0.5`: `0.130266`
- overall AP@IoU `0.7`: `0.046459`
- 0-30 m AP@IoU `0.5` / `0.7`: `0.195438` / `0.083737`
- 30-50 m AP@IoU `0.5` / `0.7`: `0.082856` / `0.008698`
- 50-100 m AP@IoU `0.5` / `0.7`: `0.005684` / `0.001101`

Earlier V2X-Radar detector subset runs are preserved under:

- `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182445Z/`
- `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182522Z/`

OpenCOOD environment note:

- `results/v2x_benchmarks/latest/opencood_env_smoke.md`

## Environment

The existing conda environment `mmdet3d` is the active benchmark environment:

- Python `3.9.23`
- PyTorch `2.1.0+cu121`
- CUDA visible to PyTorch: yes
- CUDA device count: `4`

Installed or built during this task:

- `tensorboardX`, `timm`, `einops`, `icecream`
- `Cython`, `cumm`
- `python-lzf`, `h5py`, `spconv-cu120`
- `efficientnet_pytorch`, `SharedArray`
- OpenCOOD `box_overlaps` Cython extension in both the upstream OpenCOOD clone
  and the V2X-Radar OpenCOOD fork
- V2X-Radar OpenCOOD `packages/Voxelization` CUDA extensions:
  `bev_pool_ext`, `bev_pool_v2_ext`, and `voxel_layer`

OpenCOOD import smokes passed for:

- upstream `opencood.tools.train`
- upstream `opencood.tools.inference`
- V2X-Radar fork `opencood.tools.train`
- V2X-Radar fork `opencood.tools.test_ddp`

## Validation

Validation run after the latest documentation/result updates:

```bash
python3 -m py_compile scripts/download_v2x_benchmarks.py scripts/build_v2x_ipi_payload_manifest.py scripts/run_v2x_ipi_loopback.py scripts/run_v2x_gpu_dataset_benchmark.py
python3 -m py_compile scripts/run_v2x_radar_detector_benchmark.py
python3 -m json.tool benchmarks/v2x/sources.json >/dev/null
python3 -m json.tool results/v2x_benchmarks/latest/download_manifest.json >/dev/null
python3 -m json.tool results/v2x_benchmarks/latest/v2x_ipi_payload_manifest.json >/dev/null
python3 -m json.tool results/v2x_benchmarks/v2x-ipi-loopback-20260629T175556Z/summary.json >/dev/null
python3 -m json.tool results/v2x_benchmarks/v2x-gpu-dataset-benchmark-20260629T175939Z/summary.json >/dev/null
python3 -m json.tool results/v2x_benchmarks/v2x-radar-opencood-smoke-20260629T180847Z/summary.json >/dev/null
python3 -m json.tool results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/summary.json >/dev/null
git diff --check
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

Result:

- Python compile checks passed.
- JSON validation checks passed, including the full V2X-Radar detector summary.
- `git diff --check` passed.
- CMake build passed.
- CTest passed: `6/6` tests.

## GPU State

All four RTX 2080 Ti GPUs are visible and were used by both the four-GPU
dataset artifact benchmark and the full V2X-Radar detector validation run. An
existing `llama-server` process was also using memory on all four GPUs during
this work, but the radar-only detector run still completed with less than
`0.9G` peak allocated memory per detector worker.

## Next Steps

Optional follow-up work:

1. Stage full OPV2V, V2XSet, V2V4Real, or V2X-Real dataset/checkpoint pairs for
   additional detector comparisons beyond V2X-Radar.
2. Run larger camera/lidar V2X-Radar models if desired; they may need more GPU
   memory than the radar-only late-fusion checkpoint.
3. Convert the V2X-Radar detector outputs into additional IPI cooperative
   service payload sweeps if a detector-output transport benchmark is needed.
