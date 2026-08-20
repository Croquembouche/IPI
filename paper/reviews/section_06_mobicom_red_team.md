# Section 6 MobiCom Red-Team Review: Future Research Directions

## Scope and verdict

Reviewed only the current manuscript and current supporting sources: `AGENTS.md`, `current_task.md`, `agent_context.md`, the binding Section 6 outline (`paper/paper_outline.md:3949-4315`), the current Section 4 application/readiness and measurement contract (`paper/current_manscript/sections/04_system_design_setup.tex:421-568,616-651`), Section 5.9 and its three approved insights (`paper/current_manscript/sections/05_experiment_results.tex:379-424`), the current Section 6 source, and the cited entries in `paper/current_manscript/references.bib`. The current rendered PDF pages 23--25 were inspected. No legacy draft or web source was used, and no manuscript file was edited.

**Consensus verdict: Conditional Weak Accept.** Section 6 is a useful field-level MobiCom agenda rather than a conventional paper-limitation or “our future work” section. It is logically derived from the three Section 5 insights, has exactly five research directions rather than additional findings, gives CAV/ITS, traffic-signal, radio/carrier, and systems researchers actionable problems, and generally uses the four citations at their proper strength. It needs a short set of pre-submission clarity and experimental-design repairs before it is fully ready.

| Reviewer | Vote | Summary |
|---|---:|---|
| A. CAV/CV/ITS and traffic-signal researcher | Weak Accept | Strong safety and controller-authority boundaries; make the task-level outcome and intersection terminology more concrete. |
| B. Cellular/RAN/carrier/5G-Advanced/6G researcher | Weak Accept | Strong contract, scheduling, TDD, and carrier-evidence agenda; make the benchmark factorized and tier its optional RAN evidence. |
| C. MobiCom systems/research-agenda editor | Weak Accept | Excellent evidence-to-agenda transition and no extra insight inflation; sharpen a few outcome metrics and remove avoidable ambiguous jargon. |

## What passes now

### It is a field agenda, not paper-specific future work

This boundary is explicit and credible. The opening says the directions are “not additional insights or planned extensions to this deployment” (`06_future_research_directions.tex:4-11`), uses no “we will” language, and asks for community capabilities rather than a larger version of the present experiment. The otherwise paper-specific statement that logical clients behind one UE cannot study fleet contention is correctly transformed into a general evaluation requirement (`:57-61`), consistent with the Section 4 scope (`04_system_design_setup.tex:616-624`). Do **not** remove that sentence merely because it originates in the measurement scope: it establishes a field-level experimental validity condition.

### It follows from, and does not add to, the three approved insights

The five directions are a faithful unpacking of Section 5.9 rather than five new conclusions:

| Section 5.9 evidence-bound insight | Section 6 agenda response | Audit |
|---|---|---|
| Direct V2X alone does not support all dependable large-object or multi-step exchange. | Service contract, object/compute adaptation, continuity, and evidence benchmarks (`06:13-36,63-110,112-138`). | Appropriate research response; no new measured claim. |
| The 5G ACK envelope is application-, deadline-, load-, and operating-condition-dependent. | Heterogeneous-service admission/scheduling, direction/TDD-aware resource management, adaptive representation, and all-attempt metrics (`06:38-84`). | Appropriate and consistent with the accepted-ACK, not semantic-completion, boundary. |
| Future support must join application semantics with verified representation, path, scheduling, admission, and fallback decisions. | The contract, continuity, and benchmark directions (`06:24-36,86-110,125-138`). | Appropriate; the text correctly says IPI provides only part of the application vocabulary. |

No new result numbers, performance claims, technology comparisons, “fourth insight,” or unsupported PC5-to-Uu fallback claim appears. The Section 5 definition that “support” is a necessary communication condition rather than semantic service execution remains intact (`05_experiment_results.tex:398-400`).

### It gives each intended reader group concrete value

- **CAV/CV/ITS developers:** a concrete application/network contract with deadline, freshness, partial-result, fallback, admission, and revocation state (`06:24-36`), plus representation and placement decisions (`:63-84`).
- **Traffic-signal and infrastructure researchers:** a bounded role for infrastructure sensing/edge assistance that preserves authoritative controller and safety constraints (`:74-84`).
- **Radio/carrier researchers:** deadline-aware admission, preemption, Uu direction/TDD constraints, cross-domain PC5/Uu coordination, and carrier-visible evidence (`:38-61,125-133`).
- **Systems researchers:** session migration, replicated state, idempotence, duplicate suppression, terminal state, correlation/freshness, and recovery/fallback metrics (`:86-110`).
- **Methodology and benchmark researchers:** a workload portfolio, all-attempt denominator, complete-object accounting, achieved-rate reporting, and confidence bounds (`:112-138`).

## Consensus must-change items

### M1. Make the Section 6.3 outcome measurable instead of introducing an undefined generic “decision utility”

**Problem.** “Deadline-qualified decision utility” (`06:80-84`) is a promising agenda label but is not defined. Different applications can use incompatible safety and utility functions; without a task-specific interpretation, the proposed outcome can sound like an unmeasurable global score. Likewise, “useful-object goodput” in Section 6.2 (`:57-59`) is nonstandard phrasing when the manuscript elsewhere uses “complete-object goodput.”

**Why it matters.** This is the point at which a CAV/ITS reader must see how semantic reduction becomes safely testable rather than a claim that fewer bytes are intrinsically better. It is also the main outcome offered to edge/AI researchers.

**Safe replacement text.** Replace the first sentence beginning “The outcome should be evaluated...” at lines 80--82 with:

> The outcome should be evaluated with a task-specific decision metric: whether a correct action or safe deferral, with stated uncertainty and provenance, is available before its deadline. Report the associated uncertainty, privacy and energy cost, and the time consumed by inference, serialization, security, and fusion.

Replace “useful-object goodput” at lines 57--59 with **“complete-object goodput”**, or define it on first use as “complete, usable application objects per second.”

### M2. Make the benchmark a modular, factorized portfolio rather than an impracticable all-factors-at-once experiment

**Problem.** Section 6.5 says that “The benchmark should combine” controlled mixed services with multiple UEs, heterogeneous radio conditions, city mobility, handover, roaming, interference, backhaul/edge distance, TDD, and failures (`06:125-133`). Read literally, this invites one confounded, costly, unreproducible super-experiment—the opposite of the experimental-control argument in the preceding paragraph and in Section 4. The paper should ask for a portfolio that deliberately varies factors, not require every benchmark run to include all factors.

**Safe replacement text.** Replace lines 125--133 with:

> A benchmark portfolio should provide modular scenarios that separately and, where justified, jointly vary multiple independently scheduled UEs, radio condition, mobility and handover/roaming, interference, backhaul/edge distance, TDD context, and repeatable failure events. Each scenario should publish which factors are held fixed, the foreground and achieved background workload, and the causal scope of its evidence. Minimum reproducibility artifacts are application and transport records. When a deployment can export network-side instrumentation, those records should be aligned with serving cell, PRB use, MCS, BLER, retransmission, bearer, and scheduler evidence; otherwise, the report should state that radio-layer causal attribution is unavailable.

Then retain the current metric sentence beginning “Reports should include...” with the terminology repair in M1.

**Boundary.** This is not a request to add those experiments to Edge4AV. It makes the requested community benchmark scientifically tractable.

### M3. Remove or define the niche “transformative intersections” term and keep authority boundaries explicit

**Problem.** “Transformative intersections” appears at line 78 without definition or local citation. The term has context earlier in the paper (`04_system_design_setup.tex:466-477` cites `he2024transformative`), but a reader entering this field-level agenda may not remember that specific label. More importantly, the agenda should avoid sounding as if an edge application can alter signal control.

**Safe replacement text (preferred).** Replace lines 78--80 with:

> For infrastructure-assisted intersections, this work must preserve the authority and safety constraints of the traffic-signal controller: an edge service may supply observations or advisories, but it must not bypass controller-authorized state or safety logic.

**Alternative if the authors intentionally retain the named concept.** Define it on first use as an infrastructure-assisted intersection with sensing and edge computation, and cite `he2024transformative`. Do not leave the label unexplained.

## Reviewer A — CAV/CV/ITS and traffic-signal assessment

**Strengths.** Section 6 correctly carries forward the distinction between a common broadcast and an individualized, correlated, freshness-sensitive service (`06:15-36,74-84`). Its safe-degradation subsection is particularly sound: late trajectories, duplicate commands, expired sessions, authority transfer, explicit terminal states, and local minimal-risk behavior are appropriate concrete research problems (`:86-110`). The NHTSA citation is used narrowly to support the local-safety boundary, not as a certification claim.

**Required repairs.** Implement M1 and M3. They ensure that “smaller object,” “partial result,” and intersection assistance are evaluated by a correct/safe task outcome rather than bytes, latency, or a generic utility label.

**Recommended refinement.** In the contract subsection, use **“application programming interface (API)”** once at first mention or simply “API” if it has already been expanded in the paper. “Admitted treatment” is correct carrier language, but add the operative interpretation on first use if space permits: “the resources/policy the network accepted for the request.” This improves readability for traffic-signal and CAV readers without changing the research claim.

**Do not follow.** Do not turn the agenda into a proposal to put communication in the emergency braking/control loop. Lines 103--105 correctly require communication support to integrate with, rather than replace, local minimal-risk behavior. Do not present a small representation, an edge inference result, or an ACK as a safe vehicle decision by itself.

## Reviewer B — cellular/RAN/carrier/5G-Advanced/6G assessment

**Strengths.** The scheduling direction correctly separates PC5 sidelink and Uu resource domains and includes the non-radio bottlenecks that actually matter for stateful CAV services: gateway, transport, core, edge compute, backhaul, and human-assistant capacity (`06:38-61`). It also correctly says uplink-heavy services must respect TDD, interference, and deployment constraints rather than infer symmetric capacity from a peak-rate label (`:45-47`). The text asks for observed treatment and causal evidence rather than assuming that a priority, QoS label, or configured 5QI name proves delivery treatment (`:19-36`). This matches the current Section 4.7 offered-versus-achieved, host-side, and no-unverified-QoS boundaries (`04_system_design_setup.tex:616-651`).

**Required repair.** Implement M2. It is especially important for carrier readers: scheduler/PRB/MCS/BLER data are not universally exportable, and a benchmark must distinguish mandatory application evidence from optional privileged RAN telemetry. The present “where available” clause is good; the portfolio must also state the causal limit when it is absent.

**Recommended refinement.** Replace “human-assistant capacity” (`06:44-45`) with **“remote-assistant capacity”** for clarity. Add an optional sentence to Section 6.1 saying that a future contract must report its enforcement confidence or revocation state rather than merely return a QoS class name. This supports carrier action without implying that current 5G standards already expose the proposed lifecycle.

**Citation audit.**

| Citation and use | Review finding |
|---|---|
| `etsi122186r19` and `fivegaa2021usecases` for heterogeneous direction/rate/latency/reliability/recovery requirements (`06:19-20`) | Appropriate authoritative requirement anchors. They support heterogeneity; the paper correctly does not portray their values as universal certification thresholds. |
| `ngmn2021tdd` for uplink-heavy verticals and TDD constraints (`06:45-47`) | Appropriate as a general TDD-uplink motivation. It is not used to claim that the present deployment's allocation caused a measured result or that a particular allocation is universally optimal. Preserve that restraint. |
| `itu2023m2160` for broad IMT-2030 context (`06:140-147`) | Correctly limited. The next clause explicitly says a technology-generation label does not establish application readiness. Do not replace that with a claim that 6G will solve the agenda. |

**Uncertain user decision.** If the authors want Section 6 to be explicitly framed as a 5G-Advanced/6G standards agenda, it needs a deliberate scoped expansion and additional current normative sources. The current, better-bounded version uses 5G/IMT-2030 only as context and remains valid without that expansion.

## Reviewer C — MobiCom systems and research-agenda assessment

**Strengths.** The section meets the binding outline's central boundary: it poses common research problems rather than naming missing devices, new probes, or planned Edge4AV extensions. Its five directions have a clear problem-to-capability-to-evaluation progression:

1. application/network contract and auditable admission;
2. mixed-service scheduling/admission across radio and edge;
3. representation, computation, and communication co-design;
4. state continuity and safe degradation; and
5. reproducible application-level evidence.

The final synthesis is appropriately modest: ITU IMT-2030 is context, not evidence of CAV readiness (`06:140-147`). The text does not imply that IPI, semantic communication, edge computing, multi-connectivity, or a new radio generation alone solves the whole problem.

**Required repairs.** Implement M1--M3. In particular, M2 turns a potentially generic “benchmark everything” paragraph into a credible systems-methodology proposal.

**Recommended editorial improvements.**

- Add a one-sentence explicit open question at the beginning of each subsection, if page space permits. The questions are currently inferable and the technical content is concrete, but the binding outline asks for a repeated question/problem/outcome order. For example, Section 6.2 could open: “The open question is how radio, transport, and edge resources should admit and schedule mixed CAV services so foreground deadlines hold without starving other traffic.”
- Use consistent terminology: “complete-object goodput” appears later in the section (`06:131-133`) and should be the preferred term throughout.
- The rendered Section 6 is readable and has no clipping, broken citations, or figure/table defects. On PDF page 24, the final line of 6.1 carries over alone before 6.2. This is optional layout polish only; a small paragraph shortening could avoid the orphan continuation, but no structural redesign is needed.

## Items not to follow

- Do not rewrite Section 6 as limitations, a planned experiment list, or “future versions of IPI.” The opening and all five directions already correctly establish the field-level boundary.
- Do not add a fourth insight or renumber the five directions as insights. The only three insights remain in Section 5.9.
- Do not claim that IPI already enforces admission, reservation, session migration, expiration, fallback, QoS, or network treatment. Section 6 correctly says it supplies **part** of the application vocabulary (`06:33-36`); Section 3 documents the unimplemented enforcement boundaries.
- Do not say PC5 and Uu draw from one resource pool or imply a deployed PC5-to-Uu failover mechanism. The agenda correctly calls coordination a future research problem and requires accounting for duplication, correlated failure, and state inconsistency.
- Do not make every benchmark run include every mobility, radio, carrier, failure, and workload factor. Use the modular/factorized portfolio in M2.
- Do not require private carrier RAN counters as a universal benchmark prerequisite. Require all-attempt application/transport evidence universally and state a causal limitation when RAN instrumentation is unavailable.
- Do not use NHTSA guidance as evidence that a network-assisted application is safe or certified. It supports the local minimal-risk boundary only.

## Uncertain decisions requiring author direction, not automatic manuscript expansion

1. **Named intersection framing.** Decide whether “transformative intersection” is a necessary paper-specific concept. If yes, define and cite it locally; if not, use the broader “infrastructure-assisted intersection” wording in M3.
2. **Standards-design ambition.** Decide whether the application/network contract is a research agenda for interoperable interfaces or a proposal for a new standards profile. The present text appropriately advocates research; specifying message schemas, 5QI/QFI mappings, or carrier APIs would be a materially larger contribution and needs new authority and sources.
3. **Benchmark governance.** Open workload generators and privacy-preserving traces are valuable, but a concrete release policy requires operator, agency, vehicle, and privacy approval. The paper should state the research goal, not promise access to protected RAN/vehicle records.
4. **5G-Advanced/6G depth.** The current ITU anchor provides restrained future-network context. A deeper 5G-Advanced/6G subsection should be added only if the authors want to spend space on specific standards mechanisms and can support them with current normative references; it is not required for this agenda to be useful.

## Final readiness assessment

After M1--M3, Section 6 is MobiCom-ready: it provides a disciplined, cross-community research agenda whose research problems and outcomes arise from the measured evidence without recasting the paper as a testbed, a standards proposal, or a collection of future experiments. The current source already has the hard parts right—scope, causality restraint, safety boundary, and three-insight discipline. The remaining changes are concise precision improvements, not a change of thesis.

## Second-pass re-review (revised Section 6)

**PASS — no remaining P0/P1 or must-fix item.** Re-reviewed the revised current source and the rebuilt `main.pdf` pages 23--25 only.

| Prior required item | Re-review finding | Current source evidence |
|---|---|---|
| M1: measurable Section 6.3 outcome and consistent goodput terminology | **Resolved.** The agenda now evaluates a task-specific metric: whether a correct action or safe deferral, with stated uncertainty and provenance, is available before deadline. It also consistently uses “complete-object goodput.” | `06_future_research_directions.tex:62-64,84-94,150-153` |
| M2: modular/factorized benchmark rather than an all-factors-at-once super-experiment | **Resolved.** The benchmark is now a portfolio of modular scenarios that separately and, where justified, jointly vary factors; each scenario publishes held-fixed factors, foreground/achieved background workload, and causal scope. Application/transport records are minimum artifacts, while network-side instrumentation is conditional on availability and absence is a stated causal limit. | `06_future_research_directions.tex:139-153` |
| M3: undefined “transformative intersections” and controller-authority ambiguity | **Resolved.** The revised wording uses “infrastructure-assisted intersections” and explicitly limits an edge service to observations/advisories; it cannot bypass controller-authorized state or safety logic. | `06_future_research_directions.tex:84-87` |

The added explicit open-question sentences at the start of Sections 6.1--6.5 (`06_future_research_directions.tex:15-16,43-44,70-71,98-99,126-127`) improve the intended question-to-problem-to-outcome structure. They are concise, flow directly into their evidence/motivation paragraphs, and do not create new claims, insights, or ambiguity.

**Visual result.** The latest rendered Section 6 on PDF pages 23--25 is readable and professionally laid out. Headings, question transitions, citations, and page breaks render cleanly; no clipped text, overlap, broken glyph, orphaned heading, or figure/table issue was found. The long benchmark subsection fits cleanly before the references.

**Remaining must-fix items: none.**
