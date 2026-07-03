# Experiment 06 Private-5G Load/QoS Repeat Summary

- Run ID: `edge4av-real-20260701-load-qos-run-2`
- Date/time: `2026-07-01`, stationary private-5G run
- Edge server: `10.100.100.6`
- Payload: `1024` B application payload, `1096` B encoded service frame
- Probe count: `1000` per condition
- Probe interval: `200` ms
- GNSS: recorded with NovAtel ROS 2 package under `gps/`
- QoS profile label: `default`; no special 5QI/QoS mapping asserted
- Background load: local vehicle host to edge load server, target `25` Mbps
- Background load packet size: `65536` B

## Latency Results

| Condition | Transport | Load label | Accepted | p50 RTT ms | p95 RTT ms | p99 RTT ms | Mean RTT ms | Max RTT ms |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-tcp-loadqos-payload-1024-idle` | TCP | idle | 1000/1000 | 129.781 | 161.851 | 181.829 | 131.344 | 337.685 |
| `p5g-tcp-loadqos-payload-1024-uplink-25mbps` | TCP | heavy | 1000/1000 | 150.454 | 387.992 | 665.835 | 221.348 | 777.806 |
| `p5g-mqtt-loadqos-payload-1024-idle` | MQTT | idle | 1000/1000 | 35.281 | 52.063 | 67.788 | 36.488 | 179.782 |
| `p5g-mqtt-loadqos-payload-1024-uplink-25mbps` | MQTT | heavy | 1000/1000 | 116.110 | 175.905 | 477.905 | 111.201 | 729.846 |

## Background-Load Results

The load generator was configured for `25` Mbps, but achieved about `2` Mbps in
this run. Report the achieved load as the experimental condition.

| Condition | Side | Target Mbps | Achieved Mbps | Bytes | Packets | Elapsed s |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| TCP loaded | client | 25.0 | 2.265 | 65142784 | 994 | 230.087 |
| TCP loaded | server | 0.0 | 2.004 | 65142784 | 20080 | 260.058 |
| MQTT loaded | client | 25.0 | 2.267 | 65273856 | 996 | 230.305 |
| MQTT loaded | server | 0.0 | 2.007 | 65273856 | 19953 | 260.200 |

## GNSS Snapshot

- Samples in `gps/gps_samples.csv`: `293662`
- Mean latitude: `39.663530839`
- Mean longitude: `-75.756989587`
- Mean altitude: `3.170` m
- Latitude range: `39.663525981` to `39.663536441`
- Longitude range: `-75.757001729` to `-75.756978483`
- ROS bag: `gps/rosbag/`
- ROS bag duration: `1334.987` s
- ROS bag messages: `360442`

## Interpretation

- RTT tail latency increases under measured uplink load for both transports.
- TCP p95 increases from `161.851` ms to `387.992` ms, a `2.40x` increase.
- MQTT p95 increases from `52.063` ms to `175.905` ms, a `3.38x` increase.
- Use RTT for analysis because the run is marked `clock_sync_state=unsynced`;
  one-way uplink/downlink columns are not reliable without synchronized clocks.
- This is a valid stationary no-Mocar private-5G load-stress repeat.
- It is not a complete QoS comparison until a real non-default QoS/5QI policy is
  configured and repeated against the same load condition.
