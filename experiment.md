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
- Figure 4: edge-offload vs RSU-local compute
- Figure 5: vehicle-level outcome under baseline vs stressed conditions

## Experiment Checklist

### Core Main-Paper Experiments

- [ ] `E1` Baseline service envelope
- [ ] `E2` Scaling and contention
- [ ] `E3` Crowded-network / QoS stress tests
- [ ] `E4` Edge-offload vs RSU-local compute
- [ ] `E5` Vehicle-level outcome

### Supplementary Experiments

- [ ] `S1` Payload sensitivity
- [ ] `S2` Failure detection and recovery
- [ ] `S3` Cold-start / session resumption
- [ ] `S4` Security overhead
- [ ] `S5` RF gradient sensitivity
- [ ] `S6` Mobility / handover
- [ ] `S7` Additional queue-discipline ablations

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
| `radio-spat-baseline` |  |  |  |  |  |  |
| `p5g-tcp-spat-baseline` |  |  |  |  |  |  |
| `p5g-mqtt-spat-baseline` |  |  |  |  |  |  |
| `p5g-tcp-guided-baseline` |  |  |  |  |  |  |
| `p5g-mqtt-guided-baseline` |  |  |  |  |  |  |

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
- `N = 3`
- `N = 6` using emulation to extend beyond the physical device count

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
| `scale-guided-n1` |  |  |  |  |  |  |  |
| `scale-guided-n2` |  |  |  |  |  |  |  |
| `scale-guided-n3` |  |  |  |  |  |  |  |
| `scale-guided-n6` |  |  |  |  |  |  |  |

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
| `p5g-tcp-idle-default` |  |  |  |  |  |  |
| `p5g-tcp-heavy-default` |  |  |  |  |  |  |
| `p5g-tcp-heavy-qos` |  |  |  |  |  |  |
| `p5g-mqtt-idle-default` |  |  |  |  |  |  |
| `p5g-mqtt-near-sat-fifo` |  |  |  |  |  |  |
| `p5g-mqtt-near-sat-qos` |  |  |  |  |  |  |

### Figure Takeaway

- Does QoS materially improve the high-priority service tail?
- Where does the service envelope break?
- What can you cautiously infer about public-network-like contention?

## E4 Edge-Offload vs RSU-Local Compute

### Goal

Prove whether edge offload helps or hurts the fallback / guided-planning
service relative to RSU-local handling.

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

### S1 Payload Sensitivity

| Condition ID | Payload Size | Transport | p95 ms | Notes |
| --- | --- | --- | --- | --- |
| `payload-0p2kb` | 0.2 KB |  |  |  |
| `payload-1kb` | 1 KB |  |  |  |
| `payload-4kb` | 4 KB |  |  |  |

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
|  |  |  |  |  |  |

## Paper Readiness Check

- [ ] Baseline envelope figure complete
- [ ] Scaling and fairness figure complete
- [ ] Crowded-network / QoS figure complete
- [ ] Edge-offload vs local-compute figure complete
- [ ] Vehicle-level outcome figure complete
- [ ] One clear radio vs 5G comparison
- [ ] One clear QoS vs no-QoS comparison
- [ ] One clear local vs offloaded comparison
- [ ] Main claim supported without over-claiming public-5G equivalence

## Final Claim Notes

Write the final paper claim here after the data is in:

> Private 5G plus edge-assisted intersection infrastructure can support
> ________________________________________________
> within _________________________________________
> under __________________________________________
> while crowded-network private-5G conditions act as a bounded proxy for
> ________________________________________________ .
