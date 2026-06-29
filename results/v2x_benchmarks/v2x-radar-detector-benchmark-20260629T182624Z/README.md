# V2X-Radar Detector Benchmark

Run ID: `v2x-radar-detector-benchmark-20260629T182624Z`
Generated UTC: `2026-06-29T18:40:14.953346+00:00`

This run loads the official V2X-Radar radar-only late-fusion checkpoint
and evaluates a bounded subset of the staged cooperative validation split
across all selected GPUs. It is a partial detector benchmark unless
`max_samples` covers the full validation split.

## Result

- samples requested: `922`
- successful samples: `922`
- failed samples: `0`
- elapsed seconds: `828.747236`
- AP overall IoU 0.3: `0.192143`
- AP overall IoU 0.5: `0.130266`
- AP overall IoU 0.7: `0.046459`

## Workers

| gpu | name | assigned | successful | failures | elapsed_s | max_mem_bytes |
| ---: | --- | ---: | ---: | ---: | ---: | ---: |
| 0 | NVIDIA GeForce RTX 2080 Ti | 231 | 231 | 0 | 817.35021 | 878480896 |
| 3 | NVIDIA GeForce RTX 2080 Ti | 230 | 230 | 0 | 819.986662 | 879993856 |
| 2 | NVIDIA GeForce RTX 2080 Ti | 230 | 230 | 0 | 825.310519 | 879707136 |
| 1 | NVIDIA GeForce RTX 2080 Ti | 231 | 231 | 0 | 827.541378 | 880253440 |

Raw per-sample detections are in `per_sample.csv`; per-sample failures
are in `failures.csv`; complete AP/stat details are in `summary.json`.
