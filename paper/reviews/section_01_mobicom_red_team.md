# Section 1 MobiCom Red-Team Review

Date: 2026-08-12

Scope: the new `paper/manuscript/sections/01_introduction.tex` only. The panel
did not review or reuse any file under `paper/legacy_draft/`.

Panel: one GPT-5.6 Terra Max subagent acting independently as 1) a MobiCom
networking and systems reviewer, 2) a CAV/CV/ITS protocol reviewer, and 3) a
cellular, radio, 5G, and V2X measurement reviewer.

## Reviewer 1: networking and systems framing

Verdict: major revision, with a strong underlying systems argument.

The reviewer found that the Introduction moves logically from the need for
communication to application interaction patterns, directional resource
demands, an application-level readiness question, IPI, and the two measured
paths. The quantitative preview and the separation between contributions and
insights were also strengths.

Required changes:

1. State the collective interface gap precisely before introducing IPI, without
   asserting a universal first-of-kind claim before Related Work is complete.
2. Shorten the contribution and insight bullets so they remain high-level.
3. Replace the defensive out-of-scope sentence with a positive description of
   the measured LTE C-V2X PC5 and isolated private-5G NR Uu paths.
4. Replace or define `edge service`, `application-valid workload`,
   `state-mirror`, and undefined uses of `reliably`.
5. Replace `reliabilities` with `reliability requirements` and describe a
   complete exchange as finishing rather than arriving.

## Reviewer 2: CAV, CV, ITS, and protocol semantics

Verdict: major revision because the protocol contribution is compelling but
some claims exceed the current reference implementation.

The reviewer found that the stateless/stateful distinction is useful and that
the metadata, session, service-state, expiration, confidence, and object-list
claims map to implemented structures and tests.

Required changes:

1. Use BSM, PSM, MAP, and SPaT as replaceable-state examples. Present SRM and
   SSM separately as a related request/status exchange.
2. Do not imply full SAE J2735 ASN.1 conformance. State the implemented support:
   lightweight typed profiles, preservation of pre-encoded J2735 payloads, and
   a cooperative-service message designed for regional-extension integration.
3. Replace broad `failure semantics` with the implemented rejection,
   validation, expiration, service-state, and session-termination mechanisms.
4. Say that IPI carries fields needed to evaluate freshness; do not imply that
   the current interface always enforces expiration.
5. Identify the workload source as a completed 922-sample V2X-Radar detector
   run. Do not imply that the network experiment validates a complete
   perception-to-control application.

## Reviewer 3: cellular, radio, 5G, and V2X measurement

Verdict: major revision for claim calibration, with strong path and metric
separation.

The reviewer found that the Introduction correctly distinguishes PC5 from Uu,
identifies the private-network components, scopes logical clients to one host
and attachment, uses sender-measured RTT because clocks were unsynchronized,
and reports all-attempt availability and deadline misses.

Required changes:

1. Describe `5qi-mapped` as an application-side label. The host captures showed
   no packet-marking difference, so the paper cannot claim verified
   network-enforced QoS.
2. Replace `reliably` with the measured attempt-level completion range.
3. Add the 910-attempt denominator to the no-reply direct-path result.
4. Separate controlled offered-load and restart effects from associations with
   weak-signal locations and high logical-client demand.
5. Keep all conclusions tied to one certified LTE C-V2X device path, one
   isolated private-5G deployment, and the stated controlled stressors.

## Evidence audits

Every quantitative teaser matched `experiment_summary.md`:

1. The 2 KiB direct-path failure rate rises from 0.2% to 99.2% across the
   relevant stationary points; the next point has 910/910 observed failures.
2. The four driven routes have 59.1--76.3% reply availability.
3. The SPaT/state-mirror group contains 2,000 attempts, with 46.60% missing
   100 ms and none missing 500 ms.
4. The 128--512 KiB group contains 18,000 attempts, with 11.64% missing 500 ms.
5. The 1--2 MiB group contains 14,000 attempts, all of which exceed 500 ms.

The implementation audit found support for typed message profiles, opaque
J2735-payload preservation, envelopes, sessions, correlation, requested
horizons, expiration metadata, perception/planning/control/offload payloads,
confidence, service progress states, rejection, validation, acknowledgements,
and session termination. It did not find formal SAE ASN.1 conformance,
general freshness enforcement, or a complete protocol taxonomy for timeout,
missing response, broker/receiver restart, and fallback.

## Main-agent adjudication

Accepted and applied:

1. All nine consensus wording and scope corrections.
2. The exact two-contribution structure.
3. The exact three approved insight headings, including `strictly` in Insight 2.
4. The full quantitative preview, because every number is supported and the
   paper is organized around application deadlines rather than raw throughput.

Deferred:

1. A stronger first-of-kind novelty claim. The Introduction now states a
   feature-based, collective gap. Any stronger wording must wait for the full
   Related Work review.

Not applied:

1. No third testbed, dataset, or insight contribution was added.
2. The insight headings were not weakened or replaced; their supporting prose
   remains scoped to the measured paths.
3. No NR-V2X, public-network, one-way-latency, radio-scheduler, or complete-CAV-
   application claim was added.

One implementation-expansion question remains genuinely uncertain and is
recorded in `paper/manuscript/proposed_changes.md` for user approval.
