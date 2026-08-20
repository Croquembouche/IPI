# MobiCom Academic-Writing Review

Reviewer: GPT-5.6 Terra, maximum reasoning effort

Scope: current manuscript source (`paper/current_manscript/main.tex` and Sections
00--08) and the current 16-page rendered PDF

Review mode: read-only; no manuscript, figure, script, or PDF was edited

## Verdict

**Major revision.** The paper has a coherent experiment set, but the prose
needs a manuscript-wide consistency pass. The principal problem is that some
sentences shift from evaluating CAV application workloads through correlated
request/response exchanges to language that can be read as evaluating completed
applications or services. Metric names, technical terms, abbreviations,
captions, and transitions also vary across sections.

Finding count: **P0: 1; P1: 3; P2: 1.**

## Findings

### P0. Keep the evaluation boundary at the workload request/response level

Locations:

- `paper/current_manscript/sections/04_system_design_setup.tex:246--253`
- `paper/current_manscript/sections/05_experiment_results.tex:78--83`
- `paper/current_manscript/sections/06_future_research_directions.tex:16--18`
- `paper/current_manscript/sections/06_future_research_directions.tex:35--36`

Phrases such as "complete workload objects," "large transfer completion,"
"complete results," and services that "complete in time" can move the claim
from the measured workload exchange to completion of the application or
service. The paper should consistently state that the experiments carry CAV
application workloads in IPI frames and measure the arrival of correlated
responses within a deadline.

Representative replacement:

> The matrix evaluates request/response delivery of workload objects within
> each deadline, foreground deadline availability under competing demand, and
> response continuity after restart.

### P1. Use one metric vocabulary throughout the paper

Locations:

- `paper/current_manscript/sections/00_abstract.tex:18--24`
- `paper/current_manscript/sections/05_experiment_results.tex:125--132`
- `paper/current_manscript/sections/07_conclusion.tex:14--19`

The paper alternates among "reliably carries," "completion," "complete-object
loss," "dependable," and "availability." This variation makes it unclear
whether each term denotes the same measured outcome.

Recommended vocabulary:

1) **request/response round-trip time (RTT)** for the time from transmission of
the request to receipt of its correlated response;

2) **all-attempt deadline availability** for the fraction of all attempts whose
correlated response arrived within the stated deadline;

3) **received response** and **missing response** for per-attempt outcomes.

After defining these terms once, use them consistently in prose, captions,
legends, and the conclusion.

### P1. Make abbreviations and captions self-contained

Locations:

- `paper/current_manscript/sections/00_abstract.tex:11--13`
- `paper/current_manscript/sections/04_system_design_setup.tex:62--67`
- `paper/current_manscript/sections/05_experiment_results.tex:258--264`

PC5, Uu, RTT, and \(A_{\mathrm{cycle}}\) are not always explained where a
reader first encounters them or where a figure may be read independently.
Define PC5 and Uu as the direct and network-assisted Third Generation
Partnership Project (3GPP) interfaces, respectively. A caption that reports
availability should also identify its denominator: every issued attempt,
including missing and late responses.

### P1. Strengthen the transition from Related Work to IPI

Locations:

- `paper/current_manscript/sections/02_related_work.tex:57--61`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex:4--8`

Related Work ends with a common-application-level evaluation gap, but the IPI
section opens with a definition rather than explicitly showing how IPI enables
that comparison.

Representative replacement for the opening of Section 3:

> To make that common application-level comparison possible, IPI represents
> conventional messages and correlated service operations through one typed
> interface.

The next sentence can then explain how the interface complements SAE J2735
message definitions and Vehicular Application Enabler session services.

### P2. Replace the workload shopping list with an argumentative sequence

Location:

- `paper/current_manscript/sections/04_system_design_setup.tex:226--239`

One paragraph currently combines workload provenance, IPI encoding, detector
object sizes, dataset transfer sizes, transport protocols, traffic direction,
background traffic, logical-client demand, quality-of-service labels, restart,
and an appendix pointer. Although each fact is relevant, the paragraph reads as
an inventory and obscures why the experiments progress from one factor to the
next.

Split it into three linked paragraphs:

1) establish workload provenance and explain how each workload is represented
in IPI;

2) introduce the transport comparison and explain which delivery behavior it
tests;

3) introduce direction, demand, and interruption as factors that test whether
the path sustains those workloads under competition and state disruption.

## Recommended Revision Order

1) Define the measured request/response boundary and the four preferred metric
terms once in System Design and Setup.

2) Apply those terms consistently to the abstract, results prose, captions,
future directions, and conclusion.

3) Rewrite the cited workload-description paragraph as a causal experimental
sequence rather than a list.

4) Repair the Related Work-to-IPI transition and then inspect every remaining
section boundary for the same question-to-answer relationship.

5) Finish with an abbreviation and caption-only pass so that abbreviations are
expanded at first manuscript use and each result figure can be understood
without reconstructing its denominator from the main text.

## Review Boundary

This review evaluates academic word choice, sentence structure, logical
linking, terminology consistency, abbreviation handling, captions, and
MobiCom-appropriate argumentative flow. It does not change the paper's claims,
reanalyze the data, or edit the manuscript.
