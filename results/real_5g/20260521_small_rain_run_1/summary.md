# Real Private-5G TCP/MQTT Latency Summary

- Run folder: `results/real_5g/20260521_small_rain_run_1`
- Run ID: `edge4av-real-20260521-small-rain-run-1`
- Weather/location note: small rain, stationary vehicle at recorded GPS position
- Payload sequence: TCP then MQTT for each requested payload from 0 bytes through 1 MiB
- Count per condition: 1000 messages, interval 200 ms
- Clock sync state: `unsynced`; RTT is the reliable metric. One-way uplink/downlink columns are retained as raw approximate evidence only.
- Timing note: sender send time is recorded after request encoding and immediately before TCP send or MQTT publish, so sender-side packaging time is excluded from RTT.
- Base station logs copied to: `base_station/`
- Exact commands: `commands/alternating_tcp_mqtt_commands.txt`
- Spreadsheet exports: `sender_combined_for_excel.csv`, `sender_metrics.csv`

## GPS Evidence

- Latest GPS sample: latitude 39.688992729, longitude -75.816216868, altitude_m -6426.114583964, horizontal_speed_mps 0.000000000, sample_topic /novatel/oem7/odom, position_source /novatel/oem7/fix, speed_source /novatel/oem7/odom.
- Raw GPS files: `gps_20260521/gps/gps_samples.csv`, `gps_20260521/gps/gps_latest.csv`, `gps_20260521/gps/rosbag/`
- Note: the latest odometry-derived altitude appears invalid/unstable in this sample, but latitude/longitude and zero speed were recorded.

## Metrics

| Transport | Requested payload bytes | Attempts | Accepted | Success rate % | RTT p50 ms | RTT p95 ms | RTT p99 ms | Uplink nonneg n | Uplink p50 ms | Uplink p95 ms | Uplink p99 ms | Downlink nonneg n | Downlink p50 ms | Downlink p95 ms | Downlink p99 ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| mqtt | 0 | 1000 | 1000 | 100.00 | 22.049 | 40.312 | 47.476 | 0 |  |  |  | 1000 | 76188.916 | 76193.755 | 76196.479 |
| tcp | 0 | 1000 | 1000 | 100.00 | 119.864 | 142.443 | 159.690 | 0 |  |  |  | 1000 | 76219.269 | 76230.300 | 76232.088 |
| mqtt | 256 | 1000 | 1000 | 100.00 | 29.979 | 50.004 | 67.976 | 0 |  |  |  | 1000 | 76186.327 | 76191.844 | 76195.031 |
| tcp | 256 | 1000 | 1000 | 100.00 | 120.613 | 159.691 | 169.879 | 0 |  |  |  | 1000 | 76217.318 | 76228.221 | 76233.168 |
| mqtt | 1024 | 1000 | 1000 | 100.00 | 31.726 | 51.591 | 69.744 | 0 |  |  |  | 1000 | 76184.299 | 76190.244 | 76192.479 |
| tcp | 1024 | 1000 | 1000 | 100.00 | 129.668 | 170.991 | 203.799 | 0 |  |  |  | 1000 | 76213.204 | 76223.750 | 76227.294 |
| mqtt | 4096 | 1000 | 1000 | 100.00 | 51.664 | 79.659 | 103.491 | 0 |  |  |  | 1000 | 76187.120 | 76193.441 | 76196.447 |
| tcp | 4096 | 1000 | 1000 | 100.00 | 80.227 | 136.305 | 175.628 | 0 |  |  |  | 1000 | 76212.457 | 76218.733 | 76225.378 |
| mqtt | 8192 | 1000 | 1000 | 100.00 | 63.772 | 87.893 | 109.712 | 0 |  |  |  | 1000 | 76183.460 | 76190.150 | 76192.235 |
| tcp | 8192 | 1000 | 1000 | 100.00 | 89.947 | 169.970 | 210.028 | 0 |  |  |  | 1000 | 76209.524 | 76217.019 | 76226.508 |
| mqtt | 16384 | 1000 | 1000 | 100.00 | 69.703 | 129.629 | 181.537 | 0 |  |  |  | 1000 | 76179.949 | 76186.243 | 76189.375 |
| tcp | 16384 | 1000 | 1000 | 100.00 | 119.686 | 193.728 | 224.563 | 0 |  |  |  | 1000 | 76206.163 | 76214.539 | 76223.702 |
| mqtt | 32768 | 1000 | 1000 | 100.00 | 63.516 | 77.485 | 110.172 | 0 |  |  |  | 1000 | 76178.378 | 76183.763 | 76186.358 |
| tcp | 32768 | 1000 | 1000 | 100.00 | 90.185 | 107.931 | 119.823 | 0 |  |  |  | 1000 | 76204.506 | 76211.363 | 76215.557 |
| mqtt | 65536 | 1000 | 1000 | 100.00 | 159.382 | 211.600 | 289.810 | 0 |  |  |  | 1000 | 76175.416 | 76179.558 | 76183.996 |
| tcp | 65536 | 1000 | 1000 | 100.00 | 169.910 | 231.526 | 265.590 | 0 |  |  |  | 1000 | 76202.035 | 76208.152 | 76213.633 |
| mqtt | 131072 | 1000 | 1000 | 100.00 | 251.810 | 333.252 | 471.766 | 0 |  |  |  | 1000 | 76173.527 | 76177.898 | 76182.055 |
| tcp | 131072 | 1000 | 1000 | 100.00 | 283.971 | 350.892 | 638.668 | 0 |  |  |  | 1000 | 76197.326 | 76203.535 | 76209.808 |
| mqtt | 262144 | 1000 | 1000 | 100.00 | 728.377 | 1156.083 | 1412.295 | 0 |  |  |  | 1000 | 76174.777 | 76180.765 | 76183.550 |
| tcp | 262144 | 1000 | 1000 | 100.00 | 579.062 | 784.454 | 935.409 | 0 |  |  |  | 1000 | 76201.452 | 76207.845 | 76212.613 |
| mqtt | 524288 | 1000 | 1000 | 100.00 | 866.372 | 1085.127 | 1195.337 | 0 |  |  |  | 1000 | 76149.374 | 76155.933 | 76158.243 |
| tcp | 524288 | 1000 | 1000 | 100.00 | 1485.407 | 3598.768 | 4417.241 | 0 |  |  |  | 1000 | 76183.145 | 76198.001 | 76203.363 |
| mqtt | 1048576 | 1000 | 1000 | 100.00 | 2042.731 | 3018.356 | 3460.328 | 0 |  |  |  | 1000 | 76137.562 | 76180.713 | 76185.956 |
| tcp | 1048576 | 1000 | 1000 | 100.00 | 1135.347 | 1255.798 | 1575.411 | 0 |  |  |  | 1000 | 76167.082 | 76179.601 | 76183.691 |

## Completion And Failures

- Completed conditions: 24/24 sender conditions had 1000 attempts and 1000 accepted replies.
- Sender `.err` files contain the sender summary lines; no non-summary sender failure output was observed.
- Remote receiver and MQTT broker processes were stopped after the run, and no local experiment processes remained after cleanup.

## Run Metadata

# Alternating TCP/MQTT Metadata

- Start time: 2026-05-21T13:37:41-04:00
- Weather: small rain
- Payload arguments: 0 256 1024 4096 8192 16384 32768 65536 131072 262144 524288 1048576
- Count per condition: 1000
- Interval ms: 200
- Order: TCP then MQTT for each payload
- Base station: 10.100.100.6
- TCP port: 36666
- MQTT port: 1883
- Mobility: stationary
- Clock sync state: unsynced
- Timestamp note: sender send time is set after request encoding and immediately before TCP send or MQTT publish, so sender-side packaging time is excluded from RTT.

- End time: 2026-05-21T17:39:35-04:00

## Evidence Files

- Environment/build: `environment.md`, `local_build.log`, `base_build.log`, `ssh_remote_check.log`
- Sender CSV/error files: `p5g-{tcp,mqtt}-service-payload-*_sender.csv`, `p5g-{tcp,mqtt}-service-payload-*_sender.err`
- Base-station receiver/broker logs: `base_station/`
- Runner script: `run_alternating_tcp_mqtt_0_to_1mb.sh`

