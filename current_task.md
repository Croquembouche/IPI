# Current Task

Last updated: 2026-06-29

## Task

Review the current IPI codebase and refresh the agent-facing Markdown files so
future agents can quickly understand:

- how agents should work in this repository;
- what stop, pass, and fail conditions apply;
- what each important file and folder does; and
- what the active task status is.

## Status

Complete.

## Scope

Files in scope for this task:

- `agent.md`
- `agent_context.md`
- `current_task.md`

Files reviewed for repository context:

- `README.md`
- `setup.md`
- `instructions.md`
- `cpp/CMakeLists.txt`
- `cpp/` source, examples, and tests at a routing level
- `third_party/mocar/J2735-2020/samples/`
- `scripts/`
- `results/real_5g/`
- `paper/`
- `web/experiment-tracker/`
- `ros2_ws/src/v2x_msg/`

## Work Completed

- Added an explicit three-file agent handoff contract to `agent.md`.
- Added stop conditions to `agent.md`.
- Extended pass/fail conditions for agent-documentation consistency.
- Rebuilt `agent_context.md` as a current repository map.
- Added this `current_task.md` file as the mutable task handoff.

## Validation

Documentation-only task. No C++ source code was changed.

Validation run:

```bash
git diff --check
git status --short
```

Result: `git diff --check` passed. `git status --short` shows only the
expected documentation changes:

- `agent.md`
- `agent_context.md`
- `current_task.md`

The CMake/CTest flow is not required for this documentation-only change unless
source code or build files are modified later:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

## Next Steps

No required follow-up for this documentation refresh.
