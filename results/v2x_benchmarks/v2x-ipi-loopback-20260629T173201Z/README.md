# V2X IPI Loopback Benchmark

Run ID: `v2x-ipi-loopback-20260629T173201Z`
Generated UTC: `2026-06-29T17:32:01.521204+00:00`

This run uses dataset-derived payload sizes with the local TCP IPI probe pair.
It measures IPI message validation and loopback transport behavior, not detector accuracy.

## Environment

```text
0, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 6362 MiB, 26 %
1, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5419 MiB, 0 %
2, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5957 MiB, 0 %
3, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5923 MiB, 0 %
```

## Conditions

| payload_bytes | attempts | accepted | success_rate_pct | p50_ms | p95_ms | p99_ms |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 0 | 50 | 50 | 100.0 | 87.888 | 87.950 | 87.985 |
| 256 | 50 | 50 | 100.0 | 87.881 | 87.926 | 91.864 |
| 1024 | 50 | 50 | 100.0 | 87.879 | 87.951 | 87.983 |
| 4096 | 50 | 50 | 100.0 | 87.888 | 87.956 | 91.892 |
| 36307 | 50 | 50 | 100.0 | 87.851 | 87.914 | 87.934 |
| 60000 | 50 | 50 | 100.0 | 87.842 | 87.868 | 87.907 |

Raw CSVs and stderr logs are in this directory.
