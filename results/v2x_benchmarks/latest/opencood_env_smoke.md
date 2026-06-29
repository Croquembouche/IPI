# OpenCOOD Environment Smoke Check

Generated: 2026-06-29

## Environment

- Conda environment: `mmdet3d`
- Python: `3.9.23`
- PyTorch: `2.1.0+cu121`
- CUDA visible to PyTorch: yes
- CUDA device count: `4`

## Installed During This Task

```bash
conda run -n mmdet3d python -m pip install tensorboardX timm einops icecream
conda run -n mmdet3d python -m pip install Cython
conda run -n mmdet3d python -m pip install cumm
conda run -n mmdet3d python -m pip install python-lzf h5py
conda run -n mmdet3d python -m pip install spconv-cu120
conda run -n mmdet3d python -m pip install efficientnet_pytorch SharedArray
```

## Built In Ignored OpenCOOD Checkout

```bash
cd benchmarks/v2x/repos/OpenCOOD
conda run -n mmdet3d python opencood/utils/setup.py build_ext --inplace
```

This built:

```text
benchmarks/v2x/repos/OpenCOOD/opencood/utils/box_overlaps.cpython-39-x86_64-linux-gnu.so
```

The same Cython extension was also built inside the ignored V2X-Radar OpenCOOD
fork:

```bash
cd benchmarks/v2x/repos/V2X-Radar/CodeBase/OpenCOOD
conda run -n mmdet3d python opencood/utils/setup.py build_ext --inplace
```

The V2X-Radar fork's voxelization CUDA extensions were also built in the
ignored checkout:

```bash
cd benchmarks/v2x/repos/V2X-Radar/CodeBase/OpenCOOD
MAX_JOBS=4 conda run -n mmdet3d python packages/Voxelization/setup.py build_ext --inplace
MAX_JOBS=4 conda run -n mmdet3d python packages/Voxelization/setup_v2.py build_ext --inplace
```

## Smoke Checks

```bash
conda run -n mmdet3d env PYTHONPATH=/media/william/mist2/william/Github/IPI/benchmarks/v2x/repos/OpenCOOD \
  python -c "import opencood.tools.train; print('train ok')"

conda run -n mmdet3d env PYTHONPATH=/media/william/mist2/william/Github/IPI/benchmarks/v2x/repos/OpenCOOD \
  python -c "import opencood.tools.inference; print('inference ok')"
```

Observed output:

```text
train ok
inference ok
```

V2X-Radar fork import checks also passed:

```bash
conda run -n mmdet3d env PYTHONPATH=/media/william/mist2/william/Github/IPI/benchmarks/v2x/repos/V2X-Radar/CodeBase/OpenCOOD \
  python -c "import opencood.tools.train; print('v2x-radar train import ok')"

conda run -n mmdet3d env PYTHONPATH=/media/william/mist2/william/Github/IPI/benchmarks/v2x/repos/V2X-Radar/CodeBase/OpenCOOD \
  python -c "import opencood.tools.test_ddp; print('v2x-radar test_ddp import ok')"
```

The V2X-Radar cooperative validation split was loaded through the fork's
OpenCOOD dataset path in:

```text
results/v2x_benchmarks/v2x-radar-opencood-smoke-20260629T180847Z/
```

That smoke built a `922`-sample dataset across `5` scenarios and retrieved one
validation sample with lidar, radar, and target-box arrays.

The V2X-Radar detector model creation and full validation benchmark also
passed through `scripts/run_v2x_radar_detector_benchmark.py`. That script uses a
small runtime compatibility shim for the fork's old `mmcv.runner` imports while
leaving the ignored upstream checkout unchanged.

Latest full detector run:

```text
results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/
```

Observed result:

- official radar-only late-fusion checkpoint loaded with no missing or
  unexpected keys
- `922/922` validation samples completed
- `0` sample failures
- all four RTX 2080 Ti GPUs used
- overall AP@IoU `0.5`: `0.130266`
- overall AP@IoU `0.7`: `0.046459`

## Remaining Optional Detector Benchmark Targets

- Stage full OPV2V/V2XSet, V2V4Real, or V2X-Real dataset/checkpoint pairs for
  additional detector comparisons.
- Keep the existing `llama-server` GPU memory use in mind when increasing
  detector batch size or moving to larger camera/lidar models.
