# Experiment 08 End-To-End Deadline Analysis

- Date: 2026-07-02
- Analysis type: post-hoc service-envelope analysis over completed private-5G sender CSVs
- Timing metric: request/response RTT from sender CSVs
- Pass rule used here: a request is deadline-available only when `accepted == true` and `rtt_ms <= deadline_ms`
- Miss rule: `deadline_miss = no accepted reply OR accepted RTT_ms > deadline_ms`
- Live/incomplete data excluded: `results/real_5g/20260702_multiclient_scalability_run_4/`

## Deadline Thresholds

These thresholds are service-envelope analysis points, not universal safety guarantees.

| Threshold | Figure label | Service interpretation |
| ---: | --- | --- |
| 10 ms | tight cooperative automation | Cooperative collision avoidance, high-automation maneuver coordination |
| 25 ms | strict cooperative maneuver | Strict CAV coordination / cooperative lane-change sensitivity |
| 100 ms | safety awareness / intersection warning | BSM/SPaT freshness, IMA, signal violation, VRU, abnormal vehicle, local hazard |
| 120 ms | emergency brake warning | Emergency electronic brake warning sensitivity |
| 400 ms | lane-change / blindspot advisory | Lane-change warning and low-speed blindspot advisory |
| 500 ms | compact infrastructure assistance | Guided-planning advisory, low-speed CAV fault fallback, service response |
| 1000 ms | prefetch / asynchronous advisory | Map fragment, object summary, low-speed fallback route segment |
| 5000 ms+ | route / backhaul / bulk | Route-level roadwork, traffic jam, cloud/backhaul, diagnostic transfer |

## Service-Envelope Summary

| Evidence group | Conditions | Attempts | Success % | p50 RTT ms | p95 RTT ms | p99 RTT ms | Miss @100 ms | Miss @120 ms | Miss @400 ms | Miss @500 ms | Miss @1000 ms | Miss @5000 ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| SPaT/state mirror | 2 | 2,000 | 100.00 | 58.748 | 130.323 | 140.479 | 46.60% | 21.50% | 0.00% | 0.00% | 0.00% | 0.00% |
| Compact service <=4 KiB, excluding E11 | 60 | 60,000 | 100.00 | 119.739 | 413.725 | 791.676 | 62.24% | 47.81% | 5.77% | 3.71% | 0.55% | 0.03% |
| E06 load/QoS 1 KiB | 34 | 34,000 | 100.00 | 139.163 | 602.354 | 990.812 | 75.88% | 65.93% | 10.15% | 6.53% | 0.96% | 0.05% |
| E09 detector-output replay | 21 | 20,100 | 99.54 | 108.039 | 245.879 | 349.732 | 57.14% | 35.11% | 0.95% | 0.68% | 0.46% | 0.46% |
| E11 multiclient 1 KiB | 21 | 564,000 | 99.79 | 97.832 | 233.349 | 298.077 | 49.08% | 40.97% | 0.60% | 0.49% | 0.27% | 0.21% |
| Mid payload 8-64 KiB | 41 | 40,100 | 99.78 | 83.265 | 199.793 | 303.647 | 29.63% | 16.05% | 0.48% | 0.34% | 0.22% | 0.22% |
| Map/perception 128-512 KiB | 18 | 18,000 | 100.00 | 227.193 | 667.975 | 946.023 | 99.01% | 92.55% | 18.67% | 11.64% | 0.73% | 0.01% |
| Bulk >=1 MiB | 14 | 14,000 | 100.00 | 1146.068 | 3122.273 | 3579.058 | 100.00% | 100.00% | 100.00% | 100.00% | 62.61% | 0.08% |

## Multiclient Deadline View

| Transport | Clients | Attempts | Success % | p50 ms | p95 ms | p99 ms | Miss @100 ms | Miss @120 ms | Miss @400 ms | Miss @500 ms | Miss @1000 ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 | 1,000 | 100.00 | 129.613 | 177.841 | 340.295 | 97.60% | 69.30% | 0.30% | 0.10% | 0.00% |
| TCP | 20 | 20,000 | 100.00 | 131.861 | 221.684 | 359.732 | 98.49% | 73.00% | 0.55% | 0.14% | 0.00% |
| TCP | 100 | 100,000 | 100.00 | 203.798 | 283.765 | 331.926 | 99.97% | 99.75% | 0.50% | 0.31% | 0.18% |
| UDP | 1 | 1,000 | 99.60 | 30.354 | 51.798 | 77.783 | 0.70% | 0.50% | 0.40% | 0.40% | 0.40% |
| UDP | 20 | 20,000 | 99.98 | 30.553 | 45.927 | 61.820 | 0.28% | 0.19% | 0.02% | 0.02% | 0.02% |
| UDP | 100 | 100,000 | 99.36 | 65.220 | 145.894 | 169.839 | 25.11% | 13.31% | 0.64% | 0.64% | 0.64% |
| MQTT | 1 | 1,000 | 100.00 | 30.446 | 43.242 | 46.308 | 0.00% | 0.00% | 0.00% | 0.00% | 0.00% |
| MQTT | 20 | 20,000 | 100.00 | 39.668 | 89.831 | 165.962 | 3.43% | 2.03% | 0.08% | 0.04% | 0.00% |
| MQTT | 100 | 100,000 | 100.00 | 114.053 | 177.053 | 547.738 | 61.21% | 44.63% | 1.39% | 1.19% | 0.12% |

## Payload Envelope

Baseline payload sweeps show the cutoff between compact deadline-sensitive services and asynchronous/prefetch payloads.

| Requested payload | TCP p95 ms | MQTT p95 ms | Main deadline interpretation |
| ---: | ---: | ---: | --- |
| 0 B | 141.499 | 39.724 | MQTT passes 100 ms; TCP fits 500 ms service-response framing |
| 1 KiB | 156.069 | 46.768 | Compact MQTT fits 100 ms; TCP fits 500 ms |
| 4 KiB | 119.639 | 59.905 | Compact advisory fits 500 ms and often 100 ms, especially MQTT |
| 16 KiB | 146.132 | 104.834 | Fits 500 ms; marginal for strict 100 ms |
| 64 KiB | 209.497 | 187.816 | Fits 500 ms; not 100 ms |
| 128 KiB | 321.223 | 309.048 | Fits 500 ms at p95 in baseline sweeps |
| 256 KiB | 697.092 | 962.579 | Treat as 1000 ms asynchronous/prefetch |
| 512 KiB | 2949.865 | 1031.485 | Conditional or unsupported for 1000 ms; prefetch only |
| 1 MiB | 1867.666 | 2702.546 | Seconds-scale bulk/prefetch only |
| 2 MiB | 3632.799 | 2536.359 | Seconds-scale bulk/prefetch only |

## Paper Interpretation

- Current private-5G request/response RTT data does not support 10 ms or 25 ms tight cooperative automation claims.
- 100 ms / 120 ms deadlines are only supported for compact MQTT/UDP under favorable conditions. They are not robust for TCP RTT, weak-signal/load stress, or high-client-count MQTT/TCP.
- 400 ms / 500 ms deadlines are strongly supported for compact infrastructure assistance, detector-output advisory, and most multiclient conditions.
- 1000 ms is appropriate for some larger asynchronous artifacts, but 512 KiB and larger payloads become conditional or unsupported depending on transport and radio condition.
- 5000 ms+ is the correct framing for route/backhaul/bulk diagnostics and rich perception transfer.

Recommended main-paper claim:

> Compact awareness and compact infrastructure-assistance services meet 400-500 ms service envelopes across the measured private-5G conditions, while 100 ms safety-warning envelopes require compact MQTT/UDP and favorable radio/load conditions. Large detector/map/perception artifacts should be handled as deadline-aware asynchronous or prefetched data rather than tight-loop control messages.

## Source Data

Completed folders included:

- `results/real_5g/20260513_sunny_fintechparking_run_1/`
- `results/real_5g/20260521_small_rain_run_1/`
- `results/real_5g/20260522_cloudy_run_1/`
- `results/real_5g/20260701_load_qos_run_1/`
- `results/real_5g/20260701_load_qos_run_2/`
- `results/real_5g/20260701_load_qos_run_3/`
- `results/real_5g/20260701_load_qos_run_4/`
- `results/real_5g/20260701_load_qos_weak_signal_run_1/`
- `results/real_5g/20260702_detector_output_to_ipi_run_1/`
- `results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1/`
- `results/real_5g/20260702_multiclient_scalability_run_2/`
- `results/real_5g/20260702_multiclient_scalability_run_3/`
