# Edge4AV Experiment Tracker

This file is the single place to record the Edge4AV experiment campaign for the
paper.

Use it for:

- run planning
- condition IDs
- commands and logs used
- summary metrics
- figure takeaways
- notes on what is paper-ready versus supplement-only

Recommended log labels:

- `radio-baseline`
- `private-5g-baseline`
- `private-5g-stressed`

Recommended per-run metadata:

- `run_id`
- `condition_id`
- `condition_label`
- `request_id`
- `service_type`
- `transport`
- `av_id`
- `obu_id`
- `rsu_id`
- `network_load_level`
- `qos_profile`
- `mobility_state`
- `clock_sync_state`

## Campaign Summary

### Site and Hardware

- Primary site:
- AVs used:
- OBU count:
- RSU count:
- Edge node:
- Private 5G deployment:
- Broker host:
- Time sync method:

### Main-Paper Service Classes

- `intersection-state assistance`
  Current implementation path: radio SPaT/MAP-style state and private-5G SPaT probes
- `fallback / guided-planning assistance`
  Current implementation path: `IPI-CooperativeService`

### Main-Paper Figures to Earn

- Figure 1: baseline service envelope across radio, private-5G TCP, private-5G MQTT
- Figure 2: scaling and fairness under physical plus emulated load
- Figure 3: crowded-network / QoS stress behavior
- Figure 4: edge-offload vs RSU-local compute, covered by existing paper result
- Figure 5: vehicle-level outcome under baseline vs stressed conditions

## Experiment Checklist

### Core Main-Paper Experiments

- [x] `E1` Baseline service envelope (private-5G TCP/MQTT and stationary Mocar V2X exp_01 radio baseline complete)
- [x] `E2` Scaling and contention (stationary private-5G TCP/UDP/MQTT 1-100 client sweep collected; runbook repeats pending)
- [ ] `E3` Crowded-network / QoS stress tests (default-load Phase A complete; real QoS comparison pending)
- [x] `E4` Edge-offload vs RSU-local compute (covered by existing "Towards Collaborative Autonomous Driving" paper result from Yuankai He; no new collection needed here)
- [ ] `E5` Vehicle-level outcome

### Supplementary Experiments

- [x] `S1` Payload sensitivity
- [ ] `S2` Failure detection and recovery
- [ ] `S3` Cold-start / session resumption (some MQTT resume artifacts exist, but no controlled S3 run yet)
- [ ] `S4` Security overhead
- [ ] `S5` RF gradient sensitivity (GNSS-indexed signal maps exist; controlled near/mid/far sweep pending)
- [x] `S6` Mobility / handover (Mocar V2X mobility collected; handover/weak-signal kept as discussion only)
- [ ] `S7` Additional queue-discipline ablations

### Recorded Evidence Snapshot

- Private-5G TCP/MQTT SPaT and guided-service baselines: completed in
  `results/real_5g/20260513_sunny_fintechparking_run_1/summary.md`.
- Private-5G TCP/MQTT payload sensitivity, stationary with GNSS: completed in
  `results/real_5g/20260521_small_rain_run_1/summary.md` and
  `results/real_5g/20260522_cloudy_run_1/summary.md`.
- V2X detector workload and detector-output payload source: completed in
  `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/`.
- Dataset-derived IPI local loopback: completed in
  `results/v2x_benchmarks/v2x-ipi-loopback-20260629T175556Z/`.
- Mocar V2X exp_01 stationary baseline/payload sweep with NovAtel GNSS:
  completed across multiple locations under `results/mocar_v2x/`; final
  location evidence is
  `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_164129/summary.md`.
- Detector-output-to-IPI private-5G TCP/MQTT replay with GNSS: completed in
  `results/real_5g/20260702_detector_output_to_ipi_run_1/summary.md`.
- Detector-output-to-IPI private-5G fragmented UDP replay with GNSS: completed
  in `results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1/summary.md`.
- Experiment 08 end-to-end deadline/service-envelope analysis over completed
  private-5G sender CSVs: completed in
  `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/summary.md`.
- Experiment 11 multiclient scalability private-5G TCP/UDP/MQTT sweep with
  GNSS: completed for two stationary current-location repeats. First repeat is
  split across `results/real_5g/20260702_multiclient_scalability_run_2/summary.md`
  and `results/real_5g/20260702_multiclient_scalability_run_3/summary.md`;
  second full sweep is in
  `results/real_5g/20260702_multiclient_scalability_run_4/summary.md`.
- Experiment 12 failure/fallback stationary no-Mocar subset with GNSS:
  receiver-restart for TCP/UDP/MQTT and MQTT broker-restart completed in
  `results/real_5g/20260703_failure_fallback_run_1/summary.md`.
- Partial July stationary run with GNSS and interrupted TCP transport-comparison
  replicate: `results/edge4av_top_tier/edge4av-stationary-goodfit-20260701T142538Z/`.
- Experiment 06 weak-signal stationary repeat with GNSS: completed in
  `results/real_5g/20260701_load_qos_weak_signal_run_1/summary.md`.
- Experiment 02 Mocar V2X radio-distance/mobility with NovAtel GNSS:
  completed in
  `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_173816/summary.md`,
  `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_175820/summary.md`,
  `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_182104/summary.md`,
  and `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_183232/summary.md`.
  Runs 3-4 include corrected send-time GNSS joins and a 500 ms timeout.
- Experiment 03 broadcast contention: marked complete by operator based on prior
  collection; no additional Mocar V2X run needed for the current paper plan.
- Experiment 05 handover/weak-signal: no new collection needed; keep as a paper
  discussion / limitation point rather than a measured result.
- Experiment 07 transport comparison: complete for the current paper plan.
  TCP/MQTT stationary evidence exists in the May runs; UDP behavior is covered
  by later private-5G UDP experiments.

### Claims to Avoid Unless You Have Real Data

- [ ] Multi-vendor interoperability
- [ ] Broad public-5G equivalence
- [ ] Cooperative-perception headline claims without a real end-to-end perception path

## E1 Baseline Service Envelope

### Goal

Measure baseline end-to-end envelopes for the two main service classes across:

- radio path
- private-5G TCP
- private-5G MQTT

### Required Conditions

| Condition ID | Service | Transport | Label | Load | QoS | Mobility |
| --- | --- | --- | --- | --- | --- | --- |
| `radio-spat-baseline` | intersection-state | radio | `radio-baseline` | idle | default | stationary |
| `p5g-tcp-spat-baseline` | intersection-state | tcp | `private-5g-baseline` | idle | default | stationary |
| `p5g-mqtt-spat-baseline` | intersection-state | mqtt | `private-5g-baseline` | idle | default | stationary |
| `p5g-tcp-guided-baseline` | guided-planning | tcp | `private-5g-baseline` | idle | default | stationary |
| `p5g-mqtt-guided-baseline` | guided-planning | mqtt | `private-5g-baseline` | idle | default | stationary |

### Minimum Data to Collect

- per-probe logs
- sender CSV
- receiver CSV
- radio bridge CSV for radio runs
- summary `p50`, `p95`, `p99`
- success rate
- jitter
- per-hop timing when clocks are synchronized

### Results

| Condition ID | Samples | Success Rate | p50 ms | p95 ms | p99 ms | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `radio-spat-baseline` |  |  |  |  |  | Pending Mocar/radio run |
| `p5g-tcp-spat-baseline` | 1000 | 100.0% | 119.797 | 137.470 | 149.907 | Completed in `20260513_sunny_fintechparking_run_1` |
| `p5g-mqtt-spat-baseline` | 1000 | 100.0% | 23.356 | 34.114 | 39.656 | Completed in `20260513_sunny_fintechparking_run_1` |
| `p5g-tcp-guided-baseline` | 1000 | 100.0% | 119.065 | 137.970 | 185.428 | Completed using `p5g-tcp-service-payload-0` |
| `p5g-mqtt-guided-baseline` | 1000 | 100.0% | 23.154 | 36.620 | 43.797 | Completed using `p5g-mqtt-service-payload-0` |

### Figure Takeaway

- Which path has the best baseline tail behavior?
- Is MQTT meaningfully worse than TCP in your deployment?
- Does guided-planning stay within the envelope you want to claim?

## E2 Scaling and Contention

### Goal

Characterize fairness and tail behavior as you move from one physical client to
multiple physical plus emulated clients.

### Target Loads

- `N = 1`
- `N = 2`
- `N = 5`
- `N = 10`
- `N = 20`
- `N = 50`
- `N = 100`

### Required Conditions

| Condition ID | Service | Transport | Participants | Label | Load |
| --- | --- | --- | --- | --- | --- |
| `scale-guided-n1` | guided-planning | tcp or mqtt | 1 | `private-5g-baseline` | idle |
| `scale-guided-n2` | guided-planning | tcp or mqtt | 2 | `private-5g-stressed` | moderate |
| `scale-guided-n3` | guided-planning | tcp or mqtt | 3 | `private-5g-stressed` | heavy |
| `scale-guided-n6` | guided-planning | tcp or mqtt | 6 | `private-5g-stressed` | heavy |

### Minimum Data to Collect

- per-client `p95`
- max/min fairness ratio
- drops
- sender and receiver CPU
- NIC throughput
- queue depth if available

### Results

| Condition ID | Participants | Per-Client p95 ms | Fairness Ratio | Drop Rate | CPU | NIC | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `p5g-scale-tcp-payload-1024-clients-1` | 1 | 177.841-177.841 | 1.00 | 0.00% |  |  | p95 177.841 ms, run 2 |
| `p5g-scale-tcp-payload-1024-clients-20` | 20 | 215.920-228.003 | 1.06 | 0.00% |  |  | p95 221.684 ms, run 2 |
| `p5g-scale-tcp-payload-1024-clients-100` | 100 | 274.832-290.742 | 1.06 | 0.00% |  |  | p95 283.765 ms, run 3 |
| `p5g-scale-udp-payload-1024-clients-1` | 1 | 51.798-51.798 | 1.00 | 0.40% |  |  | p95 51.798 ms, run 2 |
| `p5g-scale-udp-payload-1024-clients-20` | 20 | 45.692-48.106 | 1.05 | 0.02% |  |  | p95 45.927 ms, run 2 |
| `p5g-scale-udp-payload-1024-clients-100` | 100 | 138.870-151.839 | 1.09 | 0.63% |  |  | p95 145.894 ms, run 3 |
| `p5g-scale-mqtt-payload-1024-clients-1` | 1 | 43.242-43.242 | 1.00 | 0.00% |  |  | p95 43.242 ms, run 2 |
| `p5g-scale-mqtt-payload-1024-clients-20` | 20 | 82.232-92.480 | 1.12 | 0.00% |  |  | p95 89.831 ms, run 2 |
| `p5g-scale-mqtt-payload-1024-clients-100` | 100 | 173.854-179.697 | 1.03 | 0.00% |  |  | p95 177.053 ms, run 3 |

### Figure Takeaway

- At what participant count do tails become unstable?
- Is fairness acceptable with one vendor stack plus emulated load?

## E3 Crowded-Network / QoS Stress Tests

### Goal

Show how high-priority AV services degrade as network load increases, and how
much QoS helps.

### Required Load Bands

- `idle`
- `moderate`
- `heavy`
- `near-saturation`

### Required Comparisons

- `default` or `fifo`
- `5qi-mapped` or priority-aware handling

### Required Conditions

| Condition ID | Service | Transport | Label | Load | QoS |
| --- | --- | --- | --- | --- | --- |
| `p5g-tcp-idle-default` | guided-planning | tcp | `private-5g-baseline` | idle | default |
| `p5g-tcp-heavy-default` | guided-planning | tcp | `private-5g-stressed` | heavy | default |
| `p5g-tcp-heavy-qos` | guided-planning | tcp | `private-5g-stressed` | heavy | `5qi-mapped` |
| `p5g-mqtt-idle-default` | guided-planning | mqtt | `private-5g-baseline` | idle | default |
| `p5g-mqtt-near-sat-fifo` | guided-planning | mqtt | `private-5g-stressed` | near-saturation | fifo |
| `p5g-mqtt-near-sat-qos` | guided-planning | mqtt | `private-5g-stressed` | near-saturation | `5qi-mapped` |

### Minimum Data to Collect

- `p50`, `p95`, `p99`
- success rate
- where service breaks
- throughput counters
- RSRP / SINR / portal counters if available

### Results

| Condition ID | Success Rate | p50 ms | p95 ms | p99 ms | Tail Growth vs Idle | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `p5g-tcp-idle-default` | 100.0% | 129.781 | 161.851 | 181.829 | 1.00x | Repeat in `20260701_load_qos_run_2` with 1024 B payload |
| `p5g-tcp-heavy-default` | 100.0% | 150.454 | 387.992 | 665.835 | 2.40x p95 | Uplink load target 25 Mbps, achieved about 2.27 Mbps client-side |
| `p5g-tcp-heavy-qos` |  |  |  |  |  |  |
| `p5g-mqtt-idle-default` | 100.0% | 35.281 | 52.063 | 67.788 | 1.00x | Repeat in `20260701_load_qos_run_2` with 1024 B payload |
| `p5g-mqtt-near-sat-fifo` | 100.0% | 116.110 | 175.905 | 477.905 | 3.38x p95 | Default QoS load-stress proxy only; target 25 Mbps, achieved about 2.27 Mbps client-side |
| `p5g-mqtt-near-sat-qos` |  |  |  |  |  |  |

Weak-signal stationary repeat:

| Condition Group | Success Rate | p95 RTT Range | Measured Load | GNSS | Notes |
| --- | --- | ---: | ---: | --- | --- |
| `p5g-tcp-weak-signal-default-*` | 100.0% | 668.347-933.823 ms | 0.000-0.262 Mbps client-side | mean `39.662931577, -75.757349646` | Idle plus 1/2/4 stream load sweep completed in `20260701_load_qos_weak_signal_run_1` |
| `p5g-mqtt-weak-signal-default-*` | 100.0% | 551.800-769.656 ms | 0.000-0.251 Mbps client-side | mean `39.662931577, -75.757349646` | Background load generators often timed out at weak-signal location |
| `p5g-tcp-weak-signal-qos-5qi-mapped-*` | 100.0% | 661.442-780.026 ms | 0.000-0.214 Mbps client-side | mean `39.662931577, -75.757349646` | QoS profile label only; core-side 5QI enforcement not independently verified |
| `p5g-mqtt-weak-signal-qos-5qi-mapped-*` | 100.0% | 702.193-773.772 ms | 0.175-0.322 Mbps client-side | mean `39.662931577, -75.757349646` | QoS profile label only; core-side 5QI enforcement not independently verified |

### Figure Takeaway

- Does QoS materially improve the high-priority service tail?
- Where does the service envelope break?
- What can you cautiously infer about public-network-like contention?

## E4 Edge-Offload vs RSU-Local Compute

Status: completed previously. This result is already covered by the existing
"Towards Collaborative Autonomous Driving" paper result from Yuankai He, so no
new Exp 10 data collection is needed for the current paper plan.

### Goal

Document whether edge offload helps or hurts the fallback / guided-planning
service relative to RSU-local handling using the existing covered result.

### Required Conditions

| Condition ID | Service | Placement | Label | Load |
| --- | --- | --- | --- | --- |
| `local-guided-idle` | guided-planning | RSU-local | `private-5g-baseline` | idle |
| `edge-guided-idle` | guided-planning | edge-offloaded | `private-5g-baseline` | idle |
| `local-guided-heavy` | guided-planning | RSU-local | `private-5g-stressed` | heavy |
| `edge-guided-heavy` | guided-planning | edge-offloaded | `private-5g-stressed` | heavy |

### Minimum Data to Collect

- end-to-end latency
- success rate
- degradation under edge load
- CPU split across RSU and edge

### Results

| Condition ID | Success Rate | p50 ms | p95 ms | p99 ms | CPU RSU | CPU Edge | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `local-guided-idle` |  |  |  |  |  |  |  |
| `edge-guided-idle` |  |  |  |  |  |  |  |
| `local-guided-heavy` |  |  |  |  |  |  |  |
| `edge-guided-heavy` |  |  |  |  |  |  |  |

### Figure Takeaway

- Is offload worth the added network path?
- Under what load does local compute beat edge offload?

## E5 Vehicle-Level Outcome

### Goal

Tie communication quality to actual AV behavior.

### Choose One or Two Primary Vehicle Metrics

- path deviation
- stop-line behavior
- fallback completion
- on-time guidance arrival

### Required Conditions

| Condition ID | Service | Network Condition | Vehicle Metric |
| --- | --- | --- | --- |
| `vehicle-guided-baseline` | guided-planning | baseline |  |
| `vehicle-guided-stressed` | guided-planning | stressed |  |
| `vehicle-state-baseline` | intersection-state | baseline |  |
| `vehicle-state-stressed` | intersection-state | stressed |  |

### Results

| Condition ID | On-Time Service Success | Vehicle Metric 1 | Vehicle Metric 2 | Notes |
| --- | --- | --- | --- | --- |
| `vehicle-guided-baseline` |  |  |  |  |
| `vehicle-guided-stressed` |  |  |  |  |
| `vehicle-state-baseline` |  |  |  |  |
| `vehicle-state-stressed` |  |  |  |  |

### Figure Takeaway

- Does degraded network quality produce degraded AV behavior?
- Is the effect large enough to justify the infrastructure claim?

## Supplementary Experiments

### Experiment 08 End-To-End Deadline Analysis

The deadline analysis uses RTT directly for private-5G request/response
services. A request is deadline-available only when `accepted == true` and
`rtt_ms <= deadline_ms`.

| Evidence group | Success Rate | p95 RTT ms | Miss @100 ms | Miss @500 ms | Miss @1000 ms | Notes |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| SPaT/state mirror | 100.00% | 130.323 | 46.60% | 0.00% | 0.00% | Use only as non-safety signal-state awareness over private-5G RTT |
| Compact service <=4 KiB, excluding E11 | 100.00% | 413.725 | 62.24% | 3.71% | 0.55% | Good 500 ms compact assistance envelope; not robust 100 ms safety envelope |
| E06 load/QoS 1 KiB | 100.00% | 602.354 | 75.88% | 6.53% | 0.96% | Conditional under load/weak-signal stress |
| E09 detector-output replay | 99.54% | 245.879 | 57.14% | 0.68% | 0.46% | Good 500 ms advisory/perception-aid envelope |
| E11 multiclient 1 KiB | 99.79% | 233.349 | 49.08% | 0.49% | 0.27% | Scales for 500 ms compact assistance; 100 ms depends on transport/client count |
| Map/perception 128-512 KiB | 100.00% | 667.975 | 99.01% | 11.64% | 0.73% | Treat as asynchronous/prefetch |
| Bulk >=1 MiB | 100.00% | 3122.273 | 100.00% | 100.00% | 62.61% | Bulk/backhaul only |

Summary: current private-5G RTT evidence supports 400-500 ms compact
infrastructure-assistance envelopes broadly, supports 100 ms only for compact
MQTT/UDP under favorable conditions, and does not support 10-25 ms tight
cooperative automation claims.

### S1 Payload Sensitivity

| Condition ID | Payload Size | Transport | p95 ms | Notes |
| --- | --- | --- | --- | --- |
| `payload-0p2kb` | 0.2 KB | TCP/MQTT | TCP 146.860; MQTT 37.980 | Completed, 1000/1000 accepted per transport in `20260522_cloudy_run_1` |
| `payload-1kb` | 1 KB | TCP/MQTT | TCP 155.301; MQTT 47.302 | Completed, 1000/1000 accepted per transport in `20260522_cloudy_run_1` |
| `payload-4kb` | 4 KB | TCP/MQTT | TCP 119.639; MQTT 47.683 | Completed, 1000/1000 accepted per transport in `20260522_cloudy_run_1` |

### S2 Failure Detection and Recovery

| Condition ID | Failure Type | Time to Detect | Time to Recover | Notes |
| --- | --- | --- | --- | --- |
| `failure-rsu` | RSU loss |  |  |  |
| `failure-edge` | edge loss |  |  |  |

### S3 Cold-Start / Session Resumption

| Condition ID | Resume Type | Registration Time | Resume Time | Notes |
| --- | --- | --- | --- | --- |
| `resume-cold-start` | cold start |  |  |  |
| `resume-transient-drop` | transient drop |  |  |  |

### S4 Security Overhead

| Condition ID | Security Mode | p95 ms | Overhead vs Plaintext | Notes |
| --- | --- | --- | --- | --- |
| `sec-plain` | plaintext |  |  |  |
| `sec-tls` | TLS / mTLS |  |  |  |

### S5 RF Gradient Sensitivity

| Condition ID | RF State | RSRP | SINR | p95 ms | Notes |
| --- | --- | --- | --- | --- | --- |
| `rf-near` | near |  |  |  |  |
| `rf-mid` | mid |  |  |  |  |
| `rf-far` | far |  |  |  |  |

### S6 Mobility / Handover

| Condition ID | Speed | Handover? | Latency Spike | Session Gap | Notes |
| --- | --- | --- | --- | --- | --- |
| `mobility-slow` |  |  |  |  |  |
| `mobility-fast` |  |  |  |  |  |

### S7 Queue-Discipline Ablations

| Condition ID | Queue Mode | p95 ms | p99 ms | Drop Rate | Notes |
| --- | --- | --- | --- | --- | --- |
| `queue-fifo` | FIFO |  |  |  |  |
| `queue-strict-priority` | strict priority |  |  |  |  |
| `queue-wfq` | WFQ |  |  |  |  |

## Run Log Index

Use this table to map raw files to conditions.

| Date | Run ID | Condition ID | Nodes Used | Log Files | Notes |
| --- | --- | --- | --- | --- | --- |
| 2026-05-13 | `edge4av-real-20260513-sunny-fintechparking-run-1` | `p5g-{tcp,mqtt}-{spat,service}-baseline` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260513_sunny_fintechparking_run_1/` | Private-5G baseline and SPaT/guided-service evidence |
| 2026-05-21 | `edge4av-real-20260521-small-rain-run-1` | `p5g-{tcp,mqtt}-service-payload-*` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260521_small_rain_run_1/` | Payload sensitivity replicate, stationary GNSS |
| 2026-05-22 | `edge4av-real-20260522-cloudy-run-1` | `p5g-{tcp,mqtt}-service-payload-*` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260522_cloudy_run_1/` | Payload sensitivity and TCP/MQTT transport comparison |
| 2026-06-29 | `v2x-radar-detector-benchmark-20260629T182624Z` | detector compute/output source | four local RTX 2080 Ti GPUs | `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/` | Detector workload completed for 922 samples |
| 2026-06-29 | `v2x-ipi-loopback-20260629T175556Z` | dataset-derived IPI loopback | local host | `results/v2x_benchmarks/v2x-ipi-loopback-20260629T175556Z/` | Local IPI payload transport mechanics |
| 2026-07-01 | `edge4av-stationary-goodfit-20260701T142538Z` | partial `transport-tcp-payload-1024` | vehicle host, edge `10.100.100.6` | `results/edge4av_top_tier/edge4av-stationary-goodfit-20260701T142538Z/` | Interrupted after noting existing experiment 07 evidence; partial TCP and GNSS retained |
| 2026-07-01 | `edge4av-real-20260701-load-qos-run-1` | `p5g-{tcp,mqtt}-loadqos-payload-1024-{idle,uplink-25mbps}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260701_load_qos_run_1/` | Experiment 06 default-load Phase A, stationary GNSS, 1000/1000 accepted per condition |
| 2026-07-01 | `edge4av-real-20260701-load-qos-run-2` | `p5g-{tcp,mqtt}-loadqos-payload-1024-{idle,uplink-25mbps}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260701_load_qos_run_2/` | Experiment 06 repeat with 64 KiB load packets, stationary GNSS, 1000/1000 accepted per condition |
| 2026-07-01 | `edge4av-real-20260701-load-qos-run-3` | `p5g-{tcp,mqtt}-loadqos-payload-1024-uplink-streams-{1,2,4}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260701_load_qos_run_3/` | Experiment 06 load sweep, stationary GNSS, measured aggregate load about 1.99-5.74 Mbps, 1000/1000 accepted per condition |
| 2026-07-01 | `edge4av-real-20260701-load-qos-run-4` | `p5g-{tcp,mqtt}-qos-5qi-mapped-payload-1024-uplink-streams-{1,2,4}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260701_load_qos_run_4/` | Experiment 06 non-default QoS-profile leg, `qos_profile=5qi-mapped`, stationary GNSS, 1000/1000 accepted per condition; network-side 5QI enforcement not independently verified |
| 2026-07-01 | `qos-verification-20260701` | TCP packet-marking verification | vehicle host `eno2`, edge `10.100.100.6`, gateway `10.100.100.1` | `results/real_5g/20260701_qos_verification_run_1/` | Successful default and `5qi-mapped` TCP probes both captured with `tos 0x0`; core-side QoS-flow counters still required for verified 5QI claim |
| 2026-07-01 | `edge4av-real-20260701-load-qos-weak-signal-run-1` | `p5g-{tcp,mqtt}-weak-signal-{default,qos-5qi-mapped}-payload-1024-{idle,uplink-streams-{1,2,4}}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260701_load_qos_weak_signal_run_1/` | Experiment 06 weak-signal stationary repeat with NovAtel GNSS; measured client load collapsed to about 0.00-0.322 Mbps, 1000/1000 accepted per condition |
| 2026-07-02 | `edge4av-real-20260702-detector-output-to-ipi-run-1` | `p5g-{tcp,mqtt}-detector-output-payload-*` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260702_detector_output_to_ipi_run_1/` | Experiment 09 detector-output replay over private-5G TCP/MQTT with NovAtel GNSS; payload sizes derived from detector output distribution; MQTT set pruned to avoid near-duplicate p99/max points |
| 2026-07-02 | `edge4av-real-20260702-detector-output-to-ipi-udp-fragmented-run-1` | `p5g-udp-detector-output-payload-*` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1/` | Experiment 09 detector-output replay over private-5G UDP with application-level fragmentation and NovAtel GNSS; main detector payloads 4096-23968 B achieved 99.5-100.0% message success; weak-signal repeat pending |
| 2026-07-02 | `edge4av-real-20260702-end-to-end-deadline-analysis-run-1` | `deadline-service-envelope-{10,25,100,120,400,500,1000,5000}ms` | completed private-5G sender CSVs | `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/` | Experiment 08 post-hoc deadline/service-envelope analysis; uses RTT directly for request/response services and treats threshold misses as unavailable replies |
| 2026-07-02 | `edge4av-real-20260702-multiclient-scalability-run-2` | `p5g-scale-{tcp,udp,mqtt}-payload-1024-clients-{1,2,5,10,20}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260702_multiclient_scalability_run_2/` | Experiment 11 stationary multiclient sweep with NovAtel GNSS; TCP/MQTT accepted 100.0%, UDP accepted 99.10-99.98% across base client levels |
| 2026-07-02 | `edge4av-real-20260702-multiclient-scalability-run-3` | `p5g-scale-{tcp,udp,mqtt}-payload-1024-clients-{50,100}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260702_multiclient_scalability_run_3/` | Experiment 11 high-client extension with NovAtel GNSS; TCP/MQTT accepted 100.0%, UDP accepted 98.97% at 50 clients and 99.37% at 100 clients |
| 2026-07-02 | `edge4av-real-20260702-multiclient-scalability-run-4` | `p5g-scale-{tcp,udp,mqtt}-payload-1024-clients-{1,2,5,10,20,50,100}` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260702_multiclient_scalability_run_4/` | Experiment 11 second stationary repeat with NovAtel GNSS; TCP/MQTT accepted 100.0%, UDP accepted 99.965-100.0% across client levels |
| 2026-07-03 | `edge4av-real-20260703-failure-fallback-run-1` | `p5g-failure-{receiver-restart-{tcp,udp,mqtt},broker-restart-mqtt}-payload-1024` | vehicle host, edge `10.100.100.6` | `results/real_5g/20260703_failure_fallback_run_1/` | Experiment 12 stationary failure/fallback subset with NovAtel GNSS; receiver-restart accepted TCP 95.3%, UDP 96.9%, MQTT 99.1%; MQTT broker-restart accepted 95.5% |
| 2026-07-04 | `20260704_exp_01_payload_sweep_0_2kb_164129` | `mocar-exp01-payload-0-2048-stationary-final-location` | Mocar OBU/RSU, NovAtel GNSS | `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_164129/` | Final Experiment 01 stationary Mocar V2X baseline run; 0 B completed as 1000/1000 timeouts, 256 B stopped after 332 timeout rows, and 256/512/1024/2048 B counted as timeout for analysis by operator instruction |
| 2026-07-04 | `edge4av-exp02-mobility-20260704T173816` | `mocar-exp02-mobility-route-los-moving-live-payload-256` | Mocar OBU/RSU, NovAtel GNSS | `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_173816/` | Experiment 02 mobility run 1; 675 rows before operator stop, 515 successes, 160 timeouts, p50 RTT 99.610 ms |
| 2026-07-04 | `edge4av-exp02-mobility-20260704T175820` | `mocar-exp02-mobility-route-los-moving-run2-payload-256` | Mocar OBU/RSU, NovAtel GNSS | `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_175820/` | Experiment 02 mobility run 2; 1000 rows, 731 successes, 269 timeouts, p50 RTT 28.326 ms |
| 2026-07-04 | `edge4av-exp02-mobility-20260704T182104` | `mocar-exp02-mobility-route-los-moving-run3-payload-256` | Mocar OBU/RSU, NovAtel GNSS | `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_182104/` | Experiment 02 mobility run 3 with corrected send-time GNSS join and 500 ms timeout; 1000 rows, 591 successes, 409 timeouts, p50 RTT 29.204 ms |
| 2026-07-04 | `edge4av-exp02-mobility-20260704T183232` | `mocar-exp02-mobility-route-los-moving-run4-payload-256` | Mocar OBU/RSU, NovAtel GNSS | `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_183232/` | Experiment 02 mobility run 4 with corrected send-time GNSS join and 500 ms timeout; 1000 rows, 716 successes, 284 timeouts, p50 RTT 28.276 ms |

## Paper Readiness Check

- [ ] Baseline envelope figure complete
- [ ] Scaling and fairness figure complete
- [ ] Crowded-network / QoS figure complete
- [x] Edge-offload vs local-compute figure complete
- [ ] Vehicle-level outcome figure complete
- [ ] One clear radio vs 5G comparison
- [ ] One clear QoS vs no-QoS comparison
- [x] One clear local vs offloaded comparison
- [ ] Main claim supported without over-claiming public-5G equivalence

## Final Claim Notes

Write the final paper claim here after the data is in:

> Private 5G plus edge-assisted intersection infrastructure can support
> ________________________________________________
> within _________________________________________
> under __________________________________________
> while crowded-network private-5G conditions act as a bounded proxy for
> ________________________________________________ .
