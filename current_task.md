# Current Task

Last updated: 2026-08-14

## Joint TDD, Signal-Context, and Three-Insight Revision

Complete. The historical `40/40/20` and locked-cell `70/20/10` application
campaigns are no longer presented as a TDD-only comparison. The manuscript and
experiment summaries now compare four joint operating conditions: historical
`40/40/20` at the favorable placement, and `70/20/10` at common/typical, weak,
and strong placements. The same sole CAV UE, dedicated 40-MHz n48 radio and
channel, Cell 2 path, and `10D4G` packing support comparison across collection
times; the signal/placement context remains part of every result.

Figure 9 was rebuilt as two aligned heatmaps. Rows combine TDD profile and
signal/placement context, while columns separate 1-KiB and 23,968-B MQTT/TCP
workloads. The figure shows that 1-KiB p95 remains comparatively stable within
each transport, whereas 23,968-B 100-ms availability under the same
`70/20/10` profile ranges from 4.4%/0.0% at the weak placement to
99.7%/97.8% at the strong placement for MQTT/TCP. The Results and
cross-application synthesis now attribute the historical-to-follow-up outcome
to the combined TDD profile, signal/placement, payload, and transport condition.

The three top-level insights were aligned with the user's stakeholder goals:

1. current direct V2X supports compact J2735 messages only within a limited
   payload and coverage envelope;
2. the CAV application-packet size that current 5G uplinks can complete within
   a decision deadline is severely limited; and
3. 5G/6G systems must improve vehicular uplink performance and isolate
   concurrent application demand.

The approximately 20-KiB conservative ceiling for a 500-ms p95 TCP/MQTT
application deadline now appears in the private-5G experiment summary and the
Results evidence, but not in the top-level Insight 2 sentence. It is explicitly
paired with the tightly controlled one-UE, dedicated 40-MHz, clean-n48
environment. Future Research Directions and the Conclusion were rewritten to
connect roadside coverage, smaller/adaptive CAV representations, and vehicular
uplink resource control without turning the Conclusion into a list.

Updated files include:

- `agent_context.md`
- `experiment_summary.md`
- `paper/analysis/5g_experiment_summary.md`
- `paper/paper_outline.md`
- `paper/manuscript/Makefile`
- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/manuscript/figures/section5_tdd_sensitivity.pdf`
- `paper/manuscript/main.pdf`

Validation: the pooled common, weak, and strong C1/C2 values were independently
recomputed from the retained sender CSVs and checked against the stored
condition summaries. A forced LaTeX build completed without undefined
citations, undefined references, rerun requests, overfull boxes, compilation
errors, or fatal errors. Pages 1, 2, 5, 9--11, 13, and 14 were rendered at
180 dpi and visually inspected; the revised text, deployment table, signal map,
joint heatmaps, insights, future directions, and Conclusion are legible without
overlap or clipping. The PDF has 18 US-Letter pages as a build fact; no
page-limit optimization was performed. It contains zero annotations, zero
links, zero embedded files, no author or title metadata, and fully embedded
fonts. The PDF SHA-256 is
`2ca82a67148651a08e940adc2a5a90dfc7ee6a8d467728093eb1476ae18afdf9`.
The updated private-5G summary SHA-256 is
`ae16410ef9525d9cbd98f672d5e969864795414b8e9cacabe9f759446c42aabc`.
The pre-revision PDF is preserved at
`tmp/pdfs/tdd_signal_revision/main_before_revision.pdf` with SHA-256
`737a81beb252495332e981b72ce8b53ef7e2237577ce1f8d6cf2dcc387ca005f`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Private-5G Experiment Summary

Complete. A source-traceable summary of the retained autonomous-vehicle
private-5G experiments was created at
`paper/analysis/5g_experiment_summary.md`. It defines the measured
vehicle--MG52--Airspan--core--MX250--d1 path; the sole-UE, dedicated-radio,
clean-n48 experimental boundary; the IPI workload and RTT timing boundary; and
the response-success and all-attempt deadline-availability rules. It covers all
retained experiment families: five stationary payload campaigns, the separate-
UE signal survey, deliberate load, the QoS-label control, detector-output
replay, raw and fragmented UDP, deadline analysis, logical-client demand,
controlled restart, host-side direction controls, the 40/40/20-to-70/20/10 TDD
comparison, and the locked-cell location follow-up.

Validation: 17 high-risk source-value signatures were checked against the
stored detector, fragmented-UDP, multiclient, deadline, and restart summaries;
all eight reported 100-ms deadline-availability values were independently
recomputed from the stored miss rates. Stationary payload ranges were checked
against `paper/analysis/limiting_factors/condition_level_metrics.csv`, and the
direction controls were checked against the Airspan manifest, validation JSON,
and load CSVs. All referenced source paths exist, Markdown whitespace
validation passes. The summary was later expanded with the joint TDD and
signal/placement comparison and the conservative 20-KiB deadline boundary; its
current SHA-256 is
`ae16410ef9525d9cbd98f672d5e969864795414b8e9cacabe9f759446c42aabc`.
A durable memory pointer now directs future sessions to this summary before
reopening the complete `results/real_5g/` tree.

## Direct C-V2X Experiment Table

Complete. A source-traceable summary of the retained commercial LTE C-V2X
direct-PC5 experiments was created at
`paper/analysis/v2x_experiment_summary.md`. It records the functional SPaT,
BSM, and bidirectional custom-path checks; the complete 35-condition stationary
payload matrix; the signal-diagnostic probes; and all four GNSS-backed mobile
routes. Per the user's instruction, every planned stationary payload phase uses
the 1,000-attempt analysis denominator, and operator-stopped or skipped
remainders after timeout-dominant behavior are treated as timeouts. Successful-
response RTT statistics remain conditional on receiving a matching response.

Validation: all 35 stationary table rows were compared programmatically with
their stored `payload_summary.csv` files. The mobile aggregate was independently
verified as 2,553 responses from 3,675 attempts, or 69.5%. The setup, signal,
mobility, and GNSS source artifacts were checked directly. Markdown whitespace
validation passes. A durable memory pointer now directs future sessions to this
verified summary before reopening raw result files. No manuscript source or PDF
was changed.

## Year 2 Report CAV/ITS and Stakeholder Evidence Expansion

Complete. `proposals/Project_Implementation_Report_TIER_IV-UD-IPI_Year2.docx`
was expanded from the original five-page summary to 27 US-Letter pages using
only current repository evidence. No experiment was added. Per the user's
instruction, the revision does not add a `Why IPI?` section because that topic
is covered in the previous report.

The report retains the detailed implementation, private-5G and PC5 procedures,
application/deadline map, four success levels, successful-response variance,
PCA, matched factor ranking, and WP4-WP6 reference contracts. It now also adds
an executive stakeholder summary and Sections 14-18 covering proposal-
deliverable closure, supporting commitments, exact condition and attempt
denominators, collection/repetition limits, application-use decisions,
delivery-versus-survivor timing, ranking robustness, an end-to-end module and
authority trace, negative results, a hash-bound reproducibility manifest, and a
measurement glossary. The closure matrix states which WP4-WP6 artifacts are
implemented and validated and which current evidence is absent, including a
complete CARLA/Autoware driving scenario, downstream vehicle-action adapter,
immutable licensed release, and retained seminar/tutorial records.

Measured field results, implementation-only validation, and unmeasured CAV
application outcomes remain explicitly separated. The report preserves the
protocol-provenance boundary: retained private-5G runs carry encoded IPI
cooperative-service frames, whereas the historical PC5 performance sweeps use
the Mocar sequence-matched vendor packet/echo; the later formal J2735/IP5X
adapter is host-validated and is not retroactively claimed as the field packet.

Validation: the final DOCX contains 251 paragraphs, 37 tables, 9 inline figures
with alt text, 19 Heading 1 entries, and 45 Heading 2 entries. LibreOffice
rendered 27 US-Letter pages. Every page was inspected at original detail; the
final-path render was also verified image-for-image against the inspected
candidate. There is no clipping, overlap, margin overflow, broken table split,
or illegible figure. DOCX ZIP integrity passes, and the file contains no
comments, tracked insertions/deletions, external links, placeholders, or `Why
IPI?` text. The accessibility audit reports 0 high-, 0 medium-, and 0 low-
severity findings. Extracted text contains 12,902 words. No software source
changed in this report-only revision, so the C++ tests were not rerun; the
report retains the previously recorded fresh 18/18 CTest result. Final
SHA-256: `77f89668b191ea151e6477758fab6b522765791ed18f3c46aafe072c66329e23`.

## Stakeholder-Specific Insight Revision

Complete. The user's message was treated as the revision instruction; the
current PDF contained no annotation objects. The three top-level insights were
rebuilt as stakeholder-specific design conclusions rather than surface-level
restatements of the measurements:

1. CV and ITS applications for human-driven vehicles depend on timely
   information availability, not received-packet latency alone.
2. CAVs and autonomous transportation systems require stateful exchanges to
   complete before the vehicle must act, not merely to arrive eventually.
3. Mobile networks must allocate resources based on concurrent, deadline-bound
   application demand rather than physical UE count or aggregate throughput.

The Abstract now states only these three high-level conclusions. The
Introduction identifies the affected decision for each stakeholder. The
Results provides one compact evidence-and-implication paragraph per insight:
PC5 received-response latency versus missing information for human-driven
vehicles; eventual 5G transfer versus deadline-compliant CAV completion; and a
single physical UE carrying multiple controlled application demands for
operators and 5G/6G developers and researchers. Future Research Directions now
follows the same stakeholder order. The Conclusion connects the three insights
as one argument instead of presenting a shopping list. The binding outline was
updated throughout.

Updated files include:

- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/manuscript/main.pdf`

Validation: three stable LaTeX builds completed without undefined citations,
undefined references, rerun requests, overfull boxes, compilation errors, or
fatal errors. Pages 1, 2, 12, and 13 were rendered at 180 dpi and visually
inspected; the revised Abstract, Introduction list, Results discussion, Future
Research Directions, and Conclusion are legible without overlap, clipping, or
awkward list wrapping. The output remains 17 US-Letter pages as a build fact;
no page-limit optimization was performed. The PDF contains zero annotations,
zero links, zero embedded files, no author or title metadata, and fully embedded
fonts. Its SHA-256 is
`737a81beb252495332e981b72ce8b53ef7e2237577ce1f8d6cf2dcc387ca005f`.
The pre-revision PDF was preserved at
`tmp/pdfs/deeper_stakeholder_insights/main_before_revision.pdf` with SHA-256
`a395de990fce0dbef06b14e4aba92c5174b0ae1eecde8d2830522e7b135fdddd`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## General Academic Writing Lessons

Complete. A domain-independent writing guide was created at
`paper/general_academic_writing_lessons.md`. It generalizes the recurring
revision failures without referring to this manuscript's technologies,
experiments, figures, or results. The guide covers argument-first drafting,
causal direction, the separation of capability from limitations and success
criteria, sentence responsibility, accurate verbs, placement of qualifications,
synthesis, logical transitions, commentary-style prose, repetition, terminology
consistency, and visual communication. It also provides an eight-pass revision
procedure and a reusable final checklist.

Validation: the document contains no project-specific system, protocol,
technology, venue, or manuscript-title terminology. Markdown whitespace checks
pass, and the file remains under the approved nested `paper/` directory rather
than adding a root-level Markdown file.

## Prior Insight 2 Causal-Wording Correction (Superseded as a Top-Level Insight)

Complete at that revision stage. The sentence `5G can carry larger stateful
exchanges, but payload size and network conditions dictate whether they
complete within the application deadline` separates transfer capability from
deadline compliance. It is retained as supporting interpretation in the
Results but has been superseded as a top-level insight by the
stakeholder-specific CAV conclusion above. The former phrasing that required
payload and service behavior to `fit` a deadline and operating conditions
remains removed.

The following validation record and hash describe that intermediate revision:
a forced LaTeX rebuild completed without undefined citations,
undefined references, rerun requests, overfull boxes, compilation errors, or
fatal errors. Pages 1, 2, 12, and 13 were rendered and visually inspected; the
revised insight is legible in the abstract, Introduction, Results, and
Conclusion without overlap, clipping, or awkward list wrapping. The PDF has 17
US-Letter pages, zero annotations, zero links, zero embedded files, no author or
title metadata, and fully embedded fonts. Its SHA-256 is
`a395de990fce0dbef06b14e4aba92c5174b0ae1eecde8d2830522e7b135fdddd`.
No page-limit optimization was performed, and `paper/manuscript/28p.pdf`
remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Controlled-Environment and Full-Matrix Manuscript Revision

Complete. The six new page-1 annotations and the user's follow-up clarification
were treated as manuscript-wide revision instructions. The central question now
asks whether both direct V2X and 5G can support both compact CV workloads and
larger, stateful CAV workloads, and at what payload sizes and operating
conditions each path no longer satisfies the application's response-latency and
availability requirements. This full workload-by-path matrix is stated in the
abstract, Introduction, Related Work gap, system and experiment design, results,
future research directions, conclusion, appendix, and binding outline.

The private-5G deployment is now described consistently as a tightly controlled
environment: the CAV gateway was the sole physical user equipment; the radio
and 40-MHz channel were dedicated to the experiment; and the n48 band was clean
without ambient traffic or contention. Background streams and application
clients are identified as deliberately introduced experimental conditions. The
three approved insight statements were restored without the former
audience-prefixed wording or compressed replacement sentences. The transition
from CV/CAV workloads to PC5 and Uu was rewritten, and the Introduction now uses
an explicit compact-SSM example before discussing competition with larger CAV
exchanges.

Updated manuscript files include:

- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/02_related_work.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/manuscript/sections/08_appendices.tex`
- `paper/paper_outline.md`
- `paper/manuscript/main.pdf`

Validation: a forced LaTeX rebuild completed without undefined citations,
undefined references, rerun requests, overfull boxes, compilation errors, or
fatal errors. Pages 1--6, 12--13, and 16--17 were rendered and visually
inspected; the revised prose, figures, tables, insights, conclusion, and
appendix are legible without overlap or clipping. The output is 17 US-Letter
pages as a build fact; no page-limit optimization was performed. All fonts are
embedded. The PDF contains zero annotations, zero links, zero embedded files,
and no author or title metadata. Its SHA-256 is
`24be3990f514202064a30f16865d9560f64354c4b54376d102e3fb50acb26ba5`.
The annotated input was preserved at
`tmp/pdfs/full_matrix_annotations/main_annotated.pdf` with SHA-256
`e7c8674a739d2fa828d1f34d5d8b48b3cfb421a4da8e722f12d5d86ba56486f4`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## New Page-1 Annotation Review

Complete as a read-only review. The current `paper/manuscript/main.pdf` has
SHA-256 `e7c8674a739d2fa828d1f34d5d8b48b3cfb421a4da8e722f12d5d86ba56486f4`
and contains twelve annotation objects on page 1: six substantive highlights
and six associated popup containers. The comments request: 1) describe the
measurement setting as tightly controlled: one physical CAV client, a dedicated
radio, dedicated bandwidth, and a clean n48 band with no ambient contention;
any experimental load or client demand is deliberately generated; 2) replace
the second insight because it combines multiple ideas into one confusing
sentence; 3) restore the stronger prior wording of all three insights while
removing only the audience-prefixed `For ...` structure and avoiding the same
multi-idea sentence problem; 4)
rewrite the transition between the combined-workload discussion and the path
descriptions; 5) introduce the contention example with `For example` and name a
compact message such as SSM; and 6) correct the central argument. The corrected
argument tests both CV and CAV workloads over both V2X and 5G. It asks whether
each path can carry each workload class and identifies the packet size and
operating conditions at which the path no longer satisfies the application's
latency and availability requirements. The increasing-packet-size experiments
therefore reveal the support boundary for more complex future CV/CAV
applications rather than assuming a fixed CV-to-V2X and CAV-to-5G division. The
preserved prior PDF confirms the requested insight wording. No manuscript
source or PDF was revised during this review.

## Abstract and Introduction Annotation Revision

Complete. All fifteen substantive highlights in the annotated
`paper/manuscript/main.pdf` were treated as revision instructions. The abstract
and Introduction were rebuilt around the user's communication-readiness
question, and recurring instructions were propagated through Related Work, IPI
design, setup, results, future directions, the appendix, figure labels, and the
binding outline. The revision now uses the full J2735 message-set scope,
distinguishes standardized CV messages from proposed CAV applications, includes
human-driver information needs, numbers the three insights without audience
prefixes, uses application-client demand and vehicle-specific trajectories, and
states issued-request denominators instead of the label `all-attempt`. The
Introduction asks whether current V2X and 5G paths support combined CV/CAV
applications within their required deadlines and how operating conditions
affect response latency and availability. It then introduces IPI and the
detailed real-vehicle field study as the two contributions used to answer that
question.

The Section 5 comparison figure was moved early enough in the double-column
float queue to appear before the cross-application discussion, the three
insights, future research directions, and conclusion. A separate appendix
column overflow found during visual inspection was also removed. No prose was
condensed, expanded, or reflowed to meet a page limit.

Updated files include:

- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/02_related_work.tex`
- `paper/manuscript/sections/03_ipi_protocol_design.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/08_appendices.tex`
- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/paper_outline.md`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: three-pass LaTeX builds succeed without undefined citations,
undefined references, rerun requests, compilation errors, or fatal errors.
Pages 1, 2, 11, 12, 13, 14, and 16 were rendered and inspected. The revised
abstract, Introduction, result figures, Figure 12 handoff, three insights,
conclusion, references, and appendix are legible without overlap or clipping.
The output is 17 US-Letter pages; this page count is recorded only as a build
fact, and no page-limit optimization was performed. All fonts are embedded.
The PDF contains zero annotations, zero links, no embedded files, and no author
or title metadata. The annotated input was preserved at
`tmp/pdfs/abstract_intro_annotations/main_annotated.pdf` with
SHA-256 `f073a087293e64c8c379a3523e9058f5b5f531b7f829b312d5348e4674e7d2c8`.
The revised PDF SHA-256 is
`8de0b9aa01d09875b30ddd072765c3533ad662cc3cff760d545f268541580551`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Year 2 IPI Implementation and Experimental Report

Complete. The five-page report at
`proposals/Project_Implementation_Report_TIER_IV-UD-IPI_Year2.docx` was
rewritten as a technical summary of the IPI implementation and the retained
experiments. It states explicitly that every summarized field experiment used
IPI at the application layer. It explains the common IPI contract, session and
correlation rules, J2735 regional encoding, PC5 and private-5G bindings,
failure/status model, measured byte overhead, and software validation. It then
summarizes the private-5G payload, detector-replay, background-load, logical-
client, interruption, direction, location, and QoS-label tests and the
stationary and mobile direct-PC5 payload/coverage tests.

The final page reports the joint analysis of 364 condition-level observations
representing 1,708,263 issued IPI attempts. PCA separates latency/tail burden
from availability loss: PC1 explains 62.15% of the variance and is dominated
by p50, p95, and p99 RTT and tail inflation; PC2 explains 20.20% and is
dominated by the failure fraction. Matched experimental contrasts identify
payload and representation scale as the strongest latency factor, followed by
concurrent demand without verified isolation. The report preserves the RTT
boundary for unsynchronized private-5G clocks and does not claim causal TDD,
5QI, or PC5 radio effects that the retained evidence cannot isolate.

A separate successful-attempt analysis now returns to 1,688,330 finite sender-
side RTT records in the 355 conditions with at least 20 successful responses.
On log10 RTT and with equal condition weights, differences between condition
means explain 89.1% of successful-attempt variance and within-condition
attempt-to-attempt variation explains 10.9%. The median condition-level
coefficient of variation is 0.282, the median p95/p50 ratio is 1.427, and the
95th-percentile p95/p50 ratio is 6.223. The result is explicitly survivor-
conditioned and does not replace delivery, deadline, or outage analysis. The
reproducible inputs and outputs are recorded in
`scripts/analyze_successful_ipi_variance.py` and
`paper/analysis/limiting_factors/successful_attempt_variance*`.

Validation: system LibreOffice rendered the final document as exactly five
US-Letter pages, and all five pages were inspected at 150 dpi. Headings,
tables, callouts, the implementation diagram, PCA and PC5 figures, running
headers, footers, and page numbering are legible without overlap, clipping, or
crowding. The document has four explicit page breaks, five tables, five Heading
1 entries, four Heading 2 entries, one portrait section with 1-inch side/top
margins, accessible descriptions for all images, no tracked changes or
comments, no placeholder text, and no archive errors. The accessibility audit
reported zero high-, medium-, or low-severity findings. The final DOCX SHA-256
is `d7abf87b0cb70d3f5389b7b41cdd12c90b268de43b6332306a24b16759766bc1`.

## Annotated-PDF Revision

Complete. The five highlight comments in the prior `paper/manuscript/main.pdf`
were treated as binding revision instructions. The abstract now opens with the
communication-readiness question and introduces the two contributions with
`In this paper, we contribute`. The three insights now address distinct
audiences: CV/ITS researchers, CAV-system researchers, and 5G/6G network and
radio researchers. Related Work now begins with the coupled communication
challenges and asks whether current paths support safety-critical CV/CAV
workloads and, when they do not, which measured conditions place responses
beyond their deadlines. Its synthesis and Figure 1 follow the same readiness
and limiting-factor logic.

Updated files:

- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/02_related_work.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: two stable LaTeX builds succeed without undefined citations or
references, rerun requests, overfull horizontal boxes, compilation errors, or
fatal errors. Pages 1, 2, 3, 12, and 13 were rendered at 144 dpi and inspected;
the revised abstract, audience-specific insights, Related Work opening, Figure
1, Future Research Directions, conclusion, and reference transition are
legible without overlap, clipping, or crowding. The PDF contains 16 US-Letter
pages; core content ends on page 12 and references begin on page 13. All fonts
are embedded. The final PDF contains zero annotations, no embedded files, and
no author metadata, so the original comments are absent. Its SHA-256 is
`d8655ff35c445ac16e441755c6d3f5db1bc1ca0f72ff24e517bf801febd21bcb`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Communication-Latency Terminology Revision

Complete. The paper now uses `communication latency` for its central measured
outcome rather than the ambiguous term `communication overhead`. The title is
`Edge4AV: Communication Latency for Connected and Automated Vehicle
Applications`. The Introduction frames the question through response latency
and all-attempt deadline availability, and the contribution is now a
communication-latency evaluation. Related Work, System Design, Results, Future
Research Directions, and the conclusion use the same terminology.

The metric boundary is explicit: endpoint clocks were unsynchronized, so
sender-side response RTT is the communication-latency metric, while all-attempt
deadline availability accounts for late, missing, rejected, and timed-out
responses. `Overhead` remains only where the paper measures a concrete addition
such as IPI packaging bytes, representation bytes, or transport/security bytes.
The binding writing requirements in `paper/paper_outline.md` now preserve this
distinction.

The Future Research Directions opening and conclusion were condensed to prevent
the terminology revision from leaving three conclusion lines on an otherwise
empty page. The conclusion now uses two connected paragraphs that retain both
contributions and the measured PC5/5G answer. Core content ends on page 12 and
the references begin on page 13.

Updated files:

- `paper/manuscript/main.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/02_related_work.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: two stable LaTeX builds succeed without undefined citations or
references, changed-label requests, overfull horizontal boxes, compilation
errors, or fatal errors. Pages 1, 2, 3, 6, 9, 12, and 13 were rendered at 200
dpi and inspected. The new title, Introduction question and contribution,
Related Work caption, latency definition, Results transitions, Future Research
Directions, conclusion, and reference transition are legible without overlap,
clipping, or crowding. The final PDF remains 16 US-Letter pages. Its SHA-256 is
`c1e40321ab5fb0173c79952c939cdde2db0067968d944b3071b6acf927da561c`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Abstract Contribution-Balance Revision

Complete. The abstract now makes the detailed communication-latency
measurement study, rather than IPI, the primary response to the communication-
readiness question. The opening states the missing cross-path application
evidence without introducing IPI. The contribution paragraph gives IPI one
sentence and gives the measurement study two sentences that identify the real-
vehicle PC5 and Uu paths, response RTT and deadline availability, and the
payload, transport, stationary/mobile location, directional-allocation,
contention, logical-demand, and interruption factors. The final paragraph
retains exactly three concise insights. The revised abstract is 228 words.

Updated files:

- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the full LaTeX build succeeds with stable cross-references. Page 1
was rendered at 220 dpi and inspected; the revised three-paragraph abstract and
the transition into the Introduction are legible without overlap, clipping, or
crowding. The PDF remains 16 US-Letter pages. The final PDF SHA-256 is
`c73030976e95f93f4bf431b6c8906205fb82bf9482504b8c0e1afcbde36a5a24`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Task

Complete P-001's standards path by defining IPI as a formal SAE J2735 SEP2023
regional extension carried by a reserved test message, integrating a selectable
licensed ASN.1/UPER toolchain, and adding deterministic conformance vectors and
negative tests. The user has approved the current retained data as the evidence
source for the earlier field-path items.

## Status

Complete within the artifacts available in this checkout. P-001 now has a
separate typed ASN.1 regional module, a strict complete-frame C++ UPER codec,
independent deterministic vectors, a licensed-bundle preparation path, and
end-to-end use in the Mocar PC5 sample. The SAE J2735 SEP2023 reference PDF and
Mocar-generated descriptors are present, but the separately distributed
`J2735ASN_202309` source modules are not. Exact compilation against that
licensed module set remains an external-input gate and is not represented as a
completed certification result.

## Implementation Completed On 2026-08-14

- P-001: typed failure, termination, fallback, freshness, and terminal-service
  semantics; strict enum/flag/trailing-byte/class-content validation; and
  deterministic negative tests.
- P-001 standards path: `IPI.asn` defines the typed cooperative-service
  regional value; reserved `TestMessage00` uses `DSRCmsgID` 240 and local
  `RegionId` 200; `J2735IpiRegionalCodec` encodes and strictly decodes the
  complete UPER `MessageFrame`; `UperCodec` delegates cooperative messages to
  it; and the PC5 sample decodes the request and returns a terminal formal
  response frame.
- P-001 toolchain path: deterministic planning, perception, control, minimal,
  all-optional, malformed, identifier, extension, padding, and size vectors;
  identical regional bytes from `asn1tools` and `pycrate`; USDOT J2735 202409
  outer-frame encode/decode; and a non-destructive script for injecting IPI
  into a caller-supplied licensed `Reg-TestMessage00` object set with a SHA-256
  manifest.
- P-002: clock-injected lifecycle enforcement, full registration retention,
  patch/termination/high-level service updates, isolated in-memory state,
  versioned `IPIS` per-topic records, broker client and endpoint, MQTT
  authentication fields/reconnect/resubscription, and broker integration tests.
- P-003: local steady-clock RTT, request/message/response/correlation/session/
  sequence/expiry validation across TCP, UDP, and MQTT probes, audit log fields,
  and a versioned native detector-result schema.
- P-004: strict CRC-protected `IP5X` request/response framing, exact complete-
  packet limits, outstanding-result validation, boundary/negative tests, and a
  Mocar custom-message initiator/responder sample.
- WP4: activation-zone geometry, validity, hysteresis, dwell, infrastructure
  loss, and exactly-once state-transition events.
- WP5: typed service tiers and deterministic policy decisions. Safety-critical
  work is never intentionally paused; link loss selects local fallback when it
  exists.
- WP6: local/edge completion prediction with transfer, RTT, queue, compute,
  uncertainty, deadline, confidence, service-availability, and fallback inputs;
  plus identity/deadline/transition hardening in `TaskOffloader`.
- ROS 2: new `ipi_msgs` and `ipi_runtime` packages with a reference-zone
  publisher, activation manager, tiered service manager, offload node, launch
  configuration, and launch test. The adapter publishes decisions and does not
  command Autoware operation mode or motion.
- Packaging/documentation: installable `IPI::ipi` CMake package configuration,
  updated C++ and deployment documentation, and proposal-status boundaries.

## Validation Completed On 2026-08-14

- Clean CMake configure and C++17 build with warnings enabled: passed.
- CTest: 18 of 18 tests passed, including formal J2735 regional vectors and
  schema-bundle preparation plus lifecycle, strict session wire,
  in-memory broker integration, PC5, detector, activation, policy, offload,
  task lifecycle, private-5G loopbacks, and the prior API/J2735/log tests.
- AddressSanitizer and UndefinedBehaviorSanitizer build: the same 18 tests
  passed with leak detection and undefined-behavior stack traces enabled.
- Independent ASN.1 vector verification: four deterministic profiles matched
  byte-for-byte between `asn1tools` 0.167.0 and `pycrate` 0.7.11; the unmodified
  USDOT J2735 202409 package encoded and decoded each outer `MessageFrame` while
  preserving the unknown local regional open type.
- CMake install/package consumption: passed; the ROS package resolved
  `find_package(IPI CONFIG REQUIRED)` and linked `IPI::ipi`, and the installed
  `share/ipi/asn1/IPI.asn` is byte-identical to the source schema.
- ROS 2 Humble build: `ipi_msgs` and `ipi_runtime` passed with the system ROS
  Python selected.
- ROS 2 launch contract: passed 10 consecutive clean runs. It observed `ACTIVE`,
  best-effort congestion pause, safety-critical local fallback on link loss,
  local offload on link loss, and edge offload on a healthy link. The test also
  asserts that all four runtime processes exit with success after shutdown.
- Manual ROS 2 live smoke: the same activation, policy, and offload outcomes
  were observed and the launch shut down cleanly.
- Mocar PC5 sample: the formal `TestMessage00` request/terminal-response path
  passed warning-as-error AArch64 cross-compilation against the available
  vendor headers and libraries; generated build outputs were removed afterward.

## Follow-on Gates Not Required For This Task

The user approved the currently retained evidence for the earlier field-path
items and asked to implement only P-001's standards path. The boundaries below
therefore remain documented engineering or release work; they do not block the
completion status above.

- Exact compilation of the prepared schema against the separately licensed
  SAE `J2735ASN_202309` source modules and an independent SEP2023 endpoint. The
  repository contains the standard PDF, not that separately distributed ASN.1
  package; local dual-compiler regional vectors and a USDOT 202409 outer-frame
  check are complete.
- A real authenticated MQTT broker disconnect/restart test plus production TLS,
  access-control, and delivery-QoS work. The current reference client uses plain
  MQTT 3.1.1 QoS 0.
- Binding the deployed private-5G field workload to the high-level session
  facade, recollecting monotonic/correlated runs, serializing real detector
  outputs, and retaining matched Airspan counters where radio-causal claims are
  intended.
- Two-device Mocar execution of `ipi_pc5_exchange` at compact and near-limit
  conditions with retained all-attempt and negative-path artifacts.
- The approved vehicle-side Autoware action adapter, CARLA scenario/assets, and
  hardware-in-the-loop validation. The current ROS 2 slice intentionally stops
  at decision publication.
- An approved repository license before external software release.

## Figure 7 Deadline-Direction Clarification

Complete. Figure 7(a) now states that each heatmap value is the percentage of
all issued attempts whose response arrived at or before the displayed deadline.
The panel title is `Responses at or before deadline (% of all issued attempts)`,
and the color scale repeats the all-issued-attempt denominator. Therefore, a
larger percentage is better: it counts responses that met the deadline, while
late, missing, rejected, and timed-out attempts remain in the denominator but
not the numerator.

The Figure 7 caption and accessible description now use the same explicit
definition for 100, 500, and 1,000 ms and the harness timeout. The plotted
values and denominators were not changed.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/figures/section5_5g_deadline_envelope.pdf`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator passes Python compilation, regenerates the
figure, and the full LaTeX build succeeds. The standalone Figure 7 and
manuscript page 8 were rendered at 300 dpi and inspected. The new panel title,
color-scale label, deadlines, values, denominator, caption, and neighboring
content are legible without overlap or clipping. The final log contains no
undefined citation/reference, changed-label request, overfull box, compilation
error, or fatal error. The PDF remains 16 US-Letter pages; core content ends on
page 12, references begin on page 13, and appendices begin on page 15. All fonts
are embedded, and the PDF contains no annotations, embedded files, or author
metadata. The final PDF SHA-256 is
`1b41a1fc2e98ab75b9203b74293e2ab6d1e2d98f3a4d07817260e2e098b73253`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Section 5.1 IPI Packaging-Overhead Revision

Complete. Section 5.1 no longer treats callback, echo, and correlated-response
success counts as the primary validation of IPI. It now evaluates whether the
common representation supports CV and CAV applications without adding
significant communication load. The subsection is titled `IPI Packaging
Overhead`, and the System Design experiment matrix now asks the same packaging
question.

The revised comparison separates application content from encoded IPI-frame
bytes. A preserved J2735 payload of `P` bytes receives a five-byte message-frame
type and length. For the measured CAV planning request, zero optional task bytes
produce a 63-byte frame. The 256-byte, 1-KiB, and 4-KiB collections produce
334--337, 1,102--1,105, and 4,174--4,177 bytes, respectively. Thus, IPI adds
78--81 bytes when task content is present. The added fraction is 7.6--7.9% at
1 KiB and 1.9--2.0% at 4 KiB; across every retained baseline payload of at
least 4 KiB, the maximum is 1.978%. The text connects those sizes to typed BSM,
PSM, MAP, SPaT, SRM, and SSM support, byte-exact opaque TIM preservation, and
correlated CAV request, update, completion, and rejection operations.

Updated files:

- `paper/manuscript/sections/03_ipi_protocol_design.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the table values were recomputed from all 1,000 sender records in
each retained May 13 TCP collection at 0 B, 256 B, 1 KiB, and 4 KiB. The
`edge4av_interface`, `private_5g_latency_probe`, and `j2735_message_flow` tests
all pass. The full LaTeX build succeeds. Manuscript pages 3, 6, 7, and 12 were
rendered and inspected; the revised transition, experiment matrix, Section 5.1,
overhead table, following results, and conclusion are legible without overlap
or clipping. The final log contains no undefined citation/reference,
changed-label request, overfull box, compilation error, or fatal error. The PDF
remains 16 US-Letter pages; core content ends on page 12, references begin on
page 13, and appendices begin on page 15. All fonts are embedded, and the PDF
contains no annotations, embedded files, or author metadata. The final PDF
SHA-256 is
`710b69491487dd7def51f51c456084ad17bd8419df7aba8b484b3803ab36a93a`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Table 1 Caption Correction

Complete. The Table 1 caption was reduced from an editorial pointer to the
descriptive title `Deployment and measurement context.` The removed sentence
directed readers to Appendix B instead of describing the table. A complete
caption scan found no other table or figure caption that directs the reader to
an appendix, so no additional captions were changed.

Updated files:

- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the full LaTeX build succeeds. Page 5 was rendered at 300 dpi and
inspected; the revised caption, table, neighboring figures, and surrounding
text are legible without overlap or clipping. The final log contains no
undefined citation/reference, changed-label request, overfull box, compilation
error, or fatal error. The PDF remains 16 US-Letter pages; core content ends on
page 12, references begin on page 13, and appendices begin on page 15. All fonts
are embedded, and the PDF contains no annotations, embedded files, or author
metadata. The final PDF SHA-256 is
`a5cc7eb7b52bdd5e406f768c7b8e3ff7f440d365414e8218d774311608448312`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Figure 4 Overlap Correction

Complete. Figure 4's repeated internal legends were removed because each
stationary collection is already labeled at its map location. In all three
panels, the legend heading had occupied the same vertical space as the Run 1
entry. The five run markers were also reduced from a 30-pixel to a 24-pixel
radius, which separates the nearby Run 1 and Run 5 markers and exposes more of
the underlying survey map. Marker coordinates, run labels, map tiles,
interpolated surfaces, radio measurements, and color scales were not changed.
The caption now states that the run markers are directly labeled.

Updated files:

- `paper/figs/fig-signal-osm-runs.svg`
- `paper/figs/fig-signal-osm-runs.png`
- `scripts/build_signal_strength_maps.py`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the generator passes Python compilation and a focused marker test.
The corrected SVG was rendered to a 4,465-by-1,743-pixel PNG, the full LaTeX
manuscript was rebuilt to stable cross-references, and page 5 was rendered at
300 dpi and inspected. The map panels, direct labels, nearby run
markers, signal scales, caption, and adjacent page content are legible without
overlap or clipping. The final log contains no undefined citation/reference,
changed-label request, overfull box, compilation error, or fatal error. The PDF
remains 16 US-Letter pages; core content ends on page 12, references begin on
page 13, and appendices begin on page 15. All fonts are embedded, and the PDF
contains no annotations, embedded files, or author metadata. The final PDF
SHA-256 is
`770a56c495bf1300d0a70f3c438f26364b3909c69ffb4392be6df8900bca29a6`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Figure 12 V2X Signal-Strength Estimate

Complete. Figure 12 now places a route-level V2X signal-strength estimate beside
the application-deadline comparison. Panel (b) reports R1--R4 as 0.11, 0.58,
0.45, and 0.57 on a dimensionless zero-to-one scale. The plotted values are
computed from every stored 256-byte mobile-route attempt. A failed response
scores zero. A successful response scores one when its RTT is at most 20 ms and
0.1 when its RTT is at least 60 ms, with linear interpolation between those
limits; each route reports the mean score.

The System Design and Setup section defines the estimator and its evidence
boundary. The score is an application-level estimate derived from response
success and RTT, not RSSI, RSRP, RSRQ, or SNR, because the deployed OBU exposed
no usable radio-strength field. The Figure 12 axis and caption label the value
as an estimated, dimensionless quantity and refer readers to that definition.
The Conclusion was shortened without changing its claim structure so the
larger two-panel figure did not create an orphaned content page.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/manuscript/figures/section5_cross_application_comparison.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator passes Python compilation and reproduces the
unrounded route estimates as 0.105092, 0.576882, 0.454469, and 0.569107. The
full LaTeX build succeeds. Page 12 was rendered at 300 dpi and inspected; both
panels, units, values, legend, axes, caption, surrounding text, and conclusion
are legible without overlap or clipping. The final log contains no undefined
citation/reference, changed-label request, overfull box, compilation error, or
fatal error. The PDF remains 16 US-Letter pages; core content ends on page 12,
references begin on page 13, and appendices begin on page 15. All fonts are
embedded, and the PDF contains no annotations, embedded files, or author
metadata. The final PDF SHA-256 is
`122c9446a903f077d068faf285808983a257a692dbb3be0c327c016300b667f3`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Figure 12 Redesign

Complete. Figure 12 was redesigned from a three-panel collection into one
application-deadline comparison. The previous figure combined a normalized
latency ratio, three before/after availability changes, and two unrelated
availability collections on different horizontal scales. Its labels also
required the reader to infer what “changed communication” and “mobility and
restart” meant, while panels (b) and (c) duplicated results already presented
in Figures 9--11.

The revised figure has one row for each application: signal warning,
blind-spot warning, automated intersection, emergency maneuver, and remote
recovery. Blue and orange markers identify PC5 and 5G Uu. Every horizontal
value is received-response p95 divided by the corresponding application
deadline, and the logarithmic axis states that ratios below one fall within the
deadline while ratios above one exceed it. A vertical deadline line and light
within/beyond backgrounds make the comparison visible without requiring the
caption. The blind-spot marker is the median of four PC5 routes, and its
0.42--1.08 interval spans those routes. Numeric labels report every plotted
ratio.

The Results text and caption were revised with the figure. The TDD, competing
uplink, logical-client, mobility, and restart measurements now cite their
original result figures instead of being repeated in Figure 12. No measurement,
deadline, ratio, or conclusion changed.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/figures/section5_cross_application_comparison.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 12 and manuscript page 12 were rendered at high
resolution and inspected. Application labels, path markers, route interval,
numeric ratios, deadline boundary, legend, logarithmic ticks, axis definition,
and caption are legible without overlap or clipping. The final log contains no
undefined citation/reference, changed-label request, overfull box, compilation
error, or fatal error. The PDF remains 16 US-Letter pages; core content ends on
page 12, references begin on page 13, and appendices begin on page 15. All fonts
are embedded, and the PDF contains no annotations, embedded files, or author
metadata. The final PDF SHA-256 is
`d4b5b24041897d14fba0930eee775fea3bc45b9722cd09e2631bb10bf2ad4234`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Figure 11 Overlap Correction

Complete. Figure 11 was regenerated to remove collisions within and between
its four panels. In panel (a), the six-entry legend that covered the high-client
curves was replaced by a compact transport legend; line style now identifies
the favorable and weak paths. Its x-axis label was shortened because the
one-host/one-UE scope is already stated in the surrounding text. In panel (b),
the bar and response-gap encodings now have a dedicated legend above the axes,
the unrelated 99% and 99.9% guide lines were removed, and paired bar labels are
aligned away from each other. Additional row and column spacing separates the
upper plots from the two deadline-availability heatmaps. The caption now states
the visual encodings directly.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/figures/section5_concurrency_interruption.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 11 and manuscript page 11 were rendered at high
resolution and inspected. Titles, legends, curves, markers, bar labels, axes,
heatmap labels, and the caption are separated and legible without overlap or
clipping. The final log contains no undefined citation/reference,
changed-label request, overfull box, compilation error, or fatal error. The PDF
remains 16 US-Letter pages; core content ends on page 12, references begin on
page 13, and appendices begin on page 15. All fonts are embedded, and the PDF
contains no annotations, embedded files, or author metadata. The final PDF
SHA-256 is
`1a0329bef212e26e56949e4d8446d4d65c88d1171aaf6c4b4cf5d98bce2795b1`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Figure 10 Direction-Control Correction

Complete. Figure 10(a) now presents the direction controls and application
references on separate uplink and downlink rows. The previous bar chart drew
the 32-Mbit/s help-center video line across both directions, omitted the
25--400-kbit/s return-path range, and did not distinguish the common configured
target from the achieved rate.

The corrected logarithmic rate plot shows the 13.118-Mbit/s vehicle-to-d1 and
25.000-Mbit/s d1-to-vehicle application rates as circles. Crosses show the
25-Mbit/s configured target for each direction. The 32-Mbit/s video reference
appears only on the uplink row, while the 25--400-kbit/s command/return range
appears only on the downlink row. The accompanying text reports that the
measured uplink rate is 41.0\% of the video reference and that the measured
downlink rate is 62.5 times the upper edge of the return-path range.

The stored control artifacts confirm that the uplink sender and receiver each
recorded 1,475,758,800 B and that TCP backpressure limited the sender to
13.118 Mbit/s. The downlink sender and receiver each recorded 2,812,500,000 B,
and the sender sustained the configured 25.000 Mbit/s for 900 s.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/figures/section5_mixed_load_qos.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 10 and manuscript page 10 were rendered at high
resolution and inspected. The direction labels, logarithmic rate axis,
configured and achieved markers, direction-specific application references,
caption, and neighboring panels are legible without overlap or clipping. The
final log contains no undefined citation/reference, changed-label request,
overfull box, compilation error, or fatal error. The PDF remains 16 US-Letter
pages; core content ends on page 12, references begin on page 13, and
appendices begin on page 15. All fonts are embedded, and the PDF contains no
annotations, embedded files, or author metadata. The preserved
`paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/manuscript/main.pdf` has SHA-256
  `a20a788d52937734befed3fe2d082ffab2e91e1309aefd1f8961a2bccc849433`.

## Figure 9 TDD Comparison Correction

Complete. The directional-allocation panel was removed because the two TDD
profiles are already identified by their downlink/uplink/dynamic values. The
remaining panels now compare received-response p95 RTT and the percentage of
all issued attempts with a response by 100 ms. The latter replaces the previous
generic `deadline availability` label, which incorrectly implied that every
displayed workload has the same application deadline.

The plotted values were independently recomputed from the retained sender
CSVs. The historical 40/40/20 results contain 1,000 attempts per condition and
produce 100-ms response fractions of 100.0\%, 4.0\%, 30.8\%, and 18.4\%. The
70/20/10 follow-up combines two 500-attempt repetitions per condition and
produces 99.8\%, 1.2\%, 98.9\%, and 81.2\%. These values correspond to 1-KiB
MQTT, 1-KiB TCP, 23,968-B MQTT, and 23,968-B TCP, respectively.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/figures/section5_tdd_sensitivity.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 9 and manuscript page 10 were rendered at high
resolution and inspected; both panels, millisecond and percentage units,
profile-color mapping, value labels, caption, and adjacent Figure 10 are
legible without overlap or clipping. The final log contains no undefined
citation/reference, changed-label request, overfull box, compilation error, or
fatal error. The PDF remains 16 US-Letter pages; core content ends on page 12,
references begin on page 13, and appendices begin on page 15. All fonts are
embedded, and the PDF contains no annotations, embedded files, or author
metadata. The preserved `paper/manuscript/28p.pdf` remains byte-identical with
SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/manuscript/main.pdf` has SHA-256
`9ba3ce786b7cbdcdb7579a633d6155a0cdde564320f53dae242ca4f7d4e3e097`.

## Figure 8 Payload-Terminology Correction

Complete. Figure 8 no longer uses the undefined phrase `logical application
object`. Its horizontal axes now identify the measured variable as IPI workload
payload size in bytes on a logarithmic scale, and its vertical axis identifies
all-attempt response availability. The caption defines payload size as the
application content before transport framing or UDP fragmentation and explains
the 90\%, 99\%, and 99.9\% reference lines.

The same distinction is applied throughout the core manuscript and appendix:
`payload size` now denotes the byte-sized communication variable, while
`detected object` and `cooperative object` remain only where they denote actual
CAV data structures. This removes ambiguous uses of `object size`,
`application object size`, and `large object` from the communication-latency
claims without changing the measurements or the three insights.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/08_appendices.tex`
- `paper/manuscript/figures/section5_protocol_completion.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 8 and manuscript page 9 were rendered at high resolution
and inspected; both axes, legends, path-condition titles, data series, reference
lines, caption, and adjacent text are legible without overlap or clipping. The
final log contains no undefined citation/reference, changed-label request,
overfull box, compilation error, or fatal error. The PDF remains 16 US-Letter
pages; core content ends on page 12, references begin on page 13, and
appendices begin on page 15. The PDF contains no annotations, embedded files,
or author metadata. The preserved `paper/manuscript/28p.pdf` remains
byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/manuscript/main.pdf` has SHA-256
`f38234dee8ed0553cae8f141f8f85a09da9182194b4f165763bd1795ab867ce5`.

## Figure 7 Unit and Legend Correction

Complete. Figure 7 now states the unit at every point where a reader interprets
the plotted quantities. Panel (a) identifies all-attempt deadline availability
as a percentage, appends a percent sign to every heatmap value, identifies the
100-, 500-, and 1,000-ms columns, and distinguishes the harness-timeout column.
Its caption also defines $n$ as issued attempts. Panel (b) identifies RTT in
milliseconds in both the panel title and horizontal axis, labels the 100-,
500-, and 1,000-ms reference ticks, and moves the p50/p95/p99 legend away from
the plotted workload rows.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/figures/section5_5g_deadline_envelope.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 7 and manuscript page 8 were rendered at high resolution
and inspected; all percentages, millisecond labels, reference ticks, legend
entries, workload groups, caption text, and adjacent content are legible
without overlap or clipping. The final log contains no undefined
citation/reference, changed-label request, overfull box, compilation error, or
fatal error. The PDF remains 16 US-Letter pages; core content ends on page 12,
references begin on page 13, and appendices begin on page 15. All fonts are
embedded, and the PDF contains no annotations, embedded files, or author
metadata. The preserved `paper/manuscript/28p.pdf` remains byte-identical with
SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/manuscript/main.pdf` has SHA-256
`8a22da66dca494a82161691b0e25f8dce5d8d3a5b509df9cb9caf32a0f950b0a`.

## Figure 6 Layout Correction

Complete. Figure 6 retains the same PC5 payload, reference-point RTT, and
mobile-route values, but its layout no longer places labels on top of one
another. Panel (a) allocates more width to the heatmap and rotates the five
stationary-point labels. Panel (b) places the p50, p95, and p99 legend in unused
plot space, separate from the 10-, 25-, and 100-ms reference lines.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/figures/section5_pc5_payload_route.pdf`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 6 and manuscript page 8 were rendered at high resolution
and inspected; the stationary-point labels, percentile legend, deadline
references, axes, values, caption, and adjacent Figure 7 are legible without
overlap or clipping. The final log contains no undefined citation/reference,
overfull box, compilation error, or fatal error. The PDF remains 16 US-Letter
pages with embedded fonts, no annotations, and no embedded files. The preserved
`paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/manuscript/main.pdf` has SHA-256
`dabbfc09a9646cb60830815d67ced04e9bb8010a4e8f8730e8fbee6d683056dc`.

## Figure 4 OpenStreetMap Revision

Complete. Figure 4 now directly reuses the existing publication-oriented
`paper/figs/fig-signal-osm-runs.png` rather than reconstructing a new map. Its
three OpenStreetMap panels show triangulated spatial interpolation of the
separate-handset RSRP, RSRQ, and reported-SNR samples and identify the five
stationary Uu workload collections. The system-design prose, caption, and
accessibility description define those contents and preserve OpenStreetMap
attribution. The temporary duplicate map-rendering code is no longer used.

Updated files:

- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/Makefile`
- `paper/manuscript/main.pdf`
- `current_task.md`

Validation: the full LaTeX build completes successfully. Figure 4 and
manuscript page 5 were rendered at high resolution and inspected; all three
maps, metric scales, sample and run markers, caption, and attribution are
legible without clipping or overlap. The final log contains no undefined
citation/reference, overfull box, or compilation error. The PDF remains 16
US-Letter pages with embedded fonts and no annotations. The preserved
`paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/manuscript/main.pdf` has SHA-256
`aa7585a831d66eae79bb04c016704038c43081f851e1c19d1d7692f397b3a334`.

## Prior Abstract Revision

Complete. The abstract now moves directly from the two parallel contributions
to the three approved insights. It no longer lists payload size, route segment,
directional allocation, competing traffic, client demand, or interruption in a
separate result summary. The three insights themselves provide the paper-level
answer without duplicating the Results section. The abstract contains 207
whitespace-delimited words.

The shorter abstract changed double-column float placement and initially placed
Figures 11 and 12 in an overfull page box. Figure 12 is now rendered at
`0.80\textwidth`, remains legible, and eliminates the overfull box without
changing the 12-page core-paper boundary.

Updated files:

- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `current_task.md`

Validation: the full LaTeX build passes with no undefined citation/reference,
overfull box, compilation error, or fatal error. The PDF remains 16 US-Letter
pages: core content occupies pages 1--12, references start on page 13, and
appendices occupy pages 15--16. Pages 1, 11, and 12 were inspected at high
resolution; the shorter abstract and resized Figure 12 are legible without
clipping or overlap. All fonts are embedded, and the PDF contains no
annotations or identifying author metadata. `git diff --check` passes. The
final PDF SHA-256 is
`8683e14c361933a0819651657f49f2e6f6ec13c14d95b40310f4860606f4aa92`.
The preserved `paper/manuscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Prior Academic-Writing Review

The read-only GPT-5.6 Terra review is recorded in
`paper/reviews/mobicom_academic_writing_review_terra_max.md`. It reported one
P0, three P1, and one P2 finding covering the workload boundary, metric
vocabulary, abbreviations and captions, the Related Work-to-IPI transition, and
the workload paragraph's inventory structure. The revision above implements
those findings; the review document remains unchanged.

## Prior Workload-Framing Correction

Complete. The paper asks whether the tested communication paths carry CAV
application workloads within their time and availability requirements. The
private-5G application workloads are encoded in `IPI-CooperativeService`
frames. The experiments vary workload object size, direction, transport,
competing traffic, logical-client demand, location, and interruption, then
measure request/response RTT and all-attempt deadline availability. Auxiliary
direction and background streams remain identified as competing load.

The abstract, introduction, setup, results, figures, conclusion, and appendix
use this workload-centered framing. The manuscript and proposal record define
the evaluation through the application workloads carried over each path and
their measured timing and availability. Response-only labels in the regenerated
figures now use `workload response` or `received response`.

Updated manuscript files:

- `paper/manuscript/sections/00_abstract.tex`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/manuscript/sections/08_appendices.tex`
- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/proposed_changes.md`

Validation: the figure-generation script and `make all` completed successfully.
The final LaTeX log has no undefined citation/reference, overfull box, or
compilation error. The current PDF contains 16 US-Letter pages, uses embedded
fonts, starts references on page 13, starts appendices on page 15, and contains
no annotations or identifying author metadata. Pages 1, 3--9, 11--12, and 15
were rendered and visually inspected; the revised text and figure labels are
legible and have no clipping, overlap, or broken layout. The preserved full
draft remains byte-identical at
`paper/manuscript/28p.pdf`, SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The corrected current PDF is `paper/manuscript/main.pdf`, SHA-256
`107af03cc356cc44fbb9fb3d3870b16fe88a8e53a62d5950d7711873ae018ba5`.

## Prior Limiting-Factor Analysis Status

Complete. The reproducible analysis is in
`paper/analysis/limiting_factors/`. It covers 364 condition-level observations
and 1,708,263 attempts from the private-5G, PC5, and valid Airspan application
experiments. Public V2X file sizes and one complete detector benchmark are used
as supporting workload and conditional compute evidence. Conditions, rather
than attempts, are the statistical unit.

The PCA uses accepted-response p50, p95, and p99 RTT, tail inflation, and the
all-attempt failure fraction. Its first component explains 62.15% of variance
and captures latency/tail burden. Its second component explains 20.20% and
captures availability loss. Matched contrasts and an explicit evidence rubric
produce this ordinal ranking: 1) payload and representation scale; 2)
concurrent demand under no verified resource isolation; 3) operating path and
spatial availability; 4) service interruption and state continuity; 5) message
formation and transport semantics; 6) directional capacity and frame
allocation; and 7) local perception compute time, conditionally.

The analysis does not rank PC5 RF quality, the causal TDD effect, network-
enforced 5QI, weather, vehicle speed, or independent-UE density because the
required controls or telemetry are absent. In particular, no usable PC5
RSSI/SNR/RSRP/RSRQ or scheduler counters exist, and the latency-derived
mobility signal score is excluded as circular. The manuscript source and raw
result artifacts were not modified.

Generated artifacts:

- `paper/analysis/limiting_factors/limiting_factor_analysis.md`
- `paper/analysis/limiting_factors/condition_level_metrics.csv`
- `paper/analysis/limiting_factors/matched_contrasts.csv`
- `paper/analysis/limiting_factors/pca_loadings.csv`
- `paper/analysis/limiting_factors/pca_condition_scores.csv`
- `paper/analysis/limiting_factors/pc5_mobility_spatial_bins.csv`
- `paper/analysis/limiting_factors/factor_ranking.csv`
- `paper/analysis/limiting_factors/analysis_summary.json`
- four rendered PNG figures in the same folder
- `scripts/analyze_cav_limiting_factors.py`

Validation: the script rebuilds all outputs without error; Python compilation
passes; `git diff --check` passes for the new script, report, and task record;
the four figures were rendered and visually inspected.

## Prior Condensed-Manuscript Status

Complete. The prior full-length rendering remains byte-identical at
`paper/manuscript/28p.pdf`, and the current source builds the condensed paper as
`paper/manuscript/main.pdf`. Core content occupies pages 1--12, references
begin on page 13, and appendices occupy pages 15--16. The source retains the
approved outline, exactly two contributions, and exactly three insights.
Detailed IPI, radio, workload, and experiment-matrix records remain in the
post-bibliography appendices.

The rewrite now states the paper-wide cause before the observed envelope:
current direct and Uu radio/service paths do not jointly bind application
object, direction, deadline, and state to admission, scheduling, and recovery.
Related Work synthesizes what standards, application-specific systems, and
field studies collectively provide and identifies the remaining shared
application-boundary and evaluation gap. The 70/20/10 follow-up is explicitly
motivated as a test of latency sensitivity to directional TDD allocation, with
the historical-to-follow-up evidence scoped as deployment-level sensitivity.
The signal survey, TDD comparison, and cross-application synthesis are now
figures, and their captions state the comparison a reader should infer.

The abstract contains 166 words. The Introduction contains 402 words; Related
Work contains 569; IPI contains 439; Setup contains 1,967; Results contains
2,237; Future Research Directions contains 281; and the Conclusion contains
163 words. Approval-only implementation and experiment expansions remain in
`paper/manuscript/proposed_changes.md`; none is claimed as current behavior.

## Final Condensed-Draft Validation: 2026-08-13

- Current manuscript: `paper/manuscript/main.tex` with Sections 0--7 and
  post-bibliography appendices A--C.
- Current rendered draft: `paper/manuscript/main.pdf`.
- Preserved full draft: `paper/manuscript/28p.pdf`.
- Structure: exactly two contribution bullets and exactly three approved
  insight bullets; Section 5 repeats the same three insights.
- Workload boundary: J2735 functional checks, PC5 custom sequence-matched
  workload echoes, and 5G IPI workload frames remain separate. The evaluation
  measures workload response RTT and deadline availability on each path.
- Radio boundary: PC5 results use field-point and route language rather than an
  unavailable signal-strength metric. Certification is attributed to the
  vendor-reported product families. The follow-up MG52 Cell 2 lock is
  operator-verified; the iPhone survey is separate-UE spatial context.
- PDF validation: 16 US-Letter pages total; pages 1--12 contain the core paper,
  references begin on page 13, and appendices begin on page 15. The PDF is
  598,496 bytes; all fonts are embedded; no hyperlinks or annotations; anonymous
  visible authorship and metadata; no undefined citations or references,
  LaTeX errors, or overfull boxes.
- Visual validation: all 16 pages were rendered and inspected. The gap, IPI,
  measured-path, signal-survey, traffic-pattern, TDD, cross-application, and
  result figures have no clipping, overlap, missing glyph, or broken float.
  The cross-application synthesis appears with its discussion on page 11, and
  the research directions and conclusion continue on page 12.
- Preserved full-draft SHA-256:
  `6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
- Condensed PDF SHA-256:
  `53b368b9cb7b6f7786983f43ddfe2adc3ca45218bf2f42fb9e202a56ec6268f0`.
- Deferred proposals: P-001 through P-004 in
  `paper/manuscript/proposed_changes.md` require user approval plus
  implementation, tests, and/or new experiments.

## Evidence-Source Correction: 2026-08-12

The user closed R1A as unnecessary, verified that the MG52 was locked to
Airspan Cell 2, and clarified that the retained RSRP/RSRQ/SNR survey was
collected with an iPhone. The iPhone is a separate UE; its samples provide
spatial radio context but are neither MG52 telemetry nor per-request channel
measurements. Completed application-layer results may be analyzed without an
MG52 radio export. Matched Airspan counters remain necessary only for claims
about cell counters, contemporaneous cell load, or the causal effect of the
TDD configuration.

Updated files:

- `remaining_exp.md`
- `experiment_summary.md`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/proposed_changes.md`
- `current_task.md`

Validation: `make clean all` completed successfully. The final LaTeX pass has
no undefined citation/reference, overfull box, or compilation warning matched
by the manuscript validation scan. The 28-page US-Letter PDF has all fonts
embedded, no annotations or hyperlinks, and no identifying author metadata.
Pages 11, 14, 16, 17, 20, and 22 were rendered or text-checked around the
affected setup, table, result, and evidence-boundary material. No clipping or
overlap was observed. Raw result-tree summaries were not rewritten because they
are retained with checksum-backed acquisition provenance; this correction in
the current manuscript and repository-level records supersedes their old MG52
source assumption.

## Prior Experiment Status

The static Airspan inventory is partially complete. The existing experiments
used the `40/40/20` TDD configuration, and the planned follow-up uses
`70/20/10`; both report a `10D4G` structure. The deployed system did not support
the attempted `30/60/10` allocation as a usable experiment configuration, so it
produced no valid result and is a platform constraint rather than a measured
condition. The user identifies the three slash-separated components,
in order, as downlink, uplink, and dynamic frame allocations. The original
profile therefore contains 40 downlink, 40 uplink, and 20 dynamic frames; the
follow-up contains 70 downlink, 20 uplink, and 10 dynamic frames. The new
profile is downlink-heavy and is not an uplink-enhanced configuration. The
network administrator clarifies that the unchanged `10D4G` value is frame
packing for LTE coexistence. Record it for reproducibility, but do not analyze
it as part of the TDD comparison.
Header-verified per-cell carrier values and current
RF power/EIRP are now available. The complete grant record and antenna
azimuth/downtilt remain to be collected. The current SAS provider is now
identified from the 2026-07-31 CBRS configuration export. The vehicle CPE is
now identified as a Cisco Meraki `MG52-HW`, and its placement, orientation,
current firmware, and wired-link state are recorded below. TDD is the only
intended changed parameter for a newly collected same-cell comparison. A
historical-to-follow-up comparison also spans collection dates and cannot be
described as the same controlled causal comparison. The user confirms that
the original experiments used DNN `cisco5g` and were assigned the default 5QI
9 treatment.
The numeric QFI, any attempted nondefault 5QI assignment, and the ACP
performance-counter scaling remain unresolved.
The signed installation analysis now documents the outdoor mounting structure
and antenna centerline height.
The follow-up configuration plan is confirmed, and the host-side R1 validation
traffic and telemetry are now recorded under
`results/real_5g/20260805_airspan_r1_run_1/`. The MG52 must remain locked to one
Airspan cell so that no handoff occurs. The runbook requires Cell 2 because
current evidence places the original experiment traffic on Cell 2. If another
cell is selected, a new `40/40/20` reference must be collected on that same
cell before `70/20/10`.

On 2026-08-05, the user authorized installing the current IPI receiver-side
deployment on `d1` and directed the active collection to use RTT only. The user
waived R1A and the phone logger for this collection, reports that ACP and the
MG52 configuration have already been verified, designated the 11:15-11:30 EDT
ACP interval as R1 V1, and directed this host to collect V2 and V3 before the
then-planned C1-C5 block. The host-side collection is stored. Treat the Cell 2
lock as operator-verified; matched Airspan evidence remains necessary only for
cell-counter, cell-load, or TDD-effect claims. The retained iPhone survey is
separate-UE spatial context, not MG52 or per-request telemetry. Do not claim
one-way latency for this collection.

On 2026-08-06, the fixed-location C1-C4 block completed under the operator-
reported `70/20/10` and `10D4G` controls and the operator-verified Cell 2 lock.
The MG52 remained flat
on the trunk floor with its front face upward. The user corrected the current
location from the planned stronger label to `medium_typical_deployment`: it is
intended to represent the common signal strength a device might receive in a
real deployment. The label records the collection location, while the retained
iPhone survey supplies separate-UE spatial context rather than MG52 telemetry.
Raw per-transport `stronger/outside` fields are retained only as collection-
history provenance and are superseded by `metadata_corrections.json`.

The valid matrix contains all 24 expected C1-C4 samples: two repetitions of
TCP, MQTT, and UDP in separate five-minute ACP bins. It ran from the 09:05 EDT
bin through a final workload finish at 12:14:03 EDT. The matrix records 309,000
attempts, 307,662 accepted requests, and 1,338 failures, for 99.566990% overall
success. Three incomplete or recorder-transition attempts are preserved and
excluded from the valid repeats. Because the sender sleeps for 200 ms after
each completed request, the block used 500 probes per client; C4 therefore
contains 50,000 attempts per repetition and transport. One-way latency remains
invalid because the endpoint clocks were not synchronized.

Automatic GNSS initially could not connect and recovered at 10:39:02 EDT. Ten
of the 24 valid samples contain in-window ROS-derived positions; the fixed
location for the earlier samples is supported by the operator-reported
coordinate. Exact operator and ROS coordinates plus rosbag databases remain in
the locally excluded raw backup. Repository-facing coordinates are rounded to
0.001 degree and the repository-facing rosbag copies are omitted.

At 14:24 EDT on 2026-08-06, the user confirmed that the vehicle was stationary
at the weak location. The preflight found the local and edge experiment ports
idle, no experiment processes, the intended run roots absent, the edge route
on `eno2`, successful edge and GNSS reachability, and an open GNSS receiver
data port. A short ROS capture produced a position fix. The exact fix remains
in the excluded raw evidence directory; the repository context records only a
0.001-degree coordinate.

The weak-location C1/C2 matrix then completed from the 14:30 EDT bin through a
15:28:15 EDT final workload finish. All 12 expected samples passed: two
repetitions of TCP, MQTT, and UDP for each workload in separate five-minute ACP
bins. Across 6,000 attempts, 5,994 were accepted and six failed, for 99.9%
overall success. C1, C2/TCP, and C2/MQTT accepted every attempt. C2/UDP accepted
497/500 attempts in each repetition; all six failed rows report UDP
acknowledgment timeouts. The failures are retained as a repeatable observation
and are not replaced by retries. Automatic GNSS supplied in-window positions
for all 12 samples. The redacted result is stored under
`results/real_5g/20260806_airspan_followup_weak_run_1/`, and the excluded raw
backup retains exact GNSS and deployment details. Treat `weak` as a collection-
location label. The retained iPhone survey provides separate-UE spatial context
and is not an MG52 application-path measurement.

At 15:53 EDT on 2026-08-06, the user reported that the vehicle had moved to an
intended strong location. A short ROS preflight produced a position fix and a
median horizontal speed below 0.003 m/s, confirming that the vehicle was
stationary.
The local and edge experiment processes and ports were idle, the edge and GNSS
receivers were reachable, all seven CTests passed, and TCP, MQTT, and UDP
preflight checks passed. The C1/C2 matrix completed with 12 declared ACP
samples spanning 16:00-17:00 EDT, with application workloads from 16:00:15
through 16:57:42 EDT. All 12 samples passed validation. Across 6,000 attempts, 5,999
were accepted and one failed, for 99.983333% overall success. C1 and C2 TCP and
MQTT were lossless; C2/UDP accepted 999/1,000 attempts, with the single failed
row reporting a UDP acknowledgment timeout in repetition 2. All 12 samples
contain in-window GNSS positions. Exact coordinates and deployment details are
checksum-verified in the excluded raw backup; the repository copy is sanitized,
uses 0.001-degree GNSS precision, and omits serialized rosbags. Both hosts were
clean after collection.

At 17:14 EDT, the user reported inspecting RSRP for this block and the original
common/typical block and finding the same 109-110 range for both. The attempted
strong location therefore did not create a distinct radio condition. This
block is corrected to a second `medium_typical_deployment` observation under
`results/real_5g/20260806_airspan_followup_common_typical_run_2/`. Its original
`strong` run name, location ID, manifests, schedule entries, and sender labels
remain unchanged inside the collected artifacts as acquisition provenance;
`operator_context.json` and `metadata_corrections.json` supersede those fields
for every analysis and comparison. The corresponding excluded raw backup was
renamed without modifying its contents, and its original checksum manifest
still verifies. The user later clarified that the reported RSRP values came
from an iPhone rather than the MG52. Another C1/C2 block was then collected at a
candidate strong location with a distinct iPhone reading.

At 17:49 EDT, the user directed the next candidate strong-location experiment
to start. Preflight for
`20260806_airspan_followup_strong_run_2` passed: the repository matches
`origin/main`, the C++ build and all seven CTests pass, required scripts compile
or pass shell syntax validation, the local and edge experiment processes and
ports are idle, the intended local and remote run roots were absent, the d1
route uses `eno2`, and the edge and GNSS endpoints are reachable. Authenticated
TCP, MQTT, and UDP runner preflights also pass. A 25-second ROS GNSS capture in
the excluded raw evidence directory produced 5,387 position rows, a median
horizontal speed of 0.0014 m/s, a 95th-percentile speed of 0.0023 m/s, and less
than 0.7 m coordinate spread, confirming that the vehicle is stationary. The
operator then reported a current RSRP reading of 101, distinct from the
109-110 range that identified both prior common/typical observations. On
2026-08-12, the user clarified that these readings came from an iPhone and are
separate-UE spatial context rather than MG52 telemetry. The C1/C2 matrix started
at 17:59 EDT and used all 12
declared five-minute ACP samples from 18:00-19:00 EDT. Its application workloads
ran from 18:00:15 through 18:57:33 EDT. All 12 samples passed validation and all
6,000 attempts were accepted, for 100% application success with no preserved
invalid attempt. Automatic GNSS supplied in-window positions for every sample.
The exact-data backup and repository copy are independently checksum-verified;
the repository copy is sanitized, uses 0.001-degree GNSS precision, and omits
serialized rosbags. All 12 public sender CSVs remain byte-identical to the raw
backup, and both hosts were clean after collection. The block is stored under
`results/real_5g/20260806_airspan_followup_strong_run_2/`. The application-layer
result is complete. Matched Airspan evidence is needed only before attributing
the result to cell load, cell counters, or the TDD configuration.

On 2026-08-10, the repository-facing R1 and four Airspan follow-up result trees
were included in this publication update together with their analyzers, collection
scripts, run summaries, and updated experiment documentation. All stored
checksums, the C++ build and seven CTests, Python compilation, Bash syntax, and
source/documentation whitespace checks pass. Captured raw SSH logs retain their
original CRLF and trailing whitespace because rewriting them would invalidate
their manifests. A scan of 1.37 GB of publishable artifacts found no
credential, deployment RFC1918 or link-local address, raw hostname, exact
deployment path, or non-coarsened GNSS coordinate. Generic loopback and
unspecified listener literals remain because they disclose no deployment
address. The 3.6 GB exact-data backups under `CISCO_AIRSPAN_STATS/`, serialized
rosbags, and local compiled Mocar objects/binaries remain excluded from version
control. Repository-facing GNSS CSVs retain only 0.001-degree coordinates.

The pending experiment runbook now begins with a vehicle-day execution order.
It requires a pull/build/test preflight, the three-bin R1 validation, and then
the two-repetition C1-C6 matrix. R1A is closed as unnecessary. The `70/20/10`
R1/C1/C2 measurements can supply the follow-up side of R9 without duplicate
probes. A newly collected same-cell `40/40/20` reference is required for a
controlled causal comparison; otherwise R9 remains a historical-to-follow-up
sensitivity comparison. Historical result scripts must not be run in place
because some retain historical run IDs and output paths.

## Cisco And Meraki Information Retrieval

- The Cisco Networking Learning Hub URL supplied by the user is a general
  Meraki training catalog. It does not expose deployment configuration.
- The Meraki Dashboard can document the MX250 model, firmware, WAN/LAN status,
  active ports, VLANs, routes, visible clients, layer-2 topology, events, and
  packet traffic visible on routed MX interfaces. A read-only click path and
  evidence checklist are now recorded in `remaining_exp.md`.
- The user confirms that the vehicle gateway/CPE is a Cisco Meraki
  `MG52-HW`. In `Cellular Gateway > Monitor > Cellular gateways`, its `Uplink`
  and `Performance` views can provide the active RAT and band, APN, RSRP,
  RSRQ, link state, and time-ranged performance fields. Subscriber and device
  identifiers must remain in the excluded evidence directory.
- Meraki MX evidence cannot establish Airspan TDD/RAN configuration or an
  active QoS flow's QFI and 5QI. The user has supplied written confirmation for
  DNN `cisco5g` and default 5QI 9. Numeric QFI and nondefault QoS-flow evidence
  are needed only if the optional QoS comparison is pursued. Exact Cisco-core
  inventory and internal path decomposition are outside the current paper
  scope.
- Meraki packet captures can show packet presence, direction, DSCP/TOS, and
  timing at the selected MX interface. They cannot by themselves isolate
  over-the-air delay or prove a 5QI/QFI assignment.
- The user confirms that the application edge computer is directly connected
  to the MX250. This removes an external WAN or cloud hop after the local 5G
  system. It does not make the gNodeB-to-edge path literally zero-delay: RAN,
  core/UPF, MX250, Ethernet, and endpoint processing remain within the
  application RTT unless bounded separately. Treat the local wired segment as
  expected to be small, not as measured zero latency.

### Vehicle Gateway/CPE

- The user identifies the vehicle CPE as a Cisco Meraki `MG52-HW` with an
  internal antenna. During all original experiments, it was placed flat on the
  bottom of the vehicle trunk with its front face oriented upward; this
  placement did not change between runs. The current dashboard reports MG
  firmware `4.1.2`, modem firmware
  `M0R.115005`, host version `A0R.501056`, carrier PRI version `P0R.000566`,
  and carrier profile `Generic GCF`. The experiment-side Ethernet link reports
  `1000 Mbit/s`, full duplex. The user confirms that these firmware and link
  values match the original experiment campaigns. Report the observed wired
  link separately from vendor maximum data rates.
- Cisco documents the MG52 as a 5G SA/NSA sub-6 gateway using the Telit
  FN990A40 radio, with an internal antenna, two 2.5 GbE ports, maximum
  passthrough rates of 2 Gbit/s downlink and 300 Mbit/s uplink, and 4x4
  downlink MIMO. The 1 Gbit/s negotiated Ethernet link used by the experiments
  is below the device's port capability and is the relevant wired-link value
  for the measured system:
  <https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/Overviews_and_Datasheets/MG52%252F%252F52E_Technical_Specifications>.
- The FN990 family hardware guide lists typical conducted n48 receiver
  sensitivity at 30 kHz subcarrier spacing and 40 MHz bandwidth as -91, -91,
  -93, and -92 dBm for its four antenna ports and -97 dBm combined. These are
  modem-level conducted values measured under the guide's stated laboratory
  conditions. They are not a measured whole-device sensitivity value for the
  MG52 with its enclosure and internal antennas:
  <https://fccid.io/RI7FN990A40/User-Manual/User-Manual-7361571.pdf>.
- Cisco states that the MG52 internal antennas have a front-face bias. The
  original trunk-floor, front-face-up orientation is now recorded. Every
  follow-up run must record whether it retains or deliberately changes this
  placement:
  <https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/MG_Antenna_Datasheets/MG52_Internal_Antenna_Datasheet>.

## Airspan Information Retrieval

- User-reported ACP configuration values recorded on 2026-07-31: the deployed
  `AS29-N48-DSC1` is an all-in-one AirSpeed 2900 with one xPU, CU-CP, CU-UP,
  DU, and RU; two enabled DU cells map one-to-one to two RU sector carriers.
  Both sector carriers inherit the DU configuration, have CBRS enabled, are
  registered as CBSD Category B, report an antenna-gain value of 17 with
  cross-polarization enabled, and
  obtain CPI information from the SAS. The clock source is GNSS, the UE
  inactivity timer is 7 s, the primary PLMN is CBRS MCC 315/MNC 010, and the
  configured slice is eMBB with SST 1 and SD 000002. `Frequency Reuse One` and
  the CBRS measurement-capability field are both reported as disabled. These
  hardware, cell, and CBRS values are now supported by header-bearing ACP CSV
  exports and the 2026-07-31 network configuration export in the connected
  Drive folder.
- Exact node and managed-element identifiers, management addresses and
  usernames, internal user IDs, cell identities, and GNSS coordinates supplied
  in the same transcription are intentionally omitted from tracked
  documentation. Retain them only in the locally excluded
  `CISCO_AIRSPAN_STATS/` evidence directory and redact them from manuscript
  material.
- The user confirms that both cells used `40/40/20` with `10D4G` during the
  original experiments. The follow-up will use `70/20/10` with `10D4G` on the
  selected serving cell, and the MG52 will be locked to that cell so no handoff
  occurs. Use Cell 2 to preserve comparability with the original campaigns.
  Current observations show that Cell 2 served the vehicle UE and carried the
  original experiment traffic; Cell 1 did not carry that traffic. The July 31
  export reports an intermediate reconfiguration state in which Cell 1 was
  active at `30/60/10` and Cell 2 was locked at `40/40/20`; do not describe
  that snapshot as either measured experiment configuration. Both CBSD entries
  report `Authorized` with successful SAS heartbeats. The software inventory
  marks `22.00-53-0.0` with platform `22.0-24-0.0` as the current package.
- Header-bearing ACP exports dated 2026-07-31 confirm that both cells use band
  n48, TDD, 40 MHz bandwidth, 30 kHz PDCCH/SSB subcarrier spacing, and a 20 ms
  SSB periodicity. Cell 1 reports NR-ARFCN 637992 and SSB frequency
  3558.720 MHz; Cell 2 reports NR-ARFCN 645334 and SSB frequency 3669.600 MHz.
  Applying the 3GPP NR-ARFCN mapping gives reference frequencies of 3569.880
  MHz and 3680.010 MHz, respectively. The 2026-07-31 network configuration
  export captured Cell 1 as active/unlocked at `30/60/10` and Cell 2 as
  locked at `40/40/20`, both with `10D4G`. This was an intermediate
  reconfiguration snapshot, not a measurement configuration. The network
  administrator identifies `10D4G` as a fixed frame-packing setting used for
  coexistence with LTE systems. It is unchanged between the configurations and
  is not an experimental variable or an explanation for measured performance.
- The same active configuration export records a maximum of one uplink layer
  and two downlink layers for each cell, with downlink 256-QAM enabled. These
  are configured cell capabilities, not measured modulation, rank, or
  throughput during an application run.
- The 2026-07-31 configuration export records ACP statistics collection at a
  15-minute granularity. For the 2026-08-05 R1 run, the operator reports that
  the current interval is five minutes; the matching current export is still
  pending. The active configuration enables KPI families for PRB usage, DRB UE
  throughput, RACH, QoS flows, and L1 measurements, among others. This
  establishes which KPI families were enabled in the stored configuration, but
  it does not establish the current interval or the exact counters, dimensions,
  and per-UE fields available from an export.
- The header-bearing CBSD export reports, for each cell, 40 MHz bandwidth,
  33 dBm cell transmit power, 34 dBm/MHz cell EIRP, `0/4` under
  `PAL/GAA Count (10MHz)`, one requested and authorized grant, no suspended
  grant, and a successful SAS heartbeat. The header order supports reading
  `0/4` as zero PAL and four GAA 10 MHz assignments. The 2026-07-31 CBRS
  configuration export identifies Federated Wireless as the enabled primary
  SAS connection, using CBRS configuration version 1.2, with no backup domain
  proxy enabled. It does not include the four individual grant records, so
  grant-level frequency and power evidence remains unresolved.
- The user's definitive configuration record establishes that both cells used
  `40/40/20` with `10D4G` during the original experiments. The current
  follow-up uses `70/20/10` with `10D4G` on the MG52-locked serving cell. The
  user identifies the order as downlink, uplink, and dynamic frame allocation.
  The baseline therefore has 40 downlink, 40 uplink, and 20 dynamic frames; the
  follow-up has 70 downlink, 20 uplink, and 10 dynamic frames. TDD is the sole
  intended changed parameter only when the same cell, channel, signal
  condition, placement, endpoints, and application settings are retained. A
  newly collected same-cell baseline permits that controlled interpretation;
  a comparison only with historical results does not. Retain the separate
  `10D4G` value for reproducibility, but do not treat it as part of the
  comparison.
- Two ACP screenshots added on 2026-07-31 document gNodeB network and 5QI
  properties. The earlier network-properties screenshot displays
  `RLC Acknowledge Mode Support` as disabled, DSCP 48 for NG-C and Xn-C, DSCP
  16 for management, and the NG-C SCTP heartbeat/retry timers. The
  5QI-properties view shows DSCP disabled and
  `RLC-UMBidirectional` selected for each displayed 5QI profile from 1 through
  9. These are gNodeB-side profile settings. They do not identify the QFI/5QI
  assigned to an experiment PDU session and do not prove that a Cisco
  core-side QoS policy was applied.
- The 2026-07-31 network configuration export reports `RLC Acknowledge Mode
  Support` as enabled, and the user confirms that the live setting is enabled.
  Treat `Enabled` as the current capability setting and the earlier screenshot
  as a non-current view. This setting does not establish the RLC mode used by
  the measured bearer.
- In ACP, open the deployed gNodeB configuration and then the `gNodeB Network
  Properties` view; search for the exact field `RLC Acknowledge Mode Support`.
  In the active XML export, the equivalent field is
  `/root/GnbConfigs/GnbDetailsGet/GnbProperties/NetworkConfig/RlcAcknowledgeModeSupport`,
  which currently evaluates to `Enabled`. The separate `5QI Properties` view
  contains the per-profile `RLC Mode` row.
- The screenshot's UM setting means that the displayed profiles use RLC
  Unacknowledged Mode. 3GPP TS 38.322 assigns RLC ARQ error correction only to
  Acknowledged Mode, but lower-layer HARQ and transport-layer recovery remain
  separate mechanisms. Do not attribute application loss or latency to RLC UM
  without bearer-level evidence that the measured flow used one of these
  profiles:
  <https://www.etsi.org/deliver/etsi_ts/138300_138399/138322/17.03.00_60/ts_138322v170300p.pdf>.
- The user confirms that no other users share this private deployment during
  the experiments and that no interference from other bands is present. Treat
  the measurements as an isolated-deployment condition and do not introduce
  external-user load or inter-band interference as an explanation.
- A primary-source web search did not find a public Airspan definition of the
  `40/40/20`, `30/60/10`, or `70/20/10` labels. The user provides the
  deployment-specific mapping of the slash-separated fields to downlink,
  uplink, and dynamic frame allocations. 3GPP defines a TDD pattern through
  its periodicity and counts of full downlink/uplink slots and partial
  downlink/uplink symbols; remaining symbols are flexible. The network
  administrator separately identifies `10D4G` as frame packing for LTE
  coexistence. Because it remains fixed, the follow-up does not evaluate its
  effects:
  <https://www.etsi.org/deliver/etsi_ts/138200_138299/138213/17.03.00_60/ts_138213v170300p.pdf>.
- 3GPP TS 28.552 resolves part of the DU-counter interpretation. A mean active
  UE is a UE with DRB data available for uplink, downlink, or both, not every
  attached UE. Its standard average UE-throughput metric is intended for bursts
  spanning multiple slots; a small burst contained in one initial HARQ
  transmission has zero throughput-measurement time and belongs to the
  unrestricted-volume category. Short periodic probes can therefore coexist
  with a zero mean-active-UE value or contribute weakly to the displayed DRB
  throughput. Airspan must still confirm ACP scaling, rounding, sampling, and
  whether its exported columns implement these standard measurements:
  <https://www.etsi.org/deliver/etsi_ts/128500_128599/128552/18.07.00_60/ts_128552v180700p.pdf>.
- The Airspan installation guide filed under FCC ID `PIDAS2900` maps product
  code `AS29-N48-DSC1` to the integrated-antenna, DC-powered n48 variant. It
  specifies an integrated 17 dBi dual-slant antenna with 65-degree azimuth and
  8-degree elevation beamwidth and support for two carriers as `2x 2T2R`. For
  its 40 MHz, 17 dBi antenna configuration, the guide lists equipment maxima of
  33 dBm conducted power, 44.23 dBm EIRP per 10 MHz, and 50 dBm EIRP over the
  bandwidth. These are certified product capabilities, not evidence of the
  configured transmit power or the SAS-authorized EIRP used by this deployment:
  <https://fccid.io/PIDAS2900/User-Manual/User-Manual-5885368.pdf>.
- The locally excluded signed structural analysis in `CISCO_AIRSPAN_STATS/`
  identifies the installation as a 21 ft tripod/mount structure and places one
  AirSpeed 2900 at a 20 ft antenna centerline above roof level. Its antenna
  placement sketch is not referenced to north and gives no mechanical or
  electrical downtilt. It therefore supports the outdoor placement and height
  fields, but not azimuth or downtilt. Exact site coordinates and project
  identifiers remain only in the excluded evidence file.
- The Drive coverage-prediction presentation labels the AirSpeed 2900 model
  input as 20 ft above ground level and shows predicted downlink SS-RSRP, not
  measured coverage. The signed structural analysis instead describes a 20 ft
  antenna centerline above roof level. Confirm the actual installed height
  above ground and the propagation-model height before using either value or
  the predicted map in the paper.
- The ACP software and network-function exports confirm AirSpeed 2900 product
  code `AS29-N48-DSC1`, platform version `22.0-24-0.0`, application version
  `22.00-53-0.0`, and the all-in-one xPU/CU-CP/CU-UP/DU/RU composition. The
  board value `xpu_2200` remains user-reported because it is not present in
  these exports.
- The current DU Cell CSV contains performance counters only. It does not
  establish the exact gNodeB model, CBSD category, spectrum authorization,
  channel configuration, antenna installation, CPE model, or receiver
  sensitivity.
- In ACP, use the selected gNodeB's read-only `Inventory`, `Software`, `5G
  Cells`, `Status`, `Provision`, and `Network Functions` views, plus the
  top-level `Topology` view, to record installed hardware, software, cell, and
  network-function configuration. Do not save or apply configuration changes.
- Obtain individual grant parameters, authorized frequency range/EIRP, and
  remaining Category-B installation parameters from the SAS grant and CPI
  records. These values must not be inferred from the product datasheet.
- The gNodeB and gateway/CPE models are now confirmed as an AirSpeed 2900 and
  a Meraki `MG52-HW`. The available receiver-sensitivity value is for the MG52's
  FN990A40 modem under conducted laboratory conditions; do not present it as a
  measured whole-gateway value.
- Numeric QFI and nondefault QoS-flow configuration require Cisco-side records
  if the optional QoS experiment proceeds; they are not Airspan DU Cell fields.
  Cisco-core placement is outside the current paper scope.
- The user confirms that the original experiment sessions used DNN `cisco5g`
  and were assigned default 5QI 9. No numeric QFI was provided. This confirms
  the default session treatment but does not validate the separate
  `5qi-mapped` application label as a nondefault network-enforced QoS flow.
- The connected Drive folder's physical-SIM and eSIM exports contain inventory
  and session columns, including a `UPF Name` header, but they do not provide a
  usable UPF identity, DNN/APN, 5QI, QoS-flow evidence, MX250 configuration, or
  gateway/CPE model. They do not verify a nondefault QoS assignment.
- Airspan's SR22.00 ACP release notes require the ACP server to be installed in
  UTC. This is a product requirement, not proof of how this ACP version renders
  CSV timestamps for the logged-in user; the runtime export timezone still
  requires verification.
- `All_Config.xml` is a collection of generic ACP profile templates rather
  than a record of the profile assigned to the deployed AirSpeed 2900. Do not
  use its template values as deployment facts. The separate
  `Airspan_NetworkConfigExport_20260731_1234.xml` contains the active gNodeB
  configuration and is the source for the current cell, TDD-label, clock,
  inactivity-timer, and KPI-collection settings.
- Keep raw screenshots, exports, identifiers, and vendor records under the
  locally excluded `CISCO_AIRSPAN_STATS/` directory. Only redacted,
  source-attributed values may enter the manuscript or tracked result files.

## Uplink Interpretation And TDD Pilot

- Added evidence-scoped notes for reporting bandwidth/spectrum, quantitative
  stronger/lower coverage, problems observed under the phone's highest
  signal-bar level, measured-versus-theoretical configuration, explicit
  uplink/downlink direction, and positive design implications.
- Corrected `low-latency protocol` to `low-latency QoS flow` or
  `low-latency network treatment`; no such network-enforced flow has yet been
  verified in the measured deployment.
- Recorded prospective uplink improvements: uplink-oriented TDD allocation,
  verified QoS/scheduling, exterior and distributed vehicle antennas,
  regulation-compliant power/link-budget improvements, beamforming, coordinated
  beam processing, and coverage-aware operation.
- Recorded the safety boundary: do not propose relaxing RF-exposure rules.
  Higher power, gain, EIRP, and beamforming must remain within equipment,
  certification, CBRS, and exposure limits.
- Revised R9 to compare baseline `40/40/20` with follow-up `70/20/10` on an
  MG52-locked serving cell; both configurations report `10D4G`. The
  slash-separated values are downlink, uplink, and dynamic frame allocations:
  40/40/20 for the baseline and 70/20/10 for the follow-up. The attempted
  `30/60/10` configuration produced no usable measurement. R9 is an exploratory
  directionality sensitivity study. Because `70/20/10` reduces uplink
  allocation, it must not be presented as an uplink-enhanced profile. No live
  comparison has yet been recorded.
- Added primary standards, regulatory material, measurement literature, and
  vehicular-antenna/beamforming source leads for later manuscript review.

## Active Constraints

- Use the complete current `results/real_5g/` tree rather than one run or the
  manuscript summary alone.
- Preserve negative experiments as results; do not relabel an attempted but
  unverified QoS experiment as though it never occurred.
- Distinguish end-to-end application measurements from RAN/core telemetry and
  causal diagnosis.

## Files Touched In Current Task

- `current_task.md`
- `experiment_summary.md`
- `remaining_exp.md`
- `scripts/collect_host_telemetry.py`
- `scripts/analyze_airspan_followup_run.py`
- `scripts/analyze_edge4av_deadlines.py`
- `scripts/redact_followup_gnss.py`
- `scripts/sanitize_airspan_followup_results.py`
- `scripts/run_airspan_followup_condition.sh`
- `scripts/run_airspan_followup_matrix.sh`
- `results/real_5g/20260805_airspan_r1_run_1/`
- `results/real_5g/20260806_airspan_followup_run_1/`
- `results/real_5g/20260806_airspan_followup_weak_run_1/`
- `results/real_5g/20260806_airspan_followup_common_typical_run_2/`
- `results/real_5g/20260806_airspan_followup_strong_run_2/`

## Existing Experiments Relevant To The Proposed Work

- Five stationary baseline campaigns repeated payload and protocol conditions
  at different locations. Separate signal-survey measurements were assigned to
  those run locations by nearest-point selection or spatial interpolation.
- Stronger- and weak-signal versions exist for load, detector-output replay,
  and 1--100-client scalability experiments.
- Background-load sweeps vary offered uplink load and stream count, while
  multiclient sweeps vary application concurrency.
- A QoS verification experiment captured default and `5qi-mapped` TCP traffic
  at the vehicle interface. Both captures had `tos 0x0`; the experiment did not
  establish host marking or network-enforced 5QI treatment.
- Sender logs contain client/server timestamps and RTT, but all inspected
  private-5G sender files use `clock_sync_state=unsynced` and no synchronized
  CPE, gNodeB, UPF, or core timestamps are stored.

## Follow-Up Experiment Classification

1. Per-request RSRP/RSRQ/SNR or SINR, CQI, MCS, BLER, HARQ/RLC, and serving-cell
   telemetry: **not completed**. Existing radio values come from a separate
   16-point survey and are assigned at run/location level, not recorded for each
   request.
2. Controlled signal sweep using attenuation, transmit-power control, or another
   method that holds location and workload constant: **partially completed**.
   Existing experiments compare locations and stronger/weak-signal campaigns,
   but signal was not independently controlled and day/location effects remain.
3. CPE or antenna inside-versus-outside vehicle comparison: **not completed**.
   The internal-antenna `MG52-HW` remained on the trunk floor with its front
   face upward throughout the original campaigns, but no controlled
   inside-versus-outside comparison or associated run pair is stored.
4. Synchronized path decomposition across the CPE, RAN/core, and edge host:
   **excluded from the current paper scope**. The paper reports application RTT
   over the complete vehicle-to-local-edge path and does not attribute delay to
   an internal path segment. The edge computer is directly connected to the
   MX250, and the only stored packet captures are vehicle-interface captures
   for QoS verification.
5. Verified network-enforced 5QI comparison: **attempted with a negative
   verification result**. Application labels were tested, but packet captures
   showed no TOS/DSCP difference and no core-side QoS-flow evidence is stored.
6. RAN-internal load and scheduling experiment with PRB, scheduler, queue, CQI,
   MCS, BLER, or retransmission counters: **partially completed at the
   application level only**. Background-load and multiclient sweeps exist, but
   no RAN/core counters were collected.
7. Transmit-power, TDD, scheduler, or verified QoS configuration comparison:
   **not completed**. No run family changes these network configurations.

## Validation

- On 2026-08-05, completed a read-only car-host-to-`d1` readiness audit for
  the follow-up matrix. The car host routed `d1` traffic through `eno2`; five ICMP probes
  completed without loss, with a 15.5-26.3 ms diagnostic RTT range. Both the
  car-side `eno2` link and the `d1` wired link reported carrier up at
  1000 Mbit/s full duplex. This is a connectivity preflight, not an experiment
  result or a path-segment latency measurement.
- `d1` has the required TCP and UDP receiver executables, matching copies of
  the current root-level MQTT broker and load-generator helpers, a compatible
  CMake/C++17 toolchain, sufficient free CPU/memory/disk capacity, and no
  process or listener occupying the planned TCP, UDP, MQTT, or load-generator
  ports. The receiver help paths and helper-script syntax checks passed. No
  receiver, broker, load generator, or experiment collection was started.
- Preserved the stale, dirty historical `d1` checkout at `cb1e2b7` without
  pulling, cleaning, or rebuilding it. Staged the current `2c043b3` C++ source
  and helper programs in the separate deployment tree
  `/home/d1/edge4av_followup/ipi_2c043b3`; an rsync checksum dry run reported
  no source difference, and both helper hashes match the car-host copies.
- Configured and built the clean `d1` deployment with tests enabled. Because
  the installed CTest 3.16 does not support `--test-dir`, reran CTest from the
  build directory; all seven tests passed. The new TCP and MQTT receivers and
  the fragmented-UDP receiver have no missing dynamic libraries.
- Ran isolated `d1`-local deployment smoke probes without using the private-5G
  path or affecting R1. TCP and MQTT each accepted one 1024 B request, and UDP
  accepted one application-fragmented 23,968 B request. No smoke-test listener
  or process remained afterward.
- The `d1` clock reports a local chrony reference with no configured time
  source, so it is not verified as cross-host synchronized. The user has
  explicitly limited the active collection to RTT, for which cross-host clock
  synchronization is not required; keep `clock_sync_state=unsynced` and do not
  report one-way latency. `vmstat` is available, but `mpstat`, `pidstat`, and
  `sar` are absent. Added and deployed `scripts/collect_host_telemetry.py`, a
  one-second `/proc` collector for per-core CPU counters, memory, interface
  counters, and selected process counters. Three-sample smoke checks passed on
  both the car host and `d1`.
- `d1` cannot verify the Airspan TDD configuration. The user verified that the
  MG52 was locked to Cell 2. Retain matching Airspan artifacts before making a
  cell-counter, cell-load, or TDD-effect claim; they are not required to use the
  separately validated application-layer measurements.
- Added `scripts/run_airspan_followup_condition.sh` for one C1-C6 transport and
  repetition per declared ACP bin. It refuses existing result paths, uses only
  the clean `d1` deployment, keeps credentials and the edge address out of
  result metadata, records RTT with unsynchronized-clock labels, captures
  car/edge telemetry, supports C3's 25 Mbps offered uplink load and C4's 100
  clients, and preserves partial or failed sender files. The runner requires
  an exact 300-second ACP interval, reserves 15-second pre/post margins, caps
  sender execution at 260 seconds, and rejects a setup that misses the declared
  start tolerance or cannot fit the guarded sample. It now requires explicit
  `SIGNAL_CLASS`, `LOCATION_ID`, and `PHYSICAL_PLACEMENT` values so C1/C2 can be
  repeated at medium, weak, and strong locations without inheriting an
  incorrect signal label from the condition number. C6 now supplies the
  23,968 B weak-location workload missing from the original launcher.
- Added `scripts/run_airspan_followup_matrix.sh` to schedule exact five-minute
  boundaries, use forward/reverse transport order across two repetitions,
  reserve additional setup lead for C4/MQTT, stop on the first failed or
  incomplete transport, validate attempt counts, workload timestamps, signal
  class, location ID, and physical placement, and append a top-level schedule
  without recording credentials or endpoint addresses.
- The medium/typical C1-C4 application matrix passed 24/24 sample validation.
  Across 309,000 attempts, 307,662 were accepted and 1,338 failed. All 1,000
  pooled C1 and C2 attempts per transport succeeded. C3 had five UDP timeouts;
  C4 TCP and MQTT accepted all 100,000 attempts per transport, while C4 UDP
  accepted 98,667 of 100,000. C4 UDP accepted-count Jain fairness was 0.999980
  in repetition 1 and 0.999978 in repetition 2. Deadline rates include every
  attempt in the denominator, so timeout rows count as misses.
- Three non-primary attempts are preserved and explicitly excluded: two
  incomplete C3/TCP timeout-guard attempts and one C3/MQTT recorder-transition
  attempt. C3 repetition 1 TCP has no end-of-run load CSV; its achieved-uplink
  estimate comes from interface counters. The offered C3 load remains 25 Mbps,
  while achieved rates are reported separately and must not be called achieved
  25 Mbps.
- `scripts/analyze_airspan_followup_run.py` regenerated the 24-sample,
  condition/transport, C4 per-client/fairness, GNSS, invalid-attempt, run
  manifest, and validation summaries. Python compilation, live deadline-
  denominator validation on a C3/UDP file, Bash syntax checks, dry-run checks
  for C1, C2, C4, and C6 at explicit location classes, a missing-signal-class
  negative check, and `git diff --check` passed. ShellCheck is not installed.
- Automatic GNSS recovery and 10/24 samples with in-window positions are
  recorded. `scripts/redact_followup_gnss.py` verified 66 GNSS artifacts
  against the excluded raw backup, rounded 39 repository-facing GNSS CSVs to
  0.001-degree precision, and removed 27 repository-facing rosbag databases.
  The original exact CSVs and rosbags remain recoverable from the excluded raw
  backup.
- The excluded unredacted backup contains 2,601 checksum-verified files. The
  repository-facing copy contains 2,586 checksum-verified files plus its
  checksum manifest. A binary-inclusive scan found no endpoint credential,
  private endpoint address, user home path, raw hostname, MAC address, or exact
  GNSS coordinate in the repository-facing tree. No local experiment process
  or planned listener remained after collection.
- The weak-location C1/C2 matrix passed all 12 application-sample checks. It
  records 6,000 attempts, 5,994 accepted requests, and six failures. C1 and the
  C2 TCP/MQTT groups were lossless; C2/UDP accepted 497/500 attempts in each of
  its two repetitions, with all six failed rows reporting UDP acknowledgment
  timeouts. All 12 samples contain in-window GNSS positions, and no partial or
  transition attempt was created.
- The weak result was copied to an excluded unredacted backup before analysis.
  `scripts/redact_followup_gnss.py` verified the GNSS sources against that
  backup, rounded 24 public GNSS CSVs to 0.001-degree precision, and removed 12
  public rosbag databases. `scripts/sanitize_airspan_followup_results.py`
  normalized 24 host-telemetry files, removed process command lines, and
  replaced deployment-specific endpoints, hostnames, and paths. The exact
  originals remain checksum-verified and recoverable from the excluded backup.
  The repository-facing manifest verifies 352 files (353 including the
  manifest); the excluded raw manifest verifies 363 files (364 including the
  manifest). All 12 sender CSVs remain byte-identical to the raw backup, the
  public privacy and coordinate-precision scans passed, and no local or edge
  experiment process or planned listener remained after the block.
- The second common/typical-location C1/C2 matrix passed all 12 application-
  sample checks. It records 6,000 attempts, 5,999 accepted requests, and one
  failure, for
  99.983333% overall success. C1 and C2 TCP/MQTT were lossless; C2/UDP accepted
  999/1,000, with one repetition-2 UDP acknowledgment timeout. The failed row
  is preserved rather than replaced by a retry. All 12 samples contain in-
  window GNSS positions and fit their declared 16:00-17:00 EDT ACP bins.
- The second common/typical result was copied to an excluded unredacted backup
  before public redaction. The public GNSS pass rounded 24 CSVs and 791,434
  position rows to
  0.001-degree precision and omitted 12 rosbag databases. Public sanitization
  normalized 24 host-telemetry files containing 7,260 rows, removed 15,166
  process-command-line fields, and sanitized 69 other text artifacts. The
  public checksum manifest verifies 353 files (354 including the manifest),
  while the excluded raw manifest verifies 363 files (364 including the
  manifest). All 12 sender CSVs remain byte-identical to the raw backup; the
  binary-inclusive privacy and coordinate audit passed; and no local or edge
  experiment process or planned listener remained. A post-run CMake build, all
  seven CTests, Python compilation, Bash syntax checks, checksum verification,
  and `git diff --check` passed.
- The user subsequently compared RSRP for this block and the original
  common/typical block and reported the same 109-110 range. The user later
  clarified that these readings came from an iPhone, not the MG52. The repository run
  root, operator context, generated analysis, and correction metadata now label
  this as `medium_typical_deployment`; original `strong` collection fields are
  retained as provenance. The later candidate strong-location workload, which
  has distinct separate-UE iPhone radio context, is complete under
  `results/real_5g/20260806_airspan_followup_strong_run_2/`.
- Collected the host-side R1 validation run on 2026-08-05 and stored its
  redacted artifacts under
  `results/real_5g/20260805_airspan_r1_run_1/`. V1 is the operator-designated
  idle window 11:15-11:30 EDT. V2 ran from the 13:15 boundary through 13:30 as
  one 25 Mbps-offered car-to-edge TCP stream. TCP backpressure limited its
  full-window application rate to 13.118 Mbps; the client and server both
  recorded exactly 1,475,758,800 B. V3 ran from the 13:35 boundary through
  13:50 as one edge-to-car TCP stream and sustained 25.000 Mbps; both endpoints
  recorded exactly 2,812,500,000 B. Do not describe V2 as an achieved 25 Mbps
  condition.
- Both hosts produced all 910 expected one-second telemetry samples for each
  traffic window. Every load-generator, server, and local telemetry exit code
  was zero; sender/receiver byte totals matched exactly; interface error and
  drop counters did not increase; and no R1 listener, load generator, or
  collector remained afterward. The controller clock defines ACP alignment.
  The unsynchronized `d1` wall clock was approximately 173.9 seconds behind,
  so its raw timestamps cannot support one-way latency.
- Preserved the unredacted R1 host artifacts under the locally excluded
  `CISCO_AIRSPAN_STATS/20260805_airspan_r1_run_1_unredacted/` directory. The
  result copy uses logical endpoint labels and removes process command lines;
  a scan found no endpoint address, credential, user home path, or raw hostname
  in the redacted directory.
- On 2026-08-04, configured and built `cpp/` with tests enabled and ran all
  seven CTest targets successfully.
- Ran `bash -n` on the current load/QoS, multiclient, detector-output TCP/MQTT,
  and fragmented-UDP reference collection scripts; all passed syntax checking.
- Ran `git diff --check` on the five vehicle-documentation files; no whitespace
  errors were reported. Fetched `origin/main` and confirmed that local `HEAD`
  and the remote branch had no divergence before staging.
- Limited the vehicle publication scope to `AGENTS.md`, `agent_context.md`,
  `current_task.md`, `experiment_summary.md`, and `remaining_exp.md`. The local
  Cisco/Airspan evidence directory remains excluded, and unrelated paper map
  and chart scripts are not part of this publication.
- Verified that R9 distinguishes the historical `40/40/20` baseline from the
  planned locked-cell `70/20/10` follow-up and separates uplink and downlink
  measurements. A same-cell, newly collected baseline is required before TDD
  can be treated as the only changed parameter; a historical-only comparison
  is reported as a sensitivity comparison. One pair remains diagnostic rather
  than manuscript evidence.
- Ran `git diff --check -- remaining_exp.md current_task.md`; no whitespace
  errors were reported.
- R1 host-side traffic and telemetry are recorded, but the Airspan counter
  validation is not complete. The user verified the MG52 Cell 2 lock. Retain
  and inspect matching ACP DU-cell artifacts before making cell-counter, load,
  or TDD-effect claims. The existing radio survey was collected with an iPhone
  and is separate-UE spatial context rather than MG52 telemetry.
- Updated the vehicle runbook after the configuration change: Cell 2 is the
  required locked serving cell for comparison with the original campaign. The
  condition identifier C2 remains distinct from Airspan Cell 2.
- Reviewed all private-5G experiment families in `experiment_summary.md` and
  enumerated every top-level run directory under `results/real_5g/`.
- Checked all private-5G sender CSV headers. They contain application identity,
  condition, payload, endpoint timestamps, RTT, and outcome fields, but no radio
  or RAN telemetry fields.
- Searched private-5G artifacts for RSRP, RSRQ, SNR/SINR, CQI, MCS, BLER,
  HARQ/RLC, PRB, scheduler, queue, TDD, ARFCN, power, attenuation, CPE, and
  antenna records. Radio metrics occur only in the separate signal-survey and
  derived run-level mapping artifacts; no lower-layer trace was found.
- Confirmed that no sender CSV records a mobile private-5G condition or a
  synchronized clock state.
- Confirmed that the only stored private-5G packet captures are the two local
  QoS-verification captures.
- Reviewed the locally excluded 25-page signed structural analysis, including
  its antenna-placement sketch. It supports a 20 ft antenna centerline on a
  21 ft outdoor tripod/mount structure but does not establish a north-referenced
  azimuth or mechanical/electrical downtilt.
- Accessed and inventoried all 23 files under the connected `Airspan Radio`
  Drive folder. Reviewed all 10 current ACP configuration CSVs, all five
  Airspan manuals/release notes, the DU-cell counter export, the two SIM
  inventory workbooks, the eSIM provisioning document, and the four deployment
  and procurement PDFs. The review resolves the status-table headers and the
  current per-cell RF/CBRS values recorded above. Those 23 files alone did not
  contain the TDD labels or current SAS provider; the later configuration
  exports resolve those values. User confirmation resolves the frame-allocation
  mapping. The network administrator identifies `10D4G` as an unchanged
  LTE-coexistence packing setting, so it is recorded but excluded from the
  experimental comparison. Exact Cisco-core and internal-path fields are
  outside the current paper scope.
- Reviewed the two locally added ACP screenshots at original resolution. They
  resolve the configured gNodeB 5QI/RLC template values and control-interface
  DSCP/SCTP values. The screenshots alone do not resolve the active experiment
  flow's QFI/5QI, TDD pattern, SAS provider, antenna orientation, or ACP
  performance-counter semantics.
- Recorded the user's confirmation that the CPE is an `MG52-HW`, remained flat
  on the trunk floor with its front face upward for every original run, used
  DNN `cisco5g`, and was assigned default 5QI 9. Also recorded that the edge
  computer is directly connected to the MX250 and that the current firmware and
  Ethernet-link values match the original campaigns. Numeric QFI and any
  nondefault 5QI assignment remain unverified; internal delay decomposition is
  outside the current paper scope.
- Queried the active Airspan XML export at
  `/root/GnbConfigs/GnbDetailsGet/GnbProperties/NetworkConfig/RlcAcknowledgeModeSupport`
  and confirmed that it currently returns `Enabled`; the user independently
  confirms the live setting. This capability value does not override the
  `RLC-UMBidirectional` 5QI-profile settings or establish the mode used by an
  experiment bearer.
- Downloaded and XML-validated `All_Config.xml` and
  `Airspan_NetworkConfigExport_20260731_1234.xml`, and reviewed
  `CBRSConfiguration_20260731_1230.csv`. The files remain under the locally
  excluded `CISCO_AIRSPAN_STATS/` directory. The CBRS export resolves the
  current primary SAS provider; the network export resolves the current cell
  frame labels and confirms 15-minute statistics collection, GNSS clock
  source, and the 7-second UE inactivity timer. `All_Config.xml` is a generic
  profile library and was not used as active-deployment evidence.
- Checked Cisco's MG52 product documentation, technical specification, and
  internal-antenna documentation, then checked the FN990A40 hardware guide for
  n48 modem sensitivity. Vendor maximums and modem-level conducted sensitivity
  are kept distinct from the observed 1 Gbit/s full-duplex Ethernet link and
  from whole-gateway radio performance.
- Checked primary FCC/Airspan, 3GPP/ETSI, and Cisco documentation online. The
  public AirSpeed 2900 material confirms the product and certified frequency
  range but does not define the Airspan TDD shorthand. 3GPP resolves the TDD,
  active-UE, DRB-throughput, and RLC terminology; Cisco documentation confirms
  that 5QI-to-DSCP treatment is configured through SMF/DNN QoS policy, so the
  gNodeB profile screenshot alone is not flow-assignment evidence.
- Checked current official Meraki documentation for inventory, appliance
  status, firmware, VLANs, route table, clients, topology, event log, packet
  capture, and MG cellular-gateway views. Checked Cisco's private-5G
  architecture documentation to confirm that mobile-core and subscriber/QoS
  management belong to Cisco Control Center/Private 5G management rather than
  the MX appliance pages.
- The Drive folder contains eSIM activation material and procurement records
  with financial and personal information. None of those values were copied
  into the repository. Keep the folder access-controlled; if it was shared
  outside the intended team, review access and revoke or reissue any still-valid
  activation material.

## Next Steps

- Import and inspect the user's ACP DU-cell/configuration exports for
  V1 11:15-11:30 EDT, V2 13:15-13:30 EDT, and V3 13:35-13:50 EDT. Confirm the
  idle/traffic distinction, directional counters and byte scaling, Cell 2
  attribution, Cell 1 negative control, and `70/20/10` profile before marking
  the Airspan-counter portion of R1 complete. The Cell 2 lock is already
  operator-verified.
- Import matching 2026-08-06 ACP Cell 1/Cell 2 counters for the 09:05-12:15 EDT
  medium/typical block or the 14:30-15:30 EDT weak-location block only if the
  analysis will make a contemporaneous cell-load or TDD-effect claim. The
  application-layer results are usable without these exports.
- Import the matching 2026-08-06 ACP Cell 1/Cell 2 exports for the
  16:00-17:00 EDT second common/typical block if cell-load or TDD analysis is
  pursued. Preserve the iPhone-observed RSRP equality as separate-UE spatial
  context. The MG52 Cell 2 lock is operator-verified.
- No additional candidate strong-location or Android-phone run is required.
  Preserve the completed application results and identify the existing iPhone
  survey as separate-UE spatial context.
- Preserve matched ACP evidence before making cell-counter, cell-load, or
  TDD-effect claims; do not treat the iPhone measurements as MG52 telemetry.

## Subsequent Implementation Task: PSM and Cooperative J2735 Flow

Task requested 2026-07-13: add SAE J2735 Personal Safety Message support and
make both standard J2735 payloads and IPI cooperative CAV messages available
through the same send/receive interfaces.

Status: complete in the working tree.

- Added a typed PSM model with identity, position, motion, accuracy,
  acceleration, path history/prediction, and propulsion fields.
- Added PSM profile encode/decode, byte encode/decode, MessageFrame/API traits,
  private-5G probe support, and TCP/MQTT/UDP sender selection.
- Added typed `IPI-CooperativeService` packing/unpacking to the high-level
  J2735 API.
- Added opaque pre-encoded J2735 payload ingest/list/broadcast methods for full
  standard messages not modeled by the lightweight helper types.
- Added and repaired ROS 2 conversions so the optional `v2x_msg` bridge builds
  for BSM, PSM, MAP, SPaT, SRM, and SSM.

Validation:

- Normal C++ build passed.
- All 7 CTest tests passed, including the new PSM/cooperative flow test.
- Live TCP `--message psm` sender/receiver probe passed with an accepted PSM.
- Optional ROS 2 `v2x_msg` package and IPI ROS bridge builds passed using ROS 2
  Humble with the system Python interpreter.

## Active Task Update: Coauthor Email Action Plan

Task requested 2026-07-23: read the saved 35-message Cisco/UD coauthor email
chain and identify what William must do for each manuscript comment and proposed
private-5G follow-up experiment.

Status: email analysis and downloaded-material review complete. The QoS
document, sample G-NetTrack archive, and DU Cell export have been reviewed. No
paper source or experiment artifact was changed.

### Immediate User Actions

- Keep `CISCO_AIRSPAN_STATS/` in the local working tree as requested, but retain
  its local Git exclusion so vendor documents, internal addresses, device
  identifiers, and unredacted exports are never committed or uploaded.
- Run one ACP/G-NetTrack validation pilot before the follow-up experiments. Use
  one controlled traffic source, isolate other test UEs where possible, align
  the run with ACP's 15-minute reporting boundaries, and record the timezone
  used by every logger.
- Export ACP after every new experiment day, before the approximately two-day
  retention window expires.
- For the scheduled Cisco discussion, confirm numeric QFI only if the optional
  QoS comparison proceeds. The AirSpeed 2900, Category-B/GAA configuration,
  current channel and RF values, `MG52-HW` placement and firmware continuity,
  DNN `cisco5g`, and default 5QI 9 are now recorded. Exact Cisco-core inventory
  and path decomposition are not required for the current paper.
- Ask Cisco/UD administrators for the requirements and approval to install the
  Airspan ACP on a stable UD-managed VMware server.
- Tell Cisco that the desired second QoS profile is higher-priority,
  lower-latency treatment for compact CAV request/response traffic under
  contention, compared against the current default DNN/5QI 9 path. Ask Cisco to
  recommend a supported non-default 5QI and provide core-side policy, QFI/flow,
  and counter evidence.

### Download Review

- `Dynamic QoS Rules.pdf`, `G-NetTrack_Pro_Logs 1.21.zip`, and
  `DUCellExport_20260724_1002.csv` are stored under the local
  `CISCO_AIRSPAN_STATS/` directory. The directory is excluded through
  `.git/info/exclude` because the PDF is vendor-confidential and the exports
  contain internal identifiers. The files will remain in this directory as the
  user requested and must not be added to Git.
- The QoS document supports a default-session policy and an optional dedicated
  PCC flow. A dedicated flow may use GBR or non-GBR 5QI values and may match
  protocol, endpoint, port, and traffic direction. The document's camera
  example uses a dedicated 5QI 4 flow, but that example does not establish the
  correct profile for compact CAV request/response traffic.
- Dynamic QoS must not be enabled as an exploratory change. The document states
  that the feature cannot be disabled after activation and that a saved PCC rule
  cannot be removed. It also requires confirmation that the radio supports the
  requested GBR and has sufficient resources.
- The document lists default-session 5QI values 5--9 in one parameter
  description but gives 5QI 69 in its recommended example. Cisco must resolve
  this inconsistency and provide the exact supported DNN, 5QI, GBR/non-GBR,
  bit-rate, and flow-filter values for the installed software release.
- The G-NetTrack archive contains eight sample AT&T LTE runs from 2018, not
  measurements from the UD private-5G deployment. Its raw records are
  tab-delimited text files with 243 columns; the archive also includes
  per-metric KML files. Across 6,753 records, the samples contain GNSS, speed,
  network mode, signal level and quality, SNR, CQI, LTE RSSI, and bitrate fields.
- The sample runs have median timestamp intervals between 1.0 and 3.5 seconds.
  Their ping-latency and ping-loss fields are empty. The archive therefore
  demonstrates an export format but provides neither packet-level timing nor
  evidence that the current app exposes the needed 5G NR fields on the lab
  phone.
- Before a full campaign, run a short stationary pilot on the lab private-5G
  network and verify the current export fields, timestamp behavior, sampling
  interval, NR serving-cell values, and whether ping tests can run concurrently
  with the application workload. Drop device identifiers, subscriber
  identifiers, phone numbers, and local IP addresses before any derived data is
  stored in the repository.
- The DU Cell export contains 590 rows for two cells in uninterrupted
  15-minute bins from 2026-07-21 08:15 through 2026-07-24 10:00. Its 45 columns
  cover DRB volume, active-time-derived throughput, RACH attempts and outcomes,
  cell availability, and mean active-UE/preamble counters.
- Every row is identified only by cell and time bin. The export has no UE
  identifier, serving-cell RF measurements, CQI, MCS, BLER, HARQ/RLC
  retransmissions, PRB utilization, scheduler or queue state, QFI/5QI, or packet
  latency. It therefore provides coarse cell context rather than per-request or
  per-UE radio evidence.
- The export does not overlap the stored May and 2026-07-01--06 private-5G
  campaigns, so it cannot be used to explain those results. New application
  runs and ACP exports must be collected over the same time windows.
- `Mean Number of Active UEs` is zero in all 590 rows despite nonzero DRB
  traffic, and the two cells report a simultaneous nonzero fault-unavailability
  interval around 2026-07-23 20:45--21:15. Airspan must clarify the active-UE
  counter, the availability unit and event, the export timezone, and the
  mapping from cell 1/2 to the serving radio configuration before these fields
  are interpreted.

Validation:

- Extracted all six PDF pages as text and visually inspected rendered pages
  containing the configuration template and recommended values.
- Enumerated the archive and parsed all eight raw tab-delimited logs without
  extracting identifier-bearing records into the repository.
- Parsed every DU Cell row, checked interval continuity and duplicate keys,
  measured field coverage, and confirmed that both cells have 295 consecutive
  15-minute rows with no duplicate cell/time bins.
- Confirmed that `CISCO_AIRSPAN_STATS/` is absent from `git status` and locally
  excluded from Git. Confirmed that the mounted filesystem does not enforce the
  attempted owner-only permissions.

### Historical ACP Telemetry Availability

The following windows are derived from the stored application sender
timestamps. They are expressed in America/New_York local time (EDT), rounded to
15-minute ACP boundaries, with one context bin before and after each day's
traffic:

- 2026-05-13 10:45--17:45
- 2026-05-14 09:15--18:00
- 2026-05-15 08:45--18:30
- 2026-05-18 08:45--18:15
- 2026-05-21 13:15--18:00
- 2026-05-22 13:30--17:30
- 2026-07-01 10:30--17:30
- 2026-07-02 08:45--16:15
- 2026-07-03 09:45--10:45
- 2026-07-06 08:00--16:15

Confirm ACP's timezone before matching rows. If ACP only accepts whole dates,
export the complete days May 13, 14, 15, 18, 21, and 22 and July 1, 2, 3, and
6. These historical exports can add coarse cell context only; they cannot
provide per-request or per-UE radio measurements that were not originally
recorded.

ACP currently exposes only recent data, so these historical windows can no
longer be exported. The July 21--24 CSV must not be relabeled or joined as
though it were measured during the May 13--July 6 experiments. Exclusive use of
the radio improves attribution within the CSV's actual collection window, but
it does not establish that radio load, faults, configuration, or counter values
were the same during the earlier experiments.

The current CSV may be used only as a separately dated operational snapshot of
the same deployment, subject to resolving its counter semantics. It includes a
cell-unavailability interval and reports zero mean active UEs despite nonzero
DRB traffic, so it is not yet a validated idle baseline. Historical application
results remain valid at their measured layer, but no historical ACP/RAN
telemetry is available for causal interpretation.

### Required Validation Pilot

1. Confirm the ACP export timezone, the unit of cell-unavailability time, the
   mapping from cell 1/2 to the physical radio configuration, and why the mean
   active-UE counter remains zero when DRB traffic is nonzero.
2. Ask Airspan whether ACP can export shorter intervals or per-UE records and
   which report, if any, provides RSRP/RSRQ/SINR, CQI, MCS, BLER,
   retransmission, PRB-utilization, and QoS-flow fields. Do not describe these
   fields as available until an actual export is obtained.
3. Use three clean consecutive ACP bins with other test UEs removed where
   possible: one idle bin, one controlled uplink-traffic bin, and one controlled
   downlink-traffic bin. Keep traffic inside the selected 15-minute bin, record
   exact start/end times and rates, and export ACP immediately afterward.
4. Run G-NetTrack on a private-5G-capable Android phone beside the gateway for
   the full pilot. Record the phone, experiment-host, and ACP timezone/clock
   offsets, and verify that the current app export contains 5G NR measurements
   rather than assuming the 2018 LTE sample schema applies.
5. Accept the pilot only if the traffic appears in the expected cell and time
   bins, measured DRB volume/throughput tracks the known offered traffic, the
   current G-NetTrack fields and cadence are documented, and unexplained
   counters are resolved or excluded from later analysis.

### Targeted Telemetry Recollection

Do not repeat the full private-5G campaign. Recollect a predeclared subset that
reproduces the representative points needed to relate application behavior to
contemporaneous ACP and G-NetTrack measurements:

1. Stronger-signal, idle network, 1 KiB application payload.
2. Stronger-signal, idle network, 23,968 B detector-output payload.
3. Stronger-signal, 1 KiB payload under the existing 25 Mbps uplink-load
   condition.
4. Stronger-signal, 1 KiB payload with the existing 100-client condition.
5. Weaker-signal, idle network, 1 KiB application payload.
6. Weaker-signal, idle network, 23,968 B detector-output payload.

Use the existing TCP, MQTT, and UDP parameters and working collection scripts.
Where execution time permits, run the three transports sequentially within the
same 15-minute ACP bin so they share the same cell context, while preserving
separate application logs and exact transport start/end times. Use measured
signal strength to label the two signal conditions; placement alone is not the
independent variable.

One bin per condition is sufficient only for a diagnostic check. Collect at
least two bins per condition for a synchronized follow-up dataset; three is
preferred for paper comparisons. With two bins, the six-condition matrix
requires three hours, plus the 45-minute validation pilot. Export ACP and
G-NetTrack at the end of each collection day.

Run the default-versus-new QoS comparison separately, only after Cisco approves
and verifies the second DNN/5QI configuration. Do not repeat the complete
payload sweep, every client count, every load level, failure/restart runs, or
V2X experiments solely to obtain ACP context.

### Experiment Actions

1. Per-request radio context: use a private-5G-capable Android phone with
   G-NetTrack logging beside the gateway and synchronize its clock with the
   experiment host. Treat phone measurements as a co-located proxy, not gateway
   telemetry. Ask whether ACP can export per-UE gateway statistics at finer
   granularity.
2. Controlled signal/CPE comparison: do not change gNodeB power without written
   UD authorization. Run an adjacent outside-versus-inside vehicle comparison
   at one fixed location with unchanged orientation, workload, and radio
   configuration; repeat each condition and log radio context.
3. Optional path decomposition: this is outside the current paper scope. If it
   is reopened later, measure CPE-to-edge application RTT and
   Airspan-to-MX250-connected-laptop ping RTT separately. Do not claim that the
   difference isolates over-the-air delay unless the paths and interfaces are
   shown to be comparable.
4. Verified QoS: after Cisco creates the second DNN/QoS profile, run a targeted
   default-versus-new-profile comparison under idle and controlled uplink load.
   Preserve core-side proof of the actual 5QI flow; IP TOS alone is insufficient.
5. RAN statistics: use ACP Radio DU Cell exports during targeted load,
   multiclient, and QoS comparisons. Verify export interval and per-UE support;
   align or lengthen experiment windows if ACP reports 15-minute aggregates.
   Scheduler statistics and queue occupancy are unavailable from ACP and must
   not be promised.

### Manuscript Actions After User Inputs

- Replace `Cisco 5G` and `Cisco radio/core` with vendor-accurate private-5G
  terminology and add the verified configuration details once supplied.
- Describe the existing weak-signal results without calling them a commercial
  cell edge unless Cisco supplies a deployment-specific coverage definition.
- Clarify that LTE C-V2X PC5 and the private-5G NR Uu path are independent; no
  new V2X experiment is required for this clarification.
- Keep IPI as an enabling contribution while retaining the readiness question
  and three measurement-derived insights as the paper's central logic.
- Scope private-5G conclusions to the measured deployment and add causal
  interpretation only where new radio or RAN evidence supports it.
- Add peer-reviewed DRIVE-SAFE/BREAKING-LOW work when available; the news item
  alone is motivation, not technical evidence.
- Allow coauthors to use review macros temporarily, then remove all strikeout,
  comments, credentials, internal addresses, and review annotations before
  submission.

## Active Task Update: Remaining Experiment Runbook

Task requested 2026-07-24: create a detailed root-level reference document for
every experiment and supporting action that remains.

Status: complete.

Files updated:

- `remaining_exp.md`
- `AGENTS.md`
- `agent_context.md`
- `current_task.md`

The new runbook:

- preserves the distinction between existing application results and
  unavailable historical Airspan telemetry;
- defines the Airspan/G-NetTrack validation pilot;
- reduces the synchronized follow-up to six representative private-5G
  conditions rather than repeating the full campaign;
- specifies common timing, physical controls, application parameters, artifacts,
  redaction, repetitions, acceptance criteria, and end-of-day export steps;
- defines the blocked Cisco-verified QoS and Airspan path-measurement work;
- lists experiments that do not need repetition; and
- provides a completion checklist for future updates.

Validation:

- Checked the required six-condition matrix against the existing compact,
  detector-output, load, multiclient, and weak-signal result families.
- Preserved TCP, MQTT, and UDP as separate application measurements.
- Preserved the approximately two-day Airspan retention constraint and
  15-minute aggregation boundary.
- Added `remaining_exp.md` to the approved root Markdown list and required read
  path so future repository cleanup does not remove it.

Next step:

- Complete R0 and R1 in `remaining_exp.md` before collecting the six
  paper-facing follow-up conditions.

### Execution Host Clarification

Clarification received 2026-07-24:

- `d1` is not connected through the Cisco private-5G gateway.
- Do not run the private-5G application sender, load client, or R1-R5
  experiments on `d1`.
- Run those experiments on the computer in the car behind the Cisco
  private-5G gateway.
- Use `d1` only as a wired receiver or path-comparison endpoint if its physical
  connection to the MX250/Airspan-side network is confirmed.
- A separate authenticated SSH or reverse-SSH connection to the car's
  experiment computer is required before Codex can execute the collection.

## Active Task Update: Review-Comment Closure And Android Logger

Task requested 2026-07-24: re-audit `remaining_exp.md` against the saved
coauthor email chain and specify what must be collected with the user's old
Android phone and installed NetTrack-Lite/G-NetTrack Lite app.

Status: complete.

Files updated:

- `remaining_exp.md`
- `current_task.md`

The runbook now adds:

- a coauthor-comment closure matrix;
- R1A qualification for the exact phone, Android build, private-5G attachment,
  app package/version, fields, sampling behavior, and raw export;
- a precise phone field list and a rule that Lite cannot be used for
  paper-facing collection unless it exports timestamped private-5G NR records;
- explicit separation between co-located phone radio context and gateway,
  per-request, or RAN telemetry;
- deployment/topology documentation, IPI implementation validation,
  claim/citation/submission closure, and ACP availability work;
- host and network-stack telemetry for the synchronized follow-up;
- an R8 directionality check after source review confirmed that current large
  payloads are sent in the vehicle-to-edge request while the returned
  acknowledgment does not carry an equally large response body; and
- explicit alternatives for blocked or unnecessary work, including no gNodeB
  power changes, no full campaign rerun in another vehicle, no unsupported
  over-the-air delay subtraction, and no phone speed test during application
  measurements.

Current evidence relevant to the phone:

- The official G-NetTrack Lite listing says 5G reporting can include RSRP,
  RSRQ, and PCI, while SNR, CQI, and timing advance are listed only for 4G.
- The same listing places text/KML recording and active data tests under Pro
  features while also mentioning Lite log mode, so an actual Lite export must
  be inspected.
- `CISCO_AIRSPAN_STATS/G-NetTrack_Pro_Logs 1.21.zip` is an old 2018 AT&T LTE
  schema example. It does not verify current 5G NR fields or the user's phone.
- A Google Pixel 10 is now visible through local `adb`. Network Survey 1.57 is
  installed from the official arm64 GitHub release, and its cellular-only LTE
  CSV precheck has been inspected. Private-network attachment and NR field
  population remain unverified.

Next step:

Superseded on 2026-08-12: R1A and the Android logger are not required for the
current paper or completed follow-up. No further Pixel qualification is
scheduled.

## Active Task Update: Pixel 10 Network Survey Qualification

Task requested 2026-08-10: find and validate a free G-NetTrack Pro alternative
on the connected Pixel 10.

Status: closed as not required. The logger and LTE export path were validated,
but no private-5G NR qualification is needed for the current paper.

Completed:

- Installed the official Network Survey 1.57 arm64 release and verified its
  release checksum and application signing certificate.
- Configured a one-second cellular scan interval, CSV output, automatic phone
  state logging, and battery-optimization exemption.
- Disabled CDR, Wi-Fi, Bluetooth, MQTT, and community-upload logging. Revoked
  SMS, call-log, and phone-number runtime permissions.
- Collected a 28.205-second public-LTE precheck with 29 usable rows and a
  1.008-second median interval. LTE location, cell identity, channel, RSRP,
  RSRQ, SNR, timing advance, bandwidth, and connection-status fields were
  populated.
- Kept the raw export outside the repository because it contains exact GNSS,
  serving-cell, and device values.

Remaining: none. Retain the installed logger only as a contingency for future
work that explicitly requires a new Android-based survey.

## Active Task Update: Follow-Up Results Table

Task requested 2026-08-10: summarize the pulled Airspan follow-up results in a
table, integrate the table into the MobiCom manuscript, and rebuild the PDF.

Status: complete.

Files updated:

- `paper/sections/03_Testbed.tex`
- `paper/sections/04_methods.tex`
- `paper/sections/05_experiment_results.tex`
- `paper/sections/06_challenges.tex`
- `paper/sections/07_conclusion.tex`
- `paper/main.pdf`
- `current_task.md`

The manuscript now includes Table 4 with p50/p95 sender-side RTT and response
availability for TCP, MQTT, and UDP across C1--C4. The table includes the
common/typical, candidate-weak, and candidate-strong C1--C2 blocks, the C3
offered-uplink-load condition, and the C4 100-client condition. The surrounding
text reports the validated sample and attempt totals, C3 timeouts, C4 response
counts and 100 ms hit rates, C2 location-block comparison, and R1 host-side
direction-control throughput.

Evidence boundary:

- Application RTT, response availability, attempt counts, and host throughput
  come from validated sender and control artifacts.
- Location classes use field observations and separate-UE iPhone signal
  context. The MG52 Cell 2 lock is operator-verified. The `70/20/10` profile
  remains an operator record pending aligned Airspan evidence for causal TDD
  analysis.
- The paper therefore does not claim a causal TDD, RSRP, or radio-utilization
  effect from this follow-up.

Validation:

- Rebuilt `paper/main.pdf` with BibTeX and two final `pdflatex` passes because
  `latexmk` is not installed.
- Final PDF is 14 pages; main content ends on page 12 and references begin on
  page 13.
- Rendered and inspected pages 9 and 12. Table 4 is legible, aligned, and not
  clipped; page 12 ends cleanly with the conclusion.
- `main.log` contains no undefined citations/references or overfull boxes.
- PDF annotation scan found zero annotations, so the PDF contains no embedded
  hyperlinks.

## Active Task Update: Paper Outline Preparation

Task requested 2026-08-11: read the current repository context, experiment
evidence, and manuscript draft to establish the paper's intended argument, then
prepare `paper/paper_outline.md` for a later user-guided outlining session.

Status: complete. Read the required repository context, the current experiment
summary and paper-facing evidence boundaries, the IPI API and representative
tests, all current manuscript section sources, the standalone appendix, the
bibliography inventory, and the current compiled paper text. The paper's
readiness question, stateless/stateful distinction, contribution hierarchy,
three user-defined insights, and current claim boundaries are now understood.

Files updated:

- `paper/paper_outline.md` (created as an intentionally empty, zero-byte file)
- `current_task.md`

Validation:

- Confirmed `paper/paper_outline.md` is exactly 0 bytes.
- Confirmed no manuscript source, figure, bibliography, or PDF was changed.
- No build or test run was needed because this task changed no executable or
  manuscript content.

## Active Task Update: Introduction Outline

Task requested 2026-08-11: write only the introduction outline in
`paper/paper_outline.md`. The outline must motivate why connected and automated
vehicles need V2X and cellular communication, question whether today's deployed
technologies can carry the protocols, data volumes, and time-critical exchanges
that CAV applications require, introduce IPI and the real-vehicle evaluation,
cover every stored experiment dimension, and end with exactly three insights
useful to the CAV, CV, ITS, cellular, carrier, radio, and 6G communities.

Status: complete. `paper/paper_outline.md` now contains only the introduction
outline. It preserves the user's argument order: why CAVs need communication;
the mismatch between compact stateless CV updates and stateful/data-intensive
CAV exchanges; the readiness question; IPI; the real-vehicle V2X/private-5G
evaluation; contributions; and exactly three cross-community insights.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Checked the outline against all experiment families summarized in
  `experiment_summary.md`: E01-E08, M01-M04, and V01-V05.
- Confirmed that the experiment checklist covers deployment, protocol/transport,
  payload, direction, radio condition, distance/geography, obstruction,
  mobility, repetition, timeouts and application budgets, uplink load, QoS
  evidence, concurrency, failure injection, RAN context, and workload validity.
- Confirmed exactly three numbered top-level insights under the introduction's
  insights heading.
- Confirmed no trailing whitespace. The final outline is 465 lines, 3,781
  words, and 28,186 bytes with SHA-256
  `3b7f46857cc99c6cd8139e1bfbd01a253e51cb274f5a187897f476cf8266421a`.
- No manuscript source, figure, bibliography, PDF, executable, or result
  artifact was changed, so no paper build or code test was needed.

## Active Task Update: Introduction Contribution Structure

Task requested 2026-08-11: correct Paragraph 7 of the introduction outline so
that the paper's contributions are distinct from its three experimental
insights.

Status: complete. Paragraph 7 now identifies two contributions: the IPI
protocol and implementation, and the application-driven real-vehicle field
evaluation. The former third contribution, which overlapped the evaluation and
insights, was removed and replaced with a logical transition into the unchanged
three-insight paragraph.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed Paragraph 7 contains two numbered contributions.
- Confirmed Paragraph 8 still contains exactly three numbered top-level
  insights.
- Confirmed the removed `application-grounded readiness map` label no longer
  appears in the outline.
- No manuscript source, code, figure, bibliography, PDF, or result artifact was
  changed, so no build or executable test was needed.

## Active Task Update: Motivation and Related Work Outline

Task requested 2026-08-11: widen the literature search beyond the existing
draft, including Tentacles and the NSF BREAKING LOW/DRIVE-SAFE effort, and add
a short Motivation outline plus a complete Related Work outline to
`paper/paper_outline.md`.

Status: complete. The new Motivation outline defines application support as
complete response or object availability within the application's time budget,
then connects that criterion to IPI and the field evaluation in three compact
paragraphs. The Related Work outline positions both contributions against
standards and platforms, deadline-aware and multi-network middleware,
networked CAV applications, direct C-V2X PC5 studies, real 5G/MEC automotive
studies, and hybrid/application-network co-design.

The expanded search changed the novelty boundary. Tentacles already provides
a unified network abstraction, application QoS declarations, multi-interface
selection, representation adaptation, and local fallback. IPI is therefore
positioned more precisely as a domain-level application contract that combines
J2735-compatible ITS/CV messages with transport-independent stateful CAV
service semantics. The UMN commercial-5G teleoperation study is treated as a
close technical baseline for the field-evaluation contribution; the BREAKING
LOW and DRIVE-SAFE descriptions are used only as motivation and evidence of an
ongoing research direction, not as completed measurement evidence.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the Motivation outline specifies three short paragraphs and an
  optional compact table, with explicit limits preventing it from becoming a
  second Introduction.
- Confirmed Related Work covers the complete paper rather than only IPI and
  includes dedicated positioning for Tentacles, commercial-5G teleoperation,
  application-centric PC5 studies, private-5G C-ITS trials, and multi-radio
  designs.
- Confirmed the outline separates peer-reviewed/technical measurement evidence
  from project announcements and ongoing-program descriptions.
- Confirmed the paper's two contributions and exactly three top-level insights
  remain unchanged.
- Confirmed no trailing whitespace or Markdown heading errors in the added
  sections.
- No manuscript source, figure, bibliography, PDF, executable, or result
  artifact was changed, so no paper build or code test was needed.

## Active Task Update: Gap-Driven Related Work Revision

Task requested 2026-08-11: reduce Motivation to one short prose paragraph and
rebuild Related Work so that it provides the necessary information, identifies
the remaining gaps across prior work, and states directly how this paper fills
those gaps.

Status: complete. The earlier literature-inventory and claim-audit structure
has been replaced. Motivation is now one prose paragraph. Related Work now
organizes the literature into standards/platforms, CAV applications and
middleware, direct C-V2X evaluation, cellular/5G evaluation, and multi-radio
co-design. Every group explicitly moves from prior accomplishments to a
collective remaining gap and then to the part of this paper that fills it.

The section now centers two connected gaps:

1. an interface gap between conventional message-oriented ITS/CV communication
   and general stateful CAV service exchanges; and
2. an evidence gap between application- or technology-specific studies and a
   systematic account of complete application exchanges across the measured
   direct-V2X and private-5G paths.

IPI fills the interface gap. The application-level real-vehicle evaluation
fills the evidence gap. The three insights translate the combined findings
into requirements for future CAV, ITS, carrier, radio, and 5G-Advanced/6G
systems.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed Motivation contains exactly one nonempty prose paragraph between
  its section heading and Related Work.
- Confirmed all five literature groups contain explicit prior-work, remaining-
  gap, and paper-fill stages, followed by one synthesis subsection.
- Confirmed Tentacles is positioned as a complementary network/QoS abstraction
  rather than ignored or used to support an overbroad IPI novelty claim.
- Confirmed the field-evaluation gap is stated without claiming the first real
  PC5, 5G, private-network, or application-centric vehicular study.
- Confirmed the Introduction still contains two contributions and exactly
  three top-level insights.
- Confirmed no trailing whitespace.
- No manuscript source, figure, bibliography, PDF, executable, or result
  artifact was changed, so no paper build or code test was needed.

## Active Task Update: Related Work Presentation Clarification

Task requested 2026-08-11: clarify that the conversational comparison table
and the outline's prior-work/gap/fill labels are not manuscript-ready wording,
and identify a more suitable academic presentation than a dense table.

Status: complete. No comparison table was added to the paper outline. The
outline now states explicitly that the prior-work, remaining-gap, and paper-
fill labels are drafting scaffolds that must become connected academic prose.
It recommends prose as the primary Related Work presentation and records an
optional two-panel literature landscape if the final figure budget permits:
one panel for the interface gap and one for the empirical-evidence gap.

The proposed visual keeps IPI and the field evaluation separate, uses only
representative literature clusters, and avoids presenting Edge4AV as a single
top-right point that dominates unlike prior contributions. The figure remains
optional because a system or results figure may be a better use of MobiCom
page space.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed Related Work contains no Markdown comparison table.
- Confirmed the outline explicitly marks its gap labels as non-manuscript
  scaffolding.
- Confirmed the optional visual has two separately defined panels and axes.
- Confirmed Motivation remains one prose paragraph and the Introduction retains
  exactly three top-level insights.
- Confirmed no trailing whitespace.
- No manuscript source, figure, bibliography, PDF, executable, or result
  artifact was changed, so no paper build or code test was needed.

## Active Task Update: Current-to-Future Capability-Gap Figure

Task requested 2026-08-11: replace the confusing gap-bridge/two-panel figure
concept with a left-to-right view of existing technologies and research, the
remaining gap, and the communication capabilities required by future CAV/ITS
applications.

Status: complete. The outline now recommends a three-region capability-gap
diagram rather than a table, quadrant, or decorative bridge. The left region
groups today's standards/messages, platforms/applications/middleware, and
deployed PC5/5G evidence. The middle identifies the interface and empirical-
evidence gaps and maps them to IPI and the field evaluation. The right defines
application-ready communication requirements for future CAV/ITS systems.

The right side is explicitly a requirements target, not a claim that this
paper implements a complete future communication system. The outline also
avoids the informal label `dream CAV/ITS`, paper-by-paper checklists, quality
scores, and a visual construction that places Edge4AV as a universal winner.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the previous two-panel landscape recommendation has been removed.
- Confirmed the new figure concept has explicit left, middle, and right roles.
- Confirmed the middle retains the paper's two gaps and maps them to the two
  contributions without adding another contribution.
- Confirmed Motivation remains one prose paragraph and the Introduction retains
  exactly three top-level insights.
- Confirmed Related Work still contains no Markdown comparison table and no
  trailing whitespace.
- No manuscript source, rendered figure, bibliography, PDF, executable, or
  result artifact was changed, so no paper build or code test was needed.

## Active Task Update: Section 2 and Section 3 Reordering

Task requested 2026-08-11: make Related Work Section 2 and Motivation Section
3, while preserving the approved content and argument.

Status: complete. `paper/paper_outline.md` now places Related Work immediately
after the Introduction as Section 2, with its six subsections renumbered
2.1--2.6. The one-paragraph Motivation section follows as Section 3. The
positioning-figure note now refers to the Related Work/Motivation transition.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the top-level order is Introduction, Related Work, then Motivation.
- Confirmed Related Work contains exactly subsections 2.1--2.6 and no remaining
  3.x subsection headings.
- Confirmed Motivation remains exactly one prose paragraph.
- Confirmed the Introduction retains exactly two contributions and three
  top-level insights.
- Confirmed Related Work contains no Markdown comparison table and no trailing
  whitespace.
- No manuscript source, rendered figure, bibliography, PDF, executable, or
  result artifact was changed, so no paper build or code test was needed.

## Active Task Update: System Design and Experimental Setup Outline

Task requested 2026-08-11: add the System Design and Experimental Setup
outline. The section must describe the deployment deeply enough for radio and
signal reviewers, remain useful to ITS/CV/CAV readers, explain why the paper
tests LTE C-V2X PC5 and 5G NR Uu, enumerate the experiments and their readiness
questions, and avoid organizing the evaluation around the three later
insights.

Status: complete. Section 4 now opens by carrying the communication landscape
from Related Work into the selection of the two paths. It uses 3GPP TS 23.287
and the 5GAA architecture description to position PC5 and Uu as complementary
C-V2X modes, then explains why the measured off-the-shelf implementations are
appropriate current examples without calling them the only or universally
most deployed vehicular technologies.

The system outline now records:

- independent PC5 and Uu end-to-end paths and their application-RTT boundaries;
- the real autonomous vehicle, OBU, MG52-HW, GNSS, gNodeB, Cisco core/routing
  boundary, MX250-connected edge host, and exclusion of Internet/cloud hops;
- PC5 standard-message validation, the correlated packet-data probe, the
  4,080 B installed-SDK limit, and the lack of usable C-V2X received-power
  telemetry;
- the AirSpeed 2900 hardware/software, n48/40 MHz cell records, NR-ARFCNs,
  power/EIRP, CBRS/SAS state, antenna configuration and structural record,
  TDD shorthand, cell-use boundary, MG52 placement/firmware/link, DNN/default
  5QI, isolation, and unavailable bearer/per-request telemetry;
- the 16-point 5G RSRP/RSRQ/SNR survey and the separately defined normalized
  PC5 application-level path-availability proxy;
- compact J2735/IPI traffic, detector-derived output sizes, image/point-cloud/
  map size distributions, fixed transfer chunks, TCP, MQTT-over-TCP, raw UDP,
  application-fragmented UDP, and supporting workload-validation checks;
- seven experiment families derived from application-readiness conditions:
  path validation, payload scale, radio condition/mobility, direction/load/RAN
  context, concurrency, protocol/complete-message formation, and
  deadline/QoS/service interruption; and
- response availability, RTT, deadline, throughput, spatial-context,
  repetition, exclusion, and causal-claim rules.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the top-level order remains Introduction, Related Work,
  Motivation, then System Design and Experimental Setup.
- Confirmed Section 4 contains subsections 4.1--4.9 in order.
- Confirmed Motivation remains exactly one prose paragraph.
- Confirmed Section 4 does not contain numbered Insight 1/2/3 claims and
  explicitly reserves performance findings and insight derivation for later.
- Cross-checked the experiment families against E01--E08, M01--M04, and the
  workload-validation evidence V01--V05 in `experiment_summary.md`.
- Cross-checked the RF, CPE, core-boundary, TDD, QoS, clock, and telemetry
  statements against `current_task.md` and `remaining_exp.md`.
- Confirmed no trailing whitespace. The outline is 1,777 lines, 13,784 words,
  and 102,275 bytes with SHA-256
  `b7a3e9afec6e5468bb19d6ddb0ac6328b1fd6543be8391adeeab1f8585f565aa`.
- No manuscript source, rendered figure, bibliography, PDF, executable, or
  result artifact was changed, so no paper build or code test was needed.

## Active Task Update: Remove Redundant Motivation Section

Task requested 2026-08-11: remove the standalone Motivation section because it
repeats the readiness question already established by the Introduction and the
gap already synthesized by Related Work.

Status: complete. The Motivation section and its paragraph were removed.
System Design and Experimental Setup was renumbered from Section 4 to Section
3, including subsections 3.1--3.9 and nested subsections 3.5.1--3.5.3. The
section's opening argument now makes the direct chain explicit: the
Introduction poses the complete-application readiness question, Related Work
establishes PC5 and Uu as complementary communication modes, and Section 3
selects current off-the-shelf implementations and defines the experiment
matrix used to answer that question. The Related Work figure-placement note
now refers directly to the Related Work/System Design transition.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the top-level order is Introduction, Related Work, then System
  Design and Experimental Setup, with no standalone Motivation section.
- Confirmed the system section contains subsections 3.1--3.9 in order and
  nested subsections 3.5.1--3.5.3.
- Confirmed no old Section 4 headings or references to a Related
  Work/Motivation transition remain.
- Confirmed the readiness question remains in the Section 3 opening logic and
  the detailed technical setup and experiment matrix remain intact.
- Confirmed no trailing whitespace. The outline is 1,763 lines, 13,668 words,
  and 101,432 bytes with SHA-256
  `53414ae8eea78c9ec5c30b9da4fe0a15f0ae94e3c59178a66fc0839c57f7be49`.
- No manuscript source, rendered figure, bibliography, PDF, executable, or
  result artifact was changed, so no paper build or code test was needed.

## Active Task Update: Dedicated IPI Design and Corrected 5G Scope

Task requested 2026-08-11: add a detailed standalone IPI protocol-design
section; distinguish the MG52, local MX250, and remote NMS-side MX68CW; explain
that the multiclient and offered-load experiments are controlled attempts to
test load sensitivity; account for the fact that a weak-radio client can impose
a different resource cost than a strong-radio client; and state that
`30/60/10` was unsupported.

Status: complete. `paper/paper_outline.md` now places IPI Protocol Design in
Section 3 and returns System Design and Experimental Setup to Section 4. The IPI
section is grounded in the current C++ implementation and covers design goals,
two application interaction modes, common envelope fields, J2735 preservation,
typed service requests, cooperative planning/perception/control messages,
service and session state, correlation, topics, transport binding, freshness,
failure/fallback semantics, implementation layers, validation, and explicit
non-goals.

The private-5G description now records:

- the MG52 as the vehicle 5G SA/NSA cellular-to-Ethernet gateway/CPE;
- the MX250 as the on-site security/SD-WAN routing appliance and attachment
  point for the directly connected application edge host;
- the MX68CW as one separate remote NMS-side security/SD-WAN appliance with
  integrated LTE/Wi-Fi capabilities, not the 5G packet core;
- the private-5G core/edge functions as a separate component class whose exact
  deployment inventory and internal user-plane path remain evidence-bounded;
- offered uplink traffic and 1--100 logical clients as controlled proxies for
  competing traffic and aggregate application demand through one host and one
  MG52/UE, not a 100-UE public-network emulation;
- the stronger/weak-radio multiclient sweeps as an application-level test of
  signal/concurrency interaction, motivated by NR's varying spectral
  efficiency but without an unsupported PRB, MCS, BLER, or scheduler claim; and
- `30/60/10` as an unsupported usable allocation in this deployment, with no
  valid measurement and no status as a third evaluated TDD condition.

Official sources checked:

- Cisco Meraki MG52 technical specifications and installation documentation;
- Cisco Meraki MX250 and MX67/MX68/MX68CW datasheets;
- Cisco's private-5G architecture/security description; and
- 3GPP/ETSI TS 38.214 for the modulation/coding and spectral-efficiency basis
  of the radio-condition/resource-cost explanation.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed top-level order: Introduction, Related Work, IPI Protocol Design,
  and System Design and Experimental Setup.
- Confirmed IPI contains subsections 3.1--3.8 and System Design contains
  subsections 4.1--4.9 plus 4.5.1--4.5.3.
- Confirmed no stale System Design 3.x headings or old unresolved NMS/MX68CW
  wording remain.
- Rebuilt the C++ targets and ran all seven CTest tests; all passed.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
  The outline is 2,299 lines, 17,524 words, and 130,622 bytes with SHA-256
  `537c2d475362a08a1f5388184fe00ca9416abf06cdf87fbc677d366935aded15`.
- No manuscript source, rendered figure, bibliography, PDF, or raw result
  artifact was changed.

## Active Task Update: Application Thresholds and Experiment Results Outline

Task requested 2026-08-12: make the end of System Design state the conditions
required to reach future CV/CAV application support; derive thresholds from
concrete CV and CAV use cases; add an Experiment Results section in which every
paper-relevant result is presented as a comparison with an application or
analytical threshold; and end with one results summary/discussion subsection
that derives exactly the three approved insights.

Status: complete. Section 4.7 now defines semantic correctness, complete-object
delivery, deadline-qualified availability, workload capacity, operating-
envelope continuity, and failure behavior as joint readiness conditions. It
then works through five concrete examples:

- a 25 mph traffic-signal violation/red-light warning, including FHWA stopping
  distance, distance traveled per 100 ms, approximate message load, and the
  SPaT/state-mirror experiment mapping;
- 25 mph forward-collision and illustrative 25/35 mph blind-spot/lane-change
  warning geometries, including gap change, compact-message load, 10 Hz/100 ms
  VSC targets, and compact PC5/5G mappings;
- 5GAA automated intersection crossing, including 300--450 B awareness,
  approximately 100 B SPaT, 1,000 B MAP, and approximately 400 B planned-
  velocity/trajectory content, with a 10 ms/99.9999% comparison mapped to the
  compact J2735 and IPI guided-planning experiments;
- 3GPP cooperative emergency maneuver/trajectory alignment, including the
  2,000 B/3 ms/99.999%, 2,000 B/100 message/s/10 ms/99.99%, and 12,000 B/
  10 ms/99.99% cases mapped to the 2 KiB PC5 and 8--16 KiB 5G experiments; and
- fault-triggered remote recovery in which the CAV first slows or stops
  locally, then requests authenticated high-level assistance from the
  transformative-intersection edge or a help center to move to a safer
  roadside location or nearby parking area. This uses a less-than-10-km/h
  recovery speed, 100/200 ms comparisons, a 99.999% assistant-to-vehicle
  target, compact IPI fault/path objects, asymmetric direction controls, and
  the service-interruption experiment.

The validated 23,968 B detector p99 object and 60 kB stress object remain in
the payload/protocol analysis as experimental workloads. They are not presented
as one of the primary CAV application examples.

The outline explicitly distinguishes physics-derived motion/distance from a
cited communication deadline, one-way warning age from measured complete
request/reply RTT, and sequential probes from a sustained 10 Hz object stream.
The generic 100/500/1,000 ms analysis remains for cross-workload comparison,
and 200 ms is added for the high-level remote-recovery path. None is presented
as one universal CAV standard.

Section 5 now contains:

- IPI functional and compact-state results;
- direct-PC5 stationary payload/radio and moving-route comparisons;
- 5G payload/deadline and repeated-campaign comparisons;
- TCP, MQTT, raw-UDP, and fragmented-UDP complete-object comparisons;
- direction, offered-uplink-load, and verified/unverified QoS evidence;
- logical-client concurrency under good and weak field conditions plus the
  multi-location follow-up;
- receiver/broker interruption and recovery gaps; and
- one final summary/discussion subsection with the overall readiness answer and
  exactly three insight subsections.

Official sources checked:

- NHTSA Vehicle Safety Communications Task 3 and V2V technology-readiness
  reports for traffic-signal, forward-collision, blind-spot, and lane-change
  warning message frequency, latency, range, and approximate payload targets;
- FHWA stopping-sight-distance assumptions for the 25 mph worked example;
- 3GPP/ETSI TS 22.186 Release 19 for cooperative collision avoidance,
  emergency trajectory alignment, and higher-degree lane change;
- ETSI TS 103 324 V2.1.1 for collective-perception generation interval and
  resource-aware content/frequency management used only to interpret the
  detector-derived workload;
- 5GAA C-V2X Use Cases and Service Level Requirements Volume II for automated
  intersection crossing, infrastructure-based tele-operated recovery after a
  critical subsystem failure, and high-level remote-driving instructions;
- NHTSA ADS 2.0 voluntary guidance and the NHTSA ADS testable-cases framework
  for the independent minimal-risk response, remote-operator assistance, and
  movement out of an active roadway before or during network-assisted
  recovery; and
- *A Vision for Transformative Intersections* for the software-defined
  intersection edge and infrastructure-offloaded path-planning context.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed top-level paper order is Introduction, Related Work, IPI Protocol
  Design, System Design and Experimental Setup, and Experiment Results.
- Confirmed Section 4 retains subsections 4.1--4.9 and Section 5 contains
  subsections 5.1--5.9.
- Confirmed Section 5.9 contains exactly three numbered insight subsections and
  uses the approved top-level insight statements.
- Confirmed the application calculations: 25 mph = 11.176 m/s; FHWA-assumption
  stopping distance = 46.308 m; distance per 100 ms = 1.118 m; 25/35 mph gap
  change per 100 ms = 0.447 m; distance at 25 mph is 0.034 m per 3 ms and
  0.112 m per 10 ms; and the upper-bound distance at 10 km/h is 0.278 m per
  100 ms and 0.556 m per 200 ms.
- Confirmed the recovery example uses a sourced less-than-10-km/h assisted
  speed after a local slow/stop action. It does not use the 250 km/h network-
  capability ceiling and is not framed as destination parking.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
  The outline is 3,495 lines, 27,385 words, and 197,855 bytes with SHA-256
  `0c248d0e1afe9e852891b4ecb7cab6b2aa4609415c24c4f5d21387dc41a9e2cc`.
- No manuscript source, rendered figure, bibliography, PDF, executable, or raw
  result artifact was changed, so no paper build or code test was required.

## Active Task Update: Mixed-Service Traffic Behavior and Resource Competition

Task requested 2026-08-12: make explicit that the selected CV/CAV examples do
not merely have different payload and latency requirements. They produce
different uplink/downlink or sidelink behaviors, and those periodic, bursty,
sustained, broadcast, many-to-many, individualized, and long-lived services can
compete for radio, transport, edge, backhaul, and operator resources.

Status: complete. `paper/paper_outline.md` now makes mixed-service coexistence a
central part of the readiness question rather than an afterthought. The
Introduction distinguishes the traffic signatures of signal publication,
direct collision/blind-spot awareness, infrastructure-guided intersection
planning, emergency maneuver coordination, edge-local recovery, and
help-center recovery. It asks whether a compact deadline-critical exchange
retains its envelope when sustained or bursty traffic is present.

Section 4.7 now:

- defines direction, temporal pattern, fan-out, common versus individualized
  content, session duration, and service occupancy as application requirements;
- adds mixed-service coexistence as the seventh joint readiness condition;
- gives every worked application example a `Traffic behavior and competition`
  paragraph;
- keeps PC5 sidelink resources distinct from 5G Uu uplink/downlink resources;
- proposes a two-panel design figure showing application traffic lanes and the
  separate shared-resource regions in which they contend;
- retains the application mapping table only as an authoring evidence ledger,
  not proposed main-paper prose;
- defines `A_i(B_i | W)` for foreground deadline-qualified availability under a
  specified workload mix; and
- states that only the 1 KiB foreground exchange under offered 25 Mbit/s uplink
  traffic is a direct heterogeneous mixed-flow experiment. The 1--100-client
  sweep is homogeneous session scaling, while the payload/fragmentation runs
  characterize potential competing-workload cost but do not themselves measure
  coexistence.

Section 4.8 now frames Experiment Family D as a foreground/background
coexistence test within one vehicle host and one MG52/UE attachment, Family E as
homogeneous correlated-session scaling, Family F as complete-object and packet-
amplification characterization, and Family G as an unverified QoS-protection
check. It explicitly prohibits multi-UE fairness, scheduler-isolation,
emergency-preemption, or full application-mixture claims that the stored
experiments do not support.

Section 5.6 now presents the 1 KiB service as the foreground whose complete-
response envelope is compared at idle and under a sustained vehicle-originated
background flow. Section 5.9 connects the result to the existing three insights
without creating a fourth: future systems must expose traffic behavior, protect
compact emergency/control exchanges from sustained queues, distinguish common
publishable content from per-vehicle plans, provision direction-aware
resources, and coordinate PC5 and Uu without treating their resource pools as
interchangeable.

The IPI design goals now make one implementation boundary explicit: the current
reference protocol exposes only part of the application intent through
transport, priority, requested horizon, expiration, and session state. A fuller
direction/rate/temporal/fan-out/session descriptor is an extension requirement,
not a claimed implemented radio reservation or QoS mechanism.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed top-level paper order remains Introduction, Related Work, IPI
  Protocol Design, System Design and Experimental Setup, and Experiment Results.
- Confirmed Section 5.9 still contains exactly three numbered insight
  subsections with the approved top-level statements.
- Confirmed the outline distinguishes the PC5 sidelink pool, 5G Uu uplink, 5G
  Uu downlink, and CPE/transport/core/edge/backhaul/operator resources.
- Confirmed no claims of an all-application concurrent trial, multiple
  independently scheduled vehicle UEs, verified scheduler isolation, or
  verified 5QI/GBR treatment were introduced.
- Confirmed the paper outline does not frame recovery as remote parking, does
  not use 250 km/h as a vehicle scenario, and does not contain the incorrect
  0.347 m calculation; it also has no trailing whitespace or consecutive
  duplicate nonblank lines.
- The outline is 3,776 lines, 29,742 words, and 215,429 bytes with SHA-256
  `c9d3e05fb31d7700177628e253376bb3bda1d639974e0c2e9f6dbd0763cd2c6d`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Concise Insights and Field-Level Future Research Directions

Task requested 2026-08-12: keep each of the paper's three insights short and
concise, then add a Future Research Directions section that serves readers of
the paper rather than listing future work or planned extensions of Edge4AV.

Status: complete. `paper/paper_outline.md` now limits each insight to one short
manuscript paragraph containing the approved insight statement, one evidence
sentence, and one implication sentence. The previous detailed evidence has
been retained in a clearly labeled authoring-only evidence ledger, so the
results remain traceable without repeating tables, long stakeholder lists, or
future research recommendations inside the insight subsections.

The new Section 6, `Future Research Directions`, explicitly distinguishes a
field-level research agenda from paper-specific limitations or future work. It
opens from the three insights and develops five directions:

1. **Enforceable application/network contracts:** define a bidirectional,
   verifiable service profile covering complete-object size, direction, rate,
   fan-out, temporal pattern, deadline, freshness, reliability, session length,
   partial-result policy, and fallback; map it to real QoS, admission,
   scheduling, PC5, edge, and backhaul mechanisms; authenticate and audit the
   declared and applied treatment.
2. **Heterogeneous-service scheduling:** protect compact deadline-critical
   foreground exchanges while periodic awareness, common signal state,
   individualized plans, emergency bursts, detector objects, bulk transfer, and
   sustained recovery streams compete across distinct PC5, Uu uplink/downlink,
   transport, edge, backhaul, and operator resources. The proposed evaluation
   objective is foreground `A_i(B_i | W)` plus useful-object efficiency,
   resource cost, fairness, and degradation, not throughput alone.
3. **Joint information/computation/communication adaptation:** choose the
   smallest safe and useful representation and its execution location using
   semantic reduction, uncertainty, progressive results, caching/prefetch,
   common-versus-individualized content, and dynamic placement across vehicle,
   direct cooperation, transformative intersection, regional edge, and help
   center. This direction gives traffic-signal researchers and ITS operators an
   explicit role in reconciling authoritative controller state, infrastructure
   perception, common advisories, and individualized CAV plans.
4. **Stateful continuity and safe degradation:** preserve or explicitly
   terminate session state through radio loss, handover, roaming, broker/process
   failure, edge migration, backhaul loss, and remote-assistant unavailability;
   integrate prediction, multi-connectivity, failover, cybersecurity, and the
   vehicle's minimal-risk behavior; evaluate recovery gaps and state/fallback
   correctness rather than packet loss alone.
5. **Community application-readiness benchmarks:** define recognizable
   application portfolios and controlled mixed workloads; include multiple UEs,
   heterogeneous radio conditions, mobility, handover, TDD/interference, public
   and private paths, and repeatable failures; retain synchronized/application/
   RAN evidence; report all-attempt and complete-object metrics with reliability-
   appropriate confidence; and release privacy-preserving workloads, traces,
   and analysis artifacts.

The section closes with a shared research objective for CAV, CV, ITS, traffic-
signal, radio, carrier, edge, and 5G/6G researchers. It prohibits presenting
IPI, one scheduler, semantic communication, edge computing, multi-connectivity,
or 6G as a standalone solution and preserves local vehicle safety behavior when
communication is unavailable.

Authoritative source anchors are provided for the eventual prose: 3GPP/ETSI TS
22.186 Release 19, ETSI MEC 030, the NGMN 5G TDD Uplink white paper, ITU-R
M.2160 for IMT-2030, 5GAA C-V2X Use Cases Volume II, and NHTSA ADS 2.0. The
NGMN, ITU, and 5GAA links resolved successfully in the current URL check;
ETSI/NHTSA returned HTTP 403 to the automated retrieval check but retain their
official document URLs already used elsewhere in the outline.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed top-level order is Introduction, Related Work, IPI Protocol Design,
  System Design and Experimental Setup, Experiment Results, and Future Research
  Directions.
- Confirmed Section 5.9 still contains exactly three numbered insight
  subsections with the approved top-level statements.
- Confirmed every proposed insight is one compact paragraph and detailed result
  evidence is labeled as authoring-only material.
- Confirmed Section 6 contains five numbered research directions and does not
  call them additional insights or contributions.
- Confirmed the section explicitly prohibits paper-specific future-work
  framing, generic “more work is needed” language, and claims that one
  technology solves the full application-readiness problem.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,071 lines, 31,795 words, and 231,766 bytes with SHA-256
  `ec642c4412fb23dff8412504341ba5e205daef42b531777754a62849edae2375`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Concise Conclusion

Task requested 2026-08-12: add the final Conclusion section, keep it short and
concise, summarize the paper, and clearly mark the author's contributions.

Status: complete. `paper/paper_outline.md` now ends with Section 7,
`Conclusion`. The proposed manuscript conclusion is a single 151-word paragraph
that:

- restates the paper's question about whether today's deployed V2X and 5G paths
  are ready for future CV/CAV communication;
- marks exactly two contributions with explicit `Our first contribution` and
  `Our second contribution` language;
- identifies the first contribution as the transport-independent,
  J2735-compatible IPI protocol and implementation for conventional CV/ITS
  messages and correlated stateful CAV services;
- identifies the second contribution as the application-driven real-autonomous-
  vehicle evaluation of certified off-the-shelf LTE C-V2X PC5 and private-5G NR
  Uu vehicle-to-edge paths;
- summarizes the direct-V2X and bounded-5G findings without repeating result
  values; and
- closes with the need to join explicit application semantics to adaptive and
  verifiable direct, cellular, edge, and failure-handling decisions.

The conclusion explicitly prohibits new numbers, citations, limitations,
applications, future directions, stakeholder lists, equipment inventories, or
a third/testbed contribution. It also avoids presenting Mocar or Edge4AV as the
proposed system and avoids claiming that one radio, IPI, 5G, or 6G solves the
complete problem.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the complete top-level order is Introduction, Related Work, IPI
  Protocol Design, System Design and Experimental Setup, Experiment Results,
  Future Research Directions, and Conclusion.
- Confirmed exactly three insight subsections and five future-research
  directions remain.
- Confirmed the proposed conclusion contains exactly one paragraph and 151
  words, and marks exactly two contributions.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,113 lines, 32,127 words, and 234,042 bytes with SHA-256
  `01b56e1bf22325735daa2f2ca6c04860c2475ca0723cc4c8443eec18a63fa641`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Binding Submission-Format Requirements

Task clarified 2026-08-12: record the supplied venue requirements, but do not
rebudget, shorten, move, or otherwise revise the paper outline in response to
them.

Status: complete. A clearly labeled non-manuscript requirements block now
appears near the beginning of `paper/paper_outline.md`. It records:

- the 12-page limit for all numbered non-bibliographic content;
- unlimited bibliography pages and post-bibliography appendices that do not
  count toward the limit, with the warning that reviewers need not read them;
- the requirement that the core paper remain self-contained;
- minimum 10-point font;
- double-column dimensions, column spacing, and the 55-line limit;
- US letter paper;
- English-Adobe-Acrobat-compatible PDF as the only accepted format;
- anonymity in both visible paper content and PDF metadata/content;
- no embedded PDF hyperlinks;
- a file size below 15 MB;
- the ACM proceedings template and
  `\documentclass[sigconf,10pt]{acmart}`; and
- the authors' responsibility for manual or online-checker verification.

No section allocation, figure plan, table plan, appendix routing, result scope,
or manuscript source was changed. The seven paper sections remain exactly the
same. The partial PDF/source inspection performed before the scope clarification
was read-only; no PDF or LaTeX file was modified or regenerated.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the requirements block is explicitly labeled as not being a
  manuscript section.
- Confirmed the top-level paper order remains Sections 1--7 with no additional
  numbered paper section.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,147 lines, 32,413 words, and 235,974 bytes with SHA-256
  `d4e6faf324b37767482abd2c2392d172ec7c7efdbb635cd2e87f74b2d40fa92f`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Narrative Conclusion Revision

Task requested 2026-08-12: replace the conclusion because the previous version
read as a shopping list.

Status: complete. The proposed Section 7 conclusion in
`paper/paper_outline.md` has been rebuilt rather than locally rephrased. It now
uses two connected paragraphs and a question-to-answer narrative:

- CAVs require information and assistance beyond onboard sensing and
  computation, but the presence of V2X and 5G does not establish application
  readiness;
- IPI and the real-vehicle PC5/Uu evaluation appear together as the two means by
  which the paper makes that readiness question measurable, rather than as
  labeled `first` and `second` contribution items;
- the measured answer is expressed as a complementary but incomplete division
  of labor between compact/local direct V2X and richer but condition-bounded 5G
  exchanges; and
- the closing sentence gives one takeaway: judge and design CAV communication by
  whether it completes the right service within its useful time and operating
  conditions.

The revised conclusion is 154 words. It does not enumerate experiment
dimensions, result values, stakeholders, limitations, future directions, or
equipment. It preserves the two contributions and three insights without
presenting either group as a list.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the conclusion contains two short connected paragraphs.
- Confirmed both contributions remain explicit through the paired
  `designing and implementing IPI` and `evaluating complete exchanges` clauses.
- Confirmed no new result, testbed contribution, system name, citation, or
  future-work claim was introduced.
- Confirmed the seven-section outline and binding submission requirements are
  otherwise unchanged.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,153 lines, 32,462 words, and 236,219 bytes with SHA-256
  `65a5df84124c158282838adae837f8e18b03e83dfaa6813ce44d1ac6b814467a`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Binding Logical-Transition Writing Requirements

Task requested 2026-08-12: add a writing requirement that every sentence,
paragraph, subsection, and section must lead logically to the next using
transition words, linking words, or other transitional devices.

Status: complete. A new non-manuscript block titled `Binding writing and
logical-transition requirements` now appears near the beginning of
`paper/paper_outline.md`, alongside but separate from the binding submission-
format requirements. It requires:

- an explicit logical relationship between every pair of adjacent sentences;
- backward and forward links at every paragraph boundary;
- closing/opening bridges across subsection and section boundaries;
- precise transition devices for cause, consequence, contrast, qualification,
  comparison, evidence/example, continuation, and sequence;
- use of backward-pointing noun phrases, repeated technical subjects, and
  linking clauses where they produce a more natural transition than a sentence-
  initial conjunctive adverb;
- correction of missing premises or disordered reasoning rather than insertion
  of a logically false `however`, `therefore`, or `moreover`;
- synthesis of related facts into claim, evidence, and consequence instead of
  shopping-list prose; and
- an explicit revision pass over sentence, paragraph, and section boundaries.

The requirement states that transition quality is part of the paper pass
condition: technically correct prose still fails when the reader must infer why
one statement or unit follows another. It does not require every sentence to
start with a transition word; it requires the relationship itself to be
explicit and correct.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the writing requirement is labeled as not being a manuscript
  section.
- Confirmed all seven numbered paper sections, the three insights, the five
  future-research directions, the revised conclusion, and the submission-format
  requirements are otherwise unchanged.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,204 lines, 32,935 words, and 239,541 bytes with SHA-256
  `2841b20c2f0b4d29512ad2c55811253602de4b2933e2e82a9d8b734389fd45e2`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Binding Terminology and Abbreviation Requirements

Task requested 2026-08-12: add writing requirements that prohibit invented or
confusing words, require explanations of terminology, and require the full
spelling of every abbreviation at first use.

Status: complete. A new `Terminology, word choice, and abbreviation
requirements` subsection now appears inside the existing non-manuscript binding
writing requirements in `paper/paper_outline.md`. It requires:

- established terminology from standards, literature, current evidence, the
  repository, or the user's instructions rather than invented technical-
  sounding words, labels, or drafting shorthand;
- familiar, unambiguous wording when it expresses the same technical meaning;
- explanation of every specialized term at first use, including its meaning in
  this paper and why it matters when that connection is not obvious;
- the full spelling of every abbreviation or acronym before the parenthetical
  abbreviation appears;
- application of the first-use rule to the title, abstract, keywords, headings,
  main text, figures, tables, captions, footnotes, and appendix;
- separate first-use expansion in the abstract and the main paper body;
- stable use of one term for one concept and one abbreviation for one concept;
- explicit definitions for overloaded words such as latency, reliability,
  availability, success, real time, edge, ready, support, and complete;
- full names and plain-language roles for cross-field radio terms such as
  sidelink, Uu, bearer, time-division duplex, physical resource block,
  modulation and coding scheme, and block error rate; and
- a revision check from the perspective of a reader who knows one relevant
  field but not necessarily all of CAV, CV, ITS, traffic signals, networking,
  radio, carriers, edge computing, and 5G/6G.

The requirement also prohibits definitions that merely replace one unexplained
term with another. If a term cannot be explained plainly and precisely, the
writer must supply the missing explanation or use clearer wording.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the terminology rules are contained inside the binding writing
  requirements and are not a numbered manuscript section.
- Confirmed the seven paper sections and all previously agreed scientific
  content remain unchanged.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,259 lines, 33,469 words, and 243,237 bytes with SHA-256
  `3c4e85fd86f138d0429f53647ba5ca7569eee72749dd42c82c12b385dffcf80c`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Evidence-Based Reviewer-Concern Writing Requirements

Task requested 2026-08-12: prohibit commentary-style manuscript prose that
argues with anticipated reviewer comments; require anticipated concerns to be
answered through experiments or other convincing evidence, or through an
appropriately limited claim instead of overclaiming.

Status: complete. A new `Evidence-based responses to anticipated reviewer
concerns` subsection now appears inside the binding non-manuscript writing
requirements in `paper/paper_outline.md`. It requires the manuscript to:

- avoid invisible-reviewer and rebuttal phrasing such as `one might question`,
  `reviewers may argue`, `we acknowledge this concern`, and repeated defensive
  `we do not claim` statements;
- answer empirical concerns with experiments, controls, comparisons, baselines,
  ablations, repetitions, denominators, thresholds, packet captures,
  configuration records, authoritative sources, or failure analyses;
- maintain the evidence chain `research question -> required condition ->
  experimental comparison -> observed result -> supported claim`;
- narrow the research question and claim to the measured system boundary,
  workload, path, metric, and operating conditions when broader evidence is not
  available;
- state a scope boundary positively and early instead of overclaiming first and
  retreating through later disclaimers;
- place each scope boundary once at the point where it defines the experiment or
  claim, rather than scattering defensive qualifications throughout the paper;
- treat limitations as descriptions of out-of-scope conditions, not as devices
  that rescue unsupported central claims;
- answer realism, causality, treatment-verification, and reliability concerns
  with the type of evidence each concern requires; and
- remove a claim when it cannot be supported by evidence, a defensible scope
  boundary, or an authoritative argument.

The rules include paper-specific examples at the correct strength: logical
clients behind one host/one user-equipment attachment must be framed as
application-session scaling rather than a 100-vehicle result; an application
quality-of-service label without observed packet marking must be reported as an
unverified intended treatment; unmatched radio evidence supports an
application-level association rather than a scheduler mechanism; and
`1,000/1,000` completions must not be promoted into an unsupported multi-nine
reliability claim.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the new reviewer-concern rules are part of the binding writing
  requirements and are not a numbered manuscript section.
- Confirmed all seven paper sections and previously agreed scientific content
  remain unchanged.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,319 lines, 34,030 words, and 247,232 bytes with SHA-256
  `e2c1c5adaee57282674dac8e8d3bf90ec6210609a067b1262edbc9d66a3b8b46`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Binding Sentence-Length and List Requirements

Task requested 2026-08-12: require short and concise manuscript sentences unless
a longer sentence is needed to express a transition of thought, and require any
necessary list to be short and numbered in `1)`/`2)` form.

Status: complete. A new `Sentence length and list requirements` subsection now
appears inside the binding non-manuscript writing requirements in
`paper/paper_outline.md`. It requires:

- one main claim, result, relationship, or argumentative role per sentence;
- splitting sentences that combine independent claims, results, experimental
  conditions, qualifications, or implications;
- avoiding compression through comma chains, semicolons, parentheses, or
  repeated coordinating conjunctions;
- permitting a longer sentence only when its clauses must remain together to
  express contrast, condition, qualification, cause, or consequence through a
  link such as `but`, `otherwise`, `although`, `because`, `whereas`, `if`, or
  `unless`;
- keeping each clause short even when a transition justifies a compound or
  complex sentence;
- avoiding both long overloaded sentences and choppy fragments;
- avoiding lists when connected prose can synthesize the claim more clearly;
- keeping a necessary list short and numbering it as `1)`, `2)`, `3)`, and so
  on;
- requiring parallel list items plus an introductory sentence and a following
  interpretation sentence;
- prohibiting nested manuscript lists; and
- using prose, a well-designed comparison table, or an appendix when the content
  requires many items or repeated fields.

The revision rule flags sentences with multiple commas, conjunctions,
semicolons, or long parenthetical phrases. Such a sentence remains only when it
expresses one necessary transition of thought more clearly than two short
sentences would.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the sentence/list rules are part of the binding writing requirements
  and are not a numbered manuscript section.
- Confirmed the seven paper sections and all previously agreed scientific
  content remain unchanged.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,357 lines, 34,382 words, and 249,583 bytes with SHA-256
  `7fac3f4e74faa50d9aa3f8dd2377ae9dc3bf8447d7e053223588999d64185586`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: Contributions and Insights Bullet-List Exception

Task requested 2026-08-12: allow the paper's Contributions and Insights to use
bullet lists.

Status: complete. The binding `Sentence length and list requirements` in
`paper/paper_outline.md` now contain a narrow exception for the Contributions
and Insights portions of the manuscript. Those portions may use bullet lists
because they present parallel, high-level takeaways that readers should be able
to scan quickly. Each bullet must remain short and self-contained, and each
bullet must contain only one contribution or one insight. Experimental details
and nested lists do not belong inside these bullets. The rule requiring other
necessary manuscript lists to remain short and numbered as `1)`, `2)`, and so
on remains unchanged.

Files updated:

- `paper/paper_outline.md`
- `current_task.md`

Validation:

- Confirmed the exception appears inside the binding non-manuscript writing
  requirements rather than inside a numbered paper section.
- Confirmed the exception applies only to the Contributions and Insights.
- Confirmed the seven paper sections and all previously agreed scientific
  content remain unchanged.
- Confirmed no trailing whitespace or consecutive duplicate nonblank lines.
- The outline is 4,362 lines, 34,430 words, and 249,911 bytes with SHA-256
  `d138292e811c4d8a0211ee1c81850e794f67fd7f4def1d18180554e6d9fcb3fb`.
- No manuscript source, rendered figure, bibliography, PDF, executable, raw
  result artifact, or experiment configuration was changed, so no paper build
  or code test was required.

## Active Task Update: New Manuscript Draft and Section-by-Section Red Team

Task requested 2026-08-12: begin a brand-new paper draft without reusing stale
manuscript prose. After completing each section, run a GPT-5.6 Terra Max red
team as three MobiCom reviewers, adjudicate the comments, apply only clearly
justified revisions, and place uncertain revisions in a proposed-changes file
for user approval.

Status: in progress. The stale and new paper sources are physically
separated. `paper/legacy_draft/` contains the former TeX, bibliography,
Makefile, and compiled artifacts. `paper/manuscript/` is the only active draft
and contains a new wrapper, new bibliography, and newly written Sections 1--4.
The clean wrapper does not input any file from `paper/legacy_draft/`.

Current stage:

- Section 1, `Introduction`, was written from the approved outline, current
  implementation, current experiment evidence, and verified primary sources.
- A GPT-5.6 Terra Max subagent reviewed the section as three independent
  MobiCom reviewers: mobile networking/systems, CAV/CV/ITS, and cellular/radio
  measurement.
- The review was saved to `paper/reviews/section_01_mobicom_red_team.md`. Every
  required comment was adjudicated against the implementation and stored
  results. Clearly justified revisions were applied.
- The revised Introduction states the interface gap at feature level, narrows
  J2735 and failure claims to the implementation, separates SRM/SSM from
  replaceable state messages, reports the QoS-label check accurately, gives
  attempt denominators, and separates controlled effects from field
  associations.
- The Introduction contains exactly two contribution bullets and exactly three
  approved insight bullets. The word `strictly` was restored in Insight 2.
- One possible protocol and conformance expansion is genuinely uncertain and
  remains unapplied as P-001 in `paper/manuscript/proposed_changes.md`.
- Section 2, `Related Work`, was written from the approved two-gap structure
  and newly verified primary sources. It synthesizes standards and platforms,
  CAV applications and middleware, direct-PC5 evidence, cellular/5G trials,
  and multi-radio co-design.
- A GPT-5.6 Terra Max subagent reviewed Section 2 as three MobiCom reviewers.
  The report is saved in `paper/reviews/section_02_mobicom_red_team.md`.
- The adjudicated revision credits VAE session control, MEC message services,
  the 5GAA application-layer architecture, and ETSI ITS-station facilities at
  their actual strength. It positions IPI as the narrower operation-level
  contract that joins lightweight profiles for SAE J2735 message types and
  pre-encoded J2735 payloads with correlated CAV services.
- The revision also treats the 40-km PC5/Uu corridor, commercial teleoperation,
  SEE-V2X, adaptive image transfer, PC5 hardware-in-the-loop control, and
  CooperScene as close but partial evidence comparators. It states the paper's
  distinctive experimental combination directly instead of using broad
  absence claims.
- The approved left--gap--right positioning figure is implemented as a clean
  three-panel figure. It appears before the bibliography and was visually
  checked at ACM rendering size.
- Section 2 introduced no new uncertain manuscript or implementation change.
- Section 3, `IPI Protocol Design`, was rebuilt from the current C++ headers,
  source, transport probes, and tests. It defines the implemented common
  envelope, typed and opaque J2735 paths, local canonical encoding, service
  request and cooperative-object formats, intended session lifecycle, topic
  convention, adapter boundaries, and parser-acceptance measurement boundary.
- A GPT-5.6 Terra Max subagent reviewed Section 3 as protocol/API novelty, SAE
  J2735/ITS standards, and distributed-systems/session reviewers. The full
  report is saved in `paper/reviews/section_03_mobicom_red_team.md`.
- The adjudicated revision distinguishes the current artifact from its intended
  deployment. In particular, it does not claim formal SAE ASN.1/UPER
  conformance, an enforced session state machine, a broker-backed session
  adapter, or complete negative-test coverage.
- P-001 remains pending. P-002 was added to
  `paper/manuscript/proposed_changes.md` for an enforced cross-transport session
  path, terminal outcomes, correlation checks, retained fallback inputs, and
  expanded negative and interoperability tests. Neither proposal was applied.
- Section 4, `System Design and Experimental Setup`, was rebuilt from the
  current deployment records, implementation, result manifests, application
  requirements, and binding outline. It defines the separate PC5 and Uu paths,
  field and radio context, workload provenance, path-response timing boundary,
  five application envelopes, traffic-resource framing, and seven-family
  experiment matrix without stating performance results or the three insights.
- A GPT-5.6 Terra Max subagent reviewed Section 4 as radio/RAN/private-5G,
  C-V2X/ITS/CV/CAV requirements, and networking/systems methodology reviewers.
  The report is saved in `paper/reviews/section_04_mobicom_red_team.md`.
- The adjudicated revision corrects the private-5G timing source to same-host
  wall-clock request/response RTT and defines deadline availability over a
  sequence-matched PC5 workload echo or a returned 5G IPI workload response.
- The detector condition is now an opaque prediction-count-derived payload-size
  model rather than a claimed native detector serialization. The application
  table removes unsupported MAP interoperability and separates PC5 custom echoes
  from 5G IPI-envelope ACK probes.
- The follow-up MG52 Cell 2 lock is operator-verified. Location-associated radio
  context comes from a separate iPhone survey and is not MG52 telemetry.
  Logical-client traffic is a closed-loop sequential workload with a maximum
  nominal rate, not a fixed open-loop rate or a multi-UE field test.
- P-003 was added to `paper/manuscript/proposed_changes.md` for monotonic timing,
  workload correlation, native detector serialization, and matched radio
  exports. It remains unapplied pending user approval.
- Section 5, `Experiment Results`, was written as a comparison-first answer to
  the application-readiness question. It presents format/path-function checks,
  PC5 packet-size and route continuity, heterogeneous 5G evidence groups,
  transport and complete-object formation, direction and active-background-
  generator behavior, QoS-label evidence, logical-client demand, and injected
  service interruption before the final synthesis.
- A GPT-5.6 Terra Max subagent reviewed Section 5 as CAV/CV/ITS requirements,
  cellular/RAN/V2X measurement, and MobiCom systems-methodology reviewers. The
  report is saved in `paper/reviews/section_05_mobicom_red_team.md`.
- The adjudicated revision evaluates application-derived workloads carried in
  IPI frames using request/response RTT and deadline availability. It also
  reports the 25-Mbit/s load-generator setting as an offered target and reports
  the much lower achieved host rates separately.
- Weak-location collections are described as location-associated weak-path
  collections because aligned management-plane evidence remains pending.
  Figure 6 now states that its rows are heterogeneous evidence groups rather
  than a controlled payload curve.
- Figure 9 now reports all-attempt 100-ms and 200-ms availability for every
  logical-client count and transport, in addition to accepted-response p95.
  The text states that each logical client is sequential and that the nominal
  rates are maxima rather than sustained offered rates.
- Table 5 maps local IPI format/object validation and field workloads to their
  measured response and deadline outcomes. Section 5.9 retains exactly the
  three approved insight bullets.
- The same reviewer re-ran the P0/P1 audit after revision and returned PASS
  with no remaining correctness blocker. No new uncertain implementation
  proposal was added; P-001--P-003 remain pending.
- Section 6, `Future Research Directions`, was written as a field-level agenda
  for CAV, ITS, traffic-signal, radio, carrier, and edge researchers rather than
  as limitations or planned extensions to Edge4AV.
- A GPT-5.6 Terra Max subagent reviewed Section 6 as CAV/CV/ITS and traffic-
  signal, cellular/RAN/carrier, and MobiCom systems/research-agenda reviewers.
  The report is saved in `paper/reviews/section_06_mobicom_red_team.md`.
- The adjudicated revision defines a task-specific decision outcome, uses
  complete-object goodput consistently, makes the proposed benchmark a modular
  factorized portfolio, and states the authority boundary for infrastructure-
  assisted intersections. Each subsection now begins with an explicit research
  question.
- The same reviewer ran a focused second pass and returned PASS with no
  remaining P0/P1 or must-fix item. Section 6 introduces no additional insight
  and no new uncertain implementation proposal.
- Section 7, `Conclusion`, was written as two short connected paragraphs. It
  makes the two contributions clear through narrative and synthesizes exactly
  the three approved insights without adding numbers, citations, limitations,
  applications, or future directions.
- A GPT-5.6 Terra Max subagent reviewed Section 7 as CAV/CV/ITS and protocol,
  cellular/RAN/V2X measurement, and MobiCom systems/editorial reviewers. The
  report is saved in `paper/reviews/section_07_mobicom_red_team.md`.
- The source separates the IPI protocol contribution from the PC5/Uu field
  evaluation by saying `Alongside this protocol`. The current rebuilt PDF was
  rechecked and contains that wording, so it does not imply that IPI itself ran
  over the PC5 custom-echo path. The second review returned PASS with no
  remaining P0/P1 or must-fix item.

Files created or reorganized:

- `paper/manuscript/main.tex`
- `paper/manuscript/Makefile`
- `paper/manuscript/references.bib`
- `paper/manuscript/sections/01_introduction.tex`
- `paper/manuscript/sections/02_related_work.tex`
- `paper/manuscript/sections/03_ipi_protocol_design.tex`
- `paper/manuscript/sections/04_system_design_setup.tex`
- `paper/manuscript/sections/05_experiment_results.tex`
- `paper/manuscript/sections/06_future_research_directions.tex`
- `paper/manuscript/sections/07_conclusion.tex`
- `paper/manuscript/scripts/build_section5_figures.py`
- `paper/manuscript/figures/section5_pc5_payload_route.pdf`
- `paper/manuscript/figures/section5_5g_deadline_envelope.pdf`
- `paper/manuscript/figures/section5_protocol_completion.pdf`
- `paper/manuscript/figures/section5_mixed_load_qos.pdf`
- `paper/manuscript/figures/section5_concurrency_interruption.pdf`
- `paper/manuscript/proposed_changes.md`
- `paper/manuscript/main.pdf`
- `paper/reviews/section_01_mobicom_red_team.md`
- `paper/reviews/section_02_mobicom_red_team.md`
- `paper/reviews/section_03_mobicom_red_team.md`
- `paper/reviews/section_04_mobicom_red_team.md`
- `paper/reviews/section_05_mobicom_red_team.md`
- `paper/reviews/section_06_mobicom_red_team.md`
- `paper/reviews/section_07_mobicom_red_team.md`
- `paper/legacy_draft/` containing the stale paper sources and outputs
- `current_task.md`

Validation completed after the Section 5 red-team pass:

- `make` succeeds from `paper/manuscript/` using `pdflatex` and `bibtex`.
- The final-pass build log has no undefined citation, undefined reference,
  LaTeX error, or overfull-box warning.
- The ACM-format Sections 1--5 draft is 25 pages including references. Page
  limits are intentionally ignored at this stage.
- PDF rendering was inspected page by page. Text is legible, columns are
  aligned, and no clipping or overlap is visible. The positioning and protocol
  figures and the protocol-field table are legible and appear before the
  reference list.
- The PDF is US letter size, embeds all fonts, and contains zero annotations
  and zero link objects.
- PDF metadata contains no author or institution identity.
- The current C++ implementation was rebuilt with tests enabled. All seven
  CTests pass: interface, private-session transport, private-5G latency probe,
  J2735 message flow, experiment logging, TCP loopback, and MQTT loopback.
- The active Introduction is 1,651 words. Its SHA-256 is
  `71a427ecb0f2f4977086c8aa4429b06397771362547aba8bc5675fbc835f515e`.
- The active Related Work section is 1,925 words. Its SHA-256 is
  `f3ec8e2ea6e84234da4de22c8d2a6cc6ff155c7901a0e9830d730961e8957007`.
- The active IPI Protocol Design section is 2,819 words. Its SHA-256 is
  `43bd5738d1ca8fa4af0a6818a9ffb0f5e347cf51c6768b1bd25a3fabb418ba10`.
- The active System Design and Experimental Setup section is 5,467 words by a
  `detex` count. Its SHA-256 is
  `55e76820d2026aa23091eb12d4c69bad2fdd1aa25591a1b9dbac84b102a1d4b9`.
- The active Experiment Results section is 3,233 words by a `detex` count. Its
  SHA-256 is
  `d234b6957446cf76acac69ead0d1a9d5c38e1f528e650217c87ad4a5cfb7de6a`.
- The Section 3 red-team report SHA-256 is
  `1b09a6f6d67b1ffd2ac7a318110772099b00c2f79a5f303dc82eaed4e7ab1913`.
- The Section 4 red-team report SHA-256 is
  `b8bc01665beef18353cb4e354f869e0433be6c68cad98eb20d09f507f52c8172`.
- The Section 5 red-team report SHA-256 is
  `a24f533c1474ab5ac42dac35361c81771040f0950eee75bcc3af848b54e3aac3`.
- The active Future Research Directions section is 1,391 words by a `detex`
  count. Its SHA-256 is
  `eb73a68dbe36e5357e4fbd32f78cd6110251e9a5f9aa3c8b2d4ce57ba082da73`.
- The Section 6 red-team report, including its passing second review, has
  SHA-256
  `d020db697a2050ca30d5de2d37b76458533223d59fc9b2c0a13ad7a9d43feab1`.
- The active Conclusion is 172 words by a `detex` count. Its SHA-256 is
  `a44fb9ba6b2b689298df0abff6133a2f41600c00591ad6cbbf474b6ae54f3407`.
- The Section 7 red-team report, including its passing artifact recheck, has
  SHA-256
  `cfd66641036daecba0d2a512b9dfbaa42c35c9441f2f2679895b2d7e5f0d38d4`.
- The current seven-section PDF is 27 pages including references and has
  SHA-256
  `0da08ed2a4279390facfb7109e4c3a4cb0e81530dc6a28779f65a693c2f01063`.
  It is an integration checkpoint, not the final 12-page-compliant manuscript.
- The proposed-changes SHA-256 is
  `2779bda7ae5024181c5c72b5965cc693fd35249c7e73a14e9cce1e392e903172`.
- The clean PDF SHA-256 is
  `2548d489166f271d8b9de4d2adb1be531a41104f4a4fb24b30adc35fe8e2ee87`.

## Active Task Update: Abstract and Whole-Paper Integration

Task update requested 2026-08-12: retain the full new draft and ignore the
page-limit requirement during the current writing and integration stage.

Status: in progress. The abstract has been written and reviewed. Its controlling
structure asks whether today's edge communication is ready for tomorrow's CAV
applications, distinguishes replaceable stateless updates from correlated
stateful services, identifies IPI and the two-path evaluation, and ends with
exactly the three approved insights. The focused abstract red team returned
PASS with no remaining P0/P1 issue.

The page-limit-only compression pass was stopped immediately after the user's
instruction. The full reviewed versions of the abstract, Introduction, Related
Work, and IPI Protocol Design were restored byte-for-byte. The full System
Design and Experimental Setup content was also restored and retains the
reviewer-required corrections: same-host wall-clock request/response RTT,
application-derived workloads carried in IPI frames, prediction-count-derived
opaque detector sizes, conditional Cell 2 and location evidence, path-specific
PC5/5G probes, and closed-loop logical-client rate semantics.
No scientific result, experimental condition, protocol boundary, or future
research direction was removed to reduce page count.

Current integration validation:

- `make -C paper/manuscript clean all` succeeds.
- The final build pass contains no undefined citation, undefined reference,
  LaTeX error, or overfull-box warning.
- The current PDF is 27 US-letter pages including references and is 707,837
  bytes. The length is recorded but is not a failure during this stage.
- All 27 pages were rendered and inspected. The text, figures, tables, headers,
  page numbers, columns, and references show no clipping, overlap, missing
  glyph, or broken float.
- The abstract contains 341 `detex` words. Sections 1--7 contain 1,657, 1,952,
  2,809, 5,494, 3,233, 1,391, and 174 `detex` words, respectively.
- The current PDF SHA-256 is
  `91d27901fee64f56d31ba5200033a9092f9f554b9c50ecf39a10c7a8eb4b6404`.
- A whole-paper three-perspective MobiCom integration review is running against
  `paper/manuscript/` only. Its brief explicitly excludes page-count criticism
  and forbids use of `paper/legacy_draft/`.
