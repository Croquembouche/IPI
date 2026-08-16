# Private 5G Raw-Byte Uplink Payload Sweep

- Application matrix: `passed` (3/3 conditions).
- Attempts: `3000`; accepted: `3000`; failed: `0`; success: `100.000000%`.
- Request: exact raw application body; response: compact correlated application acknowledgment after edge length/CRC32 validation.
- RTT: sender monotonic time immediately before framed transmission through receipt and structural parsing of the complete application acknowledgment.
- Recorded radio context: `40/40/20`, RSRP `-98 dBm`, RSRQ `-13 dB`.
- Airspan administrative state: Cell 1 `unlocked` (broadcasting); Cell 2 `unlocked` (broadcasting). Serving cell recorded as Cell 1; its evidence state is listed below.
- Referenced location evidence: same-day stationary `location_1` capture; repository-safe coordinate derivative `39.664, -75.757`. Continuity with this application run was not independently reverified.
- One-way latency is excluded because endpoint clocks were unsynchronized.

## Per-Condition Results

| transport | payload | TDD | RSRP dBm | RSRQ dB | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | miss @100 ms | miss @500 ms | miss @1000 ms | app goodput Mbps |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 KiB | 40/40/20 | -98 | -13 | 1000/1000 | 39.676 | 149.399 | 363.057 | 642.942 | 61 | 1 | 0 | 0.032 |
| TCP | 10 KiB | 40/40/20 | -98 | -13 | 1000/1000 | 65.491 | 187.382 | 339.494 | 660.564 | 144 | 2 | 0 | 0.289 |
| TCP | 100 KiB | 40/40/20 | -98 | -13 | 1000/1000 | 508.809 | 927.334 | 1209.344 | 4085.308 | 1000 | 523 | 33 | 1.082 |

## Same-Day Quick-Run Comparison

The earlier quick TCP run used RSRP `-100 dBm`, Cell 1 locked/not
broadcasting, and the MG52 locked onto serving Cell 2. This run used RSRP
`-98 dBm`, both cells unlocked/broadcasting, and the MG52 locked onto serving
Cell 1. Both recorded `40/40/20`; the `-13 dB` RSRQ in this run was carried
forward rather than freshly measured. The pair is therefore not a controlled
test of RSRP, cell availability, or serving-cell selection.

| payload | run 1 p50 ms | run 2 p50 ms | change | run 1 p95 ms | run 2 p95 ms | change |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 KiB | 39.634 | 39.676 | +0.11% | 51.548 | 149.399 | +189.83% |
| 10 KiB | 57.519 | 65.491 | +13.86% | 91.421 | 187.382 | +104.97% |
| 100 KiB | 441.096 | 508.809 | +15.35% | 789.094 | 927.334 | +17.52% |

The 100 KiB maximum was 4,085.308 ms at sequence 840. It passed exact
sender/receiver sequence, length, and CRC32 correlation and remains included;
its cause is not established.

## Evidence Qualifications

- Payload labels are application-body bytes, not individual IP packets. TCP can segment one application object across many packets; MQTT adds its own framing over TCP.
- Application goodput is accepted payload bytes divided by the sender span from the first request transmission to the final application acknowledgment. Interface rates also include framing, acknowledgments, telemetry, and incidental interface traffic.
- Topic/sequence/accepted-state/payload-length/CRC32 comparisons occur immediately after the RTT timer. Every accepted row passed them, but their local CPU time is not included in `rtt_ms`.
- TDD, RSRP, and RSRQ are copied into every per-condition manifest and application-summary row, and the analyzer rejects a condition if those values differ from the run-level operator context.
- The per-condition telemetry captures are process-scoped in time, but cross-host telemetry timestamps are not used for latency because the clocks were unsynchronized.
- The referenced one-minute ROS 2 bag and exact coordinates remain in excluded raw location evidence `20260816_airspan_tdd_raw_uplink_40_40_20_location_1_mqtt_repeat_run_1_unredacted`; only the stationarity summary and 0.001-degree derivative are repository-facing.
- Evidence states: TDD `user-reported-current-on-2026-08-16`; serving context `user-post-acquisition-correction-mg52-locked-onto-cell-1`; RSRP `user-reported-current-on-2026-08-16`; RSRQ `carried-forward-from-latest-operator-report-not-freshly-measured`. Timestamp-aligned Airspan/ACP and MG52 exports are still required for artifact verification.
- This payload sweep covers one location and one profile. It cannot estimate a causal TDD effect and does not replace the planned two-location `40/40/20` versus `70/20/10` directional matrix.
- The acquisition-time condition manifests retain their original serving-cell entry. `serving_cell_selection_correction.json` supersedes that entry for serving-cell interpretation; application measurements are unchanged.
- Known acquisition anomalies are preserved in `known_anomalies.json` (1 record(s)); no row was silently filtered.
