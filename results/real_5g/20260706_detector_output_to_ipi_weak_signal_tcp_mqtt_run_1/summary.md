# Experiment 09 Weak-Signal TCP/MQTT Detector-Output Replay

- Date: 2026-07-06
- Run ID: `edge4av-real-20260706-detector-output-to-ipi-weak-signal-tcp-mqtt-run-1`
- Edge server: `10.100.100.6`
- Vehicle state: stationary weak-signal private-5G location
- Transports: TCP and MQTT
- Probe count: `1000` per payload and transport
- GNSS: NovAtel ROS 2 CSV and bag under `gps/`

## GNSS

- CSV samples: `1473435`
- BESTPOS samples: `193436`
- Mean BESTPOS position: `39.662918970, -75.757346005`
- Mean horizontal speed: `0.0022` m/s

## Results

| Condition | Transport | Attempts | Accepted | Success | p50 ms | p95 ms | p99 ms | Mean ms | Max ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-mqtt-detector-output-payload-0` | MQTT | 1000 | 1000 | 1000 | 29.864 | 71.866 | 104.011 | 33.381 | 289.810 |
| `p5g-mqtt-detector-output-payload-256` | MQTT | 1000 | 1000 | 1000 | 30.206 | 70.197 | 99.885 | 35.233 | 325.705 |
| `p5g-mqtt-detector-output-payload-1024` | MQTT | 1000 | 1000 | 1000 | 39.829 | 78.260 | 101.818 | 43.702 | 365.766 |
| `p5g-mqtt-detector-output-payload-4096` | MQTT | 1000 | 1000 | 1000 | 55.779 | 93.696 | 137.982 | 61.312 | 975.154 |
| `p5g-mqtt-detector-output-payload-19648` | MQTT | 1000 | 1000 | 1000 | 132.070 | 254.138 | 700.003 | 154.530 | 1755.743 |
| `p5g-mqtt-detector-output-payload-22816` | MQTT | 1000 | 1000 | 1000 | 168.714 | 539.780 | 824.319 | 211.557 | 1183.715 |
| `p5g-mqtt-detector-output-payload-23968` | MQTT | 1000 | 1000 | 1000 | 165.946 | 423.936 | 758.201 | 198.880 | 1099.883 |
| `p5g-mqtt-detector-output-payload-25024` | MQTT | 1000 | 1000 | 1000 | 155.767 | 325.888 | 715.870 | 185.277 | 889.843 |
| `p5g-mqtt-detector-output-payload-60000` | MQTT | 1000 | 1000 | 1000 | 345.221 | 770.058 | 1229.991 | 403.031 | 2272.031 |
| `p5g-tcp-detector-output-payload-0` | TCP | 1000 | 1000 | 1000 | 120.397 | 205.873 | 257.891 | 132.622 | 394.722 |
| `p5g-tcp-detector-output-payload-256` | TCP | 1000 | 1000 | 1000 | 129.539 | 203.797 | 259.836 | 135.447 | 543.890 |
| `p5g-tcp-detector-output-payload-1024` | TCP | 1000 | 1000 | 1000 | 130.337 | 227.307 | 279.959 | 143.692 | 369.739 |
| `p5g-tcp-detector-output-payload-4096` | TCP | 1000 | 1000 | 1000 | 80.668 | 169.852 | 229.752 | 93.901 | 281.555 |
| `p5g-tcp-detector-output-payload-19648` | TCP | 1000 | 1000 | 1000 | 151.675 | 285.725 | 670.581 | 176.225 | 841.817 |
| `p5g-tcp-detector-output-payload-22816` | TCP | 1000 | 1000 | 1000 | 170.228 | 321.940 | 750.054 | 198.035 | 942.866 |
| `p5g-tcp-detector-output-payload-23968` | TCP | 1000 | 1000 | 1000 | 169.843 | 301.906 | 577.215 | 189.878 | 825.830 |
| `p5g-tcp-detector-output-payload-25024` | TCP | 1000 | 1000 | 1000 | 179.841 | 322.378 | 695.550 | 203.003 | 1386.996 |
| `p5g-tcp-detector-output-payload-60000` | TCP | 1000 | 1000 | 1000 | 340.158 | 661.814 | 1026.538 | 382.123 | 3459.488 |

## Verification

- 18 sender CSVs contain `1000` samples plus header.
- 18 edge receiver CSVs were pulled under `base_station/`.
- GNSS CSV and ROS bag were recorded.
- Local and edge experiment processes were cleaned up after the run.

## Notes

- This is the weak-signal TCP/MQTT repeat for Experiment 09.
- All TCP and MQTT payload points completed with `1000/1000` successful probes, including the `60000` B upper stress-control payload.
