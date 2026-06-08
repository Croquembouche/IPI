# Real Private-5G Stationary Run Summary

- Run folder: `results/real_5g/20260513_sunny_fintechparking_run_1/`
- Paper run id: `edge4av-real-20260513-sunny-fintechparking-run-1`
- Date/time completed: `2026-05-13T11:52:31-04:00`
- Scenario: stationary vehicle, sunny, FinTech parking location, private-5G sender to base station `10.100.100.6`.
- Probe count: `1000` per condition; interval setting: `200 ms`.
- Clock sync state: `unsynced`; RTT is the paper-grade metric. One-way uplink/downlink columns are retained as raw evidence only and should not be interpreted as physical one-way latency.
- MQTT broker: base-station port `1883` did not have Mosquitto running, so this run used the recorded temporary `minimal_mqtt_broker.py` QoS0 MQTT 3.1.1 broker in the base-station results directory.

## Result Metrics

| Condition | Transport | Frame | Payload bytes | Attempts | Accepted | Success % | Duration s | RTT p50 ms | RTT p95 ms | RTT p99 ms | Uplink p50 ms | Downlink p50 ms |
|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| p5g-tcp-service-payload-0 | tcp | IPI-CooperativeService | 63 | 1000 | 1000 | 100.0 | 319.826 | 119.065 | 137.970 | 185.428 |  | 78402.370 |
| p5g-tcp-service-payload-256 | tcp | IPI-CooperativeService | 334 | 1000 | 1000 | 100.0 | 317.128 | 119.684 | 135.315 | 144.701 |  | 78394.352 |
| p5g-tcp-service-payload-1024 | tcp | IPI-CooperativeService | 1102 | 1000 | 1000 | 100.0 | 321.614 | 120.299 | 140.249 | 150.254 |  | 78387.693 |
| p5g-tcp-service-payload-4096 | tcp | IPI-CooperativeService | 4174 | 1000 | 1000 | 100.0 | 281.138 | 79.837 | 99.806 | 109.276 |  | 78379.102 |
| p5g-tcp-spat-baseline | tcp | SPAT | 14 | 1000 | 1000 | 100.0 | 318.380 | 119.797 | 137.470 | 149.907 |  | 78375.415 |
| p5g-mqtt-service-payload-0 | mqtt | IPI-CooperativeService | 61 | 1000 | 1000 | 100.0 | 225.309 | 23.154 | 36.620 | 43.797 |  | 78342.483 |
| p5g-mqtt-service-payload-256 | mqtt | IPI-CooperativeService | 332 | 1000 | 1000 | 100.0 | 225.309 | 23.884 | 36.148 | 39.611 |  | 78337.711 |
| p5g-mqtt-service-payload-1024 | mqtt | IPI-CooperativeService | 1100 | 1000 | 1000 | 100.0 | 234.365 | 33.817 | 44.487 | 47.836 |  | 78333.122 |
| p5g-mqtt-service-payload-4096 | mqtt | IPI-CooperativeService | 4172 | 1000 | 1000 | 100.0 | 239.243 | 39.724 | 49.474 | 51.422 |  | 78332.863 |
| p5g-mqtt-spat-baseline | mqtt | SPAT | 14 | 1000 | 1000 | 100.0 | 224.904 | 23.356 | 34.114 | 39.656 |  | 78366.207 |

Notes: blank uplink values mean the computed one-way sample was negative after clock-offset effects. Downlink values are numerically present for many rows but are clock-offset dominated because vehicle and base-station clocks are unsynced.

## GPS Evidence

- Latest recorded latitude: `39.663793229`
- Latest recorded longitude: `-75.757002726`
- Latest recorded altitude m: `-2.421217690`
- Latest horizontal speed m/s: `0.001982208`
- Position source: `/novatel/oem7/inspva`
- Speed source: `/novatel/oem7/inspva`
- GPS artifacts: `gps/gps_samples.csv`, `gps/gps_latest.csv`, `gps/rosbag/`, `gps/novatel_driver.log`, `gps/rosbag_record.log`.
- GPS caveat: GPS capture produced `532` CSV rows during driver startup at the stationary location; the latest sample is used as the run location evidence, but the GPS files are not a continuous trace through the full network trial window.

## Build And Connectivity Evidence

- Local configure/build/test logs: `local_cmake_configure.log`, `local_build.log`, `local_ctest.log`.
- Base-station build log: `base_build_and_ctest_retry.log`.
- Base-station CTest rerun from build directory: `base_ctest_from_build_dir.log`.
- Base-station receiver logs copied under: `base_station/`.
- Environment evidence: `environment.md`, `ssh_remote_check.log`, `ping_base_station.log`.

- Local CTest: `100% tests passed` (`6/6`).
- Base-station CTest: `100% tests passed` (`6/6`).

Ping summary:
```text

--- 10.100.100.6 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2002ms
rtt min/avg/max/mdev = 14.754/15.342/16.077/0.550 ms
```

## Commands

- Build/sync commands are recorded in `commands/build_commands.txt`.
- Exact trial launch commands are recorded in `commands/network_trial_commands.txt`.
- The local trial runner used for this run is preserved as `run_network_trials.sh`.

## Failures Or Caveats

```text
2026-05-13T11:03:51-04:00 first trial launch aborted: remote receiver process started but SSH launch did not detach; no sender rows were written.
```
- Initial remote build failed because a local `cpp/build` CMake cache was copied to the base station; fixed by deleting remote `cpp/build`, resyncing with `--exclude build/`, and rebuilding successfully.
- The first remote CTest command using `ctest --test-dir cpp/build` on that base-station CTest version reported `No tests were found`; rerunning from `/home/d1/Documents/Github/IPI/cpp/build` passed `6/6`.
- Base-station MQTT readiness before the run:
```text
/usr/bin/nc
mqtt_1883_local_closed
```
- Temporary SSH key was removed from the base station after log collection; remote receiver and MQTT broker processes were cleaned up.

## Raw Files

- `p5g-tcp-service-payload-0`: sender `p5g-tcp-service-payload-0_sender.csv`, receiver `base_station/p5g-tcp-service-payload-0_receiver.csv`, stderr files beside each CSV.
- `p5g-tcp-service-payload-256`: sender `p5g-tcp-service-payload-256_sender.csv`, receiver `base_station/p5g-tcp-service-payload-256_receiver.csv`, stderr files beside each CSV.
- `p5g-tcp-service-payload-1024`: sender `p5g-tcp-service-payload-1024_sender.csv`, receiver `base_station/p5g-tcp-service-payload-1024_receiver.csv`, stderr files beside each CSV.
- `p5g-tcp-service-payload-4096`: sender `p5g-tcp-service-payload-4096_sender.csv`, receiver `base_station/p5g-tcp-service-payload-4096_receiver.csv`, stderr files beside each CSV.
- `p5g-tcp-spat-baseline`: sender `p5g-tcp-spat-baseline_sender.csv`, receiver `base_station/p5g-tcp-spat-baseline_receiver.csv`, stderr files beside each CSV.
- `p5g-mqtt-service-payload-0`: sender `p5g-mqtt-service-payload-0_sender.csv`, receiver `base_station/p5g-mqtt-service-payload-0_receiver.csv`, stderr files beside each CSV.
- `p5g-mqtt-service-payload-256`: sender `p5g-mqtt-service-payload-256_sender.csv`, receiver `base_station/p5g-mqtt-service-payload-256_receiver.csv`, stderr files beside each CSV.
- `p5g-mqtt-service-payload-1024`: sender `p5g-mqtt-service-payload-1024_sender.csv`, receiver `base_station/p5g-mqtt-service-payload-1024_receiver.csv`, stderr files beside each CSV.
- `p5g-mqtt-service-payload-4096`: sender `p5g-mqtt-service-payload-4096_sender.csv`, receiver `base_station/p5g-mqtt-service-payload-4096_receiver.csv`, stderr files beside each CSV.
- `p5g-mqtt-spat-baseline`: sender `p5g-mqtt-spat-baseline_sender.csv`, receiver `base_station/p5g-mqtt-spat-baseline_receiver.csv`, stderr files beside each CSV.

## Extended Large Payload Sweep

- Capped large-payload TCP results are summarized in `large_payload_summary.md`. The sweep was stopped after the completed `2097152` byte condition per user request; larger TCP sizes and large MQTT payloads were not run.

## MQTT Payload Latency Sweep

- MQTT payload latency results from `0` through `2097152` bytes are summarized in `mqtt_latency_summary.md`. RTT excludes sender-side packaging time.
