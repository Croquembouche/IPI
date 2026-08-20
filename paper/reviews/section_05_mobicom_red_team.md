# Section 5 MobiCom Red-Team Review

**Scope reviewed.** Current files only: `AGENTS.md`; `experiment_summary.md`; `paper/paper_outline.md` Section 5; `paper/current_manscript/sections/04_system_design_setup.tex`; `paper/current_manscript/sections/05_experiment_results.tex`; the current rendered `paper/current_manscript/main.pdf` pages 17--23; current result summaries/CSVs; the current Section-5 figure-generation source; and the current private-5G probe source.  No legacy draft was used.  This is a read-only manuscript/code review; the only new artifact is this review.

## Overall decision

**Consensus: Weak Reject in its current form; conditional Weak Accept after the must-change evidence-boundary repairs below.**

The section is unusually careful about several limitations that commonly invalidate a V2X paper: it distinguishes RTT from one-way delay, labels the 5G endpoint as a decoded accepted ACK rather than semantic completion, does not claim proved 5QI treatment, marks operator-stopped PC5 cells, separates aggregate from median per-client tails, confines the three insights to Section 5.9, and says explicitly that this is not a public-network, multi-UE, handover, or PC5-to-Uu-failover study.  The numbers checked below match the current artifacts.

However, a MobiCom reviewer can still reasonably read parts of the mixed-load and insight text as stronger than the evidence.  Most importantly, a configured **25-Mbit/s offered** background generator is not a demonstrated 25-Mbit/s sustained load, particularly in weak-path collections; and an accepted probe ACK is not a correlated or semantically completed IPI service.  Those two boundaries need to be made impossible to miss in the result narrative, captions, and conclusions.  The main figure-level improvement is to show availability as client count grows, not just aggregate p95 with endpoint availability annotations.

| Reviewer | Vote now | Conditional vote after must-change repairs | Main reason |
|---|---:|---:|---|
| A. CAV/CV/ITS application-requirements reviewer | Weak Reject | Weak Accept | The paper correctly calls its tests proxies, but several concluding phrases can be read as completed CAV-service support rather than necessary communication conditions. |
| B. Cellular/RAN/V2X measurement reviewer | Weak Reject | Weak Accept | The configured 25-Mbit/s load, host-side direction control, and weak-field labels need sharper provenance and causality boundaries. |
| C. MobiCom systems-methodology reviewer | Weak Reject | Weak Accept | Core measurement definitions are soundly disclosed, but heterogeneous aggregates and the concurrency figure obscure the availability-versus-demand result. |

## Evidence audit: what the current section gets right

The following claims and numbers were audited against current artifacts.  They are strengths to preserve, rather than invitations to weaken the result.

| Manuscript claim | Current-file evidence | Audit result |
|---|---|---|
| RTT is sender-observed cycle time; 5G is a decoded accepted ACK, not a semantic result, one-way delay, or enforced request/session correlation. | Section 5, lines 4--14 and 37--55; Section 4, lines 123--140 and 390--424.  The ACK struct has sequence/timing/frame/result fields but no request or session identifier (`cpp/include/ipi/api/private_5g_latency_probe.hpp:16-43`; `cpp/src/api/private_5g_latency_probe.cpp:306-340`). | Correct and essential.  Preserve this definition verbatim in any revision. |
| A sender records an accepted ACK without checking that it correlates to its request/session or demonstrates service execution. | TCP and UDP send paths decode and record the ACK (`cpp/examples/library/private_5g_latency_sender.cpp:517-529,556-568`); receiver accepts after frame inspection (`cpp/examples/library/private_5g_latency_receiver.cpp:168-186`); default experiment logging starts with `serviceSuccess=true` (`cpp/include/ipi/api/experiment_logging.hpp:14-30`). | Correct limitation.  It conflicts with any prose that implies a completed remote planning/offload service. |
| Conventional SPaT/BSM and local IPI interoperability have the narrowly stated outcomes in Table 5. | Section 5, lines 19--55; `results/mocar_v2x/20260703_setup_test/summary.md:80-94`. | Correctly scoped as bridge/callback/observation and local format validation, not wireless delivery or a field interoperability certification. |
| PC5 payload results, including stopped/skipped phases, have the reported p95 and availability. | Section 5, lines 60--104; `experiment_summary.md:405-461`; PC5 sources under `results/mocar_v2x/`.  Figure source explicitly preserves stopped cells as asterisks (`paper/current_manscript/scripts/build_section5_figures.py:50-135`). | Correctly distinguishes observed loss from operator-stopped/unexecuted cells.  Do not turn skipped cells into measured zero-delivery trials. |
| Mobile PC5 route values are descriptive and not attributed to mobility or RSSI. | Section 5, lines 106--122; `experiment_summary.md:483-507`. | Correct.  The device lacks aligned signal evidence, and the section says so. |
| The 5G deadline-envelope counts and percentiles are aggregate field/load groups, not a one-way application result. | Section 5, lines 124--171; `experiment_summary.md:209-236`; `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/summary.md:1-50`; Figure 6 source at `build_section5_figures.py:138-196`. | Headline values match: compact 60,000/60,000 with A(100)=37.76%, loaded 34,000 with A(100)=24.12%, map 18,000 with A(100)=0.99%, and bulk 14,000 with A(500)=0%.  The heterogeneity caveat must remain prominent. |
| Protocol-comparison numbers distinguish raw UDP from application fragmentation and correctly avoid a sustained 10-Hz conclusion. | Section 5, lines 173--218; `experiment_summary.md:238-289`; Figure 7 source at `build_section5_figures.py:199-233`. | Correct.  The 60-KiB calculation is an offered-wire-rate illustration, not a measured sustained-rate experiment. |
| Good-path mixed-load A(100)/A(200), weak-path values, and p95 values match sender CSV recomputation. | Section 5, lines 220--282; `results/real_5g/20260701_load_qos_run_{1,2,3,4}/summary.md`; Figure 8 source at `build_section5_figures.py:236-284`. | Correct numerical transcription.  Interpretation of the background load needs repair below. |
| Logical-client results distinguish aggregate p95 from derived median client-specific p95/p99 and state one host/one UE. | Section 5, lines 284--335; Section 4, lines 32--47 and 616--651; `results/real_5g/20260702_multiclient_scalability_run_4/summary.md:23-54`; `results/real_5g/20260706_multiclient_weak_signal_run_1/summary.md:21-45`. | Correctly avoids claiming 100 vehicles or independent UE schedulers.  Figure 9 needs a more direct availability-by-client visualization. |
| Restart counts, p99, gap, and A(200) values are correct and there is no fallback claim. | Section 5, lines 337--355; `results/real_5g/20260703_failure_fallback_run_1/summary.md:1-42`. | Correct.  The 10-s outage is configured, not recovered by a demonstrated PC5/Uu fallback. |
| The section contains exactly three insights, all in 5.9. | Section 5, lines 357--405; Section 5 outline, lines 3816--3924. | Correct and compliant with the outline's organization rule. |

## Reviewer A — CAV/CV/ITS application-requirements assessment

### A1. Must change: prevent ACK success from becoming an implied completed CAV service

**Risk.** The section's metric definition is precise, but a few later phrases erase that precision.  In particular, “both conventional-message and service-style paths function” (Section 5, lines 54--55), “support important CV and CAV communication functions” (lines 360--366), and “IPI supplies common application semantics” (lines 393--399) can be read as evidence that a service request was correlated, executed, and returned correctly.  It was not.

The current probe receiver only inspects an IPI frame and returns an accepted ACK; its ACK does not carry `requestId` or `sessionId`, and sender code does not compare a returned sequence/timestamp to the sent request.  The sender's nominal service payload is a repeated-byte offload payload rather than a semantic remote-planning execution (`cpp/examples/library/private_5g_latency_sender.cpp:258-281`).  This is exactly why Section 4's contract properly calls the measure a necessary, not sufficient, application condition (Section 4, lines 390--424).

**Safe replacement text.**

Replace Section 5, lines 54--55 with:

> Consequently, the conventional-message path and the IPI envelope/ACK path function, but this format-level interoperability alone does not establish a semantically completed service or an application deadline.

Use the following tightening in Insight 1 (lines 360--366):

> Today's measured paths support selected communication functions and necessary communication conditions for CV/CAV applications, but readiness remains application-specific.

Use this first sentence in Insight 2 (lines 385--391):

> Today's 5G can carry some data exchanges relevant to complex CAV applications, but the usable data-size and deadline envelope is application-specific.

Replace the final IPI sentence (lines 393--399) with:

> IPI supplies a common application-object vocabulary; future communication support would need to combine deadline, freshness, direction, traffic behavior, completeness, and failure state with verified representation, path, scheduling, admission, and fallback decisions.

**If authors want the stronger claim.** It requires implementation and experiment, not prose: ACK request/session identity (or a validated digest), sender-side verification, a real service invocation and semantic response, plus an end-to-end application-correctness experiment.

### A2. Must change: state that 25 Mbit/s is an offered target, not observed coexistence load

**Risk.** Section 5, lines 220--282 and Figure 8 use a “25-Mbit/s” background generator and call it sustained vehicle-originated traffic.  The favorable repeats only achieved 1.320--2.267 Mbit/s at the host, depending on transport: `results/real_5g/20260701_load_qos_run_1/summary.md:24-32` and `...run_2/summary.md:23-33`.  The one/two/four stream follow-up achieves 1.990--5.743 Mbit/s, not 25 Mbit/s (`...run_3/summary.md:26-37`).  In the weak W1 collection, completed client-side summaries show 0--0.322 Mbit/s; W2 retains latency artifacts but has no comparable aggregate achieved-rate summary (`...weak_signal_run_1/summary.md:34-53`; `...weak_signal_run_2/summary.md:46-50`).

This matters for CAV interpretation.  The study demonstrates foreground acknowledgement behavior while a best-effort generator is active.  It does **not** demonstrate an application envelope under 25-Mbit/s vehicle upload, a controlled utilization sweep, or coexistence at a verified background rate.

**Safe replacement text.** Add after the first mixed-load setup sentence (after Section 5 line 240):

> The background generator was configured with a 25-Mbit/s offered target, but the two favorable repeats achieved only 1.320--2.267 Mbit/s at the vehicle host.  In W1, completed client-side summaries report 0--0.322 Mbit/s, and W2 has no comparable aggregate achieved-rate summary.  We therefore interpret these data as foreground response behavior while a best-effort background generator was active, not as a 25-Mbit/s coexistence guarantee or a controlled load-versus-rate curve.

Add to Figure 8's caption (Section 5, lines 263--278):

> “+UL” denotes an active background generator, not sustained 25-Mbit/s traffic; favorable repeats achieved 1.320--2.267 Mbit/s, W1's completed client summaries show 0--0.322 Mbit/s, and W2 has no comparable aggregate achieved-rate summary.

### A3. Recommended: make proxy/threshold language consistent with the application table

The paper's application thresholds are explicitly analytical rather than certification criteria (Section 4, lines 426--450 and 560--568), and the section correctly avoids treating RTT as one-way delay.  Keep that discipline in headings and takeaways.  “5G Object Size and Application Deadline” (Section 5 line 124) sounds closer to a payload-only application qualification than the experiment supports, because each row pools field/load/protocol conditions.

Suggested heading: **“5G Evidence Groups: Object Scale and Application Deadline.”**  Suggested conclusion sentence (replace lines 169--171):

> Across the evaluated combinations, acknowledged-cycle availability contracts tighten sharply at lower budgets and for some larger objects; the figure is not a monotonic payload-only curve or an application certification result.

### A4. Positive review finding

Do not add claims of formal SAE J2735 ASN.1/UPER conformance, standardized regional extensions, CV-service certification, or PC5/Uu fallback.  The present Section 5 appropriately calls the PC5 cooperative packet custom and separates it from local standards-bridge evidence.  That restraint is appropriate and should remain.

## Reviewer B — cellular/RAN/V2X measurement-rigor assessment

### B1. Must change: do not let weak-path labels imply independently verified RF causality

The paper correctly says it has no scheduler/PRB/MCS/TDD causal evidence (Section 5, lines 220--235) and appropriately does not attribute route variation to mobility (lines 106--122).  It should use the same caution for the 5G weak-field wording.  The current R1 validation artifact is still marked `pending_airspan_and_mg52_artifacts` (`results/real_5g/20260805_airspan_r1_run_1/validation_summary.json:1-24`), while host-side direction values of 13.118 and 25 Mbit/s are supported (`:25-39`).  `experiment_summary.md:310-376` likewise records unmatched/location-associated evidence and pending management-plane confirmation.

**Required scope repair.** Change “weak-field condition” / “weak path” where it reads as measured RF state to **“weak-path collection”** or **“location-associated weak-path collection”**, unless a nearby clause says the RF evidence is unavailable/unmatched.  In the direction-control paragraph, add one compact provenance statement such as:

> These are host-side direction controls; the Cell 2 / 70/20/10 context is operator-reported, and current management-plane evidence remains pending.

This does not invalidate the measured sender behavior.  It prevents an unsupported radio-layer causal interpretation.

### B2. Must change: preserve the QoS boundary and remove any causal wording introduced during revision

Section 5, lines 256--282 is presently good: the application-side label did not pass the predeclared QoS evidence check and has no observed TOS difference, but that absence is not proof of no network treatment.  The current capture summary shows TOS `0x0` for both cases (`results/real_5g/20260701_qos_verification_run_1/summary.md:11-57`).  Retain “without evidence of protected treatment,” not “no QoS,” “5QI 9,” priority, bearer isolation, RLC behavior, or scheduler causality.  Section 4, lines 291--310 already gives the correct boundary.

### B3. Recommended: disclose the sequential request generator at the first logical-client conclusion

Section 4 states that a logical client waits for its response/timeout and interval, and that five requests/s/client is a maximum rather than a maintained 500-request/s offered load at 100 clients (Section 4, lines 616--651).  The sender implementation follows request/response sequencing (`private_5g_latency_sender.cpp:517-543`).  Section 5, lines 284--335 should restate this immediately after it introduces logical clients:

> Each logical client issues its next request only after the preceding request completes or times out, followed by the configured interval.  Thus five requests/s/client and 500 requests/s at 100 clients are maxima, not maintained offered rates.

Without this sentence, a reader can over-interpret “100 logical clients” as a continuously offered fleet-scale workload.

### B4. Positive review finding

The paper properly confines the platform to two independent paths, one vehicle gateway/UE, no public-network claim, no independent UE scheduling/handover/roaming study, and no fallback (Section 4, lines 32--47; Section 5, lines 349--351 and 370--372).  This is exactly the right scope.  Do not attempt to “fix” it by implying that logical clients are separate UEs.

## Reviewer C — MobiCom systems-methodology, result logic, and presentation assessment

### C1. Must change: show availability as client count grows, not only p95 and a 100-client annotation

Figure 9(a) (Section 5, lines 316--332; rendered PDF page 22) plots aggregate p95 versus client count and places all-attempt availability annotations only at 100 clients.  The text provides important 100-client availability values, but the figure does not let a reader assess whether deadline availability erodes with demand, which is the central systems conclusion.  This is especially consequential because the favorable 100-client UDP p95 is low while availability is below TCP/MQTT, and because weak-path aggregate tails and derived per-client tails differ.

**Required revision using existing data if available.** Add a small lower panel, a second y-axis-free panel, or a compact adjacent table showing all-attempt A(100) and A(200) by client count and transport.  If complete per-count availability cannot be reconstructed from current sender CSVs, state that limitation and retain only the tested endpoint availability rather than implying a full demand curve.  This is a figure/data-reduction revision, not necessarily a new experiment.

At minimum, change the Figure 9 caption to say explicitly that panel (a) is **aggregate p95**, while the annotations are **all-attempt availability at 100 clients only**.  The current caption is mostly correct; the visual needs to carry the distinction.

### C2. Must change: make heterogeneous envelope aggregation visually unambiguous

Figure 6 is well rendered but its headline layout can be read as a simple object-size sweep.  Its rows pool different field/load conditions, and the deadline-analysis summary excluded the then-live/incomplete multiclient run 4 (`results/real_5g/20260702_end_to_end_deadline_analysis_run_1/summary.md:1-8`).  Section 5, lines 141--143 says the groups are heterogeneous, which is correct, but a strong reviewer will still see the figure title and infer payload monotonicity.

Add to the Figure 6 caption:

> Rows aggregate heterogeneous field/load/protocol evidence groups and should not be read as a controlled payload sweep or a condition-matched performance curve.

Also retain the group membership/denominator mapping in the artifact or appendix.  It is not enough for only the prose to disclose aggregation when the heatmap visually encourages comparison across rows.

### C3. Recommended: Table 5 should call the local IPI outcome format validation, not service validation

Table 5's local IPI row says “Decode and validate object” (Section 5, lines 19--35).  The rightmost scope column correctly says no wireless path, but “validate object” can be mistaken for a semantically valid offload request.  Suggested outcome wording:

> Decode and format-validate the local envelope/object.

Add “No service execution or remote semantic validation” to the boundary column if space permits.

### C4. Figure/table visual review (current `main.pdf`, pages 17--23)

All five result figures and Table 5 are present, readable, and fit without clipping or broken glyphs.  Pages 18--22 are space-efficient and preserve an appropriate results-first narrative.

| Item | Finding | Revision priority |
|---|---|---:|
| Table 5, PDF page 18 | Readable and useful functional boundary table.  The 5G row's “accepted ACK” wording is good; local IPI could be more explicit that it is format-level validation. | Recommended |
| Figure 5, PDF page 19 | Heatmap/asterisk convention is clear.  The left x-axis labels (for example, “NLOS Ref.Degraded Weak No path”) run together. | Editorial: use multi-line labels, rotation, or shorter separated labels. |
| Figure 6, PDF page 19 | Group heatmap is readable; p50/p95/p99 dot-plot legend is crowded near the lower bulk row.  More importantly, the figure visually resembles a size curve despite heterogeneous groups. | Must change for caption/label; editorial for legend placement. |
| Figure 7, PDF page 21 | Clear raw-UDP versus fragmented-UDP contrast.  The combined TCP/MQTT legend can hide protocol differences, but the text supplies them. | Recommended: direct-label or separate only if space permits. |
| Figure 8, PDF page 21 | Legible, and its caption correctly says `+UL` pools one/two/four default-label streams.  The near-zero TCP A(100) traces are visually hard to discern, and the caption currently does not expose actual achieved background rates. | Must change for load boundary; optional split/direct labels. |
| Figure 9, PDF page 22 | Generally clear, but panel (a) is busy and does not plot availability against client count.  Panel (b)'s legend crowds the lower-left bars slightly. | Must change for availability display; editorial legend relocation. |

### C5. Positive review finding

The results-to-insight organization is strong.  No insights appear before Section 5.9, and exactly three are presented there (Section 5, lines 357--405), consistent with the binding outline (lines 3179--3217 and 3816--3924).  Do not add a fourth “takeaway,” a setup recap, or generic testbed lessons to Section 5.

## Prioritized adjudication checklist

### P0 / release-blocking correctness repairs (prose and captions)

- [ ] **Replace implied semantic-service language.** Apply the safe replacement text in A1 to Table-5 discussion and all three affected insights.  Keep “decoded accepted ACK,” “not semantic completion,” and “no enforced request/session correlation” visible.
- [ ] **Correct the mixed-load interpretation.** State target versus achieved rate in the body and Figure 8 caption: 25 Mbit/s is offered; favorable achieved rate is 1.320--2.267 Mbit/s; W1 summarized rate is 0--0.322 Mbit/s; W2 lacks comparable aggregate-rate summary.  Do not present this as verified 25-Mbit/s coexistence.
- [ ] **Make weak-path radio provenance non-causal.** Replace unsupported measurement-like “weak-field” language with “weak-path collection” or give the missing-evidence qualifier; identify host-side/management-plane-pending direction context when it is used to motivate RAN interpretation.

### P1 / must-address before submission (existing-data analysis, figure, or concise prose)

- [ ] **Make Figure 6 unmistakably heterogeneous.** Change heading/caption/text so readers cannot treat it as a controlled payload-only curve.  Include group membership/denominator provenance in an appendix, artifact table, or supplemental material.
- [ ] **Expose client-count availability.** Add all-attempt A(B) by client count/transport to Figure 9 or a compact adjacent table from existing sender CSVs.  If unavailable, say so and avoid suggesting a complete capacity curve.
- [ ] **Restate sequential logical-client semantics.** Say that five requests/s/client and 500 requests/s at 100 logical clients are maxima, not a sustained offered load.
- [ ] **Tighten Table 5's local IPI label.** Call it format/object validation and explicitly exclude remote semantic execution if layout permits.

### P2 / recommended presentation repairs

- [ ] Separate/multi-line Figure 5 condition labels.
- [ ] Relocate or simplify Figure 6 percentile legend and Figure 9 restart legend.
- [ ] Consider direct labels or small multiples in Figure 8 to reveal near-zero TCP A(100) values.
- [ ] In Section 5.5, replace “hold detector-derived object scale constant” (lines 176--183) with language that accurately says it tests detector-derived object scales plus a 60-KiB stress object across transport formations; 0 B and 60 KiB are not a held detector-output scale.
- [ ] Give a stable source/appendix mapping from every plotted aggregate to the named result directory.  The current figure script transcribes values (`paper/current_manscript/scripts/build_section5_figures.py:4-7`) and is readable, but a reviewer needs reproducible membership/denominator mapping.

## Changes that require implementation or new collection — do not disguise these as prose fixes

| Desired stronger claim | What is actually required |
|---|---|
| A completed, correlated IPI service | Add ACK request/session identity or digest; sender-side comparison; semantic service invocation/response; then repeat the end-to-end test. |
| A guaranteed/verified 25-Mbit/s background coexistence envelope | Instrument and control achieved background rate on every run, including weak collections; validate traffic and queueing; repeat an explicit rate sweep. |
| 5G scheduler/QoS/radio causality | Obtain aligned network-side artifacts (PRB/MCS/QFI/5QI/TDD/cell identity as appropriate), synchronize them with trials, and design condition-matched controls. |
| Fleet/vehicular scalability | Use multiple independent UEs/vehicles or clearly scoped emulation, with scheduling/handover/roaming controls.  One host/one UE with logical clients is not this experiment. |
| PC5 robustness after stopped conditions | Complete the missing PC5 phases under a stable operator configuration; retain stopped and skipped cells until then. |
| Strict one-way CV/CAV deadlines or multi-nine reliability | Establish synchronized clocks/measurement method and much larger, condition-controlled trial counts.  Current RTT and 1,000-scale samples cannot certify them. |
| PC5-to-Uu resilience/fallback | Implement and test path selection/failover, session state, freshness/expiration, and duplicate/correlation behavior.  Current paper correctly has none. |

## Comments the authors should not follow

- **Do not convert every stopped/skipped PC5 heatmap cell into a 0% radio failure.** The asterisk convention and text correctly distinguish unexecuted cells from observed outcomes.
- **Do not claim public-network, multi-UE, handover, roaming, independent-scheduler, formal J2735 ASN.1/UPER, standardized regional-extension, or service-completion interoperability.** Current evidence does not support them.
- **Do not call the absence of a TOS difference proof that the operator applied no QoS.** It only fails this study's predeclared observability check.
- **Do not infer mobility, RSSI, MCS, PRB, TDD, scheduler priority, or cell-radio causality from the descriptive route/direction results.** The manuscript appropriately lacks aligned evidence.
- **Do not merge PC5 and Uu outcomes into an assumed fallback system.** They are independent paths here.
- **Do not add insights outside 5.9 or add a fourth insight.** The current three-insight structure is a strength.

## Submission-readiness conclusion

After the P0 and P1 repairs, Section 5 can be MobiCom-ready as an evidence-bounded systems measurement section: it reports useful negative and limiting results rather than overstating readiness.  Without them, the mixed-load and service-ACK wording leave two central claims vulnerable to a correctness challenge that would dominate review discussion.  The requested fixes mostly use existing evidence and careful language; only the explicit stronger claims in the implementation/new-collection table require user approval and expanded work.
