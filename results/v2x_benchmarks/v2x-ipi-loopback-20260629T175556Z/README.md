# V2X IPI Loopback Benchmark

Run ID: `v2x-ipi-loopback-20260629T175556Z`
Generated UTC: `2026-06-29T17:55:56.274920+00:00`

This run uses dataset-derived payload sizes with the local TCP IPI probe pair.
It measures IPI message validation and loopback transport behavior, not detector accuracy.

## Environment

```text
0, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 6362 MiB, 24 %
1, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5419 MiB, 0 %
2, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5957 MiB, 0 %
3, NVIDIA GeForce RTX 2080 Ti, 11264 MiB, 5923 MiB, 0 %
```

## Conditions

| benchmark | dataset | payload_bytes | attempts | accepted | success_rate_pct | p50_ms | p95_ms | p99_ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 0 | 20 | 20 | 100.0 | 87.889 | 87.915 | 87.946 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 256 | 20 | 20 | 100.0 | 87.875 | 87.920 | 87.929 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 1024 | 20 | 20 | 100.0 | 87.878 | 87.906 | 87.909 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 4096 | 20 | 20 | 100.0 | 87.870 | 87.922 | 87.926 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 5789 | 20 | 20 | 100.0 | 87.874 | 87.915 | 87.947 |
| OpenDAIR-V2X | DAIR-V2X-C-Example | 60000 | 20 | 20 | 100.0 | 43.844 | 43.896 | 43.905 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 0 | 20 | 20 | 100.0 | 87.896 | 87.930 | 91.854 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 256 | 20 | 20 | 100.0 | 87.878 | 87.929 | 91.846 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 1024 | 20 | 20 | 100.0 | 87.874 | 87.943 | 87.958 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 4096 | 20 | 20 | 100.0 | 87.879 | 87.904 | 87.913 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 5262 | 20 | 20 | 100.0 | 87.883 | 91.877 | 91.926 |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 60000 | 20 | 20 | 100.0 | 43.837 | 43.873 | 43.881 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 0 | 20 | 20 | 100.0 | 87.886 | 87.916 | 87.923 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 256 | 20 | 20 | 100.0 | 87.880 | 87.908 | 87.911 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 1024 | 20 | 20 | 100.0 | 87.880 | 87.973 | 91.884 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 4096 | 20 | 20 | 100.0 | 87.878 | 87.903 | 87.912 |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 60000 | 20 | 20 | 100.0 | 87.850 | 87.935 | 91.861 |
| TruckV2X | TruckV2X-test-Town1_2 | 0 | 20 | 20 | 100.0 | 87.876 | 87.924 | 87.927 |
| TruckV2X | TruckV2X-test-Town1_2 | 256 | 20 | 20 | 100.0 | 87.874 | 87.914 | 91.861 |
| TruckV2X | TruckV2X-test-Town1_2 | 1024 | 20 | 20 | 100.0 | 87.872 | 87.908 | 87.927 |
| TruckV2X | TruckV2X-test-Town1_2 | 4096 | 20 | 20 | 100.0 | 87.882 | 87.910 | 90.006 |
| TruckV2X | TruckV2X-test-Town1_2 | 40337 | 20 | 20 | 100.0 | 87.841 | 91.829 | 91.872 |
| TruckV2X | TruckV2X-test-Town1_2 | 60000 | 20 | 20 | 100.0 | 43.846 | 43.937 | 44.334 |
| V2X-Radar | V2X-Radar-C-validate | 0 | 20 | 20 | 100.0 | 87.888 | 87.919 | 87.934 |
| V2X-Radar | V2X-Radar-C-validate | 256 | 20 | 20 | 100.0 | 87.873 | 87.926 | 88.074 |
| V2X-Radar | V2X-Radar-C-validate | 1024 | 20 | 20 | 100.0 | 87.884 | 87.905 | 87.906 |
| V2X-Radar | V2X-Radar-C-validate | 4096 | 20 | 20 | 100.0 | 87.874 | 87.903 | 87.927 |
| V2X-Radar | V2X-Radar-C-validate | 60000 | 20 | 20 | 100.0 | 87.826 | 87.916 | 87.952 |
| V2X-Radar | V2X-Radar-V-ImageSets | 0 | 20 | 20 | 100.0 | 87.875 | 87.902 | 87.902 |
| V2X-Radar | V2X-Radar-V-ImageSets | 256 | 20 | 20 | 100.0 | 87.875 | 87.899 | 87.921 |
| V2X-Radar | V2X-Radar-V-ImageSets | 1024 | 20 | 20 | 100.0 | 87.880 | 87.953 | 91.890 |
| V2X-Radar | V2X-Radar-V-ImageSets | 4096 | 20 | 20 | 100.0 | 87.876 | 87.937 | 87.951 |
| V2X-Radar | V2X-Radar-V-ImageSets | 10507 | 20 | 20 | 100.0 | 87.873 | 87.905 | 87.909 |
| V2X-Radar | V2X-Radar-V-ImageSets | 60000 | 20 | 20 | 100.0 | 43.841 | 43.876 | 43.894 |

Raw CSVs and stderr logs are in this directory.
