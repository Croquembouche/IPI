# Private 5G Raw-Byte Uplink Payload Sweep

- Final requested matrix: `passed` (8/8 validated conditions; 7 completed and 1 user-stopped).
- Attempts: `6327`; accepted: `6327`; failed: `0`; success: `100.000000%`.
- Request: exact raw application body; response: compact correlated application acknowledgment after edge length/CRC32 validation.
- RTT: sender monotonic time immediately before framed transmission through receipt and structural parsing of the complete application acknowledgment.
- Recorded radio context: `60/20/20`, RSRP `-100 dBm`, RSRQ `-13 dB`.
- Airspan administrative state: Cell 1 `locked` (not broadcasting); Cell 2 `unlocked` (broadcasting). Serving cell recorded as Cell 2; its evidence state is listed below.
- Location: stationary `location_2`; repository-safe coordinate derivative `39.666, -75.757`.
- One-way latency is excluded because endpoint clocks were unsynchronized.

## Per-Condition Results

| transport | payload | TDD | RSRP dBm | RSRQ dB | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | miss @100 ms | miss @500 ms | miss @1000 ms | app goodput Mbps |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 KiB | 60/20/20 | -100 | -13 | 1000/1000 | 47.701 | 77.824 | 377.658 | 433.86 | 45 | 0 | 0 | 0.032 |
| TCP | 10 KiB | 60/20/20 | -100 | -13 | 1000/1000 | 169.206 | 487.783 | 733.619 | 2113.708 | 879 | 45 | 2 | 0.2 |
| TCP | 100 KiB | 60/20/20 | -100 | -13 | 1000/1000 | 1460.886 | 2321.439 | 2981.048 | 3773.505 | 1000 | 985 | 811 | 0.49 |
| TCP | 1,024 KiB | 60/20/20 | -100 | -13 | 227/227 | 17031.656 | 20982.534 | 24535.774 | 25263.822 | 227 | 227 | 227 | 0.481 |
| MQTT | 1 KiB | 60/20/20 | -100 | -13 | 1000/1000 | 42.125 | 90.05 | 311.772 | 562.268 | 46 | 4 | 0 | 0.032 |
| MQTT | 10 KiB | 60/20/20 | -100 | -13 | 1000/1000 | 149.582 | 461.766 | 654.742 | 1406.279 | 841 | 40 | 4 | 0.206 |
| MQTT | 100 KiB | 60/20/20 | -100 | -13 | 1000/1000 | 1717.276 | 2614.26 | 3251.222 | 4482.61 | 1000 | 1000 | 947 | 0.418 |
| MQTT | 1,024 KiB | 60/20/20 | -100 | -13 | 100/100 | 17229.249 | 20834.363 | 21150.195 | 22356.526 | 100 | 100 | 100 | 0.474 |

## Evidence Qualifications

- Payload labels are application-body bytes, not individual IP packets. TCP can segment one application object across many packets; MQTT adds its own framing over TCP.
- Application goodput is accepted payload bytes divided by the sender span from the first request transmission to the final application acknowledgment. Interface rates also include framing, acknowledgments, telemetry, and incidental interface traffic.
- Topic/sequence/accepted-state/payload-length/CRC32 comparisons occur immediately after the RTT timer. Every accepted row passed them, but their local CPU time is not included in `rtt_ms`.
- The application-summary rows use the final run-level TDD, RSRP, and RSRQ context. Acquisition manifests retain their contemporaneous entries; provisional values are superseded by the timestamped radio-context update rather than silently rewritten.
- The per-condition telemetry captures are process-scoped in time, but cross-host telemetry timestamps are not used for latency because the clocks were unsynchronized.
- The one-minute ROS 2 bag and exact coordinates are retained in raw location evidence `20260817_airspan_tdd_raw_uplink_pending_location_2_tcp_mqtt_run_1_unredacted`; the public derivative retains only the stationarity summary and 0.001-degree coordinate.
- Evidence states: TDD `user-reported-during-run-on-2026-08-17`; serving context `carried-forward-cell-2-no-reported-handoff`; RSRP `user-reported-during-run-on-2026-08-17`; RSRQ `carried-forward-from-most-recent-user-report-on-2026-08-16`. Timestamp-aligned Airspan/ACP and MG52 exports are still required for artifact verification.
- This payload sweep covers one location and one profile. It cannot estimate a causal TDD effect and does not replace the planned two-location `40/40/20` versus `70/20/10` directional matrix.
- Known acquisition anomalies are preserved in `known_anomalies.json` (2 record(s)); no row was silently filtered.
