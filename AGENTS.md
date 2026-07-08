# AGENTS.md

This is the Codex entry point for the IPI repository.

The repository root intentionally keeps only these Markdown files:

- `AGENTS.md`
- `agent_context.md`
- `current_task.md`
- `setup.md`
- `experiment_summary.md`

Nested Markdown files may exist under result, source, benchmark, reference, or
paper folders and should stay in their respective locations.

## Required Read Order

Before making changes, read:

1. `current_task.md` - active task, status, touched files, validation state, and
   next steps.
2. `agent_context.md` - repository map, result routing, and working rules.

Read `setup.md` for deployment/build work. Read `experiment_summary.md` for
paper, experiment, or results questions.

## Paper-Writing Rule

Do not use Spec Kit, superpowers, or old generated manuscript prose for paper
writing. Start from the current files, the current evidence, and the user's
explicit instructions.

## Working Rules

- Use the current filesystem as the source of truth.
- Keep claims tied to stored artifacts in `results/`.
- Do not invent terminology or broaden claims beyond the evidence.
- Do not delete raw result artifacts, generated experiment logs, vendored SDK
  files, or user-authored source content unless the user explicitly asks.
- Update `current_task.md` whenever the active task or validation state changes.

## Definition Of Done

A task is done only when:

- requested files are updated;
- relevant validation has passed or is explicitly marked not run;
- experiment or paper claims remain within current evidence;
- `current_task.md` reflects the final state.
