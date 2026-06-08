# Real Private-5G TCP/MQTT Latency Summary

- Run folder: `results/real_5g/20260522_cloudy_run_1`
- Run ID: `edge4av-real-20260522-cloudy-run-1`
- Weather/location note: cloudy, stationary vehicle at recorded GPS position
- Payload sequence: TCP then MQTT for each requested payload from 0 bytes through 1 MiB
- Count per condition: 1000 messages, interval 200 ms
- Clock sync state: `unsynced`; RTT is the reliable metric. One-way uplink/downlink columns are retained as raw approximate evidence only.
- Timing note: sender send time is recorded after request encoding and immediately before TCP send or MQTT publish, so sender-side packaging time is excluded from RTT.
- Base station logs copied to: `base_station/`
- Exact commands: `commands/alternating_tcp_mqtt_commands.txt`
- Spreadsheet exports: `sender_combined_for_excel.csv`, `sender_metrics.csv`

## GPS Evidence

- Latest GPS sample: latitude 39.665712407, longitude -75.756101036, altitude_m 1.087019542, horizontal_speed_mps 0.002803605, sample_topic /novatel/oem7/odom, position_source /novatel/oem7/fix, speed_source /novatel/oem7/odom.
- Raw GPS files: `gps_20260522/gps/gps_samples.csv`, `gps_20260522/gps/gps_latest.csv`, `gps_20260522/gps/rosbag/`

## Metrics

| Transport | Requested payload bytes | Attempts | Accepted | Success rate % | RTT p50 ms | RTT p95 ms | RTT p99 ms | Uplink nonneg n | Uplink p50 ms | Uplink p95 ms | Uplink p99 ms | Downlink nonneg n | Downlink p50 ms | Downlink p95 ms | Downlink p99 ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| mqtt | 0 | 1000 | 1000 | 100.00 | 27.636 | 39.604 | 43.817 | 0 |  |  |  | 1000 | 75929.342 | 75937.231 | 75939.835 |
| tcp | 0 | 1000 | 1000 | 100.00 | 119.733 | 148.108 | 153.765 | 0 |  |  |  | 1000 | 75958.529 | 75969.295 | 75976.017 |
| mqtt | 256 | 1000 | 1000 | 100.00 | 25.764 | 37.980 | 43.725 | 0 |  |  |  | 1000 | 75922.944 | 75930.727 | 75932.414 |
| tcp | 256 | 1000 | 1000 | 100.00 | 119.781 | 146.860 | 151.286 | 0 |  |  |  | 1000 | 75953.979 | 75965.088 | 75972.180 |
| mqtt | 1024 | 1000 | 1000 | 100.00 | 33.995 | 47.302 | 50.216 | 0 |  |  |  | 1000 | 75919.868 | 75928.697 | 75930.434 |
| tcp | 1024 | 1000 | 1000 | 100.00 | 128.209 | 155.301 | 163.190 | 0 |  |  |  | 1000 | 75950.609 | 75961.939 | 75968.419 |
| mqtt | 4096 | 1000 | 1000 | 100.00 | 39.776 | 47.683 | 51.751 | 0 |  |  |  | 1000 | 75919.941 | 75926.702 | 75928.237 |
| tcp | 4096 | 1000 | 1000 | 100.00 | 79.715 | 119.639 | 122.151 | 0 |  |  |  | 1000 | 75944.372 | 75957.085 | 75964.676 |
| mqtt | 8192 | 1000 | 1000 | 100.00 | 39.953 | 49.998 | 55.677 | 0 |  |  |  | 1000 | 75913.626 | 75922.642 | 75925.506 |
| tcp | 8192 | 1000 | 1000 | 100.00 | 89.847 | 119.775 | 136.161 | 0 |  |  |  | 1000 | 75942.873 | 75953.984 | 75962.252 |
| mqtt | 16384 | 1000 | 1000 | 100.00 | 49.767 | 63.737 | 73.856 | 0 |  |  |  | 1000 | 75912.514 | 75920.933 | 75922.876 |
| tcp | 16384 | 1000 | 1000 | 100.00 | 79.845 | 101.384 | 116.620 | 0 |  |  |  | 1000 | 75938.747 | 75950.320 | 75958.039 |
| mqtt | 32768 | 1000 | 1000 | 100.00 | 69.566 | 83.185 | 89.726 | 0 |  |  |  | 1000 | 75912.350 | 75919.887 | 75921.831 |
| tcp | 32768 | 1000 | 1000 | 100.00 | 92.036 | 115.581 | 120.636 | 0 |  |  |  | 1000 | 75936.780 | 75948.244 | 75956.360 |
| mqtt | 65536 | 1000 | 1000 | 100.00 | 107.668 | 129.445 | 139.410 | 0 |  |  |  | 1000 | 75912.361 | 75919.552 | 75920.761 |
| tcp | 65536 | 1000 | 1000 | 100.00 | 129.454 | 151.302 | 160.495 | 0 |  |  |  | 1000 | 75937.688 | 75954.295 | 75956.279 |
| mqtt | 131072 | 1000 | 1000 | 100.00 | 178.898 | 202.785 | 214.441 | 0 |  |  |  | 1000 | 75909.628 | 75915.373 | 75916.912 |
| tcp | 131072 | 1000 | 1000 | 100.00 | 200.595 | 229.267 | 300.198 | 0 |  |  |  | 1000 | 75934.873 | 75951.530 | 75953.586 |
| mqtt | 262144 | 1000 | 1000 | 100.00 | 329.057 | 378.398 | 618.777 | 0 |  |  |  | 1000 | 75912.737 | 75916.414 | 75917.364 |
| tcp | 262144 | 1000 | 1000 | 100.00 | 360.574 | 420.511 | 469.447 | 0 |  |  |  | 1000 | 75937.776 | 75951.037 | 75952.638 |
| mqtt | 524288 | 1000 | 1000 | 100.00 | 647.755 | 761.387 | 825.305 | 0 |  |  |  | 1000 | 75905.925 | 75910.019 | 75912.605 |
| tcp | 524288 | 1000 | 1000 | 100.00 | 655.053 | 982.803 | 1135.046 | 0 |  |  |  | 1000 | 75931.597 | 75944.349 | 75952.460 |
| mqtt | 1048576 | 1000 | 1000 | 100.00 | 1223.351 | 1406.789 | 1576.948 | 0 |  |  |  | 1000 | 75967.187 | 75983.944 | 75986.006 |
| tcp | 1048576 | 1000 | 1000 | 100.00 | 1236.150 | 1465.084 | 1603.686 | 0 |  |  |  | 1000 | 75929.087 | 75946.257 | 75955.573 |

## Completion And Failures

- Completed conditions: 24/24 sender conditions had 1000 attempts and 1000 accepted replies.
- Sender `.err` files contain the sender summary lines; no non-summary sender failure output was observed.
- Remote receiver and MQTT broker processes were stopped after the run, and no local experiment processes remained after cleanup.

## Run Metadata

# Alternating TCP/MQTT Metadata

- Start time: 2026-05-22T13:55:37-04:00
- Weather: cloudy
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

- End time: 2026-05-22T17:04:05-04:00

## Evidence Files

- Environment/build: `environment.md`, `local_cmake.log`, `local_build.log`, `local_ctest.log`, `base_build_test.log`, `ssh_remote_check.log`
- Sender CSV/error files: `p5g-{tcp,mqtt}-service-payload-*_sender.csv`, `p5g-{tcp,mqtt}-service-payload-*_sender.err`
- Base-station receiver/broker logs: `base_station/`
- Runner script: `run_alternating_tcp_mqtt_0_to_1mb.sh`
