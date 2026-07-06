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
  Evidence: `results/real_5g/20260701_load_qos_weak_signal_run_1/summary.md`
  and repeat `results/real_5g/20260706_load_qos_weak_signal_run_2/summary.md`.
- [x] Application-side packet marking check for QoS/5QI.
  Evidence: `results/real_5g/20260701_qos_verification_run_1/summary.md`.
- [x] `09_detector_output_to_ipi` private-5G TCP/MQTT replay for
  detector-derived payload sizes, with GNSS.
  Evidence: `results/real_5g/20260702_detector_output_to_ipi_run_1/summary.md`.
- [x] `09_detector_output_to_ipi` private-5G fragmented UDP replay for
  detector-derived payload sizes, with GNSS at the current/good-signal
  location.
  Evidence: `results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1/summary.md`.
- [x] `09_detector_output_to_ipi` weak-signal stationary repeat for TCP, MQTT,
  and fragmented UDP, with NovAtel GNSS.
  Evidence:
  `results/real_5g/20260706_detector_output_to_ipi_weak_signal_tcp_mqtt_run_1/summary.md`
  and
  `results/real_5g/20260706_detector_output_to_ipi_weak_signal_udp_fragmented_run_1/summary.md`.
- [x] `08_end_to_end_deadline` deadline/service-envelope analysis over completed
  private-5G sender CSVs.
  Evidence: `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/summary.md`.
- [x] `11_multiclient_scalability` stationary private-5G TCP/UDP/MQTT sweep for
  1, 2, 5, 10, 20, 50, and 100 clients, with NovAtel GNSS. Two
  current-location repeats and one weak-signal repeat are now collected.
  Evidence: `results/real_5g/20260702_multiclient_scalability_run_2/summary.md`
  and `results/real_5g/20260702_multiclient_scalability_run_3/summary.md` for
  repeat 1, plus `results/real_5g/20260702_multiclient_scalability_run_4/summary.md`
  for repeat 2, and
  `results/real_5g/20260706_multiclient_scalability_weak_signal_run_1/summary.md`
  for the weak-signal repeat.
- [x] `12_failure_fallback` stationary no-Mocar subset for TCP, UDP, and MQTT.
  Receiver-restart was collected for TCP/UDP/MQTT; broker-restart was collected
  for MQTT, with NovAtel GNSS and edge-side receiver/broker artifacts.
  Evidence: `results/real_5g/20260703_failure_fallback_run_1/summary.md`.
- [x] `01_broadcast_message_baseline` Mocar V2X stationary baseline/payload
  sweep across GNSS-indexed locations.
  Evidence: `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_164129/summary.md`
  plus earlier location sweeps under `results/mocar_v2x/`.
- [x] Radio baseline part of `E1` for stationary Mocar V2X.
  Evidence: `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_164129/summary.md`.
- [x] `02_radio_distance_mobility` Mocar V2X mobility/radio-distance runs with
  NovAtel GNSS.
  Evidence: `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_173816/summary.md`,
  `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_175820/summary.md`,
  `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_182104/summary.md`,
  and `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_183232/summary.md`.
  Runs 3-4 include corrected send-time GNSS joins.
- [x] `03_broadcast_contention`
  Marked complete by operator based on prior collection; no additional Mocar V2X
  run needed for the current paper plan.
- [x] `05_handover_weak_signal`
  No new collection needed; keep as a paper discussion / limitation point rather
  than a standalone measured result.
- [x] `07_transport_comparison`
  TCP/MQTT stationary evidence exists, and UDP behavior is covered by later
  private-5G UDP experiments. No separate Exp 07 blocker remains.
- [x] `10_edge_offload_tradeoff`
  No new collection needed for this paper. Edge-offload/local-compute result is
  already covered by the existing "Towards Collaborative Autonomous Driving"
  paper result from Yuankai He.

## Highest-Priority Remaining Stationary, No-Mocar Work

- [ ] `06_load_qos_stress` core-side QoS proof, only if making a QoS claim
  Independently verify private-5G core-side 5QI/QoS enforcement before making a
  causal QoS-improvement claim. Run 4 collected `qos_profile=5qi-mapped`; the
  weak-signal repeat collected both default and `5qi-mapped` labels. The
  packet-marking check showed both default and `5qi-mapped` probes leaving the
  vehicle host with `tos 0x0`, so Meraki/private-5G core QoS-flow counters are
  still needed for a verified 5QI claim.

## Optional Or Conditional Remaining Work

- [ ] Security overhead (`S4`)
  Add TLS/mTLS or equivalent security mode only if the paper needs a security
  overhead claim.
- [ ] Queue-discipline ablations (`S7`)
  Run FIFO/priority/WFQ only if queue policy is a paper claim.

## Hardware Or Mobility Work Still Open

- [ ] `04_private5g_mobility`
  Requires driving route passes and GNSS.
- [ ] Vehicle-level outcome (`E5`)
  Requires actual AV behavior metric collection under baseline and stressed
  network conditions.

## Current Practical Next Run

Run `04_private5g_mobility` if the paper needs moving private-5G route data.
For experiment 06, only the core-side QoS-flow verification remains if the paper
will make a QoS-specific claim. Experiment 10 is not a remaining blocker because
the edge-offload/local-compute result is already covered by the existing paper
result.
