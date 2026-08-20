# Proposed Manuscript-Related Changes and Implementation Status

These changes extend beyond editorial correction. The user's 2026-08-14
implementation request approves their local, testable engineering portions.
The statuses below distinguish implemented code from external conformance,
deployment, and field-evidence gates. Local implementation does not by itself
authorize a manuscript claim about formal standards conformance or field
performance.

## P-001 — Expand IPI's machine-readable failure and standards-conformance model

Status: typed regional-extension implementation and independent local
conformance vectors complete; exact licensed-SEP2023 integration gate open.

Implemented locally:

- Machine-readable timeout, missing-response, transport-interruption,
  service-unavailable, stale, duplicate, correlation, termination, and fallback
  outcomes.
- Expiration and terminal-outcome validation in the transport-neutral session
  lifecycle and strict rejection of unknown enums, flags, trailing bytes, and
  invalid service status/outcome combinations.
- Versioned strict project codecs and negative/boundary tests.
- A separate typed `IPI.asn` module bound to reserved `TestMessage00`
  (`DSRCmsgID` 240) through local `RegionId` 200, with a strict C++ UPER codec
  for the complete J2735 `MessageFrame`.
- Planning, perception, control, minimal, optional-field, malformed-input,
  identifier, extension, padding, and size vectors. Two independent ASN.1
  compilers produce identical regional bytes, and the unmodified USDOT J2735
  202409 package encodes and decodes the outer frame.
- A reproducible preparation script that copies a caller-supplied licensed
  module bundle, injects the `Reg-TestMessage00` object-set entry, and records
  SHA-256 input/output hashes without redistributing SAE files.

Still required before the full standards claim:

- Supply the separately distributed licensed `J2735ASN_202309` modules, compile
  the prepared combined module set with the selected production encoder, and
  confirm the vectors with an independent SEP2023 implementation. The PDF and
  generated Mocar descriptors in this checkout do not include those source
  modules.

The Section 1 red team originally found only service rejection, negative
acknowledgment after validation failure, expiration metadata, service progress
states, and free-text session termination. It also found no complete
machine-readable failure taxonomy or formal cooperative-service regional
schema. The implementation above closes those local design and codec gaps. The
remaining boundary is exact compilation and endpoint interoperability against
the separately licensed SEP2023 ASN.1 module set, not an absent IPI schema.

Proposed expansion:

1. Add typed timeout, missing-response, transport-interruption, service-
   unavailable, and fallback-result states with tests.
2. Add machine-readable rejection and termination reason codes while retaining
   human-readable details.
3. Define when expiration is enforced and test stale-request and stale-response
   handling.
4. Specify the cooperative-service message as a formal J2735 regional extension
   and add conformance tests against that schema.

Implementation boundary: the failure/freshness model and formal typed regional
profile are now present and locally cross-encoded. The manuscript must not call
this certified or exact SEP2023 interoperability until the separately licensed
module bundle and an independent SEP2023 endpoint complete the remaining gate.

## P-002 — Implement an enforced cross-transport IPI session path

Status: local lifecycle and broker reference path complete; deployed-broker and
field-facade gates open.

Implemented locally:

- Clock-injected session state/lease authority with registration retention,
  heartbeat activation, patch, termination, service/telemetry authorization,
  outstanding-request tracking, and terminal service results.
- High-level facade coverage for patch, termination, session lookup, and
  service updates, with isolated in-memory state instead of a process-global
  singleton.
- Strict `IPIS` per-topic records, a publish/subscribe broker abstraction,
  client and broker-hosted endpoint, MQTT 3.1.1 authentication fields,
  reconnect/resubscribe logic, and an in-memory broker integration test.
- Negative and coverage tests for topic families, identity, class/content,
  stale/duplicate/correlation cases, terminal sessions, leases, and state
  transitions.

Still required before the full deployment claim:

- Exercise the transport against a deployed authenticated broker through
  disconnect and broker restart; add TLS, production access control, and the
  required MQTT delivery QoS.
- Bind the deployed field workload process to the high-level session facade and
  retain field evidence for the common application path.
- Define deployment policy for the retained fallback-RSU, minimum-RSSI, and
  subscription inputs. The lifecycle stores these values but does not make an
  automatic radio/path-selection decision.

The Section 3 red team confirmed that the current artifact provides one C++
facade and a common type vocabulary for vehicular messages and CAV service
requests. However, its only concrete session transport is an in-memory
recorder. The MQTT field path is a separate workload request/response latency
probe rather than a broker-backed implementation of the session-topic
convention. In addition, the session types describe more behavior than the
adapter enforces: invocation and telemetry do not validate active sessions,
leases do not expire, terminal service outcomes are not generated, patch and
termination are not exposed by the high-level facade, and fallback inputs are
not retained after registration.

Proposed expansion:

1. Implement and test an explicit session state machine, lease expiry,
   active-session checks, terminal service results, facade-level patch and
   termination, and structured error outcomes.
2. Implement a broker-backed session transport for registration, heartbeat,
   service request, service update, events, telemetry, and CV responses. Add
   broker integration, reconnect, access-control, isolation, and restart tests.
3. Bind the field harness to the high-level IPI facade so the same application-
   derived workloads exercise the common interface across transports. Verify
   request/session correlation and record the workload request/response
   outcomes.
4. Retain the fallback-RSU, minimum-RSSI, and subscription inputs and define how
   a deployment consumes them. An automatic path selector would remain a
   separate decision and evaluation.
5. Expand negative and coverage tests to MAP, SRM, SSM, every topic family,
   trailing bytes, unknown enumeration values, class/content mismatch,
   correlation errors, terminated sessions, expiration, and state transitions.

Implementation boundary: the reference middleware path is now implemented and
locally tested. Production broker behavior and field-path equivalence remain
unverified and must not be claimed as completed deployment evidence.

## P-003 — Upgrade field timing, workload correlation, and aligned radio evidence

Status: local timing, correlation, and detector contracts complete; recollection
and aligned-radio gates open.

Implemented locally:

- Local `steady_clock` RTT timing; wall-clock timestamps remain diagnostic and
  one-way latency is only produced when synchronization is explicitly known.
- Request, message, response, correlation, session, sequence, expiration,
  duplicate, stale, and local-deadline enforcement for TCP, UDP, and MQTT probe
  paths, with appended audit columns that preserve the existing CSV order.
- A versioned native detector-result schema with 3-D geometry, confidence,
  velocity, covariance, strict bounds, and deterministic top-K selection.

Still required before new measurement claims:

- Recollect every condition used for a monotonic-timing claim with the updated
  executable.
- Serialize the chosen real detector outputs into the new schema and rerun the
  representative payload conditions.
- Retain matched Airspan counters, add a same-cell 40/40/20 reference where a
  TDD comparison is intended, and keep the separate iPhone survey scoped as
  spatial context rather than MG52 telemetry.

The Section 4 red team found three possible measurement extensions. First, the
private-5G harness computes same-host round-trip time from `system_clock`
wall-clock timestamps rather than a monotonic clock. Second, future experiments
could add request and session correlation to the existing IPI workload probes.
Third, the detector condition sends `IPI-CooperativeService` frames whose opaque
content is sized from predicted-object counts; a future workload could serialize
native detector objects. The follow-up MG52 Cell 2 lock is operator-verified.
The radio-location context comes from an iPhone survey and therefore
characterizes a separate UE rather than the MG52 application path.

Proposed expansion:

1. Add a local steady-clock timer for RTT while retaining wall-clock timestamps
   only for cross-log diagnostics. Add clock-adjustment regression tests and
   recollect any result used for a monotonic-timing claim.
2. Echo and enforce the outstanding sequence, request ID, session ID, and result
   identity. Reject mismatched, duplicate, stale, and late responses. This
   overlaps with P-002's cross-transport session path.
3. Select a concrete detector-result schema, serialize real prediction objects,
   decode them at the receiver, and rerun the representative size conditions.
4. Retain matched Airspan counters for each follow-up bin. Add a same-cell
   40/40/20 reference before making a causal TDD comparison, and add aligned RAN
   counters before attributing behavior to scheduling or radio-resource
   consumption. Treat the existing iPhone survey as separate-UE spatial context,
   not as MG52 telemetry.

Implementation boundary: the new measurement contract is locally verified, but
no historical result was silently reclassified as monotonic or native-detector
evidence. New claims require new field artifacts.

## P-004 — Carry and validate IPI end to end over the direct PC5 path

Status: local adapter and target build complete; two-device field gate open.

Implemented locally:

- A versioned, CRC-protected `IP5X` request/response adapter preserving message,
  request, correlation, session, sequence, expiration, source, intersection,
  J2735 type/encoding, and frame-counter identity.
- Strict malformed, CRC, size, duplicate, stale, mismatch, late, and exact-limit
  tests. The portable default caps the complete application packet at 2048
  bytes; 4080 bytes is exposed only as a deployment-specific hard ceiling.
- A Mocar custom-message initiator/responder sample that uses local monotonic RTT
  and validates the returned response. The sample now carries and decodes the
  complete formal IPI `TestMessage00` UPER frame. It cross-compiles with the
  available AArch64 toolchain and vendor libraries.

Still required before the cross-path field claim:

- Run the initiator and responder on two Mocar devices at representative compact
  and near-limit conditions, retain all-attempt availability and framing
  overhead, and test malformed/duplicate/stale/mismatched/late behavior on the
  deployed path.
- Keep `IP5X` described as a project integration envelope over the vendor custom
  message channel. Its payload is the formal IPI regional `MessageFrame`, but
  `IP5X` itself is not a standardized SAE message or bearer.

The integrated red team found that the current manuscript must keep three
evidence paths separate. The J2735 callbacks are small functional checks. The
PC5 performance sweep uses a custom sequence-matched vendor packet. The 5G
performance path carries IPI workload frames and records their responses.
Therefore, the current experiments do not establish that the same IPI workload
representation runs over both PC5 and Uu.

Proposed expansion:

1. Add an IPI-to-PC5 adapter that respects the vendor interface's application-
   payload limit and preserves message, request, session, sequence, and
   expiration fields.
2. Decode the IPI object at the PC5 receiver and validate the returned response
   against the outstanding request and session.
3. Repeat representative compact and near-limit PC5 conditions with the IPI
   adapter. Report framing overhead, all-attempt availability, and sender RTT
   separately from the existing custom-echo evidence.
4. Add negative tests for malformed, duplicate, stale, mismatched, and late
   messages before claiming an enforced cross-path protocol lifecycle.

Implementation boundary: the adapter and device sample exist, but the current
PC5 custom-echo evidence remains distinct. Only a retained two-device `IP5X`
run can establish the new path experimentally.
