# V2X-Radar Detector Benchmark

Run ID: `v2x-radar-detector-benchmark-20260629T182522Z`
Generated UTC: `2026-06-29T18:26:09.132363+00:00`

This run loads the official V2X-Radar radar-only late-fusion checkpoint
and evaluates a bounded subset of the staged cooperative validation split
across all selected GPUs. It is a partial detector benchmark unless
`max_samples` covers the full validation split.

## Result

- samples requested: `32`
- successful samples: `32`
- failed samples: `0`
- elapsed seconds: `46.656717`
- AP overall IoU 0.3: `0.187744`
- AP overall IoU 0.5: `0.14716`
- AP overall IoU 0.7: `0.069704`

## Workers

| gpu | name | assigned | successful | failures | elapsed_s | max_mem_bytes |
| ---: | --- | ---: | ---: | ---: | ---: | ---: |
| 2 | NVIDIA GeForce RTX 2080 Ti | 8 | 8 | 0 | 43.169194 | 877733888 |
| 1 | NVIDIA GeForce RTX 2080 Ti | 8 | 8 | 0 | 43.560418 | 877961728 |
| 3 | NVIDIA GeForce RTX 2080 Ti | 8 | 8 | 0 | 43.866362 | 878065664 |
| 0 | NVIDIA GeForce RTX 2080 Ti | 8 | 8 | 0 | 45.518447 | 878011392 |

Raw per-sample detections are in `per_sample.csv`; per-sample failures
are in `failures.csv`; complete AP/stat details are in `summary.json`.
