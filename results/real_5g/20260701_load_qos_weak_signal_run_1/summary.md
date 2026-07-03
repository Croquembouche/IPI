# Experiment 06 Weak-Signal Load/QoS Summary

- Run ID: `edge4av-real-20260701-load-qos-weak-signal-run-1`
- Date/time: `2026-07-01`, stationary private-5G weak-signal location
- Edge server: `10.100.100.6`
- Vehicle/private-5G interface: `eno2`, UE IP observed earlier as `10.120.121.35/29`
- Payload: `1024` B application payload, `1096` B encoded service frame
- Probe count: `1000` per condition
- Probe interval: `200` ms
- GNSS: recorded with NovAtel ROS 2 package under `gps/`
- QoS profile labels: `default` and `5qi-mapped`; these are experiment metadata labels unless core-side 5QI enforcement is independently verified
- Background load target: local vehicle host to edge load server, nominal `25` Mbps per stream, `65536` B load packets
- Stream counts: `1`, `2`, `4`; each stream uses a separate edge TCP port

## Latency Results

| Condition | Transport | QoS | Streams | Accepted | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms | Mean RTT ms | Max RTT ms |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-tcp-weak-signal-default-payload-1024-idle` | TCP | default | 0 | 1000/1000 | 1000/1000 | 225.479 | 668.347 | 917.137 | 283.621 | 11244.603 |
| `p5g-mqtt-weak-signal-default-payload-1024-idle` | MQTT | default | 0 | 1000/1000 | 1000/1000 | 91.391 | 551.800 | 861.854 | 143.127 | 2625.670 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-1` | TCP | default | 1 | 1000/1000 | 1000/1000 | 230.823 | 681.939 | 1103.948 | 301.995 | 11507.854 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-2` | TCP | default | 2 | 1000/1000 | 1000/1000 | 310.600 | 933.823 | 1742.241 | 372.778 | 6009.106 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-4` | TCP | default | 4 | 1000/1000 | 1000/1000 | 309.837 | 819.544 | 1221.815 | 358.641 | 3475.614 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-1` | MQTT | default | 1 | 1000/1000 | 1000/1000 | 114.748 | 637.832 | 1073.792 | 175.741 | 5895.790 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-2` | MQTT | default | 2 | 1000/1000 | 1000/1000 | 129.923 | 717.610 | 1179.126 | 194.085 | 2371.936 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-4` | MQTT | default | 4 | 1000/1000 | 1000/1000 | 139.675 | 769.656 | 1175.146 | 228.612 | 5487.634 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-1` | TCP | 5qi-mapped | 1 | 1000/1000 | 1000/1000 | 310.648 | 752.440 | 1102.336 | 356.042 | 6167.329 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-2` | TCP | 5qi-mapped | 2 | 1000/1000 | 1000/1000 | 239.680 | 661.442 | 918.585 | 284.869 | 6520.286 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-4` | TCP | 5qi-mapped | 4 | 1000/1000 | 1000/1000 | 261.505 | 780.026 | 1187.425 | 322.183 | 3587.928 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-1` | MQTT | 5qi-mapped | 1 | 1000/1000 | 1000/1000 | 131.685 | 702.193 | 1129.698 | 204.651 | 5956.464 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-2` | MQTT | 5qi-mapped | 2 | 1000/1000 | 1000/1000 | 125.809 | 773.772 | 1371.831 | 217.738 | 5827.717 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-4` | MQTT | 5qi-mapped | 4 | 1000/1000 | 1000/1000 | 120.111 | 767.818 | 1392.704 | 221.948 | 8241.675 |

## Measured Background Load

Report measured aggregate load, not the nominal `25` Mbps per-stream target.
Several client streams timed out while sending at the weak-signal location; the
table records only streams that produced completed load-generator summary rows.

| Condition | Transport | QoS | Streams | Client Aggregate Mbps | Server Aggregate Mbps | Client Bytes | Server Bytes | Client Streams Reported | Server Streams Reported |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-1` | TCP | default | 1 | 0.000 | 0.026 | 0 | 851968 | 0 | 1 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-2` | TCP | default | 2 | 0.262 | 0.234 | 7602176 | 7602176 | 2 | 2 |
| `p5g-tcp-weak-signal-default-payload-1024-uplink-streams-4` | TCP | default | 4 | 0.137 | 0.316 | 3997696 | 10263432 | 1 | 4 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-1` | MQTT | default | 1 | 0.000 | 0.042 | 0 | 1376256 | 0 | 1 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-2` | MQTT | default | 2 | 0.251 | 0.234 | 7274496 | 7601744 | 1 | 2 |
| `p5g-mqtt-weak-signal-default-payload-1024-uplink-streams-4` | MQTT | default | 4 | 0.137 | 0.273 | 3997696 | 8876008 | 1 | 4 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-1` | TCP | 5qi-mapped | 1 | 0.214 | 0.189 | 6160384 | 6160384 | 1 | 1 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-2` | TCP | 5qi-mapped | 2 | 0.000 | 0.013 | 0 | 437296 | 0 | 2 |
| `p5g-tcp-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-4` | TCP | 5qi-mapped | 4 | 0.178 | 0.271 | 5111808 | 8818688 | 1 | 4 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-1` | MQTT | 5qi-mapped | 1 | 0.261 | 0.232 | 7536640 | 7536640 | 1 | 1 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-2` | MQTT | 5qi-mapped | 2 | 0.322 | 0.286 | 9306112 | 9306112 | 2 | 2 |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-payload-1024-uplink-streams-4` | MQTT | 5qi-mapped | 4 | 0.175 | 0.200 | 5046272 | 6529680 | 1 | 4 |

## GNSS Snapshot

- CSV samples in `gps/gps_samples.csv`: `1474144`
- NovAtel `BESTPOS` samples: `67017`
- Mean `BESTPOS` latitude: `39.662931577`
- Mean `BESTPOS` longitude: `-75.757349646`
- Mean `BESTPOS` altitude: `34.422` m
- `BESTPOS` latitude range: `39.662918006` to `39.662952481`
- `BESTPOS` longitude range: `-75.757359286` to `-75.757339274`
- Mean horizontal speed across GPS CSV topics: `0.0023` m/s
- ROS bag: `gps/rosbag/`
- ROS bag duration: `6701.762` s
- ROS bag messages: `1809475`

## Verification

- All 14 sender CSV files contain `1000` samples plus header.
- All 14 edge receiver CSV files under `base_station/` contain `1000` samples
  plus header.
- Local and edge latency/load/broker processes were cleaned up after the run.
- `ros2 bag info` succeeded for `gps/rosbag/`.

## Interpretation

- This run is the weak-signal stationary repeat of experiment 06. Compared with
  the earlier good-signal sweep at approximately `39.663556252, -75.757007732`,
  this run was collected at approximately `39.662931577, -75.757349646`.
- The weak-signal location severely limited background load. Good-signal run 3
  measured about `1.99-5.74` Mbps aggregate client load, while this weak-signal
  run measured about `0.00-0.322` Mbps in completed client summaries.
- Despite weak-signal load-generator timeouts, every latency condition completed
  with `1000/1000` accepted probes.
- TCP weak-signal p95 RTT ranged from `661.442` to `933.823` ms across the
  default and `5qi-mapped` load conditions. MQTT weak-signal p95 RTT ranged
  from `637.832` to `773.772` ms across those load conditions.
- Use RTT for analysis because the run is marked `clock_sync_state=unsynced`;
  one-way uplink/downlink columns are not reliable without synchronized clocks.
- Do not claim a causal 5QI/QoS improvement from these data alone. The earlier
  packet-marking check showed both default and `5qi-mapped` probes leaving the
  vehicle host with `tos 0x0`; private-5G core counters are still needed for a
  verified QoS-flow claim.
