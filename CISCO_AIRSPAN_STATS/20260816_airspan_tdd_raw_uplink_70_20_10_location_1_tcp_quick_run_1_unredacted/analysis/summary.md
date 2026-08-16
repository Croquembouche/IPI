# Private 5G Raw-Byte Uplink Payload Sweep

- Application matrix: `passed` (3/3 conditions).
- Attempts: `3000`; accepted: `3000`; failed: `0`; success: `100.000000%`.
- Request: exact raw application body; response: compact correlated application acknowledgment after edge length/CRC32 validation.
- RTT: sender monotonic time immediately before framed transmission through receipt and structural parsing of the complete application acknowledgment.
- Recorded radio context: `70/20/10`, RSRP `-98 dBm`, RSRQ `-13 dB`.
- Airspan administrative state: Cell 1 `locked` (not broadcasting); Cell 2 `unlocked` (broadcasting). Serving cell recorded as Cell 2; its evidence state is listed below.
- Referenced location evidence: same-day stationary `location_1` capture; repository-safe coordinate derivative `39.664, -75.757`. Continuity with this application run was not independently reverified.
- One-way latency is excluded because endpoint clocks were unsynchronized.

## Per-Condition Results

| transport | payload | TDD | RSRP dBm | RSRQ dB | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | miss @100 ms | miss @500 ms | miss @1000 ms | app goodput Mbps |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 KiB | 70/20/10 | -98 | -13 | 1000/1000 | 31.583 | 41.975 | 49.638 | 101.337 | 1 | 0 | 0 | 0.035 |
| TCP | 10 KiB | 70/20/10 | -98 | -13 | 1000/1000 | 39.594 | 49.19 | 53.55 | 110.003 | 1 | 0 | 0 | 0.342 |
| TCP | 100 KiB | 70/20/10 | -98 | -13 | 1000/1000 | 99.582 | 112.947 | 120.936 | 191.354 | 479 | 0 | 0 | 2.716 |

## Comparison with the First Complete 2026-08-15 70/20/10 TCP Run

Both runs used TCP, the same three application-body sizes, 1,000 attempts per
condition, the same `70/20/10` profile, stationary `location_1`, serving Cell 2,
and no reported handoff. The August 15 run recorded RSRP `-100 dBm` with both
Airspan cells unlocked/broadcasting. This run carried forward RSRP `-98 dBm`
and recorded Cell 1 locked/not broadcasting; both recorded RSRQ `-13 dB`.

| payload | Aug. 15 p50 ms | Aug. 16 p50 ms | change | Aug. 15 p95 ms | Aug. 16 p95 ms | change | Aug. 15 p99 ms | Aug. 16 p99 ms | change |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 KiB | 31.639 | 31.583 | -0.18% | 41.721 | 41.975 | +0.61% | 47.781 | 49.638 | +3.89% |
| 10 KiB | 39.596 | 39.594 | -0.01% | 49.082 | 49.190 | +0.22% | 53.815 | 53.550 | -0.49% |
| 100 KiB | 101.290 | 99.582 | -1.69% | 111.503 | 112.947 | +1.29% | 119.386 | 120.936 | +1.30% |

The medians differ by at most 1.69% and p95 values by at most 1.29% across
these samples. Single maxima were higher in this run and should not be treated
as stable tail estimates. This is a repeat under the same reported TDD profile,
not a controlled test of Cell 1 availability or RSRP: those two conditions
differ, and neither run includes timestamp-aligned ACP/MG52 configuration and
radio exports.

## Evidence Qualifications

- Payload labels are application-body bytes, not individual IP packets. TCP can segment one application object across many packets; MQTT adds its own framing over TCP.
- Application goodput is accepted payload bytes divided by the sender span from the first request transmission to the final application acknowledgment. Interface rates also include framing, acknowledgments, telemetry, and incidental interface traffic.
- Topic/sequence/accepted-state/payload-length/CRC32 comparisons occur immediately after the RTT timer. Every accepted row passed them, but their local CPU time is not included in `rtt_ms`.
- TDD, RSRP, and RSRQ are copied into every per-condition manifest and application-summary row, and the analyzer rejects a condition if those values differ from the run-level operator context.
- The per-condition telemetry captures are process-scoped in time, but cross-host telemetry timestamps are not used for latency because the clocks were unsynchronized.
- The referenced one-minute ROS 2 bag and exact coordinates remain in excluded raw location evidence `20260816_airspan_tdd_raw_uplink_40_40_20_location_1_mqtt_repeat_run_1_unredacted`; only the stationarity summary and 0.001-degree derivative are repository-facing.
- Evidence states: TDD `user-reported-current-on-2026-08-16`; serving context `user-reported-current-cell-2-no-handoff`; RSRP `carried-forward-from-most-recent-user-report-on-2026-08-16`; RSRQ `carried-forward-from-most-recent-user-report-on-2026-08-16`. Timestamp-aligned Airspan/ACP and MG52 exports are still required for artifact verification.
- This payload sweep covers one location and one profile. It cannot estimate a causal TDD effect and does not replace the planned two-location `40/40/20` versus `70/20/10` directional matrix.
