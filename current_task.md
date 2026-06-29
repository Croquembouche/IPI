# Current Task

Last updated: 2026-06-29 21:12 UTC

## Task

Implement the full top-tier Edge4AV experiment collection suite: code, build
validation, starter scripts, and operator instructions for all planned
MobiCom/MobiSys-grade experiments.

## Status

Completed for the local implementation and validation scope. Physical
collection still requires the radio/private-5G devices, GPS stack, MQTT broker,
and operator-triggered route/failure runs.

## Current Implementation Scope

New/updated source and scripts:

- `cpp/examples/library/private_5g_latency_udp_sender.cpp`
- `cpp/examples/library/private_5g_latency_udp_receiver.cpp`
- `cpp/CMakeLists.txt`
- `scripts/edge4av_loadgen.py`
- `scripts/build_detector_output_ipi_payloads.py`
- `scripts/analyze_edge4av_deadlines.py`
- `scripts/create_edge4av_experiment_suite.py`
- `top_tier_experiment_collection.md`
- `agent_context.md`

Experiment suite target:

- 12 generated starter scripts under
  `results/edge4av_top_tier/<run_id>/start_scripts/`
- per-run `experiment_matrix.csv`, `INSTRUCTIONS.md`, and
  `suite_manifest.json`
- support for radio RTT, private-5G TCP/MQTT/UDP probes, controlled TCP/UDP
  load generation, detector-output payload replay, deadline analysis, offload
  tradeoff collection, multi-client scalability, and failure/fallback event
  logging.

Validation run:

```bash
python3 -m py_compile scripts/edge4av_loadgen.py scripts/build_detector_output_ipi_payloads.py scripts/analyze_edge4av_deadlines.py scripts/create_edge4av_experiment_suite.py
git diff --check
python3 scripts/create_edge4av_experiment_suite.py --results-root tmp/edge4av_suite_smoke --run-id smoke
for f in tmp/edge4av_suite_smoke/smoke/start_scripts/*.sh; do bash -n "$f" || exit 1; done
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
python3 scripts/build_detector_output_ipi_payloads.py --output tmp/detector_output_ipi_payloads.json
```

Additional smoke checks:

- Direct UDP latency loopback with
  `example_private_5g_latency_udp_receiver --once --csv` and
  `example_private_5g_latency_udp_sender --count 1 --csv`; sender and receiver
  each emitted one accepted CSV sample.
- Short UDP load-generator server/client run at `1 Mbps`; client sent 245
  packets and the server received 245 packets.

Validation results:

- Python syntax checks passed.
- Generated 12 starter scripts, `experiment_matrix.csv`, `INSTRUCTIONS.md`,
  and `suite_manifest.json`.
- Generated starter scripts passed `bash -n`.
- CMake configure/build passed.
- CTest passed: `6/6` tests.
- Detector-output payload helper found `922` samples and representative
  payloads `0, 256, 1024, 4096, 19648, 22816, 23968, 25024, 60000` bytes.

## Previous Paper Update Context

The prior task reviewed newly included V2X benchmark results and added
supported material to the Edge4AV paper. That scope is complete and preserved
below for continuity.

## Previous Paper Status

Completed for the paper-integration scope.

## Paper Updates

Paper files updated under the ignored `paper/` tree:

- `paper/sections/0_abstract.tex`
- `paper/sections/1_introduction.tex`
- `paper/sections/5_experiments.tex`
- `paper/sections/6_discussionandconclusion.tex`
- `paper/ref.bib`
- `paper/IEEE-conference-template-062824.tex`

Content added:

- Summarized the public V2X benchmark integration in the abstract while keeping
  the safety and tight-loop cooperative-perception boundaries explicit.
- Added the benchmark integration to the introduction evidence base and
  contribution list.
- Added a new `Public V2X Benchmark Integration` subsection to the experiment
  section.
- Added Table `tab:v2x-benchmark-integration`, covering dataset-derived IPI
  payload manifest results, local loopback transport, four-GPU artifact
  processing, V2X-Radar OpenCOOD smoke, and the full V2X-Radar detector
  validation.
- Updated the discussion to distinguish local benchmark/detector integration
  from deployed network-service readiness.
- Added bibliography entries for OpenCOOD, OpenDAIR-V2X, TruckV2X, and
  V2X-Radar, plus `\usepackage{url}` in the paper preamble.

Important claim boundary:

- The local IPI benchmark loopback validates payload packaging and transport
  mechanics for dataset-derived payload conditions; it is not presented as a
  private-5G network measurement.
- The V2X-Radar detector run validates the local cooperative-perception
  benchmark stack and provides workload scale; it is not presented as evidence
  for tight-loop networked cooperative perception or closed-loop AV safety.

Validation run:

```bash
pdflatex -interaction=nonstopmode IEEE-conference-template-062824.tex
bibtex IEEE-conference-template-062824
pdflatex -interaction=nonstopmode IEEE-conference-template-062824.tex
pdflatex -interaction=nonstopmode IEEE-conference-template-062824.tex
```

Result:

- Build completed successfully from `paper/`.
- New bibliography citations resolved after the BibTeX and final LaTeX passes.
- Final PDF: `paper/IEEE-conference-template-062824.pdf` with 13 pages.
- Remaining log messages are layout warnings and the existing missing-author
  warning; no LaTeX errors, undefined citations, or unresolved references were
  found in the final log scan.

Repository note:

- The top-level `.gitignore` ignores `paper/`, so these paper edits and
  regenerated paper artifacts do not appear in `git status` unless force-added.

## Benchmark Staging Context

Download and stage V2X datasets and benchmark code so IPI can be implemented
and tested against public V2X benchmark assets. Use at least three different
V2X benchmark/dataset families, preserve all recorded results, and use the
available four RTX 2080 Ti GPUs for GPU benchmark runs when those runs are
ready.

## Benchmark Status

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
