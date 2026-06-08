# Capped Large Payload Sweep Summary

- Updated: `2026-05-13T14:18:06-04:00`
- Parent run: `20260513_sunny_fintechparking_run_1`
- Scope: TCP service payload doubling sweep capped at `2097152` bytes per user request.
- Not run: `4194304`, `8388608`, `16777216`, `33554432` byte TCP conditions; large MQTT sweep.
- Count: `1000` probes per completed condition; interval setting: `200 ms`.
- Clock sync state: `unsynced`; RTT is the reliable metric. One-way columns remain raw evidence only.
- RTT timestamping excludes request packaging time: sender timestamp is patched immediately before network send/publish.

| Condition | Payload arg bytes | Encoded frame bytes | Attempts | Accepted | Success % | Duration s | RTT p50 ms | RTT p95 ms | RTT p99 ms | Server p50 ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| p5g-tcp-service-payload-8192 | 8192 | 8270 | 1000 | 1000 | 100.0 | 284.293 | 82.252 | 109.779 | 116.711 | 0.052 |
| p5g-tcp-service-payload-16384 | 16384 | 16462 | 1000 | 1000 | 100.0 | 281.521 | 80.039 | 99.814 | 109.959 | 0.054 |
| p5g-tcp-service-payload-32768 | 32768 | 32846 | 1000 | 1000 | 100.0 | 302.709 | 99.816 | 123.571 | 189.631 | 0.059 |
| p5g-tcp-service-payload-65536 | 65536 | 65614 | 1000 | 1000 | 100.0 | 340.521 | 139.448 | 165.108 | 189.117 | 0.064 |
| p5g-tcp-service-payload-131072 | 131072 | 131150 | 1000 | 1000 | 100.0 | 423.292 | 219.534 | 256.585 | 295.649 | 0.078 |
| p5g-tcp-service-payload-262144 | 262144 | 262222 | 1000 | 1000 | 100.0 | 596.526 | 389.933 | 466.062 | 523.102 | 0.103 |
| p5g-tcp-service-payload-524288 | 524288 | 524366 | 1000 | 1000 | 100.0 | 935.474 | 708.349 | 834.545 | 1202.307 | 0.157 |
| p5g-tcp-service-payload-1048576 | 1048576 | 1048654 | 1000 | 1000 | 100.0 | 1823.118 | 1550.963 | 2061.017 | 2378.588 | 2.393 |
| p5g-tcp-service-payload-2097152 | 2097152 | 2097230 | 1000 | 1000 | 100.0 | 3382.292 | 3140.270 | 3632.799 | 4053.112 | 1.984 |

## Evidence

- Commands: `commands/large_payload_commands.txt`.
- Metadata: `large_payload_metadata.md`.
- Remote cleanup log: `large_payload_remote_cleanup.log`.
- Base-station log sync: `large_payload_rsync_from_base_station.log`.
- Sender CSVs are in this run folder; receiver CSVs are under `base_station/`.

## Caveats

- The script was intentionally stopped after the completed 2 MiB TCP condition, so no larger payload rows should be interpreted as missing data due to failure.
- MQTT large-payload conditions were not run after the cap request.
- The sender is sequential request/ack plus interval; this is a large-message latency probe, not a streaming throughput benchmark.
