# Top-Tier Experiment Collection Runbook

This runbook covers the full Edge4AV experiment suite for a MobiCom/MobiSys
style submission. It is intentionally larger than the minimum publishable
subset: run all 12 experiments when hardware time is available.

## Build And Generate Starter Scripts

Run this once on the build host:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
make -C third_party/mocar/J2735-2020/samples/ipi_custom_rtt
make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge
python3 scripts/create_edge4av_experiment_suite.py
```

The generator prints a directory under `results/edge4av_top_tier/<run_id>/`.
Use its `start_scripts/` directory for collection and keep the same `RUN_ID`
for every device role in one collection pass.

For every experiment, start receiver/responder/load-server roles first, then
start sender/initiator roles. Vehicle-motion experiments should also run:

```bash
ROLE=gps RUN_ID=<run_id> ./results/edge4av_top_tier/<run_id>/start_scripts/<script>.sh
```

Use synchronized clocks when reporting one-way latency. If clocks are not
confirmed synchronized, report RTT only.

## Required Repeats

A repeat is a fresh condition run or route pass, not rerunning analysis on the
same CSV. Keep partial and failed runs, and mark the failure condition in the
event or metadata logs.

| Status | ID | Starter script | Minimum collection | Start roles and condition sweep |
| --- | --- | --- | --- | --- |
| [x] Completed | 01 | `01_broadcast_message_baseline.sh` | 5 repeats, 1000 probes per message/rate | Mocar V2X stationary baseline/payload sweeps collected with GNSS across multiple locations in `results/mocar_v2x/`, including final location `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_164129/`. |
| [x] Completed | 02 | `02_radio_distance_mobility.sh` | 5 repeats, 1000 probes per distance/mobility | Mocar V2X mobility/radio-distance runs collected with GNSS under `results/mocar_v2x/20260704_exp_02_radio_distance_mobility_{173816,175820,182104,183232}/`. Runs 3-4 use corrected send-time GNSS joining and 500 ms timeout. |
| [x] Completed previously | 03 | `03_broadcast_contention.sh` | 5 repeats, 1000 target probes while background load runs | Marked complete by operator based on prior Mocar V2X contention/comparison collection; no additional run needed for the current paper plan. |
| [ ] Not started | 04 | `04_private5g_mobility.sh` | 10 route passes per transport/payload | Run `ROLE=receiver`, `ROLE=gps`, then `ROLE=sender`. Sweep `TRANSPORT=tcp,mqtt,udp` and payloads such as `0,1024,4096,60000`. |
| [x] Completed as paper discussion | 05 | `05_handover_weak_signal.sh` | 10 passes per weak-signal/handover condition | No new data collection needed. Use as a talking point / limitation in the paper rather than a standalone measured result. |
| [x] Collected with caveat | 06 | `06_load_qos_stress.sh` | 5 repeats, 1000 latency probes per load/QoS condition | Stationary private-5G TCP/MQTT idle-vs-load, multi-load sweep, `qos_profile=5qi-mapped` leg, and weak-signal repeats completed in `results/real_5g/20260701_load_qos_run_{1,2,3,4}/`, `results/real_5g/20260701_load_qos_weak_signal_run_1/`, and `results/real_5g/20260706_load_qos_weak_signal_run_2/`. Caveat: QoS labels do not independently verify core-side 5QI enforcement. |
| [x] Completed | 07 | `07_transport_comparison.sh` | 5 repeats, 1000 probes per transport/payload | TCP/MQTT stationary sweeps are complete in `results/real_5g/20260521_small_rain_run_1` and `results/real_5g/20260522_cloudy_run_1`; UDP transport evidence is covered by later private-5G experiments including detector-output replay, multiclient scalability, and failure/fallback. |
| [x] Analysis complete | 08 | `08_end_to_end_deadline.sh` | 10 repeats, 1000 service requests per deadline/application | Existing private-5G sender CSVs were analyzed as a deadline/service-envelope study in `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/`. Uses RTT directly for request/response services; no dedicated deadline sweep needed unless the paper adds a new application class. |
| [x] Completed | 09 | `09_detector_output_to_ipi.sh` | 5 repeats, 1000 probes per detector-output payload | Detector-output manifest and private-5G TCP/MQTT replay completed in `results/real_5g/20260702_detector_output_to_ipi_run_1/`; fragmented UDP replay completed in `results/real_5g/20260702_detector_output_to_ipi_udp_fragmented_run_1/`. Weak-signal TCP/MQTT and fragmented UDP repeats completed in `results/real_5g/20260706_detector_output_to_ipi_weak_signal_tcp_mqtt_run_1/` and `results/real_5g/20260706_detector_output_to_ipi_weak_signal_udp_fragmented_run_1/`. |
| [x] Completed previously | 10 | `10_edge_offload_tradeoff.sh` | 5 repeats, at least 100 detector/application samples per placement | No new collection needed for this paper. Edge-offload/local-compute result is already covered by the existing "Towards Collaborative Autonomous Driving" paper result from Yuankai He. |
| [x] Completed | 11 | `11_multiclient_scalability.sh` | 5 repeats, 1000 probes per client per concurrency level | Stationary private-5G TCP/UDP/MQTT sweep completed for `CLIENTS=1,2,5,10,20` in `results/real_5g/20260702_multiclient_scalability_run_2/` and extended to `CLIENTS=50,100` in `results/real_5g/20260702_multiclient_scalability_run_3/`; full second repeat completed for `CLIENTS=1,2,5,10,20,50,100` in `results/real_5g/20260702_multiclient_scalability_run_4/`. Weak-signal full sweep completed in `results/real_5g/20260706_multiclient_scalability_weak_signal_run_1/`. MQTT used one edge receiver per source ID. |
| [x] Stationary subset collected; weak-signal/compact pending | 12 | `12_failure_fallback.sh` | 5 repeats, 1000 probes per failure mode | Stationary no-Mocar subset completed in `results/real_5g/20260703_failure_fallback_run_1/`: receiver-restart for TCP/UDP/MQTT and broker-restart for MQTT, 1000 probes per condition with NovAtel GNSS. Run weak-signal and compact-fallback only if the paper makes those specific claims. |

## Data To Keep For Every Condition

- Sender CSV, receiver CSV/log, and any load-generator CSV.
- `RUN_ID`, `CONDITION_ID`, transport, payload size, message type, clock-sync
  state, mobility state, load level, QoS profile, and service/deadline labels.
- GPS CSV for any vehicle movement or weak-signal/handover run.
- Radio distance, LOS/NLOS state, antenna placement, radio channel/config, and
  background sender count for V2X radio experiments.
- Failure/event timestamps for restart, outage, handover, and fallback runs.

## Local Analysis Helpers

```bash
python3 scripts/build_detector_output_ipi_payloads.py \
  --input results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/per_sample.csv \
  --output results/v2x_benchmarks/latest/detector_output_ipi_payloads.json

python3 scripts/analyze_edge4av_deadlines.py \
  results/edge4av_top_tier/<run_id>/<condition>_sender.csv \
  --deadline-ms 250 \
  --output results/edge4av_top_tier/<run_id>/<condition>_deadline_summary.json
```
