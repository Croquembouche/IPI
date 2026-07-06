# Experiment 06 Weak-Signal Load/QoS Repeat

- Date: 2026-07-06
- Run ID: `edge4av-real-20260706-load-qos-weak-signal-run-2`
- Edge server: `10.100.100.6`
- Vehicle state: stationary weak-signal private-5G location
- Application payload: `1024` B service payload
- Probe count: `1000` per condition
- Probe interval: `200` ms
- GNSS: NovAtel ROS 2 CSV and bag under `gps/`
- QoS labels: `default` and `5qi-mapped`; keep this as metadata unless core-side 5QI enforcement is independently verified

## GNSS

- CSV samples: `1246611`
- BESTPOS samples: `163428`
- Mean BESTPOS position: `39.662918377, -75.757367366`
- Mean horizontal speed: `0.0022` m/s

## Latency Results

| Condition | Transport | Attempts | Accepted | Success | p50 ms | p95 ms | p99 ms | Mean ms | Max ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-mqtt-weak-signal-default-payload-1024-idle` | MQTT | 1000 | 1000 | 1000 | 50.997 | 159.750 | 302.069 | 80.165 | 727.951 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-1` | MQTT | 1000 | 1000 | 1000 | 109.860 | 460.498 | 761.785 | 131.588 | 1059.782 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-2` | MQTT | 1000 | 1000 | 1000 | 119.561 | 639.768 | 989.957 | 156.169 | 1672.384 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-4` | MQTT | 1000 | 1000 | 1000 | 101.824 | 707.914 | 1102.121 | 166.732 | 2053.650 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-1` | MQTT | 1000 | 1000 | 1000 | 109.846 | 439.812 | 726.004 | 129.397 | 1873.921 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-2` | MQTT | 1000 | 1000 | 1000 | 107.898 | 603.847 | 804.826 | 147.859 | 3313.961 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-4` | MQTT | 1000 | 1000 | 1000 | 86.822 | 659.979 | 1042.049 | 160.911 | 3617.959 |
| `p5g-tcp-weak-signal-default-payload-1024-idle` | TCP | 1000 | 1000 | 1000 | 130.329 | 150.083 | 190.065 | 133.437 | 550.910 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-1` | TCP | 1000 | 1000 | 1000 | 255.537 | 629.612 | 765.099 | 272.297 | 1881.321 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-2` | TCP | 1000 | 1000 | 1000 | 199.796 | 718.611 | 1080.202 | 268.227 | 1874.415 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-4` | TCP | 1000 | 1000 | 1000 | 170.694 | 758.947 | 979.068 | 269.503 | 3494.782 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-1` | TCP | 1000 | 1000 | 1000 | 199.191 | 413.496 | 744.182 | 244.033 | 1811.100 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-2` | TCP | 1000 | 1000 | 1000 | 224.611 | 685.762 | 767.799 | 265.662 | 1141.897 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-4` | TCP | 1000 | 1000 | 1000 | 142.393 | 762.553 | 1148.869 | 281.430 | 6588.139 |

## Verification

- 14 sender CSVs contain `1000` samples plus header.
- 14 edge receiver CSVs were pulled under `base_station/`.
- GNSS CSV and ROS bag were recorded.
- Local and edge experiment processes were cleaned up after the run.

## Notes

- This is the July 6 weak-signal repeat of Experiment 06, collected near `39.662918377, -75.757367366`.
- Every latency condition completed with `1000/1000` successful probes.
- Background load logs are preserved in the run folder and `base_station/`; interpret measured load rather than nominal stream target when writing the paper.
