# Section 2 MobiCom Red-Team Review

Date: 2026-08-12

Reviewed artifact: `paper/current_manscript/sections/02_related_work.tex`

Review panel:

- Reviewer A: mobile networking and systems novelty
- Reviewer B: CAV, connected-vehicle, ITS, and protocol standards
- Reviewer C: cellular, radio, V2X, and measurement evidence

## Panel verdict

The section has a strong MobiCom structure. It connects prior work to two
precise gaps, preserves the paper's two contributions and three insights, and
does not use a broad "first" claim. The left--gap--right figure is clear and
does not rank unlike systems. However, two comparisons require substantive
narrowing before the novelty argument is defensible:

1. The first draft understates the session and message capabilities of 3GPP
   VAE and ETSI MEC 030.
2. The 2026 40-km 5G Standalone C-ITS corridor is a closer evidence comparator
   than the first draft acknowledges.

## Reviewer A: networking and systems

### A1. Acknowledge VAE and MEC lifecycle support

- Severity: must change.
- Reviewed phrase: "VAE and MEC provide delivery and network services" and
  the subsequent claim that no reviewed interface represents correlated
  request, update, completion, or rejection.
- Evidence: 3GPP TS 23.286 defines session identifiers, establishment, update,
  termination, notifications, and ability or inability responses. ETSI MEC 030
  defines V2X message publication and subscription, standards-declared message
  formats, notification, and subscription expiry.
- Revision: distinguish IPI as a concrete operation-level data contract. It
  combines pre-encoded SAE J2735 messages with the CAV request, update, result,
  freshness-bound, and service-state representation selected by this paper.
  State that it complements VAE session control and MEC service APIs.

### A2. Describe Tentacles at its actual strength

- Severity: recommended.
- Reviewed phrase: Tentacles "matches QoS requirements to observed delay,
  bandwidth, reliability, and security."
- Evidence: Tentacles uses a four-attribute network characterization and
  carries generic timing, deadline, and execution state. Its principal
  effectiveness evaluation uses controlled network emulation, with a smaller
  commercial-route observation.
- Revision: use "delay, bandwidth, reliability, and security
  characterizations." Preserve the narrower distinction that Tentacles does
  not define one domain model for SAE J2735 messages and general CAV service
  results.

### A3. Narrow the selector requirement

- Severity: recommended.
- Reviewed phrase: "an adaptive system needs two inputs before it can choose a
  path."
- Evidence: AutowareV2X and Tentacles already choose paths with
  application-specific freshness or QoS state.
- Revision: say that a selector benefits from common operation semantics when
  it must generalize decisions across heterogeneous services.

### A4. State the distinctive combination directly

- Severity: must change.
- Reviewed phrase: "no reviewed interface" and "no reviewed evaluation."
- Reason: these absence claims invite counterexamples from VAE, MEC, ITS
  facilities, and the 5G corridor. The paper's positive combination is already
  strong.
- Revision: state the combination of IPI's concrete operation contract and the
  paper's application-level field matrix across separately deployed LTE
  C-V2X PC5 and local 5G NR Uu paths.

## Reviewer B: CAV, CV, ITS, and standards

### B1. Add omitted application-layer architectures

- Severity: must change.
- Omitted sources: the 5GAA V2X Application Layer Reference Architecture and
  ETSI EN 302 665 ITS-station facilities architecture.
- Evidence: 5GAA defines application-layer actors, reference points, and
  deployment views. ETSI EN 302 665 includes service-oriented architecture
  support, backend-session establishment, session-loss handling, and session
  continuity during handover.
- Revision: credit these architectures with organizing interoperability and
  reusable services. Then distinguish IPI as a concrete, transport-independent
  SAE J2735-plus-CAV-operation message contract.

### B2. Scope message-dictionary claims to SAE J2735

- Severity: recommended.
- Reviewed phrase: "a message standard does not by itself tell network-aware
  middleware..."
- Revision: say that the SAE J2735 message dictionary alone does not define a
  general correlated service-result lifecycle. Do not generalize this statement
  to VAE, MEC, or all ITS standards.

### B3. Keep AutowareV2X separate from PC5 evidence

- Severity: recommended.
- Evidence: the cited AutowareV2X implementation selects the fresher
  collective-perception message across Wi-Fi and LTE. It is not PC5 evidence.
- Revision: retain it as an application-level freshness-selection example.

### B4. Correct standards dates

- Severity: required for submission readiness.
- `etsi123287r18`: official V18.4.0 document is dated October 2024.
- `etsimec030`: official V3.2.1 document is dated February 2024.

## Reviewer C: cellular, radio, V2X, and measurements

### C1. Treat the 40-km corridor as a close baseline

- Severity: must change.
- Evidence: the study evaluates PC5 and Uu, six ETSI C-ITS scenarios,
  application-layer end-to-end latency and packet-delivery ratio, three speed
  categories, and three density categories on a 40-km 5G Standalone private
  corridor.
- Revision: credit all of these dimensions. Distinguish this paper by its
  correlated service operations, detector-sized and larger objects, transport
  choices, offered uplink contention, logical-client concurrency, service
  interruption, and separately deployed LTE-PC5 and local-NR-Uu paths.

### C2. Remove "independently measured" as a novelty filter

- Severity: must change.
- Reason: the corridor already measures PC5 and Uu together. "Independently
  measured" looks contrived if used as an absence claim.
- Revision: describe the paths as separately deployed and emphasize the
  scientifically meaningful difference: the application-operation and stressor
  matrix.

### C3. Credit teleoperation's appropriate completion unit

- Severity: recommended.
- Evidence: Fezeu et al. connect radio conditions, scheduling, handovers,
  transport, and adaptation to per-frame delay and QoE for camera and lidar
  streams.
- Revision: explicitly say that continuous streams appropriately use per-frame
  freshness and QoE. Reserve this paper's complete-response metric for the
  stateless updates and correlated service operations represented by IPI.

### C4. Distinguish SEE-V2X from complete-frame evaluation

- Severity: recommended.
- Evidence: SEE-V2X is an application-shaped, cross-layer C-V2X dataset. Ku et
  al. directly evaluate complete image-frame delivery.
- Revision: state these roles separately.

### C5. Add current direct-PC5 application evidence

- Severity: high-value recommendation.
- Sources: Zhan et al.'s physical-PC5 hardware-in-the-loop cooperative-driving
  study and CooperScene's synchronized cooperative-autonomy and C-V2X
  characterization.
- Revision: add one synthesis sentence. These works deepen direct-PC5
  evidence, while remaining focused on controller or perception workloads.

### C6. DRIVE-SAFE treatment

- Severity: no change.
- Decision: retain the explicit statement that DRIVE-SAFE is an active research
  direction, not completed performance evidence.

## Figure review

The rendered figure passes visual review. Its hierarchy, arrows, color
separation, caption, and accessibility description are clear. Text is legible
at ACM size. There is no clipping, overlap, score, or third contribution.

Required figure revision: replace "one domain interface for established ITS
messages and correlated CAV services" with "a common application contract for
pre-encoded SAE J2735 messages and correlated CAV service operations."

Recommended figure revision: replace "Verified QoS" with "Verifiable QoS
treatment" so the future requirement cannot be mistaken for a result of this
paper.

## Consensus changes to apply

1. Recast the VAE and MEC comparison at their actual strength.
2. Add the 5GAA and ETSI ITS-station architecture sources.
3. Promote the 40-km PC5/Uu corridor to a close comparator.
4. Remove broad "no reviewed" and "independently measured" formulations.
5. Correct the two standards dates.
6. Narrow the figure's center label to SAE J2735 plus CAV operations.
7. Credit stream-specific, cross-layer, complete-frame, controller, and
   perception evaluations according to their actual completion units.

## Adjudication of uncertain items

The panel raised three wording choices. None requires an additional manuscript
claim. The evidence supports conservative resolution:

1. Use "pre-encoded SAE J2735 messages," not a broader claim of support for all
   established ITS profiles.
2. State that IPI complements VAE session control and MEC service APIs. Do not
   claim that those standards lack sessions or outcomes.
3. Use "verifiable QoS treatment" in the future-requirements panel. This avoids
   implying that the current experiment verified differentiated network QoS.

These revisions narrow prose to existing evidence. They do not expand IPI's
implementation or alter the paper's two contributions and three insights.
