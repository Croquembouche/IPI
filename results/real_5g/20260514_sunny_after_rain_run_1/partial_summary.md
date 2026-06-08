# Partial Real Private-5G Run Summary

- Updated: `2026-05-14T17:40:18-04:00`
- Run folder: `results/real_5g/20260514_sunny_after_rain_run_1/`
- Scenario: stationary vehicle at new location, weather `sunny after rain`, private-5G sender to base station `10.100.100.6`.
- Probe target: `1000` complete probes per condition; interval setting: `200 ms`.
- Stop status: user requested stop during MQTT `1048576` byte condition, so MQTT `1048576` is partial and MQTT `2097152` was not run.
- Timestamp note: RTT excludes sender-side message packaging time; send time is patched after encoding and immediately before TCP send or MQTT publish.
- Clock sync state: `unsynced`; RTT is the reliable metric. One-way columns are raw evidence only.

## Metrics

| Condition | Transport | Encoded frame bytes | Complete rows | Accepted | Malformed rows | Success % | Duration s | RTT p50 ms | RTT p95 ms | RTT p99 ms | Server p50 ms | Status |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| p5g-tcp-service-payload-0 | tcp | 63 | 1000 | 1000 | 0 | 100.0 | 326.722 | 117.256 | 139.760 | 545.253 | 0.044 | complete |
| p5g-tcp-service-payload-256 | tcp | 334 | 1000 | 1000 | 0 | 100.0 | 322.241 | 118.648 | 138.403 | 368.647 | 0.050 | complete |
| p5g-tcp-service-payload-1024 | tcp | 1102 | 1000 | 1000 | 0 | 100.0 | 334.392 | 124.150 | 155.687 | 418.214 | 0.050 | complete |
| p5g-tcp-service-payload-4096 | tcp | 4174 | 1000 | 1000 | 0 | 100.0 | 288.525 | 79.695 | 119.776 | 291.907 | 0.052 | complete |
| p5g-tcp-service-payload-8192 | tcp | 8270 | 1000 | 1000 | 0 | 100.0 | 299.795 | 86.816 | 151.455 | 203.811 | 0.052 | complete |
| p5g-tcp-service-payload-16384 | tcp | 16462 | 1000 | 1000 | 0 | 100.0 | 325.297 | 110.296 | 171.478 | 374.326 | 0.054 | complete |
| p5g-tcp-service-payload-32768 | tcp | 32846 | 1000 | 1000 | 0 | 100.0 | 388.291 | 164.745 | 231.800 | 411.145 | 0.060 | complete |
| p5g-tcp-service-payload-65536 | tcp | 65614 | 1000 | 1000 | 0 | 100.0 | 543.197 | 289.651 | 530.997 | 954.321 | 0.066 | complete |
| p5g-tcp-service-payload-131072 | tcp | 131150 | 1000 | 1000 | 0 | 100.0 | 717.465 | 481.047 | 686.972 | 980.054 | 0.351 | complete |
| p5g-tcp-service-payload-262144 | tcp | 262222 | 1000 | 1000 | 0 | 100.0 | 1183.056 | 931.825 | 1297.066 | 1717.193 | 0.585 | complete |
| p5g-tcp-service-payload-524288 | tcp | 524366 | 1000 | 1000 | 0 | 100.0 | 2418.895 | 2131.413 | 2848.504 | 3103.800 | 1.111 | complete |
| p5g-tcp-service-payload-1048576 | tcp | 1048654 | 1000 | 1000 | 0 | 100.0 | 3793.883 | 3430.802 | 4872.325 | 5506.342 | 0.247 | complete |
| p5g-tcp-service-payload-2097152 | tcp | 2097230 | 1000 | 1000 | 0 | 100.0 | 8841.081 | 8322.277 | 10767.201 | 13068.545 | 0.455 | complete |
| p5g-mqtt-service-payload-0 | mqtt | 61 | 1000 | 1000 | 0 | 100.0 | 236.824 | 27.365 | 70.067 | 300.194 | 0.043 | complete |
| p5g-mqtt-service-payload-256 | mqtt | 332 | 1000 | 1000 | 0 | 100.0 | 236.631 | 27.372 | 53.735 | 294.489 | 0.050 | complete |
| p5g-mqtt-service-payload-1024 | mqtt | 1100 | 1000 | 1000 | 0 | 100.0 | 244.557 | 31.358 | 70.439 | 313.385 | 0.050 | complete |
| p5g-mqtt-service-payload-4096 | mqtt | 4172 | 1000 | 1000 | 0 | 100.0 | 264.471 | 49.236 | 124.421 | 336.256 | 0.051 | complete |
| p5g-mqtt-service-payload-8192 | mqtt | 8268 | 1000 | 1000 | 0 | 100.0 | 289.109 | 61.785 | 297.547 | 546.060 | 0.051 | complete |
| p5g-mqtt-service-payload-16384 | mqtt | 16460 | 1000 | 1000 | 0 | 100.0 | 325.810 | 99.576 | 318.168 | 502.199 | 0.053 | complete |
| p5g-mqtt-service-payload-32768 | mqtt | 32844 | 1000 | 1000 | 0 | 100.0 | 394.078 | 157.464 | 416.017 | 666.294 | 0.056 | complete |
| p5g-mqtt-service-payload-65536 | mqtt | 65612 | 1000 | 1000 | 0 | 100.0 | 561.239 | 313.360 | 598.105 | 897.842 | 0.064 | complete |
| p5g-mqtt-service-payload-131072 | mqtt | 131148 | 1000 | 1000 | 0 | 100.0 | 849.933 | 591.650 | 913.242 | 1135.926 | 0.072 | complete |
| p5g-mqtt-service-payload-262144 | mqtt | 262220 | 1000 | 1000 | 0 | 100.0 | 1077.626 | 847.432 | 1154.463 | 1627.090 | 0.089 | complete |
| p5g-mqtt-service-payload-524288 | mqtt | 524364 | 1000 | 1000 | 0 | 100.0 | 2149.412 | 1827.160 | 2672.859 | 4161.275 | 0.126 | complete |
| p5g-mqtt-service-payload-1048576 | mqtt | 1048652 | 98 | 98 | 1 | 100.0 | 397.077 | 3752.546 | 5013.245 | 5328.172 | 0.366 | partial/interrupted |
| p5g-mqtt-service-payload-2097152 | mqtt |  | 0 | 0 | 0 | 0.0 |  |  |  |  |  | not run: stopped before condition |

## GPS Evidence

- Latitude: `39.666697932`
- Longitude: `-75.757000455`
- Altitude m: `33.075767262`
- Horizontal speed m/s: `0.003568845`
- Position source: `/novatel/oem7/bestpos`
- Speed source: `/novatel/oem7/odom`
- GPS artifacts: `gps/gps_samples.csv`, `gps/gps_latest.csv`, `gps/rosbag/`, `gps/novatel_driver.log`, `gps/rosbag_record.log`.

## Evidence Files

- Environment: `environment.md`, `ping_base_station.log`, `ssh_remote_check.log`.
- Build/test: `local_cmake_configure.log`, `local_build.log`, `local_ctest.log`, `base_build_and_ctest.log`.
- Commands: `commands/build_commands.txt`, `commands/full_payload_sweep_commands.txt`, `commands/mqtt_resume_from_262144_commands.txt`.
- Metadata: `full_payload_sweep_metadata.md`, `mqtt_resume_from_262144_metadata.md`.
- Base-station receiver/broker logs: `base_station/`.
- Stop cleanup: `stop_remote_cleanup.log`, `stop_rsync_from_base_station.log`, `ssh_key_remove.log`.
- Local CTest: `100% tests passed` (`6/6`).
- Base-station CTest: `100% tests passed` (`6/6`).

## Failures / Caveats

```text
2026-05-14T16:36:12-04:00 MQTT 262144-byte attempt ended with script exit 141 at 423 complete sender rows; preserving partial and rerunning 262144+ with fresh broker per condition.
2026-05-14T17:38:34-04:00 user requested stop; active MQTT 1048576-byte condition was interrupted.
```
- MQTT `262144` first attempt ended at 423 complete sender rows and was rerun successfully with a fresh threaded broker per condition.
- MQTT `1048576` is partial because the user requested stop during that condition. The final row was incomplete and is excluded from summary metrics.
- MQTT `2097152` was not run.
- The large-payload probes are sequential request/ack latency measurements, not streaming throughput benchmarks.
