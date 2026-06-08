# Agent Context

This repository contains the Intersection Programming Interface (IPI) and
Edge4AV experiment workspace. IPI is a standards-aligned message fabric for
intelligent intersections, connected/autonomous vehicles, RSUs/OBUs, edge
services, and cloud/backhaul services. The codebase combines a C++17 reference
library, V2X and private-5G latency tooling, ROS 2 integration assets, paper
drafts, reference documents, and collected experiment results.

## Top-Level Files

- `agent_context.md` - This repository map for future agents.
- `agent.md` - Agent-facing project goals, scope boundaries, pass conditions,
  fail conditions, and working rules.
- `README.md` - High-level project overview, repository layout, C++ build
  commands, web tracker instructions, example binaries, and demo workflows for
  Edge-to-device SPaT over TCP and private-5G latency over TCP/MQTT.
- `setup.md` - Deployment guide for building, staging, and running the current
  IPI reference implementation across build host, TX edge, Mocar radios,
  private-5G sender/receiver nodes, and MQTT broker roles.
- `instructions.md` - Operator runbook for latency tests using V2X radios,
  private-5G devices, shared experiment metadata, CSV logging, and GPS capture.
- `experiment.md` - Edge4AV campaign tracker. It defines service classes,
  planned figures, core and supplementary experiment matrices, required
  metadata, GPS recording requirements, and result tables.
- `experiment-tracker.html` - Root-level convenience entry point for the static
  browser experiment tracker in `web/experiment-tracker/`.
- `.gitignore` - Ignores generated build outputs, vendored bridge build output,
  temporary files, paper build artifacts, and ROS workspace build/install/log
  products.
- `.agents/` and `.codex/` - Empty local agent configuration directories at the
  time this context file was created.
- `.git/` - Local Git metadata directory. `git status` failed in this workspace
  during context creation even though the directory exists, so use filesystem
  inspection as the fallback source of truth when Git commands are unavailable.
- `.vscode/` - Local editor settings.

## Source and Build Trees

- `cpp/` - C++17 reference implementation and examples.
  - `cpp/CMakeLists.txt` - Builds the static `ipi` library, examples, optional
    ROS 2 bridge code, optional Mocar examples, and unit tests when
    `IPI_ENABLE_TESTS=ON`.
  - `cpp/README.md` and `cpp/LOGIC.md` - C++ layout, architecture notes,
    feature summary, bridge design sketch, mesh mode, task offloading, build
    options, and next steps.
  - `cpp/include/ipi/core/` - Public data models for IPI service requests,
    cooperative service messages, message frames, and validation.
  - `cpp/include/ipi/v2x/` - Public J2735 helper models, UPER codec facade, and
    optional ROS 2 bridge interface.
  - `cpp/include/ipi/api/` - Public API facades for sender/receiver semantics,
    Edge4AV dual-plane behavior, experiment logging, private-5G latency probes,
    MQTT client support, and private session transport.
  - `cpp/include/ipi/mesh/` - Public mesh manager and cooperative task
    offloader interfaces for degraded infrastructure or V2V scenarios.
  - `cpp/include/ipi/common/` - Shared debugging helpers.
  - `cpp/src/core/`, `cpp/src/v2x/`, `cpp/src/api/`, `cpp/src/mesh/` -
    Implementations for the corresponding public headers.
  - `cpp/examples/library/` - Host-side example tools:
    `build_service_request`, `v2x_roundtrip`, `spat_tcp_sender`,
    `private_5g_latency_sender`, `private_5g_latency_receiver`,
    `mesh_demo`, and `edge4av_dual_plane`.
  - `cpp/examples/device/` - Hardware/ROS-facing examples:
    `mocar_ipi_demo`, `ros2_bsm_broadcaster`, and `spat_tcp_bridge`.
  - `cpp/tests/` - Unit and loopback tests for Edge4AV API behavior,
    experiment logging, private session transport, and private-5G latency
    probes.
  - `cpp/build/` - Generated CMake build tree and compiled binaries currently
    present in the workspace. Treat as an artifact unless debugging a local
    build.

## Experiment, Paper, and Web Assets

- `paper/` - IEEE-style paper workspace for the Edge4AV study.
  - `paper/IEEE-conference-template-062824.tex` - Main LaTeX paper entry.
  - `paper/sections/` - Paper sections, including the abstract, introduction,
    system design, experimental methodology, discussion/conclusion, and a
    detailed experiment TODO/planning section.
  - `paper/figs/` - Figure directory.
  - `paper/*.bib`, `paper/IEEEtran*.bst`, `paper/IEEEtran.cls` - Bibliography
    and IEEE template support files.
  - `paper/*.pdf`, `paper/*.aux`, `paper/*.bbl`, `paper/*.blg`,
    `paper/*.log` - Rendered or generated paper artifacts. The top-level
    `.gitignore` ignores the `paper/` directory, so verify expectations before
    treating these as source deliverables.
- `web/experiment-tracker/` - Dependency-free static browser app for entering
  Edge4AV experiment results, tracking readiness, and exporting campaign state
  as JSON or Markdown. It stores edits in browser local storage.
  - `index.html` - App shell.
  - `app.js` - Tracker data model, UI behavior, import/export, and local state.
  - `styles.css` - Tracker styling.

## ROS, Device SDK, and Third-Party Assets

- `ros2_ws/` - ROS 2 workspace used for GPS and V2X message integration.
  - `ros2_ws/src/v2x_msg/` - ROS 2 message package with J2735-style V2X message
    definitions such as BSM, MAP, SPAT, SRM, SSM, TIM, and supporting message
    types.
  - `ros2_ws/src/novatel_oem7_driver/` - NovAtel OEM7 ROS 2 driver source and
    packaging scripts used by GPS recording helpers.
  - `ros2_ws/build/`, `ros2_ws/install/`, `ros2_ws/log/` - Generated ROS 2
    workspace artifacts currently present. Treat as local build output.
- `third_party/mocar/` - Vendored Mocar V2X SDK material for RSU/OBU hardware
  demos.
  - `third_party/mocar/J2735-2020/` - J2735 SDK archive, README, libraries,
    headers, and sample code. The IPI SPaT bridge sample is built separately
    from the main CMake project.
  - `third_party/mocar/J2735-2020/Readme.txt` - Vendor SDK readme.
  - `third_party/mocar/J2735-2020/samples/ipi_spat_bridge/` - Device-side TCP
    to Mocar SPaT bridge build target referenced by `README.md`, `setup.md`,
    and `instructions.md`.

## Scripts and Results

- `scripts/` - Utility scripts for experiment support.
  - `build_real5g_dashboard.py` - Reads private-5G sender CSVs and writes a
    static results dashboard with aggregate p50/p95/p99 latency, success rate,
    payload sensitivity, comparisons, trials, and GPS snapshot data.
  - `gps_topic_recorder.py` - ROS 2 node that records NovAtel GPS position and
    speed topics to CSV plus a latest-sample snapshot.
  - `record_gps_for_experiment.sh` - Launches the NovAtel OEM7 driver and GPS
    recorder for a specific experiment result directory.
  - `scripts/__pycache__/` - Generated Python bytecode artifacts.
- `results/` - Local and real private-5G experiment output.
  - `results/local_loopback/` - Local loopback experiment outputs when present.
  - `results/real_5g/` - Real private-5G campaigns grouped by dated run
    directories. Each run typically contains command logs, metadata markdown,
    sender CSV/error files, environment summaries, base-station artifacts, GPS
    recordings, generated helper scripts, and summary markdown.
  - `results/visualize/` - Generated or supporting visualization output area.

## Reference Documents

- `references/` - Specification and source reference material.
  - `references/README.md` - Human-readable IPI message-fabric design. It
    defines the RSU broadcast, 5G MQTT session, HTTP backhaul, and optional V2V
    mesh planes; J2735 alignment; message catalogue; service request and
    cooperative service schemas; cross-plane latency/security/privacy notes;
    developer enablement; interoperability; and safety/failover practices.
  - `references/J2735_202309.pdf` - SAE J2735 reference PDF.
  - `references/OpenIntersection.pdf` - Prior OpenIntersection reference paper.
  - `references/Function Design Document for Intersection Programming Interface (IPI).docx`
    - Functional design source document.
  - `references/Supported Applications Document for Intersection Programming Interface (IPI).docx`
    - Supported-application source document.

## Development Notes

- Main C++ build:

  ```bash
  cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
  cmake --build cpp/build
  ctest --test-dir cpp/build --output-on-failure
  ```

- Optional CMake flags:
  - `-DIPI_BUILD_EXAMPLES=ON` builds example executables.
  - `-DIPI_ENABLE_TESTS=ON` builds tests.
  - `-DIPI_ENABLE_ROS2_BRIDGE=ON` enables ROS 2 bridge code.
  - `-DIPI_ENABLE_MOCAR_EXAMPLES=ON` enables Mocar SDK examples when SDK
    libraries are available.

- Mocar SPaT bridge build:

  ```bash
  make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge
  ```

- GPS capture is required for future vehicle-side experiments and should be
  started before latency trials:

  ```bash
  scripts/record_gps_for_experiment.sh results/real_5g/<timestamp>
  ```

- Preserve experiment traceability. Sender, receiver, bridge, GPS, and summary
  artifacts should share `run_id`, `condition_id`, `condition_label`,
  `request_id`, `network_load_level`, `qos_profile`, `mobility_state`, and
  `clock_sync_state` wherever the tooling supports them.
