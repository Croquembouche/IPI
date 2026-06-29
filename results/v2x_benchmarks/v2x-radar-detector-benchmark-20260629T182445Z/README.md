# V2X-Radar Detector Benchmark

Run ID: `v2x-radar-detector-benchmark-20260629T182445Z`
Generated UTC: `2026-06-29T18:25:04.995335+00:00`

This run loads the official V2X-Radar radar-only late-fusion checkpoint
and evaluates a bounded subset of the staged cooperative validation split
across all selected GPUs. It is a partial detector benchmark unless
`max_samples` covers the full validation split.

## Result

- samples requested: `4`
- successful samples: `4`
- failed samples: `0`
- elapsed seconds: `19.356265`
- AP overall IoU 0.3: `0.15874`
- AP overall IoU 0.5: `0.098491`
- AP overall IoU 0.7: `0.01486`

## Workers

| gpu | name | assigned | successful | failures | elapsed_s | max_mem_bytes |
| ---: | --- | ---: | ---: | ---: | ---: | ---: |
| 3 | NVIDIA GeForce RTX 2080 Ti | 1 | 1 | 0 | 17.246559 | 731301888 |
| 0 | NVIDIA GeForce RTX 2080 Ti | 1 | 1 | 0 | 17.412735 | 731194880 |
| 2 | NVIDIA GeForce RTX 2080 Ti | 1 | 1 | 0 | 17.978784 | 731404800 |
| 1 | NVIDIA GeForce RTX 2080 Ti | 1 | 1 | 0 | 18.190667 | 730926080 |

Raw per-sample detections are in `per_sample.csv`; per-sample failures
are in `failures.csv`; complete AP/stat details are in `summary.json`.
