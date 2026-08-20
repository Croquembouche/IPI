# Section 7 MobiCom Red-Team Review: Conclusion

## Scope and verdict

Reviewed the current conclusion source (`paper/current_manscript/sections/07_conclusion.tex`), the binding conclusion outline (`paper/paper_outline.md:4317-4362`), the current introduction (`paper/current_manscript/sections/01_introduction.tex`), the approved three-insight synthesis (`paper/current_manscript/sections/05_experiment_results.tex:379-424`), the Section 6 closing transition, and the current rendered conclusion page in `paper/current_manscript/main.pdf`. No legacy prose, web source, manuscript edit, or new claim was used.

**Source-text verdict: PASS / Accept.** The current source is a concise, two-paragraph conclusion that narratively combines the two contributions, synthesizes exactly the three approved insights, avoids new material, preserves the semantic-service boundary, and ends with a strong application-ready criterion.

**Rendered-manuscript verdict: FAIL until the PDF is rebuilt.** `07_conclusion.tex` is newer than `main.pdf`, and the current rendered conclusion contains an obsolete sentence that reintroduces the exact protocol/path ambiguity the source has already repaired. This is a release-artifact failure, not a request for another source rewrite. Rebuild and visually recheck `main.pdf` from the current source before submission.

| Reviewer | Source vote | Current rendered-PDF vote | Principal finding |
|---|---:|---:|---|
| A. CAV/CV/ITS and protocol-semantics reviewer | Accept | Reject pending rebuild | Source preserves the distinction between a protocol/application model and measured path exchanges; the stale PDF does not. |
| B. Cellular/RAN/V2X measurement-scope reviewer | Accept | Reject pending rebuild | Source makes only measured PC5/Uu envelope claims and adds no radio causality; stale PDF can imply IPI was exercised over PC5. |
| C. MobiCom systems/editorial reviewer | Accept | Reject pending rebuild | Source satisfies the two-short-paragraph conclusion contract; the public rendered artifact is inconsistent with it. |

## Binding-outline and claim audit

| Required check | Finding | Evidence |
|---|---|---|
| Exactly two short connected paragraphs | **Pass.** Paragraph 1 asks the readiness question and explains the protocol-plus-evaluation design; paragraph 2 gives the measured answer and its implication. | `07_conclusion.tex:4-11,13-21`; outline `4319-4326`. |
| Two contributions clear through narrative, not a list | **Pass.** “We design and implement IPI ... Alongside this protocol, we evaluate ...” gives protocol/reference-interface and field-evaluation contributions in one causal narrative. | `07_conclusion.tex:7-11`. |
| Exactly three insights synthesized as one answer | **Pass.** The final paragraph has the direct-PC5 compact/local envelope, the private-5G larger-object/deadline/demand envelope, and the independent-paths-not-a-guarantee conclusion, without a fourth finding or list. | `07_conclusion.tex:13-21`; Section 5.9 `:403-418`. |
| No new number, citation, application, future work, limitation, or claim | **Pass.** The conclusion introduces no numeric result, citation, named new workload, Section 6 agenda, public-network limitation, or future deployment promise. | `07_conclusion.tex:1-21`; outline `4323-4326,4354-4358`. |
| No implication that IPI ran over PC5 | **Pass in source; fail in current PDF.** The source correctly says “Alongside this protocol,” separating the IPI contribution from PC5/Uu measurement paths. The rendered PDF still says “We then use it to evaluate,” which can reasonably imply IPI was deployed on both PC5 and Uu. PC5 evidence is standard-message checks and custom sequence-matched echoes; the private-5G path uses IPI-envelope ACK probes. | Source `07_conclusion.tex:7-11`; current PDF page 25; setup `04_system_design_setup.tex:589-595,641-645`; results `05_experiment_results.tex:7-14,46-49`. |
| No semantic-service-completion overclaim | **Pass in source.** “Complete service” describes the readiness objective, and the conclusion says neither path turns connectivity into an application guarantee. It does not claim that the PC5 echo or 5G ACK is a completed planning/control/actuation service. | `07_conclusion.tex:4-6,18-21`; Section 5.9 `:398-400`. |
| Proper transition from Section 6 | **Pass.** Section 6 ends with application-level evidence and the conclusion closes that argument with one application-ready design/judgment criterion; it does not repeat the five directions or become a shopping list. | `06_future_research_directions.tex:160-167`; `07_conclusion.tex:20-21`. |
| Final sentence strong and non-shopping-list | **Pass.** The final sentence states one evaluative criterion rather than enumerating 6G, scheduling, adaptation, fallback, or benchmark agenda items. | `07_conclusion.tex:20-21`. |

## Consensus must-change item

### P0 — Rebuild the rendered PDF from the current conclusion source

The source file was modified after the current PDF was built. The source now contains the safe separation:

> Alongside this protocol, we evaluate complete exchanges over certified LTE C-V2X PC5 and private-5G NR Uu paths on a real autonomous vehicle.

However, current `main.pdf` page 25 still renders the prior text:

> We then use it to evaluate complete exchanges over certified LTE C-V2X PC5 and private-5G NR Uu paths on a real autonomous vehicle.

The latter can imply that IPI itself ran over the PC5 experiment. That is unsupported: Section 4 defines PC5 as standard BSM/SPaT checks plus custom echoes and defines the 5G path as IPI-envelope ACK probes (`04_system_design_setup.tex:589-595,643-645`). The current source replacement is correct; **do not change it again.** Rebuild `paper/current_manscript/main.pdf`, verify that the rendered text says “Alongside this protocol,” and visually recheck the conclusion page.

## Reviewer A — CAV/CV/ITS and protocol-semantics assessment

The current source successfully keeps conventional J2735-message profiles and stateful CAV services at the protocol/application-model level. It does not claim formal J2735 conformance beyond the paper's established scope, a standardized regional extension, an enforced session lifecycle, or completed remote-service semantics. The closing criterion—whether the right service completes within useful time and operating conditions—is a design/evaluation standard, not a claim that the current ACK probe completed a planning or control service.

The stale PDF wording is consequently material: “use it” obscures the deliberate separation between the IPI reference contract, the PC5 custom/standard-message evidence, and the 5G IPI-envelope ACK evidence. The source's “Alongside this protocol” repair fully resolves the issue after rebuild.

**Recommended, non-blocking precision improvement.** To align exactly with the field-path language in Section 5 and avoid implying a calibrated PC5 signal-only causal variable, replace the source phrase at lines 14--16:

> ... but its dependable envelope narrows as payloads grow or signal decreases.

with:

> ... but its dependable envelope narrows as payloads grow or the field path weakens.

This is optional; it improves vocabulary consistency but does not change the conclusion's supported meaning.

## Reviewer B — cellular/RAN/V2X measurement-scope assessment

The source is correctly scoped to **measured** direct-V2X and 5G operating envelopes. It does not introduce a one-way-latency claim, public-network result, multiple-independent-UE/fleet result, handover/roaming result, scheduler/PRB/MCS/TDD causal conclusion, 5QI/QFI claim, or public-carrier conclusion. “Tight deadlines and shared demand reduce what remains usable” is an appropriate high-level synthesis of the evaluated necessary communication conditions, not a claim of a generic RAN cause.

Do not add the experiment's many qualification clauses to this two-paragraph conclusion. The detailed RTT, accepted-ACK, one-UE, offered-versus-achieved, weak-path, and RAN-instrumentation boundaries belong in Sections 4--5, where they are already explicit. The required PDF rebuild is sufficient to preserve the relevant protocol/path scope at the conclusion level.

## Reviewer C — MobiCom systems/editorial assessment

The source meets the exact conclusion structure requested by the outline. The opening moves cleanly from the CAV information need to the measurable readiness question; the next sentence makes clear that the protocol and field evaluation work together. The second paragraph is a compressed argument, not a result inventory: PC5's compact local role, 5G's richer-object but budget/demand-limited role, and the need to judge application readiness rather than connectivity are one connected answer.

The ending is especially effective:

> Application-ready communication must instead be designed and judged by whether it completes the right service within its useful time and operating conditions.

It is strong, venue-appropriate, and avoids a Section 6-style shopping list. Preserve it.

## Visual review of the current rendered conclusion

The current PDF page 25 is visually clean: the Section 7 heading is clear, both paragraphs fit in the right column without clipping, overlap, widows, broken glyphs, or bad spacing, and the references begin naturally below. There is no layout defect to fix. The visual review still **fails artifact verification** because the readable rendered text is stale and differs materially from the current source. Rebuild is required before this otherwise polished page can be treated as final.

## Do not follow

- Do not add numerical results, citations, device inventory, experiment dimensions, one-way/RTT caveats, public-network scope, or Section 6 research directions to the conclusion. The outline expressly excludes them, and the current text is appropriately compact.
- Do not replace “Alongside this protocol” with “we use it,” “IPI over PC5 and Uu,” or equivalent wording. The latter would overstate the PC5 implementation/evaluation path.
- Do not claim a semantic planning, control, actuation, or remote-recovery completion from either PC5 echoes or 5G accepted ACKs.
- Do not weaken the final sentence into a list of scheduling, adaptation, fallback, 5G-Advanced, or 6G mechanisms. Its single application-ready criterion is more persuasive.
- Do not call Edge4AV the system, Mocar the system, or frame the contribution as testbed construction.

## Uncertain user-decision items

None. The source already uses the correct two-contribution logic and safe PC5/Uu wording. The only required action is an artifact rebuild and re-verification; no additional authority or manuscript-scope decision is needed.

## Final handoff

**Current source: PASS. Current rendered PDF: FAIL pending rebuild.** After `main.pdf` is regenerated from the present `07_conclusion.tex` and the page is rechecked for the “Alongside this protocol” wording, the conclusion is submission-ready. The optional “field path weakens” wording is a minor precision improvement, not a blocker.

## Second-pass artifact recheck (rebuilt PDF)

**PASS — no remaining P0/P1 or must-fix item.** Rechecked the current `07_conclusion.tex` and the rebuilt `main.pdf` conclusion page.

- The rendered PDF now says **“Alongside this protocol, we evaluate complete exchanges ...”**, matching `07_conclusion.tex:9-11`. It no longer implies that IPI was directly exercised on the PC5 path.
- The source and PDF both now say **“the field path weakens”** (`07_conclusion.tex:14-16`), which is the preferred evidence-bounded phrasing.
- `main.pdf` is newer than the conclusion source (PDF build time 2026-08-12 14:49:00 EDT versus source modification time 14:48:54 EDT), so this is a current artifact rather than a stale rendering.
- The rendered page remains clean: two compact conclusion paragraphs fit without clipping, overlap, bad page break, or readability regression; the heading and references transition remain polished.

**Remaining must-fix items: none.**
