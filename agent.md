# Agent Guide

## Project Goals

The project goal is to evaluate and demonstrate Edge4AV, an empirical
deployment study built around the Intersection Programming Interface (IPI). IPI
connects intelligent intersections, CAVs/CVs, pedestrians, peer intersections,
edge services, and cloud services through a common SAE J2735-oriented message
fabric.

The main technical goals are:

- Preserve SAE J2735 `MessageFrame` payloads end to end across broadcast V2X,
  private-5G session transport, and HTTP/backhaul paths.
- Provide a C++17 reference implementation for IPI data models,
  validation/encoding helpers, receiver/sender APIs, private-5G latency probes,
  optional ROS 2/Mocar adapters, V2V mesh behavior, and cooperative task
  offloading.
- Support Edge4AV experiments that characterize nominal latency, scaling,
  crowded-network behavior, QoS impact, edge-offload tradeoffs, and
  vehicle-level outcomes.
- Keep experiment logs traceable across AV, RSU, edge, network, bridge, and GPS
  artifacts through shared run/condition/request metadata.
- Produce paper-ready evidence for an operational-envelope claim: under what
  measured conditions can edge-assisted private-5G intersection infrastructure
  support selected AV services?

The main paper should focus on two service classes:

- Intersection-state assistance, currently exercised through SPaT/MAP-style
  radio and private-5G probes.
- Fallback or guided-planning assistance, currently exercised through
  `IPI-CooperativeService` payloads.

## Scope and Claim Boundaries

Agents should keep the project framed as a deployment and operational-envelope
study, not as an unrestricted public-5G proof or a completed multi-vendor
interoperability claim.

Claims that are in scope:

- Baseline radio/private-5G TCP/private-5G MQTT latency envelopes.
- Tail growth and service fragility as private-5G load increases.
- QoS or priority-treatment effects where the deployment exposes them.
- Fairness and scaling behavior using physical devices plus emulated clients.
- Edge-offload versus RSU-local compute tradeoffs.
- Vehicle-level impact when real outcome data is available.

Claims to avoid unless real data is added:

- Broad public-5G equivalence.
- Multi-vendor interoperability.
- Production cooperative-perception performance without a real end-to-end
  perception path.
- Operator-specific RAN scheduling, TDD, HARQ/BLER, handover, or core-network
  behavior not measured directly in the testbed.

## Pass Conditions

A change or experiment pass is credible when all relevant items below are true:

- C++ code builds from the repository root with:

  ```bash
  cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
  cmake --build cpp/build
  ctest --test-dir cpp/build --output-on-failure
  ```

- New C++ behavior follows the existing module boundaries:
  - data models and validation in `cpp/include/ipi/core` and `cpp/src/core`;
  - J2735/UPER helpers in `cpp/include/ipi/v2x` and `cpp/src/v2x`;
  - transport/API behavior in `cpp/include/ipi/api` and `cpp/src/api`;
  - mesh/offload behavior in `cpp/include/ipi/mesh` and `cpp/src/mesh`;
  - examples under `cpp/examples`;
  - tests under `cpp/tests`.
- New experiment tooling preserves the shared logging schema: `run_id`,
  `condition_id`, `condition_label`, `request_id`, `service_type`, `transport`,
  `av_id`, `obu_id`, `rsu_id`, `network_load_level`, `qos_profile`,
  `mobility_state`, `clock_sync_state`, accepted/success fields, and timing
  fields where applicable.
- Private-5G latency results report p50/p95/p99, success rate, jitter or tail
  behavior, sample count, condition metadata, and clock-sync state.
- Vehicle-side experiments include GPS recording artifacts or explicitly record
  why GPS data is unavailable.
- Paper-facing claims are backed by current artifacts in `results/`, the web
  tracker, or paper figures/tables. Placeholders do not count as evidence.
- Documentation updates stay consistent across `README.md`, `setup.md`,
  `instructions.md`, `experiment.md`, `references/README.md`, and relevant
  `cpp/` docs when behavior or workflow changes.

## Fail Conditions

Treat work as failed or incomplete when any relevant item below is true:

- The C++ library or tests no longer build with the standard CMake/CTest flow.
- A change silently alters the IPI wire model, message encodings, topic
  semantics, or experiment log fields without updating docs and tests.
- New code bypasses the existing data model, validation, API, or mesh module
  boundaries without a clear reason.
- Experiment data lacks run/condition/request metadata, sample counts, or
  enough timing fields to reproduce p50/p95/p99 and success-rate summaries.
- One-way latency is claimed from unsynchronized clocks without clearly marking
  it as untrustworthy; RTT remains valid in that case.
- Generated artifacts in `cpp/build`, `ros2_ws/build`, `ros2_ws/install`,
  `ros2_ws/log`, `scripts/__pycache__`, or paper build outputs are treated as
  source deliverables without explicit intent.
- The paper or documentation claims public-5G equivalence, multi-vendor
  interoperability, or full cooperative-perception results without supporting
  testbed evidence.
- Receive-side Mocar sample code is treated as a drop-in receive-only harness
  without reviewing whether its `main()` also transmits.
- Vehicle-side experiment summaries omit GPS status, GPS paths, or a reason GPS
  was not collected.

## Agent Working Rules

- Read the current files before changing behavior. This workspace contains
  local generated artifacts and collected results, so use current filesystem
  state as authoritative.
- Prefer narrow, traceable edits that match existing C++17, CMake, Markdown,
  Python, ROS 2, and static-web patterns.
- Do not delete experiment outputs, generated logs, build trees, or vendored SDK
  files unless the user explicitly asks.
- When modifying experiments, preserve command logs and metadata needed to join
  sender, receiver, bridge, base-station, GPS, and summary artifacts.
- When adding tests, keep them focused on the changed behavior and use the
  existing no-framework executable test style unless the project changes its
  test harness.
- When updating paper text, keep claims bounded to the evidence currently in the
  repository and move speculative ablations to supplementary or future-work
  framing.
