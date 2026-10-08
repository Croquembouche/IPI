# IPI research and implementation

This workspace combines the C++ IPI implementation, ROS 2 integration, private-5G
and direct-V2X experiments, and the current MobiCom manuscript.

## Context by task

- Resuming work: the relevant recent entry in `current_task.md`; use `agent_context.md` for repository routing and detailed paper/evidence conventions.
- Build or deployment: `setup.md` and the affected component's sources.
- Paper or results: current files in `paper/current_manscript/`, local section instructions, `experiment_summary.md`, and relevant `paper/analysis/` summaries; trace numerical claims to `results/`.
- Planning or running pending experiments: `remaining_exp.md`.

Root Markdown is limited to `README.md`, `AGENTS.md`, `agent_context.md`,
`current_task.md`, `setup.md`, `experiment_summary.md`, and `remaining_exp.md`.
Keep supporting documents within their existing subject directories.

## Manuscript constraints

- Work directly from current user-authored instructions, manuscript, and evidence. Do not use Spec Kit, superpowers, or old generated drafts as authority.
- The title is “Can Today's Communication Technologies Support Tomorrow's Connected and Automated Vehicles?” `Mocar` is a vendor; `Edge4AV` is a legacy label. Neither names the paper's proposed system or contribution.
- Preserve the networking/systems question and the three user-defined insights. This is not a testbed-construction paper. Follow current section instructions for argument order; do not replace the thesis with a new interpretation.
- For abstract structure and measurement-specific conventions, use the current section instructions and the detailed paper rules in `agent_context.md`. Preserve the distinction between application categories and communication patterns.
- Use established technical terms, explicit logical connections, and readable sentences. Claim supported results fully, without extending them beyond their evidence.
- Use RTT for unsynchronized request/response measurements. Keep workload direction, payload definition, deployment conditions, and TDD comparison context attached to claims; do not turn confounded comparisons into causal TDD effects.

## Engineering and evidence

Preserve raw results, experiment logs, vendored SDKs, and user source unless
removal is requested. For architectural work, resolve the affected module
contracts, timing, ownership, and failure behavior rather than applying a patch
that misses the intended outcome. Distinguish implemented components from
integrated, exercised end-to-end behavior; report unsupported hypotheses and
negative results directly.

C++ validation uses:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

For manuscript edits, use its existing build, check citations and final rendered
pages, and verify changed claims against stored evidence. Do not rerun physical
experiments merely to validate prose. Complete the requested files and relevant
checks, preserve unrelated work, and record the resulting task/validation state
in `current_task.md` without rewriting historical entries.
