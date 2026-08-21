# Paper Outline

This is a living outline developed section by section with the user. It
currently covers the Introduction, Related Work, IPI Protocol Design, System
Design and Experimental Setup, Experiment Results, and Future Research
Directions, followed by a concise Conclusion. The paper should be written as a
MobiCom systems argument: it should
establish an important communication-readiness question, explain why the
question cannot be answered from standards targets or peak throughput alone,
show how the experiments answer it and how IPI enables the comparison, and turn the resulting evidence into
a field-level research agenda.

### Binding abstract structure — not a manuscript section

- The first sentence must ask whether today's V2X and 5G communication paths
  are ready for tomorrow's CAV applications.
- The opening paragraph must distinguish compact, stateless CV messages from
  correlated, stateful CAV operations that may carry larger objects and
  identify the missing readiness
  and deadline-miss evidence. The empirical question tests both workload
  classes over both V2X and 5G. Do not assume a fixed CV-to-V2X and CAV-to-5G
  assignment.
- Introduce the controlled real-vehicle communication study as the primary
  scientific contribution. Then introduce IPI as the enabling systems
  contribution developed to represent both workload classes consistently. The
  abstract's causal order is measurement question, missing comparable evidence,
  measurement study, need for one workload contract, IPI, and empirical
  insights. Do not mention the framing-cost result in the abstract.
- State that IPI carries the SAE International J2735 message set; do not narrow
  the abstract claim to a selected subset. Typed helpers may still be identified
  separately from the encoded-message path in the protocol section.
- Describe the 5G evaluation as tightly controlled: one physical CAV UE, a
  dedicated radio and 40-MHz channel, and a clean n48 band without ambient
  contention. Any background traffic or application-client demand is
  deliberately introduced as an experimental variable.
- End with exactly three numbered insights in the form `1)`, `2)`, and `3)`.
  State the technical insights directly without `For CV/ITS`, `For CAV
  systems`, or `For 5G/6G` audience prefixes. Instead, name the affected system
  and decision directly. The three implications must remain technically
  distinct: 1) the measured commercial direct V2X path supports compact J2735
  messages only within a limited payload and coverage envelope; 2) the measured
  5G uplink supports only severely limited CAV packet sizes within decision
  deadlines; and 3) 5G/6G radios must support
  event-triggered CAV bursts in either direction and isolate concurrent flows
  if cellular networks are to support CAV and ITS applications that advance the
  goal of zero road fatalities.

### Binding submission-format requirements — not a manuscript section

Any paper that does not adhere to the following requirements may be immediately
rejected. These requirements constrain preparation and final validation; they
do not change the paper's section structure or scientific outline.

- Keep these requirements recorded, but do not condense, expand, restructure,
  or reflow the manuscript to meet a page limit unless the user explicitly asks
  for page-limit work.

- The paper may contain no more than **twelve (12)** single-spaced and numbered
  pages, including figures, tables, and all other non-bibliographic material.
  Bibliographic references may occupy as many additional pages as necessary.
  Papers with more than 12 pages of non-bibliographic content will not be
  reviewed.
- Appendices may follow the bibliography and do not count toward the 12-page
  limit. Reviewers are not required to read or consider appendices, so the core
  paper must remain self-contained.
- Font size must be no smaller than **10 points**.
- Use a double-column format. Each column must be 9.25 inches high and 3.33
  inches wide, with 0.33 inches between columns and no more than 55 lines of
  text per column.
- The paper must fit properly on US letter paper measuring 8.5 by 11 inches.
- Submit the paper as a PDF compatible with the English version of Adobe
  Acrobat. PostScript, Microsoft Word, and other formats are not accepted.
- Authors' names must not appear anywhere in the paper or in the PDF file.
- The PDF must not contain embedded hyperlinks because they may compromise
  author anonymity.
- The PDF file must be smaller than 15 MB. Contact the program chairs if the
  file exceeds this limit.
- The [ACM proceedings
  templates](https://www.acm.org/publications/proceedings-template) may be used
  to satisfy the formatting requirements. LaTeX submissions should use
  `\documentclass[sigconf,10pt]{acmart}`.
- The authors retain final responsibility for manually verifying—or verifying
  with the venue's online paper checker—that the submission complies with all
  formatting rules.

### Binding writing and logical-transition requirements — not a manuscript section

The paper must read as one continuous argument rather than as a collection of
individually correct statements. Logical continuity is required at the
sentence, paragraph, subsection, and section levels.

- **Sentence-to-sentence transitions:** every sentence must follow logically
  from the preceding sentence and prepare the next one. Use appropriate
  **transition words**, **linking words**, or **transitional devices** to make
  the relationship explicit. The transition must identify the real relation:
  continuation, cause, consequence, contrast, qualification, comparison,
  example, evidence, interpretation, or sequence.
- **Paragraph-to-paragraph transitions:** every paragraph must connect to the
  preceding paragraph and establish why the next paragraph is necessary. Its
  opening should link backward to the idea, result, limitation, or question just
  established; its closing should complete the paragraph's role and create the
  logical need for what follows.
- **Subsection-to-subsection and section-to-section transitions:** the end of a
  subsection or section must state the conclusion, unresolved question, or next
  analytical need that motivates the following subsection or section. The next
  opening must explicitly take up that conclusion or question. A new heading
  does not by itself provide a logical transition.
- Use precise devices such as `because` or `since` for cause; `therefore`,
  `thus`, or `consequently` for implication; `however`, `whereas`, or `in
  contrast` for difference; `specifically` or `for example` for evidence;
  `moreover` or `in addition` for a genuine extension; `despite this` or
  `nevertheless` for qualification; and `first`, `next`, or `finally` for
  sequence. These examples are guidance, not a required phrase list.
- A transition does not have to be a conjunctive adverb at the beginning of
  every sentence. Repeating a key technical subject, using a backward-pointing
  noun phrase such as `this limitation`, or using a dependent clause that names
  the relation can provide a stronger transition. The logical link must still
  be explicit.
- Do not insert `however`, `therefore`, `moreover`, or similar words when the
  claimed relationship is not logically true. If no precise transition fits,
  repair the reasoning, add the missing premise, reorder the statements, or
  remove the disconnected sentence.
- Avoid shopping-list prose. Related facts must be synthesized into a claim,
  connected to evidence, and followed by their consequence for the paper's
  question. A list is appropriate only when the items are genuinely parallel
  and the surrounding prose explains why the list matters.
- During revision, inspect three boundaries explicitly:
  1. each pair of adjacent sentences;
  2. the final sentence of one paragraph and the first sentence of the next;
     and
  3. the closing paragraph of one section and the opening paragraph of the next.
  At each boundary, identify the relationship and ensure the prose states it.
- Transition quality is part of the paper pass condition. A technically correct
  sentence or paragraph still fails if the reader must infer why it follows from
  the preceding material or why the following material comes next.

#### Terminology, word choice, and abbreviation requirements

- **Do not invent words, labels, categories, or technical-sounding shorthand.**
  Use terminology established in the relevant standards, literature, current
  repository, experiment artifacts, or the user's instructions. An outline label
  created only to organize drafting must not automatically become manuscript
  terminology.
- **Do not use a word merely because it sounds concise or sophisticated.** If a
  familiar word expresses the same meaning more clearly, use the familiar word.
  Avoid wording that can be interpreted differently by CAV, CV, ITS, traffic-
  signal, networking, radio, carrier, edge-computing, or 5G/6G readers.
- **Explain every specialized term when it first appears.** The explanation must
  state what the term means in this paper and, when necessary, why it matters to
  the argument. Do not assume that a reader who works in one of the paper's
  communities also knows the terminology of every other community.
- **Spell out every abbreviation or acronym at its first appearance, followed by
  the abbreviation in parentheses.** For example: `connected and automated
  vehicle (CAV)`, `intelligent transportation system (ITS)`, `round-trip time
  (RTT)`, and `quality of service (QoS)`. Do not use an abbreviation before its
  full form appears.
- This first-use rule applies across the title, abstract, keywords, headings,
  main text, figures, tables, captions, footnotes, and appendix. If an
  abbreviation first appears in a title or heading, spell it out there rather
  than relying on a definition that appears later.
- Treat the abstract and the main paper as separate reading contexts: spell out
  an abbreviation at its first appearance in the abstract and spell it out again
  at its first appearance in the main body. A figure, table, or caption that must
  be understandable on its own must either spell out the abbreviation or point
  unambiguously to a definition in the surrounding text.
- After defining an abbreviation, use it consistently. Do not alternate among
  multiple abbreviations or shortened names for the same concept, and do not use
  one abbreviation for two different concepts.
- Maintain **one stable term for one concept** throughout the paper. Do not shift
  among near-synonyms when the change could suggest a technical distinction. If
  two terms are intentionally different, define the distinction before relying
  on it.
- Use **communication latency** for the paper's central measured outcome; do not
  use `communication overhead` as its synonym. Reserve `overhead` for an
  explicitly measured addition such as IPI packaging bytes, transport headers,
  security bytes, or added processing work. Pair communication latency with
  deadline completion when the claim also includes late or missing responses.
- Do not use `all-attempt` as a metric label or modifier. State the denominator
  directly: for example, `the percentage of issued requests answered within the
  deadline`. Figures and captions should likewise say `issued requests` rather
  than relying on the label `all-attempt availability`.
- Avoid the generic word `service` when the paper means an application,
  operation, request, response, exchange, endpoint, or named standards
  procedure. Use the precise term. Retain `service` only for an official name,
  an exact implementation identifier, or a clearly defined technical concept.
- Do not use `individualized plan`. Use an established and precise description,
  such as `vehicle-specific trajectory`, when that is the object being sent.
- Define overloaded performance words at the strength used in the paper.
  Specifically, do not use `latency`, `reliability`, `availability`, `success`,
  `real time`, `edge`, `ready`, `support`, or `complete` without making the
  relevant endpoint, object, deadline, denominator, system boundary, or
  operating condition clear.
- When introducing a standard-specific or radio-specific term—such as
  `sidelink`, `Uu`, `bearer`, `time-division duplex`, `physical resource block`,
  `modulation and coding scheme`, or `block error rate`—give its full name and a
  short plain-language explanation of its role in this paper before using its
  abbreviation or relying on its implications.
- Definitions must clarify rather than relocate confusion. Do not define an
  unfamiliar term using other unexplained specialist terms. If the term cannot
  be explained plainly and precisely, either add the missing explanation or
  replace it with clearer wording.
- During revision, inspect every technical noun phrase and abbreviation from the
  perspective of a reader outside that particular subfield. A sentence fails the
  writing requirement if its logic is correct only for a reader who already
  knows unstated field-specific terminology.

#### Evidence-based responses to anticipated reviewer concerns

- **Do not write in a commentary or rebuttal style.** The manuscript must not
  sound as if it is responding to an invisible reviewer through phrases such as
  `one might question`, `reviewers may argue`, `we acknowledge this concern`,
  `to address this criticism`, or repeated defensive uses of `we do not claim`.
  The paper should present a coherent scientific argument, not a prewritten
  response letter.
- Counter an anticipated reviewer concern with **evidence whenever the concern
  is empirical**. Add or foreground the experiment, control, comparison,
  baseline, ablation, repetition, denominator, threshold, packet capture,
  configuration record, authoritative source, or failure analysis that lets the
  reader resolve the concern from the paper itself.
- Build a convincing evidence chain in the manuscript:
  `research question -> required condition -> experimental comparison ->`
  `observed result -> claim at the supported strength`. Do not replace a missing
  step with persuasive commentary.
- If the available evidence cannot support the broad claim, **limit the scope of
  the question and claim** to what was actually evaluated. State the measured
  system boundary, workload, path, metric, and operating conditions positively
  and early, then draw the strongest conclusion that follows within that scope.
- Scope limitation is preferable to overclaiming followed by defensive
  qualification. For example, describe the concurrency experiment as increasing
  logical application sessions through one vehicle host and one user-equipment
  attachment; do not first present it as a 100-vehicle result and later retreat
  in a disclaimer.
- Use measurements to resolve ambiguous treatments. For example, if a condition
  is labeled as a different quality-of-service treatment but packet captures do
  not show different marking, report the observed marking and conclude that the
  intended network treatment was not verified. Do not ask the reader to accept
  or reject the treatment based on labels or argumentative prose.
- Place each scope boundary once, where it defines the research question,
  experiment, figure, or claim. Do not scatter repeated disclaimers throughout
  the manuscript. After the boundary is established, use precise scoped wording
  consistently.
- A limitations statement may explain what lies outside the evaluated scope,
  but it must not be used to rescue an unsupported central claim. If an omitted
  condition is necessary for the claim, add convincing evidence or narrow/remove
  the claim.
- Anticipated concerns about realism should be answered by explaining why the
  selected devices, paths, workloads, thresholds, and controlled factors are
  representative of the research question, while also naming the exact
  population or deployment to which the evidence applies.
- Anticipated concerns about causality should be answered with matched controls
  and the required cross-layer evidence. Without those controls, report an
  association or application-level outcome rather than speculating about a RAN,
  scheduler, transport, or radio mechanism.
- Anticipated concerns about reliability should be answered with issued-request
  denominators, deadline-qualified completion, failure accounting, repetitions,
  and confidence appropriate to the target. A statement such as `1,000/1,000
  attempts completed` must not be rhetorically promoted into an unsupported
  multi-nine reliability claim.
- If the manuscript cannot answer a likely concern with evidence, a sound scope
  boundary, or an authoritative argument, remove the affected claim rather than
  inserting commentary intended to persuade the reviewer past the gap.
- During revision, identify every sentence written mainly to preempt a reviewer
  objection. Replace it with one of four things: stronger evidence, a clearer
  experimental design, a precise scope statement, or deletion of the unsupported
  claim.

#### Sentence length and list requirements

- **Write short and concise sentences by default.** Each sentence should make
  one main claim, report one main result, explain one relationship, or perform
  one clear role in the argument.
- Split a sentence when it contains several independent claims, results,
  experimental conditions, qualifications, or implications. Do not compress a
  paragraph into one sentence through commas, semicolons, parentheses, or a
  chain of coordinating conjunctions.
- A longer or compound sentence is appropriate only when keeping the thoughts
  together makes their logical relationship clearer. This commonly applies to
  contrast, condition, qualification, cause, or consequence expressed with
  linking words or clauses such as `but`, `otherwise`, `although`, `because`,
  `whereas`, `if`, or `unless`.
- Even when a transitional relationship justifies a longer sentence, keep each
  clause short. A transition word is not permission to attach several loosely
  related ideas to the same sentence.
- Do not create choppy prose by reducing every sentence to the same length or by
  using sentence fragments. Use limited sentence-length variation when it
  improves emphasis and flow, while preserving one clear logical purpose per
  sentence.
- **Avoid lists unless a list is clearer than connected prose.** Before creating
  one, determine whether the items can be synthesized into one claim and its
  supporting explanation.
- If a list is necessary, keep it short and use numbered items in the form
  `1)`, `2)`, `3)`, and so on. Do not use a long sequence of bullets in the
  manuscript.
- **Exception:** the paper's Contributions and Insights may use bullet lists.
  These are parallel, high-level takeaways that benefit from rapid scanning.
  Keep each bullet short and self-contained, use one bullet per contribution or
  insight, and do not place experimental detail or a second embedded list inside
  a bullet.
- Every numbered list must contain genuinely parallel items. Introduce the list
  with a sentence that states what the items represent, and follow it with a
  sentence that explains their collective meaning for the argument.
- Do not create nested lists. If the content requires many items, subitems, or
  repeated fields, synthesize it in prose, use a well-designed table when exact
  comparison is necessary, or move nonessential detail to the appendix.
- During revision, flag every sentence with multiple commas, multiple
  conjunctions, a semicolon, or a long parenthetical phrase. Keep it only if the
  combined structure expresses one necessary transition of thought more clearly
  than two short sentences would.

#### Result-presentation, metric-label, and signal-category requirements

These requirements supersede older drafting notes that use `A(B)`, `response
availability`, `historical/favorable placement`, or unquantified signal labels.

- Define deadline completion as
  `C(B) = requests answered within deadline B / all issued requests`. Use
  `response completion` only for requests answered before the harness timeout.
  Every plot and table must state whether it reports deadline completion,
  response completion, or latency among completed responses.
- Report received-response round-trip-time percentiles separately from completion
  rates. A latency percentile excludes unanswered attempts; a completion rate
  includes them in its denominator.
- Classify signed handset reference-signal-received-power (RSRP) values as
  **strong** at or above -95 dBm, **common/typical** from -105 to below
  -95 dBm, and **weak** below -105 dBm. These bins aggregate the Australian
  2026 outdoor coverage-map levels: Good maps to strong, Moderate maps to
  common/typical, and Basic plus No Coverage map to weak. The stationary Uu
  workload collections cover common/typical and weak conditions; none is a
  strong-band workload run.
- Present the results in a cumulative order: interface packaging, single-path
  payload and position, payload deadlines, transport and reassembly, joint radio
  context, competing traffic, concurrent application demand, interruption, and
  the cross-application comparison. Each comparison should introduce only the
  additional variable needed for the next conclusion.
- Reference every figure and table in the body. Place each result figure with
  interpreting prose on the same page when the two-column layout permits it.
  When a wide float separates them, the caption must state the measured result,
  not merely list the panels.
- Use reader-facing axis and legend text. State units, denominators, request
  counts, deadlines, and transport direction directly. Define workload labels
  such as detector output as serialized object-detection result sizes replayed
  as IPI payloads; do not use internal labels such as `replay n=20,100`.
- Check every number in prose against the corresponding table or figure before
  rebuilding the PDF. Derived byte additions and percentages must use the same
  numerator, denominator, and framing boundary as the displayed data.

## 1. Introduction

### Purpose and intended value

The introduction must make the paper useful to all of the following groups,
without turning the paper into a broad survey:

- CAV researchers and developers deciding which application exchanges can be
  placed on direct V2X, 5G, or a combination of paths.
- Connected-vehicle and ITS developers working with traffic-signal state,
  intersection geometry, signal requests, signal-status responses, vulnerable
  road users, and vehicle warnings.
- Traffic-signal and infrastructure operators deciding what information an
  intersection must publish, at what rate, and with what freshness and failure
  semantics.
- Mobile-network operators and private-network owners deciding how to provision
  uplink and downlink capacity, coverage, tail latency, concurrent load, QoS,
  and failure recovery for vehicles.
- Radio, signal-processing, 5G-Advanced, and 6G researchers deciding which
  limitations require radio improvements and which require changes above the
  radio, such as workload reduction, deadline-aware scheduling, path selection,
  caching, or explicit application semantics.

The central value is an **application-to-network operating envelope**. The paper
should tell these groups which application communication patterns work, which
become conditional, which fail, and what must change. It should not merely
report a set of RTT measurements or present a testbed.

### Paragraph 1 — The promise of connected automated driving

**Goal:** Establish why CAVs and connected human-driven vehicles need
communication even when onboard perception, computation, or a human driver is
available.

- Buildings, trucks, vegetation, and other vehicles can obstruct both onboard
  sensors and human drivers. Neither can always infer a traffic controller's
  phase and timing data, another road user's intent, or an upcoming
  infrastructure event.
- Communication extends the information available to automated driving systems
  and human drivers:
  vehicle-to-vehicle and vehicle-to-infrastructure direct communication can
  provide nearby state and warnings, while cellular vehicle-to-network
  communication can connect a vehicle to intersections, edge applications, traffic
  management, cooperative perception, maps, planning support, and remote
  assistance.
- Give concrete examples spanning compact and data-intensive exchanges:
  BSM/PSM vehicle or road-user state; MAP/SPaT intersection geometry and signal
  timing; SRM/SSM signal request and status; an incoming-vehicle or occlusion
  warning; cooperative object lists; camera, lidar, and radar products; map
  updates; trajectory or maneuver coordination; and edge-generated guidance.
- Explain the complementary roles rather than treating “5G/V2X/NR” as one
  interchangeable technology:
  - direct V2X provides local, infrastructure-independent exchange;
  - 5G NR Uu provides vehicle-to-edge/network reach and higher-capacity IP
    service;
  - NR-V2X sidelink is intended to extend direct communication for advanced
    services, but it is not the sidelink measured in this paper.
- Standards and industry roadmaps position 5G, C-V2X, and NR-V2X as enablers of
  advanced driving, extended sensing, remote driving, collective perception,
  and cooperative maneuvering. This creates a reasonable expectation that the
  communication problem is solved. However, a target service requirement or a
  promised peak rate is not evidence that a deployed end-to-end path meets an
  application's complete exchange requirement.

**Preferred transition:** CAVs therefore need these technologies, but their
existence does not by itself establish application readiness.

### Paragraph 2 — The overlooked mismatch between messages and applications

**Goal:** Show why conventional connected-vehicle success does not automatically
imply support for complex CAV applications.

- First distinguish two application communication patterns:
  1. A **stateless update** is an independent, periodic message for which a
     newer valid message supersedes an older one. BSM, PSM, MAP, and SPaT are the main
     examples.
  2. A **stateful or complex collaborative exchange** belongs to a request,
     response, or session. It may need a specific payload, correlated response,
     freshness limit, deadline, progress/status value, and explicit failure
     outcome. Perception aid, planning aid, control aid, computation offload,
     lane-level map updates, and cooperative guidance are examples.
- Explain that J2735 provides interoperable message content for important CV
  and ITS exchanges, but an IP packet or a standalone J2735 frame does not by
  itself express the full lifecycle of a stateful edge service. The paper's
  question is therefore about both data delivery and application semantics.
- Expose the directional mismatch that is especially important to radio and
  carrier readers:
  - vehicle-to-infrastructure/edge **uplink** carries vehicle state, sensor or
    detector products, service context, telemetry, and offload input;
  - infrastructure/edge-to-vehicle **downlink** carries signal state, maps,
    warnings, cooperative results, and guidance;
  - the required balance can change by application and stage of an exchange.
- Distinguish periodic timing from event-triggered CAV demand. A driving event,
  service need, or fault can initiate a burst without a fixed start time. The
  burst may be uplink-heavy when the large object originates at the vehicle or
  downlink-heavy when a compact request triggers a large edge response. Several
  such operations may overlap.
- Make the stronger systems point that these applications also ask the network
  to behave differently over time:
  - signal state is compact, periodic, and often common to many receivers;
  - collision and blind-spot awareness is compact, periodic, and many-to-many
    on the direct sidelink;
  - intersection guidance combines recurring per-vehicle uplink state, common
    downlink intersection state, and vehicle-specific downlink plans;
  - emergency maneuver coordination arrives as an unpredictable, bidirectional
    burst with a much tighter deadline;
  - edge-assisted recovery is a long-lived stateful session, while help-center
    recovery can add a sustained, uplink-heavy video stream to a much smaller
    downlink command or path stream.
- The readiness problem is therefore about **coexistence**, not only the peak
  performance of each flow in isolation. Periodic awareness, urgent bursts,
  large sensor-derived objects, bulk transfers, and long-lived sessions may be
  active at the same time. They contend for different resources: PC5 traffic
  shares a sidelink resource pool, whereas Uu traffic uses separately scheduled
  5G uplink and downlink opportunities and also traverses CPE, transport, core,
  edge-service, and possibly backhaul queues. Do not imply that PC5 packets
  consume Uu uplink/downlink resources merely because both are called V2X.
- Ask explicitly whether a deadline-critical compact exchange still completes
  when a high-rate or long-lived flow is present. Peak rate, an isolated median,
  and a technology label cannot answer whether the network protects the right
  traffic at the right time.
- Use standards requirements to show the range rather than to assert measured
  capability. For example, TS 22.186 includes extended-sensor cases reaching
  very high data rates and a remote-driving case with a much larger uplink than
  downlink requirement. These examples make uplink/downlink allocation an
  application question, not merely a carrier configuration detail.
- State why headline bandwidth is inadequate. A useful path must deliver a
  complete and fresh application object, not merely achieve a high bulk rate or
  low median latency for the packets that happened to arrive.
- Name the possible latency costs—application encoding, protocol/broker
  behavior, queueing, RAN scheduling, retransmission, core forwarding,
  fragmentation/reassembly, and request/response handling—while stating that
  the experiments measure their aggregate application-level RTT rather than
  decomposing every component.
- Introduce the dimensions that define readiness: message semantics; payload
  and complete-object size; uplink/downlink direction; response availability;
  median and tail RTT; application time budget; signal strength and coverage;
  distance, obstruction, and mobility; competing traffic; concurrent vehicles;
  transport behavior; RAN resource allocation; QoS realization; and behavior
  during receiver or broker failure.

**Preferred transition:** These dimensions turn the broad promise of connected
automation into a concrete systems question.

### Paragraph 3 — Central question and subquestions

**Central question:**

> Can today's deployed V2X and 5G paths each support CV and CAV applications,
> and at what workload size and operating conditions does each path stop
> meeting the required response latency and availability?

Use “deployed V2X and 5G paths” in the empirical question. The broader 5G,
NR-V2X, 5G-Advanced, and 6G discussion should be presented as motivation and
design implications, because the measured direct path is LTE C-V2X PC5 and the
measured cellular path is private-5G NR Uu; the paper does not measure an
NR-V2X sidelink deployment.

Break the central question into three checks, without presenting them as three
additional paper insights:

- **Interface check:** Can CV updates and CAV operation exchanges share a clear
  contract that preserves source, time, communication path, session,
  correlation, freshness, payload, response status, and failure?
- **Operating-envelope check:** Apply compact CV and larger CAV workload sizes
  to both direct V2X and 5G. Across realistic payloads and field conditions,
  what combination of response availability, RTT distribution, complete-object
  size, and time budget can each path sustain, both alone and while different
  periodic, bursty, and sustained application flows share resources?
- **Design check:** Which limitations should CAV/ITS developers work around
  today, and which requirements should carriers, radio designers, and
  5G-Advanced/6G systems address next?

### Paragraph 4 — Answer part I: IPI

**Goal:** Introduce IPI as the protocol/interface needed to pose and measure the
readiness question at the application level.

- Present the **Intersection Programming Interface (IPI)** as a typed,
  transport-independent protocol for stateless CV/ITS applications and
  stateful CAV applications. IPI is the proposed protocol; Mocar is only the
  vendor of the off-the-shelf C-V2X equipment, and Edge4AV is only the paper
  title.
- Explain how IPI carries the J2735 message set. Typed helpers cover common BSM,
  PSM, MAP, SPaT, SRM, and SSM structures, while the encoded-payload path
  preserves any J2735 message. Its CAV operation model adds the request/session
  semantics needed by more complex applications.
- Name the contract features that make experiments meaningful: typed
  envelopes; message/source/intersection identifiers; send time and path;
  optional session and correlation identifiers; payload encoding; freshness or
  expiration; accepted, in-progress, completed, and rejected application states;
  acknowledgements; and distinct timeout, missing-response, validation, and
  restart outcomes.
- Give two short examples:
  - a CV or ITS client requests signal service or status using SRM/SSM and
    receives MAP/SPaT state;
  - a CAV requests perception assistance and receives a correlated object list
    or warning through the cooperative-service path.
- Blind-spot/occlusion or incoming-vehicle warning should be described as a
  target application whose inputs or results IPI can carry. Do **not** claim
  that the experiments validate a complete blind-spot-warning algorithm.
- Make clear that IPI is not a perception model, planner, radio technology, or
  testbed. It is the common application contract that lets the same success,
  freshness, correlation, and timing questions be asked across V2X and 5G.

**Preferred transition:** With this common contract, the paper can test the
communication paths against valid application objects rather than synthetic
packet sizes alone.

### Paragraph 5 — Answer part II: real vehicle, real communication paths, and valid workloads

**Goal:** Establish the realism and breadth of the evaluation in two or three
dense sentences in the eventual introduction. The following checklist is
intentionally exhaustive; the final prose may group dimensions, but it must not
silently omit a dimension from the claimed evaluation scope.

#### Deployment and system boundary

- A real instrumented autonomous vehicle carries the 5G gateway and the
  off-the-shelf C-V2X OBU.
- The Uu deployment is tightly controlled: the CAV gateway is the only physical
  UE, the AirSpeed radio and its 40-MHz channel are dedicated to the experiment,
  and the n48 band is clean without ambient traffic or unrelated radio
  contention. Background streams and application-client demand are introduced
  only as controlled experimental variables behind that one UE.
- The private-5G path uses an on-site Airspan AirSpeed 2900 n48 gNodeB, CBRS GAA
  spectrum, 40 MHz bandwidth, two configured cells, provisioned SIMs, a Cisco
  Meraki MG52-HW 5G gateway/CPE with a Telit FN990A40 radio, a Cisco private-5G
  core/edge service, a local MX250 security/SD-WAN appliance, a remote
  NMS-side MX68CW security/SD-WAN appliance, and an application edge host
  directly connected to the local MX250.
- The retained radio inventory reports a CBSD Category B deployment, 17 dBi
  antenna gain, 33 dBm cell transmit power, 34 dBm/MHz cell EIRP, and Federated
  Wireless as the primary SAS provider. These are configuration/context fields,
  not measured per-request radio behavior; exact SAS grants and installed
  antenna azimuth/downtilt remain unresolved.
- The MG52 was fixed on the vehicle's trunk floor with its front face upward
  and used a 1 Gbps full-duplex Ethernet link to the vehicle host. The
  application endpoint was on site and directly attached to the local MX250.
  The manuscript must distinguish this local application endpoint from the
  remote management/NMS-side MX68CW and must not place that remote appliance
  in the timed user-data path without route, packet-capture, or administrator
  evidence.
- Describe this as a **deployment-realistic private-5G path containing the same
  classes of RAN, SIM, gateway/CPE, packet-core, and edge components used in
  operator architectures**. Do not say that one private deployment reproduces
  every public 5G topology, scheduler, load, policy, or coverage condition.
- Keep the three Cisco/Meraki device roles distinct. The vehicle MG52 converts
  5G SA/NSA cellular access to an Ethernet handoff. The on-site MX250 provides
  security/SD-WAN routing and is the attachment point for the local application
  host. The remote device identified by the user is an NMS-side MX68CW, an MX
  security/SD-WAN appliance with integrated LTE and Wi-Fi capabilities. Neither
  MX model should be called the 5G packet core; Cisco's private-5G architecture
  describes packet-core functions as running on a separate edge appliance and
  control/management platform.
- The direct path uses a certified production Mocar LTE C-V2X OBU/RSU pair over
  PC5. Standard operation is checked with 10/10 SPaT delivery and bidirectional
  BSM reception before the correlated packet-data experiments.

#### Direct C-V2X experimental dimensions

- **Message/path:** standard BSM and SPaT callbacks plus a custom correlated
  request/reply over the vendor's packet-data path. The custom probe must not be
  labeled as a standardized BSM or SPaT frame.
- **Payload:** 0, 256, 512, 1,024, and 2,048 B stationary sweeps; the installed
  vendor SDK's 4,080 B maximum application packet is an implementation-specific
  limit, not a universal C-V2X limit.
- **Repetition/timing:** normally 1,000 attempts per completed stationary
  condition, a 100 ms send interval, and a 1,000 ms sender timeout. Operator-
  stopped, timeout-dominant runs retain their observed failures and identify
  skipped/remaining attempts rather than hiding them.
- **Radio/spatial condition:** multiple stationary points covering LOS, NLOS,
  and building obstruction; distance from the RSU; and normalized signal
  strength. A short distance must not be equated with a strong path when a
  building blocks LOS.
- **Mobility:** a repeated driven route with a 256 B payload, a 200 ms interval,
  500 or 1,000 ms timeouts, vehicle speed, GNSS joins, spatial coverage, and
  successful-reply RTT separated from availability over every issued request. Two fully
  characterized drives had median speeds of 7.13 and 7.54 m/s.
- **Diagnostic coverage:** the OBU signal probe validated successful traffic
  but produced no usable RSSI value from the diagnostic interface; use the
  normalized signal-strength analysis and spatial outcomes at the appropriate
  strength.

#### Private-5G experimental dimensions

- **Application messages:** compact SPaT/state-mirror updates; compact
  IPI-CooperativeService requests; detector-output object lists; map/perception-
  scale payloads; and large transfer chunks.
- **Transport:** TCP; MQTT 3.1.1 QoS 0 over TCP; raw UDP; and application-level
  UDP fragmentation/reassembly. Raw oversized UDP and fragmented UDP are
  separate sensitivity tests with different failure behavior.
- **Payload scale:** 0--4 KiB compact sweeps; dataset-derived sizes such as
  5,262, 5,789, 10,507, and 40,337 B; detector outputs spanning
  13,216--25,024 B; a 60 KiB upper condition; 128--512 KiB map/perception
  classes; and 1--2 MiB transfer chunks. Distinguish a chunk RTT from completion
  time for a full image, point cloud, or map.
- **Application direction:** uplink-heavy RTT places the declared object in the
  vehicle-to-edge request and returns a compact acknowledgment. Downlink-heavy
  RTT sends a compact vehicle request and returns an exact 1--500 KiB object
  from the edge. Exact 50-MiB TCP transfers measure upload and download
  application goodput separately.
- **Repetition/timing:** most completed baseline conditions contain 1,000
  sequential requests sent at 200 ms intervals. UDP sensitivity conditions use
  explicit sizes including 0, 256, 1,024, 1,400, 4,096, 19,648--25,024 B, and
  60 KiB; aborted or partial diagnostics remain labeled as such.
- **Repeated context:** repeated stationary runs across dates, locations, and
  recorded weather labels. Weather is context, not a controlled causal factor.
- **Radio condition and geography:** a 16-point survey spanning RSRP from -121
  to -91 dBm, RSRQ from -19 to -10 dB, and SNR from 3.5 to 27.5 dB; mapped run
  locations; strong/common/medium/weak candidate locations; and fixed gateway
  placement.
- **Background uplink load:** idle versus an offered 25 Mbps vehicle-to-edge
  load and 1, 2, or 4 load streams. Report offered load separately from achieved
  throughput.
- **Exact directional goodput:** in the matched August 17 experiment,
  `70/20/10` and `40/40/20` averaged 12.000 and 2.836 Mbit/s for ten exact
  vehicle-to-edge uploads per profile. The corresponding edge-to-vehicle means
  were 126.786 and 99.756 Mbit/s. These are endpoint application-goodput
  measurements, not PHY capacity or one-way latency.
- **Concurrent application clients:** 1, 2, 5, 10, 20, 50, and 100 logical
  clients over TCP, MQTT, and UDP, each nominally issuing five 1 KiB requests
  per second, reaching 500 requests/s in aggregate. These clients share one
  vehicle host and one MG52 radio attachment, so they are a controlled proxy
  for aggregate application demand and end-to-end contention, not 100 physical
  vehicles or 100 independently scheduled UEs. Include both stronger- and
  weak-signal sweeps, response availability, latency tails, loss, and per-client
  fairness.
- **Signal/concurrency interaction:** the weak-signal 1--100-client sweep is
  important because equal application demand does not imply equal radio
  resource cost. NR selects modulation and coding with different spectral
  efficiencies; a weak-radio client can require more time-frequency resources
  or retransmission effort for the same useful bytes than a client with a
  stronger link. The experiment probes the combined application effect, but
  absent per-UE PRB, MCS, BLER, and retransmission counters it cannot quantify
  or causally attribute that radio-resource cost.
- **Application time budgets:** post-hoc comparisons at 100, 200, 500, and
  1,000 ms. The 200 ms point is retained specifically for the high-level
  remote-recovery path; do not turn it into a universal CAV deadline.
  Sender timeouts ranged from 3 to 60 s and must not be confused with these
  application budgets. Do not infer 10 or 25 ms support when those budgets were
  not met.
- **QoS:** default traffic versus the application-side `5qi-mapped` label. Both
  vehicle-host packet captures showed IP TOS `0x0`, so the current evidence
  does not prove packet marking, a dedicated bearer, or network-enforced 5QI.
- **Failure:** TCP and UDP receiver restarts, MQTT receiver restart, and MQTT
  broker restart while sending 1 KiB requests. Report missed requests and tail
  changes; no alternative network fallback path was actually exercised.
- **Follow-up application blocks:** C1 = 1 KiB idle; C2 = 23,968 B detector p99
  payload idle; C3 = 1 KiB under offered uplink load; C4 = 100 concurrent
  clients. Each transport used 500 sequential requests per repetition, two
  repetitions, and its own five-minute collection interval.
- **RAN configuration context:** every application campaign before the August
  15--18 TDD study used 70/20/10 downlink/uplink/dynamic allocation. The study
  also used 40/40/20 with 10D4G frame packing unchanged. The August 15--16
  uplink-heavy diagnostic cannot rank the profiles because route, cell
  administrative state, and serving-cell selection changed. The August 17
  downlink-heavy and exact directional-transfer blocks provide the matched
  comparison. They show workload-dependent latency and higher measured
  70/20/10 application goodput in both directions; nominal allocation alone
  does not determine endpoint performance.
  The attempted 30/60/10 configuration was not supported as a usable
  measurement configuration in this deployment and has no valid experiment
  result. Treat this as a platform/configuration constraint, not a third TDD
  condition. The retained configuration inventory identifies Cell 1 as
  NR-ARFCN 637992
  (3569.880 MHz reference frequency) and Cell 2 as NR-ARFCN 645334
  (3680.010 MHz reference frequency); serving-cell attribution remains subject
  to the stated export-alignment boundary. The inventory also records at most
  one uplink layer, at most two downlink layers, and downlink 256-QAM enabled;
  these are configured capabilities, not proof of the rank or modulation used
  for a measured request.
- **Core/session context:** the user confirms DNN `cisco5g` and default 5QI 9
  for the experiment sessions; QFI was not provided. This does not validate the
  separate `5qi-mapped` application condition as a nondefault QoS flow.
- **Latency definition:** most vehicle and edge clocks were unsynchronized, so
  sender-side RTT is the latency metric. Availability uses every attempt; RTT
  percentiles use accepted replies only. Do not report one-way 5G latency from
  the retained raw timestamps.
- **Deployment load boundary:** the private network was tightly controlled. The
  CAV gateway was the only physical UE, the radio and 40-MHz channel were
  dedicated, and the n48 band was clean without ambient contention. The offered-uplink-load and
  client-demand experiments deliberately approximate two consequences of a
  loaded deployment—competing traffic and aggregate request demand—but they do
  not recreate independent public-network UEs, an operator scheduler's full
  user mix, handover, roaming, backhaul distance, or operator policy.

#### Workload-validity dimensions

- Source candidate data from public cooperative-driving datasets and official
  benchmark releases: OpenDAIR-V2X/DAIR-V2X-C, V2X-Seq, TruckV2X, and
  V2X-Radar. OPV2V, V2XSet, V2V4Real, and V2X-Real are present only as source
  registry entries and must not be described as completed result sets.
- The current payload manifest indexes 22,431 staged artifacts. This inventory
  supports workload-size selection; it is not itself an experiment sample count
  or a network-performance result.
- The staged corpus contains image, point-cloud, annotation, and map artifacts.
  Observed object sizes include images of 25.5--666.9 KiB, point clouds from
  roughly 15 KiB to 3.85 MiB, and HD-map files from 22.3 to 44.9 MiB.
- The four-GPU artifact check validates dataset plumbing and processing of 224
  files; it is not a wireless result or a detector-accuracy claim.
- The V2X-Radar environment check constructs a 922-sample cooperative split.
  The completed detector run processes all 922 samples without execution
  failure and yields object-list payloads of 13,216--25,024 B. Detector compute
  time is measured separately and excluded from network RTT.
- Local IPI loopback validates message handling for dataset-derived sizes; it
  does not establish wireless latency. Private-5G replay of detector-sized
  payloads is the communication evidence.

### Paragraph 6 — Preview the answer as an operating envelope

**Goal:** State the result pattern strongly enough to answer the opening
question, while reserving detailed numbers and mechanisms for the results
section.

- Direct C-V2X correctly carries standard compact messages and compact custom
  exchanges under favorable conditions. Its usefulness changes sharply with
  the joint effect of packet size and radio condition: as signal strength
  degrades, 1--2 KiB delivery becomes unreliable, and mobile coverage gaps are
  visible in availability over every issued request even when successful-packet RTT remains
  low.
- The measured 5G path carries both compact CV workloads and substantially
  larger CAV workloads. As payload size increases, the path crosses the timing
  requirements of progressively fewer applications. Directional allocation,
  deliberately introduced traffic, application-client demand, transport
  behavior, and interruptions move this boundary by changing response RTT or
  availability.
- The controlled mixed-flow experiment shows that a compact 1 KiB exchange
  develops much larger tails when offered uplink traffic shares the
  vehicle-originated path. The client-demand sweep separately shows how
  concurrent application exchanges change aggregate and per-client response
  behavior behind the same physical vehicle gateway.
- The readiness answer is therefore application-specific and condition-
  dependent. Neither a technology label, peak throughput, median RTT, nor
  successful-packet latency alone is a sufficient readiness test.

### Paragraph 7 — Contributions

Keep the contributions separate from the experimental insights. This paper has
two main contributions:

1. **Application-driven field evaluation.** We conduct a systematic evaluation
   on a real autonomous vehicle using two independently measured communication
   paths: a certified off-the-shelf LTE C-V2X PC5 OBU--RSU path and an on-site
   private-5G NR Uu vehicle-to-edge path containing the radio, gateway, SIM,
   mobile core, and edge components. We evaluate the paths using standard
   vehicular messages and dataset- and detector-derived CAV workloads while
   varying payload size, communication direction, transport, signal condition,
   distance and obstruction, mobility, mixed foreground/background uplink load,
   application-client demand, application time budget, QoS condition, and endpoint or
   broker interruption.
2. **Enabling IPI protocol and implementation.** We design and implement the
   Intersection Programming Interface, an application-layer protocol that
   supports compact CV/ITS messages and correlated CAV operations through one
   logical application contract. IPI preserves typed or pre-encoded J2735
   content and adds operation association, freshness, generic outcome, and
   application-content fields through profile-specific wire representations.

End the contribution paragraph with this logical bridge rather than introducing
a third, overlapping contribution:

> IPI makes both workload classes available to the same application-facing
> evaluation. The field study then determines how far each measured
> communication path supports them before response latency, deadline completion,
> or directional goodput no longer meets the application requirement. The
> measurements produce three main insights.

### Paragraph 8 — Three insights

The paper must contain **exactly these three numbered top-level insights**. Do
not introduce them with `For ...` audience labels. Each insight must instead
state a deeper design decision for its stakeholder and connect that decision to
the evidence. The supporting explanation is part of the same insight, not an
additional top-level insight.

1. **The measured commercial direct V2X path supports compact J2735 messages
   only within a limited payload and coverage envelope.** Nearby roadside
   infrastructure can make these messages useful at urban intersections.
   Building-obstructed streets, highways, and sparsely served roads may require
   carefully placed roadside infrastructure or a supplemental path.
2. **The measured 5G uplink supports only severely limited CAV packet sizes
   within decision deadlines.** Until the uplink envelope
   expands, CAV developers must select, compress, or progressively transmit
   vehicle-originated state according to the decision deadline. Keep the
   approximately 20-KiB measured boundary and the matched downlink contrast in
   the Results evidence, not in this top-level insight sentence.
3. **5G/6G radios must support event-triggered CAV bursts in either direction
   and isolate concurrent flows.** Nominal frame allocation does not determine
   application performance by itself. Radio implementations must preserve
   end-to-end operating state across profile changes. Schedulers must allocate
   resources according to each burst's direction and deadline while protecting
   simultaneous exchanges.

### Claim boundaries to preserve when writing the introduction

- Do not call the work a testbed contribution. The primary contribution is the
  evidence-grounded communication-readiness characterization; IPI is the
  enabling systems contribution that supplies its common workload contract.
- Do not present Mocar as the paper or system name. It is the equipment vendor.
- Do not present Edge4AV as the proposed system. It is the paper title.
- Do not imply that the experiments measured NR-V2X sidelink, independent
  public-network UEs, public-network scheduler policy, handover between cells,
  roaming, public backhaul distance, Internet/cloud latency, security overhead,
  full sensor-object completion, or a complete perception-to-control loop.
  State positively that offered uplink load and 1--100 application clients are the
  controlled proxies used to stress competing traffic and aggregate request
  demand through one vehicle host and one MG52 attachment.
- Do not use unsynchronized timestamps as one-way latency, treat weather labels
  as causal variables, or treat `5qi-mapped` as verified network QoS. All
  primary application campaigns used `70/20/10`. Present the August 15--16 TDD
  application RTT runs as an unusable latency diagnostic because route, cell
  state, and serving-cell selection changed during reconfiguration.
- Keep strong conclusions tied to the measured operating envelope. Use the
  private deployment to expose mechanisms and requirements, not to claim one
  universal performance number for all public or private 5G networks.

### Primary external sources to use in the eventual introduction

These sources establish the application need and standards vision. They do not
replace the paper's own field evidence.

- [3GPP/ETSI TS 22.186, Service requirements for enhanced V2X scenarios,
  Release 19](https://www.etsi.org/deliver/etsi_ts/122100_122199/122186/19.00.00_60/ts_122186v190000p.pdf):
  use for advanced driving, extended sensors, remote driving, and the diversity
  of rate, latency, and reliability targets. Note that these are requirements,
  not proof of deployed performance.
- [5GAA, C-V2X Use Cases and Service Level Requirements, Volume
  I](https://5gaa.org/content/uploads/2023/02/5gaa-t-200111-tr-c-v2x-use-cases-and-service-level-requirements-vol-i-v3.0-clean-version-1.pdf):
  use for high-definition sensor sharing, cooperative awareness, and the
  distinction between compact processed data and much larger unprocessed
  sensor data.
- [NGMN, 5G TDD Uplink white
  paper](https://www.ngmn.org/wp-content/uploads/220117-5G-TDD-Uplink-White-Paper-v1.0.pdf):
  use to motivate why industrial/vertical applications can require more uplink
  capacity and lower uplink latency than downlink-heavy consumer TDD
  configurations naturally provide.
- [one6G, 6G Technology Overview, fifth
  edition](https://one6g.org/download/6324/?tmstv=1758266695): use for the 6G
  direction toward cooperative connected and automated mobility, collective
  perception, real-time context/intent exchange, and infrastructure-assisted
  coordination.
- [5GAA, NR-V2X direct communication evaluation
  approach](https://5gaa.org/5g-v2x-direct-communication-evaluation-approach/):
  use to support the need for realistic evaluation against automotive
  requirements and system limitations. Do not cite it as evidence that this
  paper measured NR-V2X sidelink.
- [Cisco Meraki, MX67 and MX68
  datasheet](https://documentation.meraki.com/MX/MX_Overviews_and_Specifications/MX67_and_MX68_Datasheet):
  use only to resolve the MX68CW's product role before finalizing the deployment
  description.

## 2. Related Work

### Section purpose and writing logic

The purpose of this section is to give the reader the technical context needed
to understand the paper's novelty. Each subsection must perform the same three
steps:

1. **Prior work:** explain what the relevant standards, systems, and
   measurements already provide.
2. **Remaining gap:** identify the important capability or evidence that is
   still missing across that body of work. The gap must be collective and
   precise; it must not be manufactured by criticizing one paper for not
   solving a problem outside its scope.
3. **How this paper fills the gap:** connect the gap directly to either IPI,
   the field evaluation, or the design insights derived from that evaluation.

The `Prior work`, `Remaining gap`, and `How this paper fills the gap` labels
below are outline scaffolding only. They are not proposed subsection headings
or manuscript wording. The final paper should express each sequence as
connected academic prose. Standards and application platforms,
application-specific CAV systems, and field studies of direct and
network-assisted paths address different parts of CV/CAV communication. The
primary gap is an **evidence gap**: existing evaluations do not determine
whether today's direct V2X and 5G paths can each carry both workload classes or
identify the payload sizes and network conditions at which either path misses
application deadlines.

The evaluation also needs a stable application representation for both
workload classes. IPI fills this enabling systems gap by placing compact J2735
content and correlated CAV operations behind one application contract with
profile-specific serialized forms. The real-vehicle experiments provide the
paper's primary contribution. The three insights explain what the measured
findings mean for CAV, CV, ITS, carrier, radio, and 5G-Advanced/6G researchers.

### Visual presentation recommendation — existing technologies, gap, and future requirements

Do not use a dense feature table or a quadrant that ranks unlike papers on
subjective axes. If the manuscript has room for a positioning figure, use a
left-to-right capability-gap diagram built around the paper's central question.
The Related Work section should still provide the detailed comparisons in
academic prose.

#### Left — today's V2X/5G technologies and studies

Group representative prior work by what it contributes rather than listing
every paper:

- **standards and messages:** J2735, 3GPP V2X/VAE, and MEC/5GAA enablers;
- **platforms, applications, and middleware:** CARMA, AutowareV2X, cooperative-
  perception systems, and Tentacles; and
- **deployed communication and evidence:** commercial C-V2X PC5 devices,
  public/private 5G, automotive testbeds, corridor trials, and application-
  specific measurements.

This side should communicate that substantial technology and research already
exist. Do not portray prior work as primitive or unsuccessful.

#### Middle — the unresolved readiness and limiting-factor gap

Use the center as a narrow visual bottleneck containing the two questions that
the existing pieces do not answer together:

1. **Readiness gap:** can today's direct V2X and cellular paths each carry
   compact CV and larger CAV workloads within their application deadlines?
2. **Limiting-factor gap:** at what payload size and under which signal, load,
   concurrency, transport, direction, or failure condition does each path miss
   an application deadline?

Show the paper's two contributions as the response inside or directly beneath
this middle region: the application-level field evaluation addresses the
evidence gap, and IPI supplies the workload representation needed by that
evaluation. Avoid a decorative bridge or
chasm metaphor; use a clean transition or narrowing flow.

#### Right — communication support required by future CAV/ITS applications

Do not label this side “dream CAV/ITS” in the paper. Use an academic title such
as **Application-ready communication for future CAV/ITS**. Its target
capabilities are:

- interoperable support for stateless CV/ITS applications and stateful CAV
  applications;
- complete results delivered with explicit deadline, freshness, availability,
  and failure semantics;
- support for compact state, processed perception, maps, and larger sensor-
  derived objects according to their different time budgets;
- direction-, traffic-pattern-, signal-, load-, and concurrency-aware resource
  and path use that preserves critical exchanges under mixed-service load; and
- adaptive representation, verified QoS, caching/prefetch, and explicit local
  or alternate-path fallback when real-time completion is not possible.

The right side is a requirements target, not a claim that this paper implements
the complete future system. IPI and the evaluation identify part of the path
toward it: the necessary application semantics, the measured operating limits,
and the design inputs exposed by the three insights.

#### Figure discipline

- Use the title **From today's V2X/5G technologies to application-ready CAV/ITS
  communication**.
- Use three visually distinct regions with a clear left-to-right reading order.
- Use categories and two or three representative examples, not every citation.
- Keep citations and qualifications in the surrounding Related Work prose and
  caption.
- Do not use checkmarks, quality scores, or a top-right “winner” position.
- Place the figure near the end of Related Work, immediately before the IPI
  design section, if the final figure budget allows it; otherwise keep the
  argument in prose.

### 2.1 V2X standards and application platforms

#### Prior work — interoperable messages, network services, and deployed platforms

- SAE J2735 standardizes interoperable application messages including BSM,
  PSM, MAP, SPaT, SRM, and SSM. These messages support connected
  vehicle safety, traffic-signal interaction, and infrastructure awareness.
- 3GPP TS 23.285 and TS 23.287 define LTE and NR V2X architectures and the use
  of PC5 and Uu communication. The 3GPP V2X Application Enabler in TS 23.286
  adds important application-layer functions such as registration, message
  delivery, group delivery, file distribution, resource requirements, service
  continuity, and sessions.
- ETSI MEC V2X services and the 5GAA application-layer architecture expose
  network information, predicted QoS, application enablers, and relationships
  among vehicles, infrastructure, edge applications, and V2X servers.
- CARMA Platform, CARMA Streets, and V2X Hub operationalize cooperative driving
  and traffic-infrastructure exchanges. AutowareV2X connects ETSI C-ITS
  facilities and collective-perception messages to an autonomous-driving
  stack, while OpenCDA supports cooperative-driving research and integration.

#### Remaining gap — no common application protocol for stateless CV/ITS applications and stateful CAV applications

J2735 messages, network-assisted service procedures, and CAV application
systems provide message definitions, network architecture, proposed
applications, and platform-specific integrations. The current CV/CAV/ITS
landscape nevertheless lacks a coherent application protocol across these
components. J2735 standardizes individual messages rather than the state of a
multi-message CAV operation. The 3GPP V2X Application Enabler (VAE) and ETSI
Multi-access Edge Computing (MEC) services add sessions and delivery functions,
but leave stateful CAV application messages to individual applications.
Existing platforms similarly expose the applications and message sets
implemented by their own stacks. No current standard or platform provides one
application protocol that supports stateless CV/ITS applications and stateful
CAV applications.

#### How this paper fills the gap — IPI

IPI supplies that missing application protocol. It provides one
transport-independent interface for stateless CV/ITS applications and stateful
CAV applications. Its two interaction modes carry typed or encoded J2735
messages and correlated CAV operations. The claim should be feature-based: the current
landscape does not provide this exact combination in one protocol. The paper
must not make the broader claim that IPI is the first V2X API, the first V2X
application middleware, or the first software integration between V2X and an
autonomous-driving stack.

#### Core sources

- [SAE J2735 V2X Communications Message Set
  Dictionary](https://saemobilus.sae.org/standards/j2735_202409-v2x-communications-message-set-dictionary)
- [3GPP/ETSI TS 23.286 V2X Application
  Enabler](https://www.etsi.org/deliver/etsi_ts/123200_123299/123286/18.06.00_60/ts_123286v180600p.pdf)
- [3GPP/ETSI TS 23.287 NR V2X
  architecture](https://www.etsi.org/deliver/etsi_ts/123200_123299/123287/18.04.00_60/ts_123287v180400p.pdf)
- [ETSI MEC 030 V2X Information
  Service](https://www.etsi.org/deliver/etsi_gs/MEC/001_099/030/03.02.01_60/gs_mec030v030201p.pdf)
- [AutowareV2X](https://tus.elsevierpure.com/en/publications/autowarev2x-reliable-v2x-communication-and-collective-perception-/)
- [USDOT V2X Hub](https://github.com/usdot-fhwa-OPS/V2X-Hub)
- [CARMA Platform](https://usdot-fhwa-stol.github.io/documentation/carma-platform/md_docs_System_description.html)

### 2.2 Networked CAV applications and communication middleware

#### Prior work — application-specific systems and network-aware execution

- Cooperative-perception and infrastructure-assistance systems demonstrate why
  CAV communication extends beyond compact safety messages. EMP exchanges and
  merges vehicle sensor data at an edge server; AutoCast schedules useful
  LiDAR information over direct communication; VIPS performs
  vehicle-infrastructure perception fusion; VI-Map communicates compact
  representations for infrastructure-assisted map construction; and Soar
  coordinates several roadside CAV applications.
- These systems also establish that representation is a network decision. Raw
  sensor data, encoded images, point clouds, intermediate features, detected
  objects, maps, trajectories, and control messages have different sizes,
  deadlines, and consequences when delivery is incomplete.
- Deadline-aware autonomous-driving systems such as D3 and ERDOS connect
  end-to-end application deadlines to the execution of individual operators.
  They show that timeliness belongs to the complete application pipeline, not
  merely to one packet or one middleware call.
- Tentacles is the closest network-middleware comparison. It gives
  applications a unified abstraction over multiple network interfaces, accepts
  QoS requirements, characterizes networks by delay, bandwidth, reliability,
  and security, selects or combines interfaces, changes data quality, and falls
  back to local execution when an edge result misses its deadline. Its main
  effectiveness evaluation emulates a multi-network V2X environment using two
  Wi-Fi interfaces, bandwidth control, and injected delay, while a smaller
  commercial-route study reports 4G/5G latency observations.

#### Remaining gap — application systems and network abstractions use separate application protocols

This literature demonstrates valuable applications and adaptive mechanisms,
but each application defines its own exchanged objects, request lifecycle, and
failure handling. Tentacles makes network capability and application QoS
visible, but it does not define the domain semantics of conventional ITS
messages and general CAV services. Conversely, message standards define domain
objects but do not tell a network-aware middleware that a response belongs to a
particular request, has expired, represents a failed service, or must complete
within an application deadline. Application-specific evaluations also do not
provide one common basis for comparing compact CV exchanges with larger,
stateful CAV exchanges.

#### How this paper fills the gap — one protocol and representative workload classes

IPI supplies one application protocol for both workload classes, while the evaluation uses
standard messages and object sizes derived from real CAV applications and
datasets. IPI and Tentacles should be presented as complementary: IPI defines
what application operation is being exchanged and its state; a Tentacles-like
system could decide how that operation should be carried. Edge4AV does not
claim to invent cooperative perception, adaptive representation, compression,
deadline scheduling, path selection, or fallback. It supplies the shared
semantics and field evidence needed to use those mechanisms across application
classes.

#### Core sources

- [EMP](https://zhan6841.github.io/assets/pdf/paper/emp-mobicom21.pdf), MobiCom
  2021
- [AutoCast](https://par.nsf.gov/servlets/purl/10341257), MobiSys 2022
- [VIPS](https://yanzhenyu.com/assets/pdf/VIPS-MobiCom22.pdf), MobiCom 2022
- [VI-Map](https://doi.org/10.1145/3570361.3613280), MobiCom 2023
- [Soar](https://yanzhenyu.com/assets/pdf/Soar-MobiCom24.pdf), MobiCom 2024
- [Tentacles](https://weisongshi.org/papers/wu24-Tentacles.pdf), IEEE VTC Fall
  2024
- [INTERNEURON](https://arxiv.org/abs/2210.15939), Tentacles precursor
- [D3/ERDOS](https://pschafhalter.com/d3/), EuroSys 2022

### 2.3 Direct V2X and C-V2X PC5 evaluation

#### Prior work — radio, packet, frame, and application behavior

- A large body of DSRC, ITS-G5, LTE C-V2X, and NR-V2X research measures range,
  packet delivery, congestion, mobility, resource allocation, and channel
  behavior. NIST and USDOT programs further test standards compliance and
  interoperability among production devices.
- SEE-V2X is explicitly application-centric. It uses commercial,
  standards-compliant C-V2X radios, combines BSM and perception traffic, and
  collects application traces, MAC traces, and sidelink control information in
  several environments.
- Adaptive C-V2X sidelink work transfers fragmented RGB images over commercial
  Release-14 equipment, evaluates complete-frame loss and goodput under
  different MCS, packet-size, location, and speed settings, and shows that one
  missing fragment can invalidate the application object.
- CooperScene collects synchronized camera, LiDAR, localization, annotation,
  and C-V2X network data from multiple CAVs and an RSU. Hardware-in-the-loop
  studies connect PC5 packet loss and transmission frequency to cooperative
  driving time.

#### Remaining gap — direct-link results do not answer the paper's cross-path application-readiness question

Prior PC5 work already demonstrates application traffic, commercial radios,
fragmented objects, mobility, and complete-frame outcomes. The gap is therefore
not a lack of realistic PC5 measurement. Rather, these studies primarily
characterize the direct link, one application, or a particular protocol
adaptation. They do not establish, under one application-level criterion, how
standard CV messages and multiple stateful CAV object classes behave across
both direct V2X and a vehicle-to-edge cellular path. They also do not provide
the particular combination examined here: response availability over every issued request,
completed-response RTT, request/response correlation, payload size, radio
condition, obstruction and distance, repetition, and mobility, followed by
comparison with independently measured 5G service exchanges.

#### How this paper fills the gap — one side of the application-level field evaluation

The C-V2X portion of Edge4AV measures standard J2735 operation and correlated
request/response probes through a certified off-the-shelf LTE C-V2X PC5
OBU–RSU path. It keeps failed and missing attempts in the availability
denominator and distinguishes successful-response RTT from overall service
availability. It then relates the results to common application budgets and to
the independently measured private-5G path. Conclusions must remain scoped to
the measured LTE C-V2X equipment and conditions; the study does not claim a
universal limit for NR-V2X sidelink or all PC5 implementations.

#### Core sources

- [SEE-V2X](https://cisl.ucr.edu/SEE-V2X/), SenSys 2025
- [Adaptive C-V2X Sidelink Communications for Vehicular Applications Beyond
  Safety Messages](https://esdat.ucsd.edu/sites/esdat.ucsd.edu/files/publications/Adaptive_C_V2X_Sidelink_Communications_for_Vehicular_Applications_Beyond_Safety_Messages___camera_ready.pdf)
- [CooperScene](https://cisl.ucr.edu/CooperScene/), ECCV 2026
- [Characterizing the Impact of C-V2X PC5 Radio on Cooperative Driving
  Automation](https://saemobilus.sae.org/papers/characterizing-impact-c-v2x-pc5-radio-cooperative-driving-automation-hardware-loop-experiments-2026-01-0077)
- [NIST C-V2X interoperability testing
  datasets](https://www.nist.gov/publications/c-v2x-interoperability-testing-datasets-description-and-use)

### 2.4 Cellular, 5G, and edge support for automated vehicles

#### Prior work — public-network studies, private deployments, and MEC trials

- Commercial-network studies have examined vehicle data upload, compression,
  encryption, application quality, and teleoperation. The UMN study
  *Teleoperating Autonomous Vehicles over Commercial 5G Networks: Are We There
  Yet?* is a particularly close baseline: it evaluates real camera and LiDAR
  streams, uplink behavior, channel and resource changes, handovers,
  RTSP/WebRTC, per-frame outcomes, compression, and teleoperation QoE.
- 5G/MEC projects and field trials evaluate cooperative perception, remote
  driving, predictive QoS, digital twins, edge placement, handover, roaming,
  and cross-border operation. TARGET-X reports real automotive 5G/MEC trials
  and highlights the tension between uplink-heavy automotive workloads and
  downlink-oriented TDD resources.
- Recent private-network work includes a large 5G SA C-ITS corridor with PC5
  and Uu support and several controlled C-ITS scenarios. These studies make it
  clear that Edge4AV is not the first real-vehicle 5G experiment, private-5G
  automotive deployment, uplink study, or use of PC5 and Uu in one project.
- The NSF BREAKING LOW program and the ongoing DRIVE-SAFE project identify
  consistent application-level latency and vertical-aware 5G/Next-G design as
  important open problems. These project materials establish motivation and
  research direction, while their completed technical publications—not the
  announcements themselves—must provide performance evidence.

#### Remaining gap — existing 5G studies are deep but application- or deployment-specific

The UMN work deeply answers whether continuous teleoperation streams can
operate over commercial 5G. Other trials answer questions about a particular
C-ITS scenario, MEC placement, corridor deployment, handover, or predictive
QoS. Across this literature, the remaining question is how a broader set of
conventional and stateful application exchanges behaves when payload,
direction, transport, signal condition, background uplink traffic, concurrency,
deadline, QoS evidence, and service interruption are varied systematically, and
how a compact deadline-sensitive foreground exchange changes while a sustained
uplink flow is present.
The reviewed work does not combine that matrix with one common interface,
complete-response accounting, realistic CAV object-size classes, a locally
terminated private-5G path, and a companion direct-PC5 characterization.

#### How this paper fills the gap — the second side of the field evaluation

The private-5G portion evaluates compact state, IPI operation exchanges,
detector-sized responses, and larger chunks over a real autonomous vehicle's NR
Uu connection to a local edge service. It varies transport, payload, direction,
signal, offered uplink load, application-client demand, application deadline,
QoS condition, and endpoint or broker interruption. It reports the percentage
of issued requests that receive responses and
tail RTT among completed responses rather than treating connectivity, peak
throughput, or median latency as proof of application support. Because endpoint
clocks were not synchronized, the paper uses RTT rather than one-way delay and
scopes its conclusions to the measured deployment.

#### Core sources

- [Teleoperating Autonomous Vehicles over Commercial 5G Networks: Are We
  There Yet?](https://arxiv.org/abs/2507.20438)
- [UMN autonomous-vehicle networking publication
  list](https://networking.umn.edu/research-areas/autonomous-vehicles)
- [Multi-Modal Vehicle Data Delivery via Commercial 5G Mobile Networks: An
  Initial Study](https://doi.org/10.1109/ICDCSW60045.2023.00026)
- [TARGET-X real-world automotive
  trials](https://doi.org/10.3390/fi18040189)
- [Field Deployment and Performance Evaluation of an NR-V2X C-ITS Test
  Corridor over a 5G SA Private
  Network](https://doi.org/10.3390/electronics15122668)
- [Realistic Field Trial Evaluation of a Tele-operated Support Service for
  Remote Driving over 5G](https://doi.org/10.1109/CSCN57023.2022.10051034)
- [NSF BREAKING LOW program](https://www.nsf.gov/funding/opportunities/breaking-low-ideas-lab-breaking-low-latency-barrier-verticals-next-g/506224/nsf24-545)
- [UMN DRIVE-SAFE announcement](https://cse.umn.edu/cs/news/zhi-li-zhang-leads-7m-nsf-breaking-low-project-improving-5g-networks-enable-av)
- [DRIVE-SAFE project](https://nsf-breaking-low-drive-safe.net/)

### 2.5 Multi-radio communication and application/network co-design

#### Prior work — selecting, combining, and adapting communication paths

- Multi-radio CAV architectures have long proposed assigning different
  services to different radio access technologies. AutowareV2X demonstrates
  redundant message delivery and freshness-based selection across radio
  channels.
- Tentacles selects network interfaces according to application QoS, combines
  redundant deliveries, changes the transmitted representation, and falls back
  to local computation.
- 5GAA studies, recent RAT-selection work, and broader communication,
  computation, and control co-design frameworks consider how PC5, Uu, and
  computing resources should be selected or combined according to application
  requirements and network state.
- DRIVE-SAFE similarly proposes vertical-aware 5G/Next-G optimization and
  integration of cellular communication with C-V2X. It should be described as
  an ongoing direction unless a completed technical result is cited.

#### Remaining gap — adaptive designs need common application state and measured decision inputs

Prior work establishes the value of multiple paths and adaptive communication.
The remaining gap is not another abstract path-selection algorithm. Adaptive
systems need a consistent way to know what operation is in progress, what
constitutes its complete result, when that result expires, and what failure
means. They also need empirical evidence about which variables materially
change application outcomes on the available paths. Without this information,
a selector can optimize packet- or link-level metrics while still missing the
application deadline or delivering an incomplete result.

#### How this paper fills the gap — inputs and guidance, not a new selector

IPI exposes application identity, state, correlation, freshness, status, and
failure semantics. The evaluation identifies the operating variables that a
future adaptive design should observe: complete-response availability,
deadline, payload representation, communication direction, signal, load,
concurrency, transport behavior, QoS enforcement, and service availability.
The paper does not implement a combined PC5/Uu selector and does not claim to
invent hybrid communication. Its third insight converts the interface and
measurement findings into concrete requirements for future CAV,
5G-Advanced/6G, carrier, and radio-system designs.

#### Core sources

- [Multi-Radio 5G Architecture for Connected and Autonomous
  Vehicles](https://doi.org/10.4108/eai.20-3-2018.154368)
- [Tentacles](https://weisongshi.org/papers/wu24-Tentacles.pdf)
- [AutowareV2X](https://tus.elsevierpure.com/en/publications/autowarev2x-reliable-v2x-communication-and-collective-perception-/)
- [5GAA Study on Dynamic V2X Use
  Cases](https://5gaa.org/study-on-dynamic-v2x-use-cases/)
- [Multi-Agent Reinforcement Learning for C-V2X RAT
  Selection](https://arxiv.org/abs/2607.11744)
- [4C: A Computation, Communication, and Control Co-Design Framework for
  CAVs](https://arxiv.org/abs/2107.01142)
- [DRIVE-SAFE](https://nsf-breaking-low-drive-safe.net/)

### 2.6 Synthesis: the gap this paper fills

The Related Work section should end with one direct synthesis paragraph built
from the following logic:

1. Standards and platforms provide interoperable messages, network enablers,
   and important application integrations. Across the surveyed implementations,
   compact J2735-compatible CV/ITS messages and correlated CAV operations remain
   separate application representations.
2. CAV systems and network middleware demonstrate application-specific
   cooperation, deadlines, adaptation, and multi-network operation, but do not
   close that protocol gap.
3. PC5 and 5G studies provide substantial real-world evidence, but each
   primarily examines one path, application family, protocol adaptation, or
   deployment question. They do not apply both workload classes to both path
   types and determine where each measured path stops meeting its application
   requirements.
4. Edge4AV fills the primary evidence gap with a systematic application-level
   field evaluation across independently measured LTE C-V2X PC5 and private-5G
   NR Uu paths. IPI supplies the enabling application contract used to represent
   both workload classes.
5. The resulting three insights state that the measured commercial direct V2X
   path supports compact J2735 messages only within its payload-and-coverage
   envelope; the measured 5G uplink supports only severely limited CAV packet
   sizes within decision deadlines; and 5G/6G radios
   must support event-triggered CAV bursts in either direction and isolate
   concurrent flows.

#### Candidate closing paragraph

> Prior work has standardized V2X messages, integrated selected cooperative
> applications, developed deadline-aware and multi-network middleware, and
> measured direct PC5 and cellular 5G in increasingly realistic settings.
> However, these efforts leave two connected gaps: the surveyed implementations
> do not expose compact CV/ITS messages and correlated CAV operations through
> one application contract, and existing evaluations do not apply both workload classes to
> both path types to determine where each path stops meeting application
> deadlines.
> This paper fills the primary evidence gap with a real-vehicle,
> application-level evaluation of a
> certified LTE C-V2X PC5 path and an on-site private-5G NR Uu path. Together,
> IPI supplies the common workload contract that connects application meaning
> to measured communication outcomes. The measurements then expose the
> requirements that future CAV and 5G-Advanced/6G systems must
> address.

## 3. IPI Protocol Design

### Section purpose and argument

This section presents the enabling systems contribution: an application-layer
protocol that provides one logical contract for compact CV/ITS messages and
correlated CAV operations. The protocol exists because the primary measurement
study requires consistent workload meaning across the measured paths. It must
answer four questions before the evaluation begins:

1. What application-level information is missing when an implementation only
   exposes a radio link, IP socket, or individual J2735 message?
2. How does IPI preserve conventional J2735-based exchanges while adding the
   state, correlation, completion, and failure semantics required by complex
   CAV services?
3. Which parts of the contract are independent of PC5, Uu, TCP, MQTT, or UDP,
   and which behavior remains the responsibility of a transport or deployment?
4. Which protocol elements are implemented and validated in the reference
   library, and which remain design hooks rather than experimentally validated
   mechanisms?

The section must establish IPI as a protocol/interface contribution rather than
an arbitrary experiment header, but it must not turn the manuscript into a
standalone protocol paper. It should be detailed enough for an ITS or CAV
developer to understand the represented workloads and for a networking reviewer
to identify the application identity, correlation, outcome, freshness, binding,
and measured size boundary.

The main manuscript should answer those questions in approximately one page.
The detailed field inventory, full lifecycle rules, byte layouts, versioning,
error behavior, and golden vectors belong in the appendix or repository
artifact. A full autonomous-driving application, protocol-only campaign, or
performance comparison against TCP or MQTT is not required for this enabling
contribution.

### 3.1 Design goals and non-goals

#### Design goals

- **One interface across ITS, CV, and CAV applications.** A traffic signal,
  roadside unit, connected vehicle, automated vehicle, pedestrian device, edge
  application, or cloud application should use the same logical application
  contract even when its payload, serialized profile, and communication path
  differ.
- **Preserve deployed message ecosystems.** IPI must carry the J2735 messages
  already used for safety, signal, and intersection applications rather than
  requiring those applications to adopt a new incompatible message family.
- **Add stateful-service semantics.** Complex CAV applications need more than a
  broadcast object: they need service identity, request correlation, session
  state, progress, completion or rejection, freshness/expiration, and explicit
  failure handling.
- **Separate application meaning from transport.** The application object and
  its completion criterion should not change merely because a path-specific
  profile is carried over direct C-V2X, 5G, TCP, MQTT, UDP, a wired link, or a
  future transport.
- **Make readiness measurable.** Message identifiers, source and intersection
  identity, send time, session/correlation identifiers, status, payload type,
  and acknowledgements must let an evaluator distinguish a completed response,
  a late response, a rejection, a validation failure, and no response.
- **Expose application intent at the correct layer.** The common interface
  should let a service profile describe direction, deadline, nominal rate,
  periodic/burst/stream behavior, delivery scope, and expected session
  duration so a deployment can make informed path, admission, or scheduling
  decisions. The current reference types expose only part of this intent through
  transport, priority, requested horizon, expiration, and session state. Treat a
  fuller traffic descriptor as an explicit protocol-extension requirement, not
  as an already implemented or network-enforced reservation mechanism.
- **Permit evolution.** Typed messages should coexist with opaque pre-encoded
  J2735 payloads and service-specific data so applications can be added without
  breaking the common interface.

#### Non-goals and claim boundary

- IPI is not a new radio, routing protocol, scheduler, perception model,
  planner, controller, or edge-computing platform.
- IPI does not replace SAE J2735, 3GPP V2X, ETSI MEC, or an operator's QoS and
  security mechanisms. It supplies the application contract that those
  systems can carry.
- The current IPI encoding is a research protocol and reference implementation;
  it is not an SAE-assigned message standard. “J2735-compatible” must mean that
  IPI preserves typed or opaque J2735 message content and provides a regional
  cooperative-service extension path, not that SAE has standardized IPI.
- A transport annotation or priority field does not reserve radio resources,
  create a QoS flow, authenticate a participant, or guarantee a deadline.
  Those actions require a corresponding deployment mechanism and evidence.

#### Candidate opening paragraph

> Existing vehicular message standards define important application objects,
> while network and edge platforms provide ways to move those objects. Future
> CAV applications, however, also need a common contract for who requested a
> service, which response belongs to that request, whether the result remains
> fresh, whether the service is still in progress, and whether it completed or
> failed. IPI provides this contract without discarding deployed J2735
> messages. It gives conventional CV/ITS exchanges and correlated CAV operations
> shared application identity, content typing, timing, association, and outcome
> semantics represented through workload-specific profiles and carried through
> path-specific bindings over direct V2X or network-assisted communication.

### 3.2 One interface, two application interaction modes

IPI supports two interaction modes through the same high-level interface.
These modes should be presented as complementary application patterns rather
than separate systems.

#### Conventional message interaction

- A producer ingests or requests broadcast of a typed J2735 object; a consumer
  queries or subscribes for that message type.
- This mode supports periodic or event-driven BSM, PSM, MAP, SPaT, SRM, SSM,
  and opaque pre-encoded messages such as a full TIM not modeled by the
  lightweight reference types.
- The logical application contract associates source, intersection, time,
  transport, and optional correlation metadata with the J2735 payload. The
  compact serialized message profile does not have to reproduce every optional
  operation field.
- The primary completion question is whether the expected message or response
  was received while it was still useful. Not every conventional broadcast
  requires a session.

#### Stateful service interaction

- A vehicle registers its profile and requested services, receives a session
  descriptor, keeps the session active with heartbeats, invokes a named
  service, and receives correlated status or guidance updates.
- This mode supports perception, planning, control, computation, map-update,
  lane-keeping, and unprotected-left-turn assistance.
- A service can progress from a request to one or more updates and then to
  completion or rejection. Telemetry can be associated with the same session.
- The application completion question includes response identity, terminal
  status, expiration, and deadline—not merely receipt of one packet.

#### Protocol architecture figure

Use a compact left-to-right figure with three regions:

1. **Applications:** conventional ITS/CV applications above and stateful CAV
   services below;
2. **IPI contract:** one logical application contract in the center, with a
   compact J2735 message profile and a correlated-operation profile inside it;
3. **Bindings:** direct PC5 hardware, TCP/MQTT/UDP over Uu, ROS 2 adapters, and
   future transports on the right.

Show that the two modes share identity, time, source, context, content typing,
and optional operation association at the application-contract level. Label the
serialized forms as profile specific so the figure does not imply that every
field appears in every compact wire frame. Do not draw IPI as replacing J2735 or
as a network layer. Do not imply that every listed binding is evaluated in this
paper.

### 3.3 Logical application contract, identity, and correlation

#### Envelope metadata

The high-level IPI data model can associate the following metadata with an
application object. A serialized binding profile selects the fields needed by
its exchange:

- `messageId`: unique application-message identifier;
- `sentAt`: application send timestamp;
- `intersectionId`: logical intersection or service domain;
- `transport`: the selected communication-path annotation;
- `source.type` and `source.id`: producer role and identity;
- optional `sessionId`: stateful interaction scope;
- optional `correlationId`: request/response association; and
- optional `priority`: application-provided priority label.

The reference façade creates a message identifier and send time when the caller
omits them. When an `IpiServiceRequest` does not provide an explicit
correlation identifier, the façade derives it from the request identifier.
State clearly that identifier generation in the reference implementation is a
prototype policy, not a globally distributed uniqueness service.

#### Payload descriptor

- A J2735 payload descriptor records message type, encoding, encoded bytes,
  and an optional frame counter.
- Supported encoding labels are UPER, raw bytes, and JSON, but the reference
  implementation currently implements UPER/profile encoding and canonical byte
  encoding; JSON is declared but rejected as unimplemented.
- The payload type is checked during typed unpacking. Empty opaque J2735
  payloads, mismatched types, malformed lengths, truncated sections, and field
  values outside their allowed ranges are rejected.
- Pre-encoded opaque payload ingest/list/broadcast is the compatibility escape
  hatch for standard or regional messages not yet represented by a lightweight
  C++ type. The bytes and declared message type are preserved.

#### Field table for the manuscript

Use one compact protocol table with these columns:

- field group;
- required fields;
- optional fields;
- semantic purpose; and
- validation or failure behavior.

Rows should cover envelope metadata, J2735 payload descriptor, service request,
cooperative response, session descriptor, telemetry, and acknowledgement. Put
implementation-specific byte widths and cardinality limits in the table or a
reproduction appendix rather than scattering them through prose.

### 3.4 J2735-compatible CV and ITS message path

#### Supported message families

- The typed reference path supports BSM, PSM, MAP, SPaT, SRM, and SSM. TIM is
  represented in the message-type vocabulary and can use the opaque
  pre-encoded payload path.
- The high-level interface exposes the same operations across these types:
  ingest a received message, request broadcast to a selected target, and list
  decoded messages by type and time.
- The optional ROS 2 bridge converts the corresponding `v2x_msg` forms for BSM,
  PSM, MAP, SPaT, SRM, and SSM into the reference types. The Mocar integration
  binds selected messages to the deployed PC5 device API.
- The standard-message path and the IPI operation path share the same
  application API, identity model, and content conventions while using
  profile-specific serialized fields. This is the concrete basis for calling
  IPI one contract rather than two unrelated APIs.

#### Conventional application examples

- **Signal state:** infrastructure publishes MAP/SPaT, and a vehicle obtains
  the current geometry and phase/timing state through the common interface.
- **Signal service:** a vehicle submits an SRM and receives a correlated SSM;
  source, intersection, request identity, and time remain visible to the
  application.
- **Road-user warning:** a BSM or PSM supplies participant state, and the
  warning/acknowledgement interface records warning identity, severity,
  geofence, handling time, and acknowledgement outcome.

These examples demonstrate interface coverage. Do not claim that every example
was evaluated end to end over both radios. The field evaluation separately
identifies which standard messages and custom service objects were exercised.

#### Encoding boundary

- The reference `UperCodec` implements the project's lightweight profiles for
  the modeled J2735 types; it is not a replacement for a complete production
  SAE ASN.1 stack.
- Production-path claims come from the Mocar J2735-2020 SDK and standard-message
  callbacks. Reference-code round trips establish internal field preservation,
  type checks, and validation behavior.
- The manuscript should therefore distinguish **standard device-path
  validation** from **reference protocol serialization validation**.

### 3.5 Stateful CAV service messages and lifecycle

#### Service request

`IpiServiceRequest` names the requested capability and carries the minimum
information required to correlate and bound the request:

- `serviceType`: lane-keeping aid, unprotected-left availability, perception
  aid, planning aid, control aid, computation aid, or HD-map update;
- `requestId`: 16-bit request identifier;
- optional `desiredHorizonMs`: requested planning/service horizon; and
- `additionalData`: service-specific opaque context.

The high-level interface maps lane-keeping and unprotected-left assistance to
either CV or CAV contracts according to the registered vehicle role. Perception,
planning, control, computation, and HD-map services require a CAV role in the
current implementation. The service envelope can additionally carry vehicle
identity, optional VIN, location, speed, heading, and application context.

For fault-triggered remote recovery, use `planning aid` as the requested
capability, carry vehicle pose/kinematics in the envelope, and place the
implementation-specific fault and remaining-capability report in
`additionalData`. Be explicit that the current profile does not yet define a
typed fault-diagnostic schema; this opaque context is the extension point until
such a common recovery profile is standardized.

The canonical request encoding is version-local and length checked:
`serviceType | requestId | flags | optional horizon | data length | data`.
The current implementation limits the request horizon to 10,000 ms and the
additional-data field to 65,535 B. Present these as current protocol-profile
limits, not universal limits on the underlying network.

#### Cooperative service message

`IPI-CooperativeService` represents the state and content of an ongoing CAV
operation:

- a 16-byte session identifier and 1--16 B vehicle identifier;
- operation class (`service_class` in the implementation): guided planning,
  guided perception, or guided control;
- guidance status: request, update, complete, or reject;
- optional requested horizon, confidence, and expiration time;
- typed planning, perception, or control content; and
- optional offload task identifier and opaque offload payload.

The typed content makes the application meaning explicit:

- planning carries ordered waypoints, optional target speed and dwell time,
  and a fallback-route flag;
- perception carries object identity, class, position, optional velocity, and
  optional covariance bytes; and
- control carries bounded steering, throttle, or brake commands.

The current profile validates 1--50 planning waypoints, at most 64 detected
objects, 1--10 control commands, confidence from 0 to 100, a cooperative
horizon no greater than 60,000 ms, and at most 65,535 B for each offload task
identifier or payload. Put the complete bounds in a protocol table or appendix.

#### CAV operation state and completion

Use a state/sequence figure showing:

1. vehicle request with `requestId`, `sessionId`, desired horizon, and context;
2. accepted or in-progress acknowledgement;
3. zero or more cooperative `Update` messages carrying partial guidance;
4. terminal `Complete` with the result or `Reject` with explicit failure; and
5. application timeout/expiration when no valid terminal response arrives in
   time.

The API-level response vocabulary—accepted, rejected, in progress, and
completed—describes operation handling. The cooperative-message vocabulary—
request, update, complete, and reject—describes the guidance exchange. Explain
the mapping instead of presenting the two enumerations as unrelated states.
A network acknowledgement only confirms acceptance at that interface; it is
not equivalent to application completion.

### 3.6 Session management and transport binding

#### Session contract

- Registration supplies an envelope, vehicle profile, requested services,
  optional RSU fallback identifier, optional minimum sidelink RSSI, and an
  optional event subscription.
- The returned descriptor contains the session identifier, vehicle profile,
  transport annotation, session state, lease duration, heartbeat interval,
  preferred channels, and granted services.
- Defined session states are registered, active, suspended, and terminated.
  Heartbeats can carry current vehicle telemetry; patch and termination
  operations update or close the session.
- The in-memory reference currently creates an active session with a 30 s
  lease and a 5 s heartbeat interval, accepts heartbeats for known sessions,
  and supports explicit termination. It does not yet enforce automatic lease
  expiry or a complete production recovery policy. Do not claim otherwise.

#### Topic and routing contract

The private-session binding defines stable topics for registration, events,
heartbeat, service request, service update, telemetry, and CV response. The
topic hierarchy is scoped by intersection and then by session or vehicle, for
example:

- `ipi/{intersection}/session/register`;
- `ipi/{intersection}/session/{session}/heartbeat`;
- `ipi/{intersection}/session/{session}/service/request`;
- `ipi/{intersection}/session/{session}/service/update`;
- `ipi/{intersection}/session/{session}/telemetry`; and
- `ipi/{intersection}/pcv/{vehicle}/response`.

The topic syntax supplies deterministic routing and observability. MQTT is one
binding used by the evaluation; the topic model is not a claim that IPI
requires MQTT.

#### Transport independence at the correct strength

- The common type system records a transport choice without placing transport-
  specific fields inside the application payload.
- The reference library demonstrates typed J2735 ingest/broadcast, an in-memory
  session transport, TCP/MQTT/UDP experiment framing, a ROS 2 conversion layer,
  and Mocar-backed examples.
- Transport labels also include future or deployment-specific choices such as
  wired backhaul, BLE, and HTTP backhaul. Their presence in the type system is
  an extension point, not evidence that every adapter is production complete.
- Reliability, ordering, congestion behavior, maximum datagram size, security,
  and QoS remain properties of the selected binding. IPI preserves the
  application-level completion criterion across those differences.

### 3.7 Freshness, failure semantics, and fallback hooks

#### Freshness and deadline semantics

- `sentAt`, requested horizon, optional expiration, correlation, and terminal
  status let an application determine whether the right result arrived while
  it was still useful.
- A successful application exchange requires a matching accepted response and,
  when the application defines a deadline, completion before that deadline.
- A harness timeout is an observation cutoff, not the application's semantic
  deadline. The evaluation applies this distinction explicitly.

#### Failure categories

The protocol and evaluation should keep the following outcomes separate:

- local validation rejection before transmission;
- transport or connection failure;
- unknown or terminated session;
- explicit service rejection;
- missing or uncorrelated response;
- response after expiration or application deadline;
- malformed, truncated, or mismatched payload; and
- receiver or broker interruption followed by recovery.

Use the acknowledgement's accepted flag and detail identifier to expose
immediate interface errors. Use service status and correlation for application
outcomes. A timeout must remain a timeout rather than being silently retried
and counted as an original success.

#### Fallback scope

- Registration exposes an RSU fallback identifier and a minimum sidelink-signal
  hook; the repository also contains local mesh and task-offload helpers that
  reuse `IPI-CooperativeService` messages.
- These hooks show how IPI can preserve the service object when a deployment
  changes path. The field experiments do not execute an automatic PC5/Uu
  selector or alternate-radio fallback, so fallback remains protocol support
  and implementation groundwork rather than an evaluated result.
- Authentication, authorization, confidentiality, replay protection, and
  safety-policy enforcement are deployment responsibilities not validated by
  the present reference implementation. Do not imply that the optional
  subscription secret or a trusted test VLAN constitutes a complete security
  design.

### 3.8 Reference implementation and validation

#### Implementation layers

- **Core model:** `IpiServiceRequest`, `IPI-CooperativeService`, message frame,
  field validation, and canonical encoding/decoding.
- **V2X integration:** lightweight J2735 models, profile codec, opaque payload
  path, ROS 2 conversions, and Mocar device examples.
- **Application API:** common sender/receiver interfaces, typed high-level
  façade, vehicle/session registration, heartbeat, service invocation,
  telemetry, response retrieval, warnings, intersection exchange, and cloud-
  export hooks.
- **Transport and experiment adapters:** private-session topics, TCP/MQTT/UDP
  request/reply framing, correlated acknowledgements, and experiment logging.
- **Optional continuity helpers:** local neighbor/mesh management and
  cooperative task-offload state using the same service message.

#### Validation evidence to report

- Round-trip tests cover modeled J2735 messages, PSM, cooperative perception
  content, canonical service requests, session topics, session registration,
  heartbeat, telemetry, request correlation, response retrieval, malformed
  input rejection, and private-5G probe framing.
- The high-level dual-mode test ingests a typed BSM, registers a CAV session,
  sends a heartbeat, submits a planning request, sends telemetry, and retrieves
  the correlated session response through one façade.
- The optional ROS 2 bridge builds for BSM, PSM, MAP, SPaT, SRM, and SSM. The
  Mocar path validates standard SPaT/BSM device callbacks separately from the
  reference serializer.
- Field experiments carry `IPI-CooperativeService` objects and correlated
  acknowledgements over the private-5G transports; dataset and detector output
  sizes populate the service payloads used in the readiness evaluation.

#### Validation claim boundary

Unit and loopback tests establish type, field, state, correlation, and encoding
behavior in the reference implementation. They do not establish interoperability
with every vendor stack, production security, network-enforced QoS, automatic
fallback, or deadline satisfaction. Those properties require their own
deployment evidence.

#### Section close

> IPI makes conventional messages and correlated CAV operations comparable at
> the application level through shared identity, context, timing, content
> typing, and operation association. Section 4 next maps the profile used by each
> experiment to deployable direct-PC5 and 5G-Uu paths and defines the measured
> size and timing boundaries.

## 4. System Design and Experimental Setup

### Section purpose and argument

This section must let a networking, radio, ITS, CV, or CAV reader determine
exactly what was tested, why these communication paths were selected, what was
varied, what was held constant, and what the resulting measurements can and
cannot establish. It should be detailed enough that a signal or cellular
reviewer cannot reasonably dismiss the evaluation because the carrier,
bandwidth, power, antenna, coverage, CPE placement, core boundary, traffic
direction, or radio-observation method is unclear.

The section must continue the logic established by the Introduction, Related
Work, and the IPI design in Section 3:

1. The Introduction asks whether both direct V2X and 5G can carry compact CV
   and larger CAV application exchanges within their time budgets, and where
   each path stops doing so as workload and operating conditions change.
2. Related Work establishes that current C-V2X architectures and the
   automotive literature repeatedly use two complementary modes: direct PC5
   communication for local V2V/V2I/V2P exchange, and Uu communication through
   a cellular network for vehicle-to-network or vehicle-to-edge services.
3. Section 3 defines the common IPI protocol used to represent conventional
   messages and complete, stateful CAV operation exchanges across either mode.
4. This section selects an off-the-shelf implementation of each mode, defines
   their end-to-end boundaries, and constructs an experiment matrix that
   applies both workload classes to both path types.
5. The Results section will report the observations and derive the three
   insights. This section must not state the insights, organize experiments as
   proofs of predetermined insights, or foreshadow the experimental outcome.

This is a system and evaluation-design section, not a testbed-construction
contribution. Hardware and configuration details establish the operating
regime and the scope of the evidence; they are not presented as a new testbed.

### 4.1 Why we evaluate LTE C-V2X PC5 and 5G NR Uu

#### Transition from Related Work

- Open with the connection the reader should already see from Section 2:
  3GPP's V2X architecture defines V2X communication over the PC5 reference
  point and over the Uu reference point as two modes that a vehicle may use
  independently. Industry descriptions similarly distinguish direct local
  communication from mobile-network communication.
- State their expected roles precisely:
  - PC5 supports direct local exchange between vehicles, roadside
    infrastructure, and other road users without requiring the application
    data to traverse a mobile core.
  - Uu carries vehicle traffic through the radio access and core networks to
    an application server or edge service and therefore exposes the complete
    uplink/downlink, scheduling, core-forwarding, and service path.
- Explain that these two modes correspond directly to the two application
  patterns introduced earlier: compact, local CV/ITS information and addressed
  vehicle-to-edge CAV services. This is a reason to test both, not a claim that
  each mode is restricted to only one application class.
- Use the accurate selection claim: the paper tests **the two principal and
  complementary access modes defined by current C-V2X architecture and widely
  proposed for vehicular deployments**. Do not call them the only ITS
  technologies or claim without evidence that they are universally the two
  most deployed technologies.

#### Why these concrete implementations

- Select the certified off-the-shelf LTE C-V2X PC5 OBU/RSU path because it is
  a commercially available implementation of the direct communication mode
  that vehicles and roadside infrastructure can deploy today.
- Select the on-site 5G NR Uu path because it contains the same component
  classes that an operator-supported vehicle-to-edge service uses: provisioned
  SIM, vehicle gateway/CPE, NR radio access, mobile core, local routing, and an
  edge endpoint.
- Describe the private deployment as **deployment-realistic and controllable**,
  not as an exact replica of every public 5G network. Its local termination
  keeps the application endpoint and its MX250 attachment controlled, while its
  isolated use makes the experiment-generated load explicit. Verify the
  private-core/user-plane route before claiming that every remote backhaul hop
  is absent.
- Explain the controlled approximation: offered uplink streams and 1--100
  logical application clients stress competing traffic, request concurrency,
  the common MG52/Uu/core path, and the edge service. Repeating the client sweep
  under weaker radio conditions is especially important because a weak link can
  deliver fewer useful bits per scheduled resource than a strong link. These
  tests do not reproduce a commercial network's population of independent UEs,
  scheduler policy, handover, roaming, or geographic backhaul.
- Explain why the measured direct path is LTE C-V2X PC5 rather than NR-V2X
  sidelink: the experiment uses available certified production equipment. NR
  PC5 is an important future or complementary technology discussed in Related
  Work, but it was not measured and must not inherit the results.

#### Scope of the comparison

- The paper does not perform a head-to-head throughput contest between PC5 and
  Uu. Their hardware, protocol stacks, roles, and tested payload envelopes are
  different.
- The paths are evaluated independently in the same road environment. No
  request travels outward over one path and returns over the other, and the
  evaluation does not implement a PC5/Uu selector, aggregation scheme, or
  fallback between radios.
- The study does not evaluate DSRC/ITS-G5, Wi-Fi, mmWave, satellite,
  commercial-public-network handover, or NR-V2X sidelink. These are outside the
  empirical scope, not judged ineffective.
- Conclusions must be scoped to the measured production LTE C-V2X device path
  and the measured private-5G NR Uu deployment. The paper can derive design
  requirements for future systems, but it cannot universalize one deployment's
  measured limits to every PC5 or 5G implementation.

#### Candidate opening paragraph

> The C-V2X architecture and the systems surveyed in Section 2 place two
> complementary communication paths at the center of connected and automated
> driving: direct PC5 communication for local V2V/V2I exchange, and Uu
> communication through a cellular network for vehicle-to-edge and
> vehicle-to-network services. We therefore evaluate one off-the-shelf
> implementation of each path: a certified LTE C-V2X PC5 OBU/RSU pair and an
> on-site 5G NR Uu deployment connecting an autonomous vehicle to a local edge
> service. These paths do not represent every vehicular radio. They provide
> concrete, currently deployable instances of the two communication modes most
> directly associated with today's C-V2X application claims.

#### Architecture sources for this selection

- [3GPP/ETSI TS 23.287, V2X architecture over PC5 and
  Uu](https://www.etsi.org/deliver/etsi_ts/123200_123299/123287/18.04.00_60/ts_123287v180400p.pdf)
- [5GAA explanation of direct PC5 and mobile-network Uu
  communication](https://5gaa.org/c-v2x-explained/)

### 4.2 End-to-end system and measurement boundary

#### System overview figure

Use one left-to-right architecture figure with the autonomous vehicle on the
left and the fixed infrastructure on the right. It should show two visibly
separate paths rather than one combined pipeline.

- **Inside the vehicle:** vehicle experiment computer, Mocar OBU, Cisco Meraki
  MG52-HW gateway/CPE, fixed gateway placement, and NovAtel OEM7 GNSS source.
- **Direct PC5 path:** vehicle OBU -> LTE C-V2X PC5 air interface -> fixed
  HUALI RSU -> correlated reply over the same air interface -> vehicle OBU.
- **5G NR Uu user-data path:** vehicle sender -> 1 Gbit/s full-duplex Ethernet
  -> MG52-HW -> Airspan AirSpeed 2900 n48 gNodeB -> Cisco private-5G
  core/edge functions -> local MX250 -> directly connected application edge
  host -> correlated response. Use this as a component-level boundary, not an
  unverified internal forwarding order.
- **Remote management context:** draw the user-identified remote NMS-side
  MX68CW separately with a dashed management/control-context link. Do not put
  it on the solid timed user-data path unless routing, capture, or administrator
  evidence shows that application packets traverse it.
- **Workload inputs:** J2735 messages, IPI service objects, public-dataset size
  distributions, and V2X-Radar detector outputs enter the sender as workload
  inputs. Draw them outside the two network paths so they are not mistaken for
  a third communication system.
- **Measurement markers:** place the start timer after request encoding and
  immediately before transmission, and stop it when the sender receives the
  correlated application response.
- **Boundary annotation:** label the application endpoint as local to the site
  and directly attached to the MX250. State separately that the precise
  private-5G core/user-plane route and the role of the remote NMS-side MX68CW
  must be supported by deployment evidence; remote management presence alone
  does not establish a remote application-data hop.

The figure should identify Mocar as an equipment/vendor name and Edge4AV as the
paper title. It must not label either as the proposed system. IPI is the common
application interface carried by the experiments.

#### Vehicle and endpoint roles

- State only evidenced vehicle details. The field runs use a real instrumented
  autonomous vehicle carrying the OBU, MG52-HW, experiment host, and GNSS
  source. Do not invent the vehicle make, autonomy stack, CPU/GPU, sensor suite,
  or antenna mounting details that are not present in the retained records.
- The autonomous vehicle provides the physical vehicle environment, mobility,
  gateway placement, and GNSS observations. The paper does not put a complete
  perception-planning-control loop in the timed path.
- The V2X-Radar detector runs offline on a separate four-GPU system; their
  output-size distribution is replayed by the vehicle-side communication
  sender. Detector execution time is therefore excluded from network RTT.
- For the primary PC5 sweeps, the OBU initiates a request to the fixed RSU and
  the RSU echoes a correlated reply. Initial validation also confirms that the
  custom path works in both directions.
- For 5G, the vehicle-side host sends an addressed request to a fixed edge
  responder. TCP and UDP responders run on the edge host. The MQTT condition
  uses an MQTT 3.1.1 QoS 0 broker and application receiver on the edge side.

#### What the measured RTT contains

- PC5 RTT contains sender processing around the vendor API, two traversals of
  the direct radio path, responder processing, and request/reply correlation.
- 5G RTT contains the vehicle host and Ethernet path, CPE processing, NR Uu
  scheduling and radio transmission, gNodeB and private-5G core/user-plane
  forwarding, local MX250/Ethernet forwarding, edge service or broker behavior,
  and the reverse response path. Include the remote MX68CW only if the final
  deployment evidence places it in that user-data path.
- The measurements do not decompose those aggregate RTTs into radio,
  scheduler, core, broker, or endpoint components. Configuration capability
  must not be used to assign latency to a component without matching telemetry.
- Most 5G endpoint clocks were unsynchronized, including a large clock offset
  in the follow-up. Sender-side RTT is therefore the timing metric. One-way
  uplink or downlink timestamps are diagnostic fields only and must not appear
  as valid one-way latency.

### 4.3 Field environment, geometry, and radio coverage

#### Physical setting

- Measurements take place around fixed roadside infrastructure in one road
  environment containing the Airspan gNodeB, the fixed V2X RSU, multiple
  stationary vehicle locations, repeated driving routes, and nearby buildings.
- Anonymize exact deployment coordinates in the paper while retaining a metric
  scale, road geometry, infrastructure markers, collection points, routes, and
  building footprints. Readers must still be able to judge distance,
  obstruction, and LOS/NLOS conditions.
- Record that 5G and V2X stationary collections do not necessarily use the same
  exact points. The figure must not imply paired radio observations where none
  exist.
- Weather labels and collection dates are deployment context, not controlled
  propagation experiments and not causal variables.

#### 5G coverage characterization

- Report the separate 16-point radio survey used to describe the deployment's
  coverage:
  - RSRP: -121 to -91 dBm, mean -106.88 dBm;
  - RSRQ: -19 to -10 dB, mean -11 dB;
  - reported SNR: 3.5 to 27.5 dB, mean 16.44 dB.
- Explain the signal-bin selection in one short paragraph. 3GPP defines
  SS-RSRP but not qualitative coverage levels. Aggregate the Australian 2026
  outdoor coverage-map levels into strong (at least -95 dBm), common/typical
  (-105 to below -95 dBm), and weak (below -105 dBm). State that the weak bin
  combines the source standard's Basic and No Coverage levels.
- Map the survey points and the five historical stationary run-location
  markers. State that the map interpolates a sparse field survey for spatial
  context; it is not a calibrated propagation model or a per-request channel
  trace.
- The survey and application runs are joined at run/location granularity. They
  do not provide contemporaneous per-request RSRP, RSRQ, SINR, CQI, MCS, BLER,
  HARQ/RLC retransmission, PRB use, scheduler state, queue state, or QFI.
- The more recent common/typical, candidate-weak, and candidate-strong labels
  must remain operator-reported until their matching MG52 and ACP exports are
  aligned. Do not convert the displayed unsigned values such as 101 or 109-110
  into signed dBm without the actual export and sign convention.
- Do not call a location a commercial **cell edge** unless the operator
  supplies a deployment-specific coverage boundary. Use the measured radio
  values and the neutral location labels instead.

#### PC5 coverage characterization

- The Mocar diagnostic interface did not expose usable C-V2X RSSI, RSRP, RSRQ,
  or SNR; the `diag-rssi` outputs were empty. Realtek Wi-Fi `RTW: rssi` logs are
  unrelated and must never be used as C-V2X radio measurements.
- The PC5 map is therefore a **normalized application-level path-availability
  proxy** derived from GNSS-joined request success, timeout, and successful-
  response RTT along the mobile routes. Do not label it measured RSSI or RSRP.
- Use the map to order the stationary points by normalized condition, then
  combine that ordering with recorded geometry. Point 1 is geographically
  close to the RSU but is NLOS because a building intersects the path; this is
  why distance alone is not used as signal strength.
- Separate the stationary payload sweeps from the mobile route samples used to
  build the proxy. The map supplies spatial context; it is not a per-request RF
  measurement for each stationary packet.

#### Coverage figure

Use a three-panel figure or one carefully layered field map:

1. measured 5G RSRP with gNodeB and run locations;
2. measured 5G SNR with the same geometry;
3. normalized PC5 application-level path availability with the RSU, mobile
   route, stationary points, and obstructing building footprints.

The caption must distinguish measured RF quantities from the normalized PC5
proxy. Include the sample count, measurement range, infrastructure markers,
and LOS/NLOS interpretation without publishing exact coordinates.

### 4.4 Direct LTE C-V2X PC5 path

#### Production equipment and standard-path validation

- Use a vehicle-side Mocar OBU and fixed HUALI RSU running the vendor LTE
  C-V2X PC5 stack and J2735-2020 SDK material.
- State the vendor-reported protocol-conformance status for the MOCAR OBU and
  HUALI RSU. Certification supports describing the devices as production,
  standards-oriented equipment; it does not certify this paper's application
  performance.
- Before latency experiments, validate the standard message path:
  - an IPI SPaT bridge forwards 10/10 encoded SPaT frames through the vendor
    SPaT API to the OBU receive callback;
  - BSM reception is observed in both directions between the OBU and RSU.
- This validation shows that the deployed radio path carries standard CV/ITS
  messages. It is a functional check, not a latency distribution or proof of
  reliability under every operating condition.

#### Correlated request/reply instrumentation

- The public `mde_v2x_custom_send()` symbol in the installed SDK build was a
  no-op. The working probe therefore uses the same deployed radio stack through
  `v2x_packet_data_send(payload, len, 0x1b)`.
- The packet-data probe adds a sequence number, request identifier, endpoint
  identifier, and echo reply so the OBU can distinguish a response from an
  unrelated broadcast and measure sender-side RTT.
- Initial tests validate 10/10 correlated replies in both RSU-to-OBU and
  OBU-to-RSU directions. The main stationary and mobile sweeps use the OBU as
  initiator and fixed RSU as responder.
- The custom packet-data probe is not a BSM, SPaT, or other standardized J2735
  frame. Describe it as a correlated measurement message sent through the same
  production PC5 stack after the standard path was validated.
- Static analysis of the installed packet-data SDK identifies a 4,080 B
  maximum application packet after vendor framing. This is an implementation-
  specific API limit, not a 3GPP PC5 or C-V2X standard limit. The evaluated
  sweep stops at 2,048 B, so the observed delivery behavior is not caused by
  submitting packets above this interface limit.

#### Radio information available and unavailable

- Report the radio mode, device roles, message path, payload limit, geometry,
  mobility, send rate, timeout, and application outcomes that the retained
  artifacts support.
- Before final manuscript submission, add any vendor export that establishes
  PC5 carrier/channel, channel bandwidth, resource pool or mode, transmit
  power, antenna gain and placement, MCS policy, congestion-control setting,
  firmware, and lower-layer retransmission behavior.
- If those fields cannot be recovered, state that they were not exposed or
  retained for this production device path. Do not infer them from generic
  product capabilities or from the 5G deployment.
- Make the evidence asymmetry explicit: the 5G path has a detailed exported RF
  configuration and a separate RF survey; the PC5 path has certified hardware,
  standard-message validation, geometry, GNSS, and application-level path
  outcomes but no usable per-packet received-power telemetry.

#### Device and certification source

- [HUALI/Mocar LTE-V2X protocol-conformance
  certification](https://www.huali-tec.com/news_page.php?cid=24&id=287)

### 4.5 Private-5G NR Uu path

#### 4.5.1 RAN, spectrum, and antenna deployment

- The radio access node is an outdoor Airspan AirSpeed 2900, product code
  `AS29-N48-DSC1`, integrating the xPU, CU-CP, CU-UP, DU, and RU functions.
- The retained software inventory records platform version `22.0-24-0.0` and
  application version `22.00-53-0.0`.
- Two enabled DU cells map one-to-one to two RU sector carriers. Both are
  configured for NR band n48 TDD in CBRS spectrum with:
  - 40 MHz channel bandwidth;
  - 30 kHz PDCCH/SSB subcarrier spacing;
  - 20 ms SSB periodicity;
  - at most one configured uplink layer and two downlink layers;
  - downlink 256-QAM enabled as a cell capability.
- Give the two retained channel records in a compact RF-configuration table:
  - Cell 1: NR-ARFCN 637992, SSB frequency 3558.720 MHz, mapped reference
    frequency 3569.880 MHz;
  - Cell 2: NR-ARFCN 645334, SSB frequency 3669.600 MHz, mapped reference
    frequency 3680.010 MHz.
- Distinguish configuration from use. Current evidence assigns the original
  experiment traffic to Cell 2, and the follow-up locks the MG52-HW to Cell 2;
  the final manuscript must preserve the pending management-export alignment
  caveat wherever serving-cell attribution matters. Cell 1 is not a second
  traffic path in the application experiments.
- The header-bearing CBSD export reports, for each cell:
  - CBSD Category B and GAA operation;
  - 40 MHz bandwidth;
  - 33 dBm cell transmit power;
  - 34 dBm/MHz cell EIRP;
  - zero PAL and four GAA 10 MHz assignments;
  - one requested and authorized grant, no suspended grant, and successful
    SAS heartbeat;
  - Federated Wireless as the enabled primary SAS provider.
- The integrated antenna configuration records 17 dBi gain and cross
  polarization. The Airspan equipment guide describes the integrated antenna
  as dual-slant with approximately 65-degree azimuth and 8-degree elevation
  beamwidth. Mark these as equipment/configuration characteristics, not
  measured beam coverage.
- The signed structural record places the outdoor unit at a 20 ft antenna
  centerline above roof level on a 21 ft tripod/mount structure. Installed
  height above ground, north-referenced azimuth, mechanical downtilt, electrical
  downtilt, and individual grant details remain unresolved. Do not substitute
  the propagation-study assumption of 20 ft AGL for the structural record.
- The gNodeB uses GNSS as its configured clock source. This does not synchronize
  the vehicle and edge application clocks and therefore does not authorize
  one-way application latency.
- The active configuration also records a 7 s UE inactivity timer, primary
  PLMN MCC/MNC 315/010, and an eMBB slice with SST 1 and SD `000002`. These
  fields belong in the reproducibility inventory; they are not performance
  explanations without bearer-level evidence.
- `Frequency Reuse One` and the CBRS measurement-capability field are disabled
  in the retained export. If space is limited, place these secondary settings,
  software versions, and KPI-collection configuration in the reproduction
  appendix rather than omitting them from the artifact record.
- The network was tightly controlled during the experiments: the CAV gateway
  was the only physical UE, the radio and its full 40-MHz channel were dedicated,
  and the n48 band was clean without ambient contention. Controlled offered uplink load and application clients are therefore the
  explicit competing workloads. Every application client is generated behind one
  vehicle host and one MG52 attachment, so the RAN still observes one UE.
- State why radio condition and client count are evaluated together. 3GPP NR
  modulation/coding tables span widely different spectral efficiencies, so a
  UE in a weaker condition can require more scheduled resources or recovery
  effort to deliver the same application bytes. The strong/weak multiclient
  sweeps test the resulting application-level interaction. Because matched
  PRB, MCS, BLER, HARQ/RLC, and per-UE scheduler records are unavailable, do
  not claim that the experiments directly measured how many radio resources a
  weak client consumed.

#### TDD, direction, and cell configuration context

- Every application campaign before the August 15--18 TDD study used the
  operator-reported `70/20/10` allocation. Earlier `40/40/20` labels for those
  campaigns are superseded.
- The study used `40/40/20` and `70/20/10`. The fields
  denote downlink, uplink, and dynamic frames. The former assigns 40% to fixed
  uplink use, while the latter assigns 20%.
- Both configurations retain `10D4G`, which the network administrator defines
  as frame packing for LTE coexistence. Report it for reproducibility, but do
  not treat it as an experimental variable.
- Treat `40/40/20`, `70/20/10`, and `10D4G` as deployment/operator shorthand,
  not self-interpreting 3GPP slot-pattern notation. The paper can report the
  administrator's downlink/uplink/dynamic interpretation, but it must not map
  the labels to full and partial DL/UL slots or flexible symbols without the
  Airspan schema that defines that mapping.
- A `30/60/10` label appears in an intermediate reconfiguration export, but the
  operator's attempt to use that allocation for the experiment was not
  supported as a usable configuration in this deployment. It produced no valid
  measurement. Present this as an unsupported configuration attempt and
  platform constraint, not an evaluated third condition or a failed
  application workload.
- Use the August 15--16 uplink-heavy runs only for deployment-repeatability
  analysis. Route recovery, cell administrative-state changes, a serving-cell
  change, and missing raw timestamps make those RTT observations unusable for
  profile ranking.
- Use the August 17 location-3 blocks for the matched profile comparison:
  8,000/8,000 accepted downlink-heavy exchanges per profile and ten exact
  50-MiB uploads and downloads per profile. Keep RTT, upload goodput, and
  download goodput separate.
- Use the August 18 MQTT 500-KiB, 40/40/20, -115-dBm pair for the matched
  application-direction comparison. The uplink-heavy p50/p95 is 61.5/41.0
  times the downlink-heavy p50/p95.

#### 4.5.2 Vehicle gateway and wired attachment

- The vehicle CPE is a Cisco Meraki `MG52-HW` with an internal antenna and
  Telit FN990A40 modem. It supports sub-6 GHz 5G SA/NSA and vendor-documented
  4x4 downlink MIMO; capability is not proof of the rank used in a run.
- In all original and current follow-up experiments, the gateway remains flat
  on the bottom of the vehicle trunk with its front face upward. Cisco
  documents a front-face bias for the internal antenna. Report this placement
  exactly; do not claim an external-roof antenna or an inside/outside placement
  comparison.
- The experiment-side Ethernet link negotiated 1,000 Mbit/s full duplex. This
  measured link state is more relevant than the gateway's advertised maximum
  2 Gbit/s downlink and 300 Mbit/s uplink passthrough rates, which are product
  capabilities rather than experiment throughput.
- Firmware matching the experiment campaigns is MG `4.1.2`, modem
  `M0R.115005`, host `A0R.501056`, carrier PRI `P0R.000566`, and carrier profile
  `Generic GCF`.
- The FN990A40 guide's conducted n48 receiver-sensitivity values are modem-lab
  specifications, not measured whole-gateway over-the-air sensitivity. Keep
  them in an appendix or omit them from the performance argument; do not use
  them to label a run weak or strong.

#### 4.5.3 Mobile core, session, and edge boundary

- Provisioned SIMs attach the MG52-HW to the Cisco private-5G core path. The
  experiment sessions use DNN `cisco5g` and the operator-confirmed default 5QI
  9 treatment.
- The numeric QFI and a verified nondefault QoS flow are not available. The
  application label `5qi-mapped` and an ACP 5QI-profile screen do not establish
  the bearer assigned to an experiment session.
- The active export enables RLC acknowledge-mode capability, while a separate
  5QI-properties view displays bidirectional RLC UM for profiles 1--9. Neither
  record proves the RLC mode of the measured bearer. Do not attribute loss or
  latency to RLC UM, HARQ, or retransmission behavior without bearer evidence.
- The application edge computer is directly connected to the on-site MX250.
  Cisco documents the MX250 as a security/SD-WAN routing, firewall, and VPN-
  concentration appliance. This establishes the local enterprise attachment
  and local application endpoint; it does not by itself reveal the internal
  private-5G user-plane route.
- The exact Cisco core topology, UPF identity, and internal delay decomposition
  are outside the retained evidence. Describe the path at the supported
  boundary rather than assigning unmeasured delay to individual core
  functions.
- The deployment also includes a remote NMS-side MX68CW. Cisco documents the
  MX68CW as a small-site security/SD-WAN appliance with routing, firewall,
  AutoVPN/IPsec, integrated LTE uplink, and Wi-Fi capabilities. It is one remote
  device, distinct from the vehicle MG52 and the local MX250. Do not call it the
  packet core or merge it with the local MX250 in the topology.
- Cisco's private-5G architecture places access/session and local user-plane
  functions on a private-5G edge appliance and management/subscriber functions
  in a control platform. Use this component-class description until the exact
  deployment-specific edge/core inventory is retained. Whether application
  packets traverse the remote MX68CW remains unresolved and must not be inferred
  from its presence in the management topology.
- Explain the relationship to public deployments carefully: this path uses
  real SIM, CPE, RAN, core, routing, and edge component classes, but its
  isolated radio attachment and controlled endpoint do not reproduce a public
  network's independent background UEs, handover, roaming, geographic backhaul,
  or operator scheduling and policy. The offered-load and client-demand sweeps
  are controlled proxies for competing traffic and aggregate application
  demand; they partially address load sensitivity without becoming a public-
  network emulation.

#### RF configuration table

Include one compact, academically formatted table whose columns are:

- deployment element;
- value used in the experiments;
- evidence type: active export, structural record, vendor specification,
  operator confirmation, or unresolved;
- interpretation boundary.

The table should cover band, bandwidth, cell/NR-ARFCN, TDD profile, transmit
power, EIRP, CBRS/SAS status, antenna gain/pattern and installation height, CPE
model/placement, negotiated Ethernet rate, DNN/default 5QI, isolation, and
telemetry granularity. It should not contain narrative results or turn vendor
maximums into measured performance.

#### Hardware and configuration sources

- [Cisco Meraki MG52/MG52E technical
  specifications](https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/Overviews_and_Datasheets/MG52%252F%252F52E_Technical_Specifications)
- [Cisco Meraki MG52 internal-antenna
  datasheet](https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/MG_Antenna_Datasheets/MG52_Internal_Antenna_Datasheet)
- [Cisco Meraki MX250
  datasheet](https://documentation.meraki.com/SASE_and_SD-WAN/MX/Product_Information/Overviews_and_Datasheets/MX250_Datasheet)
- [Cisco Meraki MX67/MX68/MX68CW
  datasheet](https://documentation.meraki.com/SASE_and_SD-WAN/MX/Product_Information/Overviews_and_Datasheets/MX67_and_MX68_Datasheet)
- [Cisco private-5G architecture and security
  description](https://www.cisco.com/c/en/us/products/collateral/wireless/cisco-private-5g-solution-wp.html)
- [3GPP/ETSI TS 38.214, NR modulation, coding, spectral efficiency, and
  transport-block/resource
  procedures](https://www.etsi.org/deliver/etsi_ts/138200_138299/138214/17.14.00_60/ts_138214v171400p.pdf)
- [Airspan AirSpeed 2900 FCC equipment and installation
  guide](https://fccid.io/PIDAS2900/User-Manual/User-Manual-5885368.pdf)

### 4.6 Application workloads and communication protocols

#### Compact CV and ITS traffic

- Standard BSM and SPaT traffic validates the production PC5/J2735 path.
- IPI stateless messages and `IPI-CooperativeService` requests supply compact
  request/reply traffic on the 5G path. Compact conditions include 0, 256,
  1,024, and 4,096 B application payloads, together with the fixed IPI envelope
  overhead recorded in the logs.
- Explain that these sizes cover empty/control messages, common compact
  state/service content, and the upper end of the compact sweep. Do not claim
  that an arbitrary byte count is itself a named J2735 message.

#### Detector-output workloads

- Use the official V2X-Radar radar-only late-fusion checkpoint over the full
  staged 922-sample cooperative validation split to obtain a real detector
  object-count distribution.
- Estimate the serialized result as a 256 B header plus 96 B per predicted
  object. The resulting distribution is:
  - minimum 13,216 B;
  - p50 19,648 B;
  - p95 22,816 B;
  - p99 23,968 B;
  - maximum 25,024 B.
- Replay the representative percentiles directly and include 60,000 B as an
  explicit upper stress condition. The 60,000 B value is not a percentile of
  the observed detector output and must be labeled separately.
- Detector computation time is measured separately and excluded from network
  RTT. The timed communication experiment begins with an already constructed
  payload, so it evaluates post-inference delivery rather than full
  sense-infer-communicate-act latency.

#### Image, point-cloud, and map scale

- The staged workload manifest contains 22,431 artifacts from completed
  OpenDAIR-V2X, V2X-Seq, TruckV2X, and V2X-Radar sources. The manifest supplies
  size distributions; it is not a count of wireless experiment requests.
- Report the observed application-object ranges that motivate the larger
  transfer conditions:
  - encoded images: 25.5--666.9 KiB, p50 334.1 KiB, p95 539.8 KiB, p99
    648.9 KiB;
  - point clouds: approximately 15 KiB--3.85 MiB, p50 937.8 KiB, with p95 and
    p99 at 3.85 MiB in the staged sample;
  - HD-map files: 22.3--44.9 MiB, p50 35.8 MiB, p95 44.0 MiB, p99
    44.9 MiB.
- The 5G payload sweep uses fixed 8, 16, 32, 64, 128, 256, and 512 KiB and 1
  and 2 MiB conditions in addition to compact sizes. A measured 128--512 KiB or
  1--2 MiB transfer is one transfer size or chunk, not proof that a full
  multi-megabyte point cloud or tens-of-megabytes map completed within the same
  RTT.
- OPV2V, V2XSet, V2V4Real, and V2X-Real remain source-registry entries without
  completed local benchmark results and should not be described as evaluated
  workload sources.

#### Supporting workload-validation experiments

- The dataset-specific local IPI loopback checks verify that the selected
  payload sizes can be constructed, serialized, validated, correlated, and
  accepted by the IPI sender/receiver without a wireless path. They isolate
  message-format failures from network failures; their loopback RTT is not a
  V2X or 5G result.
- The four-GPU artifact pass processes 224 staged image, point-cloud,
  annotation, and map files to validate data staging and GPU plumbing. It is
  not a detector-accuracy, wireless-performance, or end-to-end CAV experiment.
- The V2X-Radar environment check constructs the 922-sample cooperative
  validation split, and the final detector benchmark processes all 922 samples
  with the official checkpoint. These steps validate the provenance of the
  detector-output sizes used in the network replay.
- Present these checks as workload validation supporting the readiness
  experiment. Do not make them additional communication paths or primary
  network-performance experiment families.

#### Protocol conditions

- TCP uses an addressed request/response receiver.
- MQTT is MQTT 3.1.1 QoS 0 over TCP with an edge broker and application
  receiver. Treat it as an application messaging condition over TCP, not as a
  transport independent of TCP.
- UDP is evaluated in two distinct forms:
  - one raw UDP datagram per logical message, used to expose path-MTU/IP-
    fragmentation sensitivity;
  - adapter-level fragmentation into datagrams capped at 1,400 B, with
    receiver-side reassembly and a correlated acknowledgment.
- Never combine raw UDP and adapter-fragmented UDP conditions into one UDP
  reliability curve. They implement different delivery behavior.
- The vehicle-originated payload is primarily an uplink request or application
  object, and the correlated reply travels downlink. Separate direction-control
  traffic evaluates sustained vehicle-to-edge and edge-to-vehicle transfer;
  the request/reply RTT itself is not a one-way direction measurement.

### 4.7 Conditions and thresholds for application readiness

This subsection must make the paper's decision logic explicit **before** the
reader sees any performance outcome. Related Work identifies the communication
support assumed by future CV/CAV applications; Sections 3 and 4.1--4.6 define
the interface, measured paths, and representative workloads. The remaining
question is whether an application can obtain the complete information it
needs, within its time and reliability requirements, under the conditions in
which it must operate. The experiments in Section 4.8 are selected to test
those conditions.

#### Why there is no single universal CAV threshold

- A conventional signal-status request, a blind-spot warning, an
  infrastructure-guided intersection trajectory, a cooperative emergency
  maneuver, a fault-recovery session, and a detector or map transfer do not
  have the same payload, update pattern, direction, fan-out, session duration,
  deadline, or consequence of loss.
- Do not call one throughput, latency, packet-size, or reliability value *the*
  CAV requirement. Define each application class by:
  - the complete application object or state transition that must be delivered;
  - object size and update/message rate;
  - whether traffic is periodic, event-driven/bursty, sustained, or
    asynchronous/bulk;
  - traffic direction and whether the interaction is one-way, request/reply, or
    multi-step;
  - delivery scope: one-to-one, one-to-many/broadcast, or many-to-many;
  - whether content is common to many vehicles or vehicle-specific per vehicle;
  - session duration and the edge, backhaul, or operator capacity held while
    the service remains active;
  - maximum useful age or completion deadline;
  - required probability of complete delivery within that deadline;
  - geographic range and operating conditions over which that requirement must
    hold; and
  - behavior when the path, service, or deadline is unavailable.
- Standards requirements are necessary comparison points, not proof that a
  deployed network supports the application. In particular, 3GPP TS 22.186
  defines radio reliability for delivery of a specified number of bytes within
  a delay, whereas this paper measures a complete application-layer correlated
  response over both directions. The metrics are related but not identical.

#### Necessary conditions tested by this paper

An application is supported only when **all** applicable conditions hold:

1. **Semantic and functional correctness:** the receiver can interpret the
   message or application state, associate a response with the request/session, and
   reject stale or invalid state.
2. **Complete-object delivery:** the whole object or state transition—not one
   packet or one successfully received fragment—reaches the application.
3. **Deadline-qualified availability:** the complete, accepted, correlated
   response arrives within the application's useful time budget often enough
   to meet its required availability/reliability.
4. **Workload capacity:** the path sustains the required object size, update
   rate, and traffic direction without replacing application deadlines with a
   peak-rate claim.
5. **Operating-envelope continuity:** the preceding conditions remain true at
   the required distance and route locations and under the relevant
   obstruction, radio, offered-load, and concurrency conditions.
6. **Failure behavior:** interruption, late delivery, and recovery are exposed
   to the application so it can stop, reduce, cache, prefetch, or select another
   communication/computation mode safely.
7. **Mixed-service coexistence:** every deadline-critical exchange remains
   within its required envelope while the relevant periodic, bursty, bulk, and
   long-lived flows share the applicable PC5, Uu uplink, Uu downlink, transport,
   edge, backhaul, or operator resources. A path that passes only when one flow
   runs alone is not ready for the same application in the intended traffic mix.

These conditions are evaluated jointly. For example, a 30 ms median among
packets that arrived does not meet a 100 ms application requirement if 30% of
requests never return, and 100% eventual delivery does not support a 100 ms
service if most objects complete after that budget.

#### Concrete application thresholds: from vehicle motion to communication load

Do not open the evaluation with an abstract latency/reliability range. Select a
small number of recognizable CV and CAV applications and show where each
communication threshold comes from. For each example, use the same chain:

`vehicle speed and conflict geometry -> available time/distance -> information`
`and update rate -> communication budget -> measured experiment`.

The physical calculation makes the consequence of communication delay visible,
but it does not independently define the networking requirement. Cite the
application requirement for the deadline, frequency, payload, range, and
reliability; then translate that delay into distance traveled in the stated
scenario.

Use these equations for the worked examples:

- distance traveled while information is delayed: `d_delay = v * t_delay`;
- closing-distance change: `d_close = |v_1 - v_2| * t_delay` for same-direction
  vehicles;
- level-road stopping sight distance:
  `d_stop = v * t_reaction + v^2 / (2a)`; and
- application payload rate before protocol/security overhead:
  `R_payload = object_bytes * updates_per_second * 8`.

For the stopping-distance illustration, use the FHWA/AASHTO design assumptions
of a 2.5 s perception-reaction time and 3.4 m/s^2 (11.2 ft/s^2) deceleration.
State that this is a roadway-design/human-warning reference, not an automated-
braking model or a guarantee for every pavement, tire, grade, or driver.

##### Example A — CV traffic-signal violation/red-light warning at city speed

**Scenario.** A connected vehicle approaches a signalized intersection at
25 mph (11.18 m/s). The infrastructure supplies signal state/timing and stop-
line geometry so the vehicle can determine whether a warning is required.

**Physical window.** Under the FHWA design assumptions, the level-road stopping
sight distance at 25 mph is approximately 46.3 m (152 ft): about 27.9 m during
the 2.5 s perception/reaction interval and 18.4 m during braking. The historical
VSC traffic-signal-violation application calls for communication out to roughly
250 m, 10 updates/s, and no more than about 100 ms allowable latency. At 25 mph,
100 ms consumes 1.12 m of approach distance. If one message is generated every
100 ms and delivery itself can take 100 ms, the receiving application may act
on state almost 200 ms old—2.24 m of travel—before local processing and driver
response are counted.

**Communication object and load.** The preliminary VSC estimate is roughly a
500 B message at 10 Hz, or about 40 kbit/s of application payload before
J2735/security/transport overhead. The content includes signal phase and
timing, directionality, stop location/road shape, and optional surface/weather
context. In IPI, this maps to the J2735 SPaT/MAP path and compact signal-status
request/reply rather than a detector object.

**Traffic behavior and competition.** The base service is a compact periodic
infrastructure-to-vehicle publication whose content can be common to many
approaching vehicles; an optional SRM/query adds a much smaller vehicle-to-
infrastructure request. A broadcast or publish-once implementation scales
differently from sending one identical unicast copy per vehicle. On PC5 it
competes for RSU/OBU sidelink resources; on Uu it primarily consumes downlink
delivery opportunities, plus any uplink request and edge request-handling
capacity.

**Threshold used in this paper.** Functional J2735 decoding is mandatory. For
periodic one-way warning delivery, 100 ms is the cited reference. The measured
request/reply RTT is a stricter full-cycle comparison and cannot be relabeled as
one-way SPaT latency. PC5/5G response availability under 100 ms therefore tests
whether an acknowledged IPI query can fit inside the same numerical budget; it
does not certify the one-way warning application.

##### Example B — CV forward-collision and blind-spot/lane-change warning

Use two geometries because longitudinal stopping and lateral gap prediction do
not create the same physical constraint.

**Forward-collision warning.** At 25 mph, the same design stopping-distance
reference is approximately 46.3 m. The VSC cooperative-forward-collision-
warning requirement uses periodic one-way vehicle state at roughly 10 Hz,
approximately 100 ms allowable latency, and approximately 150 m range. The
communicated state is position, speed, acceleration, heading, and yaw rate. A
preliminary approximately 200 B message at 10 Hz is about 16 kbit/s per source
before overhead. A 100 ms communication delay corresponds to 1.12 m traveled;
generation-plus-delivery age approaching 200 ms corresponds to 2.24 m.

Do not claim that the 46.3 m stopping distance *produces* the 100 ms network
requirement. The stopping-distance calculation shows the safety margin consumed
by stale information; the 100 ms/10 Hz values come from the application-
communication requirement. If the manuscript uses the NHTSA FCW test-track
window instead, keep its quantities separate: at 50 km/h, 4.0 s and 2.1 s TTC
correspond to approximately 55.6 m and 29.2 m to a stationary target.

**Blind-spot/lane-change warning.** Use an illustrative city maneuver with the
subject vehicle at 25 mph and an adjacent vehicle at 35 mph, giving a 10 mph
(4.47 m/s) closing speed. VSC defines roughly 10 Hz updates, approximately
100 ms allowable latency, and approximately 150 m communication range for lane-
change/blind-spot warnings. In 100 ms, the subject travels 1.12 m, the adjacent
vehicle travels 1.56 m, and the longitudinal gap changes by 0.45 m. At the
maximum sampling-plus-delivery age of roughly 200 ms, the gap changes by about
0.89 m before local prediction and driver response. The application needs
position, heading, velocity, acceleration, turn-signal status, and for blind-
spot warning, vehicle dimensions.

**Threshold used in this paper.** Use the 256 B PC5 condition as the closest
measured compact-payload comparison and the mobile-route result for spatial
availability. The paper can test complete compact response, coverage, and a
full 100 ms request/reply cycle. It cannot claim one-way warning latency because
the direct-radio clocks/path instrumentation do not isolate it, and it cannot
claim driver-warning effectiveness because no human response was tested.

**Traffic behavior and competition.** These warnings are driven by periodic
state from many nearby vehicles, commonly around 10 Hz per source. On the direct
path, the vehicles form a many-to-many awareness workload that shares the PC5
sidelink pool; demand grows with the number of transmitting neighbors even
though each message is small. A network-assisted implementation would instead
turn the same application into many recurring Uu uplinks plus selective or
broadcast downlink warnings. The manuscript must name which realization it is
discussing rather than combining those resource domains.

##### Example C — CAV infrastructure-guided automated intersection crossing

**Scenario.** A CAV approaches a transformative intersection at 25 mph
(11.18 m/s). Unlike Example A, the infrastructure does not merely warn a human
driver. An intersection-edge service supplies the lane geometry, signal state,
and a planned velocity or trajectory that the automated-driving stack uses to
stop at the correct location or traverse the intersection. This is the
guided-planning service demonstrated conceptually by *A Vision for
Transformative Intersections* and carried explicitly by IPI.

**Application object and standard requirement.** The 5GAA Automated
Intersection Crossing use case defines the following application components:

- 300--450 B vehicle-awareness messages at up to 10 Hz;
- approximately 100 B of signal-phase/timing information at 1 Hz;
- approximately 1,000 B of intersection geometry within 1 s; and
- approximately 400 B per vehicle for intersection-manager data such as a
  trajectory or planned velocity.

The same use case gives a 10 ms service-level latency, 99.9999% reliability,
and 500 m range. At the 25 mph city speed used in this paper, 10 ms corresponds
to 0.112 m of vehicle travel. The important distinction from a conventional
SPaT warning is semantic as well as numerical: the CAV must receive the correct
plan for its vehicle, intersection, lane, time horizon, and session, not merely
receive any packet before the deadline.

**Traffic behavior and competition.** This is a continuing stateful service,
not one isolated trajectory packet. Every approaching CAV contributes recurring
uplink awareness and intent; SPaT/MAP is largely common downlink state; and the
intersection manager returns an vehicle-specific plan to each participating
vehicle. Multiple simultaneous arrivals therefore create recurring uplink
demand, downlink fan-out, correlated session state, and edge planning/service
work. Common intersection state may be broadcast or cached, but per-vehicle
plans cannot be amortized in the same way.

**Threshold used in this paper.** Use the J2735 SPaT/MAP functional path for
intersection state and the 256 B, 512 B, and 1 KiB PC5/5G conditions for the
compact state and trajectory-size comparison. An IPI guided-planning request
passes only if a correlated response for the correct session is accepted
within 10 ms at the cited reliability. The experiments do not execute the
returned path through a live intersection, so they test necessary
communication and interface conditions rather than automated-intersection
safety.

##### Example D — CAV cooperative emergency maneuver and trajectory alignment

**Scenario.** A CAV detects an obstacle or another urgent conflict and proposes
an evasive trajectory. Nearby CAVs or an infrastructure service must associate
that intent with the right vehicles, accept or reject it, and adjust their own
motion before the maneuver begins. This is a stateful coordination problem,
not a sensor-sharing problem: the application outcome is agreement on who will
occupy which space-time path.

**Physical window and standard requirement.** 3GPP TS 22.186 Release 19 lists:

- cooperative collision avoidance: a 2,000 B payload, 100 messages/s, 10 ms
  maximum latency, 99.99% reliability, and a 10 Mbit/s data-rate requirement;
- emergency trajectory alignment: a 2,000 B payload, 3 ms maximum latency,
  99.999% reliability, and a 30 Mbit/s data-rate requirement; and
- higher-degree cooperative lane change: a 12,000 B payload, 10 ms maximum
  latency, and 99.99% reliability.

At 25 mph, a vehicle moves approximately 0.034 m in 3 ms and 0.112 m in 10 ms.
The 2,000 B object at 100 messages/s corresponds to at least 1.6 Mbit/s of
application payload before overhead; the larger stated data-rate requirements
cover the wider cooperative scenario rather than that one object alone.

**Traffic behavior and competition.** Unlike periodic awareness, this workload
arrives as an event-driven burst and may require rapid reciprocal messages among
several vehicles or between vehicles and an edge coordinator. It is precisely
the flow that must not wait behind a sensor upload, map transfer, or long-lived
video stream. The direct realization competes with ongoing PC5 awareness in the
sidelink pool; a network-assisted realization competes in both Uu directions
and at the edge. The experiment does not prove scheduler preemption or priority;
it asks whether the complete measured cycle is already outside the 3/10 ms
envelope before such isolation is considered.

**Threshold used in this paper.** The 2 KiB direct-PC5 sweep maps closely to
the 2,000 B trajectory/maneuver object, and the 8--16 KiB 5G sweep brackets the
12 KiB higher-degree lane-change object. A complete multi-step IPI exchange
must meet the applicable 3 or 10 ms budget with the cited availability;
eventual transfer or a median below 100 ms is not sufficient. The experiments
can establish a necessary-condition failure when request/reply RTT already
exceeds the budget, but they do not reproduce a multi-vehicle maneuver or
certify 3GPP radio-layer reliability.

##### Example E — CAV fault-triggered remote recovery to a safer location

**Scenario.** A CAV suffers a critical computing, sensing, or automated-driving
failure near or within an intersection and becomes, or is at risk of becoming,
an obstacle in an active lane. The immediate response and the assisted recovery
must be separated:

1. the vehicle detects the fault and independently slows, stops, or enters a
   minimal-risk state; the network is not the sole emergency braking loop;
2. the vehicle opens an authenticated IPI session with an assistant at the
   intersection edge or a remote help center, requests planning assistance,
   carries pose/kinematics in the envelope, and supplies its fault and remaining
   capabilities as service-specific context with a desired recovery horizon;
3. the assistant returns a high-level recovery path—ordered waypoints, target
   speed, optional dwell, and fallback-route state—toward the roadside, a
   shoulder, or a nearby parking area; and
4. the vehicle validates and executes that path at low speed using its
   remaining onboard control and braking capability, requesting updated paths
   until it reaches the safer location.

The service objective is fault recovery and safe relocation: clear a faulted
or operationally stuck CAV from a dangerous location and restore safe traffic
operation. NHTSA's ADS 2.0 voluntary guidance states that a higher-automation
ADS must be able to reach a minimal-risk condition without depending on driver
intervention and that this may include a safe stop outside an active lane. A
separate NHTSA ADS test-framework report identifies the harder continuation of
this scenario: a remote operator may help maneuver the vehicle to a safe state,
and a fail-safe action may move it out of the active roadway and stop or park
it. The 5GAA Infrastructure-Based Tele-Operated Driving use case supplies the
communication stage almost verbatim: after a critical subsystem failure and
local slow/stop action, a remote driver provides a trajectory and maneuver
instructions that move the vehicle to a safer location. Its stated road
environments include urban roads, highways, and intersections.

**Communication directions and application requirements.** The 5GAA
requirements expose two realistic deployment choices rather than one symmetric
“remote driving” bitrate:

- for an intersection-edge assistant with infrastructure sensing and edge
  fusion, the environment model is estimated at 50--500 kbit/s; the return
  control/path channel is up to 1,000 B per message and up to 400 kbit/s, with
  50 ms in each direction (100 ms round trip), 99.999% reliability for the
  assistant-to-vehicle path, less than 10 km/h assisted speed, and 0.1 m
  positioning accuracy; and
- for a help-center operator using vehicle-originated context, the cited
  remote-support use case estimates 32 Mbit/s for four compressed video feeds
  (optionally 36 Mbit/s with interpreted objects), while a returned path is
  only up to 25 kbit/s. High-level remote-driving instructions use 100 ms from
  vehicle to assistant and 200 ms from assistant to vehicle, with 99% and
  99.999% reliability respectively, at less than 10 km/h.

At the conservative 10 km/h upper bound, the vehicle travels approximately
0.278 m in 100 ms and 0.556 m in 200 ms. These are upper-bound motion
translations for low-speed recovery, not ordinary city-driving speeds.

**Traffic behavior and competition.** The edge-local and help-center variants
have fundamentally different signatures. Edge-local recovery is a long-lived
but compact stateful session: small vehicle fault/pose updates travel uplink and
small vehicle-specific path/status updates return downlink while edge computation
and session state remain occupied. Help-center recovery is a sustained,
uplink-heavy session: vehicle video dominates the data rate, while the return
path remains comparatively small. Several simultaneous recovery sessions can
therefore exhaust uplink, backhaul, edge, or human-assistant capacity long before
their downlink command rate becomes large.

**Threshold used in this paper.** Treat 1 KiB and 4 KiB IPI requests/replies as
compact fault-report and recovery-path brackets, not as claims of an exact
standard encoding. Evaluate accepted correlated recovery responses at 100 and
200 ms. The sourced 100 ms edge value combines two 50 ms legs and the sourced
200 ms value is an assistant-to-vehicle high-level-instruction requirement;
the measured request/reply RTT is therefore a complete-cycle numerical
comparison, not a reproduction of either one-way topology. Likewise, compare
application response availability over every issued request with the 99.999% assistant-to-
vehicle target without relabeling it as measured one-way radio reliability. For
an edge-local assistant, the 50--500 kbit/s fused environment model is produced
and consumed at or behind the intersection edge rather than sent over the
vehicle Uu path;
the measured vehicle path carries the compact fault/session state and returned
trajectory. For a remote help center, compare measured vehicle-originated
capacity with the 32 Mbit/s video class, while keeping any edge-to-help-center
backhaul outside the measured claim. Compare the service-restart timeline
against the requirement to expose an unavailable assistant promptly enough for
the vehicle to remain stopped or abort recovery. The experiment does not
include a remote human, path execution, or vehicle actuation; it tests the
communication, directionality, concurrency, and interruption conditions that
such an IPI service would require.

##### Primary design figure — application traffic behavior and shared-resource contention

Use a two-panel figure here. A latency-versus-payload quadrant is not sufficient
because it hides direction, periodicity, fan-out, state, and competition.

**Panel (a): application traffic signatures.** Use one horizontal lane per
application and a small, consistent visual grammar:

- arrow direction identifies `V -> I/E` Uu uplink, `I/E -> V` Uu downlink, or
  `V <-> V/I` PC5 sidelink;
- arrow width represents relative payload-rate class, not an exact scaled
  bitrate;
- repeated short arrows represent periodic traffic, a clustered group
  represents an emergency burst, a continuous band represents a sustained
  stream, and a bracket marks a stateful session; and
- a small deadline label carries the cited 3/10/100/200 ms requirement. Do not
  use color as the only carrier of meaning.

The lanes should show:

1. **signal warning:** compact periodic, common downlink publication, with an
   optional small uplink request;
2. **FCW/blind-spot awareness:** compact periodic many-to-many PC5 state;
3. **intersection crossing:** per-vehicle periodic uplink state, common
   downlink SPaT/MAP, and vehicle-specific downlink trajectories inside a
   correlated session;
4. **emergency maneuver:** a short, urgent, bidirectional/many-party burst;
5. **edge-local fault recovery:** a long-lived session with compact uplink
   fault/pose state and compact downlink paths/status; and
6. **help-center fault recovery:** a long-lived thick uplink video band with a
   thin downlink command/path band.

**Panel (b): where the flows contend.** Route the lanes into four explicitly
separate shared-resource regions:

- the PC5 sidelink resource pool for direct awareness and maneuver traffic;
- the 5G Uu uplink scheduler/opportunities for vehicle state, detector objects,
  video, and offload input;
- the 5G Uu downlink scheduler/opportunities for common signal state,
  vehicle-specific plans, warnings, and commands; and
- CPE/transport/core/edge-service/backhaul/operator capacity for correlated and
  long-lived services.

Show that Uu uplink and downlink share a configured TDD operating context while
remaining distinct measured directions; do not draw PC5 as another Uu queue.
Add the paper's 25 Mbit/s background uplink beside the compact 1 KiB IPI probe
as the measured coexistence proxy. Add the 23,968 B detector object and large
transfer class as possible competing Uu workloads, but label them experimental
workloads rather than named safety applications. A weak-radio annotation may
state that equal useful bytes can require different radio effort; because the
paper lacks matched PRB/MCS/BLER counters, the figure must mark this as
motivation rather than a measured resource count.

The figure's visual question should be immediate: **when these different traffic
signatures coexist, which deadline-critical exchange keeps its application
envelope?**

##### Authoring evidence ledger — do not reproduce word for word

Retain the following compact table in the outline to keep every figure lane
connected to a stored measurement. It is an authoring ledger, not the proposed
main-paper presentation:

| Example | Traffic signature and principal shared resource | Object/load and deadline | Stored measurement that tests a necessary condition |
|---|---|---|---|
| Traffic-signal warning | periodic common I/E-to-V publication; PC5 pool or Uu downlink; optional small uplink query | approx. 500 B at 10 Hz; approx. 100 ms one-way reference | J2735 SPaT bridge; 5G state mirror; compact PC5/5G acknowledged cycle |
| FCW / blind-spot / lane-change warning | periodic many-to-many PC5 awareness; network variant becomes many Uu updates plus warning fan-out | approx. 200 B per source at 10 Hz; approx. 100 ms | 256 B stationary PC5; PC5 mobile route; compact 5G necessary-condition comparison |
| Infrastructure-guided intersection crossing | recurring per-vehicle Uu state plus common downlink state and vehicle-specific downlink plans; edge session capacity | 300--450 B awareness, 100 B SPaT, 1,000 B MAP, approx. 400 B plan; 10 ms/99.9999% | J2735 SPaT/MAP; 256/512 B and 1 KiB PC5/5G; correlated planning response; concurrency proxy |
| Cooperative emergency maneuver/trajectory alignment | unpredictable bidirectional PC5 or Uu/edge burst that must coexist with periodic and bulk traffic | 2,000 B or 12,000 B objects; 3 ms/99.999% or 10 ms/99.99% | PC5 2 KiB; 5G 8--16 KiB; strict deadline markers; no reproduced multi-vehicle burst |
| Fault-triggered remote recovery | compact long-lived edge session, or sustained uplink-heavy help-center video plus thin downlink path; edge/backhaul/operator occupancy | 1--4 KiB compact state/path, or 32 Mbit/s video; 100/200 ms and 99.999% return-path targets | compact IPI cycle; direction control; 1 KiB foreground under offered uplink load; concurrency; interruption |

##### Coexistence decision rule and evidence boundary

For application `i` under a defined workload mix `W`, report
`A_i(B_i | W)`: the fraction of all application-`i` attempts that produce a
complete accepted result within its budget `B_i` while `W` is active. Compare
this with `A_i(B_i | idle)` and report the background flow's achieved rate or
request count. This makes the foreground application's deadline-qualified
availability—not merely aggregate throughput—the isolation outcome.

The current experiments cover three controlled parts of this question:

- a compact 1 KiB correlated foreground exchange shares the vehicle-originated
  Uu path with an offered 25 Mbit/s sustained background flow;
- 1--100 application clients create increasing correlated-session/request demand
  through one host, one UE attachment, the core path, and the edge service; and
- payload, fragmentation, and detector-object experiments show the completion
  cost and packet amplification of larger objects that could form competing
  traffic, while the direction controls expose the path's uplink/downlink
  asymmetry.

Only the first is a direct heterogeneous mixed-flow experiment. The client
sweep is homogeneous aggregate demand, and the payload/fragmentation sweeps run
conditions separately. The paper therefore does **not** claim that it ran all
five named applications concurrently, mixed PC5 and Uu traffic in one trial,
tested multiple independently scheduled vehicle UEs, reproduced emergency
preemption, measured scheduler isolation, or verified a dedicated 5QI/GBR flow.
These missing cases are part of the future evaluation agenda exposed by the
paper, not hidden behind a broad coexistence claim.

The paper should cite the [NHTSA Vehicle Safety Communications Task 3
report](https://www.nhtsa.gov/document/vehicle-safety-communications-project-task-3-final-report),
the [NHTSA V2V technology-readiness
report](https://www.nhtsa.gov/sites/nhtsa.gov/files/readiness-of-v2v-technology-for-application-812014.pdf),
the [FHWA stopping-sight-distance
assumptions](https://highways.dot.gov/safety/speed-management/speed-concepts-informational-guide/chapter-4-engineering-and-technical),
[3GPP/ETSI TS 22.186 Release
19](https://www.etsi.org/deliver/etsi_ts/122100_122199/122186/19.01.00_60/ts_122186v190100p.pdf),
and [ETSI TS 103 324 Collective Perception
Service](https://www.etsi.org/deliver/etsi_ts/103300_103399/103324/02.01.01_60/ts_103324v020101p.pdf),
and the [5GAA C-V2X Use Cases and Service Level Requirements Volume
II](https://5gaa.org/content/uploads/2021/01/5GAA_T-200116_TR_C-V2X_Use_Cases_and_Service_Level_Requirements_Vol_II_V2.1.pdf).
For the failure/recovery split, also cite [NHTSA's *Automated Driving Systems
2.0: A Vision for Safety*](https://www.nhtsa.gov/document/automated-driving-systems-20-voluntary-guidance)
and [*A Framework for Automated Driving System Testable Cases and
Scenarios*](https://www.nhtsa.gov/node/97186), and connect the edge-guided
planning service to [*A Vision for Transformative
Intersections*](https://doi.org/10.1109/MC.2024.3421963).
The VSC requirements are early connected-vehicle design targets, not current
regulatory mandates; say this explicitly. Use the latest authoritative row for
each 3GPP/ETSI example in the final manuscript.

#### Decision rule and threshold presentation

- Match the decision quantity to the interaction:
  - for a one-way periodic warning, use the fraction of required updates whose
    age at the consuming application is no greater than `B`;
  - for a request/reply or multi-step service, use complete correlated response
    within `B`; and
  - for a stream, require both complete-object deadline attainment and the
    sustained object/update rate for the stated duration.
- The current field instrumentation directly supports the second quantity. For
  an application budget `B`, define deadline-qualified response availability as:

  `A(B) = attempts with a complete accepted correlated response and RTT <= B`
  `/ all sender attempts`.

  Do not use `A(B)` as if it were measured one-way broadcast latency. When a CV
  example is one-way, present the full RTT as an acknowledged-cycle comparison
  and state which one-way timing evidence is absent.

- A result supports a named application only if `A(B)` meets that application's
  required reliability/availability **and** the object size, update pattern,
  direction, range, and operating conditions are represented. Otherwise, state
  exactly which condition is not met.
- For the cross-workload analysis, report `A(100 ms)`, `A(500 ms)`, and
  `A(1,000 ms)` rather than converting them into one universal pass/fail label.
  The 100 ms line connects to less-stringent advanced-driving and
  extended-sensor examples; 500 ms is a relaxed interactive/service threshold;
  and 1,000 ms distinguishes time-sensitive exchange from asynchronous or bulk
  transfer. The latter two are analytical thresholds, not universal standards.
- Add `A(200 ms)` wherever fault-triggered remote recovery is evaluated. That
  line comes from the high-level remote-driving-instruction use case and should
  not be silently omitted merely because the original cross-workload aggregate
  used 100/500/1,000 ms.
- Show 10 and 25 ms standards markers where strict cooperative applications are
  discussed, even though the stored deadline aggregation uses 100/500/1,000 ms.
  The manuscript can conclude that a request/reply condition whose RTT already
  exceeds these markers does not support the strict budget, but it must not
  imply that meeting a round-trip marker certifies the one-way standard.
- Overlay horizontal or vertical threshold lines directly on every applicable
  result plot. State in the caption whether each condition is above or below
  the line and report the issued-request denominator. For non-numerical conditions
  such as IPI interoperability or QoS verification, state a binary criterion
  before reporting the outcome.
- Do not equate `1,000/1,000` observed successes with demonstrated 99.99% or
  99.999% reliability. Report the sample count and, when making a statistical
  reliability comparison, include an interval or an explicit statement that
  the experiment does not contain enough trials to validate a multi-nine
  target.

#### What would constitute the “future CAV/ITS” communication support in this paper

The right side of the Related Work figure is reached only when a communication
method can expose common application semantics, deliver the complete required
object within the application's deadline and reliability target, sustain the
needed uplink/downlink and update rate, preserve critical exchanges while other
services share the applicable resources, remain usable through the required
coverage/load envelope, and expose failure or predicted QoS changes early
enough for adaptation. Section 5 therefore asks where each measured path meets
these conditions and where it falls short. It does not ask whether a network
has a `5G` or `C-V2X` label.

### 4.8 Experiment matrix and analysis rules

#### Tests derived from the readiness question

Open this subsection by stating that the experiments are derived from the
necessary conditions for application readiness. They are not organized around
the three eventual insights. The design asks whether the path functions with
real messages, whether complete exchanges remain available as workload scale
changes, whether the result survives field radio conditions and mobility,
whether shared capacity and concurrency alter service, and whether an accepted
response arrives within the required time budget.

#### Experiment family A — path validation and compact exchanges

**Question:** Do the production paths carry conventional CV/ITS messages and
small correlated service exchanges before more complex workloads are added?

- PC5: 10/10 SPaT bridge delivery to the OBU callback, bidirectional BSM
  reception, and bidirectional custom request/reply bring-up.
- PC5 compact probes: 0, 256, and 512 B conditions as part of the stationary
  sweeps.
- 5G: SPaT/state-mirror and IPI service requests at 0, 256, 1,024, and 4,096 B
  over TCP and MQTT.
- Record functional delivery, response availability over every issued request, and sender-
  side RTT. The standard-message functional check and the custom-probe
  performance measurements must remain separately labeled.

#### Experiment family B — payload and application-object scale

**Question:** How does increasing the size of one complete request/reply object
change availability and latency on each current path?

- PC5 stationary sweep: 0, 256, 512, 1,024, and 2,048 B; normally 1,000
  attempts per payload; 100 ms interval; 1,000 ms sender timeout.
- Preserve operator-stopped timeout-dominant conditions as observed partial
  runs with skipped/remaining attempts identified. Do not silently convert an
  unexecuted phase into measured radio transmissions.
- 5G generic sweep: 0, 256, 1,024, 4,096, 8,192, 16,384, 32,768, 65,536,
  131,072, 262,144, 524,288, 1,048,576, and 2,097,152 B over TCP and MQTT
  across five stationary campaigns collected May 13, 14, 15, 21, and 22,
  2026. Most completed conditions contain 1,000 attempts at 200 ms intervals;
  collection-day weather labels remain descriptive context.
- 5G detector replay: 0/compact controls, 19,648, 22,816, 23,968, and where
  available 25,024 B, plus the separate 60,000 B stress condition, over TCP,
  MQTT, raw UDP, and adapter-fragmented UDP.
- The fragmented-UDP main payloads use 1,000 attempts at 200 ms intervals;
  the 60,000 B stress condition uses 100 attempts. Sender timeouts are 3 s for
  compact datagrams, 5 s for larger detector payloads, and 8 s for the 60,000 B
  condition.
- Keep generic byte sweeps, detector-derived objects, and larger source-object
  chunks distinct in the analysis. They answer related but not identical
  application questions.

#### Experiment family C — coverage, obstruction, and mobility

**Question:** Does a compact exchange remain available across the spatial and
radio conditions a vehicle encounters?

- PC5 stationary points: attempt the full 0--2,048 B sweep at the reference and
  five field locations spanning LOS, NLOS, building obstruction, strong
  normalized conditions, weak conditions, and unavailable segments. Some
  timeout-dominant sweeps were stopped before all payload phases; preserve the
  distinction between transmitted attempts and analysis-only skipped entries.
- PC5 mobile route: four route collections with 256 B requests at a 200 ms
  interval. One operator-stopped drive contains 675 attempts; the other three
  contain 1,000 attempts. The configured timeout is 500 or 1,000 ms depending
  on the run.
- Join requests to NovAtel GNSS position and speed. Report the route, coverage,
  obstruction, the percentage of issued requests that receive responses, and
  successful-response RTT separately. Two fully characterized routes have median speeds of 7.13 and
  7.54 m/s.
- 5G location context: use the 16-point RSRP/RSRQ/SNR survey and repeated
  stationary application runs at mapped locations. The principal 5G payload,
  load, detector, and multiclient experiments are stationary; this is not a 5G
  handover or driving-route study.
- Multi-location follow-up: repeat C1 and C2 at common/typical, candidate-weak,
  repeated common/typical, and candidate-strong stationary positions while
  holding MG52 placement and nominal RAN configuration fixed. Each transport
  uses its own measurement interval and two repetitions where specified.
- Until matched exports are retained, use these follow-ups for application-
  layer location comparison and label radio classes as operator-reported. Do
  not claim RSRP causality or serving-cell equivalence.

#### Experiment family D — direction, mixed-flow uplink competition, and RAN context

**Question:** Does a compact, deadline-sensitive correlated service preserve its
response envelope while a sustained vehicle-originated flow shares the Uu path,
and how different are the two traffic directions in the deployed configuration?

- Treat the 1 KiB IPI-style request/reply as the **foreground** flow and the
  offered 25 Mbit/s vehicle-to-edge transfer as the **background** flow. Compare
  the foreground at idle and under background load using response availability,
  `A(100/200/500/1,000)`, p95, and p99; report the background's achieved rate
  rather than only its configured target.
- This is a controlled proxy for compact intersection/recovery state competing
  with a vehicle-originated sensor, video, or bulk stream. It does not reproduce
  any one of those media streams byte for byte, and the 1 KiB flow has no
  verified scheduler priority.
- Repeat the loaded condition with 1, 2, and 4 uplink streams under stronger
  and weaker field conditions. Keep `qos_profile=default` and the application-
  side `5qi-mapped` label separately identifiable.
- Run 15-minute direction controls configured for 25 Mbit/s in each direction:
  vehicle-to-edge and edge-to-vehicle. Results must report achieved throughput
  separately from configured rate, because transport backpressure may keep a
  flow below its offered target.
- Record that all primary application campaigns before the focused TDD study
  used `70/20/10`. Use the August 15--16 uplink-heavy runs only for the
  deployment-repeatability diagnostic because route, cell state, serving-cell
  selection, and retained raw timing differ. Use the matched August 17 blocks
  for downlink-heavy RTT and exact directional-goodput comparisons. Keep
  `10D4G` fixed throughout.
- The focused follow-up conditions are:
  - C1: one client, 1 KiB, idle;
  - C2: one client, 23,968 B detector p99 payload, idle;
  - C3: one client, 1 KiB, with offered 25 Mbit/s uplink load;
  - C4: 100 clients, 1 KiB, idle apart from the experiment traffic.
- For C1--C4, run TCP, MQTT, and UDP in separate five-minute ACP bins with two
  repetitions. Each single-client condition uses 500 sequential probes per
  repetition; C4 uses 500 probes per client per repetition.
- Cell and management-plane counter interpretation remains conditional on the
  matched ACP Cell 2 and MG52 exports. Do not infer scheduler, PRB, or per-UE
  behavior from host traffic alone.
- Because both flows originate behind the same vehicle host and MG52/UE, this
  experiment measures end-to-end coexistence within one attachment. It does not
  measure fairness or resource isolation among independent vehicle UEs.

#### Experiment family E — concurrency and aggregate service demand

**Question:** Does one edge service continue to return complete correlated
responses as aggregate request rate and client-demand count grow, and how
does that stress interact with the radio condition of the shared UE?

- Emulate 1, 2, 5, 10, 20, 50, and 100 application clients over TCP, MQTT, and UDP.
- Each historical client sends 1,000 sequential 1 KiB requests at a nominal
  200 ms interval, or five requests/s, producing nominal aggregate offered
  rates from 5 to 500 requests/s.
- Repeat the full 1--100 client sweep under a weaker signal condition.
- Motivate the paired sweep explicitly: a weak-radio UE can operate at lower
  spectral efficiency and can therefore impose a different radio-resource cost
  for the same useful application traffic. Comparing stronger and weaker
  conditions tests whether concurrency sensitivity changes with that operating
  regime, while the absence of matched scheduler counters prevents a direct
  PRB/MCS attribution.
- Measure response availability over every issued request, p50/p95/p99 RTT for completed
  replies, aggregate offered rate, and per-client distribution/fairness.
- State the important scope boundary: application clients are generated behind one
  vehicle host and one MG52-HW. The experiment stresses application, transport,
  CPE, Uu, core, and edge-service handling, but it is not a 100-vehicle or
  100-UE scheduler experiment.
- Present this experiment together with the offered-uplink-load sweep as the
  paper's controlled attempt to approximate load pressure found in a public
  deployment. It covers aggregate demand and competing traffic, but not an
  operator's heterogeneous UE population, mobility/handover state, roaming,
  backhaul geography, admission control, or scheduling policy.
- Describe this family as homogeneous session/request scaling, not a
  heterogeneous application mix. Its value is to show whether service demand
  alone changes the foreground envelope and how that effect interacts with the
  one shared UE's field condition.
- The locked-cell C4 follow-up uses 100 clients with 500 requests per client and
  two repetitions per transport; it is a repeated high-concurrency endpoint,
  not a replacement for the historical full client-count sweep.

#### Experiment family F — protocol behavior and complete-message formation

**Question:** Does a nominally successful network path deliver the complete
application object under different protocol and fragmentation choices?

- Compare TCP request/reply, MQTT QoS 0 over TCP, raw UDP, and UDP with
  adapter-level fragmentation for common compact and detector-derived sizes.
- Use the raw-UDP sizes 0, 1,024, 1,400, 4,096, and 19,648 B to locate the
  onset of path-MTU/IP-fragment sensitivity in the measured path.
- Use adapter-fragmented UDP with a 1,400 B maximum datagram payload for
  0, 256, 1,024, 4,096, 19,648, 22,816, 23,968, 25,024, and 60,000 B as
  available in the good- and weak-condition runs.
- Count a fragmented message as available only after reassembly and correlated
  acknowledgment. Individual fragment transmission is not complete application
  delivery.
- Report message-to-datagram amplification and completion time where the stored
  data permit it. This characterizes why a large logical object can occupy more
  queue/radio work and thereby become harmful competing traffic. Because the
  foreground probe was not multiplexed into these runs, do not call this family
  a measured isolation or mixed-flow experiment.
- Preserve the first aborted raw-UDP diagnostic as diagnostic evidence and do
  not merge it with the finalized fragmented-UDP experiment.

#### Experiment family G — application time budgets, QoS evidence, and service interruption

**Question:** Is a reply merely eventual, or is it available within a useful
application budget and robust to service-layer disruption?

- Re-evaluate every selected sender attempt at 100, 200, 500, and 1,000 ms
  budgets.
  These are cross-workload evaluation thresholds, not standards guarantees or
  deadlines assigned to every named CAV application.
- A deadline hit requires both a correlated accepted response and
  `RTT <= budget`. A timeout, missing response, rejection, or late response is
  a miss.
- Sender/harness timeouts range from 3 to 60 s across 5G experiments and 500 or
  1,000 ms for the mobile PC5 experiments. They define how long the harness
  waits and must not be confused with application-analysis budgets.
- QoS evidence: compare the default and `5qi-mapped` application labels under
  load and capture packets at the vehicle interface. Both retained captures
  show TOS `0x0`; the experiment therefore tests whether a label alone changes
  the observed application path, not a verified dedicated 5QI bearer.
- Connect this binary verification to coexistence: without observable marking
  or bearer evidence, the paper cannot attribute foreground protection—or the
  lack of it—to a configured QoS mechanism.
- Service interruption: while 1 KiB requests are active, restart the TCP
  receiver, UDP receiver, MQTT application receiver, or MQTT broker. Each
  condition contains 1,000 attempts.
- Measure missed requests, RTT tails among completed replies, and recovery
  interval. No alternate radio or local-computation fallback is exercised, so
  name the experiment service interruption/recovery rather than demonstrated
  fallback.

#### Compact experiment-matrix table

Use one concise table in the manuscript with one row per experiment family and
the following columns:

- readiness question;
- communication path;
- controlled factors;
- fixed controls and repetitions;
- measured outcomes.

Keep exact payload lists and condition-specific timeouts in a reproduction
table or appendix if the main table becomes too dense. Unlike the rejected
Related Work comparison table, this table represents an actual experimental
design with repeated fields and exact mappings; it should not contain novelty
claims or prose about prior papers.

#### Metrics and evidence boundaries

##### Primary application metrics

- **Response availability:** number of sender attempts that receive the
  expected correlated application response before the configured harness
  timeout divided by all issued sender requests.
- **Sender-side RTT:** elapsed monotonic time from the request send point to the
  correlated response at the same sender; defined only for attempts that
  receive a response.
- **RTT distribution:** report at least p50, p95, and p99 among completed
  responses, always alongside the response-availability denominator.
- **Deadline attainment:** number of issued requests with an accepted response and
  RTT no greater than the selected 100, 200, 500, or 1,000 ms budget divided by
  all issued requests.
- **Throughput/direction:** distinguish configured offered rate, achieved host
  throughput, application goodput where available, and request rate.
- **Spatial context:** position, route, speed, LOS/NLOS/obstruction, measured 5G
  RSRP/RSRQ/SNR at survey granularity, and normalized PC5 path availability at
  its stated granularity.

Response availability and RTT must never be collapsed into one successful-
packet latency statistic. A low p50 for the responses that arrived does not
show that the service was available to all requests.

##### Repetition and controls

- Most completed baseline conditions contain 1,000 attempts. Report exceptions
  explicitly: operator-stopped PC5 runs, 100-attempt 60,000 B fragmented-UDP
  controls, 500-attempt follow-up repetitions, and the 100-client multiplication
  of attempts.
- Treat repeated requests within one field condition as repeated observations,
  not independent deployments. Separate repeated days, positions, route passes,
  and follow-up blocks from per-request samples.
- Preserve failures, timeouts, partial files, and aborted diagnostics. Exclude a
  run only with an explicit reason, and never replace failures with retries in
  the reported denominator.
- Weather, collection date, and operator signal labels are context unless a
  controlled design and matched telemetry support a causal comparison.

##### Claims this section must prohibit

- No one-way 5G latency from unsynchronized endpoints.
- No over-the-air-only latency claim from application RTT.
- No universal LTE C-V2X, NR-V2X, 5G, or 6G limit from one device path.
- No claim that a 100-client workload is a 100-radio or 100-vehicle field test.
- No claim that the private deployment reproduces a public network's
  independent background UEs, handover, roaming, geographic backhaul, or
  operator policy. Report the offered-load and client-demand sweeps positively
  as controlled load proxies and state exactly which shared components they
  stress.
- No claim that weak-signal clients consumed a measured number of additional
  PRBs or caused a particular MCS/retransmission decision without matched RAN
  counters. The experiments measure the application-level interaction between
  radio condition and concurrency.
- No claim that `5qi-mapped` establishes a nondefault 5QI, QFI, DSCP marking,
  GBR flow, or enforced QoS.
- No causal RSRP, TDD, MCS, layer, scheduler, PRB, RLC, or interference claim
  without matched radio evidence.
- No claim that a transferred chunk is a complete image, point cloud, or map.
- No claim that the timed detector replay includes sensing, inference,
  planning, or vehicle actuation.
- No claim that the PC5 normalized path-availability map is measured RF power.
- No performance findings, conclusions, or three-insight language in this
  section beyond the functional checks and coverage/configuration facts needed
  to establish the measurement setup.

### 4.9 Section close and transition to Results

End with a short synthesis, not an outcome:

> Together, the two independently measured paths cover the direct local and
> network-assisted communication modes identified in Related Work. The
> experiment matrix moves from standard and compact messages to detector-sized
> objects and larger transfer chunks, then varies propagation context,
> mobility, protocol, direction, offered uplink load, concurrency, deadline,
> QoS evidence, and service availability. The Results section applies the same
> complete-response and RTT definitions across these conditions to determine
> where today's measured paths do and do not meet the application-readiness
> criterion, including whether a compact deadline-sensitive exchange retains
> its envelope while sustained uplink traffic shares the path.

This transition should point forward only to measurements. Although the
Introduction previews the paper's three insights, Section 4 must not use them
to organize or justify the experiment design. Their detailed evidence chain and
discussion belong after the Results have been presented and synthesized.

## 5. Experiment Results

### Section purpose and writing logic

This section answers the readiness question using the conditions and thresholds
defined in Section 4.7. It reports only outcomes that determine whether a
measured communication path supports a CV/CAV exchange or provide actionable
information to ITS, V2X, 5G/6G, CAV/CV, carrier, RAN, and signal researchers.

Do **not** repeat the testbed inventory, radio installation, device roles,
coverage-survey summary, IPI field definitions, benchmark-environment bring-up,
or experiment procedure here. Those facts belong in Sections 3 and 4. The
Mocar SDK bring-up, 5G survey statistics, dataset registry, local loopback,
four-GPU artifact pass, OpenCOOD smoke test, and detector-checkpoint execution
remain protocol/setup/workload validation unless one outcome is required to
interpret a communication result.

The section should follow this evidence order:

1. establish that IPI carries conventional and service-style application
   exchanges;
2. characterize the direct PC5 path across payload and spatial conditions;
3. characterize the 5G path across payload and application time budget;
4. isolate complete-object effects from protocol choice and fragmentation;
5. test direction and mixed-flow coexistence by placing a compact correlated
   foreground exchange beside sustained offered uplink traffic, then test radio
   condition, homogeneous session concurrency, QoS evidence, and service
   interruption; and
6. only after all results are visible, synthesize the answer and derive the
   three main insights.

Every result subsection should contain, in this order:

- the readiness condition being tested;
- the external or analytical threshold drawn on the figure/table;
- the comparison and issued-request denominator;
- no more than one short paragraph describing the measured outcome; and
- one short result sentence stating which condition is met or missed.

Within this section, do not restate or argue for an insight before Section 5.9.
The Introduction may preview the three findings, but the Results headings and
plots must present the comparison first so the data visibly lead to the detailed
insight discussion rather than appearing chosen to prove fixed conclusions.

### 5.1 IPI workload representation and framing cost

#### Measurement purpose and comparison

Establish that the enabling protocol represents the two workload classes used
by the communication study and adds a fixed or bounded number of bytes. Keep
this subsection proportional to a supporting systems contribution; it is not a
separate protocol evaluation.

- The compact profile preserves typed BSM, PSM, MAP, SPaT, SRM, and SSM content
  and byte-exact pre-encoded J2735 content.
- The correlated-operation profile represents request, update, completion, and
  rejection together with session and correlation information.
- Report the measured content representation before path-specific experiment
  and transport framing. A preserved J2735 payload adds the measured 1-B type
  and 4-B length fields. The measured operation representation adds 78--81 B
  when an application object is present.
- Distinguish these representation costs from the complete PC5 performance-frame
  sizes and from the Uu application-object size selected before serialization.

#### Presentation

Use one narrow table with rows for the compact J2735 profile, a planning request
without an object, and the measured 256-B, 1-KiB, and 4-KiB operation objects.
Columns should show IPI fields, application content, and representation total.
The supporting prose should explain that the added bytes remain fixed or
bounded as the application object grows. Full wire-profile details and
conformance evidence belong in the appendix or artifact.

**One-sentence result summary:** IPI represents both compact J2735 messages and
correlated CAV operations with fixed or bounded framing cost, allowing the
subsequent field experiments to compare the two workload classes through the
same application-facing methodology.

### 5.2 Direct LTE C-V2X: payload size and radio condition jointly determine complete delivery

#### Readiness condition and thresholds

Test whether one complete direct-link request/reply remains available as the
payload grows from compact state to multi-kilobyte service content and as the
radio condition changes. Draw 10, 25, and 100 ms latency markers and the
reliability target for any explicitly named application example. Always plot
response availability beside successful-response RTT.

Tie the sizes to the Section 4.7 applications: 256 B approximates the
preliminary 200 B FCW/blind-spot/lane-change warning object, while 2 KiB maps
closely to the 2,000 B 3GPP cooperative-collision-avoidance object. At 25 mph,
label the 100 ms and 10 ms lines as 1.12 m and 0.112 m of vehicle travel,
respectively. These are complete request/reply comparisons; the 100 ms PC5 RTT
does not directly measure the one-way VSC warning latency.

The main comparison is not “C-V2X versus no network.” It is:

- same deployed PC5 devices and request/reply implementation;
- payloads 0, 256, 512, 1,024, and 2,048 B;
- reference/strong, degraded, building-obstructed NLOS, and unavailable field
  conditions; and
- complete replies per issued request, not packets emitted.

#### Reference-link result

At the stable reference location:

| Payload | Accepted / attempts | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---:|---:|---:|---:|---:|
| 256 B | 999 / 1,000 | 99.980 | 106.917 | 111.691 |
| 512 B | 1,000 / 1,000 | 99.972 | 106.694 | 114.577 |
| 1,024 B | 1,000 / 1,000 | 99.966 | 106.878 | 112.515 |
| 2,048 B | 1,000 / 1,000 | 99.958 | 111.663 | 120.567 |

The 10 and 25 ms markers lie below even the successful-response median. The
100 ms marker lies near the median and below the p95 for every listed payload.
Accordingly, high eventual delivery at this stationary location must not be
written as support for strict cooperative-driving deadlines.

#### Joint payload/radio result

- Across normalized Points 2--4, the 2 KiB failure rate rose from 0.2% to
  99.2%, followed by 910/910 observed failures at the next weaker point.
- Across the same degradation sequence, the 1 KiB failure rate rose from 0% to
  2.1% and then 82.9%.
- The 256 and 512 B conditions remained more available than 1 and 2 KiB as the
  radio condition weakened, until the path itself became unavailable.
- Point 1 must be shown as building-obstructed NLOS rather than ordered by
  geometric distance alone. Its short distance does not imply a strong path.
- Operator-stopped timeout-dominant conditions must retain observed and skipped
  attempt counts. A skipped phase is not a transmitted packet failure.

#### Primary figure

Use a two-panel figure:

- **Panel (a):** heatmap with field location/radio ordering on the x-axis,
  payload size on the y-axis, and complete-response availability in each cell.
  Print the attempt denominator and use a color scale that makes the 90%, 99%,
  and 99.9% comparison levels visible without calling them universal PC5
  requirements.
- **Panel (b):** p50/p95/p99 successful-response RTT versus payload at the
  reference location with 10, 25, and 100 ms lines. Use an arrow or annotation
  that points from the high-reference success cells in panel (a) to the latency
  limitation in panel (b).

The figure caption should state the comparison result directly: larger packets
lose availability first as the field path weakens, while the stable
reference-link tail already exceeds the strict 10/25 ms markers.

**One-sentence result summary:** The measured direct path is useful for compact
local exchanges in favorable radio conditions, but neither successful-link RTT
nor multi-kilobyte delivery remains inside the strict application envelope as
payload and propagation conditions change.

### 5.3 Direct LTE C-V2X: successful-packet latency hides route-level unavailability

#### Readiness condition and thresholds

Test operating-envelope continuity for a moving vehicle. Compare issued-request
availability against the reliability requirement of the named application;
show the 90% lower end of the cited TS 22.186 examples only as an illustrative
reference and show stricter targets when relevant. Successful-response latency
must be plotted separately.

Use the 256 B route probe as the compact FCW/blind-spot/lane-change payload
comparison from Section 4.7. The route experiment tests whether the information
is available across the driven spatial envelope; it does not reproduce the
specific 25/35 mph lane-change geometry or a driver-warning response.

#### Results to report

| Mobile run | Attempts | Accepted | Availability | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---|---:|---:|---:|---:|---:|---:|
| Route 1 | 675 | 515 | 76.3% | 99.610 | 108.421 | 112.436 |
| Route 2 | 1,000 | 731 | 73.1% | 28.326 | 41.887 | 55.880 |
| Route 3 | 1,000 | 591 | 59.1% | 29.204 | 43.923 | 54.317 |
| Route 4 | 1,000 | 716 | 71.6% | 28.276 | 41.621 | 50.651 |

- All four availability over every issued request values fall below the illustrative 90%
  line; they are much farther below application classes requiring 99% or more.
- Among replies that arrived in the later route runs, p95 RTT remained below
  50 ms. That apparently favorable tail does not include the 26.9--40.9% of
  requests with no successful reply.
- Attribute the failures to the observed spatial weak/no-signal portions of the
  route, not to mobility alone. Preserve the normalized signal/availability
  terminology because the device diagnostic path did not return a usable RSSI.

#### Primary figure

Use the GNSS route map already defined in Section 4 as the left panel, colored
by request outcome or normalized availability. Use an aligned right panel with
two y-axes or two stacked plots: availability over every issued request and successful-reply
p95 RTT by route segment. Do not color only successful packets.

**One-sentence result summary:** Low RTT where the direct path works does not
compensate for route segments in which the application receives no reply.

### 5.4 5G payload size determines which application deadlines remain attainable

#### Readiness condition and thresholds

Test complete request/reply availability as application objects grow from
compact state through detector output, map/perception chunks, and bulk
transfers. The primary quantity is `A(B)` at 100, 500, and 1,000 ms. Add 10 and
25 ms markers to show that the measured request/reply path does not represent
the strictest cooperative-control class.

Annotate the payload axis with the Section 4.7 examples: approximately 400 B
for an intersection-manager trajectory/planned-velocity result, 2,000 B for
cooperative collision avoidance or emergency trajectory alignment, 12,000 B
for higher-degree cooperative lane change, and 1--4 KiB for the compact IPI
fault-report/recovery-path bracket. Also label 23,968 B as the repository's
validated detector-output p99 replay size. That last point is an experimentally
grounded object-scale workload for protocol and fragmentation analysis, not one
of the paper's primary CAV examples and not a standardized message encoding.

Use byte size on a logarithmic x-axis. Do not substitute advertised link rate,
configured TDD share, or successful transfer for completion within the
application budget.

#### Cross-workload deadline result

Present the stored aggregation as deadline-qualified availability rather than
deadline-miss rate:

| Evidence group | Attempts | Eventual response availability | p50 / p95 / p99 RTT ms | `A(100 ms)` | `A(500 ms)` | `A(1,000 ms)` |
|---|---:|---:|---:|---:|---:|---:|
| SPaT/state mirror | 2,000 | 100.00% | 58.748 / 130.323 / 140.479 | 53.40% | 100.00% | 100.00% |
| Compact service <=4 KiB, excluding multiclient | 60,000 | 100.00% | 119.739 / 413.725 / 791.676 | 37.76% | 96.29% | 99.45% |
| Loaded/QoS-label 1 KiB | 34,000 | 100.00% | 139.163 / 602.354 / 990.812 | 24.12% | 93.47% | 99.04% |
| Detector-output replay | 20,100 | 99.54% | 108.039 / 245.879 / 349.732 | 42.86% | 99.32% | 99.54% |
| Multiclient 1 KiB | 564,000 | 99.79% | 97.832 / 233.349 / 298.077 | 50.92% | 99.51% | 99.73% |
| Mid payload 8--64 KiB | 40,100 | 99.78% | 83.265 / 199.793 / 303.647 | 70.37% | 99.66% | 99.78% |
| Map/perception 128--512 KiB | 18,000 | 100.00% | 227.193 / 667.975 / 946.023 | 0.99% | 88.36% | 99.27% |
| Bulk >=1 MiB | 14,000 | 100.00% | 1,146.068 / 3,122.273 / 3,579.058 | 0.00% | 0.00% | 37.39% |

State that each row aggregates multiple conditions and therefore describes the
measured envelope, not one homogeneous statistical population. Follow the
table with condition-level plots so a good location/transport is not averaged
with a stressed one.

#### Repeated-campaign and tail result

- In the stable May 13 campaign, compact MQTT 0--4 KiB p95 RTT was roughly
  36--49 ms and compact TCP p95 was roughly 100--140 ms. At 2 MiB, MQTT
  p50/p95/p99 reached 2,136/2,536/2,815 ms and TCP reached
  3,140/3,633/4,053 ms.
- A second location/campaign produced larger tails: MQTT 0 B had
  p50/p95/p99 27/70/300 ms, TCP 0 B had 117/140/545 ms, and TCP 2 MiB p95
  reached 10,767 ms.
- Under the much weaker or more variable May 15 condition, even compact tails
  increased (TCP 0 B p95 1,142 ms and MQTT 0 B p95 510 ms), while MQTT
  256 KiB completed 999/1,000 attempts with p50 15,153 ms, p95 43,836 ms,
  and p99 199,429 ms.
- The stable May 22 repeat returned to MQTT 0--4 KiB p95 around 38--48 ms and
  TCP p95 around 120--155 ms, while 1 MiB remained seconds-scale.

These repeats are important to radio and carrier readers because one favorable
median cannot define the deployment. Present collection day and field condition
as repeated contexts, not weather-causality experiments.

#### Primary figure

Use a payload/deadline envelope figure rather than a bar chart:

- x-axis: complete request payload bytes on a logarithmic scale;
- y-axis: deadline-qualified availability;
- curves: 100, 500, and 1,000 ms budgets;
- facets or line styles: TCP and MQTT, with detector-derived and generic
  payloads marked separately; and
- horizontal lines: only reliability targets explicitly associated with the
  application example in the text.

An inset can show p95/p99 RTT on a log scale for MiB objects. The caption should
say that 100% eventual response at MiB scale is still 0% within 500 ms and only
37.39% within 1 s in the aggregate.

**One-sentence result summary:** The 5G path supports compact and some
detector-sized exchanges at relaxed budgets, but the usable object size shrinks
rapidly as the deadline tightens or the field path becomes variable, and MiB
objects are asynchronous/bulk transfers in the measured request/reply path.

### 5.5 Protocol choice changes complete-object availability, not only latency

#### Readiness condition and thresholds

Test whether the application receives one complete detector-derived object.
The threshold is complete reassembly plus correlated acknowledgment within the
selected deadline; sending one datagram or receiving some fragments is not a
success. Separate TCP, MQTT-over-TCP, raw UDP, and adapter-fragmented UDP.

Use the repository's validated detector-output p99 workload to make the
complete-object and fragmentation cost concrete. If a 23,968 B object were
offered at 10 Hz, it would carry 1.917 Mbit/s of payload and require 180
adapter fragments/s at the tested 1,400 B fragment cap; the 60,000 B stress
object would carry 4.8 Mbit/s and require 430 fragments/s. Label this as a
workload translation, not a primary application requirement. The experiment
sends sequential request/reply probes rather than a continuous 10 Hz stream,
so it tests per-object completion and latency, not sustained fulfillment of
that hypothetical offered rate.

#### TCP and MQTT comparison

Under the good-signal detector replay:

| Application payload | MQTT p50 / p95 / p99 ms | TCP p50 / p95 / p99 ms | Complete responses |
|---:|---:|---:|---:|
| 0 B | 27.023 / 49.826 / 65.177 | 120.004 / 151.696 / 185.130 | 1,000/1,000 each |
| 23--25 KiB | 111.672 / 183.705 / 209.486 | 111.538--134.948 / 169.540--213.894 / 203.890--275.649 | 1,000/1,000 per listed condition |
| 60 KiB | 243.656 / 403.790 / 681.227 | 240.056 / 381.409 / 659.838 | 1,000/1,000 each |

Under the weak-signal replay, TCP and MQTT still returned 100% of the selected
objects, but 60 KiB MQTT p95/p99 rose to 770/1,230 ms and TCP rose to
662/1,027 ms. Thus complete eventual response and 500/1,000 ms attainment must
be reported separately.

#### Raw and adapter-fragmented UDP comparison

- On the finalized raw-UDP path, 0 and 1,024 B returned replies, whereas
  1,400, 4,096, and 19,648 B produced zero successful replies. Treat this as
  the measured path's datagram/fragmentation behavior, not a universal IP MTU
  value.
- With 1,400 B adapter fragments and receiver-side reassembly, objects from
  0 through 23,968 B achieved 99.5--100.0% complete-response availability in
  the good condition. The 60 KiB stress object completed only 20/100 times.
- Under the weak condition, fragmented objects through 4,096 B achieved
  99.9--100%; 19--25 KiB objects fell to 93.5--96.1%; and 60 KiB completed
  0/100 times.
- Preserve the first aborted raw-UDP diagnostic as diagnostic evidence only;
  do not average its empty/partial files into the finalized comparison.

#### Primary figure

Use a small-multiple complete-object success plot:

- x-axis: logical object size;
- y-axis: complete-response availability;
- panels: good and weak conditions;
- series: TCP, MQTT, raw UDP, and adapter-fragmented UDP; and
- threshold lines: 90%, 99%, and 99.9% as labeled comparison levels, plus
  application-specific lines only where sourced.

Add p95/p99 deadline markers for TCP/MQTT in a companion panel rather than
putting incompatible success and latency units on one axis.

**One-sentence result summary:** Adapter-level fragmentation extends UDP beyond
the raw-datagram failure point, but it does not make large complete objects
reliable under weak signal, while TCP/MQTT preserve eventual completion at the
cost of deadline-sensitive tails.

### 5.6 Directional asymmetry and competing uplink traffic constrain mixed CAV services

#### Readiness condition and thresholds

Test whether the deployed path sustains both the asymmetric capacity and the
mixed-flow behavior needed by the application portfolio in Section 4.7. The
central comparison is a compact 1 KiB correlated foreground exchange at idle
and while an offered 25 Mbit/s vehicle-to-edge background flow is active. Report
foreground response availability, p95/p99, and
`A(100/200/500/1,000 ms)` alongside the background's achieved throughput. This
directly asks whether a compact deadline-sensitive service retains its envelope
when a sustained uplink-heavy workload shares the same host, UE attachment, Uu
path, core path, and edge endpoint.

Use the application examples to explain why the comparison matters without
claiming that the exact applications ran concurrently:

- the foreground represents the compact correlated portion of an intersection-
  guidance or edge-local recovery session;
- the background represents the resource pressure of a sustained vehicle-
  originated sensor, video, or bulk stream; and
- an emergency maneuver would be an even tighter event-driven foreground, but
  the experiment did not generate or prioritize a 3/10 ms emergency burst.

Then compare achieved vehicle-to-edge and edge-to-vehicle throughput with the
configured 25 Mbit/s controls. The result is an application-level coexistence
measurement within one UE attachment; it is not a multi-UE fairness test, a
verified scheduler-isolation experiment, or a reproduction of all application
lanes in the Section 4.7 figure.

Example E supplies three communication roles:

- compact vehicle fault/session state when the transformative intersection
  already observes the scene and builds the 50--500 kbit/s fused environment
  model locally or behind the edge;
- 32 Mbit/s vehicle-to-help-center video when remote context must originate at
  the vehicle; and
- up to 400 kbit/s for direct commands or up to 25 kbit/s for a high-level
  return path, with 99.999% assistant-to-vehicle reliability.

Plot exact upload and download goodput in separate panels. Show all ten
repetitions per profile, the mean, and the 95% confidence interval. Compare the
upload means with the 32 Mbit/s help-center video reference in prose. The
matched 500-KiB direction panel should show the 500-ms application reference.
The direction controls were stationary, so they characterize communication
performance rather than a complete remote-assistance or vehicle-control loop.

#### Direction-control result

- The August 15--16 uplink-heavy runs characterize deployment repeatability.
  The established 70/20/10 profile repeated closely, while route recovery,
  cell-state changes, serving-cell selection, and missing raw timestamps make
  the alternative-profile comparison inconclusive for a TDD ranking.
- The matched August 17 downlink-heavy experiment completed 8,000/8,000
  exchanges per profile. The 40/40/20 profile lowered small-response latency,
  while the effect disappeared or reversed in larger MQTT tails.
- Ten exact 50-MiB transfers per profile produced 12.000 versus 2.836 Mbit/s
  vehicle-to-edge and 126.786 versus 99.756 Mbit/s edge-to-vehicle under
  70/20/10 versus 40/40/20. These are application-goodput observations, not
  evidence that the nominal frame ratio determines radio capacity.
- At the matched August 18 40/40/20, MQTT, 500-KiB, -115-dBm condition,
  uplink-heavy p50/p95 was 61.5/41.0 times the downlink-heavy result.

#### Foreground-under-background-load result

- Under the good-signal load sweeps, a 25 Mbit/s vehicle-to-edge background
  flow increased MQTT p95 from roughly 52--60 ms at idle to 174--176 ms and
  TCP p95 from roughly 162--194 ms to 388--420 ms.
- Present this first as a foreground-envelope comparison, not as a bulk-
  throughput observation: the compact flow is the one whose deadline-qualified
  availability must be preserved while the background is present.
- With 1, 2, and 4 offered uplink streams under the default label, MQTT p95 was
  170, 468, and 182 ms; TCP p95 was 391, 402, and 410 ms. Preserve the
  non-monotonic MQTT result rather than imposing a monotonic load story on
  sequential field data.
- With the application-side `5qi-mapped` label, MQTT p95 was 377, 487, and
  529 ms and TCP p95 was 431, 680, and 772 ms for 1, 2, and 4 streams.
- Under the first weak-signal sweep, even idle MQTT/TCP p95 was 552/668 ms;
  loaded default conditions reached MQTT p95 638--770 ms and TCP p95
  682--934 ms. In the second weak repeat, idle improved to 160/150 ms, while
  loaded default p95 remained 460--708 ms for MQTT and 630--759 ms for TCP.
- In the locked-cell C3 follow-up, the offered-load workload produced five UDP
  acknowledgment timeouts. Report those failures in the complete-response
  denominator rather than replacing them with retries.
- Because the request travels vehicle-to-edge and its correlated reply returns
  edge-to-vehicle, the observed RTT change cannot be assigned to uplink
  scheduling alone. State only that the end-to-end foreground exchange degrades
  while a vehicle-originated background flow shares the measured path.

#### QoS-verification result

- Both the default and `5qi-mapped` vehicle-interface captures decoded observed
  packets with TOS `0x0`.
- The application label therefore fails the predeclared criterion for verified
  host marking, and the evidence does not establish a nondefault 5QI/QFI or
  network-enforced QoS treatment.
- Do not interpret different p95 values between sequential default and labeled
  runs as a QoS effect when the intended treatment is not verified.

#### Primary figure

Use three aligned panels:

- configured versus achieved throughput for the two directions, with the
  25 Mbit/s configured line and 32 Mbit/s help-center-video line on the uplink,
  and the 25--400 kbit/s path/command band on the downlink;
- foreground p50/p95/p99 and `A(100/200/500/1,000)` for idle versus offered
  uplink load under good and weak field conditions, with the achieved background
  throughput stated in the caption; and
- a binary QoS-evidence panel showing the requested application label and the
  observed `TOS 0x0` capture result.

**One-sentence result summary:** The deployed path carries a compact correlated
service at idle, but its tail grows when a sustained vehicle-originated flow
shares the path; the measured uplink also does not demonstrate the 32 Mbit/s
help-center video class, and the application-side QoS label provides no evidence
that the foreground received protected treatment.

### 5.7 Concurrency amplifies the effect of the shared radio condition

#### Readiness condition and thresholds

Test whether one edge service continues to return complete 1 KiB responses as
client-demand demand rises from 1 to 100, and whether the result changes when
the shared UE operates under a weaker field condition. Plot
`A(100/200/500/1,000)` and response availability over every issued request against client
count; show p95/p99 only beside those denominators. The 200 ms line is useful
for asking whether one edge service can sustain multiple high-level recovery-
path sessions, while the one-host/one-UE scope boundary remains explicit.

Restate once, in the caption rather than repeatedly in prose, that all logical
clients share one vehicle host and one MG52/UE. This is an application-path
load experiment, not a 100-UE scheduler experiment.

State how this result complements Section 5.6: Section 5.6 is a heterogeneous
foreground/background coexistence test, whereas this subsection scales one
homogeneous compact request pattern. Together they expose two distinct causes
of contention, but neither reproduces a public-network mixture of independent
vehicle UEs.

#### Good-condition comparison

Representative endpoints are:

| Run | Transport | Clients | Accepted / attempts | Reported median condition p50 / p95 / p99 ms |
|---|---|---:|---:|---:|
| Sweep 2 | MQTT | 20 | 20,000 / 20,000 | 39.658 / 89.857 / 150.457 |
| Sweep 2 | TCP | 20 | 20,000 / 20,000 | 131.856 / 221.672 / 358.833 |
| Sweep 2 | UDP | 20 | 19,997 / 20,000 | 30.776 / 45.927 / 61.520 |
| Sweep 3 | MQTT | 100 | 100,000 / 100,000 | 114.125 / 176.239 / 526.698 |
| Sweep 3 | TCP | 100 | 100,000 / 100,000 | 203.807 / 283.521 / 329.811 |
| Sweep 3 | UDP | 100 | 99,365 / 100,000 | 65.526 / 145.953 / 169.719 |
| Sweep 4 | MQTT | 100 | 100,000 / 100,000 | 41.776 / 81.835 / 119.849 |
| Sweep 4 | TCP | 100 | 100,000 / 100,000 | 187.678 / 263.984 / 304.022 |
| Sweep 4 | UDP | 100 | 99,965 / 100,000 | 46.206 / 112.121 / 139.776 |

Show all 1, 2, 5, 10, 20, 50, and 100-client points in the figure, not only
these endpoints. The two 100-client repeats should remain visible because their
MQTT tails differ substantially; selecting only the better repeat would
overstate stability.

#### Weak-condition comparison

| Transport | Clients | Accepted / attempts | Availability | Reported median condition p50 / p95 / p99 ms |
|---|---:|---:|---:|---:|
| MQTT | 50 | 49,998 / 50,000 | 99.996% | 173.885 / 1,115.787 / 1,937.951 |
| MQTT | 100 | 99,499 / 100,000 | 99.499% | 851.780 / 3,814.102 / 13,149.421 |
| TCP | 50 | 50,000 / 50,000 | 100.000% | 316.839 / 771.208 / 1,539.818 |
| TCP | 100 | 100,000 / 100,000 | 100.000% | 410.177 / 2,938.408 / 6,400.146 |
| UDP | 50 | 48,364 / 50,000 | 96.728% | 83.609 / 169.862 / 180.218 |
| UDP | 100 | 90,811 / 100,000 | 90.811% | 103.532 / 172.519 / 181.779 |

- At 100 clients under the weak condition, MQTT and TCP retain high eventual
  response availability but their p95/p99 tails move far beyond 500/1,000 ms.
- UDP keeps a much lower successful-response tail but loses 9,189 of 100,000
  requests. A latency-only comparison would therefore incorrectly favor UDP.
- These results establish an application-level interaction between radio
  condition and concurrency. They do not establish how many PRBs the UE used
  or which scheduler/MCS/retransmission mechanism caused the outcome.

#### Multi-location follow-up comparison

- The medium/typical C1--C4 block retained 307,662/309,000 accepted responses
  (99.566990%). C4 TCP and MQTT accepted all 100,000 attempts per transport;
  C4 UDP accepted 98,667/100,000.
- The candidate-weak C1/C2 block accepted 5,994/6,000 (99.9%); all six failures
  were C2 UDP acknowledgment timeouts. The repeated common/typical block
  accepted 5,999/6,000, and the candidate-strong block accepted 6,000/6,000.
- Using the two common blocks as one descriptive reference, the candidate-
  strong location had lower p95 in every matched C1/C2 aggregate: 2.84--12.49%
  lower for the listed TCP/UDP cases and 4.03--21.16% lower for MQTT/UDP across
  C1/C2. State the exact per-transport values in a compact table or appendix.
- Call this an association. The sequential blocks and unmatched MG52/ACP
  exports do not establish that RSRP alone caused the change.

#### Primary figure

Use a two-row, three-column figure with one transport per column:

- top row: good-condition `A(B)` or p95/p99 versus application clients;
- bottom row: weak-condition values on the same axes and thresholds; and
- an inset or adjacent marker for the locked-cell C4 repeat.

The caption should explicitly compare the same client count and transport
across radio conditions. Avoid a single aggregated “multiclient p95” that hides
transport and field condition.

**One-sentence result summary:** Aggregate demand is manageable for some
compact exchanges under favorable conditions, but weak-radio operation turns
high TCP/MQTT concurrency into deadline misses and turns UDP's low completed-
reply tail into substantial loss.

### 5.8 Service interruption appears as missing requests and recovery gaps

#### Readiness condition and thresholds

Test whether service availability survives a receiver or broker restart while
1 KiB requests are active. The comparison is to uninterrupted application
service: every request during the application budget must either complete or be
reported unavailable in time for a fallback decision. For Example E, an
unavailable assistant must be exposed while the faulted CAV remains stopped or
before it begins the next recovery segment. Show 100, 200, 500, and 1,000 ms
budget markers and 99%/99.9% descriptive availability lines; also show the
99.999% remote-path target while stating that 1,000 attempts cannot demonstrate
five-nines reliability. Do not label the experiment a demonstrated radio
fallback.

#### Results to report

| Restart condition | Accepted / attempts | Availability | p50 / p95 / p99 RTT ms among replies | Success gap ms |
|---|---:|---:|---:|---:|
| MQTT broker | 955 / 1,000 | 95.5% | 31.644 / 47.871 / 90.553 | 15,100.7 |
| MQTT receiver | 991 / 1,000 | 99.1% | 31.977 / 49.855 / 259.906 | 13,367.4 |
| TCP receiver | 953 / 1,000 | 95.3% | 126.629 / 161.464 / 361.228 | 11,714.7 |
| UDP receiver | 969 / 1,000 | 96.9% | 30.305 / 44.139 / 63.027 | 13,463.5 |

- The configured service outage was 10 s, but the measured gap between
  successful responses was 11.7--15.1 s depending on transport/service.
- MQTT-receiver restart is the only condition above the descriptive 99% line;
  none reaches 99.9%, and all are far below the cited 99.999% remote-path
  target over the injected event.
- Completed-response medians remain close to their ordinary values, and even
  p99 is below 500 ms. Those percentiles exclude 9--47 missing requests and the
  multi-second recovery interval.
- Failure detail is transport-specific: TCP recorded a peer close and reconnect
  failures; UDP and MQTT receiver restarts primarily appeared as acknowledgment
  timeouts; broker restart included peer-close, reconnect, and acknowledgment
  failures.
- No alternate radio, cached answer, local computation, or safe-control action
  was executed. The result measures service interruption and recovery only.

#### Primary figure

Use an event-aligned timeline for each restart, with accepted requests,
timeouts/reconnect failures, failure-begin, restart-begin, restart-complete, and
first post-restart success. A small adjacent table can report availability and
p99. The timeline is more informative than a latency CDF that drops missing
requests.

**One-sentence result summary:** Median and p99 RTT among completed replies hide
11.7--15.1 s application-service gaps, so an IPI remote-recovery session must
expose interruption promptly enough for the faulted CAV to remain stopped or
abort the assisted maneuver rather than assuming eventual reconnection is a
safe fallback.

### 5.9 Summary of results, discussion, and insights

This is the only subsection in which the paper should synthesize across
experiments and state the three main insights. It should be concise enough that
the reader can connect every conclusion to a preceding figure or table.

#### 5.9.1 Overall answer to the readiness question

Open with one short paragraph:

> Today's measured communication paths support important parts of connected
> and automated vehicle operation, but readiness is application- and
> condition-specific. The direct PC5 path carries conventional messages and
> compact exchanges under favorable local radio conditions, yet payload growth
> and route coverage sharply reduce complete-response availability. The 5G
> path supports compact and some detector-sized service exchanges at relaxed
> deadlines, but its usable object size contracts under tight deadlines, weak
> field conditions, competing uplink traffic, concurrency, and service
> interruption. The application portfolio also matters: periodic broadcasts,
> many-to-many awareness, vehicle-specific stateful plans, emergency bursts, and
> sustained video do not consume the same resources or fail in the same way.
> In the controlled coexistence test, a compact foreground exchange develops
> larger tails when sustained uplink traffic shares its path. Neither a low
> successful-packet median, 100% eventual transfer, nor an isolated-flow result
> is sufficient evidence of application readiness.

Then use one compact synthesis figure, not another large results table. A good
form is an **application communication envelope**:

- horizontal axis: complete application-object size / interaction complexity;
- vertical axis: increasingly difficult operating conditions (reference radio,
  degraded radio/route, offered uplink load, concurrency, interruption);
- direct PC5 and 5G shown as separate measured envelopes;
- markers for 3/10/25/100/200/500/1,000 ms application budgets; and
- regions labeled `measured support`, `conditional`, and `not met`, with every
  boundary linked to a prior result figure.

Add one explicit coexistence slice that connects the same 1 KiB foreground
marker at idle and under offered uplink load. Do not generalize that slice into
an all-application scheduler-isolation result.

Do not turn the synthesis into an architectural “gap bridge” diagram. The
Related Work figure already explains the conceptual gap; this figure summarizes
measured operating envelopes.

Include one limitations sentence in this overall answer: the envelopes apply to
the tested devices and paths and do not reproduce independent public-network
UEs, handover, roaming, geographic backhaul, or operator scheduling policy.
Their value is the controlled comparison method and the exposed failure
dimensions, not a universal performance number for all C-V2X or 5G networks.

#### 5.9.2 Insight 1 — Direct-V2X payload and coverage envelope

Use one compact manuscript paragraph:

> **The measured commercial direct V2X path supports compact J2735 messages
> only within a limited payload and coverage envelope.** The functional tests
> deliver standard SPaT
> and BSM messages, but stationary obstruction and mobile weak/no-path segments
> remove compact requests. Urban intersections can exploit nearby roadside
> infrastructure, whereas high-rise street canyons, highways, and sparsely
> served roads may require carefully placed roadside units or a supplemental
> path.

#### 5.9.3 Insight 2 — CAV packet-size limit on the 5G uplink

Use one compact manuscript paragraph:

> **The measured 5G uplink supports only severely limited CAV packet sizes
> within decision deadlines.** Every evaluated TCP/MQTT detector
> condition through 19,648 B remains below a 500-ms p95 deadline at the two
> measured placements, while weak-path MQTT crosses the deadline at 22,816 B.
> Approximately 20 KiB of application content is the conservative measured
> 500-ms p95 ceiling for this path. For a 100-ms deadline, even the aggregate
> group at or below 4 KiB completes only 37.76% of issued requests in time.
> These limits occur with one UE on a dedicated 40-MHz clean n48 channel. At
> the matched 500-KiB, -115-dBm condition, the downlink-heavy p95 is 226.787 ms
> while the uplink-heavy p95 is 9,287.828 ms.

#### 5.9.4 Insight 3 — Event-triggered bursts in either direction

Use one compact manuscript paragraph:

> **5G/6G radios must support event-triggered CAV bursts in either direction
> and isolate concurrent flows.** The established 70/20/10 profile
> repeats closely, while the alternative-profile transition changes route and
> cell state. A later matched 40/40/20 block completes every exchange, but its
> effect depends on payload and transport, and nominal frame allocation does
> not predict exact application goodput. Concurrent application demand also
> moves a compact exchange across its deadline. Radio developers must make
> alternative profiles interoperable and stable. Schedulers must allocate
> resources according to each burst's direction and deadline while protecting
> simultaneous flows.

#### Authoring evidence anchors for the three insights — not manuscript prose

Keep the detailed evidence in the outline and preceding result subsections, not
in the three insight paragraphs:

- **Insight 1 anchors:** functional J2735/compact PC5 delivery; 256 B--2 KiB
  stationary payload/signal interaction; building obstruction; and 59.1--76.3%
  mobile-route issued-request response availability.
- **Insight 2 anchors:** all evaluated TCP/MQTT detector points through 19,648 B
  below a 500-ms p95 at favorable and weak placements; weak MQTT at 22,816 B
  above 500 ms; the approximately 20-KiB conservative ceiling; and 37.76% of
  aggregate at-most-4-KiB attempts within 100 ms on the dedicated 40-MHz clean
  single-UE network.
- **Insight 3 anchors:** 0.22--1.29% p95 repeat variation under the established
  `70/20/10` profile; route, cell-state, and serving-cell changes during the
  August 15--16 alternative-profile transition; 8,000/8,000 matched
  downlink-heavy completions per profile; 12.000 versus 2.836 Mbit/s upload
  and 126.786 versus 99.756 Mbit/s download; foreground tail growth under
  deliberately introduced uplink traffic; and one-to-100 application-client
  demand. The application model supplies the event-triggered arrival and
  variable-direction premise; the measurements supply the direction,
  configuration, and concurrent-demand consequences.

Each insight should remain one compact paragraph containing the design
statement, synthesized evidence, and its stakeholder consequence. Do not repeat
entire result tables or introduce the broader research agenda here.

### Result-writing and figure discipline

- Present measured comparisons first and interpretation only in Section 5.9.
- Give every plot an explicit application threshold or a sentence explaining
  why no universal numerical threshold applies.
- Report attempts, failures, and deadline-qualified availability in the main
  text or caption; never place only p50/p95/p99 among successful replies.
- Use consistent colors for PC5 and 5G across the paper, and consistent line
  styles for 100/200/500/1,000 ms budgets across all 5G plots.
- Use log scale for payload size and long RTT tails, but print important values
  so the conclusion is readable without estimating from an axis.
- Mark aborted, partial, operator-stopped, unmatched-radio, and diagnostic-only
  runs visually and explain whether they enter each denominator.
- Avoid statistical significance language unless the analysis actually models
  repeated field conditions rather than treating per-request rows as
  independent deployments.
- Do not use `better`, `worse`, `supports`, or `ready` without naming the
  comparison axis, application condition, and threshold.
- Keep raw run-folder names out of the main narrative. Put exact result-source
  mapping, payload lists, timeouts, and repetition counts in an appendix or
  artifact table so the main section reads as an argument rather than a log
  inventory.

## 6. Future Research Directions

### Section purpose and boundary

This is a research agenda for readers of the paper, not a conventional
“limitations and future work” section about Edge4AV. Do not enumerate devices
we could test, implementation features we could add, or experiments the authors
plan to run. Do not use “we will,” “our next step,” or “future versions of our
system.” A missing condition in this evaluation becomes a research direction
only when it exposes a broader unanswered problem for CAV, CV, ITS, traffic-
signal, radio, carrier, edge, or 5G-Advanced/6G researchers.

The section should answer:

> What communication research is now needed if future CAV/ITS applications have
> different message semantics, directions, traffic patterns, deadlines, and
> failure consequences, and must coexist on shared infrastructure?

The five directions below are not additional findings or numbered insights.
The three concise insights in Section 5 establish the evidence; this section
uses them to pose field-level research questions and describe the capabilities
the community should create. Each subsection should take one short manuscript
paragraph, or two only when necessary. Use the same internal order:

1. unresolved research question;
2. why it matters for future CAV/ITS communication;
3. concrete technical problems to solve; and
4. the outcome that would let the community evaluate progress.

### Opening transition from the results

Use one concise transition:

> The results point beyond choosing between PC5 and 5G or increasing one peak
> data rate. Future CAV/ITS communication must make application requirements
> visible, protect heterogeneous services when they coexist, adapt data and
> computation to the available path, preserve state through disruption, and
> evaluate these capabilities with application-level evidence. We organize the
> resulting research agenda around these five needs.

### 6.1 Turn application requirements into an enforceable network service

#### Research question

How can a CAV/ITS application communicate what one complete result requires,
and how can the communication system expose and enforce what it can actually
provide?

#### Why this is a field-level problem

- Applications know whether an object is common or vehicle-specific, periodic or
  event-driven, deadline-critical or prefetchable, and whether a partial or late
  result is useful. A RAN, edge platform, or transport commonly sees packets,
  flows, and configured QoS classes without the full service meaning.
- Conversely, the network observes radio condition, load, available uplink and
  downlink opportunities, bearer treatment, congestion, mobility, and predicted
  interruption that the application cannot infer reliably from a connectivity
  label.
- A priority field, 5QI name, API option, or technology choice is not yet a
  contract unless the application declaration is mapped to an observable and
  enforced treatment.

#### Research problems

- Define a minimal, interoperable service profile that can express complete-
  object size, update rate, direction, one-to-one/one-to-many/many-to-many scope,
  periodic/burst/stream behavior, deadline, freshness, required availability,
  expected session duration, acceptable partial result, and fallback action.
- Determine how that profile maps to 5G/6G QoS flows, admission control,
  scheduling, multicast/broadcast, PC5 priority/resource selection, edge-
  compute reservation, and backhaul policy without binding the application to
  one radio generation or vendor.
- Make the contract bidirectional: the network should return verified current
  or predicted capacity, delay/availability range, path state, and enforcement
  status so the application can reduce fidelity, defer, prefetch, change paths,
  or enter a safe local mode before a deadline is missed.
- Prevent misuse. Research must authenticate service declarations, detect
  inflated priority or rate claims, isolate untrusted participants, and provide
  auditable evidence of the treatment actually applied.
- Define conflict resolution when many applications request incompatible
  guarantees. The interface needs explicit rejection, negotiated degradation,
  and revocation semantics rather than silently accepting every request.

#### Desired research outcome

A transport-independent, machine-readable application/network contract whose
declared requirement, admitted treatment, observed behavior, and failure state
can be verified end to end. IPI demonstrates why common application semantics
are useful, but this direction is broader than extending IPI: it requires
standards, operating-system/network APIs, carrier mechanisms, edge platforms,
and application policy to agree on one enforceable lifecycle.

### 6.2 Schedule heterogeneous vehicular services across radio and edge resources

#### Research question

How should a communication and edge system allocate resources when compact
periodic awareness, common signal state, event-triggered uplink- and
downlink-heavy CAV bursts, vehicle-specific trajectories, detector objects,
bulk transfers, and sustained recovery streams coexist?

#### Why this is a field-level problem

- Optimizing isolated throughput or average packet latency does not protect the
  foreground application's complete-object deadline under a mixed workload.
- The contention domains are not interchangeable. PC5 traffic uses sidelink
  resources; Uu traffic has direction-specific radio opportunities; stateful
  services also occupy CPE, transport, core, edge CPU/GPU, backhaul, and
  sometimes human-assistant capacity.
- Equal useful bytes can have different radio cost under different channel
  conditions. A weak or rapidly changing vehicle may need more radio effort,
  making deadline protection, efficiency, and fairness a joint problem.

#### Research problems

- Develop deadline- and complete-object-aware scheduling that protects compact
  safety or control exchanges from sustained sensor, video, detector, and bulk-
  transfer queues without starving lower-priority services indefinitely.
- Study admission, preemption, and graceful-degradation policies for long-lived
  stateful services. A network may need to admit fewer full-fidelity recovery
  streams, reduce their representation, or keep a vehicle in a safe state
  rather than accept sessions it cannot sustain.
- Co-design Uu uplink/downlink allocation with the application portfolio. This
  includes when and how TDD allocation can adapt, how neighboring-cell alignment
  and interference constrain that adaptation, and how the system prevents an
  uplink-heavy vehicular service from invalidating downlink-critical control.
- Coordinate PC5 and Uu at the service layer without pretending that they draw
  from one resource pool. Research should determine which information remains
  local and direct, which goes through the edge, when duplication is worth its
  cost, and how simultaneous use affects congestion in both domains.
- Extend resource management beyond the RAN. Edge scheduling should account for
  correlated session state, compute deadlines, accelerator contention, shared
  maps, backhaul, and remote-assistant capacity together with radio delivery.
- Protect efficiency and fairness across vehicles with different radio
  conditions, application criticality, and requested rates, while resisting
  starvation and strategic priority inflation.

#### Desired research outcome

Schedulers and admission policies evaluated by foreground
`A_i(B_i | W)`—complete, deadline-qualified application availability under a
declared workload mix—together with useful-object goodput, resource cost,
fairness, and service degradation. Aggregate throughput remains useful context,
but it is not the optimization objective by itself.

### 6.3 Jointly adapt information, computation, and communication

#### Research question

What is the smallest useful representation, and where should it be produced,
when the original CAV object cannot reach its consumer within the available
communication envelope?

#### Why this is a field-level problem

- An application often does not need every raw sensor byte. Depending on the
  decision, it may need a hazard flag, selected objects, uncertainty, a local
  occupancy region, an intended maneuver, or a trajectory.
- The correct representation depends on the current deadline, radio condition,
  competing load, available vehicle/edge computation, and how the consumer will
  use a partial or lower-fidelity result.
- Moving computation to an intersection edge is not automatically superior.
  It reduces some vehicle-originated traffic only when the infrastructure has
  the required observations, models, freshness, trust, and compute capacity.

#### Research problems

- Develop application-aware semantic reduction, region-of-interest selection,
  uncertainty-preserving compression, object prioritization, progressive
  transmission, early-exit inference, and partial-result policies. The research
  criterion is safe decision utility before the deadline, not compression ratio
  alone.
- Decide dynamically among local execution, vehicle-to-vehicle cooperation,
  transformative-intersection computation, regional edge, and remote help
  center according to end-to-end sensing, compute, communication, and actuation
  budgets.
- Separate information that is common to many vehicles from per-vehicle state.
  SPaT, MAP, shared maps, or a fused scene may be published once, broadcast,
  cached, or prefetched; vehicle intent and vehicle-specific paths require
  per-participant freshness, correlation, and authorization.
- Study the transformative intersection as a joint traffic-control,
  communication, sensing, and edge-computing system. Signal researchers and ITS
  operators need methods for reconciling authoritative controller state,
  infrastructure perception, common advisories, and vehicle-specific CAV plans
  without allowing an edge application to violate signal-controller safety
  constraints.
- Quantify when the overhead of coding, inference, serialization, security,
  fusion, and coordination consumes the latency or energy saved by sending a
  smaller object.
- Establish safety and uncertainty semantics for partial or degraded results so
  the receiver never interprets a reduced object as a complete view of the
  scene.

#### Desired research outcome

Policies that maximize deadline-qualified decision utility—not bytes delivered—
while preserving uncertainty, provenance, safety constraints, privacy, and an
explicit local fallback. This direction joins semantic communication, edge AI,
traffic-signal operation, and communication/computation co-design around a
measurable CAV task outcome.

### 6.4 Build service continuity and safe degradation into stateful CAV communication

#### Research question

How should a multi-step CAV service preserve or safely terminate its state when
the radio path, broker, receiver, edge process, backhaul, or remote assistant
becomes unavailable?

#### Why this is a field-level problem

- Packet reliability does not guarantee service continuity. A stateful service
  can have low RTT among completed messages while the application experiences a
  long interval with no usable response.
- A late trajectory, duplicate command, response from an expired session, or
  silently migrated service may be more dangerous than an explicit failure.
- CAVs will cross RSU, cell, edge, operator, and administrative boundaries; the
  communication contract and safety state must survive those transitions or
  fail early enough for local control to take over.

#### Research problems

- Design session migration, replicated edge state, idempotent operations,
  sequence/version control, duplicate suppression, and explicit terminal state
  across handover, roaming, broker failover, edge migration, and process restart.
- Combine predicted path/service availability with proactive make-before-break
  migration, caching, or representation reduction. Predictions must include
  calibrated uncertainty so the application can choose a conservative fallback.
- Determine when redundant PC5, cellular, multi-operator, or multi-edge paths
  improve deadline-qualified availability and when correlated failures,
  duplicated load, or inconsistent state remove that benefit.
- Integrate communication failure with the vehicle's minimal-risk behavior. A
  remote-recovery service should distinguish “assistant temporarily delayed,”
  “session unavailable,” “path revoked,” and “vehicle must remain stopped,” with
  authenticated transitions and bounded detection time.
- Treat cybersecurity and service continuity jointly: failover must not permit
  stale, replayed, conflicting, or unauthorized commands, and security
  re-establishment must enter the end-to-end deadline budget.
- Evaluate detection time, deadline misses, uninterrupted-service fraction,
  recovery gap, state consistency, fallback completion, and safety outcome—not
  only packet loss during the failure.

#### Desired research outcome

Fail-operational or fail-safe service protocols whose state, authority,
deadline, and fallback remain explicit across radio and service disruptions.
The application should know quickly whether to continue, reduce capability,
change communication/computation mode, or remain in a minimal-risk state.

### 6.5 Establish community benchmarks for application-ready CAV communication

#### Research question

What common benchmark and reporting standard would let researchers compare
whether a communication system supports future CAV/ITS applications rather than
compare only radios, peak rates, or successful-packet medians?

#### Why this is a field-level problem

- A byte sweep is reproducible but lacks application meaning; an end-to-end
  driving demonstration is meaningful but often makes it difficult to isolate
  communication causes. The community needs both controlled objects and
  recognizable application scenarios.
- Results cannot be compared when papers use different success denominators,
  omit failed requests, report only completed-packet latency, mix one-way and
  RTT measurements, or leave workload direction and operating conditions
  unspecified.
- Five-nines application claims cannot be inferred from hundreds or thousands
  of attempts, especially when route, weather, radio, and load samples are not
  independent deployments.

#### Benchmark design

- Define a small application portfolio covering compact periodic broadcast,
  many-to-many awareness, vehicle-specific request/reply, strict event-driven
  coordination, processed-perception objects, sustained uplink-heavy sessions,
  asynchronous/prefetch traffic, and service interruption.
- For every workload, publish the complete object, direction, temporal pattern,
  fan-out, update rate, deadline, freshness, required availability, geographic
  scope, session behavior, and consequence/fallback for a missed result.
- Include controlled mixed-service portfolios, multiple independently scheduled
  UEs, heterogeneous radio conditions, obstruction, realistic city mobility,
  handover, roaming, backhaul/edge distance, interference, TDD context, public
  and private deployments, and repeatable failure events.
- Retain synchronized one-way timing where possible, sender-side RTT,
  application and transport traces, achieved foreground/background rates, and
  matched RAN evidence such as serving cell, PRB use, MCS, BLER, HARQ/RLC events,
  and scheduler decisions. When a layer is unavailable, state the causal limit
  rather than filling it with inference.
- Report response availability over every issued request, `A_i(B_i | W)`, p50/p95/p99 among
  completed results, complete-object goodput, fragment/packet amplification,
  resource cost per useful object, per-vehicle fairness, interruption/recovery
  gaps, energy where relevant, and confidence bounds appropriate to the target
  reliability.
- Release workload generators, object-size manifests, scenario definitions,
  analysis code, anonymized traces, and failure/partial-run accounting. Traffic
  agencies, vehicle developers, equipment vendors, and carriers should define
  privacy-preserving release rules so reproducibility does not expose vehicle,
  subscriber, or infrastructure-sensitive data.

#### Desired research outcome

A shared evidence standard that lets the community compare PC5, Uu,
multi-connectivity, edge placement, schedulers, QoS mechanisms, semantic
representations, and 5G-Advanced/6G proposals by the application exchanges they
complete within deadline—not by technology labels alone.

### Section close

End with a short synthesis rather than a prediction that 6G will solve the
problem:

> The central research opportunity is to treat CAV communication as a portfolio
> of time-bounded services rather than a collection of packets. Progress will
> require a common application/network contract, mixed-service resource
> management, joint information-computation-communication adaptation, explicit
> continuity and safe degradation, and reproducible application-level evidence.
> These directions give CAV, ITS, signal, radio, carrier, edge, and 5G/6G
> researchers a shared basis for deciding what future communication systems must
> provide.

### Writing and claim discipline

- Do not call these items limitations, planned experiments, or contributions of
  this paper.
- Do not number them as additional insights. The paper still has exactly three
  insights, all in Section 5.9.
- Do not imply that IPI, a new scheduler, semantic communication, edge
  computing, multi-connectivity, or 6G alone solves the complete problem.
- Tie each direction to the communication behaviors established in Section 4.7
  and the evidence pattern summarized in Section 5, but do not repeat result
  numbers unless one is essential to motivate the problem.
- Prefer precise research questions and testable outcomes over “more work is
  needed,” “higher bandwidth,” “lower latency,” or “test at larger scale.”
- Keep vehicle safety decisions local when communication is unavailable; future
  networking support may enable or improve an application but must not be
  presented as the sole emergency-control loop.

### Source anchors for the eventual section

Use authoritative sources to show that these directions connect to active
standards and future-network programs; the directions themselves should remain
our evidence-grounded synthesis:

- [3GPP/ETSI TS 22.186 Release
  19](https://www.etsi.org/deliver/etsi_ts/122100_122199/122186/19.01.00_60/ts_122186v190100p.pdf):
  heterogeneous advanced-driving, extended-sensor, and remote-driving service
  requirements.
- [ETSI MEC 030 V2X Information
  Service](https://www.etsi.org/deliver/etsi_gs/MEC/001_099/030/03.02.01_60/gs_mec030v030201p.pdf):
  edge V2X information and network-assisted service context.
- [NGMN 5G TDD Uplink white
  paper](https://www.ngmn.org/wp-content/uploads/220117-5G-TDD-Uplink-White-Paper-v1.0.pdf):
  vertical uplink requirements and TDD uplink constraints.
- [ITU-R M.2160, Framework and overall objectives of the future development of
  IMT for 2030 and
  beyond](https://www.itu.int/rec/R-REC-M.2160-0-202311-I/en): future-network
  usage scenarios and capability framework. Use it as a broad 6G/IMT-2030
  anchor, not as proof that a future system already meets a CAV requirement.
- [5GAA C-V2X Use Cases and Service Level Requirements, Volume
  II](https://5gaa.org/content/uploads/2021/01/5GAA_T-200116_TR_C-V2X_Use_Cases_and_Service_Level_Requirements_Vol_II_V2.1.pdf):
  application-specific direction, rate, latency, reliability, and recovery
  requirements.
- [NHTSA, *Automated Driving Systems 2.0: A Vision for
  Safety*](https://www.nhtsa.gov/document/automated-driving-systems-20-voluntary-guidance):
  minimal-risk behavior and the boundary between communication assistance and
  vehicle safety responsibility.

## 7. Conclusion

### Purpose and length

Use two short connected paragraphs: the first moves from the CAV need to the
question and explains how the paper makes that question measurable; the second
states the measured answer and its meaning. The two contributions must be clear
from this logic rather than introduced as “our first contribution” and “our
second contribution.” Do not introduce new numbers, citations, limitations,
applications, future research directions, or claims.

### Proposed conclusion paragraph

> Edge4AV contributes an application-facing, real-vehicle measurement of whether
> direct PC5 and network-assisted Uu support compact CV/ITS messages and
> correlated CAV operations. The study increases application payload and varies
> communication conditions to determine where each measured path stops meeting
> response-latency, deadline-completion, and directional-goodput requirements.
> IPI supplies the enabling application contract that makes the two workload
> classes comparable through profile-specific wire representations.
>
> The results show that communication readiness is an application-specific
> operating envelope. Direct PC5 carries compact J2735 messages when roadside
> coverage is available, but obstruction and sparse infrastructure can remove
> that information before a human driver can use it. The 5G path carries larger
> stateful exchanges, although the measured uplink crosses a decision deadline
> at a far smaller object size than the matched downlink. The established TDD
> profile is repeatable, whereas the alternative-profile transition changes the
> end-to-end path and serving cell. A later matched alternative-profile block
> completes every exchange, but its latency effect depends on transport and
> payload, and nominal allocation does not predict exact application goodput.
> These results show that object size, direction, coverage, and radio operation
> jointly determine application readiness. IPI provides the common CV/CAV
> interface, while the field evaluation identifies the communication limits
> that radio implementations and schedulers must address. Radios must preserve
> stable TDD operation, while schedulers allocate resources according to each
> event-triggered burst's direction and deadline.

### Writing discipline

- Keep the final manuscript conclusion to these two compact paragraphs.
- Preserve exactly two contributions—the primary application-driven field
  evaluation and the enabling IPI protocol/implementation—by showing how they
  work together to make the readiness question measurable. Do not label them as
  list items and do not add a testbed contribution.
- Summarize the three insights as one connected answer rather than numbering or
  re-explaining them.
- Do not enumerate experiment dimensions or repeat experiment values, equipment
  inventory, source citations, limitations, stakeholder advice, or the five
  research directions.
- Do not call Mocar the system and do not call Edge4AV the proposed system;
  neither name is needed in the conclusion.
- End on application-ready communication, not on a claim that 5G, 6G, IPI, or a
  new radio alone solves the problem.
