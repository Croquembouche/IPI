# Consolidated Section 3 red-team review

I made no edits and did not use `paper/legacy_draft`. I reviewed the current Section 3 against the binding outline, current Sections 1–2, repository guidance, experiment summary, and the live C++ implementation/tests. I also reran the current suite: **all 7 CTests pass**. That is useful internal evidence, but it is not PC5, SAE/J2735, MQTT-session, ROS 2, or production-service interoperability evidence.

## Bottom line

**Section 3 is not MobiCom-ready in its current form.** The recently tightened wording fixes several real overclaims, but several remaining passages still conflate:

1. a reference C++ facade with deployed cross-transport bindings;
2. a local bit-packed/canonical profile with formal SAE ASN.1/UPER behavior;
3. a topic-name convention and a separate MQTT latency harness with a broker-backed session transport;
4. type-level lifecycle/freshness fields with enforced session, completion, correlation, expiry, fallback, or service-execution semantics.

These can be made **claims-safe mostly through prose**. However, retaining strong claims about formal J2735/UPER interoperability, a real MQTT session binding, enforced stateful service semantics, automatic fallback, or completed CAV-service outcomes would require implementation and new validation/experiments.

The current Section 3 has a sound high-level problem statement and correctly distinguishes an application acknowledgement from service completion in principle. The major issue is that several claims still sit one layer above what the code implements.

## Verification of the five tightened corrections

The five corrections requested during review are now materially correct:

| Corrected point | Current manuscript status | Code/test evidence |
|---|---|---|
| Envelope scope | Correctly limited to “message or service request submitted through the envelope API” at [§3 L107](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:107); heartbeat/telemetry are said to carry session ID directly. | `HeartbeatUpdate` and `TelemetrySubmission` contain session-scoped fields rather than `EnvelopeMetadata` at [types.hpp L271](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/types.hpp:271). |
| Opaque J2735 path | Correctly says opaque bytes are preserved rather than decoded at [§3 L149](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:149). | Opaque ingest checks only nonempty bytes and stores them at [edge4av_interface.cpp L99](/media/william/blueicedrive/Github/IPI/cpp/src/api/edge4av_interface.cpp:99). |
| Cooperative class/content consistency | Correctly says it is not enforced at [§3 L151](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:151). | `CooperativeServiceMessage::validate()` checks selected bounds but does not bind `serviceClass` to planning/perception/control payloads at [ipi_cooperative_service.cpp L69](/media/william/blueicedrive/Github/IPI/cpp/src/core/ipi_cooperative_service.cpp:69). |
| Unknown-session scope | Correctly limits directory checks to heartbeat, patch, and termination at [§3 L251](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:251). | `invokeService()` and `submitTelemetry()` append data without a lookup at [receiver.cpp L146](/media/william/blueicedrive/Github/IPI/cpp/src/api/receiver.cpp:146). |
| MAP/SRM/SSM test coverage | Current test paragraph no longer says those serializers are covered. | The named `j2735_message_flow` test exercises BSM, PSM, one cooperative message, opaque TIM, and truncated PSM only at [j2735_message_flow_test.cpp L67](/media/william/blueicedrive/Github/IPI/cpp/tests/j2735_message_flow_test.cpp:67). |

The changes above should stay. The residual mismatches below are the ones that remain high-impact.

# P0 — Correct before treating Section 3 as submission-ready

## P0-1. “UPER” and J2735 wording still risks a false formal-conformance implication

**Manuscript locations:** [§3 L119–136](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:119), especially L127–134; [§3 Table 1, L149](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:149).

**Why this remains high risk:** The prose properly says that the profile is not SAE’s production ASN.1 codec. But it continues to call the reference implementation a “bit-packed profile,” labels the API encoding `UPER`, and describes the cooperative bytes as a future J2735 regional-extension route. A standards reviewer can still reasonably infer that the actual serialized bytes are formal UPER or a defined regional extension.

That inference is false for the custom cooperative object:

- The cooperative `UPER` encoder directly returns `to_canonical_encoding()` at [uper_codec.cpp L557](/media/william/blueicedrive/Github/IPI/cpp/src/v2x/uper_codec.cpp:557).
- The cooperative `BYTES` encoder returns the same canonical encoding at [j2735_payload_codec.hpp L167](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/j2735_payload_codec.hpp:167).
- Thus, for `IPI-CooperativeService`, the `UPER` and `BYTES` paths are the same project-local byte format.
- The source header itself inaccurately calls every `MessageFrame` payload “ASN.1 UPER-encoded” at [message_frame.hpp L23](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/core/message_frame.hpp:23), although the custom cooperative frame is not.
- The service-request header is more accurate: it says a real deployment would need J2735 ASN.1 marshalling and that the present helper is a simple canonical encoding pending a full ASN.1 toolchain at [ipi_service_request.hpp L24](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/core/ipi_service_request.hpp:24).

**Safe replacement for §3 L127–136:**

> For the selected reference profiles, the library provides project-local bit-packed and canonical encodings. Typed unpacking checks the requested API type and applies the validators implemented for that profile. These encodings are not a complete SAE J2735 ASN.1/UPER implementation. In particular, the current `IPI-CooperativeService` `UPER` and `BYTES` options both carry the same canonical IPI bytes. The custom cooperative object is a research-format proposal for a future deployment-specific J2735 extension path; it is neither an SAE-assigned message nor a validated ASN.1 regional extension. The type vocabulary also declares JSON, but the current codec rejects it as unimplemented.

**Safe replacement for the Table 1 J2735 payload behavior cell:**

> Rejects empty opaque payloads. Typed unpacking checks the requested type and decodes the selected project profile; opaque bytes are preserved without decoding. The profile validators are partial and do not establish SAE conformance.

**What needs implementation/experiments to make a stronger claim:**

- Generated or independently validated SAE J2735 ASN.1/UPER codecs;
- a formally specified ASN.1 module and assignment mechanism for the custom extension;
- golden vectors plus vendor-stack interoperability tests;
- device-path evidence showing that a byte emitted by the facade is accepted by an independent J2735 stack, and vice versa.

Without that, do **not** use “UPER-conformant,” “J2735-compatible” for the typed local profiles, “regional extension” as if standardized, or “interoperable” for the custom cooperative bytes.

## P0-2. The manuscript still claims an MQTT/session topic binding that the repository does not implement

**Manuscript locations:** [§3 L226–237](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:226); Figure 3 and caption at [§3 L70–101](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:70).

**Why this is a direct implementation mismatch:**

- The only concrete `PrivateSessionTransport` factory is `make_in_memory_private_session_transport()` at [private_session_transport.cpp L250](/media/william/blueicedrive/Github/IPI/cpp/src/api/private_session_transport.cpp:250).
- It records `SessionPublication` records in a local vector; it does not publish to a broker at [private_session_transport.cpp L42](/media/william/blueicedrive/Github/IPI/cpp/src/api/private_session_transport.cpp:42).
- The transport API contains registration, heartbeat, invocation, telemetry, list responses, and list publications only; it has no MQTT implementation, no update publisher, no event publisher, and no CV-response publisher at [private_session_transport.hpp L47](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/private_session_transport.hpp:47).
- `SessionTopic::to_string()` syntactically defines names for events, service updates, and CV responses at [private_session_transport.cpp L161](/media/william/blueicedrive/Github/IPI/cpp/src/api/private_session_transport.cpp:161), but the concrete in-memory transport only records register/heartbeat/service-request/telemetry publications at [private_session_transport.cpp L42](/media/william/blueicedrive/Github/IPI/cpp/src/api/private_session_transport.cpp:42).
- The MQTT unit test uses a **different** latency-harness namespace, `ipi/int-1/latency/veh-1/{request,ack}`, not session topics at [private_5g_latency_mqtt_loopback_test.cpp L186](/media/william/blueicedrive/Github/IPI/cpp/tests/private_5g_latency_mqtt_loopback_test.cpp:186).
- The session-topic test exercises exactly one `ServiceRequest` topic string round trip and in-memory publications; it does not contact MQTT at [private_session_transport_test.cpp L25](/media/william/blueicedrive/Github/IPI/cpp/tests/private_session_transport_test.cpp:25).

Therefore, “This hierarchy makes an operation observable and routable when MQTT is used” is presently an **unimplemented-adapter claim**, not a demonstrated behavior.

**Safe replacement for §3 L226–237:**

> The reference library defines a deterministic topic-naming convention for registration, events, heartbeat, service request, service update, telemetry, and CV response. Its only concrete session transport records registration, heartbeat, service-request, and telemetry publications in memory. The measured MQTT probe uses separate latency request/ack topics and is not an implementation of the session-topic convention. An MQTT session adapter could map the named operations to broker topics, but that adapter is outside the current reference implementation. The repository also contains optional ROS 2 conversion code and separate device examples for selected standard messages.

**Figure repair:**

- Replace **“Communication bindings”** with **“Reference paths and adapter boundaries.”**
- Replace **“Direct PC5 device path”** with **“Separate optional PC5 device examples for selected standard messages.”**
- Replace **“Internet Protocol transports over 5G Uu”** with **“Separate TCP/MQTT/UDP latency-probe framing over IP.”**
- Replace the caption sentence “The reference implementation covers … the bindings named in the text” with:

> The source tree contains a common in-memory facade, optional conversion/device examples, and separate field-measurement probe paths. Section 4 identifies which probe paths are exercised in the field.

**What needs implementation/experiments to make the present claim true:**

- A broker-backed `PrivateSessionTransport`;
- serialization/deserialization contracts for each session operation;
- actual service-update/event/CV-response publication;
- broker ACL/authentication/QoS/reconnect semantics;
- integration tests against an external broker, including failure/recovery and topic isolation;
- field evidence that the session binding—not merely the latency probe—operated over MQTT.

## P0-3. Lifecycle, session state, patch/termination, and failure semantics are represented but not enforced

**Manuscript locations:** [§3 L203–213](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:203), [§3 L217–224](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:217), and Table 1 at [§3 L152–153](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:152).

The current qualifier at L211–213—“the in-memory implementation does not enforce every legal transition”—is necessary but still too soft. The actual gap is more specific:

- Registration always returns `ACTIVE`, with a hard-coded 30-s lease and 5-s heartbeat period at [receiver.cpp L89](/media/william/blueicedrive/Github/IPI/cpp/src/api/receiver.cpp:89).
- `REGISTERED` and `SUSPENDED` exist as enums, but the in-memory lifecycle does not enter them at [types.hpp L82](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/types.hpp:82).
- Heartbeat checks existence and writes `ACTIVE`; it does not enforce time/lease state at [receiver.cpp L106](/media/william/blueicedrive/Github/IPI/cpp/src/api/receiver.cpp:106).
- `invokeService()` does **no** directory lookup or active/terminated-state check and always creates an `IN_PROGRESS` response at [receiver.cpp L146](/media/william/blueicedrive/Github/IPI/cpp/src/api/receiver.cpp:146).
- `submitTelemetry()` also creates/extends an entry without a session lookup at [receiver.cpp L154](/media/william/blueicedrive/Github/IPI/cpp/src/api/receiver.cpp:154).
- The in-memory service adapter creates no `COMPLETED` or `REJECTED` result for session invocation. It emits only `IN_PROGRESS`; non-session service APIs create `ACCEPTED` at [receiver.cpp L56](/media/william/blueicedrive/Github/IPI/cpp/src/api/receiver.cpp:56).
- `ReceiverApi` declares patch and terminate at [receiver.hpp L29](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/receiver.hpp:29), but neither `PrivateSessionTransport` nor `Edge4AvInterface` exposes corresponding facade operations at [private_session_transport.hpp L47](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/private_session_transport.hpp:47) and [edge4av_interface.hpp L91](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/edge4av_interface.hpp:91).

Thus §3 L223–224 currently overstates user-visible API behavior: the lower-level receiver has patch/terminate methods, but the advertised high-level private-session transport/facade does not.

**Safe replacement for §3 L203–213:**

> The protocol types describe the intended lifecycle: \emph{request $\rightarrow$ accepted or in progress $\rightarrow$ zero or more updates $\rightarrow$ complete or reject}. The in-memory reference is intentionally narrower: registration returns an active session, a known-session heartbeat restores the active state, and a session invocation records an in-progress response. It does not validate that an invocation or telemetry submission names an active session, generate completed or rejected service responses, enforce transition ordering, or advance a request automatically to completion. The sequence is therefore a protocol model rather than an enforced state machine in the current adapter.

**Safe replacement for §3 L217–224:**

> A registration accepts an envelope, vehicle profile, requested services, optional event-subscription data, optional RSU fallback, and optional minimum sidelink RSSI. The descriptor returns the session ID, vehicle profile, transport annotation, state, lease, heartbeat period, preferred channels, and granted services. The type system declares registered, active, suspended, and terminated states; the current in-memory receiver returns active on registration and supports lower-level explicit termination, but does not implement lease expiry or a complete state-transition machine. The high-level facade currently exposes registration, heartbeat, invocation, telemetry, and response retrieval; patch and termination remain lower-level receiver operations.

**Safe replacement for the Table 1 response/acknowledgement behavior cell:**

> The types declare accepted, rejected, in-progress, and completed service states. The in-memory adapter emits accepted or in-progress responses and returns unknown-session acknowledgements only for heartbeat, patch, and termination; it does not generate a complete structured failure taxonomy or terminal session-service outcomes.

**Implementation required for a stronger claim:**

- an explicit session state machine;
- active/terminated/unknown checks on invocation and telemetry;
- lease timer and restart/recovery policy;
- public patch and termination methods on the facade/transport;
- terminal result generation, correlation uniqueness, and state-transition tests;
- structured errors rather than an optional `Ack::id` string.

## P0-4. The private-5G probe is a parsing/acceptance RTT harness, not a completed stateful CAV-service execution

**Manuscript locations:** [§3 L264–285](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:264), especially L269–282.

The paragraph says that adapters frame “correlated requests and acknowledgements” and log “application outcome.” That wording exceeds the code:

- `Private5gProbeRequest` carries a textual request ID and optional session ID, but `Private5gProbeAck` does not echo either; it contains sequence, client send time, frame type, payload size, `accepted`, and free-form detail at [private_5g_latency_probe.hpp L16](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/private_5g_latency_probe.hpp:16).
- The receiver only parses/inspects the frame and marks accepted/rejected based on that parsing; it does not invoke a planner, service, or cooperative lifecycle at [private_5g_latency_receiver.cpp L168](/media/william/blueicedrive/Github/IPI/cpp/examples/library/private_5g_latency_receiver.cpp:168).
- The regular sender accepts an ACK without checking that its sequence or client send time matches the outstanding request at [private_5g_latency_sender.cpp L510](/media/william/blueicedrive/Github/IPI/cpp/examples/library/private_5g_latency_sender.cpp:510) and [private_5g_latency_sender.cpp L552](/media/william/blueicedrive/Github/IPI/cpp/examples/library/private_5g_latency_sender.cpp:552).
- Logged `serviceSuccess` is `ack.accepted && args.context.serviceSuccess` at [private_5g_latency_sender.cpp L406](/media/william/blueicedrive/Github/IPI/cpp/examples/library/private_5g_latency_sender.cpp:406). The latter is caller-controlled and defaults true at [experiment_logging.hpp L14](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/experiment_logging.hpp:14), not an observed planner/vehicle outcome.
- The test probe uses fabricated synchronized-looking clock values at [private_5g_latency_probe_test.cpp L26](/media/william/blueicedrive/Github/IPI/cpp/tests/private_5g_latency_probe_test.cpp:26), while the actual experiment summary correctly limits unsynchronized field timing claims to RTT.

**Safe replacement for §3 L264–271:**

> The experiment adapters carry a versioned private-5G probe request and acknowledgement over TCP, MQTT, and UDP and log condition, transport, frame type, request metadata, timing, and parser acceptance. The acknowledgement indicates whether the receiver accepted and inspected the probe frame; it is not a `VehicleServiceResponse` or a completed planning, perception, control, or recovery result. The current `serviceSuccess` log field is a caller-supplied experiment annotation combined with acknowledgement acceptance, not an independently observed application outcome.

**Safe replacement for §3 L273–285:** use the test paragraph in P1-2 below.

**Implementation/experiment required for a stronger claim:**

- bind the field harness to `Edge4AvInterface` and a real session transport;
- execute a real service and return a distinct terminal result;
- echo/verify an unambiguous request and session correlation key;
- separate transport acceptance, parse acceptance, service admission, service completion, vehicle actuation, and safety outcome in logs;
- measure that end-to-end operation under the reported failure/restart conditions.

## P0-5. Fallback and transport-independence wording overstates retained behavior

**Manuscript locations:** [§3 L239–246](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:239).

The high-level distinction between application information and transport mechanisms is good. Two details need tightening:

1. `TransportType` is a coarse path-class enum (`C_V2X`, `CELLULAR_5G`, etc.), not a TCP/MQTT/UDP binding descriptor at [types.hpp L14](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/types.hpp:14).
2. `rsuFallback`, `minSidelinkRssi`, and `inlineSubscription` are accepted in `SessionRegistration` at [types.hpp L262](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/types.hpp:262), but the concrete receiver discards them: it stores only vehicle profile, transport, fixed lifecycle data, and requested services at [receiver.cpp L89](/media/william/blueicedrive/Github/IPI/cpp/src/api/receiver.cpp:89). They are not preserved in the returned descriptor or in the session directory.

**Safe replacement for §3 L239–246:**

> Transport independence here means that envelope metadata records a coarse path class without embedding TCP, MQTT, UDP, PC5, or 5G-NR protocol fields in the application object. TCP, MQTT, and UDP are separate probe-binding choices. The registration API accepts fallback-RSU and minimum-RSSI inputs as future-selection hooks, but the current in-memory receiver does not retain or act on them after registration. IPI therefore supplies a common object vocabulary for deployments that change paths; it does not implement path selection, fallback, or continuity in this paper.

# P1 — Important revisions before a defensible MobiCom novelty claim

## P1-1. “One interface” is correct as a C++ facade claim, but not as a deployed binding claim

**Residual mismatch:** [§3 L52–58](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:52) says both modes “draw from the same source, place, time, transport, and correlation vocabulary.” The parent’s envelope correction at L107–117 is accurate, but L57 and Figure 3 still imply that all stateful calls have that metadata.

Heartbeat and telemetry are direct session calls, not envelope calls; compare [types.hpp L271](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/types.hpp:271) with envelope metadata at [types.hpp L121](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/types.hpp:121).

**Safe replacement for §3 L52–58:**

> The stateful mode registers a profile, uses a session identifier for heartbeats or telemetry, submits envelope-based service requests, and retrieves session responses. Envelope-based message, registration, and service-request calls can share source, place, time, transport, and correlation metadata; direct heartbeat and telemetry calls are session-scoped and do not carry the full envelope. Only a stateful operation uses the service-lifecycle vocabulary.

**Figure repair:** Change “Common envelope” to **“Common metadata for envelope-based messages, registration, and service requests”** and add a small note: **“heartbeat/telemetry: session-scoped calls.”**

## P1-2. The test paragraph still overstates correlation, routing, and malformed-input coverage

**Manuscript locations:** [§3 L273–285](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:273).

The sentence still says tests cover “session topics” and “request correlation,” and that the test retrieves a “correlated session response.” The actual coverage is narrower:

- one `ServiceRequest` topic naming/parse round trip, not all topic families or MQTT-session routing: [private_session_transport_test.cpp L25](/media/william/blueicedrive/Github/IPI/cpp/tests/private_session_transport_test.cpp:25);
- registration, heartbeat, invocation, telemetry, and a response list keyed by session, not a correlation assertion: [edge4av_interface_test.cpp L49](/media/william/blueicedrive/Github/IPI/cpp/tests/edge4av_interface_test.cpp:49);
- one malformed case, a truncated PSM: [j2735_message_flow_test.cpp L186](/media/william/blueicedrive/Github/IPI/cpp/tests/j2735_message_flow_test.cpp:186);
- no MAP/SRM/SSM serialization tests;
- no unknown-session invoke/telemetry, terminal-state, expiry, fallback, broker-backed session, external J2735, PC5, ROS 2, or negative-correlation test.

**Exact safe replacement for §3 L273–285:**

> Seven automated CTests validate selected internal flows. They exercise BSM and PSM facade round trips; BSM/PSM profile behavior, one SPaT probe loopback, and one cooperative-message round trip; opaque TIM byte preservation; rejection of one truncated PSM; one service-request topic parse/format round trip; registration, heartbeat, invocation, telemetry, and session-keyed response retrieval; and TCP/MQTT latency-probe loopback. All seven pass in the current artifact. They do not cover MAP, SRM, or SSM serialization; every topic family or a broker-backed session binding; correlation enforcement; negative session states; lease expiry; fallback; external J2735 interoperability; or service execution and completion. They establish selected internal field preservation and validation behavior, not formal SAE conformance, production security, automatic fallback, or deadline satisfaction.

This paragraph is accurate, strong enough for a reference artifact, and less vulnerable to reviewer attack.

## P1-3. Partial validators should not be described as generic malformed-input protection

**Manuscript locations:** [§3 L127–130](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:127), Table 1 [§3 L149–151](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:149), and [§3 L248–257](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:248).

The present prose says broadly that the implementation rejects invalid ranges, lengths, truncation, and type mismatches. It does reject many selected cases, but not all of those classes:

- `IpiServiceRequest::from_canonical_encoding()` accepts trailing bytes and unknown flag bits; it validates only size/horizon, not an enum range or full consumption at [ipi_service_request.cpp L29](/media/william/blueicedrive/Github/IPI/cpp/src/core/ipi_service_request.cpp:29).
- `CooperativeServiceMessage::from_canonical_encoding()` accepts arbitrary cast enum values and does not require final buffer consumption at [ipi_cooperative_service.cpp L242](/media/william/blueicedrive/Github/IPI/cpp/src/core/ipi_cooperative_service.cpp:242).
- Cooperative validation does not require one payload matching its class, exactly one payload, or a semantically valid command/status relationship at [ipi_cooperative_service.cpp L69](/media/william/blueicedrive/Github/IPI/cpp/src/core/ipi_cooperative_service.cpp:69).
- The lightweight MAP, SPaT, SRM, and SSM validators enforce only selected profile bounds, e.g., MAP lane count and SPaT phase count at [j2735_messages.cpp L394](/media/william/blueicedrive/Github/IPI/cpp/src/v2x/j2735_messages.cpp:394).

**Safe replacement for the validation sentence:**

> For the exercised profiles, parsers reject selected range, declared-length, truncation, and requested-type errors. They are not yet complete hostile-input or semantic validators: some canonical parsers accept trailing bytes or unknown enum values, and a cooperative message need not match one service class to one content section.

This is a prose-only correction. Achieving a security/robustness claim requires validator hardening, fuzzing, negative tests, exact-consumption checks, enum validation, and a documented semantic policy.

## P1-4. PC5 and ROS 2 figure/caption claims need to distinguish source-tree examples from facade bindings

**Manuscript locations:** Figure 3 [§3 L83–101](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:83).

The code has useful device and conversion material, but not a single facade-to-device transport:

- Mocar examples are conditionally compiled only if the SDK/library is available at [CMakeLists.txt L107](/media/william/blueicedrive/Github/IPI/cpp/CMakeLists.txt:107).
- The current seven-test CTest configuration contains no Mocar or ROS 2 test at [CMakeLists.txt L145](/media/william/blueicedrive/Github/IPI/cpp/CMakeLists.txt:145).
- The SPaT bridge is a separate TCP receiver that decodes a local `SpatMessage`, maps it to the vendor structure, and sends through `mde_cv2x_spat_send()` at [spat_tcp_bridge.cpp L130](/media/william/blueicedrive/Github/IPI/cpp/examples/device/spat_tcp_bridge.cpp:130).
- It is not an `Edge4AvInterface` transport adapter.

**Safe prose:**

> The repository also contains optional ROS 2 conversions and separate Mocar device examples for selected standard-message callbacks. These examples are distinct from the in-memory `Edge4AvInterface` test path and do not establish an end-to-end facade-to-PC5, facade-to-ROS 2, or session-to-device binding.

# P2 — Lower-priority but worthwhile precision repairs

- **Table caption:** At [§3 L139–140](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:139), add: “Required and optional identify the intended object schema; they do not imply that every listed field is runtime-enforced by the reference implementation.” The structs are default-constructible, and source/intersection emptiness is deliberately not rejected.

- **Identifier scope:** §3 correctly says IDs are prototype policies at [§3 L113–117](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/03_ipi_protocol_design.tex:113), but should state that derived correlation is merely `std::to_string(request.requestId)` at [edge4av_interface.cpp L200](/media/william/blueicedrive/Github/IPI/cpp/src/api/edge4av_interface.cpp:200). A 16-bit request ID and per-process counters are not sufficient across vehicles, restart, or concurrent request originators.

  Safe addition:

  > The default message and derived correlation identifiers are process-local convenience values. Applications requiring uniqueness across vehicles, restarts, or concurrent requests must supply their own correlation identifier; a 16-bit request ID alone is not globally unique.

- **Freshness:** §3 L248–260 is mostly well scoped, but “let an application test” should be read literally. The current API does not evaluate expiration, has no correlation-indexed retrieval, and exposes no completed-response matcher. Add “when the application performs those checks” after “let an application test.”

- **Control wording:** The cooperative object bounds command count/value, but does not authenticate, authorize, safety-check, execute, or bind commands to a vehicle-control policy. The scope paragraph already excludes security; add “The current message is not a remote-control authorization or actuation interface” if control is foregrounded in the figure/text.

# Reviewer A — Protocol/API and networking-systems novelty

**Assessment:** The best defensible novelty is a deliberately narrow, typed reference contract that puts selected J2735-profile/opaque-message operations and stateful service metadata under one C++ facade, then uses that vocabulary to normalize the paper’s workload and readiness discussion. That can be useful, but it is not yet a strong standalone MobiCom systems contribution.

**High-priority novelty issue:** Section 2 already explains that VAE offers session procedures and that Tentacles provides generic QoS/timing/execution-state abstractions at [02_related_work.tex L21](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/02_related_work.tex:21) and [02_related_work.tex L80](/media/william/blueicedrive/Github/IPI/paper/manuscript/sections/02_related_work.tex:80). Section 3 must make the irreducible delta precise:

- What exact operation representation cannot be expressed by existing VAE/MEC/Tentacles interfaces?
- What is normative rather than merely represented by a C++ struct?
- Which promised advantages are shown by the artifact versus only anticipated by the contract?
- Why is this more than a measurement packet format plus a common metadata wrapper?

The outline correctly requires that IPI be an interface/protocol contribution rather than merely the experiment’s packet format at [paper_outline.md L1272](/media/william/blueicedrive/Github/IPI/paper/paper_outline.md:1272). The current implementation evidence supports that claim only if the text is very clear that IPI is a **reference application contract**, not a completed interoperable middleware/binding framework.

**Protocol/API findings:**

- One facade: supported in source at [edge4av_interface.hpp L25](/media/william/blueicedrive/Github/IPI/cpp/include/ipi/api/edge4av_interface.hpp:25).
- One deployed interface over PC5/Uu/MQTT/ROS 2: not supported.
- Stable cross-transport object vocabulary: supported at the type level.
- Same lifecycle semantics across transports: not supported; only the in-memory session model has an invocation API, and the field probe is separate.
- Explicit transport/fallback policy: represented as coarse annotations/hooks only, not operational.

**Reviewer A vote: Weak Reject.**

A substantially claims-disciplined revision could clear the correctness issue without new measurements. To clear the novelty bar more comfortably, the paper needs either a sharper formal/API comparison against VAE/MEC/Tentacles or evidence that the unified contract enables a capability/error classification/portability outcome that existing interfaces do not.

# Reviewer B — SAE J2735, CV/ITS standards, and interoperability

**Assessment:** The opaque pre-encoded path is appropriately conservative and is the strongest standards-facing element: it preserves caller-provided bytes without pretending to decode unknown J2735 types. The typed local profile and cooperative-service presentation are the danger zone.

**Fatal/high-priority standards concerns:**

1. A project-local serializer is exposed and labeled `UPER`; custom cooperative `UPER` is literally the canonical byte encoding. This must not imply SAE ASN.1/UPER conformance.
2. The custom cooperative type has no SAE assignment, no ASN.1 module, no defined regional-extension encoding, and no independent interoperability evidence.
3. The current Mocar material is separate optional device code, not proof that the common facade emits/receives interoperable J2735 wire frames.
4. The typed models are intentionally lightweight subsets; “preserve established J2735 content” is defensible only for the opaque route, not for a claim that the selected profiles preserve complete J2735 message semantics.

**Exact safe revision for §3 L18–24:**

> First, one interface should accept and preserve pre-encoded J2735 payloads while providing lightweight local profiles for selected message fields. The local profiles are a reference-model convenience, not a substitute for a complete deployed J2735 schema and codec.

**What is already good:**

- The current text says opaque payloads preserve bytes rather than decode them.
- It says custom cooperative bytes are not SAE-assigned.
- It avoids calling transport annotations a QoS reservation.
- It distinguishes device experiments from reference profiles in principle.

**Reviewer B vote: Reject.**

This vote is driven by correctness risk, not by the idea of IPI. The section should be reconsidered after the P0-1 language is fixed everywhere, including figure/table/nearby manuscript claims. If authors want a stronger interoperability claim, it needs actual external codec/device tests, not prose.

# Reviewer C — Distributed systems, sessions, transport, and failure semantics

**Assessment:** The section identifies the right distributed-systems concepts—correlation, timeout versus deadline, session scope, progress, terminal state, and fallback boundary. But the implementation is largely a data model with an in-memory recorder, not an operational session protocol. The manuscript must not blur that distinction.

**High-priority correctness concerns:**

- The implementation permits invocation/telemetry for unknown or terminated sessions.
- The session state machine is not operational; no expiry, no suspended state, no terminal service completion/rejection, no transition validation.
- Patch and termination are not accessible through the advertised high-level session transport/facade.
- No actual broker-backed session transport exists.
- A latency-probe ACK is conflated too closely with correlated service behavior and application outcome.
- Fallback/RSSI/subscription inputs are not retained or acted on.
- Correlation metadata is copied but neither globally unique nor enforced/queried as a key.

The section’s failure-boundary prose should be recast as: **these fields make a full implementation possible and let a caller perform its own check; the current reference implementation does not perform it.**

**Reviewer C vote: Weak Reject.**

The section could become a credible “reference semantics and limitations” description after P0/P1 prose repairs. It cannot claim a deployable stateful-service transport without the corresponding implementation and negative/failure tests.

# Prioritized adjudication checklist

1. **P0 prose repair — required before any submission draft**
   - Replace all lingering formal-UPER/regional-extension implications.
   - Recast MQTT/session topics as naming conventions plus an unimplemented adapter.
   - Recast PC5/ROS 2 as separate optional examples, not facade bindings.
   - Explicitly distinguish lifecycle model from current in-memory behavior.
   - Replace probe “application outcome” wording with parse/acceptance and caller annotation.
   - Correct Figure 3 and Table 1 at the same time; otherwise readers will infer the stronger claim from the visual/table even if prose is qualified.

2. **P1 prose repair — required for internal consistency**
   - Fix “both modes share vocabulary” to account for direct heartbeat/telemetry.
   - Replace the test paragraph with the bounded version above.
   - Clarify partial validation, correlation scope, freshness checks, fallback retention, and patch/terminate API scope.
   - State the precise novelty delta from VAE/MEC/Tentacles rather than letting “one interface” stand as a self-evident systems contribution.

3. **P1 implementation and validation — required only if retaining stronger systems claims**
   - Implement a real session state machine, expiry, state checks, terminal service outcomes, and structured error codes.
   - Add facade-level patch/terminate APIs and test negative session cases.
   - Implement an MQTT session adapter and broker integration tests.
   - Preserve/use fallback, RSSI, and subscription fields.
   - Add correlation verification and completion matching.
   - Expand tests to MAP/SRM/SSM, all topics, malformed/trailing/enum input, terminal/error paths, and state transitions.

4. **P2 standards/interoperability work — required for standards claims**
   - Replace local `UPER` terminology or implement/validate formal ASN.1/UPER.
   - Define any actual extension mechanism/assignment and test against an independent stack.
   - Demonstrate facade-to-PC5/device interoperation instead of relying on separate examples.

5. **Decision**
   - With only P0/P1 prose repairs, Section 3 can be accurate as a **reference interface and workload-normalization contract** supporting the larger field-evaluation paper.
   - With current wording, it invites fatal reviewer objections on standards conformance, session/broker binding, and application-completion semantics.
   - With the stronger implementation/validation work, it could support a substantially stronger protocol/middleware contribution; that is not what the current source/tests establish.

<oai-mem-citation>
<citation_entries>
MEMORY.md:565-577|note=[current IPI manuscript authority and evidence boundary]
</citation_entries>
<rollout_ids>
019f143f-e74c-77d3-a4f2-b3ec2647ef6c
</rollout_ids>
</oai-mem-citation>
