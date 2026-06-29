# V2X-Radar OpenCOOD Dataset Smoke

Run ID: `v2x-radar-opencood-smoke-20260629T180847Z`

Generated UTC: `2026-06-29T18:08:47Z`

This smoke test verifies that the staged V2X-Radar cooperative validation split
can be loaded through the V2X-Radar fork of OpenCOOD. It is a dataset and
framework integration check, not detector AP/IoU evaluation.

## Inputs

- repository: `benchmarks/v2x/repos/V2X-Radar/CodeBase/OpenCOOD`
- config: `opencood/hypes_yaml/v2x-radar/radar_only/collab_radaronly_radarpillarnet_late.yaml`
- data path: `benchmarks/v2x/data/v2x-radar/v2x-radar-c/validate`
- source extraction: `benchmarks/v2x/data/extracted/V2X-Radar-C-validate/validate`
- conda environment: `mmdet3d`

The `benchmarks/v2x/data/v2x-radar/v2x-radar-c/validate` path is an ignored
local symlink to the extracted validation split. The V2X-Radar OpenCOOD fork
uses the literal `v2x-radar` path component to choose camera filename patterns,
so preserving that path shape is required.

## Result

- `opencood.tools.train` import: passed before this smoke.
- `opencood.tools.test_ddp` import: passed before this smoke.
- dataset construction: passed.
- dataset length: `922` samples.
- scenario folders: `5`.
- first sample retrieval with `visualize=True`: passed.
- first sample top-level keys: `ego`, `142`, `-1`.
- ego sample keys include `input_m1`, `origin_lidar`, `origin_radar`,
  `object_bbx_center`, `object_bbx_mask`, `pairwise_t_matrix`, and
  `yaml_file_path`.
- `origin_lidar` shape: `(42238, 4)`.
- `origin_radar` shape: `(10008, 4)`.
- `object_bbx_center` shape: `(150, 7)`.
- `object_bbx_mask` shape: `(150,)`.

## Notes

Single-modality item retrieval with `visualize=False` currently fails inside the
V2X-Radar OpenCOOD fork because `unified_fusion_dataset.py` unconditionally
appends both projected lidar and projected radar arrays. The `visualize=True`
path loads both projections and allows a real sample retrieval smoke without
patching the ignored upstream checkout. Full detector AP/IoU evaluation still
requires checkpoint/config selection and a separate recorded run.
