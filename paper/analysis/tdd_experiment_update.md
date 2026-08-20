# TDD Experiment Update

Last updated: 2026-08-17

## Decision

All private-5G application campaigns collected before the August 15--16 TDD
diagnostic used the `70/20/10` downlink/uplink/dynamic allocation. Earlier
documents that labeled those campaigns `40/40/20` are superseded. Their
application-latency and deadline results remain valid as `70/20/10`
observations, but they do not compare TDD configurations.

The August 15--16 application RTT runs are not usable for ranking
`40/40/20` against `70/20/10`. The radio and software path did not remain in a
stable state while the profiles were changed. Route availability, cell
administrative state, and serving-cell selection changed across runs. The
`40/40/20` repetitions also produced large changes in response-latency tails.
Consequently, the observed application RTT differences cannot be assigned to
the TDD allocation.

The separate uplink-capacity control gives a different result. The
`40/40/20` configuration assigns 40 percent of the frame to fixed uplink use,
whereas `70/20/10` assigns 20 percent. Under a 25-Mbit/s offered vehicle-to-edge
TCP rate, `70/20/10` achieved 13.118 Mbit/s and `40/40/20` achieved 25.000
Mbit/s. Thus, the capacity result follows the expected benefit of a larger
uplink allocation even though the application RTT runs cannot estimate the
latency effect of that allocation.

## August 15--16 Application Evidence

Each application attempt uploaded the selected number of bytes from the
vehicle to `d1` in an IPI request and waited for a compact correlated
acknowledgment. The numbers below are response p95 RTTs, in milliseconds.

### TCP

| TDD profile and repetition | 1 KiB | 10 KiB | 100 KiB | Recorded state |
|---|---:|---:|---:|---|
| `70/20/10`, Aug. 15 | 41.721 | 49.082 | 111.503 | Cell 2 serving; both cells broadcasting |
| `70/20/10`, Aug. 16 | 41.975 | 49.190 | 112.947 | Cell 2 serving; Cell 1 not broadcasting |
| Relative change | +0.61% | +0.22% | +1.29% | Same-profile repeat observation |
| `40/40/20`, Aug. 15 reference | 49.886 | 71.438 | 293.285 | Retained in comparison CSV; raw reference tree absent |
| `40/40/20`, Aug. 16 run 1 | 51.548 | 91.421 | 789.094 | Cell 2 serving; Cell 1 not broadcasting |
| Relative change | +3.33% | +27.97% | +169.05% | Cross-day same-profile observation |
| `40/40/20`, Aug. 16 run 2 | 149.399 | 187.382 | 927.334 | Cell 1 serving; both cells broadcasting |
| Run 1 to run 2 change | +189.83% | +104.97% | +17.52% | Serving cell and cell state changed |

### MQTT under `40/40/20`

| Payload | Aug. 15 p95 RTT | Aug. 16 p95 RTT | Change |
|---:|---:|---:|---:|
| 1 KiB | 46.708 ms | 51.016 ms | +9.22% |
| 10 KiB | 105.436 ms | 169.247 ms | +60.52% |
| 100 KiB | 353.556 ms | 746.947 ms | +111.27% |
| 1,024 KiB | 2,125.408 ms | 5,608.034 ms | +163.86% |

The August 15 MQTT reference values survive in the August 16 derived
comparison CSV, but the corresponding raw August 15 result directory is not
present in the current repository. The paper may use the comparison to show
why the application-latency TDD diagnostic is unusable, not as a profile
performance estimate.

## Uplink-Capacity Evidence

| TDD profile | Offered vehicle-to-edge rate | Achieved rate | Evidence status |
|---|---:|---:|---|
| `70/20/10` | 25.000 Mbit/s | 13.118 Mbit/s | Stored host-side sender and receiver traces |
| `40/40/20` | 25.000 Mbit/s | 25.000 Mbit/s | Coauthor experimental record; raw trace not present in the current repository |

The stored `70/20/10` control transferred 1,475,758,800 bytes in 900 seconds,
with matching sender and receiver totals. The `40/40/20` result is retained as
a coauthor-provided experiment record until its raw trace is added.

## Manuscript Claim Boundary

The paper can state that the application-latency diagnostic did not isolate a
TDD effect because the platform state was unstable. It can also report that
the capacity controls followed the expected uplink-allocation direction. The
paper must not claim that `70/20/10` has intrinsically lower application RTT,
that `40/40/20` causes the observed tail growth, or that the two configurations
were compared under identical radio and software conditions.

## Primary Artifacts

- `results/real_5g/20260815_airspan_tdd_raw_uplink_70_20_10_location_1_run_2/`
- `results/real_5g/20260816_airspan_tdd_raw_uplink_70_20_10_location_1_tcp_quick_run_1/`
- `results/real_5g/20260816_airspan_tdd_raw_uplink_40_40_20_location_1_tcp_quick_run_1/`
- `results/real_5g/20260816_airspan_tdd_raw_uplink_40_40_20_location_1_tcp_quick_run_2/`
- `results/real_5g/20260816_airspan_tdd_raw_uplink_40_40_20_location_1_mqtt_repeat_run_1/`
- `results/real_5g/20260805_airspan_r1_run_1/`
