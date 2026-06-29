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

| ID | Starter script | Minimum collection | Start roles and condition sweep |
| --- | --- | --- | --- |
| 01 | `01_broadcast_message_baseline.sh` | 5 repeats, 1000 probes per message/rate | Run `ROLE=responder` on the peer radio, then `ROLE=initiator` on the target radio. Sweep `MESSAGE_CLASS=spat,map,bsm,tim` where supported and at least two `INTERVAL_MS` rates. |
| 02 | `02_radio_distance_mobility.sh` | 5 repeats, 1000 probes per distance/mobility | Run responder once per location. Sweep `DISTANCE_M`, `LINK_STATE=los,nlos`, and `MOBILITY_STATE=stationary,moving-5mph,moving-15mph`. |
| 03 | `03_broadcast_contention.sh` | 5 repeats, 1000 target probes while background load runs | Run responder, then one or more `ROLE=background_initiator` terminals, then `ROLE=target_initiator`. Sweep background payload/rate. |
| 04 | `04_private5g_mobility.sh` | 10 route passes per transport/payload | Run `ROLE=receiver`, `ROLE=gps`, then `ROLE=sender`. Sweep `TRANSPORT=tcp,mqtt,udp` and payloads such as `0,1024,4096,60000`. |
| 05 | `05_handover_weak_signal.sh` | 10 passes per weak-signal/handover condition | Run receiver and GPS before driving. Sweep `SIGNAL_ZONE=edge-of-coverage,handover,nlos` and keep modem signal logs if available. |
| 06 | `06_load_qos_stress.sh` | 5 repeats, 1000 latency probes per load/QoS condition | Run `ROLE=load_server`, `ROLE=latency_receiver`, then `ROLE=load_client` and `ROLE=latency_sender`. Sweep `LOAD_DIRECTION`, `LOAD_MBPS`, `QOS_PROFILE`, and transport. |
| 07 | `07_transport_comparison.sh` | 5 repeats, 1000 probes per transport/payload | Run receiver then sender for `TRANSPORT=tcp,mqtt,udp` with identical `PAYLOAD_BYTES`, `COUNT`, and `INTERVAL_MS`. |
| 08 | `08_end_to_end_deadline.sh` | 10 repeats, 1000 service requests per deadline/application | Run receiver then sender, then `ROLE=analyze`. Sweep `DEADLINE_MS` values matching the application claim. |
| 09 | `09_detector_output_to_ipi.sh` | 5 repeats, 1000 probes per detector-output payload | First run `ROLE=build_payloads` from the detector CSV. Then run receiver and sender for each transport to replay representative detector-output payload sizes. |
| 10 | `10_edge_offload_tradeoff.sh` | 5 repeats, at least 100 detector/application samples per placement | Run `ROLE=local_detector` for compute timing, receiver for transfer timing, then `ROLE=network_transfer`, then `ROLE=deadline_analyze`. Sweep `PLACEMENT=onboard-only,edge-only,hybrid`. |
| 11 | `11_multiclient_scalability.sh` | 5 repeats, 1000 probes per client per concurrency level | Run receiver, then `ROLE=sender_group`. Sweep `CLIENTS=1,2,5,10,20` where the testbed remains stable. Use TCP or UDP unless you run one MQTT receiver per source ID. |
| 12 | `12_failure_fallback.sh` | 5 repeats, 1000 probes per failure mode | Run receiver and sender, trigger the failure, and run `ROLE=event` at the trigger time. Sweep `FAILURE_MODE=receiver-restart,broker-restart,weak-signal,compact-fallback`. |

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
