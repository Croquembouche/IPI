# Private 5G Raw-Byte Uplink Payload Sweep

- Application matrix: `passed` (10/10 conditions).
- Attempts: `10000`; accepted: `10000`; failed: `0`; success: `100.000000%`.
- Request: exact raw application body; response: compact correlated application acknowledgment after edge length/CRC32 validation.
- RTT: sender monotonic time immediately before framed transmission through receipt and structural parsing of the complete application acknowledgment.
- Radio context: operator-reported `70/20/10`, RSRP `-100 dBm`, and RSRQ `-13 dB`. Cell 1 and Cell 2 were administratively unlocked and broadcasting; Cell 2 was the reported serving cell with no handoff.
- Location: stationary `location_1`; repository-safe coordinate derivative `39.664, -75.757`.
- One-way latency is excluded because endpoint clocks were unsynchronized.

## Per-Condition Results

| transport | payload | accepted/attempts | p50 ms | p95 ms | p99 ms | max ms | miss @100 ms | miss @500 ms | miss @1000 ms | app goodput Mbps |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 KiB | 1000/1000 | 31.639 | 41.721 | 47.781 | 91.669 | 0 | 0 | 0 | 0.035 |
| TCP | 10 KiB | 1000/1000 | 39.596 | 49.082 | 53.815 | 83.748 | 0 | 0 | 0 | 0.342 |
| TCP | 100 KiB | 1000/1000 | 101.29 | 111.503 | 119.386 | 151.802 | 547 | 0 | 0 | 2.71 |
| TCP | 1,024 KiB | 1000/1000 | 770.485 | 818.754 | 878.972 | 1580.884 | 1000 | 1000 | 4 | 8.619 |
| TCP | 2,048 KiB | 1000/1000 | 1500.456 | 1584.062 | 1628.938 | 2263.905 | 1000 | 1000 | 1000 | 9.847 |
| MQTT | 1 KiB | 1000/1000 | 29.982 | 40.476 | 44.539 | 60.608 | 0 | 0 | 0 | 0.036 |
| MQTT | 10 KiB | 1000/1000 | 39.573 | 50.225 | 55.383 | 83.142 | 0 | 0 | 0 | 0.34 |
| MQTT | 100 KiB | 1000/1000 | 99.473 | 111.174 | 127.308 | 161.32 | 474 | 0 | 0 | 2.716 |
| MQTT | 1,024 KiB | 1000/1000 | 767.345 | 810.505 | 868.133 | 1328.069 | 1000 | 1000 | 4 | 8.639 |
| MQTT | 2,048 KiB | 1000/1000 | 1501.583 | 1591.377 | 1682.788 | 1949.352 | 1000 | 1000 | 1000 | 9.82 |

## Evidence Qualifications

- Payload labels are application-body bytes, not individual IP packets. TCP can segment one application object across many packets; MQTT adds its own framing over TCP.
- Application goodput is accepted payload bytes divided by the sender span from the first request transmission to the final application acknowledgment. Interface rates also include framing, acknowledgments, telemetry, and incidental interface traffic.
- Topic/sequence/accepted-state/payload-length/CRC32 comparisons occur immediately after the RTT timer. Every accepted row passed them, but their local CPU time is not included in `rtt_ms`.
- During TCP 2 MiB sequences 97-116, a 22.7-second read-only Git integrity scan briefly raised car CPU to 11.2%. The rows remain included and no saturation or failure occurred. The final condition maximum was sequence 613, outside this window.
- The per-condition telemetry captures are process-scoped in time, but cross-host telemetry timestamps are not used for latency because the clocks were unsynchronized.
- The one-minute ROS 2 bag and exact coordinates remain in the excluded raw location evidence; only the stationarity summary and 0.001-degree derivative are repository-facing.
- TDD, serving Cell 2/no-handoff state, both cells' administrative/broadcast states, RSRP, and RSRQ remain operator-reported pending timestamp-aligned Airspan/ACP and MG52 exports. The post-acquisition cell-state clarification is preserved in `cell_administrative_state_correction.json`.
- This payload sweep covers one location and one profile. It cannot estimate a causal TDD effect and does not replace the planned two-location `40/40/20` versus `70/20/10` directional matrix.
