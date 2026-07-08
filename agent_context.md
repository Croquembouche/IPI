# Agent Context

This repository contains the Intersection Programming Interface (IPI) and the
Edge4AV experiment workspace. It combines a C++17 reference implementation,
private-5G latency tools, Mocar V2X assets, ROS 2 message assets, V2X benchmark
staging, and collected experiment results.

## Root Markdown Files

The repository root documentation is intentionally reduced to:

- `AGENTS.md` - Codex entry point and working rules.
- `agent_context.md` - this repository map.
- `current_task.md` - mutable handoff for the active task.
- `setup.md` - build and deployment guide.
- `experiment_summary.md` - detailed summary of the experiment results.

Nested Markdown files under result, source, benchmark, reference, or paper
folders may exist and should remain in their respective locations.

## Source Map

- `cpp/` - C++17 IPI library, examples, and tests.
  - Core models: `cpp/include/ipi/core`, `cpp/src/core`.
  - V2X helpers: `cpp/include/ipi/v2x`, `cpp/src/v2x`.
  - API and private-5G transport: `cpp/include/ipi/api`, `cpp/src/api`.
  - Mesh/offload logic: `cpp/include/ipi/mesh`, `cpp/src/mesh`.
  - Examples: `cpp/examples`.
  - Tests: `cpp/tests`.
- `third_party/mocar/J2735-2020/` - vendored Mocar SDK material and samples.
  - `samples/ipi_spat_bridge/` is the SPaT bridge deployment path.
  - `samples/ipi_custom_rtt/` is the custom Mocar radio RTT probe path.
- `ros2_ws/src/v2x_msg/` - ROS 2 J2735-style message definitions.
- `scripts/` - experiment collection, analysis, plotting, V2X benchmark, GPS,
  and load-generation scripts.
- `benchmarks/v2x/sources.json` - current V2X source/dataset registry.
- `web/experiment-tracker/` - static browser experiment tracker.
- `results/` - collected experiment evidence. Keep raw files intact unless the
  user explicitly asks to remove them.

## Results Map

- `results/real_5g/` - private-5G TCP, MQTT, UDP, load/QoS, multiclient,
  detector replay, failure/fallback, deadline-analysis, GPS, and signal-map
  results.
- `results/mocar_v2x/` - Mocar V2X setup, payload sweep, signal-probe, and
  radio-distance/mobility results.
- `results/v2x_benchmarks/` - V2X dataset loopback, GPU dataset processing,
  OpenCOOD smoke, and V2X-Radar detector benchmark results.
- `results/local_loopback/` - currently present but empty.

Use `experiment_summary.md` first for the current high-level experiment summary,
then inspect raw artifacts under `results/` when exact evidence is needed.

## Validation Commands

For C++ changes:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

For paper or experiment-claim changes, validate against current artifacts in
`results/` and avoid using stale generated manuscript text as evidence.

## Paper-Writing Boundaries

- Do not use Spec Kit or superpowers for paper writing.
- Do not reuse old Codex-generated paper prose as authority.
- Start from the user's current draft files and instructions.
- Keep private-5G claims scoped to the measured deployment.
- Use RTT for private-5G request/response timing unless clock synchronization is
  explicitly supported.
- Treat V2X/Mocar claims as measured device-path results, with placement and
  mobility caveats preserved.
