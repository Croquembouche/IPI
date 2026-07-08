# Current Task

Last updated: 2026-07-08

## Task

Clean up root-level repository Markdown so that only these files remain at the
repository root:

- `AGENTS.md`
- `agent_context.md`
- `current_task.md`
- `setup.md`
- `experiment_summary.md`

Create root-level `experiment_summary.md` as a detailed summary of experiments under
`results/`, including private 5G, Mocar V2X, and V2X dataset benchmark results.

Correction from user: only root-level Markdown files should be cleaned up.
Markdown files in subdirectories should remain in their respective locations.

## Status

Complete for tracked Markdown files; partial for untracked nested Markdown files.

The root-level Markdown set has been reduced to the five requested files.
The retained docs are now self-contained and no longer route paper writing
through Spec Kit, superpowers, or deleted Markdown planning files.

Tracked non-root Markdown files that were deleted by the first cleanup pass have
been restored.

Important limitation: untracked nested Markdown files that existed only in the
working tree before the overbroad cleanup cannot be restored exactly from git.
The affected empty/untracked areas include `.agents/skills/*`, `.specify/memory`,
`.specify/templates`, `specs/*/checklists`, `specs/*/contracts`, and paper/spec
Markdown files under untracked directories. The TeX, PDF, figure, script, CSV,
JSON, and log artifacts in those folders remain present.

## Files Touched

- `AGENTS.md`
- `agent_context.md`
- `current_task.md`
- `experiment_summary.md`
- restored tracked non-root `*.md` files after the root-only scope correction

## Experiment Summary Coverage

`experiment_summary.md` now summarizes:

- private-5G baseline TCP/MQTT payload sweeps;
- private-5G signal maps and run locations;
- private-5G load, QoS-label, and QoS verification runs;
- detector-output replay over TCP, MQTT, raw UDP, and fragmented UDP;
- end-to-end deadline analysis;
- multiclient scalability under good and weak signal conditions;
- failure/fallback restart runs;
- Mocar setup/debug, stationary payload sweeps, OBU signal probe, and mobility
  runs;
- V2X dataset source registry, IPI loopback, four-GPU artifact processing,
  OpenCOOD smoke checks, and V2X-Radar detector benchmarks.

## Validation

- Restored 148 tracked non-root Markdown files with git after the user clarified
  the cleanup was root-level only.
- Ran `git ls-files --deleted '*.md'` to verify only old root-level tracked
  Markdown files remain deleted.
- Ran `find . -type f -name '*.md' ! -path './.git/*' | sort` to verify nested
  Markdown files are present again.
- Checked untracked nested folders. No exact git copy exists for deleted
  untracked Markdown files in `.agents/`, `.specify/`, `specs/`, or `paper/`.
- Did not run C++ build/tests because this was a documentation cleanup and
  result-summary task only.

## Next Steps

Use `experiment_summary.md` as the first evidence summary for future paper or
experiment discussion, then inspect raw artifacts under `results/` when exact
rows or full logs are needed.
