# Remaining Edge4AV Experiments

This file tracks what remains after reconciling `experiment.md`,
`top_tier_experiment_collection.md`, and the recorded results under `results/`.

## Already Covered

- [x] Private-5G TCP/MQTT baseline for SPaT and guided-service probes.
  Evidence: `results/real_5g/20260513_sunny_fintechparking_run_1/summary.md`.
- [x] Payload sensitivity for private-5G TCP/MQTT at stationary GNSS-indexed
  locations.
  Evidence: `results/real_5g/20260521_small_rain_run_1/summary.md` and
  `results/real_5g/20260522_cloudy_run_1/summary.md`.
- [x] TCP/MQTT stationary transport comparison evidence.
  Evidence: `results/real_5g/20260522_cloudy_run_1/summary.md`.
- [x] V2X-Radar detector workload and detector-output CSV source.
  Evidence: `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/`.
- [x] Dataset-derived local IPI loopback mechanics.
  Evidence: `results/v2x_benchmarks/v2x-ipi-loopback-20260629T175556Z/`.
- [x] `06_load_qos_stress` stationary private-5G Phase A: TCP/MQTT idle
  versus uplink background load with GNSS.
  Evidence: `results/real_5g/20260701_load_qos_run_1/summary.md` and
  `results/real_5g/20260701_load_qos_run_2/summary.md`.
- [x] `06_load_qos_stress` multi-load sweep with 1, 2, and 4 background
  streams.
  Evidence: `results/real_5g/20260701_load_qos_run_3/summary.md`.
- [x] `06_load_qos_stress` non-default QoS-profile collection leg.
  Evidence: `results/real_5g/20260701_load_qos_run_4/summary.md`.
- [x] `06_load_qos_stress` weak-signal stationary repeat with NovAtel GNSS.
  Evidence: `results/real_5g/20260701_load_qos_weak_signal_run_1/summary.md`.
- [x] Application-side packet marking check for QoS/5QI.
  Evidence: `results/real_5g/20260701_qos_verification_run_1/summary.md`.
- [x] `09_detector_output_to_ipi` private-5G TCP/MQTT replay for
  detector-derived payload sizes, with GNSS.
  Evidence: `results/real_5g/20260702_detector_output_to_ipi_run_1/summary.md`.
- [x] `09_detector_output_to_ipi` private-5G fragmented UDP replay for
  detector-derived payload sizes, with GNSS at the current/good-signal
  location.
  Evidence: `results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1/summary.md`.
- [x] `08_end_to_end_deadline` deadline/service-envelope analysis over completed
  private-5G sender CSVs.
  Evidence: `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/summary.md`.
- [x] `11_multiclient_scalability` stationary private-5G TCP/UDP/MQTT sweep for
  1, 2, 5, 10, 20, 50, and 100 clients, with NovAtel GNSS. Two
  current-location repeats are now collected; weak-signal collection remains.
  Evidence: `results/real_5g/20260702_multiclient_scalability_run_2/summary.md`
  and `results/real_5g/20260702_multiclient_scalability_run_3/summary.md` for
  repeat 1, plus `results/real_5g/20260702_multiclient_scalability_run_4/summary.md`
  for repeat 2.
- [x] `12_failure_fallback` stationary no-Mocar subset for TCP, UDP, and MQTT.
  Receiver-restart was collected for TCP/UDP/MQTT; broker-restart was collected
  for MQTT, with NovAtel GNSS and edge-side receiver/broker artifacts.
  Evidence: `results/real_5g/20260703_failure_fallback_run_1/summary.md`.

## Highest-Priority Remaining Stationary, No-Mocar Work

- [ ] `06_load_qos_stress` core-side QoS proof, only if making a QoS claim
  Independently verify private-5G core-side 5QI/QoS enforcement before making a
  causal QoS-improvement claim. Run 4 collected `qos_profile=5qi-mapped`; the
  weak-signal repeat collected both default and `5qi-mapped` labels. The
  packet-marking check showed both default and `5qi-mapped` probes leaving the
  vehicle host with `tos 0x0`, so Meraki/private-5G core QoS-flow counters are
  still needed for a verified 5QI claim.
- [ ] `11_multiclient_scalability` weak-signal stationary data collection
  Two stationary current-location repeats are collected for TCP/UDP/MQTT at
  `CLIENTS=1,2,5,10,20,50,100`. Repeat the same sweep at the weak-signal
  location with NovAtel GNSS if the paper compares RF-good versus RF-weak
  multiclient behavior.
- [ ] `09_detector_output_to_ipi` weak-signal stationary repeat
  Repeat detector-output replay at the weak-signal location with GNSS. Use the
  same detector payload set and include TCP, MQTT, and fragmented UDP if the
  paper compares all three transports under RF degradation.

## Optional Or Conditional Remaining Work

- [ ] `07_transport_comparison` UDP leg
  TCP/MQTT evidence already exists. Run UDP only if the paper explicitly claims
  a three-transport comparison.
- [ ] Security overhead (`S4`)
  Add TLS/mTLS or equivalent security mode only if the paper needs a security
  overhead claim.
- [ ] Queue-discipline ablations (`S7`)
  Run FIFO/priority/WFQ only if queue policy is a paper claim.

## Hardware Or Mobility Work Still Open

- [ ] `01_broadcast_message_baseline`
  Requires Mocar/radio devices.
- [ ] `02_radio_distance_mobility`
  Requires Mocar/radio devices and distance/LOS/NLOS setup.
- [ ] `03_broadcast_contention`
  Requires Mocar/radio devices and background radio senders.
- [ ] `04_private5g_mobility`
  Requires driving route passes and GNSS.
- [ ] `05_handover_weak_signal`
  Requires weak-signal/handover route passes and modem/signal evidence.
- [ ] Radio part of `E1`
  Needed only if the main paper keeps a direct radio-vs-private-5G baseline
  figure.
- [ ] Vehicle-level outcome (`E5`)
  Requires actual AV behavior metric collection under baseline and stressed
  network conditions.

## Current Practical Next Run

Repeat `09_detector_output_to_ipi` or `11_multiclient_scalability` at the
weak-signal stationary location if the paper needs RF-good versus RF-weak
comparisons. For experiment 06, only the core-side QoS-flow verification remains
if the paper will make a QoS-specific claim.
