# V2X IPI Loopback Benchmark

Run ID: `v2x-ipi-loopback-20260629T173649Z`
Generated UTC: `2026-06-29T17:36:49.350387+00:00`

This run uses dataset-derived payload sizes with the local TCP IPI probe pair.
It measures IPI message validation and loopback transport behavior, not detector accuracy.

## Environment

```text
0, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 6362 MiB, 27 %
1, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5419 MiB, 0 %
2, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5957 MiB, 0 %
3, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5923 MiB, 0 %
```

## Conditions

| benchmark | dataset | payload_bytes | attempts | accepted | success_rate_pct | p50_ms | p95_ms | p99_ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 0 | 20 | 20 | 100.0 | 87.878 | 87.914 | 87.944 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 256 | 20 | 20 | 100.0 | 87.888 | 87.915 | 87.916 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 1024 | 20 | 20 | 100.0 | 87.870 | 87.905 | 87.905 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 4096 | 20 | 20 | 100.0 | 87.877 | 87.902 | 87.910 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 5789 | 20 | 20 | 100.0 | 87.869 | 87.908 | 87.919 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 60000 | 20 | 20 | 100.0 | 87.839 | 87.877 | 87.882 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 0 | 20 | 20 | 100.0 | 87.888 | 87.927 | 87.950 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 256 | 20 | 20 | 100.0 | 87.875 | 87.907 | 87.942 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 1024 | 20 | 20 | 100.0 | 87.865 | 87.905 | 87.906 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 4096 | 20 | 20 | 100.0 | 87.872 | 87.897 | 87.910 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 5262 | 20 | 20 | 100.0 | 87.871 | 87.900 | 87.915 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 60000 | 20 | 20 | 100.0 | 87.840 | 87.881 | 87.903 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 0 | 20 | 20 | 100.0 | 87.881 | 87.932 | 91.897 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 256 | 20 | 20 | 100.0 | 87.877 | 87.906 | 87.915 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 1024 | 20 | 20 | 100.0 | 87.879 | 91.825 | 91.906 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 4096 | 20 | 20 | 100.0 | 87.878 | 91.855 | 91.860 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 60000 | 20 | 20 | 100.0 | 43.837 | 43.903 | 43.948 |
| TruckV2X | TruckV2X-test-Town1_2 | 0 | 20 | 20 | 100.0 | 87.887 | 87.955 | 88.100 |
| TruckV2X | TruckV2X-test-Town1_2 | 256 | 20 | 20 | 100.0 | 87.873 | 87.905 | 87.918 |
| TruckV2X | TruckV2X-test-Town1_2 | 1024 | 20 | 20 | 100.0 | 87.886 | 91.865 | 91.878 |
| TruckV2X | TruckV2X-test-Town1_2 | 4096 | 20 | 20 | 100.0 | 87.873 | 87.901 | 87.912 |
| TruckV2X | TruckV2X-test-Town1_2 | 40337 | 20 | 20 | 100.0 | 87.849 | 91.847 | 91.848 |
| TruckV2X | TruckV2X-test-Town1_2 | 60000 | 20 | 20 | 100.0 | 43.841 | 43.886 | 43.892 |
| V2X-Radar | V2X-Radar-V-ImageSets | 0 | 20 | 20 | 100.0 | 87.884 | 87.910 | 87.930 |
| V2X-Radar | V2X-Radar-V-ImageSets | 256 | 20 | 20 | 100.0 | 87.885 | 87.905 | 88.132 |
| V2X-Radar | V2X-Radar-V-ImageSets | 1024 | 20 | 20 | 100.0 | 87.873 | 87.903 | 87.905 |
| V2X-Radar | V2X-Radar-V-ImageSets | 4096 | 20 | 20 | 100.0 | 87.879 | 87.898 | 87.909 |
| V2X-Radar | V2X-Radar-V-ImageSets | 10507 | 20 | 20 | 100.0 | 87.881 | 87.916 | 88.117 |
| V2X-Radar | V2X-Radar-V-ImageSets | 60000 | 20 | 20 | 100.0 | 43.856 | 43.904 | 43.912 |

Raw CSVs and stderr logs are in this directory.
