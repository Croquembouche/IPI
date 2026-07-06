# Experiment 09 Weak-Signal Fragmented UDP Detector-Output Replay

- Date: 2026-07-06
- Run ID: `edge4av-real-20260706-detector-output-to-ipi-weak-signal-udp-fragmented-run-1`
- Edge server: `10.100.100.6`
- Vehicle state: stationary weak-signal private-5G location
- Transport: UDP with application-level fragmentation and reassembly
- UDP maximum datagram payload: `1400` B
- Probe count: `1000` per main payload; `100` for the `60000` B stress-control payload
- GNSS: NovAtel ROS 2 CSV and bag under `gps/`

## GNSS

- CSV samples: `974033`
- BESTPOS samples: `127890`
- Mean BESTPOS position: `39.662913314, -75.757324694`
- Mean horizontal speed: `0.0022` m/s

## Results

| Condition | Attempts | Accepted | Success | p50 ms | p95 ms | p99 ms | Mean ms | Max ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-udp-detector-output-payload-0` | 1000 | 999 | 999 | 22.358 | 69.966 | 99.859 | 31.220 | 146.602 |
| `p5g-udp-detector-output-payload-256` | 1000 | 999 | 999 | 30.281 | 71.878 | 100.449 | 35.827 | 166.033 |
| `p5g-udp-detector-output-payload-1024` | 1000 | 1000 | 1000 | 37.714 | 73.450 | 100.398 | 39.530 | 126.492 |
| `p5g-udp-detector-output-payload-4096` | 1000 | 999 | 999 | 50.323 | 99.825 | 130.846 | 56.883 | 160.784 |
| `p5g-udp-detector-output-payload-19648` | 1000 | 961 | 961 | 132.903 | 171.222 | 190.365 | 137.356 | 210.833 |
| `p5g-udp-detector-output-payload-22816` | 1000 | 935 | 935 | 147.769 | 189.321 | 210.608 | 148.799 | 241.815 |
| `p5g-udp-detector-output-payload-23968` | 1000 | 948 | 948 | 149.938 | 180.396 | 200.979 | 150.439 | 240.106 |
| `p5g-udp-detector-output-payload-25024` | 1000 | 936 | 936 | 159.852 | 200.034 | 227.578 | 160.807 | 272.340 |
| `p5g-udp-detector-output-payload-60000` | 100 | 0 | 0 |  |  |  |  |  |

## Verification

- 9 sender CSVs were produced.
- Main UDP payloads contain `1000` samples plus header.
- The `60000` B stress-control file contains `100` samples plus header by design.
- 9 edge receiver CSVs were pulled under `base_station/`.
- GNSS CSV and ROS bag were recorded.
- Local and edge experiment processes were cleaned up after the run.

## Notes

- This completes the weak-signal UDP leg of Experiment 09.
- Main detector-size UDP payloads were usable but degraded at larger payloads: `19648` to `25024` B accepted `93.5%` to `96.1%`.
- The `60000` B stress-control point had `0/100` accepted probes, which is useful evidence for the large-fragment reliability limit under weak signal.
