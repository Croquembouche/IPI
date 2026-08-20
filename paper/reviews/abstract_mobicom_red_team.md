# Abstract MobiCom Red-Team Audit

## Decision

**FAIL — three bounded revisions are required.** Every reported number agrees
with Sections 4 and 5, the abstract keeps exactly the three approved insights,
and it now distinguishes an accepted acknowledgment from semantic service
completion. The remaining failures concern visible compound-word spacing,
J2735 naming, and separation of the PC5 performance probe from the IPI Uu
measurement.

### Three reviewer perspectives

- **CAV/ITS semantics: PASS.** The stateless/stateful distinction is sound. The
  IPI description is limited to lightweight local profiles, pre-encoded J2735
  payloads, and correlated service operations. Lines 27--28 correctly state
  that an accepted path response is not a semantically completed service.
- **Cellular/V2X measurement: FAIL.** Lines 14--20 place IPI and both access
  paths in one narrative but do not state that the PC5 performance results come
  from a custom sequence-matched echo rather than an IPI acknowledgment.
- **MobiCom editorial: FAIL.** Two compounds render with spaces after literal
  hyphens, and `SAE International (SAE)` treats the standards body's proper name
  as though the first `SAE` had been expanded.

## Must-fix issues and exact replacements

### 1. Remove source-induced spaces inside compounds

The rendered first page visibly contains `vehicle- to-everything` and
`application- derived`. These are source-line spaces after literal hyphens, not
normal TeX hyphenation.

Replace:

```tex
presence of vehicle-
to-everything (V2X)
```

with:

```tex
presence of vehicle-to-everything (V2X)
```

Replace:

```tex
across application-
derived objects
```

with:

```tex
across application-derived objects
```

### 2. Use the standards body's proper name without a false acronym expansion

Replace:

```tex
IPI provides a typed application protocol that combines lightweight
local profiles and pre-encoded payloads for selected SAE International (SAE)
J2735 message types with correlated CAV service operations.
```

with:

```tex
IPI provides a typed application protocol that combines lightweight local
profiles and pre-encoded payloads for selected J2735 message types standardized
by SAE International with correlated CAV service operations.
```

This retains the correct narrow J2735 scope without introducing `SAE` as an
unexpanded abbreviation.

### 3. Separate the PC5 performance object from the IPI Uu acknowledgment

Replace the sentence beginning `Alongside this protocol` and the following
measurement sentence with:

```tex
We separately evaluate a production Long-Term Evolution cellular V2X
(LTE C-V2X) PC5 direct-sidelink path between nearby devices and a private 5G New
Radio (NR) Uu vehicle-to-network path through the cellular network on a real
autonomous vehicle. The PC5 performance measurements use a custom
sequence-matched request/reply probe, whereas the Uu measurements use decoded,
accepted IPI acknowledgments. Across both paths, we report all-attempt
path-response availability and sender-measured round-trip time (RTT) for
application-derived objects under field conditions, shared demand, and service
interruption.
```

This replacement does not erase the separate PC5 standard-message functional
check. It states exactly which object produced the PC5 performance results.

## Required audit checks

### Quantitative claims

- **PC5 2-KiB failure:** `0.2%` to `99.2%` matches the stationary field-path
  comparison in Section 5.
- **PC5 route availability:** `59.1--76.3%` matches the four mobile collections:
  `515/675`, `731/1,000`, `591/1,000`, and `716/1,000`.
- **Uu 1/2-MiB deadline result:** the pooled group contains 14,000 eventual
  accepted ACKs, and every sender-measured request/ACK RTT exceeds 500 ms. The
  abstract's numerical claim is correct.

### Scope and terminology

- **J2735 scope:** PASS after must-fix item 2. `Lightweight local profiles` and
  `pre-encoded payloads for selected J2735 message types` do not claim full SAE
  J2735 ASN.1 conformance.
- **IPI versus PC5:** FAIL until must-fix item 3 identifies the PC5 custom echo
  and the Uu IPI acknowledgment separately.
- **Accepted ACK versus semantic completion:** PASS. Lines 25--28 define a
  sender-measured request/accepted-ACK RTT and explicitly deny semantic service
  completion.
- **Acronym first use:** CAV, V2X, 5G, IPI, LTE C-V2X, NR, RTT, KiB, MiB, and
  ACK are spelled out at first use. PC5 and Uu are reference-point names rather
  than acronyms; `direct-sidelink path` and `vehicle-to-network path` explain
  their roles. Must-fix item 2 removes the remaining SAE first-use problem.
- **Insights:** exactly three approved insights remain. The abstract adds no
  fourth contribution or insight, and Insight 2 correctly says `strictly
  limited`.

### Visible page-1 layout

The rebuilt first page otherwise **passes**: the title and abstract are legible;
there is no clipping, overlap, orphaned abstract heading, or broken column flow;
and the Introduction begins cleanly below the abstract. Must-fix item 1 addresses
the only visible typesetting defects inside the abstract.

## Validation

- Audited abstract SHA-256:
  `1187d85426103a93570c18e0660f35a6c546a8698044044a06d860487896aabf`
- Forced manuscript rebuild: successful; current output is 28 pages.
- PDF page 1: rendered and visually inspected at 160 dpi after the rebuild.
- `git diff --check`: clean; no whitespace errors reported.

## Focused second pass: revised abstract and rebuilt page 1

**PASS — no remaining P0/P1 item.** Re-read the revised abstract and inspected
the current rendered first page only. `main.pdf` is newer than
`00_abstract.tex` (15:03:23 versus 15:03:14 EDT), so the visual check is of the
current source.

- **M1, metric and service boundary: resolved.** The abstract now reports
  “all-attempt path-response availability” and sender-measured RTT
  (`00_abstract.tex:18-20`). Its Uu result ends at an accepted ACK and expressly
  says that this path response is not a semantically completed service
  (`:25-28`).
- **M2, J2735 scope: resolved.** IPI is now limited to lightweight *local*
  profiles and pre-encoded payloads for *selected* SAE J2735 message types
  (`:12-14`); it makes no ASN.1/UPER, standardized-extension, or external
  interoperability claim.
- **M3, PC5/Uu path interpretation: resolved.** “Alongside this protocol”
  preserves the IPI-versus-measurement separation, while PC5 is described as a
  direct-sidelink path between nearby devices and Uu as the vehicle-to-network
  cellular path (`:14-18`). No aggregation, selector, fallback, public-network,
  or measured-performance-certification claim was introduced.
- **M4, PC5 route denominator: resolved.** The 59.1--76.3% figure is now named
  as all-attempt route path-response availability (`:23-25`).
- **Approved ending: resolved.** Lines 29--35 retain exactly the three approved
  insights: direct V2X alone is insufficient for dependable large/multi-step
  exchange; 5G's usable object size is bounded by time/conditions; and
  collaborative applications need methods beyond independently used current
  V2X and 5G paths. No fourth insight or semantic-service overclaim appears.

**Word count.** The reported 322 detex words are **compression advice only**;
they are not a correctness, evidence, or MobiCom-readiness blocker in the
absence of a submission-portal word limit. Compress only if the target venue
imposes a cap, and preserve the repaired metric and standards boundaries.

**Visual result: PASS.** The rebuilt page 1 is legible and balanced, with no
clipping, overlap, broken glyphs, or abstract-to-Introduction layout defect.
The longer boundary text still fits cleanly in the left column.

**Remaining P0/P1: none.** `git diff --check` and the final SHA-256 are verified
in the handoff for this appended review.
