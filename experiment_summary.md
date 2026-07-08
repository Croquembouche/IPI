# Experiment Summary

Last updated: 2026-07-08.

This file summarizes the experiments currently present under `results/`. It is
based on the current result artifacts, not on prior generated paper prose. Raw
CSV, JSON, log, GPS, and receiver artifacts remain under `results/`.

## Inventory

| Result area | Scope | Current folders |
|---|---|---:|
| `results/real_5g/` | Private-5G TCP/MQTT/UDP latency, payload sweeps, load/QoS labels, detector-output replay, multiclient scaling, failure/fallback, deadline analysis, GPS, and signal maps | 24 run folders plus derived signal-map artifacts |
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
weather labels, and vehicle placements. The sender used 1000 probes per
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
load, multiple load streams, weak signal placement, and an application-level
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
  clients under weak-signal placement.

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
under stationary placements.

Stable stationary evidence:

| Source | Payload | Rows | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---|---:|---:|---:|---:|---:|---:|
| full single-payload run | 256 B | 1000 | 999 | 99.980 | 106.917 | 111.691 |
| full single-payload run | 512 B | 1000 | 1000 | 99.972 | 106.694 | 114.577 |
| full single-payload run | 1024 B | 1000 | 1000 | 99.966 | 106.878 | 112.515 |
| full single-payload run | 2048 B | 1000 | 1000 | 99.958 | 111.663 | 120.567 |
| consolidated sweep | 0 B | 1000 | 1000 | 99.939 | 106.873 | 111.814 |
| consolidated sweep | 2048 B | 1000 | 1000 | 99.958 | 111.661 | 120.562 |

Placement-sensitive July 4 sweeps:

- `20260704_exp_01_payload_sweep_0_2kb_133527`: 0, 256, and 512 B mostly
  succeeded; 1024 and 2048 B were treated as timeout after operator stop.
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

The Mocar payload results are therefore not only payload-dependent; placement
and link condition are central to interpreting the radio path.

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

### M04. Radio Distance And Mobility

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

The mobility results show low RTT on successful packets in later runs, but
success rate ranges from 59.1% to 76.3%, so reliability rather than successful-
packet median RTT is the limiting observation.

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

## Cross-Experiment Takeaways For Future Paper Drafting

These are evidence summaries, not final paper claims:

- Private-5G compact request/response traffic is feasible under favorable
  conditions, but 100 ms deadlines are not robust across TCP, load, weak signal,
  and high-client-count conditions.
- Private-5G payload size matters strongly. 128-512 KiB payloads become
  conditional for 500-1000 ms deadlines, and MiB-scale payloads are seconds-
  scale bulk or prefetch traffic in the measured request/response path.
- Application-level `5qi-mapped` labels were collected, but current packet
  capture does not verify network-enforced QoS or 5QI behavior.
- Mocar V2X compact payload RTT can be stable near 100 ms for successful
  stationary links, but placement and mobility produce large reliability
  differences. Successful-packet RTT alone is not enough; success rate is
  central.
- V2X dataset experiments currently support workload realism, payload sizing,
  dataset plumbing, and detector-output generation. They do not by themselves
  prove wireless V2X or private-5G deployment performance.
