# Experiment 06 Private-5G QoS-Labeled Sweep Summary

- Run ID: `edge4av-real-20260701-load-qos-run-4`
- Date/time: `2026-07-01`, stationary private-5G run
- Edge server: `10.100.100.6`
- Payload: `1024` B application payload, `1096` B encoded service frame
- Probe count: `1000` per condition
- Probe interval: `200` ms
- GNSS: recorded with NovAtel ROS 2 package under `gps/`
- QoS profile label: `5qi-mapped`
- Background load: local vehicle host to edge load server, target `25` Mbps per
  stream, `65536` B load packets
- Stream counts: `1`, `2`, `4`; each stream uses a separate edge TCP port

Important caveat: this run sets `qos_profile=5qi-mapped` in the experiment
metadata. The application code does not set DSCP/TOS and this script does not
verify private-5G core-side 5QI bearer enforcement. Treat this as a QoS-labeled
control run unless network-side QoS mapping is independently confirmed.

## Latency Results

| Condition | Transport | Streams | QoS | Accepted | p50 RTT ms | p95 RTT ms | p99 RTT ms | Mean RTT ms | Max RTT ms |
| --- | --- | ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `p5g-tcp-qos-5qi-mapped-payload-1024-uplink-streams-1` | TCP | 1 | `5qi-mapped` | 1000/1000 | 223.017 | 430.775 | 732.779 | 255.012 | 2322.005 |
| `p5g-tcp-qos-5qi-mapped-payload-1024-uplink-streams-2` | TCP | 2 | `5qi-mapped` | 1000/1000 | 153.225 | 679.946 | 948.013 | 253.146 | 1528.924 |
| `p5g-tcp-qos-5qi-mapped-payload-1024-uplink-streams-4` | TCP | 4 | `5qi-mapped` | 1000/1000 | 229.860 | 771.833 | 1182.417 | 308.470 | 2673.111 |
| `p5g-mqtt-qos-5qi-mapped-payload-1024-uplink-streams-1` | MQTT | 1 | `5qi-mapped` | 1000/1000 | 107.978 | 376.994 | 516.019 | 124.041 | 2502.188 |
| `p5g-mqtt-qos-5qi-mapped-payload-1024-uplink-streams-2` | MQTT | 2 | `5qi-mapped` | 1000/1000 | 127.867 | 487.030 | 578.021 | 154.797 | 4434.092 |
| `p5g-mqtt-qos-5qi-mapped-payload-1024-uplink-streams-4` | MQTT | 4 | `5qi-mapped` | 1000/1000 | 122.418 | 529.321 | 806.177 | 154.035 | 2987.557 |

## Measured Background Load

Report measured aggregate load, not the nominal `25` Mbps per-stream target.
Measured load was lower than the default-load sweep in
`20260701_load_qos_run_3`.

| Condition | Streams | Client Aggregate Mbps | Server Aggregate Mbps | Client Bytes | Server Bytes |
| --- | ---: | ---: | ---: | ---: | ---: |
| TCP 5QI-labeled stream sweep 1 | 1 | 0.993 | 0.878 | 28573696 | 28573696 |
| TCP 5QI-labeled stream sweep 2 | 2 | 0.807 | 0.716 | 23265280 | 23265280 |
| TCP 5QI-labeled stream sweep 4 | 4 | 0.665 | 0.594 | 19333120 | 19333120 |
| MQTT 5QI-labeled stream sweep 1 | 1 | 0.850 | 0.751 | 24444928 | 24444928 |
| MQTT 5QI-labeled stream sweep 2 | 2 | 0.997 | 0.884 | 28770304 | 28770304 |
| MQTT 5QI-labeled stream sweep 4 | 4 | 0.977 | 0.870 | 28311552 | 28311552 |

## GNSS Snapshot

- Samples in `gps/gps_samples.csv`: `562894`
- Mean latitude: `39.663561556`
- Mean longitude: `-75.757004369`
- Mean altitude: `5.054` m
- Latitude range: `39.663550059` to `39.663567640`
- Longitude range: `-75.757013097` to `-75.756992813`
- ROS bag: `gps/rosbag/`
- ROS bag duration: `2561.332` s
- ROS bag messages: `691535`

## Interpretation

- This run completes the requested non-default QoS-profile collection leg.
- All six conditions completed with 1000/1000 accepted probes.
- Because measured load differs substantially from the default run and 5QI
  enforcement is not independently verified, do not claim that QoS improved or
  degraded latency based on this run alone.
- Use this run as `qos_profile=5qi-mapped` evidence and compare only with
  measured-load normalization.
- Use RTT for analysis because the run is marked `clock_sync_state=unsynced`;
  one-way uplink/downlink columns are not reliable without synchronized clocks.
