# Experiment 06 Private-5G Load/QoS Phase A Summary

- Run ID: `edge4av-real-20260701-load-qos-run-1`
- Date/time: `2026-07-01`, stationary private-5G run
- Edge server: `10.100.100.6`
- Payload: `1024` B application payload, `1096` B encoded service frame
- Probe count: `1000` per condition
- Probe interval: `200` ms
- GNSS: recorded with NovAtel ROS 2 package under `gps/`
- QoS profile label: `default`; no special 5QI/QoS mapping asserted
- Background load: local vehicle host to edge load server, target `25` Mbps

## Latency Results

| Condition | Transport | Load label | Accepted | p50 RTT ms | p95 RTT ms | p99 RTT ms | Mean RTT ms | Max RTT ms |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-tcp-loadqos-payload-1024-idle` | TCP | idle | 1000/1000 | 129.882 | 193.832 | 250.118 | 138.342 | 537.025 |
| `p5g-tcp-loadqos-payload-1024-uplink-25mbps` | TCP | heavy | 1000/1000 | 140.960 | 419.831 | 743.656 | 226.193 | 1812.879 |
| `p5g-mqtt-loadqos-payload-1024-idle` | MQTT | idle | 1000/1000 | 34.774 | 60.233 | 77.925 | 37.475 | 276.849 |
| `p5g-mqtt-loadqos-payload-1024-uplink-25mbps` | MQTT | heavy | 1000/1000 | 119.391 | 173.777 | 456.887 | 109.166 | 823.833 |

## Background-Load Results

The load generator was configured for `25` Mbps, but achieved lower throughput
on this run. Report the achieved load, not only the target.

| Condition | Side | Target Mbps | Achieved Mbps | Bytes | Packets | Elapsed s |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| TCP loaded | client | 25.0 | 1.320 | 37959600 | 31633 | 230.001 |
| TCP loaded | server | 0.0 | 1.167 | 37959600 | 42770 | 260.213 |
| MQTT loaded | client | 25.0 | 1.624 | 46687200 | 38906 | 230.003 |
| MQTT loaded | server | 0.0 | 1.434 | 46687200 | 50595 | 260.369 |

## GNSS Snapshot

- Samples in `gps/gps_samples.csv`: `318590`
- Mean latitude: `39.663535100`
- Mean longitude: `-75.756994776`
- Mean altitude: `5.037` m
- Latitude range: `39.663524302` to `39.663541022`
- Longitude range: `-75.757001822` to `-75.756978258`
- Latest snapshot: `gps/gps_latest.csv`
- ROS bag: `gps/rosbag/`

## Notes

- Use RTT for analysis because the run is marked `clock_sync_state=unsynced`;
  one-way uplink/downlink columns are not reliable without synchronized clocks.
- This is a valid stationary no-Mocar private-5G load-stress run.
- It is not a complete QoS comparison until a real non-default QoS/5QI policy is
  configured and repeated against the same load condition.
