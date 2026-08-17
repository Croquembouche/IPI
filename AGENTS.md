# AGENTS.md

This is the Codex entry point for the IPI repository.

The repository root intentionally keeps only these Markdown files:

- `README.md`
- `AGENTS.md`
- `agent_context.md`
- `current_task.md`
- `setup.md`
- `experiment_summary.md`
- `remaining_exp.md`

Nested Markdown files may exist under result, source, benchmark, reference, or
paper folders and should stay in their respective locations.

## Required Read Order

Before making changes, read:

1. `current_task.md` - active task, status, touched files, validation state, and
   next steps.
2. `agent_context.md` - repository map, result routing, and working rules.

Read `setup.md` for deployment/build work. Read `experiment_summary.md` for
paper, experiment, or results questions. Read `remaining_exp.md` before planning
or running any pending experiment.

## Paper-Writing Rule

Do not use Spec Kit, superpowers, or old generated manuscript prose for paper
writing. Start from the current files, the current evidence, and the user's
explicit instructions.

For this paper:

- Do not take the easiest path when writing or revising paper sections. Take
  the more correct path: rebuild the logic from the user's stated goal, the
  section instruction, the three insights, the experiment evidence, and the
  necessary citations, even when that requires discarding a flawed draft instead
  of patching it.
- `Mocar` is a company/vendor name. Do not present it as the paper title, the
  system name, or the research contribution.
- `Edge4AV` is the paper title, not the system being proposed or evaluated.
- This is not a testbed-building paper. Do not frame the contribution as
  designing or constructing a testbed.
- This is a MobiCom paper. Use MobiCom-level networking/systems framing and do
  not drift into MobiSys-style testbed narration unless the user asks.
- Each paper section may contain a brief user-written instruction describing
  what that section should cover. Treat those section instructions as binding.
  Follow them strictly when drafting or revising that section.
- Preserve the user's intended claim structure before rephrasing. Do not invent
  intermediate framings, substitute a new thesis, or promote evidence
  interpretation into the paper's central logic unless the user explicitly asks
  for that change. For the abstract, the controlling structure is: ask whether
  today's edge communication technologies are ready for tomorrow's CAV
  applications; divide the problem into stateless applications and
  stateful/complex collaborative applications; then state the three
  user-defined insights.
- Do not introduce terminology that is not common in the relevant technical
  area. If a phrase is only assistant-created shorthand, do not use it. Prefer
  explicit wording from the user's draft, section instructions, repo docs,
  experiment summaries, or established CAV/V2X/private-5G terminology.
- Do not compress multi-step reasoning into a single sentence when it makes the
  logic hard to follow. Avoid vague bridge phrases such as "this hides the main
  question" when the paper should explicitly state the connection between the
  previous sentence and the next claim.

Paper pass condition:

- The manuscript is a fully logical MobiCom research paper.
- Research word choices are precise, natural, and appropriate for the claim
  being made.
- Sentence structure is easy to understand and easy to read.
- Paragraphs and sections have explicit logical links, so the reader can follow
  why each idea follows from the previous one.
- The paper claims the experimental results at the right strength; do not
  underclaim results that are directly supported by evidence.

Paper fail conditions:

- Ambiguous wording.
- Awkward or confusing sentence structure.
- Missing logical links between sentences, paragraphs, sections, experiments,
  results, and insights.
- Underclaiming supported experimental results.

## Research Engineering Rules

This repository includes research work and system-building work. Research can
mean building a new system from existing modules with a small but important
change in assumptions, timing, data flow, ownership, trust boundary, or
composition.

When the task is novel, architectural, or evidence-dependent:

- Do not optimize for the smallest local patch. Identify the intended end
  state, the affected boundaries, and the validation evidence before editing.
- Separate observed facts, inferences, assumptions, hypotheses, unknowns, risks,
  and decisions when that distinction matters.
- Do not present an untested hypothesis as fact.
- If evidence contradicts the current plan, revise the plan instead of
  continuing stale work.
- Prefer explicit module contracts, adapters, invariants, and failure modes over
  scattered glue code.
- If the correct answer is a negative result, failed hypothesis, sharper problem
  statement, or benchmark exposing a tradeoff, report it directly.

## Instruction Layering

Codex may load global guidance from `~/.codex/AGENTS.md`, then this repository's
root `AGENTS.md`, then any nested `AGENTS.md` closer to the working directory.
This file should stay focused on repo-specific rules. Put universal personal
preferences in `~/.codex/AGENTS.md`, and put subsystem-specific rules in nested
`AGENTS.md` files only when that subsystem needs different guidance.

Do not copy a large generic AGENTS bundle into this repository. Add supporting
Markdown only when it is project-specific and useful; keep root-level Markdown
limited to the approved root files.

## Subagent Use

Subagents are not automatic. Use them only when the user explicitly asks or when
the task clearly benefits from parallel read-heavy work such as repository
mapping, experiment triage, log analysis, test failure review, or claim/risk
review. Keep write-heavy implementation in the main agent unless the user
explicitly asks for parallel implementation.

Subagent output must be consolidated by the main agent into one decision with
file paths, evidence, risks, and remaining work.

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
