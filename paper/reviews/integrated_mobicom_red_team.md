# Integrated MobiCom Red-Team Review: Current New Manuscript

## Review scope and bottom line

This review covers only the current manuscript in `paper/manuscript`: `main.tex`,
Sections 0--7, `references.bib`, and the rendered `main.pdf`. The implementation,
`experiment_summary.md`, the outline, and prior section-review reports were used
only to audit claims. No material from `paper/legacy_draft` was read or reused.
The twelve-page limit is intentionally outside this review. No recommendation
below is motivated by page count or compression.

**Panel decision: weak reject in the current integrated form, with no P0 flaw and
a clear path to weak accept after the P1 corrections below.** The manuscript has
a coherent MobiCom question, an unusually careful all-attempt measurement
method, exactly two contributions, and exactly three approved insights. Its
remaining problems are integration errors: several sentences still make the
separate PC5 custom-echo and 5G IPI-ACK experiments sound like one IPI service
evaluation; one PC5 claim invokes a radio metric the device did not expose; one
Cell 2 statement exceeds the retained evidence; and a few summary sentences
slide from accepted path response to semantic completion. These are correctable
without new experiments.

There are **no P0 issues**. The P1 issues are listed first because they should be
adjudicated before another prose-polish pass.

## P1 must-fix issues

### P1.1 Define “support” at its first central use, not only in Section 5.9

The manuscript eventually gives the correct boundary: Section 5.9,
`05_experiment_results.tex:398--400`, says that support means meeting a measured
necessary communication condition and does not mean semantic service execution.
However, the abstract and Introduction ask whether a path can “support” an
application before the reader receives that definition. The Introduction then
says the contributions determine which exchanges each path “can support”
(`01_introduction.tex:166--168`). This leaves the strongest interpretation in
force for four sections.

Required change: after the central question at `01_introduction.tex:59--62`, add
a sentence equivalent to:

> Here, support means meeting the measured necessary communication conditions
> defined below; it does not mean that a remote service executed correctly or
> that vehicle safety was established.

The abstract already contains the accepted-ACK boundary at
`00_abstract.tex:28--31`, so it needs no substantive change. The Conclusion
should repeat the same boundary as specified under P1.3.

### P1.2 Keep the PC5 custom probe, the J2735 callbacks, and the 5G IPI ACK path separate everywhere

Sections 3--5 contain the necessary facts, but several integration sentences and
figures erase the distinction:

- `01_introduction.tex:115--116`: “the two paths expose complete application
  behavior” is not established. The PC5 performance workload is a custom
  sequence-matched echo; J2735 evidence is a separate 10/10 SPaT callback and
  observed BSM reception; the 5G path returns a decoded accepted IPI probe ACK.
- `02_related_work.tex:128--138`: “one IPI operation model and one completion
  rule” sounds as if IPI was carried and completed on both paths.
- `02_related_work.tex:161`: Figure 1 says “one complete-exchange test,” which is
  stronger than the measured common criterion.
- `03_ipi_protocol_design.tex:326--330`: “binds this contract to the measured
  LTE C-V2X PC5 and 5G NR Uu paths” conflicts with the same section’s explicit
  statement that there is no facade-to-PC5 binding (`:263--270`).
- `04_system_design_setup.tex:76--107`: Figure 3 labels the common vehicle host
  “IPI sender and timer” and feeds “J2735, IPI, dataset, and detector workloads”
  into both branches. The drawing visually implies an IPI-over-PC5 experiment.
- `07_conclusion.tex:9--11`: “evaluate complete exchanges over ... PC5 and ...
  Uu” again makes the probes appear semantically and procedurally identical.

Required replacements:

1. Replace `01_introduction.tex:115--116` with:

   > Together, the two paths expose path-response behavior for complementary
   > direct and network-assisted communication under controlled field
   > conditions; they do not execute the same application protocol or establish
   > semantic service completion.

2. Replace the core comparison at `02_related_work.tex:128--138` with wording
   that states the actual design, for example:

   > Our study places conventional-message functional checks and
   > application-derived object classes under one all-attempt path-response
   > criterion. PC5 performance uses a custom sequence-matched echo after
   > separate BSM/SPaT functional checks, whereas Uu performance uses decoded,
   > accepted IPI probe ACKs. The commonality is the application-level decision
   > rule, not a shared wire protocol or a completed remote service.

3. In Figure 1 (`02_related_work.tex:161`), replace “one complete-exchange test”
   with **“one all-attempt path-response test.”**

4. Replace `03_ipi_protocol_design.tex:326--330` with:

   > IPI therefore gives compact vehicular messages and CAV service operations a
   > common application boundary without claiming to solve the network beneath
   > it. The next section defines the separately measured PC5 custom-echo and 5G
   > IPI-ACK paths and the common necessary-condition test applied to them.

5. In Figure 3, replace “IPI sender and timer” with **“Experiment sender and
   timer.”** Replace the workload box with **“J2735 functional checks; PC5 custom
   echoes; 5G IPI ACK probes; dataset- and detector-derived sizes.”**

6. Replace `07_conclusion.tex:9--11` with the conclusion wording in P1.3.

This issue is not cosmetic. The clean separation is already a strength of the
abstract, Table 4, Table 5, and the opening of Section 5. It must remain true in
the paper’s framing and final synthesis.

### P1.3 Remove the remaining accepted-ACK/semantic-completion ambiguity

The metric boundary itself is sound. Section 4 defines
`A_cycle(B)` using all sender attempts, calls the PC5 response a sequence-matched
echo, calls the 5G response a decoded accepted ACK, and explicitly excludes
planning/recovery execution and enforced request/session correlation
(`04_system_design_setup.tex:388--422`). Section 5 repeats the distinction
(`05_experiment_results.tex:4--14`). The abstract also says that the large-object
ACK is not a semantically completed service (`00_abstract.tex:28--31`).

Three remaining phrases should be aligned:

- `01_introduction.tex:141--142`: replace “object completion depends on the
  application’s time budget” with **“complete-object ACK return depends on the
  application’s time budget.”**
- `05_experiment_results.tex:54--56`: “this interoperability” is unsupported.
  Local format acceptance, one vendor J2735 callback path, a custom PC5 echo,
  and an IPI ACK do not establish cross-vendor IPI or J2735 interoperability.
  Replace the paragraph with:

  > Consequently, the conventional-message checks and the IPI envelope/ACK
  > probes function at their stated format and path-response levels. These checks
  > do not establish external interoperability, semantic service completion, or
  > satisfaction of an application deadline.

- Replace `07_conclusion.tex:7--11` with:

  > To make this readiness question measurable, we design and implement IPI, a
  > typed application protocol and reference interface that places selected
  > J2735 message profiles and pre-encoded payloads beside a correlated CAV
  > service-operation model. We then evaluate necessary communication conditions
  > using separate J2735 functional checks, PC5 custom request/reply probes, and
  > 5G IPI request/accepted-ACK probes on a real autonomous vehicle. These probes
  > measure path response, not semantic execution of a remote CAV service.

At `07_conclusion.tex:21--22`, the design aspiration is valid but should be made
explicitly prospective: **“Application-ready communication must ultimately be
designed and judged by whether it completes the right service within its useful
time and operating conditions.”**

### P1.4 Remove unsupported PC5 signal-strength language

The Introduction says the 2-KiB failure rate rises “as normalized signal
strength decreases” (`01_introduction.tex:134--138`) and later includes “signal
condition” among the factors varied (`:121--123`). In contrast, the setup says
the PC5 device exposed no usable radio metric; its GNSS-joined application proxy
is neither measured radio power nor an RF-quality ranking, and distance is not a
signal-strength estimate (`04_system_design_setup.tex:164--170`). Results
correctly use “as the field path weakened” and decline to attribute route misses
to mobility (`05_experiment_results.tex:76--82,119--120`).

Required changes:

- Replace `01_introduction.tex:136--138` with:

  > However, across the stationary field-point sequence, the failure rate for a
  > 2-kibibyte (KiB) packet rises from 0.2% to 99.2%.

- At `01_introduction.tex:121--123`, replace “signal condition, distance,
  obstruction” with **“field location and route, obstruction, survey-level 5G
  radio context.”** Do not imply a controlled or measured PC5 signal variable.
- At `01_introduction.tex:148--150`, use the Results section’s scoped phrase
  **“location-associated weak-path collections”** rather than “weak-signal
  locations.”

The abstract’s “as the field path weakens” is acceptable because it is a
qualitative application-path description, not an RF attribution.

### P1.5 Qualify certification consistently

The setup gives the exact evidence boundary: the vendor reports that the OBU and
RSU product families passed LTE-V2X protocol-conformance certification, which
supports calling them production, standards-oriented devices but does not
certify measured performance (`04_system_design_setup.tex:172--179`). In
contrast, the Introduction, Related Work, contribution list, and Conclusion call
the measured path itself “certified” (`01_introduction.tex:103--106,160--163`;
`02_related_work.tex:131--133`; `07_conclusion.tex:9--11`).

Required change: in global claims, use **“production, standards-oriented
off-the-shelf LTE C-V2X OBU--RSU path”** or **“a path using vendor-reported
certified product families.”** Do not call the end-to-end measured path or its
performance certified.

### P1.6 Correct the Cell 2 evidence statement

`04_system_design_setup.tex:229--231` states as fact that the follow-up locks the
MG52 to Cell 2. The adjacent table correctly says that the lock remains
operator-reported pending matched exports (`:257`). The retained experiment
summary uses the same boundary.

Required replacement:

> Current evidence associates the original traffic with Cell 2, and the
> operator reports that the follow-up locked the MG52 to Cell 2; matched exports
> for the follow-up are not retained.

No causal comparison should be built from the original/follow-up TDD records;
the rest of the paper already avoids doing so.

### P1.7 State the IPI novelty at the strength of the implemented artifact

The Related Work comparison against VAE, MEC, and Tentacles is substantively
good. It does not claim that those systems lack sessions, delivery, QoS, or
multi-network control. Instead, it identifies IPI’s narrower contribution as a
concrete operation-level application data model spanning selected local J2735
profiles/pre-encoded payloads and correlated CAV service operations
(`02_related_work.tex:16--63,80--101`). Section 3 also candidly states that the
current adapter does not enforce the full lifecycle, generate terminal outcomes,
or bind the facade to PC5 (`03_ipi_protocol_design.tex:231--244,263--270,
297--324`).

The contribution bullet nevertheless says IPI “joins ... profiles with
correlated, stateful CAV services” (`01_introduction.tex:155--158`). That phrase
can be read as an executed, enforced service system. Required change: retain the
user-approved common-protocol contribution but name the concrete novelty:

> We design and implement IPI as a transport-independent application protocol
> and reference interface that places selected J2735 profiles and pre-encoded
> payloads beside a correlated CAV service-operation data model.

Then add a short clause in Section 3’s opening or contribution discussion that
the reference adapter represents but does not enforce the complete service
lifecycle. Do not weaken the contribution to “only a schema,” and do not claim
that IPI is the first protocol to provide sessions, completion state, or
transport independence. Its defensible novelty is the concrete domain data
model and common facade at the J2735/CAV-service integration boundary.

## Reviewer 1: MobiCom networking/systems, novelty, and argument

**Verdict: weak reject pending the P1 integration corrections.**

### Strengths

1. The paper has a clear systems question: a radio link and a low successful-
   packet median do not establish application readiness. The Introduction
   builds from application behavior to direction, deadline, object size, and
   coexistence before stating that question (`01_introduction.tex:4--70`).
2. The two-part contribution is coherent: IPI supplies an application-boundary
   representation, while the field evaluation tests necessary communication
   conditions on two complementary deployed paths. This is MobiCom framing, not
   testbed narration.
3. Related Work is unusually fair to VAE, MEC, Tentacles, AutowareV2X, and prior
   field studies. Tentacles is correctly identified as the closest middleware
   comparison, and the text explains complementarity rather than manufacturing
   an empty “no one has done networking for CAVs” gap.
4. The experiment matrix links application envelopes to payload, direction,
   deadline, load, concurrency, protocol, and interruption. The all-attempt
   denominator plus accepted-response RTT is a credible and reusable systems
   methodology.
5. Section 6 translates the measurements into a research agenda without
   presenting five new “insights” or claiming they are implemented.

### Required revisions

- Apply P1.1--P1.3 and P1.7. Those fixes make the central claim falsifiable and
  keep the protocol novelty distinct from the measurement probes.
- Preserve the current “necessary condition” language from Sections 4 and 5 in
  the abstract, Introduction, and Conclusion. The paper should not alternate
  between application readiness as the target and semantic execution as the
  measured outcome.
- Preserve exactly two contributions and exactly three insights. Do not turn
  the interface gap, evidence gap, or five future directions into additional
  numbered contributions/insights.

### Recommended revisions

- In Related Work Figure 1, change “Complete, fresh, and time-bounded service
  results” to **“Target: complete, fresh, and time-bounded service results.”**
  The caption already says the right-hand capabilities are not all implemented,
  but the visual should communicate that without relying on caption repair.
- Normalize “acknowledgment” to the American spelling. Section 3 currently uses
  “acknowledgement,” whereas the abstract, setup, and Results use
  “acknowledgment.” Also change `05_experiment_results.tex:8`
  “round-trip-time” to **“round-trip time.”**
- Use “private 5G” on first mention and thereafter “5G path,” “5G network,” or
  “5G deployment.” The current mixture of “private 5G” and “private-5G” does not
  change the result, but it weakens terminology discipline.

### Uncertain changes requiring implementation work or new evidence

- A broker-backed session binding, enforced lifecycle transitions, automatic
  completion/rejection, and full correlation checks would materially strengthen
  IPI as a systems artifact. They are not required to make the current paper
  truthful because Section 3 already scopes them out.
- Carrying canonical IPI bytes end to end over PC5 would make the “common
  protocol across paths” story stronger. It would be a new implementation and
  experiment, not a prose correction. Do not imply it already happened.

## Reviewer 2: CAV, CV, ITS, traffic-signal, and application semantics

**Verdict: weak accept on application framing, conditional on P1.2, P1.3, and
P1.7.**

### Strengths

1. The stateless/stateful distinction is useful and mostly precise. BSM, PSM,
   MAP, and SPaT illustrate replaceable state; SRM/SSM illustrate a related
   request/status exchange; larger CAV assistance is modeled as a correlated
   operation (`01_introduction.tex:21--37`).
2. The manuscript does not reduce CAV networking to sensor sharing. Its five
   application envelopes include warnings, automated intersection crossing,
   emergency maneuver/trajectory alignment, cooperative perception/map
   delivery, and fault-triggered remote recovery. These examples expose
   different uplink/downlink, periodic/burst/stream, size, deadline, and
   coexistence behaviors (`04_system_design_setup.tex:424--457`).
3. Safety and authority boundaries are responsible. The IPI control object is
   data rather than authorization; vehicle policy determines use
   (`03_ipi_protocol_design.tex:211--222`). Section 6 preserves traffic-signal
   controller authority and does not let an edge service bypass controller-
   authorized state or safety logic.
4. The standard-message evidence is not inflated in the detailed sections:
   SPaT 10/10 and bidirectional BSM observation are functional samples, not
   reliability or full interoperability tests (`04_system_design_setup.tex:
   181--200`; `05_experiment_results.tex:19--56`).
5. The detector workload is described as a size model/opaque replay, not as
   native serialization of a deployed perception service. Large 1--2 MiB
   objects are chunks, not an end-to-end map or point-cloud completion claim.

### Required revisions

- Replace “this interoperability” in Results as required by P1.3. Nothing in the
  current evidence establishes external IPI interoperability, full J2735
  conformance, MAP/SRM/SSM wire interoperability, or semantic execution.
- Keep “service-operation model” distinct from an enforced state machine. The
  current implementation can represent request/update/complete/reject, but the
  adapter does not generate completed/rejected outcomes or universally enforce
  correlation and transition order.
- Ensure every global description states that PC5 performance is a custom echo
  after separate standard-message checks. Otherwise a traffic-signal or ITS
  reader will reasonably infer that the payload sweep used J2735 callbacks.

### Recommended revisions

- At `01_introduction.tex:24`, use **“the SAE International J2735 message set”**
  once in the main body, matching the abstract’s first-use explanation.
- In Table 3’s “automated intersection crossing” row, retain the explicit “No
  MAP interoperability or plan execution is claimed” boundary. It is essential,
  not an expendable caveat.
- In the conclusion, preserve “necessary communication conditions” so that the
  final sentence about completing the right service is read as the design goal,
  not as a measured application result.

### Uncertain changes requiring implementation work or new evidence

- External J2735/ASN.1 interoperability for MAP, SRM, and SSM; a typed
  diagnostic/recovery schema; and execution of a real planning or recovery
  service would strengthen the application claim. None is present now, and none
  should be silently promoted to a current-paper requirement.
- A real multi-vehicle intersection or recovery-service deployment would test
  semantic usefulness. The current paper can still publish a necessary-
  condition study if it maintains its present scope.

## Reviewer 3: cellular, RAN, C-V2X, and measurement method

**Verdict: weak reject pending P1.4--P1.6 and the path-separation corrections.**

### Strengths

1. The radio/network scope is far more complete than a typical application
   paper. The manuscript identifies the n48 TDD configuration, 40-MHz channel,
   30-kHz SCS, cells, CBRS status, configured power/EIRP, antenna product
   pattern, unresolved installation values, gateway placement, one-UE boundary,
   local endpoint, and unavailable QFI/PRB/MCS/BLER evidence.
2. It correctly separates configured capability from measured per-request
   behavior and operator records from exported configuration. It does not infer
   rank, modulation, bearer treatment, scheduler behavior, or RF causality from
   application traces.
3. The deployment boundary is explicit: no independent background UEs,
   handover, roaming, public-network users, geographic backhaul, or operator
   scheduling policy. Logical clients share one host and one UE, so the test is
   aggregate application demand, not fleet contention.
4. The path metric is methodologically sound. All sender attempts remain in the
   availability denominator; RTT percentiles use accepted responses only;
   stopped and partial runs remain visible; endpoint clocks are not used for
   unsupported one-way delay claims.
5. Direction, offered load, achieved host rate, application foreground,
   protocol, logical-client demand, restart, and packet marking are kept
   separate. The paper correctly says that application labels producing TOS
   `0x0` do not prove a nondefault bearer.

### Required revisions

- Apply P1.4: there is no measured or normalized PC5 received-signal metric.
- Apply P1.5: vendor-reported product-family conformance certification is not
  certification of the measured path or its performance.
- Apply P1.6: the follow-up Cell 2 lock is operator-reported pending matched
  exports.
- Apply P1.2: the PC5 custom measurement message is not J2735 and is not an IPI
  facade binding. The global figures and synthesis must say so.

### Recommended revisions

- Retain “field-point sequence,” “route,” “field path,” or “location-associated
  weak-path collection” for PC5. Reserve RSRP/RSRQ/SNR terminology for the 5G
  survey that actually records those quantities.
- Retain the current statements that TDD records are context rather than a
  controlled causal comparison and that the 25-Mbit/s uplink value is offered
  generator traffic rather than achieved foreground throughput.
- In Figure 3, the path labels are readable in the current PDF but tightly
  packed. After relabeling the experiment host, verify that “vendor API,” “LTE
  C-V2X PC5,” and “NR Uu” remain visually distinct.

### Uncertain changes requiring implementation work or new evidence

- Matched per-request PRB/MCS/BLER/QFI/bearer exports, verified Cell 2 follow-up
  exports, a controlled antenna-placement study, multiple independently
  scheduled UEs, and public-network/handover trials would enable causal radio
  conclusions. They are not required for the present application-boundary paper
  and should not be demanded through prose alone.
- The current PC5 device lacks exposed lower-layer configuration and RF metrics.
  A new PC5 platform or vendor telemetry would be necessary to attribute the
  field pattern to signal strength, interference, MCS, resource pools, or
  mobility.

## Panel synthesis

### Consensus must-change items

1. Define support as a measured necessary communication condition at the first
   central question and carry that boundary through the Conclusion.
2. Remove every implication that IPI was evaluated as the same protocol over
   PC5 and Uu. Distinguish J2735 functional callbacks, PC5 custom echoes, and 5G
   decoded accepted IPI ACKs in prose and figures.
3. Replace “complete application behavior,” unqualified “complete exchanges,”
   “object completion,” and “this interoperability” with the measured
   path-response/accepted-ACK language.
4. Remove “normalized signal strength” and other unsupported PC5 RF language.
5. Qualify conformance certification as vendor-reported product-family evidence,
   not certification of the measured path or result.
6. Qualify the follow-up Cell 2 lock as operator-reported pending matched
   exports.
7. State IPI’s novelty as a concrete operation-level application data model and
   reference interface, while preserving the fact that the current adapter does
   not enforce the full service lifecycle.

### Disputed or uncertain items requiring user approval

1. **Implementation expansion:** enforcing session/correlation transitions and
   generating terminal complete/reject states would strengthen Contribution 1,
   but it would expand the artifact and likely the experiment plan.
2. **End-to-end IPI over PC5:** this would strengthen the common-interface
   narrative, but current evidence does not include it. The paper can instead
   remain a common-criterion comparison of path-specific probes.
3. **New radio evidence:** multi-UE, matched scheduler telemetry, public-network
   load/handover, and a radio-observable PC5 device would deepen causal claims.
   The current scoped conclusions do not require them.
4. **Exact novelty ambition:** the current defensible claim is a concrete domain
   data model spanning selected J2735 representations and correlated general CAV
   service operations. A stronger claim that no standard/platform defines any
   comparable common interface would require a standards-completeness argument
   beyond the present manuscript.

### Comments that should not be followed

The panel explicitly rejects the following possible review reactions because
they would conflict with the evidence or approved paper structure:

1. Do not frame Edge4AV as a new testbed, a Mocar system, or a universal
   technology limit. Mocar is a vendor; Edge4AV is the paper title.
2. Do not merge PC5 and Uu into a hybrid path or claim automatic fallback/path
   selection. The paths are measured independently.
3. Do not report one-way latency from unsynchronized clocks or reinterpret RTT
   as one-way SPaT freshness.
4. Do not turn logical clients behind one UE into independent vehicles or claim
   reproduced public-network congestion, roaming, handover, or carrier policy.
5. Do not infer PC5 RF power, MCS, interference, resource pools, or mobility
   causality from the route proxy.
6. Do not claim that 5G accepted ACKs are completed planning, perception,
   control, recovery, or safety outcomes.
7. Do not call the detector-size replay native detector serialization or the
   1--2 MiB transfers full point-cloud/map completion.
8. Do not add more contributions or insights. The five future directions are a
   research agenda, not five additional findings.
9. Do not weaken Insight 2 by removing “strictly limited,” and do not weaken the
   supported all-attempt results to vague statements such as “may degrade.”
10. Do not recommend cutting content merely to meet a page limit; page count was
    explicitly excluded from this review.

## Quantitative and denominator audit

All cross-section quantitative teasers match the current experiment summary and
the detailed Results. No arithmetic or denominator contradiction was found.

| Claim | Manuscript locations | Audit result |
|---|---|---|
| Favorable stationary PC5 compact reply availability is 99.8--100% in 1,000-attempt conditions | Introduction `:134--138`; Results `:70--82`; Figure 5(a) | **Pass.** The 2-KiB favorable point is 998/1,000; other cited compact favorable conditions reach 1,000/1,000. |
| PC5 2-KiB failure rises from 0.2% to 99.2%; next location has 910/910 observed failures | Abstract `:25--27`; Introduction `:136--139`; Results `:70--82` | **Numbers pass.** Replace only the unsupported “normalized signal strength” explanation. Partial/stopped phases remain in the denominator and are marked. |
| Four PC5 route all-attempt availabilities are 76.3%, 73.1%, 59.1%, and 71.6% | Abstract `:26--28`; Introduction `:139--140`; Results Section 5.3/Figure 5(c) | **Pass.** Counts are 515/675, 731/1,000, 591/1,000, and 716/1,000. Low RTT is conditional on received replies. |
| SPaT/state-mirror has 2,000/2,000 eventual ACKs, 1,068 by 100 ms, hence 53.40% available and 46.6% missing at 100 ms; none missing by 500 ms | Introduction `:142--145`; Results `:29--35`; Figure 6 | **Pass.** This is a request/accepted-ACK RTT screen, not one-way SPaT delay. |
| 128--512 KiB group has 18,000 attempts and 88.36% within 500 ms, hence 11.64% misses | Introduction `:145--147`; Figure 6 | **Pass.** Aggregation spans heterogeneous experiments and is labeled as such. |
| Every 1- or 2-MiB request-to-accepted-ACK RTT exceeds 500 ms | Abstract `:28--31`; Introduction `:146--147`; Figure 6 | **Pass.** The group contains 14,000 attempts and zero accepted ACKs by 500 ms; 37.39% are accepted by 1 s and all eventually return under the recorded sender condition. |
| J2735 functional sample: SPaT 10/10 and BSM observed in both directions | Setup `:181--185`; Results `:19--27`; Table 5 | **Pass with stated boundary.** This is functional coverage, not reliability, formal conformance, or cross-vendor interoperability. |
| 5G functional sample: 2,000/2,000 accepted IPI ACKs; local IPI accepts listed 5,262--60,000-B objects | Results `:19--35`; Table 5 | **Pass with stated boundary.** Neither is semantic application execution. |
| Competing uplink traffic, logical clients, protocol, QoS-label, and restart results use all-attempt availability while p50/p95/p99 use accepted responses | Section 5 opening, Figures 7--9, and Section 5.9 | **Pass.** Offered generator rate is kept separate from achieved host rate; logical clients share one UE; restart gaps are not labeled fallback. |

The paper’s denominator convention is consistent: availability includes every
sender attempt; response RTT distributions exclude missing/rejected attempts and
are shown beside the denominator. This is a major strength and should not be
changed.

## IPI capability audit against the current implementation and tests

| Manuscript capability | Current artifact evidence | Audit result |
|---|---|---|
| Common facade for selected local J2735 profiles, opaque pre-encoded payloads, and CAV service operations | C++17 core/V2X/application layers and facade described in Section 3; seven CTests pass | **Supported at reference-model level.** |
| BSM, PSM, MAP, SPaT, SRM, and SSM lightweight profiles | Types exist, but the profiles are local project encodings, not the formal SAE ASN.1/UPER schema | **Supported only with the manuscript’s current qualification.** |
| Pre-encoded J2735 preservation | Opaque byte path exists; opaque bytes are not decoded | **Supported.** Do not convert this into external interoperability. |
| Correlated stateful operation vocabulary | Types represent session/correlation identifiers and request/update/complete/reject states | **Supported as a data model.** The current adapter does not enforce the lifecycle or all correlations. |
| Session lifecycle | Registration, heartbeat, invocation, telemetry, and response retrieval are exercised | **Partially supported.** In-memory only; no broker-backed session binding, universal active-session check, lease expiry, automatic completion/rejection, or transition enforcement. |
| Response/ACK semantics | Adapter returns accepted/in-progress; experiment receiver decodes and accepts probe frames | **Supported as interface acceptance only.** `serviceSuccess` is caller supplied plus ACK acceptance, not an independently observed service outcome. |
| PC5 transport binding | No facade-to-PC5 binding; PC5 performance uses a separate custom vendor-packet echo | **Not implemented.** Global prose/figures must not imply otherwise. |
| MQTT session transport | MQTT latency probe uses separate request/ACK topics | **Not a broker-backed IPI session.** Current Section 3 says so correctly. |
| Formal J2735/SAE conformance and cooperative regional extension | Standard PC5 device stack handles selected standard messages; IPI cooperative object is a research-format proposal | **Not established.** The current manuscript generally scopes this correctly. |
| Semantic planning, perception, control, or recovery completion | No executed remote application result in the communication experiments | **Not measured.** Accepted ACK must remain separate from semantic completion. |

The capability audit finds no hidden implementation contradiction inside Section
3. The contradiction is cross-sectional: the abstract and detailed method are
precise, while a few Introduction, Related Work, figure, and Conclusion phrases
make the artifact sound more integrated than it is.

## Contribution and insight audit

**Pass, with P1 wording calibration.**

- The Introduction contains exactly **two** contribution bullets
  (`01_introduction.tex:153--164`): 1) the common IPI application protocol and
  reference interface; 2) the application-driven field evaluation. The
  Conclusion restates those two contributions in prose. No third contribution
  is introduced.
- The Introduction and Section 5.9 each contain exactly **three** insight
  bullets, with matching approved titles:
  1. current direct V2X paths cannot by themselves support dependable
     large-object or multi-step complex CAV exchange;
  2. today’s 5G can support some complex exchanges, but usable data size is
     **strictly limited** by time budget and operating conditions;
  3. collaborative CAV applications need methods beyond current V2X and 5G used
     independently and as-is.
- The abstract states the same three findings in the same order. The Conclusion
  synthesizes the same operating-envelope logic without adding a fourth
  insight. Section 6 explicitly says its five directions are not additional
  insights (`06_future_research_directions.tex:4--11`).

The only required calibration is to define “support” before the Introduction’s
insight list and to keep the evidence at the path-response/necessary-condition
level. The approved titles themselves should remain unchanged.

## Cross-section consistency audit

| Topic | Abstract | Introduction | Method/Results | Conclusion | Decision |
|---|---|---|---|---|---|
| Central question | Complete exchange before expiry | Same | Operationalized by object/deadline/direction/availability | Same | **Pass after support definition is moved earlier.** |
| Stateless vs stateful | Replaceable update vs correlated service | Expanded with J2735/CAV examples | Reflected in IPI model and workload table | Condensed | **Pass.** |
| PC5 method | Explicit custom sequence-matched probe | Currently blurred in global summary | Explicit custom echo separate from J2735 callbacks | Currently blurred | **P1.2.** |
| Uu method | Decoded accepted IPI ACK | Described less explicitly | Explicit accepted ACK, not semantic result | Currently “complete exchanges” | **P1.2/P1.3.** |
| PC5 field variable | “field path weakens” | Incorrect “normalized signal strength” | No usable PC5 radio metric | “field path weakens” | **P1.4.** |
| 5G scope | Measured Uu path | One UE/logical clients/unsynchronized clocks | Detailed private deployment; no public-network equivalence | “measured 5G path” | **Pass.** |
| IPI novelty | Typed selected J2735 representations plus correlated operations | Common protocol | Concrete data model; lifecycle not enforced | “stateful services” can overread | **P1.7.** |
| Three insights | Same order and meaning | Exact approved bullets | Exact approved bullets with support definition | Same synthesis | **Pass.** |

## Rendered-PDF and reference audit

- The current `main.pdf` renders on US Letter with embedded fonts, no visible
  clipping, no figure/table overlap, and no embedded hyperlinks/annotations.
  Page 1 is readable: title, anonymous author line, abstract, and the start of
  the Introduction fit cleanly in two columns. Figures 5--9 and the Conclusion
  are also readable at rendered-page scale.
- The build log has no undefined citations/references, LaTeX errors, or overfull
  boxes. Underfull boxes occur mainly in dense tables and the bibliography but
  do not obscure content.
- One pre-submission editorial correction remains: every running header shows
  **“MobiCom ’26, ,”** because `main.tex:16` supplies empty conference fields.
  Fill the venue date/location fields required by the official call or suppress
  the incomplete conference header in the anonymous-review configuration. This
  is not a page-count recommendation.
- The references cover the standards and systems used in the novelty argument,
  including SAE J2735, 3GPP VAE/Uu/PC5, ETSI MEC/ITS, Tentacles, AutowareV2X,
  PC5/5G field studies, application requirements, and workload datasets. This
  review did not re-browse or independently revalidate each external source; it
  audited internal claim/citation alignment only.

## Consolidated vote and adjudication checklist

### Vote

- Reviewer 1, networking/systems: **Weak Reject**
- Reviewer 2, CAV/CV/ITS: **Weak Accept, conditional**
- Reviewer 3, cellular/RAN/measurement: **Weak Reject**
- Panel: **Weak Reject now; Weak Accept after the P1 corrections. No new
  experiment is necessary for that change in vote.**

### Concrete adjudication checklist

- [ ] Define “support” as a measured necessary communication condition in the
      Introduction immediately after the central question.
- [ ] Make every overview and figure distinguish J2735 functional checks, PC5
      custom sequence echoes, and 5G decoded accepted IPI ACKs.
- [ ] Replace “complete application behavior,” unqualified “complete
      exchanges,” “object completion,” and “this interoperability” as specified.
- [ ] State in the Conclusion that the probes do not measure semantic remote
      service execution.
- [ ] Replace the Introduction’s PC5 “normalized signal strength” and
      “weak-signal” wording with field-point/route/weak-path language.
- [ ] Qualify product certification as vendor-reported product-family
      conformance evidence, not measured-path certification.
- [ ] Qualify the follow-up Cell 2 lock as operator-reported pending matched
      exports.
- [ ] State IPI’s novelty as the concrete J2735/CAV-service operation data model
      and common reference interface; do not imply an enforced lifecycle.
- [ ] Preserve exactly two contributions and exactly three approved insights,
      including “strictly limited” in Insight 2.
- [ ] Preserve all-attempt availability denominators and accepted-response-only
      RTT percentiles.
- [ ] Normalize acknowledgment spelling and “round-trip time”; clean up
      private-5G/private 5G terminology.
- [ ] Correct the incomplete “MobiCom ’26, ,” running header.
- [ ] Rebuild and confirm no undefined citations, references, overfull boxes,
      embedded hyperlinks, author metadata, figure overlap, or clipping.

## Reviewed artifact hashes

```text
6b1edf812590e8703a4a4a74caefc32593e94727aba60e3d5b1835d6b15115c3  main.tex
0bf6631f74c3b1ca00051d7e561bc0ebd26efcf1e5dc54ac9a6640c31d43eca9  sections/00_abstract.tex
71a427ecb0f2f4977086c8aa4429b06397771362547aba8bc5675fbc835f515e  sections/01_introduction.tex
f3ec8e2ea6e84234da4de22c8d2a6cc6ff155c7901a0e9830d730961e8957007  sections/02_related_work.tex
43bd5738d1ca8fa4af0a6818a9ffb0f5e347cf51c6768b1bd25a3fabb418ba10  sections/03_ipi_protocol_design.tex
7cb2244b5a980c87ea5681840e26cdc2638b9306b5d0236d5d699d723a5d8626  sections/04_system_design_setup.tex
d234b6957446cf76acac69ead0d1a9d5c38e1f528e650217c87ad4a5cfb7de6a  sections/05_experiment_results.tex
eb73a68dbe36e5357e4fbd32f78cd6110251e9a5f9aa3c8b2d4ce57ba082da73  sections/06_future_research_directions.tex
a44fb9ba6b2b689298df0abff6133a2f41600c00591ad6cbbf474b6ae54f3407  sections/07_conclusion.tex
bc29225c9b14537b061cc5856c1b592bda917d48631e74fb36cd7582aeb010f9  references.bib
91d27901fee64f56d31ba5200033a9092f9f554b9c50ecf39a10c7a8eb4b6404  main.pdf
```

---

## Focused second-pass verdict after integrated revisions

**PASS. No P0 or P1 issue remains from the integrated review.**

This second pass reviewed the revised current sources and rebuilt PDF. It
ignored page limits as directed. The rebuilt PDF has SHA-256
`b2240ec08973a250e1f130dfd88985d01e920c92386963013ee44cfc422a61eb`
and was generated after the last reviewed source change.

- **P1.1 passes:** the Introduction defines “support” at its first central use
  as meeting measured necessary communication conditions and excludes remote
  service correctness and vehicle safety (`01_introduction.tex:59--64`).
- **P1.2 passes:** the Introduction, Related Work, Section 3 transition, Figure
  3, experiment matrix, Results, and Conclusion consistently separate J2735
  functional checks, PC5 custom sequence-matched echoes, and 5G decoded,
  accepted IPI ACKs. They do not claim a shared wire protocol or IPI-over-PC5
  experiment.
- **P1.3 passes:** the manuscript distinguishes received path responses and
  accepted ACKs from semantic service completion. Related Work now reports RTT
  tails “among received path responses,” and the Conclusion explicitly says the
  probes do not measure semantic execution of a remote CAV service.
- **P1.4 passes:** the unsupported PC5 “normalized signal strength” and
  “weak-signal” overview claims are gone. The paper uses field-point, route,
  obstruction, and location-associated weak-path language while reserving
  survey-level radio quantities for 5G.
- **P1.5 passes:** global prose calls the PC5 devices production and
  standards-oriented and attributes protocol-conformance certification to the
  vendor-reported product families. The setup states that this evidence does not
  certify measured performance.
- **P1.6 passes:** the Cell 2 follow-up lock is explicitly operator-reported,
  and the missing matched exports remain disclosed (`04_system_design_setup.tex:
  230--234,260`).
- **P1.7 passes:** IPI novelty is consistently framed as a concrete
  operation-level data model and reference interface spanning selected local
  J2735 profiles/pre-encoded payloads and correlated CAV service operations.
  The text explicitly says that the current adapter represents but does not
  enforce the full lifecycle or generate terminal outcomes.
- **Figure 1 passes:** the center panel says “one all-attempt path-response
  test,” the right panel labels complete/fresh/time-bounded service results as a
  target, and the caption says the paper does not implement every capability on
  the right. The rendered figure is readable with no overlap or clipping.
- **Figure 3 passes:** the host is labeled as the experiment sender/timer; the
  workload box separately names J2735 checks, PC5 custom echoes, and 5G IPI ACK
  probes; PC5 and Uu are visually distinct; the remote MX68CW is shown as
  management-only. The rendered labels and arrows are readable with no overlap
  that changes meaning.
- **Running-header check passes:** the incomplete “MobiCom '26, ,” header is no
  longer rendered. The rebuilt PDF uses page numbers only under the anonymous
  review layout.
- **Contribution/insight check passes:** the Introduction contains exactly two
  contribution bullets and exactly three approved insight bullets. Section 5.9
  contains the same three insights, in the same order and at the same strength,
  including “strictly limited” in Insight 2. Section 6 still states that its
  research directions are not additional insights.
- **No-new-overclaim check passes:** the revision does not promote ACK to
  semantic completion, the PC5 probe to J2735/IPI interoperability, logical
  clients to independent UEs, operator records to verified radio exports, or
  the measured paths to universal V2X/5G limits.
- **Build and PDF check passes:** the current PDF is US Letter, has embedded
  fonts, no hyperlinks, annotations, or embedded files, and no rendered header
  defect. The log has no undefined citations/references, LaTeX errors, or
  overfull boxes; the only matched package message is Hyperref draft mode.

### Revised artifact hashes audited in this pass

```text
f3dada720977a7136e116f21227583b52bd052a9bdb66ab4afd2c66cfedfd2f3  main.tex
0bf6631f74c3b1ca00051d7e561bc0ebd26efcf1e5dc54ac9a6640c31d43eca9  sections/00_abstract.tex
3a3310631c66a4be43ee42e679c2a2a3091a3bfe87f36c24d1fb8a102d5829a4  sections/01_introduction.tex
af79d99cc3a87b7c44aafe6ad57c2be248f45147ef29c3616d7675dad16d4679  sections/02_related_work.tex
9030f5ed6dffc3c20837d4cb62698201c0e9c4fe23fd3fa34c356a12d76f30b3  sections/03_ipi_protocol_design.tex
e93b1452ed455cad5aeab694a89d4a99fceac22fec33c864b423fe2a5a75fc48  sections/04_system_design_setup.tex
8c4305f9a07f42d5ac66cb83a910e65d4c6d658df00b94ed2d4967fd10492037  sections/05_experiment_results.tex
eb73a68dbe36e5357e4fbd32f78cd6110251e9a5f9aa3c8b2d4ce57ba082da73  sections/06_future_research_directions.tex
b7f4fbe6b21d5e4376934f01602bbaad5ac9440edc49c2587c2e488a9b22429a  sections/07_conclusion.tex
bc29225c9b14537b061cc5856c1b592bda917d48631e74fb36cd7582aeb010f9  references.bib
b2240ec08973a250e1f130dfd88985d01e920c92386963013ee44cfc422a61eb  main.pdf
```
