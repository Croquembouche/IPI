# Agent Context

This repository contains the Intersection Programming Interface (IPI) and the
Edge4AV experiment workspace. It combines a C++17 reference implementation,
private-5G latency tools, Mocar V2X assets, ROS 2 message assets, V2X benchmark
staging, and collected experiment results.

## Root Markdown Files

The repository root documentation is intentionally reduced to:

- `README.md` - public overview of the IPI protocol, current implementation,
  build path, repository structure, and release boundaries.
- `AGENTS.md` - Codex entry point and working rules.
- `agent_context.md` - this repository map.
- `current_task.md` - mutable handoff for the active task.
- `setup.md` - build and deployment guide.
- `experiment_summary.md` - detailed summary of the experiment results.
- `remaining_exp.md` - pending private-5G experiment runbook, collection
  protocol, acceptance criteria, and completion checklist.

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
- Do not take the easiest path for paper writing. Take the more correct path:
  rebuild the section logic from the user's stated goal, the section
  instruction, the three insights, the experiment evidence, and the necessary
  citations. If existing prose is flawed or tainted by prior mistakes, discard
  that structure instead of patching around it.
- `Mocar` is the company/vendor name for devices or SDK components; do not use
  it as the system name, paper title, or contribution.
- `Edge4AV` is the title of the paper, not the name of the system.
- This paper is not about building a testbed. Avoid testbed-construction
  framing as the main contribution.
- The target venue is MobiCom. Keep framing at a MobiCom networking/systems
  level and do not switch to a MobiSys-style testbed paper unless instructed.
- Each section may contain a brief user-written instruction for what that
  section should cover. Read and follow that local section instruction strictly
  before drafting or revising the section.
- Preserve the user's intended claim structure before improving wording or
  adding synthesis. Do not invent intermediate framings, replace the thesis, or
  turn evidence interpretation from `experiment_summary.md` into the paper's
  central logic unless the user explicitly requests that change.
- Do not introduce terminology that is not common in the relevant technical
  area. Avoid assistant-created shorthand such as compressed labels that are not
  established CAV, V2X, private-5G, or networking terminology. Use explicit
  wording from the user's draft, section instructions, repo docs, experiment
  summaries, or established technical vocabulary.
- Do not compress multi-step reasoning into one sentence when the reader needs
  the steps spelled out. Avoid vague bridge phrases; make the logical link
  between application categories, experiment results, and paper claims explicit.
- For the abstract specifically, keep the controlling structure as the user
  stated it: the paper asks whether today's edge communication technologies are
  ready for tomorrow's CAV applications; it divides the task into stateless
  applications and stateful/complex collaborative applications; and the results
  lead to the three user-defined insights about V2X, private 5G, and the need
  for new networking or communication technology.
- Paper pass condition: the result must be a fully logical MobiCom research
  paper with precise research word choices, readable sentence structure, clear
  paragraph/section links, and claims that fully use the supported experimental
  results.
- Paper fail conditions: ambiguous wording, confusing sentence structure,
  missing logical links between paragraphs or sections, and underclaiming
  evidence-backed results.
- Keep private-5G claims scoped to the measured deployment.
- Use RTT for private-5G request/response timing unless clock synchronization is
  explicitly supported.
- In paper prose, use `private 5G` only on the first mention; after that, use
  `5G networks` or `5G network paths`.
- Treat V2X/Mocar claims as measured device-path results, with signal-strength
  and mobility caveats preserved.
- The V2X claim is a joint signal-strength and packet-size limit. The moving
  runs provide normalized signal strength; the separate stationary sweeps show
  that larger packets fail first as signal weakens. Point 1 is building-
  obstructed NLOS. These results lead to Insight 1: current direct V2X supports
  compact J2735 messages only within a limited payload and coverage envelope.
- The private-5G detector replay supports a conservative application-content
  ceiling of approximately 20 KiB for a 500-ms p95 deadline on the evaluated
  TCP/MQTT path. Keep this value in the Results evidence and experiment summary,
  not in the top-level Insight 2 sentence. The boundary was measured with one
  UE, a dedicated 40-MHz n48 channel, and no ambient contention.
- Classify every retained historical 40/40/20 versus follow-up 70/20/10
  comparison as inconclusive for TDD inference. The application measurements
  remain valid observations of their recorded operating conditions, but TDD
  and placement/signal changed jointly. Do not rank the profiles, estimate a
  TDD effect, or call the data a TDD sensitivity result. The 70/20/10
  directional throughput control is also a single-profile measurement, not a
  TDD comparison. Only a newly collected same-placement matched experiment may
  support a within-deployment TDD conclusion.
- Preserve exactly three top-level insights: 1) the direct-V2X payload-and-
  coverage envelope; 2) the severe CAV packet-size limit for deadline-compliant
  5G uplink exchange; and 3) the need for improved vehicular uplink performance
  and concurrent-demand isolation in 5G/6G systems.

## Agent Operating Boundaries

- Use root `AGENTS.md` for repo-specific guidance. Universal Codex preferences
  belong in `~/.codex/AGENTS.md`; nested `AGENTS.md` files should be added only
  when a subdirectory needs different rules.
- Keep the root Markdown set limited to `README.md`, `AGENTS.md`,
  `agent_context.md`, `current_task.md`, `setup.md`, `experiment_summary.md`,
  and `remaining_exp.md`.
- Do not copy a generic AGENTS/supporting-doc bundle into this repo. Add new
  supporting Markdown only when it is project-specific and the user asks for it
  or the task clearly needs it.
- For large paper rewrites, experiment redesigns, migrations, refactors, or
  architecture changes, avoid local-patch bias. First identify the intended end
  state, affected files/subsystems, validation evidence, and checkpoint order.
- For novel research or system-building work, separate observations,
  inferences, assumptions, hypotheses, unknowns, risks, and decisions when those
  distinctions affect the work.
- Treat research as both discovery and system composition. When combining
  existing modules, define the contracts, adapters, invariants, failure modes,
  and the specific twist that makes the composition research-relevant.
- Use subagents only for bounded read-heavy work such as mapping, triage,
  logs, tests, paper-claim review, or risk review. Do not use them for
  uncontrolled parallel editing.
