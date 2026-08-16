# Private 5G Raw-Byte Uplink Payload Sweep

- Application matrix: `passed` (3/3 conditions).
- Attempts: `3000`; accepted: `3000`; failed: `0`; success: `100.000000%`.
- Request: exact raw application body; response: compact correlated application acknowledgment after edge length/CRC32 validation.
- RTT: sender monotonic time immediately before framed transmission through receipt and structural parsing of the complete application acknowledgment.
- Radio context: operator-reported `40/40/20`, RSRP `-100 dBm`, and RSRQ `-13 dB`. Cell 1 was administratively locked and not broadcasting; Cell 2 was broadcasting and was the reported serving cell with no handoff.
- Location: stationary `location_1`; repository-safe coordinate derivative `39.664, -75.757`.
- One-way latency is excluded because endpoint clocks were unsynchronized.

## Per-Condition Results

| transport | payload | TDD | RSRP dBm | RSRQ dB | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | miss @100 ms | miss @500 ms | miss @1000 ms | app goodput Mbps |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 KiB | 40/40/20 | -100 | -13 | 1000/1000 | 39.634 | 51.548 | 77.657 | 359.632 | 7 | 0 | 0 | 0.034 |
| TCP | 10 KiB | 40/40/20 | -100 | -13 | 1000/1000 | 57.519 | 91.421 | 211.822 | 465.504 | 43 | 0 | 0 | 0.312 |
| TCP | 100 KiB | 40/40/20 | -100 | -13 | 1000/1000 | 441.096 | 789.094 | 937.482 | 1120.317 | 1000 | 361 | 3 | 1.206 |

## Three-Run Radio Context

| Date | TDD | Cell 1 | Cell 2 | Serving cell | RSRP | RSRQ |
|---|---|---|---|---:|---:|---:|
| 2026-08-15 | 70/20/10 | Unlocked; broadcasting | Unlocked; broadcasting | 2; no handoff | -100 dBm | -13 dB |
| 2026-08-15 | 40/40/20 | Unlocked; broadcasting | Unlocked; broadcasting | 2; no handoff | -100 dBm | -13 dB |
| 2026-08-16 | 40/40/20 | Locked; not broadcasting | Unlocked; broadcasting (inferred) | 2; no handoff | -100 dBm | -13 dB |

## Same-Profile Repeat Context

Compared with the valid TCP subset collected on 2026-08-15 under the same
operator-reported `40/40/20` profile, the 2026-08-16 p50/p95 changes were
`+0.52%/+3.33%` at 1 KiB, `+11.38%/+27.97%` at 10 KiB, and
`+132.55%/+169.05%` at 100 KiB. The 100 KiB repeat is therefore materially
slower across the distribution. This is a repeat difference, not a causal TDD
result, because timestamp-aligned radio, scheduler, and cell-load evidence is
absent. The pair is also not configuration-matched: Cell 1 was
unlocked/broadcasting on 2026-08-15 and locked/not broadcasting on 2026-08-16.

## Evidence Qualifications

- Payload labels are application-body bytes, not individual IP packets. TCP can segment one application object across many packets; MQTT adds its own framing over TCP.
- Application goodput is accepted payload bytes divided by the sender span from the first request transmission to the final application acknowledgment. Interface rates also include framing, acknowledgments, telemetry, and incidental interface traffic.
- Topic/sequence/accepted-state/payload-length/CRC32 comparisons occur immediately after the RTT timer. Every accepted row passed them, but their local CPU time is not included in `rtt_ms`.
- TDD, RSRP, and RSRQ are copied into every per-condition manifest and application-summary row, and the analyzer rejects a condition if those values differ from the run-level operator context.
- The per-condition telemetry captures are process-scoped in time, but cross-host telemetry timestamps are not used for latency because the clocks were unsynchronized.
- The referenced one-minute ROS 2 bag and exact coordinates remain in excluded raw location evidence `20260816_airspan_tdd_raw_uplink_40_40_20_location_1_mqtt_repeat_run_1_unredacted`; only the stationarity summary and 0.001-degree derivative are repository-facing.
- TDD, Cell 1 administrative state, serving Cell 2/no-handoff state, RSRP, and RSRQ remain operator-reported pending timestamp-aligned Airspan/ACP and MG52 exports. Cell 2 broadcasting is inferred from completed serving-cell traffic; the correction is preserved in `cell_administrative_state_correction.json`.
- This payload sweep covers one location and one profile. It cannot estimate a causal TDD effect and does not replace the planned two-location `40/40/20` versus `70/20/10` directional matrix.
