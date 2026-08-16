# Private 5G Raw-Byte Uplink Payload Sweep

- Application matrix: `passed` (3/3 conditions).
- Attempts: `3000`; accepted: `3000`; failed: `0`; success: `100.000000%`.
- Request: exact raw application body; response: compact correlated application acknowledgment after edge length/CRC32 validation.
- RTT: sender monotonic time immediately before framed transmission through receipt and structural parsing of the complete application acknowledgment.
- Recorded radio context: `60/20/20`, RSRP `-98 dBm`, RSRQ `-13 dB`.
- Airspan administrative state: Cell 1 `locked` (not broadcasting); Cell 2 `unlocked` (broadcasting). Serving cell recorded as Cell 2; its evidence state is listed below.
- Referenced location evidence: same-day stationary `location_1` capture; repository-safe coordinate derivative `39.664, -75.757`. Continuity with this application run was not independently reverified.
- One-way latency is excluded because endpoint clocks were unsynchronized.

## Per-Condition Results

| transport | payload | TDD | RSRP dBm | RSRQ dB | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | miss @100 ms | miss @500 ms | miss @1000 ms | app goodput Mbps |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 KiB | 60/20/20 | -98 | -13 | 1000/1000 | 49.604 | 75.391 | 361.531 | 387.686 | 30 | 0 | 0 | 0.032 |
| TCP | 10 KiB | 60/20/20 | -98 | -13 | 1000/1000 | 101.216 | 248.853 | 377.315 | 1013.779 | 507 | 3 | 1 | 0.254 |
| TCP | 100 KiB | 60/20/20 | -98 | -13 | 1000/1000 | 753.242 | 1321.325 | 1773.416 | 2547.593 | 1000 | 836 | 201 | 0.828 |

## Intentionally Stopped 1 MiB Condition

At the user's direction, the 1,024 KiB condition stopped after exactly 500
completed application acknowledgments rather than the 1,000 planned when the
runner started. All 500 sender rows are accepted and match 500 edge rows with
sequences exactly 1 through 500 and exact payload-length/CRC32 correlation.
The sender's termination status is 143 and no completion marker exists, so
this condition is preserved separately and excluded from the complete
three-condition matrix.

| transport | payload | corrected TDD | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | app goodput Mbps |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1,024 KiB | 60/20/20 | 500/500 retained | 7484.305 | 10187.364 | 11175.804 | 13368.893 | 1.086 |

## Evidence Qualifications

- Payload labels are application-body bytes, not individual IP packets. TCP can segment one application object across many packets; MQTT adds its own framing over TCP.
- Application goodput is accepted payload bytes divided by the sender span from the first request transmission to the final application acknowledgment. Interface rates also include framing, acknowledgments, telemetry, and incidental interface traffic.
- Topic/sequence/accepted-state/payload-length/CRC32 comparisons occur immediately after the RTT timer. Every accepted row passed them, but their local CPU time is not included in `rtt_ms`.
- Acquisition-time manifests contain the initial TDD entry `60/30/10`; the user's post-acquisition correction `60/20/20` is applied through `tdd_profile_correction.json`. RSRP and RSRQ remain consistent across manifests and run-level context.
- The per-condition telemetry captures are process-scoped in time, but cross-host telemetry timestamps are not used for latency because the clocks were unsynchronized.
- The referenced one-minute ROS 2 bag and exact coordinates remain in excluded raw location evidence `20260816_airspan_tdd_raw_uplink_40_40_20_location_1_mqtt_repeat_run_1_unredacted`; only the stationarity summary and 0.001-degree derivative are repository-facing.
- Evidence states: TDD `user-post-acquisition-correction-on-2026-08-16`; serving context `user-reported-current-cell-2-no-handoff`; RSRP `carried-forward-from-most-recent-user-report-on-2026-08-16`; RSRQ `carried-forward-from-most-recent-user-report-on-2026-08-16`. Timestamp-aligned Airspan/ACP and MG52 exports are still required for artifact verification.
- This payload sweep covers one location and one profile. It cannot estimate a causal TDD effect and does not replace the planned two-location `40/40/20` versus `70/20/10` directional matrix.
- The acquisition-time condition manifests retain their original TDD entry. `tdd_profile_correction.json` supersedes that entry for radio-context interpretation; application measurements are unchanged.
- Known acquisition anomalies are preserved in `known_anomalies.json` (1 record(s)); no row was silently filtered.
