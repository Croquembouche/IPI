# Experiment 06 Private-5G Load Sweep Summary

- Run ID: `edge4av-real-20260701-load-qos-run-3`
- Date/time: `2026-07-01`, stationary private-5G run
- Edge server: `10.100.100.6`
- Payload: `1024` B application payload, `1096` B encoded service frame
- Probe count: `1000` per condition
- Probe interval: `200` ms
- GNSS: recorded with NovAtel ROS 2 package under `gps/`
- QoS profile label: `default`; no special 5QI/QoS mapping asserted
- Background load: local vehicle host to edge load server, target `25` Mbps per
  stream, `65536` B load packets
- Stream counts: `1`, `2`, `4`; each stream uses a separate edge TCP port

## Latency Results

| Condition | Transport | Streams | Accepted | p50 RTT ms | p95 RTT ms | p99 RTT ms | Mean RTT ms | Max RTT ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-tcp-loadqos-payload-1024-uplink-streams-1` | TCP | 1 | 1000/1000 | 139.252 | 391.450 | 412.029 | 216.494 | 1563.644 |
| `p5g-tcp-loadqos-payload-1024-uplink-streams-2` | TCP | 2 | 1000/1000 | 133.553 | 401.730 | 679.990 | 222.344 | 807.035 |
| `p5g-tcp-loadqos-payload-1024-uplink-streams-4` | TCP | 4 | 1000/1000 | 132.537 | 409.887 | 743.631 | 227.540 | 1151.857 |
| `p5g-mqtt-loadqos-payload-1024-uplink-streams-1` | MQTT | 1 | 1000/1000 | 121.701 | 169.640 | 465.976 | 113.525 | 809.875 |
| `p5g-mqtt-loadqos-payload-1024-uplink-streams-2` | MQTT | 2 | 1000/1000 | 106.988 | 467.762 | 858.230 | 129.456 | 3144.385 |
| `p5g-mqtt-loadqos-payload-1024-uplink-streams-4` | MQTT | 4 | 1000/1000 | 145.692 | 181.671 | 533.936 | 123.095 | 1617.805 |

## Measured Background Load

Report measured aggregate load, not the nominal `25` Mbps per-stream target.

| Condition | Streams | Client Aggregate Mbps | Server Aggregate Mbps | Client Bytes | Server Bytes |
| --- | ---: | ---: | ---: | ---: | ---: |
| TCP stream sweep 1 | 1 | 4.371 | 3.862 | 125698048 | 125698048 |
| TCP stream sweep 2 | 2 | 4.679 | 4.139 | 134610944 | 134610944 |
| TCP stream sweep 4 | 4 | 5.602 | 4.955 | 161153024 | 161153024 |
| MQTT stream sweep 1 | 1 | 5.743 | 5.083 | 165281792 | 165281792 |
| MQTT stream sweep 2 | 2 | 1.990 | 1.759 | 57212928 | 57212928 |
| MQTT stream sweep 4 | 4 | 4.734 | 4.186 | 136183808 | 136183808 |

## GNSS Snapshot

- Samples in `gps/gps_samples.csv`: `507288`
- Mean latitude: `39.663556252`
- Mean longitude: `-75.757007732`
- Mean altitude: `4.148` m
- Latitude range: `39.663548127` to `39.663561459`
- Longitude range: `-75.757012356` to `-75.756999754`
- ROS bag: `gps/rosbag/`
- ROS bag duration: `2305.959` s
- ROS bag messages: `622609`

## Interpretation

- This run adds the missing load-sweep evidence for experiment 06.
- The TCP p95 is consistently around `391-410` ms across the measured
  `4.37-5.60` Mbps client-side aggregate load range.
- MQTT has higher variability: p95 ranges from `169.640` ms to `467.762` ms
  across measured `1.99-5.74` Mbps client-side aggregate load.
- The 2-stream and 4-stream MQTT achieved loads were not monotonic, so figures
  should plot measured Mbps rather than stream count.
- Use RTT for analysis because the run is marked `clock_sync_state=unsynced`;
  one-way uplink/downlink columns are not reliable without synchronized clocks.
- This is still a default-load stress sweep, not a QoS/5QI comparison.
