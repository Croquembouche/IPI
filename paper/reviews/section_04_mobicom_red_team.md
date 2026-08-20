# Section 4 MobiCom Red-Team Review

**Review target.** `paper/current_manscript/sections/04_system_design_setup.tex`, rendered in `paper/current_manscript/main.pdf` (18 pages; Section 4 spans PDF pp. 10--17).

**Review basis.** I read the binding Section 4 outline (`paper/paper_outline.md`, especially lines 1724--1733, 2797--2844, 2911--3138, and 3155--3173), `experiment_summary.md`, `current_task.md`, current result/config artifacts, and the current C++ probe, sender, receiver, and test sources. I also rendered and visually inspected the Section 4 PDF pages. This is a source-and-artifact review, not a new standards or web-literature survey.

## Bottom line

Section 4 has a strong high-level boundary: it measures two independent access modes, explicitly refuses to equate one UE with a public network, treats RTT as distinct from one-way latency, and avoids turning the setup section into a results/insights section. Those are real strengths for a MobiCom paper.

However, it is **not MobiCom-ready as submitted**. Three measurement descriptions materially exceed the current implementation: the 5G RTT clock is described as monotonic although the code uses `std::chrono::system_clock`; an accepted probe ACK is described as a complete, correlated service result although it is a syntactic parser acknowledgment and the sender does not validate its correlation fields; and the detector workload is described as serialized detector output although it is a modeled opaque byte payload sized from prediction counts. A fourth high-priority mismatch maps a MAP function to the PC5 evidence even though the retained hardware validation is SPaT plus BSM, not MAP.

These are repairable without changing the paper's central scope, but the paper must either (a) narrow the prose and all result interpretations to the currently measured request/ACK probe, or (b) implement the stronger semantics and recollect affected data. The Radio/RAN reviewer also requires the Cell 2 and stronger/weaker-location descriptions to retain their current operator-reported/pending-export qualification everywhere, including Table 4.

| Reviewer | Vote now | Conditional vote after the must-change items | Core reason |
|---|---|---|---|
| A. Radio/RAN/private-5G measurement | Weak Reject | Weak Accept | Careful configuration scope overall, but table-level Cell 2/location language outruns the pending ACP/MG52 evidence. |
| B. C-V2X/ITS/CV/CAV requirements | Weak Reject | Weak Accept | The application-envelope framing and arithmetic are good, but MAP and detector-workload provenance must be made exact. |
| C. Networking/systems experimental methodology | Reject | Weak Accept | The advertised timing, correlation, and completion quantities do not currently match the 5G harness implementation. |
| **Consolidated** | **Weak Reject** | **Weak Accept** | Correct the P0/P1 claim mismatches, make Tables 3--4 readable, and keep unmeasured expansions explicitly out of scope. |

There is no fatal problem with the two-path experimental idea itself. The fatal-to-claim issue is the gap between the paper's *complete correlated application response / monotonic RTT* language and the actual 5G probe semantics.

## Evidence snapshot and affirmative findings

The following claims are supported and should be retained.

1. **Two-path boundary passes.** Section 4 states that PC5 and Uu are independently measured; no request crosses from PC5 to Uu or vice versa, and there is no selector, aggregation, or automatic fallback (`04_system_design_setup.tex` lines 32--38). It also correctly limits the private deployment to one UE and explicit load/logical-client controls, rather than claiming a public-network replica (lines 40--47). This follows the binding scope rule in `paper_outline.md` lines 3130--3138.

2. **Private-5G configuration is mostly well scoped.** The configuration/capability distinction is explicit at lines 213--224, 244--264, 266--283, and 299--313. The Airspan model, n48, 40 MHz, 30 kHz, SSB period, layer cap, 256-QAM capability, CBRS values, antenna caveats, DNN, default 5QI 9, unavailable QFI, and no per-request RAN counters have corresponding evidence statements in `current_task.md` lines 266--342 and 430--459. The prose correctly avoids claiming per-request MCS/rank/QFI/scheduler behavior.

3. **Public-network scope passes.** Lines 40--47 do not overclaim public-network realism. The section correctly excludes independent background UEs, admission/scheduling policy, handover, roaming, and geographic backhaul. This is exactly the positive, controlled-proxy framing the outline requires.

4. **One-way versus RTT intent is correct, except for the clock-source wording.** The manuscript properly says unsynchronized clocks preclude one-way 5G latency (lines 130--136), says direction controls do not convert RTT into one-way latency (lines 361--365), and says a round-trip marker cannot certify a one-way standard requirement (lines 378--391 and 538--546). `experiment_summary.md` lines 21--30 and 320--326 independently require that boundary.

5. **PC5/J2735 claim scope is mostly careful.** The custom RTT probe is explicitly not a standardized J2735 message (lines 185--198), and the unavailable lower-layer PC5 parameters are listed rather than inferred (lines 200--207). The actual setup evidence supports 10/10 SPaT forwarding/OBU callbacks and observed bidirectional BSM reception: `results/mocar_v2x/20260703_setup_test/summary.md` lines 80--103. It also supports the `mde_v2x_custom_send()` no-op and the `v2x_packet_data_send(..., 0x1b)` workaround (`experiment_summary.md` lines 394--403).

6. **Application-envelope arithmetic checks out.** At 25 mph, 11.18 m/s times 100/10/3 ms is 1.118/0.1118/0.0335 m, consistent with lines 433--442. The 46.3 m stopping-distance calculation is consistent with \(v(2.5)+v^2/(2\cdot3.4)\), and the 0.45 m relative-gap and 0.278/0.556 m low-speed values are arithmetically correct. Citation keys used for the table and its explanatory prose are present in `paper/current_manscript/references.bib`. The manuscript also correctly calls the VSC values design targets rather than regulations (lines 406--412).

7. **Workload provenance has real artifacts, once accurately labeled.** The 922-sample V2X-Radar detector run exists (`experiment_summary.md` lines 613--633). The estimated payload summary exists in `results/real_5g/20260702_detector_output_to_ipi_run_1/detector_output_ipi_payloads.json`, with 256 base bytes, 96 bytes per predicted box, and the stated 13,216/19,648/22,816/23,968/25,024 byte statistics. The 22,431-file manifest and category statistics also exist in `results/v2x_benchmarks/latest/v2x_ipi_payload_manifest.json`.

8. **The seven-family matrix is substantively complete.** Families A--G in lines 567--573 cover functional checks, object scale, coverage/mobility, mixed load/direction, logical-client demand, protocol/fragmentation, and deadline/QoS/interruption. The procedural text at lines 578--624 preserves most needed controls, counts, timeouts, partial runs, and all-attempt denominators. Its content tracks the binding outline's matrix requirements (`paper_outline.md` lines 2885--3069).

9. **Section 4 obeys the no-results/no-insights rule.** It reports only setup facts and allowed functional/configuration checks (for example, the SPaT/BSM and 10/10 bring-up checks). It does not state the three insights or present a performance conclusion. The future-facing sentence at lines 626--632 is an appropriate transition to Section 5. This passes the binding prohibition in `paper_outline.md` lines 3147--3153 and 3170--3173.

## Consensus must-change checklist

The first four are claim-correctness blockers. The remaining P1 items should be completed before submission because they change what experiment evidence a reviewer believes exists.

### P0-1 — Correct the 5G clock-source claim

**Problem.** Lines 130--136 call the 5G sender-side RTT a ``monotonic clock'' measurement. Current code does not do that. `current_unix_time_ns()` uses `std::chrono::system_clock` in `cpp/src/api/private_5g_latency_probe.cpp` lines 366--369. The sender timestamps the request with it (`cpp/examples/library/private_5g_latency_sender.cpp` lines 517--529), and `compute_private_5g_latency_metrics()` subtracts those wall-clock values (`cpp/src/api/private_5g_latency_probe.cpp` lines 371--385). UDP follows the same pattern (`cpp/examples/library/private_5g_latency_udp_sender.cpp` lines 556--568). The 5G metric is same-host and avoids *endpoint* synchronization, but it is not monotonic and is sensitive to a local wall-clock adjustment.

**Why this matters.** A MobiCom reviewer will treat a monotonic sender RTT as a fundamental instrumentation invariant. Calling a `system_clock` subtraction monotonic makes the timing-method claim objectively false even if no observed run happened to experience a clock adjustment.

**Safe prose replacement for lines 130--136.**

> The vehicle and edge clocks were unsynchronized in most 5G runs. One follow-up also recorded a large endpoint-clock offset. We therefore report same-host sender-side round-trip elapsed time from the sender's send and receive timestamps. This measure does not depend on edge-clock synchronization, but the current harness obtains its timestamps from the host wall clock; it is not used to derive one-way latency and is interpreted subject to any local clock adjustment during a run. Stored uplink and downlink timestamps remain diagnostic fields. The timed path begins with an already encoded object and ends when the harness receives an accepted acknowledgment; it excludes sensing, detector execution, planning, vehicle actuation, and human response.

**What requires implementation/experiments.** To retain the word *monotonic*, change the sender timing path to an injected monotonic/steady clock, preserve that value locally rather than treating it as a cross-host epoch, add regression tests, rebuild the deployed sender, and recollect any data used for a monotonic-clock claim. Narrowing the prose is sufficient only if the paper accepts the current wall-clock timing limitation.

### P0-2 — Replace “complete, accepted, correlated service response” with the actual 5G request/ACK measurement

**Problem.** The following terms overstate what the 5G probe establishes: ``accepted, correlated reply'' at lines 134--136; \(N_{\mathrm{complete,accepted,correlated}}\) in lines 378--385; ``receiver interprets and correlates the complete object'' as a measured condition at lines 393--402; ``correlate'' in lines 345--350; and ``correlated reply'' at lines 361--365 and 616--618.

The current 5G ACK contains only sequence, sender timestamp, server timestamps, frame type, payload size, `accepted`, and diagnostic detail (`cpp/include/ipi/api/private_5g_latency_probe.hpp` lines 16--43; encoder/decoder at `cpp/src/api/private_5g_latency_probe.cpp` lines 306--340). It carries neither `requestId` nor `sessionId`. The receiver sets `accepted=true` when `inspect_private_5g_probe_frame()` successfully decodes the frame (`cpp/examples/library/private_5g_latency_receiver.cpp` lines 168--186); it does not execute guided planning, create a service result, inspect opaque `offloadPayload` bytes, or enforce a session transition. The sender accepts the next decoded ACK without checking `ack.sequence` or `ack.clientSendTimeNs` against the outstanding request (`private_5g_latency_sender.cpp` lines 510--529; MQTT lines 552--576; UDP lines 556--568). `serviceSuccess` is additionally an operator-supplied context annotation whose default is true, not an observed service completion (`cpp/include/ipi/api/experiment_logging.hpp` lines 20--27 and sender lines 418--435).

The PC5 custom RTT harness is different: it uses an echoed message and checks the expected sequence before signaling a reply (`third_party/mocar/J2735-2020/samples/ipi_custom_rtt/ipi_custom_rtt.c` lines 339--400). Do not use that stronger PC5 behavior to describe the 5G probe.

**Safe replacement for the equation and immediately following text (lines 378--391).**

> For a selected analysis budget \(B\), the current 5G harness supports deadline-qualified acknowledgment availability:
> \[
> A_{\mathrm{ack}}(B)=
> \frac{N_{\mathrm{accepted\ ACK},\;\mathrm{sender\ RTT}\leq B}}
> {N_{\mathrm{sender\ attempts}}}.
> \]
> A timeout, missing ACK, rejected frame, or ACK received after \(B\) is a miss. RTT percentiles use only attempts with an accepted ACK and are always reported beside the all-attempt denominator. The ACK confirms that the receiver decoded the probe frame and emitted its acknowledgment; it is not a completed guided-planning result, semantic validation of an opaque application payload, or enforced session/request correlation. For a one-way periodic message, the correct quantity would instead be the fraction of updates whose age at the consuming application is at most \(B\). The current instrumentation does not measure that quantity. Hence, a full RTT is labeled an acknowledged-cycle comparison rather than one-way broadcast latency.

**Safe addition after the application-support conditions (lines 393--402).**

> These are application-readiness conditions, not claims that every condition is directly measured by the present probe. In particular, the 5G experiments measure frame decoding and acknowledgment return, not execution or validation of an individualized planning/recovery result.

**What requires implementation/experiments.** A genuine complete/correlated service metric requires an ACK/result that carries and is validated against a request ID, session ID, and result identity; sender rejection of mismatches/stale replies; a service handler that produces a semantic result; and tests for mismatched sequence/request/session, duplicate ACK, late ACK, and semantic failure. Once changed, re-run the 5G conditions used to support a complete/correlated-service claim. No prose fix can turn existing parser ACKs into completed planning or recovery cycles.

### P0-3 — Correct detector-workload provenance and opaque-payload semantics

**Problem.** Lines 324--332 state that the experiment ``serialize[s] each detector result as a 256-byte header plus 96 bytes per predicted object.'' That is not what the current artifact does. `scripts/build_detector_output_ipi_payloads.py` lines 32--50 calculates `base_bytes + pred_boxes * bytes_per_box`; its default values are 256 and 96 (lines 70--100), and its own note calls the sizes *estimates*. The sender then constructs a `GuidedPlanning` IPI envelope and fills `offloadPayload` with repeated sequence-byte data (`cpp/examples/library/private_5g_latency_sender.cpp` lines 258--281; UDP equivalent lines 296--319), rather than serializing native detector boxes. The receiver decodes the outer canonical IPI message but does not decode the opaque `offloadPayload` (`cpp/src/api/private_5g_latency_probe.cpp` lines 226--252; `cpp/src/core/ipi_cooperative_service.cpp` lines 228--239).

**Safe replacement for lines 324--332.**

> For a communication workload derived from the official V2X-Radar radar-only late-fusion checkpoint, we process all 922 samples in the staged cooperative validation split. We estimate a detector-output payload as a 256-byte fixed header plus 96 bytes per predicted object. This is an application-level size model derived from the checkpoint's prediction counts, not the detector's native serialization or a standardized perception-message encoding. The resulting minimum and median sizes are 13,216 and 19,648 bytes; the 95th, 99th, and maximum sizes are 22,816, 23,968, and 25,024 bytes. The network replay carries opaque byte payloads of representative sizes plus a separate 60,000-byte stress condition; the latter is not a detector percentile. Detector execution runs offline on a separate four-GPU host and is excluded from communication RTT.

**What requires implementation/experiments.** To say that detector results are serialized, select and document a concrete detector-result schema, serialize the detector outputs under that schema, validate the decoder at the receiver, and rerun the workload. This is a material design choice and should not be assumed without author approval.

### P1-4 — Remove the unsupported MAP-function mapping and disambiguate PC5 versus 5G object probes

**Problem.** Table 3 line 426 says ``SPaT/MAP function,'' but the retained hardware evidence shows 10/10 SPaT bridge/OBU callbacks and bidirectional BSM reception, not MAP hardware serialization or interoperability. `results/mocar_v2x/20260703_setup_test/summary.md` lines 80--93 is the direct evidence. The generic C++ codec has a MAP type, but a unit or loopback codec path is not PC5 MAP function evidence. In the same cell, ``256/512-B and 1-KiB IPI planning cycles'' obscures that PC5 uses a non-J2735 custom echo while 5G uses the IPI probe.

**Safe Table 3 replacement for the final cell in the automated-intersection row.**

> SPaT functional check; 256/512-B PC5 custom acknowledged cycles; 1-KiB 5G IPI-envelope acknowledgment probe; logical-client load proxy. No MAP-over-PC5 interoperability or individualized plan execution is claimed.

**Related Table 4 repair.** In line 567, split the compact-object wording by path, for example:

> PC5: BSM/SPaT functional checks and 0--2-KiB custom echo objects. 5G: 0--4-KiB IPI-envelope acknowledgment probes over TCP/MQTT.

This is prose/table correction only. A MAP interoperability claim would require a new MAP encode/send/receive validation on the actual OBU/RSU path.

### P1-5 — Keep Cell 2, TDD follow-up, and field-location labels conditional everywhere

**Problem.** The surrounding prose is careful: line 227 says evidence *associates* original traffic with Cell 2, and lines 266--274 call historical/follow-up comparison descriptive. But Table 2 line 255 says without qualification that ``Original and locked follow-up traffic use Cell 2,'' and Table 4 lines 570--571 calls locations ``stronger/weaker.'' Current evidence is not paper-facing confirmation of those radio conditions. The C1--C4 follow-up manifest sets `paper_facing_state` to `blocked_pending_acp_mg52_alignment`; its validation says the ACP/MG52 artifacts are absent and lists Cell 2/radio-class confirmation as a blocker (`results/real_5g/20260806_airspan_followup_run_1/run_manifest.json` and `validation_summary.json`). `current_task.md` lines 77--85 says the initial stronger label was corrected to `medium_typical_deployment`, with the radio class still operator-reported. `experiment_summary.md` lines 320--376 makes the same association-not-causation boundary explicit.

**Safe Table 2 replacement for the Cell 1/Cell 2 interpretation boundary (line 255).**

> Current records associate original traffic with Cell 2. The Cell 2 lock reported for the follow-up remains operator-reported pending matched ACP and MG52 exports; Cell 1 is not a second application path.

**Safe Table 4 replacements (lines 570--571).**

> `One host and UE; operator-reported location classes; separate direction runs`

and

> `1-KiB, maximum nominal 5 requests/s/client, one UE; historical and repeated endpoint runs at operator-reported location classes`

Use ``candidate-weak,'' ``common/typical,'' and ``candidate-strong'' only when discussing the named follow-up blocks, never as verified RSRP, serving-cell, or scheduler categories until matched exports are available.

**What requires external evidence.** Matched ACP Cell 2 and MG52 exports aligned to collection bins, plus a same-cell 40/40/20 reference if a causal profile comparison is desired. Do not silently convert this into a new experiment without user authorization.

### P1-6 — Correct the logical-client request-rate description

**Problem.** Lines 597--599 say that each client offers five sequential requests per second and therefore a maximum nominal 500 requests/s. The sender sleeps for the configured interval *after each completed attempt* (`private_5g_latency_sender.cpp` lines 517--543; current task record at lines 87--95). Thus five requests/s/client is an upper bound under negligible response time, not a maintained open-loop offered load; timeout/RTT tails lower the realized rate. This distinction matters precisely under concurrency, where the paper is testing delay/load interaction.

**Safe replacement for lines 594--605.**

> Separately, the logical-client sweep creates 1, 2, 5, 10, 20, 50, or 100 clients. Each client uses sequential requests with a 200-ms post-attempt interval. Five requests/s/client, and hence 500 requests/s at 100 clients, is the maximum nominal rate when reply time is negligible; the realized per-client and aggregate rates can be lower when requests wait or time out. This is homogeneous application demand behind one UE, not a 100-vehicle field test. [...]

The existing logs can support a realized-rate analysis; no recollection is necessary merely to make this wording correct.

### P1-7 — Make payload terminology exact and prevent an on-wire-size misunderstanding

**Problem.** The application table properly says source object sizes exclude protocol/security overhead (line 416), but the workload text calls nominal 0/256/1,024/4,096 byte values ``application payload'' without explaining that the measurement logs record larger encoded IPI frames and then add a probe envelope/transport headers. A representative TCP artifact logs 63, 334, 1,102, and 4,174 encoded IPI-frame bytes for nominal 0, 256, 1,024, and 4,096 byte offload payloads (`results/real_5g/20260513_sunny_fintechparking_run_1/p5g-tcp-service-payload-*_sender.csv`). The serializer adds the message fields and optional sections (`cpp/src/core/ipi_cooperative_service.cpp` lines 124--239), and the probe request adds metadata (`cpp/src/api/private_5g_latency_probe.cpp` lines 255--303).

**Safe addition after lines 317--322.**

> Throughout this section, a named payload size denotes requested application/offload-payload bytes. The encoded IPI frame, probe metadata, and transport headers add bytes; the experiment logs record encoded frame bytes separately. We use named payload bytes to relate the workload to an application object class and report encoded sizes with the results.

This is a reproducibility and interpretation repair, not a demand to count IP/TCP/TLS or radio MAC overhead in the current study.

## Reviewer A — Radio/RAN/private-5G measurement review

### Assessment

The RAN description is much better than a typical private-5G paper because it differentiates configuration from per-request radio state, tells the reader when an operator record is the evidence source, and lists unavailable counters. The public-network and one-UE boundaries at lines 40--47 are exemplary. Lines 149--159 also correctly treat the 16-point survey as spatial context rather than a propagation model.

The central remaining concern is evidence alignment. The manuscript is strongest when it says the original traffic is *associated* with Cell 2 and when it calls the follow-up operator-reported. It weakens that discipline in Table 2 and Table 4. A table is where many reviewers will extract the experimental premise; it cannot be stronger than the adjacent caveat.

### High-priority findings

- **Cell/condition attribution is not yet verified at paper-facing strength.** See P1-5. This is not a demand for a new RAN theory section. It is a requirement to write the strongest claim only where matched ACP/MG52 evidence permits it.

- **No causal TDD or RAN-efficiency claim should appear later unless a same-cell matched reference and counters exist.** Lines 266--283 avoid this correctly. Retain the warning in Section 5: `70/20/10` is downlink-heavy and decreases the reported uplink allocation; it is not an uplink improvement. The configuration values in `current_task.md` lines 31--65 and 331--342 support context, not an isolated causal comparison.

- **The 5G timing clock issue is a measurement-method issue, not merely an editorial detail.** See P0-1. Same-host wall-clock RTT is more defensible than inferred one-way latency, but it is not the claimed monotonic measure.

### Medium/low-priority improvements

- At lines 161--168, say ``route-derived application-quality proxy used as qualitative spatial context'' rather than ``use this proxy with geometry to order the stationary points.'' The current sentence is technically caveated, but ``order'' invites a reader to read it as a radio-quality ranking. No RSRP/RSRQ-like inference should be attached to PC5 from that proxy.

- Table 2 is nearly a full configuration appendix in the main paper. Its content is sound, but it is visually dense. Keep the small set of fields that alter interpretation (n48/40 MHz, one UE, Cell 2 association caveat, TDD context, DNN/default 5QI/no QFI, absent per-request RAN counters) and move version strings/less consequential inventory detail to an appendix if page pressure permits.

- Do not add a claimed KPI/scheduler analysis simply because the ACP configuration lists possible KPI families. The paper correctly lacks matched PRB/MCS/BLER/HARQ/QFI data; that absence is a useful limitation, not a gap that prose can fill.

### Vote

**Weak Reject.** The measurement scope becomes technically defensible with P0-1 and P1-5 resolved. The section can then earn a **Weak Accept** as an evidence-bounded private-5G path study, not a public-network/RAN-generalization paper.

## Reviewer B — C-V2X/ITS/CV/CAV application-requirements review

### Assessment

The paper is appropriately more rigorous than a generic ``low latency is good'' narrative. The five envelope classes distinguish periodic one-way warning age, direct awareness, individualized planning, bursty maneuver coordination, and recovery. The table states that the experiments test necessary conditions rather than certify safety (lines 406--412), and the one-way/RTT distinction is explicitly stated. The motion calculations are correct and are not used to manufacture communication requirements.

The standards/interoperability risks are in the mapping details. A hardware SPaT validation is real evidence, but it is not a MAP validation. A custom packet-data echo is an appropriate measurement instrument, but it must not slide into an IPI/J2735 or complete CAV-service claim. A detector-size model is a useful workload, but it must not masquerade as a serialized cooperative-perception object.

### High-priority findings

- **MAP claim mismatch.** See P1-4. Replace ``SPaT/MAP function'' with the evidence actually retained. This is especially important because MAP and SPaT have distinct data models, message lifecycles, and interoperability relevance.

- **Detector workload is an estimated opaque size model, not detector serialization.** See P0-3. The existing artifact has legitimate value: it grounds size percentiles in a checkpoint's prediction counts. Its value is weakened, not strengthened, by calling it a real serialized detector message.

- **RTT success cannot certify one-way V2X requirements.** The section already says this at lines 387--391 and 543--546. Preserve it in all Table 3/Section 5 captions. An RTT no larger than a one-way budget is a conservative screening comparison, not proof that a one-way broadcast or per-leg reliability requirement is met. Conversely, missing the RTT screen is useful necessary-condition evidence; passing it does not certify the cited one-way target.

### Citation/calculation findings

- Citation linkage is present for VSC, TS 22.186, 5GAA, FHWA, and the recovery references (`references.bib` entries include `nhtsa2005vsc`, `etsi122186r19`, `fivegaa2021usecases`, `fhwa2009speed`, `nhtsa2017ads20`, `thorn2018adsframework`, and `he2024transformative`).

- I verified the arithmetic and in-manuscript linkage, not the original standards documents line by line. The paper should keep the source-specific qualification in Table 3 and avoid treating the set of heterogeneous targets as a single universal CAV requirement.

- The table caption's statement that source payloads exclude protocol/security overhead is appropriate. P1-7 is needed so readers do not mistake these source-level object sizes for exact bytes traversing the private-5G wire.

### Novelty/framing finding

The paper should not sell a single 1 KiB IPI probe as an implementation of automated intersection crossing or recovery. The contribution is a carefully scoped readiness screen across real paths and workload classes. The best framing is: *the measurement can rule out or reveal an unrepresented necessary communication condition; it cannot establish a deployed safety application*. Lines 406--412 are close to this already. P0-2 makes the actual probe semantics consistent with it.

### Vote

**Weak Reject.** I would move to **Weak Accept** after the MAP mapping and detector/probe-semantic corrections. I do not require the authors to build a full autonomous-driving application merely to keep the paper; I require them not to claim that they did.

## Reviewer C — Networking/systems experimental-methodology review

### Assessment

The section has the correct systems shape: explicit end-to-end boundary, all-attempt denominator, failed/partial-run retention, separate raw versus fragmented UDP, load/direction/concurrency controls, and no multi-UE fairness claim. The result inventory is richer than the prose needs, and the section has a credible controlled-load design once its actual metric is named correctly.

The main methodological flaw is an invalid promotion from a parser request/ACK harness into an application-completion/correlation metric. This infects the timing boundary, formula, application mapping, and matrix wording. The second flaw is rate semantics: a sequential closed-loop sender should not be described as a fixed five-request-per-second offered-rate generator.

### High-priority findings

- **Clock, correlation, and completion definitions need one consistent correction.** See P0-1 and P0-2. The following text should all use the same exact noun phrase: ``same-host wall-clock request/ACK RTT'' or ``accepted acknowledgment RTT,'' not a mixture of ``monotonic,'' ``complete response,'' ``service success,'' and ``correlated reply.''

- **The current `serviceSuccess` field is not service evidence.** It is derived from `ack.accepted && args.context.serviceSuccess`, with the context value supplied from the command line/default. It must not be used in Section 5 as validation that a planner, detector, recovery assistant, or vehicle outcome succeeded. Evidence: `cpp/include/ipi/api/experiment_logging.hpp` lines 20--27 and `private_5g_latency_sender.cpp` lines 406--446.

- **Closed-loop client rate needs exact language.** See P1-6. Report achieved request rate in Section 5, not only the nominal maximum.

### Matrix completeness and missing controls

The matrix is substantively complete for the *stated controlled proxies*. It is not complete for a public-network or real-application certification study, and it should not pretend to be. Existing text properly excludes these missing controls; do not erase those limits:

- no independent UEs, per-UE fairness, admission-control, scheduler-isolation, handover, roaming, or public-network backhaul control;
- no matched per-request RAN counters or verified QoS bearer/QFI;
- no semantic service output, freshness/expiration enforcement, request-ID/session-ID correlation enforcement, or fallback execution;
- no actual detector-object serialization/decoder or multi-vehicle maneuver;
- no synchronized one-way timing; and
- no real emergency braking, remote human, planning execution, or actuation loop.

These are missing experimental dimensions, not omissions that should be patched with prose. The existing lines 32--47, 149--168, 276--283, 299--313, 342--365, 393--402, and 607--624 already provide much of the correct caveat structure.

### Test coverage finding

The current focused C++ test suite passes: `ctest --test-dir cpp/build --output-on-failure` completed 7/7 tests on this review pass, including probe codec, J2735 flow, TCP loopback, and MQTT loopback. Those tests establish basic codec and loopback behavior; they do **not** establish the claims above. In particular, they do not test sender rejection of an ACK with wrong sequence, wrong timestamp, wrong request/session identity, stale/duplicate ACK, semantic payload failure, a wall-clock adjustment, or real PC5/5G behavior. The probe-codec test merely round-trips an ACK it constructed itself (`cpp/tests/private_5g_latency_probe_test.cpp` lines 66--88), and the loopback test checks a hand-built ACK's sequence (`cpp/tests/private_5g_latency_loopback_test.cpp` lines 88--140).

### Vote

**Reject.** The core experiment can be saved through exact metric language, but I cannot accept a systems evaluation whose declared deadline-availability metric is stronger than its protocol checks. After P0-1/P0-2 and the aligned table edits, I would vote **Weak Accept** for the bounded request/ACK performance study.

## Figure and table visual review of `main.pdf`

| Item | PDF page / source | Finding | Priority and concrete revision |
|---|---|---|---|
| Figure 3, measurement paths | PDF p. 11; source lines 63--120 | Concept is understandable, but the top PC5 line, `vendor API`, OBU/RSU labels, and arrows collide at normal paper scale. The lower MG52/NR path is also visually cramped despite excess unused white space. | **P1.** Re-layout as two clearly separated horizontal lanes or use a taller figure. Put labels above/below rather than on crossing arrow paths. Add a small timing-boundary marker only if it remains legible. |
| Table 2, operating context | PDF p. 13; source lines 244--264 | Readable but dense; long interpretation cells generate visually fragmented text. A reader will miss the Cell 2 qualification in the current layout. | **P2.** Shorten to claim-critical fields and move low-level inventory/version details to an appendix; make the pending Cell 2 qualifier a concise, visible phrase. |
| Table 3, application envelopes | PDF p. 15; source lines 414--431 | Dense but still usable. The cross-column mapping is cognitively demanding, especially for the intersection and recovery rows. | **P1.** Apply P1-4 and shorten the final measurement column; use explicit `PC5 custom echo` versus `5G IPI ACK probe` wording rather than compressed generic labels. |
| Figure 4, traffic contention | PDF p. 15; source lines 462--536 | Visually coherent, and its caption properly says only the 1 KiB/background pair is measured together. It is compact, though labels are small and it risks being read as measured scheduler architecture rather than a conceptual resource map. | **P2.** Add ``conceptual resource map'' or ``application requirements'' in the caption/label; keep the existing separate-workload disclaimer. Do not add measured-looking queue/resource-count annotations without counters. |
| Table 4, experiment matrix | PDF p. 16; source lines 557--576 | This is the least readable item. Five dense prose columns cause severe hyphenation and row-alignment burden at MobiCom PDF scale. | **P1.** Split detailed fixed controls/repetitions into prose or an appendix; retain a three- or four-column main-paper table: question, path/controlled factor, primary outcome, and a concise scope boundary. |

No clipping, missing glyphs, blank figures, or unreadable captions were observed. The defects are density and path-label collision, not a broken PDF build.

## Consolidated adjudication checklist

### Must change before submission

1. Replace the 5G ``monotonic clock'' wording with the current same-host wall-clock RTT definition, or change implementation and rerun affected experiments (P0-1).
2. Replace all 5G ``complete/correlated service response'' and service-success language with accepted parser-ACK language, unless semantic service/correlation enforcement is implemented and reevaluated (P0-2).
3. Describe the V2X-Radar workload as a predicted-box-count-derived opaque payload-size model, not serialized detector output (P0-3).
4. Remove the MAP functional/interoperability implication and distinguish PC5 custom echo from 5G IPI-envelope probe in Table 3/4 (P1-4).
5. Reinsert operator-reported/pending-export qualification for Cell 2, 70/20/10 follow-up state, and stronger/weaker location labels everywhere, especially tables (P1-5).
6. Correct sequential logical-client rate wording; report it as a maximum nominal rate and analyze achieved rate (P1-6).
7. Rework Figure 3 and Table 4 for normal-scale readability (P1 visual).

### Recommended but not blocking

- Add the short payload-size/encoded-frame clarification in P1-7.
- Tighten the PC5 availability-proxy wording so it cannot be read as RF ranking.
- Compress Table 2 and identify Figure 4 explicitly as conceptual application/resource framing.
- Add source tests for wrong/stale ACK rejection and wall-clock/monotonic timing behavior if the implementation is upgraded.
- In Section 5, report the all-attempt denominator and achieved foreground/background/client request rates beside RTT percentiles; never infer multi-nine reliability from 1,000 successes.

### Comments the authors should **not** follow

- Do **not** turn the custom PC5 echo into a claimed J2735 BSM/SPaT/MAP or regional extension. The existing distinction at lines 193--198 is correct.
- Do **not** divide RTT by two or call directional throughput controls one-way latency. The no-one-way boundary is essential.
- Do **not** call 100 logical clients a 100-vehicle, 100-UE, scheduler-fairness, or public-network experiment.
- Do **not** infer PRB/MCS/BLER/HARQ/RLC/QFI/5QI behavior from host traffic or from the `5qi-mapped` label. The current caveats are correct.
- Do **not** introduce a PC5-to-Uu fallback, public-network comparison, or all-application-concurrent claim merely to make the system look broader. Those would need a different experiment and would violate the current two-path boundary.
- Do **not** insert Section 5 results or the paper's three insights into Section 4. Its present setup/result separation is a strength.

### Genuine expansions that require user approval, implementation, or new experiments

| Expansion | Why it is not a prose-only fix |
|---|---|
| Monotonic timing and enforced request/session/result correlation | Requires probe protocol and sender/receiver implementation changes, regression tests, and recollection before replacing current performance evidence. |
| Semantic guided-planning/recovery service completion | Requires a defined result schema, service execution, result validation, state/freshness/failure behavior, and a new end-to-end experiment. |
| Native detector or cooperative-perception serialization | Requires choosing an actual schema, serializing real outputs, receiver-side semantic decode, and replay. |
| Verified Cell 2/TDD/location/RAN causality | Requires matched ACP and MG52 exports and, for causal TDD comparison, a same-cell matched baseline; scheduler claims additionally need appropriate counters. |
| Public-network, multi-UE, handover, roaming, or fairness evaluation | Requires a materially different deployment/experiment and should not be inferred from the current one-UE controlled proxy. |
| Demonstrated cross-radio fallback or vehicle-level safety/recovery | Requires application policy/actuation design, not simply restarting the current receiver or adding a sentence. |

## Final readiness judgment

Section 4 is close to a strong MobiCom setup section in structure and evidence discipline, and it already handles several common reviewer traps unusually well: private-versus-public scope, 5QI evidence, no one-way latency from unsynchronized clocks, non-equivalence of successful-RTT percentiles and availability, and separation of PC5 from Uu. The required changes are concentrated rather than architectural. Until they are made, though, the paper's central application-readiness metric is stronger on the page than in the current harness, which is a serious systems-methodology flaw. Resolve the P0/P1 checklist before treating this section as submission-ready.
