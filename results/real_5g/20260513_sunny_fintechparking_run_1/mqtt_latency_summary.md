# MQTT Latency Payload Sweep Summary

- Updated: `2026-05-13T17:16:44-04:00`
- Parent run: `20260513_sunny_fintechparking_run_1`
- Scope: MQTT service latency payload sweep from `0` bytes through `2097152` bytes.
- Count: `1000` probes per completed condition; interval setting: `200 ms`.
- Clock sync state: `unsynced`; RTT is the reliable metric. One-way columns remain raw evidence only.
- RTT timestamping excludes sender-side message packaging time: send time is patched after request encoding and immediately before MQTT publish.
- Broker evidence: initial sizes used `minimal_mqtt_broker.py`; the first 1 MiB attempt stalled, so the 1 MiB and 2 MiB final results used `threaded_mqtt_broker.py`.

| Condition | Payload arg bytes | Encoded frame bytes | Attempts | Accepted | Success % | Duration s | RTT p50 ms | RTT p95 ms | RTT p99 ms | Server p50 ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| p5g-mqtt-latency-payload-0 | 0 | 61 | 1000 | 1000 | 100.0 | 226.981 | 27.666 | 38.760 | 41.890 | 0.043 |
| p5g-mqtt-latency-payload-256 | 256 | 332 | 1000 | 1000 | 100.0 | 225.033 | 22.292 | 36.403 | 41.722 | 0.049 |
| p5g-mqtt-latency-payload-1024 | 1024 | 1100 | 1000 | 1000 | 100.0 | 235.191 | 35.996 | 42.803 | 47.772 | 0.050 |
| p5g-mqtt-latency-payload-4096 | 4096 | 4172 | 1000 | 1000 | 100.0 | 241.750 | 39.948 | 52.023 | 57.119 | 0.050 |
| p5g-mqtt-latency-payload-8192 | 8192 | 8268 | 1000 | 1000 | 100.0 | 246.271 | 46.105 | 57.730 | 61.705 | 0.051 |
| p5g-mqtt-latency-payload-16384 | 16384 | 16460 | 1000 | 1000 | 100.0 | 262.266 | 59.889 | 79.595 | 99.206 | 0.053 |
| p5g-mqtt-latency-payload-32768 | 32768 | 32844 | 1000 | 1000 | 100.0 | 289.496 | 87.637 | 110.529 | 129.438 | 0.056 |
| p5g-mqtt-latency-payload-65536 | 65536 | 65612 | 1000 | 1000 | 100.0 | 340.454 | 139.101 | 169.441 | 190.147 | 0.061 |
| p5g-mqtt-latency-payload-131072 | 131072 | 131148 | 1000 | 1000 | 100.0 | 441.165 | 230.842 | 298.985 | 460.124 | 0.070 |
| p5g-mqtt-latency-payload-262144 | 262144 | 262220 | 1000 | 1000 | 100.0 | 656.190 | 442.226 | 578.218 | 680.425 | 0.089 |
| p5g-mqtt-latency-payload-524288 | 524288 | 524364 | 1000 | 1000 | 100.0 | 1038.203 | 803.430 | 1039.419 | 1208.596 | 0.124 |
| p5g-mqtt-latency-payload-1048576 | 1048576 | 1048652 | 1000 | 1000 | 100.0 | 1336.718 | 1093.927 | 1455.390 | 1810.023 | 0.364 |
| p5g-mqtt-latency-payload-2097152 | 2097152 | 2097228 | 1000 | 1000 | 100.0 | 2379.680 | 2136.365 | 2536.359 | 2814.849 | 0.268 |

## Evidence

- Main MQTT sweep commands: `commands/mqtt_latency_commands.txt`.
- MiB rerun commands: `commands/mqtt_latency_resume_mib_commands.txt`.
- Metadata: `mqtt_latency_metadata.md`, `mqtt_latency_resume_mib_metadata.md`.
- Broker tools: `tools/minimal_mqtt_broker.py`, `tools/threaded_mqtt_broker.py`.
- Remote cleanup log: `mqtt_latency_remote_cleanup.log`.
- Base-station log sync: `mqtt_latency_rsync_from_base_station.log`.
- Sender CSVs are in this run folder; receiver CSVs are under `base_station/`.

## Caveats

- Preserved first 1 MiB stalled attempt: `p5g-mqtt-latency-payload-1048576_sender.partial_stalled_20260513_161057.csv` with `243` complete sender rows before it stopped advancing. Matching receiver artifacts are under `base_station/` with the same `partial_stalled` stamp.
- The first 1 MiB attempt with the single-thread temporary broker stalled with queued TCP data; it was not used for the final metrics table.
- The successful 1 MiB and 2 MiB conditions used the threaded temporary broker to avoid broker-level head-of-line blocking during large publish forwarding.
- This remains a sequential request/ack latency probe, not a streaming throughput benchmark.
