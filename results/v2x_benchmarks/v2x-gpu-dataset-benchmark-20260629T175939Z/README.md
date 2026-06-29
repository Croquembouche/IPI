# V2X Four-GPU Dataset Benchmark

Run ID: `v2x-gpu-dataset-benchmark-20260629T175939Z`
Generated UTC: `2026-06-29T17:59:54.051066+00:00`

This benchmark replays staged V2X dataset artifacts through a small CUDA
tensor workload on all visible GPUs. It is a dataset/GPU plumbing and
payload-processing benchmark, not detector AP/IoU.

## Result

- files: `224`
- successes: `224`
- failures: `0`
- processed bytes: `128904577`
- wall time seconds: `14.770803`
- throughput MB/s: `8.727`

## Workers

| gpu | name | files | failures | processed_bytes | throughput_mb_s | max_mem_bytes |
| ---: | --- | ---: | ---: | ---: | ---: | ---: |
| 0 | NVIDIA GeForce RTX 2080 Ti | 56 | 0 | 32973011 | 2.826 | 17349632 |
| 1 | NVIDIA GeForce RTX 2080 Ti | 56 | 0 | 32086079 | 2.895 | 17347584 |
| 2 | NVIDIA GeForce RTX 2080 Ti | 56 | 0 | 31994604 | 2.887 | 17346560 |
| 3 | NVIDIA GeForce RTX 2080 Ti | 56 | 0 | 31850883 | 2.874 | 17344512 |

## Datasets

| benchmark | dataset | files | processed_bytes | gpu_ms_total | categories |
| --- | --- | ---: | ---: | ---: | --- |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 48 | 26750377 | 882.263 | {"annotation": 16, "image": 16, "point_cloud": 16} |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 48 | 29359370 | 25.215 | {"annotation": 16, "image": 16, "point_cloud": 16} |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 28 | 29360128 | 17.21 | {"annotation": 28} |
| TruckV2X | TruckV2X-test-Town1_2 | 48 | 17278974 | 20.489 | {"annotation": 16, "image": 16, "point_cloud": 16} |
| V2X-Radar | V2X-Radar-C-validate | 48 | 26006775 | 21.864 | {"annotation": 16, "image": 16, "point_cloud": 16} |
| V2X-Radar | V2X-Radar-V-ImageSets | 4 | 148953 | 0.921 | {"annotation": 4} |

Raw per-file timings are in `per_file.csv`; failures are in `failures.csv`.
