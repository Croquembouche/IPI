# V2X IPI Loopback Benchmark

Run ID: `v2x-ipi-loopback-20260629T170448Z`
Generated UTC: `2026-06-29T17:04:48.921370+00:00`

This run uses dataset-derived payload sizes with the local TCP IPI probe pair.
It measures IPI message validation and loopback transport behavior, not detector accuracy.

## Environment

```text
0, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 6362 MiB, 28 %
1, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5419 MiB, 0 %
2, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5957 MiB, 0 %
3, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5923 MiB, 0 %
```

## Conditions

| payload_bytes | attempts | accepted | success_rate_pct | p50_ms | p95_ms | p99_ms |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 0 | 50 | 50 | 100.0 | 87.891 | 87.946 | 87.949 |
| 256 | 50 | 50 | 100.0 | 87.885 | 87.919 | 91.732 |
| 1024 | 50 | 50 | 100.0 | 87.883 | 91.876 | 91.898 |
| 4096 | 50 | 50 | 100.0 | 87.885 | 91.846 | 91.889 |
| 5447 | 50 | 50 | 100.0 | 87.884 | 87.971 | 91.882 |
| 60000 | 50 | 50 | 100.0 | 87.847 | 91.693 | 91.863 |

Raw CSVs and stderr logs are in this directory.
