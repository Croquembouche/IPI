# Agent Context

This repository contains the Intersection Programming Interface (IPI) and the
Edge4AV experiment workspace. IPI is a standards-aligned message fabric for
intelligent intersections, connected/autonomous vehicles, RSUs/OBUs, edge
services, and cloud/backhaul services. The checkout combines a C++17 reference
library, private-5G and radio latency tooling, ROS 2 message assets, Mocar SDK
samples, paper artifacts, static experiment-tracker UI, and collected real
private-5G results.

Read `current_task.md` first for the active assignment and handoff state. Use
this file to decide which source folders matter for that task.

## Top-Level Files

- `agent.md` - Stable agent operating guide: project goals, scope boundaries,
  pass conditions, fail conditions, stop conditions, and working rules.
- `agent_context.md` - This repository map. Keep it updated when files,
  folders, source-of-truth locations, or build/deployment paths change.
- `current_task.md` - Mutable current-task handoff. Update it at task start,
  when status changes, and before handing work back.
- `README.md` - High-level project overview, repository layout, C++ build
  commands, web tracker instructions, example binaries, and demo workflows for
  SPaT-over-radio, Mocar radio RTT, and private-5G TCP/MQTT latency.
- `setup.md` - Deployment guide for build host, TX edge, Mocar radio, private
  5G sender/receiver, and MQTT broker roles.
- `instructions.md` - Operator runbook for latency tests using V2X radios,
  private-5G devices, shared experiment metadata, CSV logging, optional Mocar
  RTT, and GPS capture expectations.
- `experiment.md` - Edge4AV campaign tracker defining service classes, planned
  figures, experiment matrices, required metadata, GPS requirements, and result
  tables.
- `top_tier_experiment_collection.md` - Full 12-experiment MobiCom/MobiSys
  collection runbook with starter-script generation, role startup order,
  condition sweeps, repeat counts, and data-retention requirements.
- `experiment-tracker.html` - Root-level convenience entry point for the static
  tracker under `web/experiment-tracker/`.
- `benchmarks/v2x/` - Public V2X benchmark staging area. It tracks source URLs,
  ignored downloaded data, ignored benchmark code checkouts, and reproducible
  instructions for DAIR-V2X/OpenDAIR-V2X, OPV2V/OpenCOOD, V2XSet/OpenCOOD,
  V2V4Real, TruckV2X, V2X-Radar, and V2X-Real IPI tests. Current staged data
  covers OpenDAIR-V2X examples, TruckV2X, and V2X-Radar.
- `paper_review_instructions_detailed.md` - Paper-specific review guidance and
  current rendered text snapshot used for manuscript review tasks.
- `.gitignore` - Ignores generated build outputs, ROS build/install/log trees,
  Python cache files, temporary files, and paper build artifacts.
- `.git/` - Git metadata. `git status --short` currently works in this checkout.
- `.vscode/`, `.agents/`, `.codex/`, and `tmp/` - Local editor, agent, or
  scratch areas. Treat as local state unless the user explicitly asks to
  publish or preserve a specific file.

## C++ Source And Build Tree

- `cpp/` - C++17 reference implementation and examples.
  - `cpp/CMakeLists.txt` - Builds the static `ipi` library, examples, optional
    ROS 2 bridge code, optional Mocar SDK examples, install targets, and tests.
    Defaults: `IPI_BUILD_EXAMPLES=ON`, `IPI_ENABLE_TESTS=OFF`,
    `IPI_ENABLE_ROS2_BRIDGE=OFF`, and `IPI_ENABLE_MOCAR_EXAMPLES=OFF`.
  - `cpp/README.md` and `cpp/LOGIC.md` - C++ layout, architecture notes,
    bridge design sketch, mesh behavior, task offloading, build options, and
    next steps.
  - `cpp/include/ipi/core/` and `cpp/src/core/` - IPI data models, service
    requests, cooperative service payloads, `MessageFrame`, and validation.
  - `cpp/include/ipi/v2x/` and `cpp/src/v2x/` - SAE J2735 helper models,
    UPER codec facade, and optional ROS 2 bridge implementation.
  - `cpp/include/ipi/api/` and `cpp/src/api/` - Sender/receiver APIs,
    Edge4AV dual-plane behavior, experiment logging, private session transport,
    private-5G latency probe encoding/timing, shared state, and minimal MQTT
    3.1.1 client support.
  - `cpp/include/ipi/mesh/` and `cpp/src/mesh/` - Mesh manager and cooperative
    task offloader for degraded infrastructure or V2V scenarios.
  - `cpp/include/ipi/common/` - Shared debugging helpers.
  - `cpp/examples/library/` - Host-side examples:
    `build_service_request`, `v2x_roundtrip`, `spat_tcp_sender`,
    `private_5g_latency_sender`, `private_5g_latency_receiver`,
    `private_5g_latency_udp_sender`, `private_5g_latency_udp_receiver`,
    `mesh_demo`, and `edge4av_dual_plane`.
  - `cpp/examples/device/` - Device/ROS-facing examples built through CMake
    when optional dependencies are available: `mocar_ipi_demo`,
    `spat_tcp_bridge`, and `ros2_bsm_broadcaster`.
  - `cpp/examples/README.md` - Example-level guidance.
  - `cpp/tests/` - No-framework executable tests for Edge4AV API behavior,
    experiment logging, private session transport, private-5G probe encoding,
    TCP loopback, and MQTT loopback.
  - `cpp/build/` - Generated CMake build tree and compiled binaries currently
    present. Treat as generated output unless debugging a local build.

Primary validation command:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

## Mocar And Radio Assets

- `third_party/mocar/J2735-2020/` - Vendored Mocar J2735 SDK material used for
  the deployment path documented in `README.md`, `setup.md`, and
  `instructions.md`.
  - `include/` and `lib/` - Vendor headers and runtime libraries such as
    `libmocarcv2x.so` and `libzlog.so`.
  - `samples/ipi_spat_bridge/` - TCP-to-Mocar SPaT bridge. Build separately
    from the main CMake project with:

    ```bash
    make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge
    ```

  - `samples/ipi_custom_rtt/` - Custom-channel Mocar radio RTT probe. It has
    initiator and responder roles, computes RTT on the initiator's monotonic
    clock, and includes `run_payload_sweep.sh` for payload sweeps.
  - `samples/spat/spat_sample.c` - Vendor SPaT sample useful as a receive
    callback reference. Its `main()` also transmits, so do not treat it as a
    drop-in receive-only latency harness.
  - Other vendor samples (`bsm`, `map`, `srm`, `ssm`, `tim`, `custom`, etc.) -
    Reference implementations for Mocar API usage.
- `cpp/CMakeLists.txt` also has an optional `IPI_ENABLE_MOCAR_EXAMPLES` path
  that expects a different SDK layout at `third_party/mocar/new_V2X_64bit` or
  `-DMOCAR_SDK_ROOT=...`. That is not the documented deployment path for the
  current `J2735-2020` bridge and RTT samples.

## ROS 2 Assets

- `ros2_ws/src/v2x_msg/` - ROS 2 message package with J2735-style definitions
  such as BSM, MAP, SPAT, SRM, SSM, TIM, and supporting message types.
- `ros2_ws/build/`, `ros2_ws/install/`, and `ros2_ws/log/` - Generated ROS 2
  workspace outputs when present. Treat as local build artifacts.

## Scripts

- `scripts/build_real5g_dashboard.py` - Reads private-5G sender CSVs and writes
  a static dashboard with aggregate latency, success rate, payload sensitivity,
  comparisons, trials, and GPS snapshots.
- `scripts/build_real5g_paper_figures.py` - Generates paper-facing figures from
  real private-5G result artifacts.
- `scripts/build_run_signal_latency_summary.py` - Builds run-level summaries
  that join signal information and latency output.
- `scripts/build_signal_strength_maps.py` - Parses signal measurements and
  produces signal strength map artifacts.
- `scripts/build_radio_latency_relationship.py` - Builds the signal/latency
  relationship dataset and estimator output.
- `scripts/download_v2x_benchmarks.py` - Clones V2X benchmark repositories,
  downloads official example archives when possible, records full dataset
  entrypoints, and writes `benchmarks/v2x/data/manifests/download_manifest.json`.
- `scripts/build_v2x_ipi_payload_manifest.py` - Scans staged V2X dataset files
  and writes an IPI payload manifest with raw artifact sizes, IPI-safe payload
  sizes, and chunk-count estimates.
- `scripts/run_v2x_ipi_loopback.py` - Runs the local TCP IPI probe pair across
  dataset-derived payload sizes, optionally grouped by dataset family, and
  records CSV/summary artifacts under `results/v2x_benchmarks/`.
- `scripts/run_v2x_gpu_dataset_benchmark.py` - Uses all selected CUDA devices
  for a lightweight tensor workload over staged V2X annotation, image, and
  point-cloud artifacts. It validates dataset/GPU plumbing and records per-file
  timings, but it is not detector AP/IoU.
- `scripts/run_v2x_radar_detector_benchmark.py` - Loads the official V2X-Radar
  radar-only late-fusion checkpoint, applies a small runtime compatibility shim
  for the fork's old `mmcv.runner` imports, shards validation samples over all
  selected GPUs, and records detector AP/statistics under
  `results/v2x_benchmarks/`.
- `scripts/edge4av_loadgen.py` - Dependency-free TCP/UDP background-load
  generator used by controlled private-5G load and QoS stress experiments.
- `scripts/build_detector_output_ipi_payloads.py` - Converts V2X-Radar
  detector `per_sample.csv` output into representative IPI detector-output
  payload sizes for replay over the private-5G transport probes.
- `scripts/analyze_edge4av_deadlines.py` - Computes deadline-hit rates and
  p50/p95/p99 RTT summaries from Edge4AV sender CSV files.
- `scripts/create_edge4av_experiment_suite.py` - Generates the full 12-script
  top-tier Edge4AV experiment suite under `results/edge4av_top_tier/<run_id>/`
  plus a matrix, manifest, and per-run instructions.
- `scripts/gps_topic_recorder.py` - ROS 2 node that records NovAtel GPS topics
  to CSV plus a latest-sample snapshot.
- `scripts/record_gps_for_experiment.sh` - Starts the NovAtel OEM7 driver and
  GPS recorder for a specific result directory.
- `scripts/__pycache__/` - Generated Python bytecode. Do not treat as source.

## Results And Experiment Evidence

- `results/README.md` - Results-area overview.
- `results/real_5g/` - Real private-5G campaigns and derived analysis files.
  Top-level derived artifacts include parsed signal measurements, run-location
  summaries, signal maps, `run_signal_latency_summary.csv`,
  `radio_latency_relationship.csv`, and
  `radio_latency_estimator_model.json`.
- Dated run directories under `results/real_5g/` currently include:
  - `20260513_sunny_fintechparking_run_1/`
  - `20260514_sunny_after_rain_run_1/`
  - `20260515_sunny_run_1/`
  - `20260521_small_rain_run_1/`
  - `20260522_cloudy_run_1/`
- Run directories typically contain sender CSV/error files, base-station
  receiver artifacts, command logs, environment summaries, metadata markdown,
  GPS folders, generated helper scripts, and summary markdown. Preserve these
  as experiment evidence.
- `results/local_loopback/` and `results/visualize/` may appear for local
  generated or visualization outputs.
- `results/v2x_benchmarks/` - V2X benchmark evidence generated from staged
  public datasets and the IPI loopback harness. Keep `download_manifest.json`,
  `v2x_ipi_payload_manifest.json`, per-run CSVs, summaries, environment
  snapshots, four-GPU dataset artifact benchmark outputs, OpenCOOD/V2X-Radar
  smoke notes, full V2X-Radar detector benchmark outputs, and benchmark command
  logs as recorded results.

## Paper And Web Assets

- `paper/` - IEEE-style Edge4AV paper workspace.
  - `IEEE-conference-template-062824.tex` - Main LaTeX entry point.
  - `sections/` - Paper sections and TODO/planning material.
  - `figs/` - Paper figures, including generated PNGs and source SVG/PNG
    artifacts.
  - `ref.bib`, `IEEE*.bib`, `IEEEtran*.bst`, and `IEEEtran.cls` -
    bibliography and template support files.
  - `*.pdf`, `*.aux`, `*.bbl`, `*.blg`, `*.log`, and `tmp/rendered_figures/` -
    generated/rendered artifacts. Verify whether a paper task wants these
    regenerated before editing or committing them.
- `web/experiment-tracker/` - Dependency-free static browser app for entering
  Edge4AV experiment results, tracking readiness, and exporting campaign state.
  It stores edits in browser local storage.
  - `index.html` - App shell.
  - `app.js` - Tracker data model, UI behavior, import/export, and local state.
  - `styles.css` - Tracker styling.

## Reference Documents

- `references/README.md` - Human-readable IPI message-fabric design. It covers
  RSU broadcast, private-5G MQTT session, HTTP backhaul, optional V2V mesh
  planes, J2735 alignment, message catalogue, service request and cooperative
  service schemas, latency/security/privacy notes, interoperability, and
  safety/failover practices.
- `references/J2735_202309.pdf` - SAE J2735 reference PDF.
- `references/OpenIntersection.pdf` - Prior OpenIntersection reference paper.
- `references/Function Design Document for Intersection Programming Interface (IPI).docx`
  - Functional design source document.
- `references/Supported Applications Document for Intersection Programming Interface (IPI).docx`
  - Supported-application source document.

## Routing Rules For Future Agents

- For C++ model, API, encoding, mesh, or private-5G changes, start in `cpp/`
  and run the CMake/CTest flow above when feasible.
- For Mocar radio deployment, start with `README.md`, `setup.md`,
  `instructions.md`, and `third_party/mocar/J2735-2020/samples/`.
- For Mocar RTT work, route to
  `third_party/mocar/J2735-2020/samples/ipi_custom_rtt/`; keep initiator-side
  RTT timing explicit and do not require synchronized clocks for RTT.
- For one-way latency claims, verify `clock_sync_state` and supporting
  metadata. Without synchronized clocks, report RTT only.
- For experiment evidence, route to `results/real_5g/` and preserve run IDs,
  condition IDs, sender/receiver logs, base-station artifacts, GPS paths, and
  summary markdown.
- For GPS tasks, use `scripts/record_gps_for_experiment.sh`,
  `scripts/gps_topic_recorder.py`, and run-specific GPS folders under
  `results/real_5g/`.
- For paper claims, verify the current evidence in `results/`, generated
  figures, and paper sections before broadening conclusions.
- For static tracker tasks, use `web/experiment-tracker/` and verify in a local
  browser or simple HTTP server when UI behavior changes.
- For public V2X dataset/benchmark tasks, start with `benchmarks/v2x/README.md`
  and `benchmarks/v2x/sources.json`; preserve downloaded data under ignored
  `benchmarks/v2x/data/`, preserve external benchmark code under ignored
  `benchmarks/v2x/repos/`, and record derived IPI/ML results under
  `results/v2x_benchmarks/`.
- For documentation-only tasks, update `current_task.md` with status and
  validation notes even when code tests are not run.
