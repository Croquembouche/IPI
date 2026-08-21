# Direct C-V2X Experiment Summary

This document summarizes the retained commercial Long-Term Evolution cellular
vehicle-to-everything (LTE C-V2X) direct PC5 experiments in
`results/mocar_v2x/`. The field path connected a Mocar on-board unit (OBU) to a
fixed HUALI roadside unit (RSU). The devices used the vendor's 2020 SAE J2735
stack.

## Experimental definition

| Item | Experimental definition |
|---|---|
| Functional-message validation | Standard Signal Phase and Timing (SPaT) and Basic Safety Message (BSM) paths were exercised before the frame-size experiments. |
| Frame-size and mobility exchanges | The OBU sent a serialized `IPI_RTT1` performance request through the vendor packet interface using custom message type `0x1b`. The RSU decoded its origin and sequence fields and returned a sequence-matched IPI response. Every nonzero condition used the configured complete frame length in both directions. |
| Timing boundary | Round-trip time (RTT) began immediately before the OBU sent a request and ended when the OBU received the matching response. Thus, RTT includes two PC5 traversals and both vendor-interface paths. |
| Stationary workload | Configured complete IPI frame sizes were 256, 512, 1,024, and 2,048 B. A separate zero control produced the minimum valid 51--55-B request header rather than a zero-length transmission. Each planned phase contained 1,000 attempts, a 100-ms interval after each attempt, and a 1,000-ms response timeout. The OBU stack was restarted between phases. |
| Mobility workload | Each route used a complete 256-B serialized IPI frame and a 200-ms interval. Runs 1 and 2 used a 1,000-ms timeout; runs 3 and 4 used a 500-ms timeout. |
| Availability | Availability is the percentage of planned attempts that returned the matching response. Consistent with the experiment analysis, uncompleted or skipped attempts after an operator-stopped timeout-dominant phase are counted as timeouts. |
| Latency statistics | Mean, p50, p95, and p99 RTT are calculated only from successful responses. A dash means that the condition produced no successful response in the analysis. |
| Configured zero setting | The zero setting still carried the required IPI identity and timing header. The retained request records span 51--55 B as identifier and decimal field lengths change. For all nonzero conditions, the transmitted request packet length equals the configured complete frame size; the responder constructs the same configured total length. |
| Radio measurements | The installed OBU image did not export usable PC5 received signal strength indicator (RSSI), signal-to-noise ratio (SNR), reference signal received power (RSRP), or reference signal received quality (RSRQ). The signal-diagnostic files were empty. Therefore, the retained radio outcomes are response availability, RTT, location, obstruction context, and mobility. |
| Installed-interface limit | Static analysis of the deployed vendor SDK found a 4,080-B maximum application packet after vendor framing. The experiments intentionally stopped at 2,048 B. |

## Functional path validation

| Test | Direction | Result | Timing |
|---|---|---:|---:|
| IPI-to-SPaT bridge and standard SPaT callback | Edge sender to RSU bridge to OBU | 10 / 10 SPaT messages received by the OBU | Not measured |
| Standard BSM reception | RSU to OBU | 3 BSMs received | Not measured |
| Standard BSM reception | OBU to RSU | 4 BSMs received | Not measured |
| Sequence-matched custom request and response | RSU initiator to OBU responder | 10 / 10 responses | Mean 97.262 ms; minimum 71.196 ms; maximum 102.850 ms |
| Sequence-matched custom request and response | OBU initiator to RSU responder | 10 / 10 responses | Mean 100.712 ms; minimum 84.300 ms; maximum 114.288 ms |

The installed public `mde_v2x_custom_send()` entry point returned success but
did not place an effective custom payload on the measured path. The frame-size
and mobility experiments therefore used the deployed packet-data function
`v2x_packet_data_send(payload, len, 0x1b)`.

## Stationary frame-size results

The reference row combines five separately collected full frame-size runs at
approximately 7 m from the RSU. The continuous sweep and Points 1--5 each ran
the configured frame sizes in ascending order at one location. Point 1 was approximately
68 m from the RSU but had a building-obstructed non-line-of-sight path.

| Location / collection | Configured IPI frame size (B) | Replies / 1,000 | Availability | Mean RTT (ms) | p50 (ms) | p95 (ms) | p99 (ms) |
|---|---:|---:|---:|---:|---:|---:|---:|
| Reference, separate full runs (approximately 7 m) | 0 | 1,000 / 1,000 | 100.0% | 93.252 | 99.939 | 106.873 | 111.814 |
|  | 256 | 999 / 1,000 | 99.9% | 98.324 | 99.980 | 106.884 | 111.691 |
|  | 512 | 1,000 / 1,000 | 100.0% | 98.848 | 99.972 | 106.691 | 114.577 |
|  | 1,024 | 1,000 / 1,000 | 100.0% | 99.841 | 99.966 | 106.877 | 112.513 |
|  | 2,048 | 1,000 / 1,000 | 100.0% | 98.515 | 99.958 | 111.661 | 120.562 |
| Continuous sweep (approximately 48 m) | 0 | 1,000 / 1,000 | 100.0% | 97.908 | 99.971 | 108.645 | 111.728 |
|  | 256 | 999 / 1,000 | 99.9% | 98.706 | 99.968 | 108.903 | 113.727 |
|  | 512 | 999 / 1,000 | 99.9% | 101.953 | 99.965 | 107.833 | 112.632 |
|  | 1,024 | 976 / 1,000 | 97.6% | 100.724 | 99.953 | 110.829 | 116.660 |
|  | 2,048 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
| P1, building-obstructed NLOS (68 m) | 0 | 1,000 / 1,000 | 100.0% | 107.235 | 99.988 | 196.691 | 204.559 |
|  | 256 | 997 / 1,000 | 99.7% | 100.628 | 99.983 | 109.781 | 112.814 |
|  | 512 | 955 / 1,000 | 95.5% | 97.080 | 99.968 | 109.045 | 112.558 |
|  | 1,024 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
|  | 2,048 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
| P2 (113 m) | 0 | 999 / 1,000 | 99.9% | 98.690 | 99.978 | 108.812 | 112.730 |
|  | 256 | 1,000 / 1,000 | 100.0% | 100.022 | 99.970 | 107.879 | 113.705 |
|  | 512 | 1,000 / 1,000 | 100.0% | 99.025 | 99.975 | 108.860 | 112.662 |
|  | 1,024 | 1,000 / 1,000 | 100.0% | 99.731 | 99.970 | 106.894 | 112.708 |
|  | 2,048 | 998 / 1,000 | 99.8% | 99.115 | 99.957 | 110.682 | 119.554 |
| P3 (212 m) | 0 | 1,000 / 1,000 | 100.0% | 98.049 | 99.969 | 107.746 | 112.612 |
|  | 256 | 1,000 / 1,000 | 100.0% | 99.616 | 99.977 | 107.805 | 113.649 |
|  | 512 | 998 / 1,000 | 99.8% | 101.220 | 99.967 | 107.815 | 110.843 |
|  | 1,024 | 979 / 1,000 | 97.9% | 97.127 | 99.965 | 109.777 | 114.644 |
|  | 2,048 | 8 / 1,000 | 0.8% | 69.609 | 74.950 | 106.210 | 106.210 |
| P4 (377 m) | 0 | 999 / 1,000 | 99.9% | 94.054 | 99.954 | 106.976 | 112.627 |
|  | 256 | 991 / 1,000 | 99.1% | 97.824 | 99.969 | 107.847 | 112.593 |
|  | 512 | 969 / 1,000 | 96.9% | 98.883 | 99.969 | 110.741 | 115.691 |
|  | 1,024 | 171 / 1,000 | 17.1% | 79.212 | 98.911 | 111.730 | 115.700 |
|  | 2,048 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
| P5 (469 m) | 0 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
|  | 256 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
|  | 512 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
|  | 1,024 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |
|  | 2,048 | 0 / 1,000 | 0.0% | -- | -- | -- | -- |

## Signal-diagnostic probe

All five 256-B traffic checks succeeded. However, the corresponding PC5 signal
diagnostic outputs contained zero bytes.

| Capture mode | Responses / attempts | Availability | p50 RTT (ms) | p95 RTT (ms) | p99 RTT (ms) | PC5 RF metric returned |
|---|---:|---:|---:|---:|---:|---:|
| Baseline | 20 / 20 | 100.0% | 49.728 | 56.116 | 56.135 | None |
| Raw diagnostic | 30 / 30 | 100.0% | 49.484 | 56.264 | 58.108 | None |
| Timestamp/frame diagnostic | 30 / 30 | 100.0% | 49.454 | 56.262 | 57.846 | None |
| Total diagnostic | 30 / 30 | 100.0% | 49.496 | 58.085 | 58.984 | None |
| Vendor-labeled `SCH` filter | 30 / 30 | 100.0% | 49.430 | 55.751 | 62.042 | None |

## Mobile-route results

| Route | Configured IPI frame size | Attempts | Responses | Availability | Timeout (ms) | p50 RTT (ms) | p95 RTT (ms) | p99 RTT (ms) | GNSS duration (s) | Median speed |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Run 1 (`173816`) | 256 B | 675 | 515 | 76.3% | 1,000 | 99.610 | 108.421 | 112.436 | 452.326 | 7.131 m/s (25.7 km/h) |
| Run 2 (`175820`) | 256 B | 1,000 | 731 | 73.1% | 1,000 | 28.326 | 41.887 | 55.880 | 531.529 | 7.541 m/s (27.1 km/h) |
| Run 3 (`182104`) | 256 B | 1,000 | 591 | 59.1% | 500 | 29.204 | 43.923 | 54.317 | 439.610 | 8.751 m/s (31.5 km/h) |
| Run 4 (`183232`) | 256 B | 1,000 | 716 | 71.6% | 500 | 28.276 | 41.621 | 50.651 | 377.888 | 7.037 m/s (25.3 km/h) |
| All four routes | 256 B | 3,675 | 2,553 | 69.5% | Mixed | -- | -- | -- | -- | -- |

## Directly supported comparisons

| Comparison | Result shown by the experiment |
|---|---|
| Complete frame size at a favorable point | At P2, availability remained 99.8--100.0% through the 2,048-B complete IPI frame. The measured path can therefore carry every tested frame size when the field condition is favorable. |
| Complete frame size as the path degrades | At P3, 1,024-B frames retained 97.9% availability while 2,048-B frames fell to 0.8%. At P4, 512-B frames retained 96.9%, 1,024-B frames fell to 17.1%, and 2,048-B frames produced no response. Larger frames lose availability before compact frames at the same location. |
| Obstruction versus distance | Building-obstructed P1 lost all 1,024-B and 2,048-B responses at 68 m, whereas P2 delivered 99.8--100.0% of all tested sizes at 113 m. Distance alone does not explain the stationary result. |
| End of the measured path | P5 produced no response even for the minimum-header control. Once the direct path became unavailable, reducing the complete IPI frame size did not restore communication. |
| Mobility | Across the four routes, only 59.1--76.3% of 256-B attempts returned a response. In runs 2--4, the p95 RTT among returned responses remained 41.6--43.9 ms. Therefore, successful-response latency and response availability describe different parts of mobile performance. |
| RF attribution | Because the installed devices did not export PC5 received-power or quality measurements, the data support comparisons by location, obstruction, complete frame size, and mobility, but they do not identify a specific radio-layer cause such as received power, modulation, resource selection, or retransmission behavior. |

## Source artifacts

| Evidence | Stored artifact |
|---|---|
| Setup and functional validation | `results/mocar_v2x/20260703_setup_test/summary.md` |
| Consolidated reference payload runs | `results/mocar_v2x/20260703_exp_01_payload_sweep_0_2kb_final/payload_summary.csv` |
| Continuous July 3 sweep | `results/mocar_v2x/20260703_exp_01_payload_sweep_0_2kb_171209/payload_summary.csv` |
| Stationary Points 1--5 | `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_*/payload_summary.csv` |
| Signal diagnostic probe | `results/mocar_v2x/20260703_obu_signal_probe_goodlink_170247/summary.md` |
| Four mobile routes | `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_*/radio_mobility_summary.csv` |
| Mobile GNSS and speed | `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_*/gps/gps_samples.csv` |
