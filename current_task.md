# Current Task

Last updated: 2026-08-24

## 2026-08-24 Experiment Collection Closure Decision

Complete. The user accepted the retained experiment corpus as sufficient for
the current paper and decided not to collect another broad performance matrix
or a new one-way-latency dataset. The August 19 GL-X3000 runs complete the
requested private-5G TCP/MQTT uplink-heavy, downlink-heavy, and exact 50-MiB
directional-goodput work under `70/20/10` and `40/40/20`. The existing direct
V2X stationary-payload, radio-condition, and mobility runs remain the V2X
evidence. Do not repeat these experiment families solely to add samples.

This collection decision does not strengthen the claims beyond the retained
controls. Treat the GL-X3000 TDD result as a sequential, matched-location
deployment observation rather than a general or isolated causal TDD effect;
serving-cell state and matched ACP evidence were not retained. The old/new
gateway comparison remains confounded by signal, day, configuration, session,
and path state. Private-5G request/response results remain complete application
RTT measurements. One-way latency is unmeasured and is not required for the
current paper; direct dissemination latency or information age is future work
if a later study adds semantically matched V2X and
vehicle-to-edge-to-vehicle delivery with a defensible common time base.

No further field-performance collection is planned for the current paper.
Network-enforced QoS remains a documented blocked experiment unless Cisco
provisions and verifies a second DNN/QFI/5QI treatment. This decision changes
only the collection plan and task record; no raw data, derived result,
experiment statistic, manuscript claim, or source file was changed.

Validation: the decision was checked against the current experiment summaries
and retained claim boundaries. `git diff --check` passes. No experiment or
software test was run because this is a documentation-only closure decision.
This record is included in the user's requested publication to `origin/main`.

## 2026-08-21 Measurement-Primary Manuscript Revision

Complete. The manuscript now presents the controlled real-vehicle communication
study as its primary scientific contribution. IPI is the enabling systems
contribution that represents compact CV/ITS messages and correlated CAV
operations consistently across the measured paths. The abstract and
introduction list exactly two contributions, while the abstract, introduction,
and results retain exactly three empirical insights.

The protocol argument now distinguishes the application programming interface,
the transport-independent wire protocol, the reference runtime, and the lower-
layer bindings. IPI exposes one application contract through a compact J2735
message profile and a correlated CAV-operation profile; path-specific bindings
carry the resulting frames over PC5 custom data, TCP, MQTT, or UDP. Section 4
states that the July 3--4 PC5 probes are complete serialized `IPI_RTT1` frames,
that their sequence field resides inside the IPI performance frame, and that
the 4,080-B vendor limit applies to the complete serialized packet. The private-
5G workloads retain their separate application-object size boundary. Section
5.1 reports measured representation cost without conflating either boundary.

Related work now identifies the comparable-measurement gap before the enabling
application-contract gap and credits the closest cross-path field studies. The
IPI novelty claim is limited to the implemented combination supported by the
survey. The results progress from representation and path-specific limits to
direction, configuration, sustained load, concurrent demand, recovery, and the
three stakeholder-facing insights. Future directions and the conclusion follow
from those measured boundaries. Stale reviewer-facing language, the TCP analogy,
the inaccurate common-envelope model, and semantic-completion disclaimers were
removed. Figure 2 and Figure 4 were flattened at publication resolution to
remove obsolete hidden text and unembedded figure fonts while preserving their
visible content.

Updated manuscript artifacts include all eight section files under
`paper/current_manscript/sections/`, the manuscript Makefile, the affected
figure scripts and generated figures, `paper/current_manscript/main.pdf`,
`paper/paper_outline.md`, and `paper/general_academic_writing_lessons.md`.

Validation: all 14 targeted C++ protocol and integration tests pass; both
modified Python figure scripts pass `py_compile`; and the complete LaTeX/BibTeX
build succeeds. The 24-page US-letter PDF was rendered and visually inspected
page by page, including full-resolution checks of every changed page. The final
log contains no overfull box, undefined citation, undefined reference, rerun
request, LaTeX error, emergency stop, or fatal error. The PDF contains zero
annotations, hyperlinks, JavaScript actions, or embedded files; all listed fonts
are embedded; its metadata contains no author identity; and its file size is
2,689,182 bytes. SHA-256 is
`006404f45e57492cdc4b84d1abf1b83d1478d7cef066208d5241cfe8468e3ff7`.
`git diff --check` passes. No page-count optimization was performed, per the
user's instruction. The user subsequently authorized publishing this complete,
related revision directly to `main`; this task record is included in that
publication commit.

## 2026-08-21 Table 4 Size-Boundary Correction

Complete. A source-level audit found that the previous Table 4 combined two
different size definitions. Its May 13 private-5G rows measure a CAV application
object before IPI serialization and the resulting encoded IPI frame. In
contrast, the July 3--4 PC5 labels already specify the complete serialized
`IPI_RTT1` performance frame submitted to the vendor packet-data interface.
The former table incorrectly treated the July 512-B and 2,048-B frame labels as
application objects and added inferred IPI bytes to them.

Table 4 now reports only measured IPI representation costs. A preserved J2735
message adds 5 B. The retained CAV planning records contain 1,000 TCP sender
frames at each measured object size: a request without an object is 63 B, while
256-B, 1-KiB, and 4-KiB objects produce 334--337-B, 1,102--1,105-B, and
4,174--4,177-B encoded frames. The unsupported 512-B and 2,048-B representation
rows were removed.

The PC5 method and results now use their independent boundary. Across 26,473
retained nonzero July sender rows, `payload_bytes` and `packet_bytes` both equal
the configured 256-, 512-, 1,024-, or 2,048-B complete frame size. The zero
control produces a 51--55-B minimum request frame. Section 4 identifies the
origin and sequence fields that match each response. Figure 4 now labels the
return as a sequence-correlated response with the configured frame size.
Section 5, Figure 6, Figure 14, Appendix B, `experiment_summary.md`, and
`paper/analysis/v2x_experiment_summary.md` use the same distinction between a
PC5 complete-frame size and a private-5G application-object size.

Validation: the independent CSV audit reproduces every Table 4 CAV range and
finds zero packet-length mismatches in all 26,473 retained nonzero PC5 sender
rows. The figure scripts pass `py_compile`; the complete LaTeX/BibTeX build
succeeds at 23 US-letter pages; and the final log contains no overfull box,
undefined citation, undefined reference, rerun request, compilation error, or
fatal error. Pages 6, 9, 10, and 15 were rendered at high resolution and
inspected. Figure 4, Table 4, Figure 6, and Figure 14 are readable without
clipping or overlap. The clean PDF contains zero annotations and has SHA-256
`96cf91b64f91e0409fd5410d21a62968a484d2c50515a144e45435c29f4ce0a3`.
The annotated review PDF remains unchanged with all 22 annotation objects. No
page-limit work was performed. No commit or push was performed.

## 2026-08-21 Complete Annotated-PDF Revision

Complete. A page-by-page annotation-object audit of
`paper/current_manscript/main_annotated.pdf` found 22 objects: ten highlights
with explicit revision comments, one empty highlight without comment text or an
identifiable visual target, and eleven associated popup objects. All ten
actionable comments were implemented, including changes to dependent captions,
section text, appendix evidence, and result claims rather than only the marked
locations.

The revision makes Figure 5 a compact single-column, side-by-side signal
survey; restricts Section 5.2 and Figure 6 to stationary PC5 experiments; uses
`timeout` consistently; and standardizes Figure 6 marker sizes. Figure 8's
y-axis is no longer clipped. Figure 11 is now a compact three-panel TDD
comparison, and Figure 12's legend no longer overlaps its data. Figure 13's
caption states the 1-KiB application payload. Figure 14 now compares p50, p95,
and maximum latency against each application deadline without overlapping
content. The conclusion was rewritten as two connected paragraphs occupying
approximately one quarter of its page. The complete PC5 route results removed
from the stationary main subsection are retained in Appendix D. A generalized
lesson on auditing and propagating review comments was added to
`paper/general_academic_writing_lessons.md`.

Updated files include:

- `paper/current_manscript/scripts/build_section4_signal_figure.py`
- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- the regenerated Figure 5--14 measurement PDFs under
  `paper/current_manscript/figs/`
- `paper/current_manscript/main.pdf`
- `paper/general_academic_writing_lessons.md`
- `current_task.md`

Validation: both figure-generation scripts pass `py_compile`; the complete
LaTeX/BibTeX build succeeds at 23 US-letter pages; and the build log contains no
undefined citation, undefined reference, rerun request, compilation error,
fatal error, or overfull box. All 23 pages were rendered and visually inspected,
with full-size checks of every revised figure and the conclusion. The clean PDF
contains zero annotations and has SHA-256
`9f965bd27c716b4e5621f3f41ece79e53bee5f42a1f28658db8178402ef4500c`.
The annotated review PDF remains unchanged with all 22 annotation objects. No
page-limit work was performed. No commit or push was performed.

## 2026-08-21 July PC5/Newer-IPI Consistency Revision

Complete. The manuscript now records the user's authoritative correction that
the July 3--4 PC5 campaigns used the newer serialized IPI format. Section 3
separates the IPI programming interface, wire protocol, and lower-layer
bindings. Section 4 identifies the vendor packet-data interface and packet type
`0x1b` as the binding beneath IPI, describes the decoded request and correlated
IPI response, and states that PC5 and Uu both carry IPI at the application
layer. Figure 4 replaces the old echo terminology with IPI request/response
labels and contains no hidden legacy text.

This pass initially interpreted the July PC5 size labels as application-object
sizes. The later source-level audit recorded above supersedes that
interpretation: the labels are complete serialized IPI performance-frame sizes,
and Table 4 now keeps the PC5 experiment separate from the measured IPI
representation-cost rows. The numerical July 3--4 PC5 completion and RTT
results remain unchanged.

Updated files:

- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/figs/figure04_measurement_paths.pdf`
- `paper/current_manscript/figs/section5_pc5_payload_route.pdf`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: four targeted IPI tests pass (`private_session_transport`,
`j2735_ipi_regional_codec`, `session_wire_codec`, and `pc5_ipi_adapter`). The
full LaTeX/BibTeX build completes at 24 US-letter pages with no undefined
citation, undefined reference, rerun request, compilation error, or fatal
error. Pages 6 and 9--11 were rendered and inspected; Figure 4, Table 4, and
Figure 6 are readable without clipping or overlap. The PDF contains zero
annotations and has SHA-256
`d97056c53ce3a8da8dd40694dc5eeab4f2009648e9a5de5ef8ca177aa03f0422`.
The pre-existing 3.731-point vertical-box warning remains on Results page 16.
No page-limit work was performed. No commit or push was performed.

## 2026-08-20 Section 4.1 IPI Path-Binding Clarification

Complete. Section 4 and Section 4.1 now identify IPI as the application
interface used by the request/response experiments before introducing the
physical radios. The revised path description states that IPI defines each
application message and its request/response correlation. A path-specific
adapter then binds the serialized exchange to direct PC5 or network-assisted
Uu. The PC5 paragraph identifies the vendor J2735 and packet-data interfaces as
the radio binding beneath IPI, while the Uu paragraph identifies TCP, MQTT, and
UDP as transport bindings for IPI Cooperative Service frames. Figure 4's lead-in,
caption, accessible description, and the later RTT-boundary paragraph use the
same relationship. Separate exact 50-MiB transfers remain directional-goodput
controls rather than IPI request/response workloads.

Updated files:

- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: the full LaTeX/BibTeX build completes; the final log contains no
undefined citation, undefined reference, rerun request, compilation error, or
fatal error. Manuscript pages 4--7 were rendered and visually inspected; the
new text, Figures 3 and 4, captions, and Table 1 are readable without clipping
or overlap. A 3.731-point vertical-box warning remains on Results page 16, whose
render has no visible clipping or overlap. The generated figure PDFs were
restored after the build because no figure source or data changed and those
artifacts are outside this revision's scope. No page-limit work was performed.

## 2026-08-20 Main-Branch Consolidation

Complete. At the user's explicit request, the manuscript-reorganization history
was fast-forwarded into `main` without creating another branch or pull request.
The earlier `codex/paper-proposal-updates-20260820` branch was already identical
to `main`, so all work from both non-main branches is retained in the consolidated
history. The updated `main` was pushed to `origin`, after which both non-main
branches were deleted locally and from GitHub. `main` is the repository's only
remaining local and remote branch. The pre-existing Git stash was not changed.

Validation: the tracked worktree and index are clean; local `main`,
`origin/main`, and GitHub's `refs/heads/main` resolve to the same commit; and the
three manuscript-publication commits remain consecutive in `main`. The clean
23-page `paper/current_manscript/main.pdf` retains zero annotations and SHA-256
`129d5e2fc21bebb24ebaeed33dca2e88646fbdc664c9bd61750205c9d3e7a6d7`.
The ignored local `main_annotated.pdf` remains outside Git history with all 22
annotation objects intact. No page-limit work was performed.

## 2026-08-20 Manuscript Reorganization Publication

Complete. At the user's request, the current manuscript reorganization and
Figure 3 replacement were divided into separate commits on branch
`codex/manuscript-reorganization-figure3-20260820` and pushed to `origin`.

1. Commit `b305ef7d60e17e5e0903b8ddd72c85706cde2279` reorganizes the active
   manuscript as the self-contained `paper/current_manscript` draft, moves its
   current figures under the local `figs` directory, installs the new physical
   testbed as Figure 3, preserves the detailed request/response diagram as
   Figure 4, and publishes a clean source-consistent `main.pdf`.
2. Commit `d81182dc91cf3ca82717108480e237de9bb035e6` updates repository
   documentation, review links, figure-generation paths, and targeted ignore
   rules for the new current and legacy draft structure.
3. This task record is the final documentation commit for the publication.

Validation: all four affected Python scripts pass `py_compile`; all 14 current
`includegraphics` paths resolve; and a complete LaTeX/BibTeX build succeeds at
23 US-letter pages without missing figures, overfull boxes, undefined citations
or references, or a rerun warning. The published
`paper/current_manscript/main.pdf` contains zero annotations and has SHA-256
`129d5e2fc21bebb24ebaeed33dca2e88646fbdc664c9bd61750205c9d3e7a6d7`.
The ignored local `main_annotated.pdf` retains all 22 annotation objects and
SHA-256
`3ce0c27e461576fe38f3cdddca6897516e883254697187876f01f2e01f056d6d`.
No LaTeX auxiliary file, cache, temporary render, editor-held file, or annotated
PDF appears in the published commit range. No page-limit work was performed.

## 2026-08-20 Figure 3 Physical-Testbed Replacement

Complete. The supplied
`edge4av_figure3_physical_testbed_hybrid_editable_boxed_v2.pptx` artwork now
replaces the former inline topology diagram used as Figure 3. The matching
one-page PDF export was renamed to
`paper/current_manscript/figs/figure03_physical_testbed.pdf`. The existing
detailed request/response-path asset remains Figure 4 and was renamed to
`paper/current_manscript/figs/figure04_measurement_paths.pdf` so that both asset
names match their manuscript roles.

`paper/current_manscript/sections/04_system_design_setup.tex` now includes the
new Figure 3 PDF and describes its vehicle installation, fixed infrastructure,
and deployment map. The surrounding testbed paragraph, caption, and accessible
description were revised to match the new artwork. Figure 4's include path and
the current Makefile dependencies were updated for the two descriptive asset
names.

Validation: a separate three-pass LaTeX/BibTeX build completed at 23 US-letter
pages. Figures 3 and 4 resolve as Figures 3 and 4 on pages 5 and 6. Full-page
renders of pages 4--6 show readable labels and no clipping or overlap. The log
contains no overfull boxes, missing figures, undefined citations or references,
or rerun warning. Existing underfull-box warnings remain. The annotated review
PDF was preserved unchanged. A clean `main.pdf` was later rebuilt for
publication, as recorded in the publication entry above. No page-limit work was
performed.

## 2026-08-20 Self-Contained Draft Directory Reorganization

Complete. The user replaced the root-level figure-tree design with two
self-contained draft directories. `paper/current_manscript` now contains all
current TeX sources, build scripts, manuscript PDFs, and its local `figs`
directory. The 15 current figure PDFs include the 14 figures referenced by the
current TeX and the additional directional-workload figure produced by the
current generator. `paper/legacy_draft` retains the old TeX draft and its local
`figs` directory. The 54 old or unused assets remain grouped as concepts, maps,
and measurements inside that directory. The temporary root-level
`paper/figures` tree and the former `paper/manuscript` directory no longer
exist.

All current and legacy TeX image paths resolve within their own draft
directories. The current Makefile, both current figure generators, two older
root-level figure scripts, repository documentation, historical review paths,
and targeted `.gitignore` exceptions use the new locations. The current draft
sources and figures can be versioned normally. The old figure collection
remains ignored unless explicitly selected for publication.

Validation: all 13 current and eight legacy `includegraphics` paths exist. The
four affected Python scripts pass `py_compile`, and both current figure
generators completed in `paper/current_manscript/figs`. A separate current-draft
build completed at 23 US-letter pages with no missing figures, overfull boxes,
undefined citations or references, or rerun warning. The retained old draft
also built successfully at 14 pages, and its supplementary appendix built at
five pages. The appendix retains its pre-existing 20.33-point overfull vertical
box; the directory move introduced no missing asset or new build failure. All
three validation PDFs were rendered and visually inspected without missing,
clipped, or overlapping figures.

At the time of the directory move, the two user-annotated current-draft PDFs
were preserved without rebuilding. The later publication step rebuilt a clean
`main.pdf` while retaining the ignored `main_annotated.pdf` unchanged. No
page-limit work was performed.

## 2026-08-20 Raw Signal Artifact Publication and Main Merge

Complete. At the user's explicit request, the remaining August 18--19
G-NetTrack exports, validation collections, interactive maps, offline assets,
and OpenStreetMap tile cache were retained in a separate data commit after the
paper and proposal commits. The publication adds 420 previously untracked
artifacts totaling 98,636,075 bytes. The largest individual file is 1,970,704
bytes, below GitHub's normal blob limit.

The pre-publication scan found no API keys, passwords, bearer tokens, private
keys, or credential-bearing URLs. The vendor schemas contain IMEI, IMSI, and
MSISDN field names, but no populated numeric values for those fields were
detected. The exports do contain the expected exact GNSS route records. They
were published following the user's explicit instruction to commit and merge
the remaining files into the public repository.

The feature branch was pushed after the data commit. Local `main` was then
fast-forwarded to the complete feature-branch history and pushed to
`origin/main`. The proposal, paper, and raw-data scopes remain separate
commits. Local `main`, `origin/main`, and the feature branch resolve to the
same final commit, and the tracked worktree and index are clean.

## 2026-08-20 Paper and Proposal Publication

Complete. The current paper and proposal work was separated on branch
`codex/paper-proposal-updates-20260820`. Proposal commit `1ba2ef7` contains
only the revised Year 2 implementation report and its new 10-page version.
The following paper commit contains the current manuscript source and PDFs,
the retained 28-page comparison PDF, required figure assets and generators,
paper analysis and review records, aggregated signal-map inputs, and the
tracked experiment-evidence records and analysis scripts used by the paper.
Temporary renders, LaTeX auxiliary files, hidden editor files, and the raw
G-NetTrack export tree remained outside the first two commits. The initial
branch push kept the paper and proposal scopes separate.

## 2026-08-20 Detailed Experiment Appendix Revision

Complete. The Results section now keeps the argument-critical comparisons in
the main manuscript and places the complete numerical accounts for prose-only
experimental conditions in a new Appendix D. The appendix preserves four
groups of detailed evidence: field-specific uplink payload and transport
results, gateway and TDD replications, sustained-stream load and packet
marking, and concurrent-client behavior. Experiments already represented by a
dedicated result figure or table remain in the main paper.

The main manuscript does not direct readers to the appendix. The previous
setup-section appendix reference was removed, and a source audit across the
abstract through conclusion finds no appendix reference or appendix label.
The main results remain self-contained; Appendix D provides the fuller record
for readers who choose to examine material beyond the core paper.

Validation: the manuscript rebuild completes at 23 US-letter pages. The final
log has no overfull boxes, undefined citations or references, or rerun warning.
Results pages 10--17 and appendix pages 20--23 were rendered and inspected;
the revised prose and appendix have no clipping or overlap. The final PDF
contains zero annotations. Its size is 1,028,884 bytes and its SHA-256 is
`756773c116d53ba1e8a066dfadb85e6323762a02875f3b62095e66ea5a86c64e`.
`git diff --check` passes. No page-limit work was performed. No commit or push
was performed.

## 2026-08-20 Manuscript Unit Normalization

Complete. The manuscript now defines and applies one dimensionally consistent
unit convention. Protocol fields and serialization additions use bytes (B).
Application payloads and transferred objects use kibibytes (KiB) or mebibytes
(MiB). Link rates and application goodput use the decimal bit-rate units
kbit/s, Mbit/s, or Gbit/s. Ambiguous forms such as KB, MB, kbps, and Mbps do
not appear in the manuscript.

The PC5 and transport figures now express every payload tick in KiB rather
than mixing B and KiB on one axis. The 60,000-B stress object is correctly
reported as 58.6 KiB, the exact directional transfer is consistently reported
as 50 MiB, and the application-requirements table uses KiB for finite object
sizes. The IPI packaging table uses B throughout because it compares exact
serialized frame additions. The deadline-completion function now uses
$C(D)$, removing the previous conflict between the deadline symbol and B for
bytes. The Makefile also tracks the appendix source so appendix-only edits
trigger a manuscript rebuild.

Validation: both figure scripts pass `py_compile`; all affected figures were
regenerated. The manuscript rebuild completes at 23 US-letter pages. The final
log has no overfull boxes, undefined citations or references, or rerun warning.
Pages 3--17 and 21--23 were rendered and inspected; the unit definitions,
tables, axes, captions, results, and appendices have no clipping or overlap.
The final PDF contains zero annotations. Its size is 1,026,104 bytes and its
SHA-256 is
`bd2740ba5bfd122597adf6e2b8ae2e874a554ab4221a08c155ec94b695d239a6`.
The source and extracted-PDF audits find no KB, MB, kbps, Mbps, stale
60-KiB label, or stale $C(B)$ notation. `git diff --check` passes. No overall
page-limit work was performed. No commit or push was performed.

## 2026-08-20 Second Annotated-Manuscript Revision

Complete. All 12 new substantive comments embedded in
`paper/current_manscript/main.pdf` were treated as revision instructions, including
their recurring implications elsewhere in the manuscript. The Introduction is
now 490 source words and the Related Work section is 647 source words. Related
Work plus Figure 1 occupies the requested approximately one-and-one-half-page
span. The protocol, measurement, and field-evidence gaps remain intact after
condensation.

Figure 5 now places the retained RSRP and SINR OpenStreetMap panels side by side
in a shorter two-column figure. The application-traffic schematic and every
reference to it were removed. The PC5 result now uses a line plot for stationary
payload completion, labels P1 as the building-obstructed NLOS point, and
describes the driven collections as route-coverage evidence rather than a
mobility effect. The transport-completion figure is also a line plot, with
unevaluated transport-size pairs preserved as gaps.

The Results section adds a dedicated figure for detector-sized uplink behavior
across four stationary RSRP conditions and a separate subsection and figure for
downlink-heavy CAV objects. The downlink subsection defines the compact vehicle
request and validated edge-to-vehicle return of sensor, perception, planning,
control, or other application state. The following subsection now focuses on
TDD and gateway effects. The sustained-load and packet-marking figure is
single-column. The client-demand/interruption figure exposes milliseconds,
seconds, counts, and percentages directly in the axes and cells. The final
cross-application figure no longer uses a latency-derived V2X signal score; it
places each measured response p95 and application deadline on one millisecond
axis, with the connector showing whether the measured tail meets or misses the
deadline.

`paper/general_academic_writing_lessons.md` now records two generalized rules:
choose a visual encoding that matches the reader's comparison, and do not treat
the data-collection method as the measured cause of a result. The manuscript
Makefile now regenerates all Section 5 figures from the plotting script.

Validation: both plotting scripts pass `py_compile`; all revised figures were
regenerated and inspected both independently and at manuscript scale. The
manuscript rebuild completes at 23 US-letter pages. The final log has no
overfull boxes, undefined citations or references, or rerun warning. Pages
1--3, 7, and 10--16 were rendered and inspected; the revised section lengths,
side-by-side maps, line plots, single-column figure, unit labels, and deadline
comparison have no clipping or overlap. The rebuilt PDF contains zero
annotations. Its size is 1,025,724 bytes and its SHA-256 is
`6e40922e6bc9df465e4d327874dd015d4a037bdce8a25cba6f37d3ee6284c1c5`.
`git diff --check` passes. No overall page-limit condensation was performed.
No commit or push was performed.

## 2026-08-20 Annotated-Manuscript Revision

Complete. All 19 substantive comments embedded in
`paper/current_manscript/main.pdf` were treated as revision instructions. Section 4
now introduces application workloads by payload, direction, duration, and
deadline. Emergency-vehicle signal priority uses the current SRM-to-SSM
interaction, remote assistance replaces vague CAV retrieval terminology, and
the workload families are presented as a short numbered structure. Table 1 now
maps experimental controls to their analytical roles without acquisition dates.
Tables 2 and 3 use shorter, centered comparisons with explicit traffic
directions.

Figure 5 now reformats the retained OpenStreetMap RSRP/SINR maps as one
single-column figure with readable paper-sized scales. Figure 7 uses P1--P5,
distance, and a separate 7-m reference label instead of qualitative PC5 signal
names. Figure 8 removes request counts from the plot; the text reports 752,200
issued requests across its eight disjoint groups. Figure 9 now reads the raw
sender CSVs, separates TCP, MQTT, raw UDP, and fragmented UDP, and marks
unevaluated transport-payload cells explicitly. This removes the previous
unmeasured 1,400-B TCP/MQTT value. Figure 13 now precedes the Summary and
Insights subsection and no longer splits an insight.

Section 5 is ordered from packaging and PC5 limits through 5G uplink payload,
transport, and signal comparisons; the bidirectional TDD/gateway subsection
then identifies how the MG52 and GL-X3000 collections test device-related
instability before the load, client-demand, and interruption results. A current
SAE J2735 reference was added for SRM/SSM roles. General lessons on table
purpose, concrete application interactions, analytical aggregation, unmeasured
visual cells, and replication logic were added to
`paper/general_academic_writing_lessons.md`.

Validation: both figure scripts pass `py_compile`; all affected figures were
regenerated and visually inspected at manuscript scale. The manuscript rebuild
completes at 24 US-letter pages. The final log has no overfull boxes, undefined
citations or references, or rerun warning. Pages 7--18 were rendered and
inspected; the revised tables, figures, captions, and Figure 13 placement have
no clipping or overlap. The rebuilt PDF contains zero annotations. Its size is
1,043,647 bytes and its SHA-256 is
`00c5985e4850465dcb9074b24b7aef1be272e4f3e1381d689a37010661848a9e`.
No page-limit work was performed. No commit or push was performed.

## 2026-08-20 Continuous-Drive RSRP/SINR Manuscript Revision

Complete. The previously downloaded Google Drive folder was verified as the
source of the updated radio maps. Figure 5 now uses the two August 19
OpenStreetMap figures for RSRP and recovered NR SINR instead of the earlier
16-point RSRP/RSRQ/SNR graphic. The manuscript identifies the measurement
device as a Samsung 22 handset running G-NetTrack Pro and describes the collection
as three continuous drive segments through the testbed.

The combined trace contains 1,301 GNSS records. The preparation removes 123
records from a frozen radio-value interval longer than five seconds and retains
1,178 valid band-48 records. RSRP ranges from -121 to -88 dBm with a median of
-108 dBm. Recovered NR SINR ranges from -20 to 30 dB with a median of 9 dB.
The temporal sample distribution is 7.0% strong, 28.9% common/typical, and
64.1% weak under the manuscript's three-bin RSRP scale. The ordinary
G-NetTrack Pro SNR field is empty; the SINR figure uses timestamped NR
`ssSinr` values recovered from the verbose logs and aligned to GNSS records.

Section 4, Table 1, the Figure 5 caption and description, the results figure
description, Appendix B, Table 6, Table 7, and the manuscript Makefile now use
the continuous-drive collection and updated source figures. The application
campaigns remain stationary workload experiments with their own signal
records; the Samsung 22 drive survey supplies route-level testbed coverage.
`experiment_summary.md` marks the earlier sparse signal map as superseded for
current manuscript use.

Validation: the manuscript rebuild completes at 24 US-letter pages. The log has
no overfull boxes, undefined citations or references, or rerun warning. Pages
7--9 and 21--24 were rendered and visually inspected. The RSRP and SINR maps,
caption, deployment table, appendix text, and detailed configuration table have
no clipping or overlap. The final PDF is 3,339,161 bytes with SHA-256
`7774c9ce19b52a46fdc0cfd34079825a8a3bd6985cb42b8f05a25eaf624acedf`.
No page-limit work was performed. No commit or push was performed.

## 2026-08-20 Testbed Figure Order Revision

Complete. Section 4 now introduces the experimental setup from overview to
detail. Figure 3 presents the testbed topology and identifies the vehicle,
PC5, Uu, roadside/radio, private-5G, routing, and d1 equipment. Figure 4 then
expands those components into the direct-PC5 and network-assisted-Uu request,
response, and sender-side RTT paths. The existing signal survey and
application-traffic figures consequently become Figures 5 and 6.

The surrounding text follows the same order. The subsection first locates the
equipment, then describes the PC5 and Uu implementations, and finally defines
the data direction, response type, validation, and RTT boundary for each path.
The later timing paragraph now cites Figure 4 for request/response events and
Figure 6 for the communication resources within the paths. The manuscript
Makefile tracks the detailed data-path PDF as an input dependency.

Validation: the manuscript rebuild completes at 24 US-letter pages. The log has
no overfull boxes, undefined citations or references, or rerun warning. Pages
5--10 were rendered and visually inspected; Figures 3 and 4 appear in the
requested order on page 6 without clipping or overlap, and Figures 5 and 6 are
correctly renumbered. The final PDF is 3,096,905 bytes with SHA-256
`5b5d67fd90fc537fe84c19b73035aef619ef908f8fa2f20630e4595b35f99414`.
No page-limit work was performed. No commit or push was performed.

## 2026-08-20 Abstract Length Revision

Complete. The abstract was reduced from approximately 399 to 285 words. Its
rendered text now occupies the left column below the title, or approximately
one-third of the full first page, within the requested one-quarter-to-one-half
page range.

The revision preserves the binding argument order: 1) the readiness question;
2) the distinction between compact, stateless CV/ITS messages and correlated,
stateful CAV operations; 3) the common-interface and cross-path measurement
gaps; 4) the two contributions; and 5) exactly three numbered insights. The
measurement contribution receives more detail than IPI. It retains the real
vehicle, both communication paths, the sole physical CAV UE, dedicated radio,
clean 40-MHz n48 channel, controlled demand, varied workload conditions, and
the three application-level metrics. Detailed experimental results remain in
Section 5 rather than being repeated in the abstract.

Validation: the manuscript rebuild completes at 23 US-letter pages. The log has
no overfull boxes, undefined citations or references, or rerun warning. Pages 1
and 2 were rendered and visually inspected; the shorter abstract, Introduction
opening, contribution list, insights, and transition to Related Work have no
clipping or overlap. The final PDF is 3,009,876 bytes with SHA-256
`d634718b64fa9bbe2b91d5532a8273f2b26122cf5199a29bf74119447601111f`.
No other page-limit work was performed. No commit or push was performed.

## 2026-08-20 Related Work and Figure 1 Revision

Complete. Section 2 now uses one argument across standards, CAV systems, and
field measurements. The standards discussion distinguishes message, service,
edge, architecture, and deployment layers before identifying the specific
missing application contract. The CAV-systems discussion connects exchanged
representations to complete-object deadlines and distinguishes IPI's domain
contract from Tentacles-style network selection. The field-evidence discussion
credits application-level PC5, commercial-5G, teleoperation, automotive MEC,
TDD, and cross-path corridor studies before isolating the remaining comparison
gap.

The Figure 1 caption and every Section 2 reference to the figure now follow its
four rows: system integration, application timing, communication capability,
and operational robustness. The caption explains the current-state, gap, and
longer-term-capability columns and the meanings of the `Paper addresses`,
`Field corroborated`, and `Field quantified` labels. The closing synthesis maps
IPI to the protocol/shared-semantics gap and maps the PC5/Uu study to the
cross-path evidence/support-boundary gap. It also preserves the experiment
boundary: the PC5 path uses J2735 functional checks and payload-varied direct
responses, while the Uu path carries IPI workload frames.

The transition into Section 3 now starts from the two application-contract
requirements established in Related Work. Primary standards and the closest
system and field baselines were checked before the rewrite. No page-limit
condensation was performed.

Validation: the manuscript rebuild completes at 23 pages on US letter paper.
The log has no overfull boxes, undefined citations or references, or rerun
warning. Pages 2--4 were rendered and visually inspected; Figure 1, its revised
caption, both text columns, and the transition into Section 3 have no clipping
or overlap. The final PDF is 3,010,501 bytes with SHA-256
`3f2c2da9a69c8d4e1930d286d929fdf35ccc0afcb876a3620398e4446aa198bd`.
No commit or push was performed.

## 2026-08-20 CAV Traffic and Radio-Provider Results Revision

Complete. The manuscript now distinguishes finite, event-triggered CAV bursts
from sustained directional streams such as remote-teleoperation video and
control traffic. It also evaluates the case in which deadline-sensitive
exchanges begin while a sustained stream remains active. Insight 3 now asks
5G/6G radio vendors and mobile network operators to support event-triggered
bursts, sustained streams, and concurrent deadline-sensitive traffic through
stable radio configurations and scheduling based on direction, deadline, and
duration.

The results section incorporates the August 19 same-device GL-X3000 comparison
without pooling directions, transports, payload sizes, or TDD profiles. The
four-panel Figure 9 overlays TCP and MQTT within separate uplink-heavy and
downlink-heavy p95 RTT panels at 1, 10, 100, and 500 KiB, followed by exact
50-MiB upload and download goodput. Color identifies the TDD profile, while
marker and line style identify the transport. This layout reduces the generated
figure height by 26.6% without pooling or removing measurements. The text reports all 16,000
completed exchanges, deadline-miss rates,
the 500-KiB response tails, common RSRP/SINR support, the exact-transfer rates,
and the closest MG52 control. Device-associated differences are reported
separately from the same-device TDD comparison.

The abstract, introduction, setup, results, future research directions,
conclusion, and `agent_context.md` use the same traffic distinction and Insight
3 scope. The application table now describes remote recovery as an
event-triggered session with a sustained uplink video stream and returned
control or path data. No page-limit condensation was performed.

Validation: the figure generator passes `py_compile` and completes. Independent
CSV checks reproduce the 14/16 p50 and 15/16 p95 orderings, the 100/500/1,000-ms
miss rates, all exact-transfer means, and the 30.8%/31.0% GL-X3000-to-MG52
goodput differences. The manuscript rebuild completes at 23 pages. The log has
no overfull boxes, undefined references, or rerun warning. The revised pages
and all-page contact sheet were rendered and visually inspected; the new figure
and table have no clipping or overlap. Staged and unstaged whitespace checks
pass. No commit or push was performed.

## 2026-08-19 Latest Experiment Data Pull

Complete. The local `main` branch was fast-forwarded from
`a1e006909de0e89945993356bb3798a545974f02` to
`8cc0818a39873a75c4172f6e80e4e83aef0bf006`, matching `origin/main`. The
incoming commit is `Add August 19 GL-X3000 5G experiment results`. It adds 899
files with 92,401 insertions and 35 deletions.

The incoming evidence contains separate August 19 `70/20/10` and `40/40/20`
stationary radio/GNSS records, TCP and MQTT uplink-heavy and downlink-heavy
payload sweeps, exact 50-MiB bidirectional bulk-transfer repetitions, and two
repository-facing directional-repeat summaries. The pull also adds the
GL-X3000 modem collector and updates the experiment documentation and uplink
runner. No result interpretation or manuscript revision was performed as part
of this synchronization.

The pull used `git pull --rebase --autostash origin main`. Restoring the local
work produced conflicts in `current_task.md` and `experiment_summary.md`
because both the incoming commit and the local work added August 19 records at
the same insertion points. Both records were retained, the experiment entries
were numbered E19--E22, and no raw artifact was modified. The exact pre-pull
staging boundary was restored: five previously staged paths remain staged,
three previously unstaged paths remain unstaged, and the three previously mixed
paths remain mixed. The conflict-preserving autostash remains available with
the two earlier autostashes; none was dropped. The untracked G-NetTrack import
remains intact at 427 files and 102,683,139 bytes.

Validation: all ten incoming `SHA256SUMS` manifests verify, covering 884
manifest entries. All 116 incoming JSON files and all 93 incoming JSONL files
parse. The three incoming Python files parse successfully, and the changed
shell runner passes `bash -n`. Git reports no unresolved path, local `HEAD`
equals `origin/main`, and the staged and unstaged whitespace checks pass. No
commit or push was performed.

## 2026-08-19 GL-X3000 Same-Device TDD Analysis

Complete. The August 19 GL-X3000 data were analyzed as a same-device,
same-location comparison of the deployed `70/20/10` and `40/40/20` profiles.
The comparison keeps uplink, downlink, TCP, MQTT, and the 1-, 10-, 100-, and
500-KiB application payloads separate. Each of the 32 profile/direction/
transport/payload conditions contains exactly 500 validated exchanges, for
16,000 final attempts. All attempts completed; the result concerns latency,
deadline completion, and goodput rather than failures.

The deployed `40/40/20` configuration has higher p50 RTT in 14 of 16 matched
workload cells and higher p95 RTT in 15 of 16. The only practical tie is the
1-KiB uplink. At 10 KiB and above, `40/40/20` has higher p50 and p95 for both
transports and both directions. The uplink effect grows sharply with payload:

| Workload | `70/20/10` p50 / p95 | `40/40/20` p50 / p95 | `40/40/20` ratio |
|---|---:|---:|---:|
| 500-KiB TCP uplink | 231.046 / 258.200 ms | 609.652 / 1,149.368 ms | 2.64x / 4.45x |
| 500-KiB MQTT uplink | 238.607 / 267.336 ms | 580.942 / 1,104.331 ms | 2.44x / 4.13x |
| 500-KiB TCP downlink | 77.744 / 93.403 ms | 101.529 / 320.891 ms | 1.31x / 3.44x |
| 500-KiB MQTT downlink | 80.493 / 97.806 ms | 86.417 / 138.168 ms | 1.07x / 1.41x |

Across the equally sized workload matrix, the 100-ms deadline-miss rate rises
from 14.537% under `70/20/10` to 30.850% under `40/40/20`. At 500 ms it rises
from 0.038% to 9.812%, and at one second it rises from zero to 1.262%.
`40/40/20` produces 101 second-scale exchanges, including 82 in the two
500-KiB uplink conditions; `70/20/10` produces none. Downlink also contains
rare second-scale events under `40/40/20`, including a 4.051-second TCP
500-KiB response and a 3.831-second MQTT 1-KiB response. The compact MQTT
outlier shows that these events are not explained by large-object
serialization alone.

The exact 50-MiB transfers agree with the latency result. Mean vehicle-to-d1
goodput is 15.691 Mb/s under `70/20/10` and 10.913 Mb/s under `40/40/20`, a
30.4% reduction. Mean d1-to-vehicle goodput is 166.106 versus 123.451 Mb/s, a
25.7% reduction. All three individual `40/40/20` samples are below all three
`70/20/10` samples in both directions. Short-run variability is also higher:
the upload coefficient of variation rises from 0.009 to 0.250, and download
rises from 0.077 to 0.230. With only three repetitions, these values remain
descriptive rather than a population-level capacity estimate.

All 16,000 application attempts were joined to the nearest GL-X3000 radio
sample within six seconds. The p95 join distance is 4.76 seconds for both
profiles. `40/40/20` has RSRP `-102` to `-93 dBm` with median `-98 dBm`, while
`70/20/10` has `-104` to `-95 dBm` with median `-99 dBm`. Both have median
SINR 20 dB and RSRQ `-10 dB`; the complete SINR ranges are 16--24 and
15--23 dB, respectively. Restricting the comparison to the common RSRP/SINR
support retains 15,153 attempts and the same 14-of-16 p50 and 15-of-16 p95
directions. Thus, the overall `40/40/20` disadvantage is not explained by
weaker measured RSRP or SINR. The stored modem metric is NR SINR, not plain
SNR, and its ten-second cadence cannot exclude shorter channel events.

Server processing and downlink payload validation remain small and similar
between profiles. At 500-KiB TCP uplink, for example, server-processing p50 is
1.617 ms under `70/20/10` and 1.619 ms under `40/40/20`, while application RTT
p50 changes from 231.046 to 609.652 ms. Processing therefore does not explain
the communication-latency difference. The dedicated clean-band, sole-UE setup
also excludes background-user contention as the cause of the cross-profile
change.

The device-replacement result is consequential but not hardware-causal. Under
`40/40/20`, the previous August 18 500-KiB uplink p50 values were approximately
7.65--7.86 seconds; the GL-X3000 values are 0.581--0.610 seconds. Available
mean upload goodput rises from 0.515 to 10.913 Mb/s, a 21.19x ratio. Therefore,
the catastrophic August 18 condition did not persist after replacement.
However, August 18 also had substantially weaker reported RSRP, from -105 to
-115 dBm, and fewer complete bandwidth repetitions. The evidence does not
isolate hardware from signal, session, or path state. The same-device August
19 comparison nevertheless shows that the previous gateway cannot be the sole
cause, because `40/40/20` still underperforms `70/20/10` on the GL-X3000.

Several observations bound the TDD interpretation. First, `70/20/10` was
collected before `40/40/20`; the order was not counterbalanced. Second, the
`40/40/20` application run was paused after the first 1-KiB TCP condition and
an excluded 142-exchange partial 10-KiB condition, then resumed approximately
2 hours and 20 minutes later. Third, the profiles are operator reported; fresh
serving-cell, handoff, MCS, PRB, and DU-counter evidence is unavailable. The
vehicle address also changes from `10.120.121.23` to `10.120.121.20`, which is
consistent with a new session or reattachment. Finally, the vehicle `eno2`
interface accumulates 354 receive errors and 185 frame-error increments during
four `40/40/20` downlink conditions, versus nine and nine under `70/20/10`.
These local-link errors may explain part of the downlink tail. They do not
explain the uplink penalty or the exact-transfer result, because no interface
error increment occurs during either uplink application matrix or either
bulk-transfer stage.

Conclusion: **the deployed `40/40/20` configuration performs worse than
`70/20/10` in the August 19 same-device experiment, especially for larger
uplink objects, but the data do not establish that its nominal frame allocation
is inherently or universally worse.** Because `40/40/20` nominally provides
more fixed uplink opportunity, its lower measured uplink performance points to
an implementation, scheduler, reconfiguration, interoperability, or session-
state problem in the tested deployment. A causal TDD result requires balanced
profile alternation with identical stabilization and warm-up handling, fresh
profile and serving-cell verification, and block-level statistical analysis.

Validation: all ten incoming manifests verify; all 16,000 final sender and
receiver rows match in sequence, payload size, acceptance, and CRC32; all 44
derived application and bandwidth rows match their raw validation summaries;
and the independently recomputed p50, p95, p99, maximum, deadline, signal,
goodput, and interface-counter results agree with the stored artifacts. No
manuscript revision, commit, or push was performed.

## 2026-08-19 GL-X3000 Full Directional TDD 40/40/20 Repeat

Complete after an operator-requested pause and clean resume. The immediately
preceding replacement-device experiment was repeated after the operator changed
the TDD profile to `40/40/20`. The raw and derived artifacts remain separate
from the completed `70/20/10` run. A fresh 60-second ROS 2 GNSS bag and two
modem-signal collection segments document the stationary placement and radio
context.

Device identity is now explicit: the operator confirmed that the vehicle
private-5G gateway used for both August 19 runs is a GL.iNet `GL-X3000`. The
preceding campaign gateway was the Cisco Meraki `MG52-HW`; the August 18
per-run context did not repeat that model field, so its identity is tied to the
repository's campaign context and the operator-reported replacement sequence.

The unchanged application matrix contains TCP and MQTT uplink-heavy and
downlink-heavy conditions at 1, 10, 100, and 500 KiB, with exactly 500 complete
validated exchanges per condition. Uplink means vehicle to d1 followed by a
compact validated acknowledgment. Downlink means a compact vehicle request
followed by an exact d1-to-vehicle payload; RTT includes complete vehicle-side
length, sequence-number, and CRC32 validation. After the application matrix,
run three exact 50-MiB upload/download pairs. Upload is vehicle to d1; download
is d1 to vehicle. Payload bodies remain in memory, and payload-file HDD I/O is
excluded from every measured interval.

Fresh host/path preflight observes vehicle address `192.168.8.233` on `eno2`,
d1 routed through `192.168.8.1`, approximately 21-ms ICMP RTT, and d1's wall
clock approximately 188 seconds behind the vehicle. The new GNSS bag spans
59.628 seconds with 16,102 messages. Its 595 BESTPOS samples have 0.032 m
maximum radial spread and 0.0090 m/s maximum speed, and its median is 1.85 m
from the immediately preceding new-device `70/20/10` location reference; the
stationary `location_3` gate passes. Across the initial and resumed collection
segments, 382 unique ten-second 5G-SA samples cover 3,810 seconds of active
capture: RSRP `-102` to `-93 dBm` (median `-98 dBm`), RSRQ constant at
`-10 dB`, and SINR `16` to `24 dB` (median `20 dB`). Serving-cell identity,
cell administrative states, and handoff state remain unreported.

All local/remote protocol, source-hash, route, port, and Python 3.8 preflight
checks passed. The operator stopped the first uplink TCP 10-KiB attempt after
142 accepted exchanges. That partial capture remains a diagnostic and is
excluded from every final result; a clean replacement condition completed
500/500 after the resume. All 16 final application conditions contain exactly
500 accepted exchanges and zero failures: 4,000 uplink and 4,000 downlink.
Uplink p50 spans `39.220--609.652 ms` and p95 spans `49.225--1,149.368 ms`.
Downlink p50 spans `29.760--101.529 ms` and p95 spans `44.642--320.891 ms`.
Downlink validation p50 spans `0.043--1.335 ms` and p95 spans
`0.113--2.484 ms`; validation is included within RTT and does not explain the
observed multi-second tail events.

All three exact 50-MiB upload/download pairs passed endpoint byte-count and
CRC32 validation. Receiver-observed mean goodput is `10.913 Mb/s` upload
(vehicle to d1) and `123.451 Mb/s` download (d1 to vehicle). Relative to the
matched new-device `70/20/10` run, the observed means are `30.4%` and `25.7%`
lower, respectively. Uplink RTT is effectively tied at 1 KiB, but from 10 KiB
upward `40/40/20` has higher p50 and p95 for both transports. Every downlink
condition also has higher p50 and p95 under `40/40/20`; the differences are
small for compact responses but large in tail latency for several larger
conditions. Treat this as a matched, sequential within-deployment observation:
the bandwidth sample has only three repetitions, its `40/40/20` variation is
large, and the application matrix was split by an operator pause.

The same-profile comparison against the August 18 `40/40/20` run shows that
GL-X3000 uplink p50 remained effectively tied at 1 KiB, then was `63.7--69.2%`
lower at 10 KiB, `88.9--89.0%` lower at 100 KiB, and `92.2--92.4%` lower at
500 KiB across TCP and MQTT. Downlink is mixed rather than uniformly improved:
compact-response p50 is higher, 100-KiB p50 is tied or lower, and 500-KiB p50
is `27.4--30.5%` lower, while p95 changes range from `39.1%` lower to `39.9%`
higher. Available bandwidth samples are `21.19x` higher for upload and `2.89x`
higher for download with GL-X3000. This comparison is materially confounded:
August 18 used operator-reported RSRP from `-105` to `-115 dBm`, whereas the
GL-X3000 run measured median RSRP `-98 dBm`; the earlier bandwidth stage also
contains only two uploads and one download. The evidence shows that the severe
August 18 uplink condition did not recur, but it does not isolate device
hardware from signal, configuration, session, or time-varying path state.

Validation state: all 60 raw/derived JSON documents and 6,574 JSONL records
parse; every derived application and bandwidth row matches raw evidence; the
8,000 final exchanges, excluded 142-row diagnostic, six exact-byte transfers,
GNSS record, and 382 radio samples pass their declared checks. Final SHA-256,
pause-checkpoint integrity, process/listener, credential-leak, Python compile,
shell syntax, and documentation diff checks pass. The temporary d1 key was
revoked, no longer authenticates, and its local directory was moved to trash.
No experiment process or planned listener remains locally or on d1.
The two previous-device comparison CSVs were independently recalculated from
the August 18 and August 19 derived source rows; all values, sample counts,
percent changes, and ratios match.

Working files:

- `results/real_5g/20260819_airspan_tdd_40_40_20_new_device_location_3_directional_repeat/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_full_directional_40_40_20_new_device_stationary_run_1_unredacted/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_raw_uplink_40_40_20_new_device_location_3_tcp_mqtt_run_1_unredacted/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_raw_downlink_40_40_20_new_device_location_3_tcp_mqtt_run_1_unredacted/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_tcp_bulk_50mib_40_40_20_new_device_location_3_run_1_unredacted/`

## 2026-08-19 GL-X3000 Full Directional TDD 70/20/10 Repeat

Complete. The full directional application and exact-byte bandwidth matrix was
repeated using the newly installed GL-X3000 private-5G gateway while the vehicle
was stationary. A fresh 60-second ROS 2 GNSS bag was recorded before
application traffic. This run remains separate from every earlier device/run
so that it can be used as a device-replacement check rather than silently
pooled with prior data.

The declared application matrix contains TCP and MQTT in both directions at
1, 10, 100, and 500 KiB, with exactly 500 complete validated exchanges per
transport, direction, and payload. **Uplink** means the vehicle sends the
declared payload to d1 and d1 returns a compact application acknowledgment.
**Downlink** means the vehicle sends a compact request and d1 returns the
declared payload; RTT ends after the vehicle receives and validates the full
length, sequence number, and CRC32, with validation duration retained
separately and included in RTT. After the RTT matrix, run three sequential
exact 50-MiB TCP pairs: vehicle-to-d1 upload followed by d1-to-vehicle download.

TDD `70/20/10` and stationary state are operator reported for this run.
Credentialed modem history produced 285 unique ten-second 5G-SA samples over
47.5 minutes: RSRP `-104` to `-95 dBm` (median `-99 dBm`), RSRQ constant at
`-10 dB`, and SINR `15` to `23 dB` (median `20 dB`). Serving-cell identity,
cell administrative states, and handoff state were not freshly reported and
remain null rather than inheriting prior-device values. Initial host validation
observed vehicle address `192.168.8.233` on `eno2`; d1 traffic used the new
gateway at `192.168.8.1`, and d1 responded to ICMP at approximately 20 ms.

The GNSS bag spans 59.616 seconds with 16,101 messages. Its 593 BESTPOS samples
have 0.031 m maximum radial spread and 0.0067 m/s maximum speed, and its median
is 2.98 m from the August 18 location-3 reference; the stationary and
`location_3` gates pass. The d1 wall clock was approximately 188 seconds behind
the vehicle, so cross-host clocks remain explicitly unsynchronized and all RTT
uses the vehicle-side monotonic clock.

All 16 application conditions completed exactly 500 accepted exchanges with
zero failures: 4,000 uplink and 4,000 downlink. Uplink p50 RTT spans
39.411--238.607 ms and p95 spans 46.948--267.336 ms. Downlink p50 RTT spans
29.662--80.493 ms and p95 spans 40.001--97.806 ms. Downlink payload-validation
p50 spans 0.044--1.406 ms and p95 spans 0.081--2.082 ms; validation remains
included within application RTT. All six bandwidth direction-runs transferred
exactly 52,428,800 bytes and passed endpoint CRC32 validation. Receiver-observed
mean goodput is 15.691 Mb/s upload (vehicle to d1) and 166.106 Mb/s download
(d1 to vehicle), with three complete repetitions in each direction.

Payload bodies remain in memory throughout every timed operation. The uplink
receiver validates length and CRC32 before sending its acknowledgment; the
downlink vehicle validates length, sequence number, and CRC32 before ending
RTT; the bandwidth probe validates exact bytes and CRC32. Only result rows,
summaries, and telemetry are written after the applicable timing boundary, so
payload-file HDD I/O is not part of the measured RTT or goodput.

The closest old-device control is the August 17 TDD `70/20/10`, location-3
dataset at reported RSRP `-100 dBm`. The new-device run has 30.8% higher mean
upload and 31.0% higher mean download goodput, while matched downlink RTT is
mixed: p50 changes range from 7.5% lower to 20.7% higher. Relative to the
August 18 TDD `40/40/20` run, 500-KiB uplink p50 is 97.1% lower for TCP and
96.9% lower for MQTT, but TDD, signal, and collection time also changed. The
severe August 18 uplink condition did not persist during this replacement-
device run; the current evidence cannot separate hardware, configuration,
session state, and contemporaneous radio/path effects.

Working files:

- `scripts/run_airspan_uplink_payload_sweep.sh`
- `scripts/collect_glinet_modem_signal.py`
- `results/real_5g/20260819_airspan_tdd_70_20_10_new_device_location_3_directional_repeat/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_full_directional_70_20_10_new_device_stationary_run_1_unredacted/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_raw_uplink_70_20_10_new_device_location_3_tcp_mqtt_run_1_unredacted/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_raw_downlink_70_20_10_new_device_location_3_tcp_mqtt_run_1_unredacted/`
- `CISCO_AIRSPAN_STATS/20260819_airspan_tdd_tcp_bulk_50mib_70_20_10_new_device_location_3_run_1_unredacted/`

Validation state: all 56 raw/derived JSON documents and 5,064 JSONL records
parse; every derived RTT and bandwidth row matches its raw summary; declared
attempt counts, accepted counts, zero-failure states, byte counts, CRC32 values,
GNSS duration, and signal ranges pass. SHA-256 manifests for all four raw trees
and the derived tree verify. The Python probes compile, both runners pass shell
syntax checks, and `git diff --check` passes for the edited documentation,
scripts, and derived summaries. No local or d1 experiment process or planned
listener remains. The credential file is outside the repository with user-only
permissions, and a full repository scan found no stored password. The temporary
d1 key was revoked, no longer authenticates, and its local directory was moved
to trash so recovery remains possible.
## 2026-08-19 G-NetTrack RSRP And SINR Drive Import

Complete. The Google Drive folder supplied by the user was downloaded to
`results/real_5g/20260818_19_gnettrack_rsrp_snr/` with its full `gnettrack/`
hierarchy preserved. The import contains the August 18 validation collections,
the August 18 drive, the three August 19 drives, raw KML and text logs,
visualization products, the combined measurement table, the RSRP and SINR PNG
figures, and the offline OpenStreetMap assets needed by the standalone map.

The Drive inventory contains 427 files totaling 102,683,139 bytes. All 427
local files match their Drive-reported byte sizes, no `.part` file remains, and
both primary figures decode as 1,880-by-1,974 RGB PNGs. The key products are:

- `gnettrack/2026.08.19_three_drives/combined_map/interpolated_rsrp_osm.png`
- `gnettrack/2026.08.19_three_drives/combined_map/interpolated_sinr_osm.png`
- `gnettrack/2026.08.19_three_drives/combined_map/combined_signal_measurements.csv`
- `gnettrack/2026.08.19_three_drives/combined_map/combined_signal_report.md`
- `gnettrack/2026.08.19_three_drives/combined_map/combined_signal_summary.json`

The imported report records 1,301 GPS samples, excludes 123 samples from a
signal-loss plateau, and retains 1,178 valid NR RSRP and SINR samples. The
ordinary G-NetTrack SNR field is empty. The second map is therefore an NR SINR
map reconstructed from the verbose logs' timestamped `ssSinr` field, not an
ordinary G-NetTrack SNR map. This distinction must be preserved in any later
paper text or figure caption.

No manuscript claim or figure reference was changed, and no commit or push was
made.

## 2026-08-19 Bidirectional Event-Triggered CAV Bursts

Complete. The manuscript now treats event-triggered burst behavior as a core
CAV workload property. A driving event, service request, or fault can initiate
an exchange instead of waiting for a fixed periodic schedule. The large object
can originate at the vehicle, creating an uplink-heavy burst, or be returned by
the edge after a compact vehicle request, creating a downlink-heavy burst.
Independent operations can also overlap and compete for radio resources.

The application model, Figure 5, and Table 2 now distinguish periodic CV/ITS
messages, event-triggered stateful CAV exchanges, and sustained traffic such as
remote-assistance video. The experimental interpretation connects this workload
property to the measured direction asymmetry, concurrent-demand deadline
crossings, and TDD operational evidence. The paper does not claim to measure a
stochastic burst-arrival distribution; it measures the communication
consequences of representative uplink-heavy, downlink-heavy, and overlapping
workloads.

Insight 3 now states: **5G/6G radios must support event-triggered CAV bursts in
either direction and isolate concurrent flows.** The supporting text calls for
stable TDD implementation and reconfiguration, scheduling based on each burst's
direction and deadline, and isolation among simultaneous exchanges. The same
logic appears in the abstract, introduction, setup, results, future research
directions, conclusion, `paper/paper_outline.md`, and `agent_context.md`.

Validation: `make` completes; the log contains no overfull boxes, undefined
citations, or undefined references; the affected pages were rendered and
visually inspected; and the redundant shared-resource label that overlapped
Figure 5 was removed. The 22-page US-letter PDF contains no annotations or
embedded links. It is 3,025,873 bytes and has SHA-256
`41e5bb61851ebf7ab2b9555ddc3d02c4bb9195c344a3d0dd500a815d5597cdbe`.
No page-limit optimization was performed. `paper/current_manscript/28p.pdf` remains
preserved with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The ignored paper tree was not force-added, and no commit or push was made.

## 2026-08-19 TDD, Downlink, And Manuscript Update

Complete. The manuscript now incorporates the August 15--18 TDD study, the DU
Cell context, and the new downlink-heavy application experiments without
pooling communication directions, transports, TDD profiles, or endpoint and
cell estimators.

The revised argument distinguishes deployment maturity from an intrinsic TDD
ranking. The long-used `70/20/10` profile produced repeatable uplink-heavy RTT
on August 15--16. The transition to `40/40/20` changed route and serving-cell
state, so those repetitions remain inconclusive for profile ranking and instead
show that this deployment did not preserve a stable end-to-end path during the
alternative-profile rollout. The matched August 17 `40/40/20` block completed
all 8,000 downlink-heavy exchanges. Its latency advantage was confined to
smaller responses and reversed in the larger MQTT tails. Therefore, the paper
does not claim that `40/40/20` is inherently unstable or universally inferior.
It concludes that uplink-oriented TDD support needs interoperable
implementations, stable reconfiguration, direction-aware scheduling, and
concurrent-flow isolation.

Two full-width figures now present the result. The TDD figure separates
uplink-heavy TCP, uplink-heavy MQTT, matched downlink-heavy TCP, and matched
downlink-heavy MQTT. It marks the August 15 `40/40/20` values as a derived
reference because their raw timestamps are unavailable. Its matched panels are
qualified by six complete five-minute Cell 2 bins per profile with zero DU
unavailability. The directional-workload figure separately plots all ten exact
50-MiB upload repetitions, all ten exact download repetitions, their means and
95% confidence intervals, and the matched August 18 500-KiB MQTT direction
comparison. DU active-time throughput is not plotted as endpoint goodput
because the estimators and time support differ.

The application matrix now includes downlink-heavy CAV data retrieval: a
vehicle sends a compact request and the edge returns raw sensor, perception,
planning, control, or other data. The setup defines this timing boundary and
the exact directional-transfer boundary. The abstract, introduction, related
work, setup, results, future research directions, conclusion, appendices, and
`paper/paper_outline.md` now carry the direction and TDD evidence consistently.
Insight 3 states that 5G/6G radios must support event-triggered CAV bursts in
either direction and isolate concurrent flows. Its supporting argument retains
the need for interoperable TDD implementations, stable reconfiguration,
direction-aware scheduling, and concurrent-flow isolation.

Working files include:

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/figs/section5_tdd_profiles.pdf`
- `paper/current_manscript/figs/section5_directional_workloads.pdf`
- `paper/current_manscript/figs/section5_mixed_load_qos.pdf`
- `paper/current_manscript/main.pdf`
- `paper/paper_outline.md`

Validation: the plotted values were checked against the retained August 17 and
18 comparison CSVs; the matched DU context agrees with the deduplicated Cell 2
windows; `make` completes; the log contains no overfull boxes, undefined
citations, or undefined references; and all 22 rendered pages were visually
inspected. The PDF is US letter, 3,024,169 bytes, and has SHA-256
`5304efda1241c0b3da86937ce6fdd9a3d0cafadeb36b2b06513f04309089757a`.
No page-limit optimization was performed. `paper/current_manscript/28p.pdf` remains
preserved with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The ignored paper tree was not force-added, and no commit or push was made.

## 2026-08-18 TDD And DU Cell Comparison

Complete. The August 15--18 application records were aligned with the Airspan
DU Cell exports without pooling uplink-heavy RTT, downlink-heavy RTT, exact
upload, exact download, `70/20/10`, or `40/40/20`. The complete analysis is in
`paper/analysis/tdd_du_cell_comparison.md`, and the machine-readable evidence
decision is in `results/real_5g/tdd_comparison_status.json`.

The seven overlapping DU exports contain 10,012 physical rows and 3,906 unique
node/managed-element/cell/start/end keys. Repeated rows agree after numeric
normalization. The analysis uses complete five-minute bins within each
experiment window, matches the serving cell, excludes zero-duration rows and
nonzero-unavailability bins from clean comparisons, and recomputes throughput
as summed volume divided by summed active time. DU timestamps are interpreted
as America/New_York local time because their direction changes align with the
application records; the CSV itself does not contain a timezone, so this
interpretation remains inferred rather than vendor-confirmed.

The August 15--16 uplink-heavy data remain inconclusive for a TDD ranking. The
August 15 `40/40/20` raw run and timestamps are absent, while the August 16
`70/20/10` run begins after retained DU coverage ends. The DU rows confirm that
the two August 16 `40/40/20` TCP repetitions used different serving cells.

The August 17 downlink-heavy blocks provide the clean same-cell DU comparison.
Six complete Cell 2 bins per profile contain zero reported unavailability.
`70/20/10` has 8,004,535 kb of Cell downlink volume and 54.463 Mb/s of
active-time-derived Cell downlink throughput; `40/40/20` has 8,814,798 kb and
61.285 Mb/s. The application result remains payload- and transport-dependent:
`40/40/20` lowers small-response RTT, while its advantage disappears or
reverses in the larger MQTT tails.

The exact 50-MiB endpoint results remain separate by direction. On August 17,
`70/20/10` versus `40/40/20` measures 12.000 versus 2.836 Mb/s upload and
126.786 versus 99.756 Mb/s download. The alternating upload/download schedule
shares five-minute DU bins, so DU counters are not used to estimate each bulk
direction. At the matched August 18 `40/40/20`, MQTT, 500-KiB, `-115 dBm`
condition, uplink-heavy p50/p95 RTT is 61.5/41.0 times the downlink-heavy RTT.

Documentation updates also remove the stale claim that reverse-direction
payloads were not measured. `experiment_summary.md`, `remaining_exp.md`, and
`agent_context.md` now route future work to the separated evidence decision.
No manuscript source was changed.

Validation: `results/real_5g/tdd_comparison_status.json` parses with
`python -m json.tool`; every reported application value was checked against the
stored comparison CSVs; the matched DU values were recomputed from deduplicated
source rows; the two selected August 17 DU windows contain exactly six complete
five-minute bins each; and `git diff --check` passes for the edited text and
JSON files.

## 2026-08-18 Latest Experiment Data Pull

Complete. `git pull --rebase --autostash origin main` fast-forwarded local
`main` from `b3ae0975b02e0d13ac8cbd8bfa8be30211a92026` to
`a1e006909de0e89945993356bb3798a545974f02` (`Add August 18 directional 5G
experiment`). The incoming commit changes 416 files. It adds the August 18
location-3 `40/40/20` uplink-heavy and downlink-heavy TCP/MQTT collections,
fresh GNSS and host telemetry, the operator-shortened directional 50-MiB
throughput collection, a clearly excluded unintended partial preflight, and
the repository-facing directional analysis. It also updates three experiment
scripts and the experiment documentation.

The autostash restored the pre-existing staged repository records. Its only
content conflict was an insertion-point conflict in `current_task.md`; the
incoming August 18 experiment record and the local DU-log, Figure 2, and prior
pull records were all retained. No raw result or manuscript file conflicted.
Local `HEAD` and `origin/main` both resolve to `a1e0069`. No incoming result was
interpreted, and no manuscript source or experiment artifact was modified as
part of this pull.

Validation: all six incoming `SHA256SUMS` manifests verify, including the
repository-facing result and the raw, shortened, and explicitly excluded
diagnostic trees. All incoming JSON files parse. The changed Python probe
parses successfully, both changed shell runners pass `bash -n`, and the staged
and unstaged whitespace checks pass. Git reports no unresolved path. The
conflict-preserving autostash remains available as `stash@{0}` for recovery;
it was not dropped.

## 2026-08-18 Full Directional TDD 40/40/20 Repeat

Complete with an operator-shortened bandwidth stage. At the current stationary vehicle location and operator-reported
TDD `40/40/20`, collect a fresh 60-second ROS 2 GNSS record, then run the
following application and bandwidth measurements in order:

1. Uplink-heavy TCP and MQTT at 1, 10, 100, and 500 KiB. The 1, 10, and
   100-KiB conditions use 1,000 complete validated exchanges each; at the
   operator's request during acquisition, both 500-KiB conditions stop at 250
   exchanges. **Uplink** means the
   vehicle sends the declared payload to d1 and d1 returns a compact
   acknowledgment; RTT ends when that acknowledgment is received.
2. Downlink-heavy TCP and MQTT at 1, 10, 100, and 500 KiB. The 1, 10, and
   100-KiB conditions use 1,000 complete validated exchanges each; both
   500-KiB conditions use 500. The downlink-heavy timing contract is:
   (1) the vehicle sends a compact request to d1; (2) d1 sends the requested
   1, 10, 100, or 500 KiB payload to the vehicle; (3) RTT starts immediately
   before the vehicle sends the request; and (4) RTT ends after the vehicle
   receives and validates the complete payload, including length, sequence
   number, and CRC32. Store that vehicle-side validation duration separately
   as `payload_validation_ms`, and include it within `rtt_ms`.
3. The original plan requested five exact 50-MiB TCP bandwidth pairs.
   **Upload** is vehicle to d1; **download** is d1 to vehicle. During
   repetition 2 upload, the operator requested a stop after that active 50-MiB
   transfer. The final dataset therefore contains one complete upload/download
   pair plus one additional validated upload. Every retained transfer matches
   52,428,800 bytes and CRC32 at both endpoints.

TDD `40/40/20` is fresh operator-reported context for today's run. RSRP was
operator-reported as `-105 dBm` initially, `-110 dBm` from
`2026-08-18 11:07 EDT`, and `-115 dBm` from `11:18 EDT` onward, with no
location change. Conditions completed before 11:07 retain `-105 dBm`;
conditions wholly within 11:07--11:18 use `-110 dBm`; conditions starting at
or after 11:18 use `-115 dBm`; any condition crossing a boundary records every
applicable time segment and must not be summarized under a single RSRP value.
Fresh RSRQ, cell
administrative state, serving cell, and handoff state have not yet been
reported; do not silently describe carried-forward values as new measurements.

Working files:

- `scripts/private_5g_raw_downlink_probe.py`
- `scripts/run_airspan_uplink_payload_sweep.sh`
- `scripts/run_airspan_directional_bulk_repetitions.sh`
- `results/real_5g/20260818_airspan_tdd_40_40_20_location_3_directional_repeat/`
- `current_task.md`

Validation state: the protocol self-tests, Python compilation, shell syntax,
vehicle-to-d1 route over `eno2`, temporary-key d1 access, source deployment and
hash matching, remote Python 3.8 self-tests, and planned-port checks pass for
both application directions. The fresh GNSS bag contains 16,072 messages over
59.506 seconds; its median is 1.80 m from the August 17 location-3 reference,
and the recorded maximum speed is 0.012 m/s, so this run is labeled stationary
`location_3`. Uplink-heavy TCP 1 KiB, 10 KiB, and 100 KiB have each completed
at 1,000/1,000 accepted exchanges with zero failures. TCP 500 KiB was stopped
after 260 accepted exchanges when the operator reduced its target to 250; the
full 260-row capture is retained as overrun diagnostic evidence and the first
250 exchanges are the declared condition. MQTT uplink 1 KiB and 10 KiB each
completed at 1,000/1,000 with zero failures, and MQTT uplink 100 KiB is now
complete at 1,000/1,000 with zero failures. That condition crossed both RSRP
transitions and is explicitly a mixed `-105`/`-110`/`-115 dBm` condition.
MQTT uplink 500 KiB completed at 250/250 with zero failures wholly after 11:18
at `-115 dBm`. All eight downlink-heavy TCP/MQTT conditions then completed at
their declared counts with zero failures under RSRP `-115 dBm`. The five-pair
bandwidth stage ended at the operator-requested boundary. Repetition 1 passed
exact byte/CRC validation in both directions: 0.524 Mb/s vehicle-to-d1 upload
and 42.746 Mb/s d1-to-vehicle download. Repetition 2 vehicle-to-d1 upload also
passed at 0.507 Mb/s. Repetition 2 download and repetitions 3--5 did not start.
No experiment process or planned listener remains. The temporary d1 key was
revoked, the revoked key no longer authenticates, and the local temporary key
and controller were moved to trash.

Downlink timing implementation validation: the revised probe passes Python
compilation and its protocol self-test. Three-message 500-KiB TCP and MQTT
loopback tests pass with `payload_validation_ms` present and positive on every
accepted row and `rtt_ms >= payload_validation_ms` on every row. The observed
loopback validation times were approximately 0.48--0.69 ms. The live downlink
stage used the revised boundary and schema on every accepted row. Across the
eight downlink conditions, validation p50 ranges from 0.035 to 1.364 ms and
validation p95 ranges from 0.048 to 1.700 ms; every condition summary confirms
that validation is included in RTT.

An attempted bandwidth preflight exposed that the bandwidth driver does not
implement a `--help`/preflight mode and therefore began a transfer. That
transfer was stopped before completion, is retained under an explicitly named
`unintended_partial` diagnostic tree, and will not be counted. No associated
process or planned listener remains. The accepted bandwidth data use a separate
clean result path. SHA-256 manifests for all five raw trees and the derived
result verify; all JSON parses; every one of the 16 derived RTT rows and three
derived bandwidth rows matches its raw source summary; the Python probes
compile; both shell runners pass syntax checks; and `git diff --check` passes
for the edited documentation, scripts, and derived summaries. The byte-exact
raw CSV captures retain their original CRLF line endings, which the generic
Git whitespace checker reports but which are intentionally not normalized.
## 2026-08-18 DU Cell Statistics Log Download and Refresh

Complete. Downloaded the public Google Drive folder `DU Cell Stat Log` from
`https://drive.google.com/drive/folders/15SyzOEdhmJ3bMEzYWgKMJhGlPhQRYEt4?usp=sharing`
into `CISCO_AIRSPAN_STATS/DU_Cell_Stat_Log/`. A later folder refresh found and
downloaded `DUCellExport_20260818_1342.csv` without redownloading or overwriting
the six earlier exports. All seven CSV exports are preserved byte-for-byte under
their Drive filenames. They cover exports dated August 5, August 6, August 16,
and August 18, 2026.

Validation: all seven files are CSV text rather than HTML download pages. Each
file has 45 columns, every parsed row has the declared 45-column width, and the
collection contains 10,012 data rows and 1,652,350 bytes. `SHA256SUMS` verifies
all seven files. `download_manifest.json` records the source folder, Drive file
identifiers, initial download and latest refresh times, download tool, file
sizes, row and column counts, and SHA-256 digests. No CSV content was changed,
no result interpretation was made, and no manuscript source was modified for
this download or refresh.

## Figure 2 IPI Architecture Replacement

Complete. The TikZ-based two-column Figure 2 in
`paper/current_manscript/sections/03_ipi_protocol_design.tex` was replaced with the
user-supplied `paper/current_manscript/figs/figure02_ipi_protocol_architecture.pdf`.
The new figure uses a single-column `figure` float and is rendered at
`\columnwidth`. The figure label remains `fig:ipi-architecture`, its alternative
description now follows the supplied architecture, and the manuscript Makefile
tracks the new PDF asset as a build dependency.

Validation: a forced LaTeX and BibTeX rebuild completed successfully. Figure 2
appears in the right column of page 4, remains within the column boundary, and
does not overlap the caption or surrounding manuscript text. The final log has
no LaTeX errors, undefined citations or references, rerun requests, or overfull
boxes. The manuscript remains 19 US-Letter pages. The supplied Figure 2 PDF has
SHA-256 `743d5c8f458eb22b5ccbf361ab6772ac9c8b0184ebbb1cd55d70662a4f94454c`,
and the rebuilt manuscript PDF has SHA-256
`e703e17564775bc99c0d9b1aad686506d07502b87b2b234e91fafa83aab12f9a`.
The supplied artwork itself retains a source-level overlap between
"Operation mode" and "Correlated CAV operations"; the asset was embedded
unchanged as requested. No page-limit optimization was performed.

## 2026-08-18 Latest Results Pull

Complete. `git pull --rebase --autostash origin main` fast-forwarded local
`main` from `8641b0dc64dbc5c6e2f74d2cabb72f673bf69aa7` to
`b3ae0975b02e0d13ac8cbd8bfa8be30211a92026` (`Add location 3 TDD comparison
results`). The incoming commit adds the matched location-3 `40/40/20` and
`70/20/10` downlink-heavy TCP/MQTT RTT results, repeated bidirectional 50-MiB
TCP throughput measurements, their raw collections, derived comparison, and
updated experiment scripts and documentation. Restoring the pre-existing local
paper and TDD notes produced insertion-point conflicts in `current_task.md` and
`remaining_exp.md`; both chronological records were retained, with the newer
location-3 results placed before the earlier notes. No raw result or manuscript
file conflicted.
## 2026-08-17 Matched 40/40/20 Location-3 Collection

Complete. After the operator changed TDD to `40/40/20`, repeat the unchanged
location-3, RSRP `-100 dBm`, RSRQ `-13 dB` measurements that were collected at
`70/20/10`. First collect the downlink-heavy TCP/MQTT RTT matrix at 1, 10, 100,
and 500 KiB with 1,000 exact validated responses per condition. Then collect
10 new matched directional-throughput repetitions, each using an exact 50-MiB
vehicle-to-d1 upload followed by an exact 50-MiB d1-to-vehicle download. Reuse
the fresh location-3 GNSS evidence because placement is unchanged. Keep the
profiles separate until all within-profile validation passes.

Validation state: protocol, route, d1 access, source-hash, remote self-test, and
port preflight passed. The RTT matrix completed all eight TCP/MQTT conditions
at 1,000/1,000 accepted responses with zero failures. In the 10-repetition
throughput stage, all 10 repetitions completed in both directions with exact
byte-count and CRC32 validation. Receiver-observed throughput averages 2.836
Mb/s upload (95% t interval 2.685--2.987 Mb/s) and 99.756 Mb/s download (95%
t interval 82.316--117.195 Mb/s). No experiment process or planned listener
remains. The final raw aggregate and repository-facing comparison are stored
under the completed run and
`results/real_5g/20260817_airspan_tdd_profile_comparison_location_3/`. The
temporary d1 authorization was revoked, its local key pair and marker were
moved to trash, and the revoked key no longer authenticates.

Audit after the operator questioned the throughput result: direction handling,
the Mb/s conversion, exact byte count, CRC32 correlation, and independent
endpoint timers are correct. The identical SHA-256 probe was used for both TDD
profiles, a local 50-MiB control reached 4.49 Gb/s at the receiver, and vehicle
interface counters independently corroborate approximately 3-Mb/s transmission
in the current run. However, a live socket inspection found the current upload
flow at roughly 51-ms RTT with congestion window 12, 437,296 retransmitted bytes
after 23,044,936 bytes sent, and approximately 2.42-Mb/s delivery rate. RustDesk,
Codex/browser, and other TCP connections were also using the vehicle's `eno2`
default route. The vehicle point-to-point address changed from `.29` during the
70/20/10 run to `.30` after the network/TDD reconfiguration. The operator then
confirmed that TDD was the only radio-side setting changed and that the other
host applications contributed negligible work relative to the experiment.
Accordingly, retain the background connections and address transition as
experimental context rather than automatically disqualifying the comparison.
The retransmissions and reduced congestion window may be mechanisms through
which the `40/40/20` profile affected end-to-end TCP goodput. The result remains
a single-flow application-goodput measurement rather than direct radio-PHY
capacity, and the TDD state remains operator-reported without a timestamp-
aligned configuration export.

Experiment-family separation correction: do not pool or relabel the three
measurement families. The August 15--16 TCP/MQTT experiments are uplink-heavy
RTT measurements: the vehicle sends the declared large object to d1, and d1
returns a compact acknowledgment. The August 17 TCP/MQTT experiments are
downlink-heavy RTT measurements: the vehicle sends a compact request, and d1
returns the declared large object. The August 17 exact 50-MiB TCP experiment is
the only current bandwidth measurement and contains two separate directions:
vehicle-to-d1 upload and d1-to-vehicle download. It is not an RTT sweep and
must not be pooled with either TCP/MQTT request/response family. Compare TDD
profiles only within the same experiment family, direction, transport, payload,
location, and reported radio context; there is no single pooled "TDD latency"
result across these three families.

Final documentation and validation: E16--E17 in `experiment_summary.md` record
the downlink-heavy RTT and exact 50-MiB results, while `remaining_exp.md`
records which directional matrix is complete and which same-location uplink
RTT work was not collected. All six August 17 location-3 raw trees and the
derived result have SHA-256 manifests. Those manifests verify; all JSON parses;
the Python probes compile; both shell runners pass syntax checks; all 16
downlink conditions account for 16,000 accepted responses and zero failures;
all 40 primary bulk direction-runs pass exact byte/CRC validation; and every
derived RTT and throughput CSV value matches its source artifact.

## 2026-08-17 Location-3 Directional Throughput Repetitions

Complete. Collected 10 new matched repetitions of the exact 50-MiB single-flow
TCP measurement under unchanged TDD `70/20/10`, location 3, RSRP `-100 dBm`,
and RSRQ `-13 dB`. Every repetition runs vehicle-to-d1 upload first and
d1-to-vehicle download second. The preceding single run remains a pilot and is
not silently pooled into the primary `n=10` estimate. Each direction must match
52,428,800 bytes and CRC32 at both endpoints before its repetition is accepted.

All 20 direction-runs passed exact endpoint byte-count and CRC32 validation.
Receiver-observed upload throughput has a 12.000 Mb/s mean, 12.098 Mb/s median,
0.338 Mb/s sample standard deviation, and 11.759--12.242 Mb/s two-sided 95%
t interval for the mean. Receiver-observed download throughput has a 126.786
Mb/s mean, 136.166 Mb/s median, 28.643 Mb/s sample standard deviation, and
106.296--147.277 Mb/s 95% t interval. Download contains one valid 54.050-Mb/s
repetition and is materially more variable than upload. Cleanup and access
revocation passed: no experiment processes or planned ports remain, and the
temporary d1 key was revoked and removed. Aggregate structured-data, condition-
marker, protocol self-test, shell-syntax, and `git diff --check` validation pass.

## 2026-08-17 Location-3 Directional Bulk-Throughput Collection

Complete. At the unchanged stationary location-3 placement and radio
context, measure one sequential TCP bulk transfer in each direction under TDD
`70/20/10`, RSRP `-100 dBm`, and RSRQ `-13 dB`. The vehicle first transfers
exactly 50 MiB (52,428,800 B) to d1, then requests an exact 50-MiB reverse
transfer from d1. Preserve iperf3 interval and end summaries from both endpoints
and report application-layer sender/receiver throughput.
This is a single-flow host-path measurement, not a direct estimate of radio PHY
capacity. Reuse the immediately preceding fresh location-3 GNSS evidence.

The vehicle-to-d1 upload delivered 50 MiB in 34.550 seconds at a
receiver-observed 12.140 Mb/s. The d1-to-vehicle download delivered 50 MiB in
2.784 seconds at a receiver-observed 150.631 Mb/s, 12.408 times the upload
throughput in this single sequential run. Both endpoints agree on direction,
exact byte count, and CRC32, and both servers exited with status 0. TCP
retransmissions were not captured by this dependency-free exact-byte probe.

## 2026-08-17 TCP/MQTT Raw-Downlink Collection

Complete. The requested stationary experiment uses TDD `70/20/10` and
operator-reported RSRP `-100 dBm`. The vehicle sends a compact request to d1,
which returns an exact 1, 10, 100, or 500 KiB application body. Each TCP and
MQTT condition declares 1,000 requests. RTT begins immediately before request
transmission and ends only after receipt and validation of the complete
correlated response body, including its length, sequence, and CRC32. A fresh
60-second ROS 2 GNSS capture completed before the workload. It contains 16,102
messages over 59.629 seconds. Its 594 BESTPOS samples have 0.021 m maximum
radial spread, and the median position is approximately 212 m from the prior
location-2 median; the run is therefore labeled `location_3`.

RSRQ, cell administrative state, serving cell, and handoff state were not
restated for this run. Until corrected, acquisition metadata will identify
their values as carried forward from the immediately preceding location-2 run,
not as fresh measurements.

Working files:

- `scripts/private_5g_raw_downlink_probe.py`
- `scripts/run_airspan_uplink_payload_sweep.sh`
- `CISCO_AIRSPAN_STATS/20260817_airspan_tdd_raw_downlink_70_20_10_location_3_tcp_mqtt_run_1_unredacted/`
- `current_task.md`

Validation state: protocol self-tests, Python compilation, shell syntax, and
three-message TCP and MQTT loopback tests at 500 KiB pass. Fresh GNSS capture
passes the stationary gate. The first edge attempt exposed a Python 3.8
compatibility error after one valid TCP response; it was stopped, retained as
a failed diagnostic run, and did not qualify as experiment data. The compatible
implementation subsequently passed three-message 500-KiB TCP and MQTT tests
across the actual vehicle-to-d1 path. Clean run-2 acquisition ran from
2026-08-17 17:33:48 through 18:07:27 EDT. All eight TCP/MQTT conditions
completed at 1,000/1,000 accepted exchanges: 8,000 complete validated
responses with zero failures. Every condition has a complete marker and a
validation summary confirming 1,000 matching edge receiver rows. No local or
edge experiment process or planned port remains, and the temporary d1 key was
revoked and removed.
## August 15--16 TDD Experiment Update

Complete. The TDD evidence, manuscript, figures, outline, and experiment
records now use the user's corrected configuration history. Every private-5G
application campaign collected before the August 15--16 diagnostic is labeled
`70/20/10`. Earlier entries that classified those campaigns as `40/40/20` are
superseded.

The August 15--16 application RTT data are retained as a negative diagnostic.
The `70/20/10` TCP p95 values changed by 0.61%, 0.22%, and 1.29% across 1-,
10-, and 100-KiB requests. The `40/40/20` TCP p95 values changed by as much as
169.05% across dates and 189.83% across same-day repetitions. MQTT p95 under
`40/40/20` changed by 9.22--163.86% as payload increased from 1 KiB to 1 MiB.
Route recovery, cell administrative-state changes, and a serving-cell change
occurred across these runs. Therefore, the application RTT data cannot rank
the two TDD profiles or estimate a causal latency effect.

The sustained-capacity result is reported separately. With a 25-Mbit/s offered
vehicle-to-edge TCP stream, `70/20/10` achieved 13.118 Mbit/s and the
coauthor-reported `40/40/20` control achieved 25.000 Mbit/s. This direction is
consistent with increasing the fixed uplink share from 20% to 40%. The current
repository contains the raw `70/20/10` trace. The `40/40/20` value is recorded
as a coauthor experimental result because its raw trace is not present in the
current tree.

Updated evidence records include
`results/real_5g/tdd_comparison_status.json`,
`paper/analysis/tdd_experiment_update.md`, `experiment_summary.md`,
`remaining_exp.md`, and `agent_context.md`. The manuscript setup, Results,
Future Research Directions, Conclusion, appendix, figure-building script, and
`paper_outline.md` carry the same interpretation. Figure 9 now shows the
August 15--16 TCP and MQTT repetitions. Figure 10(a) now compares the two
uplink TDD-capacity controls and the retained `70/20/10` downlink control.

Validation: the result-status JSON parses successfully; the figure script and
full manuscript build complete successfully; the final LaTeX log contains no
undefined citations, undefined references, overfull boxes, or fatal errors;
and `git diff --check` passes. The 19-page US-letter PDF was rendered and the
updated setup table, Figures 9 and 10, Results discussion, insights,
future-research text, Conclusion, and appendix tables show no clipping or
overlap. No page-limit optimization was performed. The preserved 28-page
reference PDF remains unchanged with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The rebuilt manuscript PDF SHA-256 is
`be365f5a3c0121d2eb8c5a573c4435e7d60907d3d7b7a1e131f59434fd107cfc`.

## Latest Data Pull

Complete. `git pull --rebase --autostash origin main` fast-forwarded the local
`main` branch from `e3b9f6c80863b57147f6a78bb7331503f793cd84` to
`8641b0dc64dbc5c6e2f74d2cabb72f673bf69aa7` (`Add location 2 private 5G TCP
MQTT results`). The autostash restored the existing local Figure 1 task note
without conflict. The incoming commit changes 322 files, including the public
location-2 result tree at
`results/real_5g/20260817_airspan_tdd_raw_uplink_60_20_20_location_2_tcp_mqtt_run_1/`
and its raw collection at
`CISCO_AIRSPAN_STATS/20260817_airspan_tdd_raw_uplink_pending_location_2_tcp_mqtt_run_1_unredacted/`.
It also updates the experiment documentation and the raw-uplink collection and
analysis scripts.

Validation: local `HEAD` and `origin/main` both resolve to `8641b0d`. The
`SHA256SUMS` manifests pass for both new result trees. The public validation
record reports 6,327 accepted exchanges, no failed exchanges, eight validated
conditions, seven declared-count-complete conditions, and one preserved
user-stopped 1-MiB TCP condition. A commit-range whitespace check reports
line-ending/trailing-whitespace warnings in newly pulled generated CSV and log
artifacts; these raw records were preserved unchanged. The local working-tree
whitespace check passes. No result interpretation, manuscript claim, or paper
source was changed during this pull.

## Figure 1 Replacement

Complete. The Related Work section now uses the user-supplied
`paper/current_manscript/figs/figure01_related_work_landscape.pdf` asset instead of the earlier TikZ summary.
The figure label remains `fig:rw-gap`, and the caption and accessibility
description now match the replacement's current-status, key-gap, and
zero-fatality-vision structure. The manuscript Makefile tracks the new figure
as a build dependency. No page-limit optimization is included in this task.

Updated files:

- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/Makefile`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: a forced LaTeX and BibTeX rebuild completed successfully. The final
log contains no compilation errors, undefined citations or references, rerun
requests, or overfull boxes. Figure 1 resolves on page 3 and was rendered at
240 dpi; its labels, caption, and surrounding text are legible without overlap
or clipping. The PDF contains 19 US-Letter pages, zero annotations, zero links,
zero embedded files, no author or title metadata, and fully embedded fonts. No
page-limit optimization was performed. `paper/current_manscript/28p.pdf` remains
byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The replacement source has SHA-256
`12236bfdb5ff9e1ea253bc859cfc0c4d3fb4290270606528d75da2df1f616c0f`,
and the rebuilt `paper/current_manscript/main.pdf` has SHA-256
`df8d909f91e0d18165d876b6c6edcdd1cfe3b5f53194d22b8580e55a04f9fbcd`.
`git diff --check` passes.

## Repository README

Complete. A new root `README.md` explains why IPI combines stateless CV/ITS
messages with correlated stateful CAV operations. It documents the shared
application contract, session lifecycle, cooperative planning/perception/control
content, experimental J2735 regional profile, MQTT and PC5 bindings, current
C++17 implementation, build and test commands, repository layout, experiment
harness boundary, and release limitations. The root-document inventories in
`AGENTS.md` and `agent_context.md` now include the explicitly requested README.

Updated files:

- `README.md`
- `AGENTS.md`
- `agent_context.md`
- `cpp/examples/README.md`
- `current_task.md`

Validation: every relative README link resolves. The documented CMake
configuration and full C++17 build complete successfully, all 18 CTest tests
pass, and the three quick-start examples exit successfully. Markdown structure,
root-document inventory, and whitespace checks pass.

## 5G Signal-Bin Standardization

Complete. The manuscript's handset-RSRP categories are
strong at or above -95 dBm, common/typical from -105 to below -95 dBm, and weak
below -105 dBm. The three bins aggregate the four outdoor pedestrian coverage
levels in Australia's Telecommunications (Mobile Network Coverage Maps)
Industry Standard 2026. Good maps to strong, Moderate maps to common/typical,
and Basic plus No Coverage map to weak. A short setup paragraph cites that
standard and 3GPP TS 38.215, which defines SS-RSRP but does not assign
qualitative coverage levels. The binding outline and Figure 4 caption use the
same inequality-based definitions. The measured values retain their existing
categories: -101 dBm is common/typical, while -106, -109 to -110, and -120 dBm
are weak.

Updated files:

- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/references.bib`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: a forced LaTeX and BibTeX rebuild completed successfully. The
final log contains no compilation errors, undefined citations or references,
rerun requests, or overfull boxes. The PDF contains 19 US-Letter pages, zero
annotations, zero links, zero embedded files, no author or title metadata, and
fully embedded fonts. Manuscript pages 4--6 and reference page 16 were rendered
and inspected; the new paragraph, Table 1, Figure 4 caption, and both new
references are legible without overlap or clipping. No page-limit optimization
was performed. `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/current_manscript/main.pdf` has SHA-256
`e97c2be08737f7f32c058c3c8619c25f86eaed05abf44d53fc8efa2aa3be6f56`.
`git diff --check` passes.

## 2026-08-17 Location-2 TCP/MQTT Raw-Uplink Collection

Complete. At the user's new stationary location, the run collected a fresh
59.599-second ROS 2 GNSS bag and sequential uplink-oriented application-
acknowledgment RTTs over both TCP and MQTT at 1, 10, 100, and 1,024 KiB. The
bag contains 16,098 messages and passes the stationary gate with 0.189 m
maximum radial displacement. The TDD profile and
fresh RSRP were initially pending. During the TCP 1,024-KiB condition, the user
reported TDD `60/20/20` and RSRP `-100 dBm`; the timestamped context update is
stored in the raw run and supersedes the provisional carried-forward values for
analysis without rewriting acquisition manifests. Cross-location differences
must not be interpreted as a TDD or signal-strength effect without matched
controls.

The user subsequently stopped TCP 1,024 KiB after 227/227 matching sender and
receiver rows; it retains exit status 143 and is a user-stopped partial
condition, not a completed declared-count condition. TCP 1, 10, and 100 KiB
remain complete 1,000-message conditions. MQTT 1, 10, and 100 KiB remain
1,000-message conditions, while MQTT 1,024 KiB completed its explicit 100/100
declared count. All 6,327 retained sender rows are accepted and match 6,327
edge rows in sequence, payload length, CRC32, client-send timestamp, and
server-processing value.

All 68 fetched edge files match their remote copies. The temporary d1
authorization was revoked, no experiment processes or planned ports remain,
raw/public checksums pass, structured files parse, raw/public sender and
receiver measurements are identical, and the public privacy scan passes. Raw
evidence is stored under
`CISCO_AIRSPAN_STATS/20260817_airspan_tdd_raw_uplink_pending_location_2_tcp_mqtt_run_1_unredacted/`;
the sanitized result is
`results/real_5g/20260817_airspan_tdd_raw_uplink_60_20_20_location_2_tcp_mqtt_run_1/`.

## Private-5G Raw-Uplink Collection and TDD Correction

Complete. Six repository-facing experiment trees were added for the 2026-08-15
and 2026-08-16 raw-byte TCP/MQTT work. The latest result is
`results/real_5g/20260816_airspan_tdd_raw_uplink_60_20_20_location_1_tcp_run_1/`.
Its acquisition-time `60/30/10` label is preserved and explicitly superseded
by the user's `60/20/20` correction in `tdd_profile_correction.json`.

For the latest TCP run, 1 KiB, 10 KiB, and 100 KiB each completed 1,000/1,000
validated exchanges. Their p50 RTTs are 49.604, 101.216, and 753.242 ms, and
p95 RTTs are 75.391, 248.853, and 1,321.325 ms. At user request, 1,024 KiB
stopped after exactly 500 accepted exchanges; all 500 match edge rows. Its p50
is 7,484.305 ms and p95 is 10,187.364 ms. It is explicitly user-stopped and is
not mislabeled as a complete 1,000-message condition.

The context records Cell 1 locked/not broadcasting, Cell 2
unlocked/broadcasting and selected by the MG52 with no reported handoff, and
carried-forward RSRP `-98 dBm`/RSRQ `-13 dB`. All 28 fetched edge files matched
before access revocation. Raw/public checksums, structured parsing,
measurement identity, public-artifact privacy validation, process cleanup, and
source/document `git diff --check` pass. At the user's explicit direction, the
six raw 2026-08-15--16 experiment trees are committed alongside their public
derivatives. The corrected TDD and radio context remain
operator-reported because no timestamp-aligned ACP/MG52 exports are stored.

## Results-Presentation and Signal-Classification Annotation Revision

Complete. Seven substantive highlights in `paper/current_manscript/main.pdf` were
treated as manuscript-wide revision instructions. The annotated input contained
fourteen PDF annotation objects because each highlight had an associated popup.
It is preserved at
`tmp/pdfs/new_comments_20260816/main_annotated_before_revision.pdf` with
SHA-256
`b2eda34bfb14174e57335d57fe6c542072c2af0626b610f7f6a6742ea480fc1c`.

The revision defines signed handset RSRP bands as strong at or above -95 dBm,
common/typical from -105 to below -95 dBm, and weak below -105 dBm. The
stationary Uu workload collections are now identified as common/typical or weak;
none is described as a strong-band workload run. Table 1, Figure 4, Figure 9,
the setup prose, the results prose, the appendix, and the binding outline use
the same classification. Joint TDD/location comparisons retain both the
recorded TDD profile and RSRP rather than assigning the result to TDD alone.

The IPI packaging result now reproduces Table 4 exactly. A 256-B content field
produces a 334--337-B frame, while 1- and 4-KiB fields produce 1,102--1,105-B
and 4,174--4,177-B frames. The resulting 78--81-B addition is reported as
30.5--31.6%, 7.6--7.9%, and 1.9--2.0% of the respective content sizes. The
text no longer characterizes the smallest field through the relative-overhead
claim appropriate only to larger objects.

Section 5 now progresses from simple to compound comparisons: IPI packaging,
PC5 payload and route behavior, 5G payload and deadline behavior, transport and
reassembly, joint signal/TDD context, directional capacity and competing
traffic, concurrent application demand, interruption, and cross-application
comparison. Every table and figure is cited in the body. Wide result figures
are placed with interpreting text where the two-column layout permits it, and
result-first captions provide the interpretation when a wide float separates
the principal paragraph. Figure 7 uses explicit workload names, payload ranges,
request counts, units, deadlines, and denominators; `Detector output` is defined
as serialized object-detection result sizes replayed as IPI payloads. Figure 10
now groups idle/load pairs under their measured signal-condition labels without
overlapping x-axis text.

The ambiguous term `response availability` was removed from the manuscript.
The setup now defines deadline completion as
`C(B) = requests answered within B / all issued requests`, while response
completion denotes a response received before the harness timeout. Figures,
captions, prose, and appendix tables distinguish these completion rates from RTT
percentiles among completed responses. The binding outline records these
requirements and explicitly supersedes the earlier `A(B)`, availability, and
historical-placement drafting labels. `paper/general_academic_writing_lessons.md`
records the errors in domain-independent form as Lessons 15 and 16.

Updated files include:

- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/figs/section5_*.pdf`
- `paper/paper_outline.md`
- `paper/general_academic_writing_lessons.md`
- `paper/current_manscript/main.pdf`

Validation: the figure-generation script and forced LaTeX/BibTeX build complete
successfully. The final LaTeX pass contains no compilation errors, undefined
citations, undefined references, rerun requests, or overfull boxes. Every table
and figure label has at least one body reference. The rebuilt PDF contains 19
US-Letter pages as a build fact, zero annotations, zero links, no author or title
metadata, and fully embedded fonts. All 19 pages were rendered and inspected;
the revised Figures 6--12 were checked at higher resolution. No overlap,
clipping, missing figure, or broken table remains. No page-limit optimization
was performed, and `paper/current_manscript/28p.pdf` was not changed. The revised PDF
SHA-256 is
`50194c7d3f012cb8a555ee82e2dda1cc1d3d91b8ea8b9e4e75377d36706bc44b`.
`git diff --check` passes.

## Parallel Application-Class Annotation Revision

Complete. The new page-3 highlight in `paper/current_manscript/main.pdf` was treated as
a manuscript-wide revision instruction. The annotated input contained two PDF
annotation objects: one substantive highlight and its popup. It is preserved at
`tmp/pdfs/protocol_scope_comment_20260816/main_annotated_before_revision.pdf`
with SHA-256
`1f1010bee2078d1dcb5cdca396db2fd1d4c0a67b978609e3bdc591a9880d2102`.

The highlighted protocol-gap sentence compared `J2735 messages` with `stateful
CAV operations`, placing the two sides at different levels of abstraction. The
revision now defines the protocol contribution through two parallel application
classes: stateless CV/ITS applications and stateful CAV applications. The next
sentences explain their representations separately: individual J2735 messages
serve the stateless class, while correlated IPI operations serve the stateful
class.

This distinction was propagated through the Abstract, Introduction, contribution
list, Related Work opening and standards synthesis, Figure 1, IPI Protocol
Design, Figure 2, System Design and Experimental Setup, IPI packaging result,
Conclusion, and the binding paper outline. Detailed implementation passages
retain J2735 messages and correlated operations where those are the actual
objects being encoded or measured. No experiment result, citation, TDD evidence
classification, or page-limit target was changed, and no page-limit
optimization was performed.

`paper/general_academic_writing_lessons.md` now records the error in
domain-independent form as Lesson 14, `Comparing categories at different levels
of abstraction`. The lesson requires both sides of a comparison to remain at
the same conceptual level--system, application class, interaction pattern,
application object, or encoded representation--and requires mappings between
levels to be explained in a separate sentence. The final checklist now tests
conceptual parallelism and explicit class-to-representation mappings. The new
lesson contains no terminology from this paper or its technical domain.

Updated files:

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/general_academic_writing_lessons.md`
- `paper/current_manscript/main.pdf`

Validation: a forced LaTeX/BibTeX rebuild completed successfully. The final
LaTeX pass contains no compilation errors, undefined citations, undefined
references, rerun requests, or overfull boxes. The rebuilt PDF contains 18
US-Letter pages as a build fact, zero annotations, zero links, zero embedded
files, no author or title metadata, and fully embedded fonts. All 18 pages were
inspected in a rendered contact sheet; pages 1--4, 8, and 14 were also inspected
at high resolution. No overlap, clipping, missing figure, or broken table was
found. `git diff --check` passes. The revised PDF SHA-256 is
`0f95246151840b5b8c7886ad870e7c258a45ab1d9bacdaeae4df8926e4fe2e4a`.

## Protocol-Gap Logic Annotation Revision

Complete. The three substantive highlights on pages 2--3 of
`paper/current_manscript/main.pdf` were treated as manuscript-wide revision
instructions. The annotated input contained six PDF annotation objects because
each highlight had an associated popup. It is preserved at
`tmp/pdfs/related_work_comments_round2_20260816/main_annotated_before_revision.pdf`
with SHA-256
`ad2411990cb145d430940907eb1d40bc044c6637d57300c177a17cfe988a0781`.

The revision corrects three related reasoning errors. First, development
history is no longer presented as the cause of the protocol gap. Standards,
application systems, and field studies are described as addressing different
parts of communication, while the gap is defined as the present absence of a
coherent application protocol for stateless J2735-based CV/ITS messages and
stateful CAV operations. Second, the protocol claim is no longer limited to the
standards and platforms selected for the survey. The manuscript now states that
no current standard or platform provides the combined protocol and presents IPI
as the first implemented application protocol with this combination. Third,
the standards paragraph now transitions from the capabilities of existing
layers to the missing protocol through a genuine contrast, and the
application-systems paragraph explicitly states that IPI fills the same gap it
identifies.

The corrected logic was propagated through the Abstract, Introduction, Related
Work opening, Figure 1 and its caption, IPI Protocol Design, and the binding
paper outline. The experimental evidence gap remains separate: prior field
studies do not determine whether PC5 and Uu can each carry both workload classes
or identify the payload sizes and network conditions at which either path
misses application deadlines. No experiment result, citation, TDD evidence
classification, or page-limit target was changed, and no page-limit
optimization was performed.

`paper/general_academic_writing_lessons.md` now records the mistake in
domain-independent form. New Lesson 13 explains that historical development is
context rather than proof of a research gap; a gap must identify a present
missing capability, unanswered question, untested relationship, or unmet
requirement. It also distinguishes survey-scoped absence claims from
field-level novelty claims and requires an explicit gap-to-contribution handoff.
The final checklist now tests both points. The lesson contains no terminology
from this paper or its technical domain.

Updated files:

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/paper_outline.md`
- `paper/general_academic_writing_lessons.md`
- `paper/current_manscript/main.pdf`

Validation: a forced LaTeX/BibTeX rebuild completed successfully. The final
LaTeX pass contains no compilation errors, undefined citations, undefined
references, rerun requests, or overfull boxes. The rebuilt PDF contains 18
US-Letter pages as a build fact, zero annotations, zero links, zero embedded
files, no author or title metadata, and fully embedded fonts. All 18 pages were
inspected in a rendered contact sheet; pages 1--4 and 14 were also inspected at
high resolution. No overlap, clipping, missing figure, or broken table was
found. `git diff --check` passes. The revised PDF SHA-256 is
`7b812e3c9d706bad67004de054080729ea831b77a6cda030c4a83a8b2065f223`.

## Related Work Annotation Revision

Complete. The seven substantive highlights on pages 2--3 of
`paper/current_manscript/main.pdf` were treated as revision instructions. The annotated
input contained fourteen PDF annotations because each highlight had an
associated popup. It is preserved at
`tmp/pdfs/related_work_comments_20260816/main_annotated_before_revision.pdf`
with SHA-256
`e744247d5941640dde86ef348f30aad4537e807fd3d77e0e6a57b2bfc4e1f763`.

Related Work now describes three independently developed lines of work:
standards and application platforms, application-specific CAV systems, and
field studies of direct and network-assisted paths. It no longer calls these
lines `foundations`. The section identifies a protocol gap and an evidence gap.
The protocol gap is the absence of one application protocol for stateless
J2735-based CV/ITS messages and stateful CAV operations. The evidence gap is
that existing studies do not determine whether PC5 and Uu can each carry both
workload classes or identify the payload sizes and network conditions at which
either path misses application deadlines.

The standards discussion preserves the functions already provided by the 3GPP
V2X Application Enabler and ETSI Multi-access Edge Computing specifications.
It makes the narrower claim that J2735 standardizes individual messages, while
the network-service standards leave stateful CAV application messages to
individual applications. The application-system and field-study discussions
now identify cited systems through their authors, including Wu et al. for
Tentacles, Asabe et al. for AutowareV2X, and Demircioglu for the 40-km 5G
Standalone corridor study. The corridor paragraph retains its relevant
measurement scope but removes the annotated common-success-criterion detail.

Figure 1 now presents existing technologies and studies, the two unresolved
gaps, and future communication requirements. The same protocol and evidence
logic was applied to the Abstract, Introduction, IPI Protocol Design, System
Design and Experimental Setup, Conclusion, and the binding paper outline.
Across these sections, IPI is described as a transport-independent application
protocol for stateless CV/ITS applications and stateful CAV applications. No
experiment result, citation, TDD evidence classification, or page-limit target
was changed, and no page-limit optimization was performed.

Updated files:

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`

Validation: a forced LaTeX/BibTeX rebuild completed without compilation errors,
undefined citations, undefined references, rerun requests, or overfull boxes.
The rebuilt PDF contains 18 US-Letter pages as a build fact, zero annotations,
zero links, zero embedded files, no title or author metadata, and fully embedded
fonts. All 18 pages were inspected in a rendered contact sheet; pages 1--4 and
14 were also inspected at high resolution. No overlap, clipping, missing
figure, or broken table was found. The revised PDF SHA-256 is
`0ffac807ea8590298645dabcc8a443747ff1f595150e3125c962943885da6269`.

## Additional Wording Annotation Revision

Complete. The three new substantive annotations on pages 1--2 of
`paper/current_manscript/main.pdf` were treated as revision instructions. The
stateless-update definition now compares messages directly: a newer message
supersedes the previous message without preserving an application session. The
confusing statement that IPI `places` two workload classes at an application
boundary was removed. The Introduction now moves from the contribution list to
the experimental result directly by stating that the evaluation identifies the
payload sizes and operating conditions at which each path no longer meets the
required response latency or availability.

The third insight now states that cellular networks must support CAV and ITS
applications that advance the goal of zero road fatalities. This replaces the
awkward claim that the networks support `CAV and ITS contributions`. The same
wording was corrected in the Abstract, Introduction, Results discussion, and
binding outline. Related interface wording in IPI Protocol Design and System
Design was also simplified from an abstract `application boundary` to a common
application interface and the application forms that it represents. No
experiment result, TDD evidence classification, or page-limit target was
changed.

Updated files:

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`

Validation: the full LaTeX/BibTeX rebuild completed without compilation errors,
undefined citations, undefined references, rerun requests, or overfull boxes.
The rebuilt PDF contains 18 US-Letter pages as a build fact, zero annotations,
zero links, zero embedded files, no title or author metadata, and fully embedded
fonts. All pages were rendered for visual inspection; pages 1--4 and 14 were
also inspected at 180 dpi. No overlap, clipping, missing figure, or broken table
was found. The clean PDF SHA-256 is
`132b88078a7b6130c629e901b3efa88fd2cf0be4e351c6c098d1261a2f79d08d`.
The annotated input is preserved at
`tmp/pdfs/additional_comments_round2/main_annotated_before_revision.pdf` with
SHA-256
`4ca28222f7984a171e3fdefc8a1c10abd22417b5f993e8d5bef9a136e7d1e5a4`.

## Content-Flow Annotation Revision

Complete. The five new substantive annotations in
`paper/current_manscript/main.pdf` were treated as revision instructions. The opening
argument was rebuilt around one causal sequence: vehicles need information
distributed across other vehicles and roadside infrastructure; applications
exchange that information through stateless updates and stateful operations;
direct PC5 and network-assisted Uu use different network resources; and the
paper asks where each path stops meeting the application requirement. The
annotated filler statements about connection status and generic application
criteria were removed rather than rephrased.

The Abstract now introduces stateless J2735 updates and stateful CAV exchanges
before stating the interface and measurement gaps. The same distinction was
carried through Related Work, IPI Protocol Design, System Design and
Experimental Setup, Experiment Results, the Conclusion, and the appendices.
Application state and payload size remain separate concepts: the manuscript
uses payload sizes representative of the two workload classes instead of
defining stateless traffic as small and stateful traffic as large. No page-limit
optimization was performed, and no TDD evidence classification or experiment
result was changed.

Updated manuscript files:

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/main.pdf`

Validation: a full LaTeX/BibTeX rebuild completed without compilation errors,
undefined citations, undefined references, rerun requests, or overfull boxes.
The PDF contains 18 US-Letter pages as a build fact, zero annotations, zero
links, zero embedded files, no title or author metadata, and fully embedded
fonts. All pages were rendered for visual inspection; pages 1--2 were also
checked at 180 dpi. No overlap, clipping, missing figure, or broken table was
found. The rebuilt PDF SHA-256 is
`c0745d18916b6f44906deb0def81df2932c3a7c6da187e4650bb930b466483a2`.
The annotated input is preserved at
`tmp/pdfs/content_flow_annotation_revision/main_annotated_before_revision.pdf`
with SHA-256
`78af21d741fc06c5f907c29748dc1a638d1704f2ab67310529e11b1ee8d7bc60`.

## Transition-Mistake Writing Lesson

Complete. `paper/general_academic_writing_lessons.md` now records the
generalized mistake exposed by the annotated manuscript: treating a transition
problem as a word-choice problem instead of repairing the relationship between
the ideas. Lesson 8 now distinguishes cause, consequence, contrast,
qualification, extension, and sequence; identifies common false uses of each;
and requires a proposition-level test before a connector is selected. It also
records that a repeated technical subject or a precise backward-pointing phrase
can provide a stronger transition than a generic conjunctive adverb.

The transition-audit procedure and final checklist were updated to require
two-sided verification at sentence, paragraph, and section boundaries.
Validation: Markdown whitespace checks and `git diff --check` pass. No
manuscript source, experiment record, figure, or PDF was changed for this
documentation-only update.

## Transition-Logic Annotation Revision

Complete. The seven substantive annotations in
`paper/current_manscript/main.pdf` were treated as revision instructions. The marked
Introduction passages were rebuilt around their actual logical relationships:
the opening now gives one shared information need for automated and
human-driven vehicles; the traffic-controller and cooperative-maneuver ideas
are separate; V2X and 5G are linked directly to the external information they
carry; and the CV/CAV distinction uses individual J2735 messages versus
multiple messages associated with one CAV operation. Assistant-created wording
such as `replaceable updates` was removed.

The same error pattern was audited across the Abstract, Introduction, Related
Work, IPI design, experimental setup, Results, Future Research Directions,
Conclusion, and appendices. Decorative uses of contrast, cause, consequence,
and extension words were removed. Sentence and paragraph links now use repeated
technical subjects, backward-pointing phrases, explicit evidence-to-claim
relations, and true experimental sequence. Long compound sentences were split
when the clauses did not express one necessary relationship. No page-limit
optimization was performed, and the stored TDD evidence classification remains
unchanged.

Updated manuscript files:

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/main.pdf`

Validation: a forced LaTeX/BibTeX rebuild completed without compilation errors,
undefined citations, undefined references, rerun requests, or overfull boxes.
All 18 pages were rendered at 150 dpi and visually inspected; no overlap,
clipping, missing figure, or broken table was found. The rebuilt PDF contains
zero annotations, zero links, zero embedded files, and fully embedded fonts.
The revised PDF SHA-256 is
`001cedebf1308d257973aa95356a6c2b70be7e9c5cf0f320a6c86d97549a7c3a`.
The annotated input is preserved at
`tmp/pdfs/transition_annotation_revision/main_annotated_before_revision.pdf`
with SHA-256
`46f3ace44cb4160ad734ca1ca50b336a28b8fe74d9834d558f34460078ccb200`.

## TDD Data Classification

Complete. On 2026-08-15, the user classified every retained experiment that
could be interpreted as a `40/40/20` versus `70/20/10` comparison as
**inconclusive for TDD inference**. The application RTT and availability
measurements remain valid observations of their recorded operating conditions,
but they cannot rank the two profiles, estimate a TDD effect, or support a
causal or sensitivity claim about directional frame allocation. The
`70/20/10` host-side direction control remains a valid single-profile
uplink/downlink measurement, but it is also inconclusive about the effect of
TDD because no matched `40/40/20` control was collected.

The machine-readable decision record is
`results/real_5g/tdd_comparison_status.json`. The experiment summaries,
analysis summary, agent context, and remaining-experiment runbook carry the
same status. This classification supersedes any earlier task entry that calls
the retained data a TDD comparison or TDD sensitivity result. Per the user's
instruction, no manuscript TeX, paper outline, figure, caption, generated
figure, or PDF was changed. A new same-placement matched collection will
replace this inconclusive evidence when it is received and validated.
Validation: the status JSON parses successfully, `git diff --check` passes,
and no file under `paper/current_manscript/` was modified.

## Pending Matched TDD Collection And Manuscript Continuation

Active. On 2026-08-15, the user determined that the retained historical
`40/40/20` and follow-up `70/20/10` application campaigns cannot isolate the
effect of the time-division duplexing (TDD) allocation because their
placement/signal contexts differ. The user classified their TDD inference as
inconclusive. The measurements remain valid observations of their recorded
operating conditions, but they must not be used to rank the two TDD profiles or
claim that one profile caused the observed difference.

The user will provide a new matched data set that compares `40/40/20` and
`70/20/10`, ordered as downlink/uplink/dynamic, while holding the vehicle,
MG52 placement and orientation, fixed location, Cell 2 lock, 40-MHz n48
channel, gNodeB/core/MX250/d1 path, `10D4G` setting, sole-UE clean-band
condition, transport, application version, workload, request interval,
timeout, and background condition constant. If multiple signal/placement
conditions are collected, both profiles must be measured at each condition.
The collection should use balanced profile order, three repetitions where
possible, and 1,000 attempts for each combination of profile, 1,024-B or
23,968-B request payload, and TCP or MQTT transport. The primary matrix
therefore contains 24,000 attempts. Reconfiguration, reattachment,
stabilization, and warm-up intervals remain outside the measurement windows.

Every application attempt has the existing uplink-oriented transaction:
the vehicle sends an N-byte IPI request to d1, and d1 returns a compact
correlated application acknowledgment. The user accepts this scope. The
current paper does not require a download-heavy reverse-payload experiment.
Consequently, every manuscript claim, figure, caption, and table must identify
the measured vehicle-to-edge request and compact edge-to-vehicle acknowledgment
instead of implying N-byte transfers in both directions.

The binding collection and analysis requirements are recorded in
`remaining_exp.md` under R8 and R9. No manuscript source was changed by this
documentation update. The next active work is continued manuscript revision;
the matched TDD result will be incorporated after the new artifacts arrive.
Validation: `git diff --check` passed, and this documentation update changed
only `current_task.md` and `remaining_exp.md` inside the repository.

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
- `paper/current_manscript/Makefile`
- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/legacy_draft/figs/measurements/section5_tdd_sensitivity.pdf`
- `paper/current_manscript/main.pdf`

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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`

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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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
No page-limit optimization was performed, and `paper/current_manscript/28p.pdf`
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

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`

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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## New Page-1 Annotation Review

Complete as a read-only review. The current `paper/current_manscript/main.pdf` has
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
`paper/current_manscript/main.pdf` were treated as revision instructions. The abstract
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

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

Complete. The five highlight comments in the prior `paper/current_manscript/main.pdf`
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

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/main.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/paper_outline.md`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: two stable LaTeX builds succeed without undefined citations or
references, changed-label requests, overfull horizontal boxes, compilation
errors, or fatal errors. Pages 1, 2, 3, 6, 9, 12, and 13 were rendered at 200
dpi and inspected. The new title, Introduction question and contribution,
Related Work caption, latency definition, Results transitions, Future Research
Directions, conclusion, and reference transition are legible without overlap,
clipping, or crowding. The final PDF remains 16 US-Letter pages. Its SHA-256 is
`c1e40321ab5fb0173c79952c939cdde2db0067968d944b3071b6acf927da561c`.
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: the full LaTeX build succeeds with stable cross-references. Page 1
was rendered at 220 dpi and inspected; the revised three-paragraph abstract and
the transition into the Introduction are legible without overlap, clipping, or
crowding. The PDF remains 16 US-Letter pages. The final PDF SHA-256 is
`c73030976e95f93f4bf431b6c8906205fb82bf9482504b8c0e1afcbde36a5a24`.
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/figs/section5_5g_deadline_envelope.pdf`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.

## Table 1 Caption Correction

Complete. The Table 1 caption was reduced from an editorial pointer to the
descriptive title `Deployment and measurement context.` The removed sentence
directed readers to Appendix B instead of describing the table. A complete
caption scan found no other table or figure caption that directs the reader to
an appendix, so no additional captions were changed.

Updated files:

- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/legacy_draft/figs/maps/fig-signal-osm-runs.svg`
- `paper/legacy_draft/figs/maps/fig-signal-osm-runs.png`
- `scripts/build_signal_strength_maps.py`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/figs/section5_cross_application_comparison.pdf`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/figs/section5_cross_application_comparison.pdf`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/figs/section5_concurrency_interruption.pdf`
- `paper/current_manscript/main.pdf`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/figs/section5_mixed_load_qos.pdf`
- `paper/current_manscript/main.pdf`
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
`paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/current_manscript/main.pdf` has SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/legacy_draft/figs/measurements/section5_tdd_sensitivity.pdf`
- `paper/current_manscript/main.pdf`
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
metadata. The preserved `paper/current_manscript/28p.pdf` remains byte-identical with
SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/current_manscript/main.pdf` has SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/figs/section5_protocol_completion.pdf`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 8 and manuscript page 9 were rendered at high resolution
and inspected; both axes, legends, path-condition titles, data series, reference
lines, caption, and adjacent text are legible without overlap or clipping. The
final log contains no undefined citation/reference, changed-label request,
overfull box, compilation error, or fatal error. The PDF remains 16 US-Letter
pages; core content ends on page 12, references begin on page 13, and
appendices begin on page 15. The PDF contains no annotations, embedded files,
or author metadata. The preserved `paper/current_manscript/28p.pdf` remains
byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/current_manscript/main.pdf` has SHA-256
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

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/figs/section5_5g_deadline_envelope.pdf`
- `paper/current_manscript/main.pdf`
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
metadata. The preserved `paper/current_manscript/28p.pdf` remains byte-identical with
SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/current_manscript/main.pdf` has SHA-256
`8a22da66dca494a82161691b0e25f8dce5d8d3a5b509df9cb9caf32a0f950b0a`.

## Figure 6 Layout Correction

Complete. Figure 6 retains the same PC5 payload, reference-point RTT, and
mobile-route values, but its layout no longer places labels on top of one
another. Panel (a) allocates more width to the heatmap and rotates the five
stationary-point labels. Panel (b) places the p50, p95, and p99 legend in unused
plot space, separate from the 10-, 25-, and 100-ms reference lines.

Updated files:

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/figs/section5_pc5_payload_route.pdf`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: the figure generator and full LaTeX build complete successfully.
The standalone Figure 6 and manuscript page 8 were rendered at high resolution
and inspected; the stationary-point labels, percentile legend, deadline
references, axes, values, caption, and adjacent Figure 7 are legible without
overlap or clipping. The final log contains no undefined citation/reference,
overfull box, compilation error, or fatal error. The PDF remains 16 US-Letter
pages with embedded fonts, no annotations, and no embedded files. The preserved
`paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/current_manscript/main.pdf` has SHA-256
`dabbfc09a9646cb60830815d67ced04e9bb8010a4e8f8730e8fbee6d683056dc`.

## Figure 4 OpenStreetMap Revision

Complete. Figure 4 now directly reuses the existing publication-oriented
`paper/legacy_draft/figs/maps/fig-signal-osm-runs.png` rather than reconstructing a new map. Its
three OpenStreetMap panels show triangulated spatial interpolation of the
separate-handset RSRP, RSRQ, and reported-SNR samples and identify the five
stationary Uu workload collections. The system-design prose, caption, and
accessibility description define those contents and preserve OpenStreetMap
attribution. The temporary duplicate map-rendering code is no longer used.

Updated files:

- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/Makefile`
- `paper/current_manscript/main.pdf`
- `current_task.md`

Validation: the full LaTeX build completes successfully. Figure 4 and
manuscript page 5 were rendered at high resolution and inspected; all three
maps, metric scales, sample and run markers, caption, and attribution are
legible without clipping or overlap. The final log contains no undefined
citation/reference, overfull box, or compilation error. The PDF remains 16
US-Letter pages with embedded fonts and no annotations. The preserved
`paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The revised `paper/current_manscript/main.pdf` has SHA-256
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

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
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
The preserved `paper/current_manscript/28p.pdf` remains byte-identical with SHA-256
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

- `paper/current_manscript/sections/00_abstract.tex`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/sections/08_appendices.tex`
- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/proposed_changes.md`

Validation: the figure-generation script and `make all` completed successfully.
The final LaTeX log has no undefined citation/reference, overfull box, or
compilation error. The current PDF contains 16 US-Letter pages, uses embedded
fonts, starts references on page 13, starts appendices on page 15, and contains
no annotations or identifying author metadata. Pages 1, 3--9, 11--12, and 15
were rendered and visually inspected; the revised text and figure labels are
legible and have no clipping, overlap, or broken layout. The preserved full
draft remains byte-identical at
`paper/current_manscript/28p.pdf`, SHA-256
`6b4d18f3e50d722560f565ec37f2ace30e80ade484776684c75c8c53b63a6307`.
The corrected current PDF is `paper/current_manscript/main.pdf`, SHA-256
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
`paper/current_manscript/28p.pdf`, and the current source builds the condensed paper as
`paper/current_manscript/main.pdf`. Core content occupies pages 1--12, references
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
`paper/current_manscript/proposed_changes.md`; none is claimed as current behavior.

## Final Condensed-Draft Validation: 2026-08-13

- Current manuscript: `paper/current_manscript/main.tex` with Sections 0--7 and
  post-bibliography appendices A--C.
- Current rendered draft: `paper/current_manscript/main.pdf`.
- Preserved full draft: `paper/current_manscript/28p.pdf`.
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
  `paper/current_manscript/proposed_changes.md` require user approval plus
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
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/proposed_changes.md`
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
Makefile, and compiled artifacts. `paper/current_manscript/` is the only active draft
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
  remains unapplied as P-001 in `paper/current_manscript/proposed_changes.md`.
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
  `paper/current_manscript/proposed_changes.md` for an enforced cross-transport session
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
- P-003 was added to `paper/current_manscript/proposed_changes.md` for monotonic timing,
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

- `paper/current_manscript/main.tex`
- `paper/current_manscript/Makefile`
- `paper/current_manscript/references.bib`
- `paper/current_manscript/sections/01_introduction.tex`
- `paper/current_manscript/sections/02_related_work.tex`
- `paper/current_manscript/sections/03_ipi_protocol_design.tex`
- `paper/current_manscript/sections/04_system_design_setup.tex`
- `paper/current_manscript/sections/05_experiment_results.tex`
- `paper/current_manscript/sections/06_future_research_directions.tex`
- `paper/current_manscript/sections/07_conclusion.tex`
- `paper/current_manscript/scripts/build_section5_figures.py`
- `paper/current_manscript/figs/section5_pc5_payload_route.pdf`
- `paper/current_manscript/figs/section5_5g_deadline_envelope.pdf`
- `paper/current_manscript/figs/section5_protocol_completion.pdf`
- `paper/current_manscript/figs/section5_mixed_load_qos.pdf`
- `paper/current_manscript/figs/section5_concurrency_interruption.pdf`
- `paper/current_manscript/proposed_changes.md`
- `paper/current_manscript/main.pdf`
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

- `make` succeeds from `paper/current_manscript/` using `pdflatex` and `bibtex`.
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

- `make -C paper/current_manscript clean all` succeeds.
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
  `paper/current_manscript/` only. Its brief explicitly excludes page-count criticism
  and forbids use of `paper/legacy_draft/`.

## Active Task Update: Year 2 Implementation Report Evidence Refresh

Task update requested 2026-08-19: read the newly retained experiment data,
summarize it at the strength supported by the artifacts, refresh the derived
PCA and successful-response variance analyses, and update the Year 2 proposal
implementation report.

Status: complete. The report at
`proposals/Project_Implementation_Report_TIER_IV-UD-IPI_Year2.docx` now covers
the retained evidence through August 19, 2026. The revision adds the matched
August 17 downlink-heavy TDD comparison, exact bidirectional 50 MiB endpoint
goodput, DU Cell 2 traffic context, the matched August 18 weak-signal direction
pair, and the August 19 G-NetTrack route context. It also refreshes the full
condition ledger, PCA, matched-factor ranking, successful-response variance,
application decisions, evidence boundaries, reproducibility manifest, and
proposal-commitment closure.

Key evidence retained in the report:

- The condition ledger contains 429 rows, 399 private-5G Uu conditions, 30
  historical direct-PC5 conditions, and 1,769,863 issued attempts.
- The matched August 17 comparison contains eight matched conditions and 8,000
  exchanges per profile, all accepted. The median absolute p95 difference
  between the 70/20/10 and 40/40/20 profiles is 1.14x and the maximum is
  1.47x; the sign
  changes with payload, transport, and percentile, so it does not support a
  universal profile ranking.
- Exact 50 MiB endpoint goodput is 12.000 versus 2.836 Mb/s for vehicle-to-d1
  upload and 126.786 versus 99.756 Mb/s for d1-to-vehicle download under the
  recorded 70/20/10 and 40/40/20 profiles, respectively. These are endpoint
  flow measurements, not PHY-capacity estimates.
- The matched August 18 500 KiB MQTT direction pair holds profile, location,
  serving cell, reported signal, transport, and object size constant. Its
  uplink-heavy versus downlink-heavy p95 ratio is 40.95x, establishing
  application direction as a major conditional factor without identifying a
  scheduler mechanism.
- The G-NetTrack route analysis retains 1,178 valid NR RSRP/ssSinr joins after
  excluding a 123-sample frozen-radio plateau. Because the phone and MG52 are
  separate unsynchronized UEs, these samples are spatial context rather than
  per-request radio telemetry or a causal latency model.
- PCA uses 420 eligible conditions. PC1 explains 60.9935% and is the main
  latency/tail outcome axis; PC2 explains 20.4769% and is dominated by failure
  fraction. The first three components explain 99.49%.
- Successful-response variance uses 1,749,930 finite RTTs from 420 equally
  weighted conditions. On log10 RTT, 91.117% of variance lies between
  conditions and 8.883% within conditions; on raw RTT squared, 66.322% lies
  within conditions because rare seconds-scale tails dominate. Median
  condition p95/p50 is 1.428 and median coefficient of variation is 0.285.

Validation completed for this refresh:

- `scripts/analyze_cav_limiting_factors.py` and
  `scripts/analyze_successful_ipi_variance.py` were refreshed and run against
  the current retained evidence; the generated CSV, JSON, Markdown, and figure
  artifacts were regenerated.
- The updated DOCX is a valid ZIP/Open XML package. It contains 14,265 words,
  41 tables, 11 numbered figures, 65 Heading 1/Heading 2 paragraphs, and one
  US-letter section.
- The DOCX accessibility audit reports zero high-, medium-, or low-severity
  findings.
- The report was rendered with LibreOffice 7.3.7.2 to a 31-page US-letter PDF.
  All 31 pages were inspected at original detail; no clipping, overlap, broken
  table, missing glyph, misplaced figure, or unintended blank page is visible.
- A targeted content audit found none of the superseded headline condition,
  attempt, PCA, variance, or factor-ranking values in the report.
- The final DOCX SHA-256 is
  `22e34628f8fc4339257a0cce9ee830104579e5c36422baac5e244844a45a8988`.

## Active Task Update: 10-Page Year 2 Implementation Report

Task update requested 2026-08-19: create a separate 10-page version of the
Year 2 proposal implementation report using the current repository evidence,
including the newly retained August 19 GL-X3000 directional campaigns.

Status: complete. The condensed report is retained at
`proposals/Project_Implementation_Report_TIER_IV-UD-IPI_Year2_10_Page.docx`.
The 31-page source report was preserved unchanged.

The 10-page report contains:

- the implemented IPI contract, session lifecycle, wire validation, PC5
  adapter, activation, tiered service policy, offload decision, fallback, and
  ROS 2 decision-publication boundaries;
- separate experimental methods and claim boundaries for private-5G Uu and
  historical direct-PC5 measurements, including the explicit statement that
  the historical PC5 field runs used a sequence-matched vendor packet/echo
  object rather than the later host-validated formal J2735/IP5X adapter;
- the refreshed condition ledger: 461 conditions, 431 private-5G Uu
  conditions, 30 direct-PC5 conditions, 1,785,863 issued attempts, and 452
  PCA-eligible conditions;
- the August 19 GL-X3000 matched 70/20/10 versus 40/40/20 campaigns, including
  16 completed RTT conditions and three exact 50 MiB transfers per direction
  under each profile, with no accepted-response failures in the completed RTT
  cells;
- the refreshed PCA and matched-factor result: PC1 explains 61.11% of
  standardized outcome variance and is the accepted-response latency/tail
  axis; payload/representation has the largest broad matched p95 shift at a
  6.49x median, while direction reaches 40.92x and matched TDD reaches 4.45x
  in specific conditions;
- the refreshed successful-response analysis: 1,765,930 finite RTTs from 452
  equally weighted conditions, 91.2% between-condition and 8.8%
  within-condition variance on log10 RTT, and a median condition p95/p50 ratio
  of 1.419; and
- CAV/ITS application decisions, limits on causal radio and hardware claims,
  proposal work-package status, evidence routing, and consolidated
  conclusions.

Validation completed for the 10-page report:

- the DOCX is a valid ZIP/Open XML package and retains one portrait US-letter
  section with the source report's header, footer, margins, and style system;
- the rendered output is exactly 10 pages and contains 3,549 words, seven
  numbered inline figures, 10 Heading 1 paragraphs, and 12 Heading 2
  paragraphs;
- all 10 rendered pages were inspected at original detail; no clipping,
  overlap, split-row defect, missing glyph, unintended blank page, or
  unreadable figure is visible;
- the DOCX accessibility audit reports zero high-, medium-, or low-severity
  findings;
- the original 31-page report remains byte-for-byte unchanged at SHA-256
  `22e34628f8fc4339257a0cce9ee830104579e5c36422baac5e244844a45a8988`;
  and
- the final 10-page DOCX SHA-256 is
  `ba9c3390ee2bbffdf2bac09794ac44a182c3e848437e7255b0ab59a3f283f27f`.
