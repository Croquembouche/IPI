# Experiment Summary

Last updated: 2026-08-17.

This file summarizes the experiments currently present under `results/`. It is
based on the current result artifacts, not on prior generated paper prose.
Repository-facing CSV, JSON, log, GPS, and receiver artifacts remain under
`results/`. Exact GNSS, endpoint, hostname, and deployment-path copies from the
2026-08-05 and 2026-08-06 follow-up remain only in locally excluded backups.
At the user's explicit direction, the six 2026-08-15--16 raw-uplink trees and
the 2026-08-17 location-2 raw-uplink tree,
including exact GPS/deployment metadata where captured, are also committed
under `CISCO_AIRSPAN_STATS/`; their sanitized derivatives remain under
`results/real_5g/`.

## Inventory

| Result area | Scope | Current folders |
|---|---|---:|
| `results/real_5g/` | Private-5G TCP/MQTT/UDP latency, payload sweeps, load/QoS labels, detector-output replay, multiclient scaling, failure/fallback, deadline analysis, GPS, and signal maps | 37 dated run folders plus derived signal-map artifacts |
| `results/mocar_v2x/` | Mocar C-V2X setup, custom RTT, stationary payload sweeps, signal probe, and radio-distance/mobility runs | 17 run folders |
| `results/v2x_benchmarks/` | Public V2X dataset loopback, dataset/GPU processing, OpenCOOD smoke checks, and V2X-Radar detector benchmark runs | 9 result folders |
| `results/local_loopback/` | Local loopback placeholder | present but empty |

## Global Measurement Caveats

- Most private-5G runs mark `clock_sync_state=unsynced`. Treat RTT as the
  reliable latency metric. One-way uplink/downlink columns are retained as raw
  evidence only when clocks are not synchronized.
- MQTT results are MQTT 3.1.1 over TCP unless a UDP experiment explicitly says
  UDP.
- The `qos_profile=5qi-mapped` label is an application-side experiment label.
  The QoS verification run found no IP TOS/DSCP difference at the vehicle host,
  so the current evidence does not prove network-enforced 5QI behavior.
- UDP detector-output runs are sensitivity checks. Raw oversized datagrams and
  fragmented UDP have different failure behavior and should not be merged into
  a single reliable transport claim.
- Several Mocar payload sweeps were stopped at operator request after timeout-
  dominant behavior. Their summaries explicitly mark skipped or remaining
  attempts as analysis timeouts.

## Private-5G Experiments

### E01. Stationary Private-5G Baseline And Payload Sweeps

Primary folders:

- `results/real_5g/20260513_sunny_fintechparking_run_1/`
- `results/real_5g/20260514_sunny_after_rain_run_1/`
- `results/real_5g/20260515_sunny_run_1/`
- `results/real_5g/20260521_small_rain_run_1/`
- `results/real_5g/20260522_cloudy_run_1/`

Purpose: measure private-5G request/response RTT for compact SPaT-style and
`IPI-CooperativeService` payloads over TCP and MQTT under different days,
weather labels, and signal-strength conditions. The sender used 1000 probes per
completed condition with a 200 ms interval in the main stationary sweeps.

Main observations:

- On `20260513_sunny_fintechparking_run_1`, compact MQTT service payloads
  completed 1000/1000 attempts with p50 around 23-40 ms and p95 around 36-49
  ms for 0-4 KiB. Compact TCP service payloads completed 1000/1000 attempts
  with p50 around 80-120 ms and p95 around 100-140 ms for 0-4 KiB.
- The same run included larger payloads. At 2 MiB, TCP reached p50 3140 ms,
  p95 3633 ms, and p99 4053 ms; MQTT reached p50 2136 ms, p95 2536 ms, and
  p99 2815 ms. This separates compact request/response traffic from bulk
  transfer.
- `20260514_sunny_after_rain_run_1` repeated the payload sweep at a different
  location. Compact payloads still succeeded, but tails were larger. Examples:
  MQTT 0 B had p50 27 ms, p95 70 ms, p99 300 ms; TCP 0 B had p50 117 ms,
  p95 140 ms, p99 545 ms. Large payloads showed multi-second RTTs, including
  TCP 2 MiB p50 8322 ms and p95 10767 ms.
- `20260515_sunny_run_1` captured a much weaker or more variable path. Compact
  traffic still mostly succeeded, but tails grew sharply: TCP 0 B p95 1142 ms,
  MQTT 0 B p95 510 ms, MQTT 262 KiB p95 43836 ms, and some larger TCP/MQTT
  conditions had partial row counts.
- `20260521_small_rain_run_1` and `20260522_cloudy_run_1` are more stable
  later stationary repeats. In `20260522_cloudy_run_1`, MQTT 0-4 KiB p95 was
  about 38-48 ms and TCP 0-4 KiB p95 was about 120-155 ms; 1 MiB stayed in
  seconds-scale RTT for both transports.

Representative baseline rows:

| Folder | Condition | Attempts | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---|---|---:|---:|---:|---:|---:|
| `20260513_sunny_fintechparking_run_1` | MQTT 0 B service | 1000 | 1000 | 23.154 | 36.620 | 43.797 |
| `20260513_sunny_fintechparking_run_1` | TCP 0 B service | 1000 | 1000 | 119.065 | 137.970 | 185.428 |
| `20260513_sunny_fintechparking_run_1` | MQTT 2 MiB | 1000 | 1000 | 2136.365 | 2536.359 | 2814.849 |
| `20260513_sunny_fintechparking_run_1` | TCP 2 MiB | 1000 | 1000 | 3140.270 | 3632.799 | 4053.112 |
| `20260522_cloudy_run_1` | MQTT 4 KiB service | 1000 | 1000 | 39.776 | 47.683 | 51.751 |
| `20260522_cloudy_run_1` | TCP 4 KiB service | 1000 | 1000 | 79.715 | 119.639 | 122.151 |
| `20260515_sunny_run_1` | MQTT 256 KiB service | 1000 | 999 | 15152.536 | 43835.616 | 199428.746 |

### E02. Signal Maps And Run Locations

Primary artifacts:

- `results/real_5g/signal_maps_summary.md`
- `results/real_5g/signal_measurements_parsed.csv`
- `results/real_5g/run_locations_parsed.csv`
- generated SVG maps under `results/real_5g/`

Purpose: map private-5G signal context and relate run locations to RSRP, RSRQ,
and SNR survey points.

The user confirmed on 2026-08-12 that this 16-point survey was collected with
an iPhone. These values characterize the survey handset as a separate UE. They
provide spatial radio context but are not MG52 measurements, Airspan counters,
or per-request channel telemetry for the vehicle application path.

Measured signal summary:

| Metric | Samples | Min | Mean | Max |
|---|---:|---:|---:|---:|
| RSRP dBm | 16 | -121.00 | -106.88 | -91.00 |
| RSRQ dB | 16 | -19.00 | -11.00 | -10.00 |
| SNR dB | 16 | 3.50 | 16.44 | 27.50 |

The base-station marker is recorded at latitude 39.66729000 and longitude
-75.75751000. The signal-map summary records five run-location markers with
GNSS sample counts ranging from 531 to 13100 samples.

### E03. Load And QoS-Label Stress

Primary folders:

- `results/real_5g/20260701_load_qos_run_1/`
- `results/real_5g/20260701_load_qos_run_2/`
- `results/real_5g/20260701_load_qos_run_3/`
- `results/real_5g/20260701_load_qos_run_4/`
- `results/real_5g/20260701_load_qos_weak_signal_run_1/`
- `results/real_5g/20260706_load_qos_weak_signal_run_2/`
- `results/real_5g/20260701_qos_verification_run_1/`

Purpose: test 1 KiB private-5G request/response traffic under background uplink
load, multiple load streams, weak signal strength, and an application-level
`5qi-mapped` label.

Good-signal load results:

- `20260701_load_qos_run_1` and run 2 compare idle against a 25 Mbps uplink
  load. MQTT p95 increased from about 52-60 ms idle to 174-176 ms under load.
  TCP p95 increased from about 162-194 ms idle to 388-420 ms under load.
- `20260701_load_qos_run_3` varied 1, 2, and 4 uplink streams without the
  `5qi-mapped` label. MQTT p95 was 170 ms, 468 ms, and 182 ms. TCP p95 was
  391 ms, 402 ms, and 410 ms.
- `20260701_load_qos_run_4` repeated with `qos_profile=5qi-mapped`. MQTT p95
  was 377 ms, 487 ms, and 529 ms for 1, 2, and 4 streams. TCP p95 was 431 ms,
  680 ms, and 772 ms.

Weak-signal load results:

- `20260701_load_qos_weak_signal_run_1` showed high tails even at idle:
  MQTT idle p95 552 ms and TCP idle p95 668 ms. Under heavy load, MQTT p95
  ranged from 638 to 770 ms; TCP p95 ranged from 682 to 934 ms for default
  traffic.
- `20260706_load_qos_weak_signal_run_2` had improved idle tails but still high
  loaded tails. MQTT idle p95 was 160 ms, TCP idle p95 was 150 ms. Under load,
  MQTT default p95 was 460-708 ms and TCP default p95 was 630-759 ms.

QoS verification:

- `20260701_qos_verification_run_1` used `tcpdump` on vehicle interface `eno2`
  during one default and one `5qi-mapped` TCP probe.
- Both captures decoded all observed packets as `tos 0x0`.
- Current evidence supports saying that the application label was collected,
  but it does not prove host-side packet marking or network-enforced 5QI.

### E04. Detector-Output To IPI Replay

Primary folders:

- `results/real_5g/20260702_detector_output_to_ipi_run_1/`
- `results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1/`
- `results/real_5g/20260702_detector_output_to_ipi_udp_run_1/`
- `results/real_5g/20260702_detector_output_to_ipi_udp_run_2/`
- `results/real_5g/20260706_detector_output_to_ipi_weak_signal_tcp_mqtt_run_1/`
- `results/real_5g/20260706_detector_output_to_ipi_weak_signal_udp_fragmented_run_1/`

Purpose: replay detector-output-sized IPI payloads over the private-5G paths.
Payloads include compact controls, detector summary sizes around 19-25 KiB, and
60 KiB upper-bound conditions.

Good-signal TCP/MQTT results from `20260702_detector_output_to_ipi_run_1`:

| Transport | Payload | Attempts | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---|---:|---:|---:|---:|---:|---:|
| MQTT | 0 B | 1000 | 1000 | 27.023 | 49.826 | 65.177 |
| MQTT | 23-24 KiB | 1000 | 1000 | 111.672 | 183.705 | 209.486 |
| MQTT | 60 KiB | 1000 | 1000 | 243.656 | 403.790 | 681.227 |
| TCP | 0 B | 1000 | 1000 | 120.004 | 151.696 | 185.130 |
| TCP | 23-25 KiB | 1000 | 1000 | 111.538-134.948 | 169.540-213.894 | 203.890-275.649 |
| TCP | 60 KiB | 1000 | 1000 | 240.056 | 381.409 | 659.838 |

UDP results:

- `20260702_detector_output_to_ipi_udp_run_1` is explicitly aborted diagnostic
  evidence. Payload 0 completed, but payload 4096 repeatedly timed out and the
  19648/22816 byte files were empty or partial.
- `20260702_detector_output_to_ipi_udp_run_2` showed raw UDP worked for 0 and
  1024 B, but 1400 B, 4096 B, and 19648 B had 0 successful replies.
- `20260702_detector_output_to_ipi_udp_fragmented_run_1` used a fragmented UDP
  path. Payloads 0-23968 B had 99.5-100.0% success, but 60 KiB had 20/100
  success.

Weak-signal detector replay:

- TCP/MQTT remained 100% successful in
  `20260706_detector_output_to_ipi_weak_signal_tcp_mqtt_run_1`, but tails grew.
  At 60 KiB, MQTT p95 was 770 ms and p99 was 1230 ms; TCP p95 was 662 ms and
  p99 was 1027 ms.
- Fragmented UDP in
  `20260706_detector_output_to_ipi_weak_signal_udp_fragmented_run_1` had
  99.9-100% success for 0, 256, 1024, and 4096 B; 19-25 KiB payloads had
  93.5-96.1% success; 60 KiB had 0/100 success.

### E05. End-To-End Deadline Analysis

Primary folder:

- `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/`

Purpose: perform a post-hoc service-deadline analysis over completed private-5G
sender CSVs. The analysis uses sender RTT and marks a request as available only
when it was accepted and `rtt_ms <= deadline_ms`.

Key group results:

| Evidence group | Attempts | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms | Miss @100 ms | Miss @500 ms | Miss @1000 ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| SPaT/state mirror | 2000 | 100.00% | 58.748 | 130.323 | 140.479 | 46.60% | 0.00% | 0.00% |
| Compact service <=4 KiB, excluding E11 | 60000 | 100.00% | 119.739 | 413.725 | 791.676 | 62.24% | 3.71% | 0.55% |
| E06 load/QoS 1 KiB | 34000 | 100.00% | 139.163 | 602.354 | 990.812 | 75.88% | 6.53% | 0.96% |
| E09 detector-output replay | 20100 | 99.54% | 108.039 | 245.879 | 349.732 | 57.14% | 0.68% | 0.46% |
| E11 multiclient 1 KiB | 564000 | 99.79% | 97.832 | 233.349 | 298.077 | 49.08% | 0.49% | 0.27% |
| Mid payload 8-64 KiB | 40100 | 99.78% | 83.265 | 199.793 | 303.647 | 29.63% | 0.34% | 0.22% |
| Map/perception 128-512 KiB | 18000 | 100.00% | 227.193 | 667.975 | 946.023 | 99.01% | 11.64% | 0.73% |
| Bulk >=1 MiB | 14000 | 100.00% | 1146.068 | 3122.273 | 3579.058 | 100.00% | 100.00% | 62.61% |

Interpretation from the stored analysis: current private-5G request/response
RTT does not support 10 ms or 25 ms tight cooperative automation claims.
Compact services are much more plausible at 400-500 ms than at 100 ms, and
large detector/map/perception payloads belong in asynchronous or prefetch
framing.

### E06. Multiclient Scalability

Primary folders:

- `results/real_5g/20260702_multiclient_scalability_run_1/`
- `results/real_5g/20260702_multiclient_scalability_run_2/`
- `results/real_5g/20260702_multiclient_scalability_run_3/`
- `results/real_5g/20260702_multiclient_scalability_run_4/`
- `results/real_5g/20260706_multiclient_scalability_weak_signal_run_1/`

Purpose: emulate multiple vehicles sending 1 KiB private-5G requests, with
client counts from 1 to 100 over TCP, MQTT, and UDP.

Run status:

- `20260702_multiclient_scalability_run_1` was aborted during startup after a
  receiver build/link issue; use later run folders for clean data.
- `20260702_multiclient_scalability_run_2` covers 1, 2, 5, 10, and 20 clients.
- `20260702_multiclient_scalability_run_3` covers 50 and 100 clients.
- `20260702_multiclient_scalability_run_4` covers 1 through 100 clients in a
  cleaner full sweep.
- `20260706_multiclient_scalability_weak_signal_run_1` repeats 1 through 100
  clients under weak signal strength.

Good-signal examples:

| Folder | Transport | Clients | Rows | Success | Median condition p50 ms | Median condition p95 ms | Median condition p99 ms |
|---|---|---:|---:|---:|---:|---:|---:|
| `20260702_multiclient_scalability_run_2` | MQTT | 20 | 20000 | 20000 | 39.658 | 89.857 | 150.457 |
| `20260702_multiclient_scalability_run_2` | TCP | 20 | 20000 | 20000 | 131.856 | 221.672 | 358.833 |
| `20260702_multiclient_scalability_run_2` | UDP | 20 | 20000 | 19997 | 30.776 | 45.927 | 61.520 |
| `20260702_multiclient_scalability_run_3` | MQTT | 100 | 100000 | 100000 | 114.125 | 176.239 | 526.698 |
| `20260702_multiclient_scalability_run_3` | TCP | 100 | 100000 | 100000 | 203.807 | 283.521 | 329.811 |
| `20260702_multiclient_scalability_run_3` | UDP | 100 | 100000 | 99365 | 65.526 | 145.953 | 169.719 |
| `20260702_multiclient_scalability_run_4` | MQTT | 100 | 100000 | 100000 | 41.776 | 81.835 | 119.849 |
| `20260702_multiclient_scalability_run_4` | TCP | 100 | 100000 | 100000 | 187.678 | 263.984 | 304.022 |
| `20260702_multiclient_scalability_run_4` | UDP | 100 | 100000 | 99965 | 46.206 | 112.121 | 139.776 |

Weak-signal examples:

| Transport | Clients | Rows | Success | Median condition p50 ms | Median condition p95 ms | Median condition p99 ms |
|---|---:|---:|---:|---:|---:|---:|
| MQTT | 50 | 50000 | 49998 | 173.885 | 1115.787 | 1937.951 |
| MQTT | 100 | 100000 | 99499 | 851.780 | 3814.102 | 13149.421 |
| TCP | 50 | 50000 | 50000 | 316.839 | 771.208 | 1539.818 |
| TCP | 100 | 100000 | 100000 | 410.177 | 2938.408 | 6400.146 |
| UDP | 50 | 50000 | 48364 | 83.609 | 169.862 | 180.218 |
| UDP | 100 | 100000 | 90811 | 103.532 | 172.519 | 181.779 |

The weak-signal run is the clearest multiclient stress evidence: MQTT and TCP
tail latency grows sharply at 50-100 clients, while UDP loses more replies as
client count increases.

### E07. Failure And Fallback

Primary folder:

- `results/real_5g/20260703_failure_fallback_run_1/`

Purpose: inject receiver or broker restart failures while sending 1 KiB
requests over TCP, MQTT, and UDP.

| Condition | Attempts | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---|---:|---:|---:|---:|---:|
| MQTT broker restart | 1000 | 955 | 31.644 | 47.871 | 90.553 |
| MQTT receiver restart | 1000 | 991 | 31.977 | 49.855 | 259.906 |
| TCP receiver restart | 1000 | 953 | 126.629 | 161.464 | 361.228 |
| UDP receiver restart | 1000 | 969 | 30.305 | 44.139 | 63.027 |

These runs show that restart/failure events mainly appear as missed requests
and tail spikes rather than median shifts.

### E08. Airspan `70/20/10` Validation And Multi-Location Follow-Up

**TDD inference status: inconclusive.** The retained measurements remain valid
for their recorded direction, workload, placement, and signal contexts. They
must not be used to rank `40/40/20` against `70/20/10`, estimate a TDD effect,
or claim TDD sensitivity. The profile and vehicle placement/signal context
changed together, and the `70/20/10` direction control has no matched
`40/40/20` control. This status is recorded in
`results/real_5g/tdd_comparison_status.json` and remains in force until a new
same-placement matched comparison is validated.

Primary folders:

- `results/real_5g/20260805_airspan_r1_run_1/`
- `results/real_5g/20260806_airspan_followup_run_1/`
- `results/real_5g/20260806_airspan_followup_weak_run_1/`
- `results/real_5g/20260806_airspan_followup_common_typical_run_2/`
- `results/real_5g/20260806_airspan_followup_strong_run_2/`

Purpose: collect host-side direction controls and repeated C1/C2 application
workloads at multiple stationary vehicle locations under the operator-verified
Cell 2 lock, operator-reported `70/20/10` TDD profile, and unchanged `10D4G`
frame packing. The
MG52 remained on the trunk floor with its front face upward. Every application
transport occupied a separate five-minute ACP interval, used 500 sequential
RTT probes per repetition, and retained `clock_sync_state=unsynced`; one-way
latency is therefore excluded.

The R1 host-side direction controls passed artifact validation. V2 was a
car-to-edge TCP stream from 13:15-13:30 EDT and transferred 1,475,758,800 bytes
at 13.118 Mbps. TCP backpressure prevented it from reaching the configured
25 Mbps target. V3 was an edge-to-car TCP stream from 13:35-13:50 EDT and
transferred 2,812,500,000 bytes at the configured 25 Mbps. V1 is the operator-
designated 11:15-11:30 EDT idle window and has no host telemetry. The user has
verified the MG52 Cell 2 lock. R1 remains pending only for claims requiring the
matching ACP artifacts to validate the idle state, directional cell counters,
Cell 2 traffic attribution, and live radio configuration.

Application-block validation:

| Location block | Conditions | Valid samples | Attempts | Accepted | Failed | Success |
|---|---|---:|---:|---:|---:|---:|
| Medium/typical | C1-C4 | 24/24 | 309000 | 307662 | 1338 | 99.566990% |
| Weak candidate | C1-C2 | 12/12 | 6000 | 5994 | 6 | 99.900000% |
| Common/typical repeat | C1-C2 | 12/12 | 6000 | 5999 | 1 | 99.983333% |
| Strong candidate | C1-C2 | 12/12 | 6000 | 6000 | 0 | 100.000000% |

The medium block includes the offered-uplink C3 workload and the 100-client C4
workload. C3 produced five UDP timeouts. C4 TCP and MQTT accepted all 100,000
attempts per transport, while C4 UDP accepted 98,667/100,000. The weak block's
six failures and the common repeat's one failure are all C2 UDP acknowledgment
timeouts. All failed rows remain in the result artifacts rather than being
replaced by retries.

The operator reported RSRP values of 109-110 for the original and repeated
common/typical blocks and 101 for the strong candidate. On 2026-08-12, the user
clarified that these readings came from an iPhone, not from the MG52. They are
therefore separate-UE spatial context. Using the
two common C1/C2 blocks as one descriptive reference distribution, the strong
candidate had lower matched RTT percentiles in every condition/transport
aggregate:

| Workload | Transport | Common p50 ms | Strong p50 ms | p50 change | Common p95 ms | Strong p95 ms | p95 change |
|---|---|---:|---:|---:|---:|---:|---:|
| C1 | TCP | 123.729 | 117.863 | -4.74% | 149.913 | 137.309 | -8.41% |
| C1 | MQTT | 30.428 | 28.016 | -7.93% | 41.632 | 39.955 | -4.03% |
| C1 | UDP | 29.706 | 26.632 | -10.35% | 40.347 | 39.200 | -2.84% |
| C2 | TCP | 85.477 | 75.388 | -11.80% | 106.971 | 93.605 | -12.49% |
| C2 | MQTT | 59.790 | 49.753 | -16.79% | 75.752 | 59.721 | -21.16% |
| C2 | UDP | 56.623 | 47.947 | -15.32% | 70.541 | 56.947 | -19.27% |

The application-layer observations from the historical `40/40/20` campaign
and the `70/20/10` follow-up are retained below for data provenance. The same
sole CAV UE, dedicated radio and channel, Cell 2 path, and `10D4G` packing were
retained across collection times, while the TDD allocation and vehicle
location changed. Consequently, the table is inconclusive for TDD inference.
Each cell reports p95 RTT followed by the percentage of all issued requests
answered by 100 ms. The common/typical row pools both retained common blocks.

| TDD profile and signal/placement context | 1 KiB MQTT | 1 KiB TCP | 23,968 B MQTT | 23,968 B TCP |
|---|---:|---:|---:|---:|
| `40/40/20`, favorable | 44.487 ms / 100.0% | 140.249 ms / 4.0% | 183.705 ms / 30.8% | 169.540 ms / 18.4% |
| `70/20/10`, common/typical | 41.632 ms / 99.8% | 149.913 ms / 1.0% | 75.752 ms / 99.4% | 106.971 ms / 86.3% |
| `70/20/10`, weak | 46.106 ms / 99.1% | 150.429 ms / 0.5% | 162.864 ms / 4.4% | 188.673 ms / 0.0% |
| `70/20/10`, strong | 39.955 ms / 100.0% | 137.309 ms / 4.6% | 59.721 ms / 99.7% | 93.605 ms / 97.8% |

The 1-KiB rows are comparatively stable within each transport. The 23,968-B
rows vary sharply with signal/placement under the same `70/20/10` profile.
These observations do not identify the effect of TDD allocation. The retained
cross-profile data are therefore classified as inconclusive for TDD inference.

This supports an application-layer association between the strong-candidate
location and lower RTT, especially for C2. It does not yet establish that RSRP
alone caused the change: the field blocks were sequential, contain two
repetitions per transport, and use iPhone rather than MG52 radio observations.
The Cell 2 lock is operator-verified, while aligned ACP exports remain necessary
for contemporaneous cell-load or TDD claims. Repository-facing GNSS coordinates
are rounded to 0.001 degree and
serialized rosbags are omitted; the exact-data backups remain excluded from
version control.

### E09. 2026-08-15 Raw-Byte Uplink Payload Sweep

Primary folder:

- `results/real_5g/20260815_airspan_tdd_raw_uplink_70_20_10_location_1_run_2/`

Purpose: measure uplink-oriented request/application-acknowledgment RTT over
TCP and MQTT for exact 1 KiB, 10 KiB, 100 KiB, 1,024 KiB, and 2,048 KiB raw
application bodies. Each condition used 1,000 sequential attempts and a 200 ms
post-completion interval. The edge validated the exact request length and CRC32
before returning a compact correlated application acknowledgment.

All 10 conditions passed: 10,000 attempts, 10,000 accepted responses, 10,000
matching receiver rows, and zero failures. The sender used a monotonic RTT from
immediately before framed transmission through receipt and structural parsing
of the complete application acknowledgment. Subsequent local topic, sequence,
accepted-state, length, and CRC32 comparisons also passed for every row but are
outside the measured interval.

| Transport | Payload | p50 RTT ms | p95 RTT ms | p99 RTT ms | Max RTT ms | Application goodput Mbps |
|---|---:|---:|---:|---:|---:|---:|
| TCP | 1 KiB | 31.639 | 41.721 | 47.781 | 91.669 | 0.035 |
| TCP | 10 KiB | 39.596 | 49.082 | 53.815 | 83.748 | 0.342 |
| TCP | 100 KiB | 101.290 | 111.503 | 119.386 | 151.802 | 2.710 |
| TCP | 1,024 KiB | 770.485 | 818.754 | 878.972 | 1,580.884 | 8.619 |
| TCP | 2,048 KiB | 1,500.456 | 1,584.062 | 1,628.938 | 2,263.905 | 9.847 |
| MQTT | 1 KiB | 29.982 | 40.476 | 44.539 | 60.608 | 0.036 |
| MQTT | 10 KiB | 39.573 | 50.225 | 55.383 | 83.142 | 0.340 |
| MQTT | 100 KiB | 99.473 | 111.174 | 127.308 | 161.320 | 2.716 |
| MQTT | 1,024 KiB | 767.345 | 810.505 | 868.133 | 1,328.069 | 8.639 |
| MQTT | 2,048 KiB | 1,501.583 | 1,591.377 | 1,682.788 | 1,949.352 | 9.820 |

The operator reported `70/20/10`, serving Cell 2 with no handoff, RSRP
`-100 dBm`, and RSRQ `-13 dB`. The user later clarified that Airspan Cell 1
and Cell 2 were both administratively unlocked and broadcasting. Administrative
`locked` means the cell is not broadcasting and is distinct from serving-cell
attachment. The user corrected the original `-97 dBm` RSRP
entry after acquisition. These values remain operator-reported until
timestamp-aligned ACP/configuration and MG52 exports are stored. The 59.585-
second ROS 2 bag passed the stationary gate. The explicitly committed raw tree
retains the exact coordinates and bag; the public result derivative retains
only `39.664, -75.757` at 0.001-degree precision.

Payload labels describe application objects, not individual IP packets. TCP
segments the object as needed, and MQTT adds MQTT framing over TCP. Application
goodput is accepted payload divided by the same-host span from first request
transmission to final acknowledgment; interface rates include protocol and
incidental traffic. Endpoint clocks were unsynchronized, so one-way metrics
are excluded.

A read-only Git integrity scan overlapped TCP 2 MiB sequences 97-116 for 22.7
seconds. The affected rows remain included; host telemetry shows no saturation
or failure, and the final condition maximum occurred at sequence 613 outside
the overlap. All 95 copied edge files matched their remote hashes, both raw and
public checksum manifests pass, and the public derivative passes its privacy
scan. This one-location, one-profile block is not a causal TDD comparison and
does not replace the planned two-location `40/40/20` versus `70/20/10`
directional matrix.

### E10. 2026-08-16 MQTT Same-Profile Uplink Repeat

Primary folder:

- `results/real_5g/20260816_airspan_tdd_raw_uplink_40_40_20_location_1_mqtt_repeat_run_1/`

Purpose: repeat the prior day's MQTT raw-byte uplink experiment after the
private-5G route resumed. The operator reported the same `40/40/20` profile,
RSRP `-100 dBm`, RSRQ `-13 dB`, serving Cell 2/no handoff, and stationary
`location_1`. Each retained condition used 1,000 sequential requests and the
same 200 ms post-completion interval.

| Payload | Attempts | Accepted | Receiver rows | p50 ms | p95 ms | p99 ms | Max ms |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 KiB | 1,000 | 1,000 | 1,000 | 39.435 | 51.016 | 279.632 | 299.649 |
| 10 KiB | 1,000 | 1,000 | 1,000 | 63.682 | 169.247 | 263.223 | 539.404 |
| 100 KiB | 1,000 | 1,000 | 1,000 | 439.006 | 746.947 | 927.222 | 34,967.951 |
| 1,024 KiB | 1,000 | 1,000 | 1,000 | 4,549.326 | 5,608.034 | 6,193.954 | 6,700.886 |

All four retained conditions passed exact request-length, CRC32, sequence, and
application-acknowledgment validation with zero failures. The 100 KiB maximum
is a retained accepted outlier at sequence 19; the next-largest RTT in that
condition is 1,191.193 ms. The fresh 59.570-second ROS 2 bag contains 16,087
messages and passes the stationarity gate.

Relative to the valid 2026-08-15 `40/40/20` MQTT rows, p50 RTT changed by
+2.36% at 1 KiB, +10.04% at 10 KiB, +102.71% at 100 KiB, and +256.86% at
1 MiB. The corresponding p95 changes are +9.22%, +60.52%, +111.27%, and
+163.86%. Thus this repeat directly shows materially worse large-object RTT on
2026-08-16. It does not identify the cause: the runs are on different days and
lack timestamp-aligned ACP, MG52, and cell-load evidence.

At the user's direction, the 2 MiB condition stopped after 133/1,000 validated
sender/receiver pairs. It is preserved in the raw and public result trees but
excluded from totals and comparisons. Future payload sweeps stop at 1,024 KiB
(1 MiB); 2 MiB is no longer part of the planned matrix. All 50 fetched edge
files matched their remote copies before redaction, both checksum manifests
pass, the repository measurement artifacts match the raw copies, and the
public derivative contains no exact GPS bag/CSV or deployment identifiers.

### E11. 2026-08-16 Quick TCP Same-Profile Uplink Follow-Up

Primary folder:

- `results/real_5g/20260816_airspan_tdd_raw_uplink_40_40_20_location_1_tcp_quick_run_1/`

Purpose: quickly measure TCP application-acknowledgment RTT at 1 KiB, 10 KiB,
and 100 KiB after the MQTT repeat. The operator-reported context remained
`40/40/20`, RSRP `-100 dBm`, RSRQ `-13 dB`, Cell 1 administratively locked
and not broadcasting, and Cell 2 serving with no handoff. Cell 2's
broadcasting/unlocked state is inferred from the completed serving-cell traffic
at stationary `location_1`. Each condition used 1,000 sequential exact raw
application objects and the established 200 ms post-completion interval. The
validated same-day 59.570-second GPS capture was reused; no new bag was
collected for this quick follow-up.

The administrative and serving-cell parameters for the four TCP blocks in
the side-by-side comparison are:

| Date/run | TDD | Cell 1 administrative state | Cell 2 administrative state | Serving cell | RSRP | RSRQ |
|---|---|---|---|---:|---:|---:|
| 2026-08-15 `70/20/10` | `70/20/10` | Unlocked; broadcasting | Unlocked; broadcasting | 2; MG52-selected/no handoff | -100 dBm | -13 dB |
| 2026-08-15 `40/40/20` | `40/40/20` | Unlocked; broadcasting | Unlocked; broadcasting | 2; MG52-selected/no handoff | -100 dBm | -13 dB |
| 2026-08-16 run 1 | `40/40/20` | Locked; not broadcasting | Unlocked; broadcasting (inferred) | 2; MG52-selected/no handoff | -100 dBm | -13 dB |
| 2026-08-16 run 2 | `40/40/20` | Unlocked; broadcasting | Unlocked; broadcasting | 1; MG52-selected/no handoff | -98 dBm | -13 dB; carried forward |

| Payload | Attempts | Accepted | Receiver rows | p50 ms | p95 ms | p99 ms | Max ms |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 KiB | 1,000 | 1,000 | 1,000 | 39.634 | 51.548 | 77.657 | 359.632 |
| 10 KiB | 1,000 | 1,000 | 1,000 | 57.519 | 91.421 | 211.822 | 465.504 |
| 100 KiB | 1,000 | 1,000 | 1,000 | 441.096 | 789.094 | 937.482 | 1,120.317 |

Relative to the valid 2026-08-15 same-profile TCP subset, p50/p95 changed by
+0.52%/+3.33% at 1 KiB, +11.38%/+27.97% at 10 KiB, and
+132.55%/+169.05% at 100 KiB. The new 100 KiB distribution is therefore
materially slower, but the evidence does not identify the cause: the runs lack
timestamp-aligned ACP, MG52, scheduler, and cell-load evidence. It also differs
in Cell 1 administrative state: Cell 1 broadcast on 2026-08-15 but did not
broadcast on 2026-08-16.

All 3,000 requests passed exact length, CRC32, sequence, and compact
application-acknowledgment validation with zero failures. All 21 fetched edge
files match their remote copies, raw and public checksums pass, retained
measurement files are identical across the raw/public boundary, and the public
privacy scan passes. The temporary edge authorization was revoked and its
local key files were deleted. Each condition manifest and application-summary
row records TDD `40/40/20`, RSRP `-100 dBm`, and RSRQ `-13 dB`.

### E12. 2026-08-16 Quick TCP Uplink Follow-Up — Run 2

Primary folder:

- `results/real_5g/20260816_airspan_tdd_raw_uplink_40_40_20_location_1_tcp_quick_run_2/`

Purpose: repeat the three-size quick TCP sweep after the user reported RSRP
`-98 dBm` and both Airspan cells administratively unlocked and broadcasting.
The user also reported TDD `40/40/20`. RSRQ `-13 dB` was carried forward from
the latest operator report, not freshly measured. The user later clarified
that the MG52 was locked onto serving Cell 1 with no reported handoff. The
earlier carried-forward Cell 2 entry is superseded, but the corrected selection
is not verified by a timestamp-aligned MG52 export. The other three TCP runs
in the side-by-side comparison were locked onto Cell 2.

The raw and repository-facing trees preserve this clarification in
`serving_cell_selection_correction.json`. Acquisition-time condition manifests
retain their original Cell 2 value as provenance; corrected operator context
and derived analysis rows use Cell 1. No application RTT, sender/receiver row,
payload, or telemetry sample changed.

Each condition used 1,000 sequential exact raw application objects and the
same 200 ms post-completion interval. All 3,000 requests passed application-
acknowledgment, exact-length, CRC32, sequence, and sender/receiver correlation
validation with zero failures:

| Payload | Accepted/attempts | p50 ms | p95 ms | p99 ms | Max ms | Miss @500 ms | Miss @1,000 ms |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 KiB | 1,000/1,000 | 39.676 | 149.399 | 363.057 | 642.942 | 1 | 0 |
| 10 KiB | 1,000/1,000 | 65.491 | 187.382 | 339.494 | 660.564 | 2 | 0 |
| 100 KiB | 1,000/1,000 | 508.809 | 927.334 | 1,209.344 | 4,085.308 | 523 | 33 |

Relative to the first same-day quick TCP run, the second run changed as follows:

| Payload | Run 1 p50 ms | Run 2 p50 ms | p50 change | Run 1 p95 ms | Run 2 p95 ms | p95 change |
|---:|---:|---:|---:|---:|---:|---:|
| 1 KiB | 39.634 | 39.676 | +0.11% | 51.548 | 149.399 | +189.83% |
| 10 KiB | 57.519 | 65.491 | +13.86% | 91.421 | 187.382 | +104.97% |
| 100 KiB | 441.096 | 508.809 | +15.35% | 789.094 | 927.334 | +17.52% |

The 100 KiB maximum occurred at sequence 840. It passed exact sender/receiver
sequence, length, and CRC32 correlation and remains unfiltered; its cause is
not established. Car and edge CPU stayed below 10% condition-level busy time,
memory remained stable, and neither interface recorded an error or drop delta.

The run reused the earlier same-day 59.570-second stationary GPS evidence; no
new bag was collected, and continuity with the application run was not
independently reverified. The two quick runs differ in reported RSRP and Cell 1
administrative state, and they used different serving cells. Neither has
aligned ACP/MG52 or cell-load evidence. They therefore do not isolate the
effect of RSRP, Cell 1 broadcasting, serving-cell selection, or TDD.

All 21 fetched edge files matched their remote copies before access revocation.
The independent analyzer, raw/public checksums, JSON/CSV parsing, retained-
measurement identity, and repository privacy scan pass. The temporary edge key
was revoked and deleted.

### E13. 2026-08-16 Quick TCP `70/20/10` Repeat

Primary folder:

- `results/real_5g/20260816_airspan_tdd_raw_uplink_70_20_10_location_1_tcp_quick_run_1/`

Purpose: repeat the 1 KiB, 10 KiB, and 100 KiB TCP raw-object measurements
under reported TDD `70/20/10` and compare them with the first complete
2026-08-15 `70/20/10` TCP block. The user reported the MG52 locked onto serving
Cell 2 with no handoff and Airspan Cell 1 administratively locked/not
broadcasting. Cell 2 was unlocked/broadcasting. RSRP `-98 dBm` and RSRQ
`-13 dB` were carried forward from the most recent report.

| Payload | Accepted/attempts | p50 ms | p95 ms | p99 ms | Max ms |
|---:|---:|---:|---:|---:|---:|
| 1 KiB | 1,000/1,000 | 31.583 | 41.975 | 49.638 | 101.337 |
| 10 KiB | 1,000/1,000 | 39.594 | 49.190 | 53.550 | 110.003 |
| 100 KiB | 1,000/1,000 | 99.582 | 112.947 | 120.936 | 191.354 |

| Payload | Aug. 15 p50 ms | Aug. 16 p50 ms | Change | Aug. 15 p95 ms | Aug. 16 p95 ms | Change |
|---:|---:|---:|---:|---:|---:|---:|
| 1 KiB | 31.639 | 31.583 | -0.18% | 41.721 | 41.975 | +0.61% |
| 10 KiB | 39.596 | 39.594 | -0.01% | 49.082 | 49.190 | +0.22% |
| 100 KiB | 101.290 | 99.582 | -1.69% | 111.503 | 112.947 | +1.29% |

Both runs used reported `70/20/10`, serving Cell 2/no handoff, RSRQ `-13 dB`,
and stationary `location_1`. They are not configuration-matched: the August 15
run recorded RSRP `-100 dBm` with both cells broadcasting, while the new run
used carried-forward RSRP `-98 dBm` with Cell 1 not broadcasting. The close
p50/p95 values are repeat observations, not evidence that Cell 1 state or RSRP
has no effect. Timestamp-aligned ACP/MG52 and cell-load evidence is absent.

All 3,000 sender rows match 3,000 edge rows and pass exact length, CRC32,
sequence, application-acknowledgment, radio/cell-context, and telemetry
validation. All 21 copied edge files matched before key revocation. Raw/public
checksums, structured-file parsing, retained measurement identity, and the
privacy scan pass. The same-day validated stationary GPS capture was reused;
no new bag was requested.

### E14. 2026-08-16 TCP `60/20/20` Payload Sweep

Primary folder:

- `results/real_5g/20260816_airspan_tdd_raw_uplink_60_20_20_location_1_tcp_run_1/`

Purpose: measure TCP raw-object application-acknowledgment RTT at 1 KiB,
10 KiB, 100 KiB, and 1,024 KiB at the same stationary location. The corrected
reported TDD is `60/20/20`; acquisition-time `60/30/10` fields and identifiers
remain preserved and are superseded by `tdd_profile_correction.json`. The
context records Cell 1 locked/not broadcasting, Cell 2 unlocked/broadcasting
and selected by the MG52 with no reported handoff, and carried-forward RSRP
`-98 dBm`/RSRQ `-13 dB`.

| Payload | Status | Accepted/attempts | p50 ms | p95 ms | p99 ms | Max ms |
|---:|---|---:|---:|---:|---:|---:|
| 1 KiB | Complete | 1,000/1,000 | 49.604 | 75.391 | 361.531 | 387.686 |
| 10 KiB | Complete | 1,000/1,000 | 101.216 | 248.853 | 377.315 | 1,013.779 |
| 100 KiB | Complete | 1,000/1,000 | 753.242 | 1,321.325 | 1,773.416 | 2,547.593 |
| 1,024 KiB | User-stopped | 500/500 retained | 7,484.305 | 10,187.364 | 11,175.804 | 13,368.893 |

All retained rows pass exact sequence, payload-length, CRC32, and sender/edge
correlation checks with zero application failures. The 1 MiB sender was
terminated immediately after row 500 at user request, so it retains status 143
and no completion marker and is excluded from the complete three-condition
matrix. All 28 fetched edge files matched before access revocation. The same-
day GPS evidence was reused. The TDD correction and carried-forward radio
values lack timestamp-aligned ACP/MG52 evidence, so this block is not by itself
a causal TDD comparison.

### E15. 2026-08-17 Location-2 TCP/MQTT `60/20/20` Payload Sweep

Primary folder:

- `results/real_5g/20260817_airspan_tdd_raw_uplink_60_20_20_location_2_tcp_mqtt_run_1/`

Purpose: repeat the exact raw-object uplink/application-acknowledgment workload
at a new stationary vehicle location over TCP and MQTT. The user reported TDD
`60/20/20` and RSRP `-100 dBm` during the run. RSRQ `-13 dB`, Cell 1
locked/not broadcasting, Cell 2 unlocked/broadcasting and serving, and no
handoff are carried-forward context rather than fresh measurements.

| Transport | Payload | Status | Accepted/attempts | p50 ms | p95 ms | p99 ms | Max ms |
|---|---:|---|---:|---:|---:|---:|---:|
| TCP | 1 KiB | Complete | 1,000/1,000 | 47.701 | 77.824 | 377.658 | 433.860 |
| TCP | 10 KiB | Complete | 1,000/1,000 | 169.206 | 487.783 | 733.619 | 2,113.708 |
| TCP | 100 KiB | Complete | 1,000/1,000 | 1,460.886 | 2,321.439 | 2,981.048 | 3,773.505 |
| TCP | 1,024 KiB | User-stopped | 227/227 retained | 17,031.656 | 20,982.534 | 24,535.774 | 25,263.822 |
| MQTT | 1 KiB | Complete | 1,000/1,000 | 42.125 | 90.050 | 311.772 | 562.268 |
| MQTT | 10 KiB | Complete | 1,000/1,000 | 149.582 | 461.766 | 654.742 | 1,406.279 |
| MQTT | 100 KiB | Complete | 1,000/1,000 | 1,717.276 | 2,614.260 | 3,251.222 | 4,482.610 |
| MQTT | 1,024 KiB | Complete declared count | 100/100 | 17,229.249 | 20,834.363 | 21,150.195 | 22,356.526 |

The final requested matrix contains 6,327/6,327 accepted exchanges with zero
application failures. The TCP 1-MiB condition retains status 143 and no
completion marker; it is validated as a user-stopped partial condition, not a
completed 1,000-message run. MQTT 1 MiB completed the user's revised declared
count of 100.

The fresh GNSS bag spans 59.599 seconds and 16,098 messages. Its 595 BESTPOS
samples have 0.171 m p95 and 0.189 m maximum radial displacement, so the
stationarity gate passes. Exact GNSS and deployment artifacts are retained in
the committed raw tree at the user's direction; the public derivative retains
only the 0.001-degree coordinate and stationarity summary.

All 68 copied edge files match their remote copies, raw/public checksums pass,
all structured files parse, sender/receiver measurement files remain identical
across the raw/public boundary, and the public privacy scan passes. This is a
one-location, one-profile observation. It does not isolate TDD, signal, serving
cell, or location effects and lacks timestamp-aligned ACP/MG52 evidence.

## Mocar V2X Experiments

### M01. Mocar Setup And Custom RX Debug

Primary folder:

- `results/mocar_v2x/20260703_setup_test/`

Purpose: bring up the OBU/RSU Mocar environment, create remote experiment
folders, deploy binaries, and validate the custom packet path.

Summary:

- SSH to the OBU succeeded and remote experiment folders were created.
- The RSU path was accessed through the configured jump host and management
  address.
- The setup identified that the installed SDK public
  `mde_v2x_custom_send()` symbol was a no-op in this build.
- The custom probe and RTT tools were rebuilt to send through
  `v2x_packet_data_send(payload, len, 0x1b)`.
- Static analysis of the packet-data SDK installed on the OBU and RSU identified
  a 4,080 B maximum application packet after vendor framing. This is a deployed
  SDK limit, not a universal C-V2X limit and not the cause of the measured
  delivery failures.
- The folder preserves setup logs, custom RX debug notes, and local SPaT bridge
  sender evidence.

### M02. Stationary Payload 0-2 KiB Sweeps

Primary folders:

- `results/mocar_v2x/20260703_exp_01_payload_256_full_160216/`
- `results/mocar_v2x/20260703_exp_01_payload_512_full_160733/`
- `results/mocar_v2x/20260703_exp_01_payload_1024_full_161137/`
- `results/mocar_v2x/20260703_exp_01_payload_2048_full_161542/`
- `results/mocar_v2x/20260703_exp_01_payload_sweep_0_2kb_171209/`
- `results/mocar_v2x/20260703_exp_01_payload_sweep_0_2kb_final/`
- `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_133527/`
- `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_144149/`
- `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_151351/`
- `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_155120/`
- `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_164129/`

Purpose: measure Mocar custom radio RTT for compact payloads from 0 to 2048 B
under stationary LOS, NLOS, and obstruction conditions.

Stable stationary evidence:

| Source | Payload | Rows | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---|---:|---:|---:|---:|---:|---:|
| full single-payload run | 256 B | 1000 | 999 | 99.980 | 106.917 | 111.691 |
| full single-payload run | 512 B | 1000 | 1000 | 99.972 | 106.694 | 114.577 |
| full single-payload run | 1024 B | 1000 | 1000 | 99.966 | 106.878 | 112.515 |
| full single-payload run | 2048 B | 1000 | 1000 | 99.958 | 111.663 | 120.567 |
| consolidated sweep | 0 B | 1000 | 1000 | 99.939 | 106.873 | 111.814 |
| consolidated sweep | 2048 B | 1000 | 1000 | 99.958 | 111.661 | 120.562 |

Stationary radio-condition sweeps from July 4:

- `20260704_exp_01_payload_sweep_0_2kb_133527`: 0, 256, and 512 B mostly
  succeeded. The 1024 B phase recorded 900 observed attempts, all of which
  timed out, before the run was stopped; the remaining 100 attempts and the
  skipped 2048 B phase are analysis-only timeout entries.
- `20260704_exp_01_payload_sweep_0_2kb_144149`: all payloads from 0 to 2048 B
  had 99.8-100.0% success, with p50 near 100 ms and p95 around 107-111 ms.
- `20260704_exp_01_payload_sweep_0_2kb_151351`: 0-1024 B mostly succeeded,
  but 2048 B succeeded only 8/1000 times.
- `20260704_exp_01_payload_sweep_0_2kb_155120`: success degraded with payload
  size: 0 B 999/1000, 256 B 991/1000, 512 B 969/1000, 1024 B 171/1000, and
  2048 B treated as timeout after an operator-stopped partial run.
- `20260704_exp_01_payload_sweep_0_2kb_164129`: final location run had 0
  successful analysis attempts, and the remaining payloads were treated as
  timeouts or skipped timeout conditions.

The Mocar payload results show a joint limit from signal strength and packet
size. As normalized signal strength decreased across Points 2-4, the 2 KiB
failure rate increased from 0.2% to 99.2%, followed by 910/910 observed failures
at the next point. The 1 KiB failure rate increased from 0% to 2.1% and 82.9%,
while 256-512 B packets remained more reliable until the path became
unavailable. Point 1 has low signal despite its short distance because a
building blocks its direct LOS path to the RSU. These results show that even 1
KiB delivery requires strong signal and that V2X cannot reliably deliver the
multi-packet objects required by complex CAV applications. This is the evidence
for Insight 1: current V2X technologies cannot support complex CAV applications.

### M03. OBU Signal Probe

Primary folders:

- `results/mocar_v2x/20260703_obu_signal_baseline3_170204/`
- `results/mocar_v2x/20260703_obu_signal_probe_goodlink_170247/`

Purpose: check whether the OBU can expose C-V2X signal strength while custom
RSU-to-OBU traffic is successful.

Results:

- Baseline 256 B probe: 20/20 successes, p50 49.728 ms, p95 56.116 ms.
- Good-link signal probe modes each recorded 30/30 successes. Raw, timestamp/
  frame, total, and SCH-filtered rows had p50 around 49.4-49.5 ms and p95
  around 55.8-58.1 ms.
- The signal capture check recorded empty RSSI diagnostic outputs, so the run
  validates traffic success but does not provide a usable RSSI metric from the
  OBU diagnostic path.

### M04. Radio Coverage Along A Mobile Route

Primary folders:

- `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_173816/`
- `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_175820/`
- `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_182104/`
- `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_183232/`

Purpose: measure Mocar RTT while the vehicle moves along a route, joining V2X
records with GNSS when available.

| Run | Rows | Success | Success rate | p50 RTT ms | p95 RTT ms | p99 RTT ms | GNSS samples | Notes |
|---|---:|---:|---:|---:|---:|---:|---:|---|
| run 1 `173816` | 675 | 515 | 0.763 | 99.610 | 108.421 | 112.436 | 99516 | stopped by operator at end of first drive; private-5G edge unavailable diagnostic preserved separately |
| run 2 `175820` | 1000 | 731 | 0.731 | 28.326 | 41.887 | 55.880 | 116943 | moving LOS route |
| run 3 `182104` | 1000 | 591 | 0.591 | 29.204 | 43.923 | 54.317 | 96720 | 500 ms timeout threshold; clock probe did not return parseable device epoch values |
| run 4 `183232` | 1000 | 716 | 0.716 | 28.276 | 41.621 | 50.651 | 83136 | includes OBU-local clock offset and V2X/GNSS send-time join |

The mobility runs show low RTT on successful packets in later runs, but success
rate ranges from 59.1% to 76.3% because the route moved through weak- or
no-signal locations. Their GNSS-joined request latency and packet failures
produce the normalized V2X signal-strength map. The limiting observation is
spatially varying signal strength, not mobility by itself and not
successful-packet median RTT.

## V2X Dataset And Benchmark Experiments

### V01. V2X Source Registry

Current source registry:

- `benchmarks/v2x/sources.json`

The source registry lists official dataset or benchmark entry points for
OpenCOOD, OpenDAIR-V2X, V2V4Real, V2X-Real, V2X-Radar, OPV2V, V2XSet,
TruckV2X, DAIR-V2X-C examples, V2X-Seq-SPD examples, and V2X-Seq-TFD examples.
The current results folder contains completed benchmark artifacts for staged
OpenDAIR-V2X examples, TruckV2X, and V2X-Radar data. OPV2V, V2XSet, V2V4Real,
and V2X-Real are listed as source entry points but do not have completed result
folders in `results/v2x_benchmarks/`.

### V02. IPI Loopback Over Dataset-Derived Payload Sizes

Primary folders:

- `results/v2x_benchmarks/v2x-ipi-loopback-20260629T170448Z/`
- `results/v2x_benchmarks/v2x-ipi-loopback-20260629T173201Z/`
- `results/v2x_benchmarks/v2x-ipi-loopback-20260629T173649Z/`
- `results/v2x_benchmarks/v2x-ipi-loopback-20260629T175556Z/`

Purpose: replay dataset-derived payload sizes through the local TCP IPI probe
pair. This measures IPI message validation and local loopback behavior, not
wireless network latency and not detector accuracy.

Results:

- The first two generic loopback runs tested payload sizes 0, 256, 1024, 4096,
  one dataset-derived mid-size payload, and 60000 B. Each condition accepted
  all 50/50 attempts.
- Dataset-specific runs tested OpenDAIR-V2X/DAIR-V2X-C, OpenDAIR-V2X/V2X-Seq-
  SPD, OpenDAIR-V2X/V2X-Seq-TFD, TruckV2X/TruckV2X-test-Town1_2, V2X-Radar/
  V2X-Radar-C-validate, and V2X-Radar/V2X-Radar-V-ImageSets.
- Dataset-specific payload sizes include 5262, 5789, 10507, 40337, and 60000 B,
  depending on dataset family.
- All dataset-loopback summary rows report 100% accepted requests. Median RTTs
  in the local harness are mostly around 43-88 ms depending on run and payload.

Representative dataset-specific rows from `v2x-ipi-loopback-20260629T175556Z`:

| Dataset label | Conditions | p0 p50/p95 ms | Max payload | Max-payload p50/p95 ms | Success |
|---|---:|---:|---:|---:|---:|
| OpenDAIR-V2X/DAIR-V2X-C-Example | 6 | 87.889 / 87.915 | 60000 | 43.844 / 43.896 | 100% |
| OpenDAIR-V2X/V2X-Seq-SPD-Example | 6 | 87.896 / 87.930 | 60000 | 43.837 / 43.873 | 100% |
| OpenDAIR-V2X/V2X-Seq-TFD-Example | 5 | 87.886 / 87.916 | 60000 | 87.850 / 87.935 | 100% |
| TruckV2X/TruckV2X-test-Town1_2 | 6 | 87.876 / 87.924 | 60000 | 43.846 / 43.937 | 100% |
| V2X-Radar/V2X-Radar-C-validate | 5 | 87.888 / 87.919 | 60000 | 87.826 / 87.916 | 100% |
| V2X-Radar/V2X-Radar-V-ImageSets | 6 | 87.875 / 87.902 | 60000 | 43.841 / 43.876 | 100% |

### V03. Four-GPU Dataset Artifact Benchmark

Primary folder:

- `results/v2x_benchmarks/v2x-gpu-dataset-benchmark-20260629T175939Z/`

Purpose: replay staged V2X dataset artifacts through a small CUDA tensor
workload on all visible GPUs. This validates dataset/GPU plumbing and payload
processing; it is not detector AP/IoU.

Results:

- 224 files processed successfully.
- 0 failures.
- 128,904,577 bytes processed.
- Wall time: 14.770803 s.
- Aggregate throughput: 8.727 MB/s.
- Four NVIDIA GeForce RTX 2080 Ti GPUs were used.

Dataset coverage:

| Benchmark | Dataset | Files | Processed bytes | Categories |
|---|---|---:|---:|---|
| OpenDAIR-V2X | DAIR-V2X-C-Example | 48 | 26750377 | annotation, image, point_cloud |
| OpenDAIR-V2X | V2X-Seq-SPD-Example | 48 | 29359370 | annotation, image, point_cloud |
| OpenDAIR-V2X | V2X-Seq-TFD-Example | 28 | 29360128 | annotation |
| TruckV2X | TruckV2X-test-Town1_2 | 48 | 17278974 | annotation, image, point_cloud |
| V2X-Radar | V2X-Radar-C-validate | 48 | 26006775 | annotation, image, point_cloud |
| V2X-Radar | V2X-Radar-V-ImageSets | 4 | 148953 | annotation |

### V04. OpenCOOD And V2X-Radar Smoke Checks

Primary folders:

- `results/v2x_benchmarks/latest/`
- `results/v2x_benchmarks/v2x-radar-opencood-smoke-20260629T180847Z/`

Purpose: verify that the local OpenCOOD and V2X-Radar/OpenCOOD environments can
import required training/inference modules and construct the staged V2X-Radar
cooperative validation dataset.

Results:

- OpenCOOD `train` and `inference` imports passed.
- V2X-Radar fork `train` and `test_ddp` imports passed.
- The V2X-Radar cooperative validation dataset constructed successfully.
- Dataset length: 922 samples.
- Scenario folders: 5.
- A sample could be retrieved, with `origin_lidar` shape 42238 x 4,
  `origin_radar` shape 10008 x 4, and object boxes/mask arrays present.

### V05. V2X-Radar Detector Benchmark

Primary folders:

- `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182445Z/`
- `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182522Z/`
- `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/`

Purpose: load the official V2X-Radar radar-only late-fusion checkpoint and run
bounded detector evaluation over staged cooperative validation samples on four
GPUs.

| Run | Samples requested | Success | Failures | Elapsed s | AP IoU 0.3 | AP IoU 0.5 | AP IoU 0.7 | Per-sample median ms | Per-sample p95 ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| `182445Z` | 4 | 4 | 0 | 19.356 | 0.158740 | 0.098491 | 0.014860 | 3906.761 | 4172.587 |
| `182522Z` | 32 | 32 | 0 | 46.657 | 0.187744 | 0.147160 | 0.069704 | 3886.534 | 4477.112 |
| `182624Z` | 922 | 922 | 0 | 828.747 | 0.192143 | 0.130266 | 0.046459 | 3570.341 | 4214.367 |

The final detector run covers the full 922-sample staged validation split in
the current local dataset. Its per-sample CSV records a median of about 202
predicted boxes and 8 ground-truth boxes per successful sample.

## Derived Successful-Attempt RTT Variance

Primary artifacts:

- `paper/analysis/limiting_factors/successful_attempt_variance.csv`
- `paper/analysis/limiting_factors/successful_attempt_variance_summary.json`
- `paper/analysis/limiting_factors/successful_attempt_variance.md`
- `scripts/analyze_successful_ipi_variance.py`

Purpose: quantify sender-side RTT variability conditional on an accepted IPI
response. This is a derived analysis of the retained experiments, not a new
network experiment. It includes conditions with at least 20 finite successful
RTTs and gives each experimental condition equal weight.

The analysis retains 1,688,330 finite successful RTT records from 355
conditions. Because RTT spans multiple orders of magnitude, the primary
variance decomposition uses `log10(RTT)`. Differences between condition means
explain 89.1% of successful-attempt variance, while attempt-to-attempt variation
within a fixed condition explains 10.9%. Thus, when IPI succeeds, the operating
condition determines the latency regime more strongly than ordinary jitter
within one condition.

Across the 355 conditions, the median coefficient of variation is 0.282 and
the median p95/p50 ratio is 1.427. The 95th-percentile condition has a p95/p50
ratio of 6.223, and the maximum is 12.029. The median p95/p50 ratio is 1.456
for the 330 private-5G conditions and 1.089 for the 25 direct-PC5 conditions.
The direct-PC5 value is conditional on receiving at least 20 replies: complete
and near-complete outages remain in the delivery analysis and are not evidence
of low variance.

## Cross-Experiment Takeaways For Future Paper Drafting

These are evidence summaries, not final paper claims:

- Private-5G compact request/response traffic is feasible under favorable
  conditions, but 100 ms deadlines are not robust across TCP, load, weak signal,
  and high-client-count conditions.
- Private-5G payload size matters strongly. 128-512 KiB payloads become
  conditional for 500-1000 ms deadlines, and MiB-scale payloads are seconds-
  scale bulk or prefetch traffic in the measured request/response path.
- Across the favorable and weak detector replays, approximately 20 KiB of
  application content is the conservative measured ceiling for a 500-ms p95
  guarantee on TCP/MQTT. This boundary occurred with one UE, a dedicated
  40-MHz n48 channel, and no ambient contention.
- The `40/40/20` and `70/20/10` application outcomes vary jointly with the
  signal/placement context, payload, and transport. A TDD profile must be
  evaluated with the vehicular field condition and workload mix.
- Application-level `5qi-mapped` labels were collected, but current packet
  capture does not verify network-enforced QoS or 5QI behavior.
- Mocar V2X compact payload RTT can be stable near 100 ms for successful
  stationary links, but moving into weak- or no-signal locations produces large
  reliability differences. Successful-packet RTT alone is not enough; success
  rate and coverage are central.
- V2X dataset experiments currently support workload realism, payload sizing,
  dataset plumbing, and detector-output generation. They do not by themselves
  prove wireless V2X or private-5G deployment performance.
