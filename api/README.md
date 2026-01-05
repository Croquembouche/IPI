# Intersection Programming Interface (IPI) Message Fabric

IPI links intelligent intersections with personal connected vehicles (PCVs), connected and autonomous vehicles (CAVs), pedestrians, cloud services, and neighboring intersections. The interface spans three coordinated communication planes, each optimized for its traffic pattern while remaining faithful to the SAE J2735 message set wherever possible:

1. **RSU Broadcast Plane (RSU ↔ OBU)** – connectionless SAE J2735 broadcasts over DSRC or C-V2X. Reserved for safety-critical, “everyone hears it” messages such as SPaT, MAP, BSM, SRM, and SSM (supports F00-0000/F00-0001 and applications A00-0000..A00-0004).
2. **5G Session Plane (Infrastructure ↔ CAV/advanced PCV)** – tunneled SAE J2735 `MessageFrame` objects carried over MQTT 5.0/TLS 1.3 on a 5G data channel. This delivers authenticated, stateful exchanges for service requests, acknowledgements, and telemetry (supports F01-0000..F02-0001, F04-0000..F04-0001, A01-0000..A06-0000).
3. **Backhaul Control Plane (Infrastructure ↔ Cloud / Control Center)** – persistent HTTP/2 (REST or gRPC) sessions that exchange SAE J2735 payloads encapsulated in JSON or binary envelopes for configuration, analytics exports, and inter-intersection coordination (supports F05-0000, A05-0000, A06-0000).

In addition, IPI defines an optional **Vehicle–Vehicle Local Mesh Mode** used when infrastructure and backhaul are unavailable (for example, city-wide power outages). In this mode, nearby AVs and CVs form an ad‑hoc mesh and continue exchanging safety and cooperative guidance data using the same J2735 + IPI extensions described below.

Adapters MAY expose additional internal protocols (e.g., AMQP, Kafka), but the external contracts above are normative for IPI deployments. Receivers MUST ignore unknown or unrecognized fields when decoding messages. This permissive parsing rule applies to all planes and allows different vendors or newer versions to introduce additional data without breaking interoperability.

---

## J2735 Alignment Strategy

* Every payload transported by IPI is encoded as an SAE J2735 `MessageFrame`. Message frames are serialized with ASN.1 UPER for radio/MQTT transport and may be wrapped in Base64 when sent over HTTP.
* Receivers MUST treat any unrecognized `regional` extensions, supplemental elements, or additional JSON fields as optional and ignore them while continuing to process known data. Clients SHOULD log the presence of unknown data for observability but MUST NOT reject or drop the message solely because of it. This enables vendor-specific or future enhancements without requiring simultaneous upgrades across all participants.
* Vehicles reuse existing J2735 message types for identification and simple service requests:
  * **BSM (Basic Safety Message)** carries vehicle identification, kinematics, and optional `SupplementalVehicleExtensions` to tag the desired service type. This applies even over 5G so that infrastructure can correlate radio and cellular footprints without inventing new headers.
  * **SRM (Signal Request Message)** remains the mechanism for priority/availability requests at intersections. PCVs use SRM along with a complementary **SSM** response for confirmation (A01-0000/A01-0001/A03-0000).
  * **TIM (Traveler Information Message)** distributes advisories or warnings (A04-0000) using the standard J2735 structure.
* When the existing J2735 catalogue lacks a construct (e.g., guided planning/perception/control for CAVs), IPI defines an extension that still fits inside the J2735 `MessageFrame` using the `regional` extension mechanism.

The result: legacy vehicles can interpret broadcasts as before, 5G-connected vehicles reuse familiar encodings, and only advanced cooperative automation relies on new message schemas.

---

## Message Catalogue

| Functional Need | Message Type | Notes |
| --- | --- | --- |
| Vehicle self-identification, heartbeat | `BSM` (`MessageFrame.messageId = bsm(20)`) | Include `VehicleID`, `VehicleSize`, and `SupplementalVehicleExtensions.ipiServiceRequest` (see below). |
| Priority / availability requests | `SRM` (`signalRequestMessage(28)`) | PCVs and emergency vehicles notify intersection; infrastructure responds with `SSM`. |
| Pedestrian warnings | `TIM` (`travelerInformation(31)`) | TIM payload includes `itis` codes for warning categories; mirrored on RSU and MQTT. |
| MAP/SPaT dissemination | `MAP`, `SPAT` | Standard V2X broadcasts. |
| Cloud data backlog | `DataReceipt` (TIM variant) | Encoded TIM with metadata referencing cloud export job IDs. |
| Cooperative CAV services | `IPI-CooperativeService` (J2735 regional extension) | New schema detailed below. |
| Vehicle–vehicle mesh guidance | `IPI-CooperativeService` (J2735 regional extension over V2V link) | Same payload as above, but exchanged directly between vehicles when infrastructure is unavailable. |

---

## Vehicle–Vehicle Local Mesh Mode (Power Outage Scenario)

Some AVs depend heavily on external SPaT/MAP and cloud guidance. In a city‑wide power outage where intersections and backhaul may be offline, IPI enables nearby vehicles to continue cooperating by forming a local mesh network:

- **Discovery** – Vehicles periodically emit BSMs that include an `IPI-ServiceRequest` extension flagging `mesh_capable` and their preferred mesh radius. Peers that decode these advertisements add each other to an in‑memory neighbor table keyed by `temp_id`/`vehicleId`. Receivers MUST ignore unknown fields and unrecognized mesh extensions, so older software continues operating on the core BSM even if it does not participate in mesh.
- **Mesh session** – When infrastructure heartbeats (SPaT, MAP, 5G MQTT) are missing beyond a configurable timeout, a vehicle enters mesh mode. It continues to emit standard BSMs for basic V2X compatibility, and additionally exchanges `IPI-CooperativeService` messages with neighbors over whatever V2V link is available (C‑V2X sidelink, DSRC OBU‑to‑OBU, Wi‑Fi Direct).
- **Payload reuse** – The same `IPI-CooperativeService` payload used in the infrastructure‑backed 5G plane (guided planning, perception, control) is reused over the mesh, transported inside J2735 `MessageFrame` `regional` extensions. No new message families are required.
- **Implementation guidance** – The C++ reference library exposes an `ipi::mesh::MeshManager` helper that:
  - accepts decoded BSMs and infrastructure “heartbeat” events,
  - tracks neighbors and automatically switches into mesh mode when infrastructure is lost,
  - and periodically builds and broadcasts `IPI-CooperativeService` frames over an abstract mesh link.

This design keeps mesh behavior aligned with the rest of IPI: same J2735 frame family, same IPI extensions, permissive parsing of unknown fields, but with vehicles acting as both senders and receivers when infrastructure is unavailable.
| Vehicle–vehicle mesh guidance | `IPI-CooperativeService` (J2735 regional extension over V2V link) | Same payload as above, but exchanged directly between vehicles when infrastructure is unavailable. |

### Supplemental Extension for Service Requests

Vehicles add a service request extension inside the BSM to avoid duplicating identification fields:

```
BSM CORE
  -> Part II: SupplementalVehicleExtensions
       -> regional
            [ipiServiceRequest ::= IPI-ServiceRequest]
```

`IPI-ServiceRequest` (ASN.1 excerpt):

```
IPI-ServiceRequest ::= SEQUENCE {
  serviceType      ENUMERATED {
                      laneKeepingAid(0),
                      unprotectedLeftAvailability(1),
                      perceptionAid(2),
                      planningAid(3),
                      controlAid(4),
                      computationAid(5),
                      hdMapUpdate(6)
                    },
  requestId        INTEGER (0..65535),
  desiredHorizonMs INTEGER (0..10000) OPTIONAL,
  additionalData   OCTET STRING (SIZE(0..128)) OPTIONAL -- service-specific blob
}
```

*The same BSM is sent over RSU (broadcast) and, when a 5G session exists, tunneled over MQTT for redundancy. Infrastructure chooses the freshest copy based on received timestamp.*

### Cooperative Service Extension (Regional Message)

CAV-specific tasks rely on a new regional message embedded in `MessageFrame` with a reserved regional ID (`ipiRegionID`):

```
MessageFrame ::= CHOICE {
  ...
  ipiCooperativeService   [ipiRegionID] IPI-CooperativeService
}
```

```
IPI-CooperativeService ::= SEQUENCE {
  sessionId       OCTET STRING (SIZE(16)),
  vehicleId       VehicleReferenceID,
  serviceClass    ENUMERATED { guidedPlanning(0), guidedPerception(1), guidedControl(2) },
  guidanceStatus  ENUMERATED { request(0), update(1), complete(2), reject(3) },
  requestedHorizonMs INTEGER (0..60000) OPTIONAL, -- for requests
  planningData    IPI-GuidedPlanning OPTIONAL,
  perceptionData  IPI-GuidedPerception OPTIONAL,
  controlData     IPI-GuidedControl OPTIONAL,
  confidence      INTEGER (0..100) OPTIONAL,
  expirationTime  DTime OPTIONAL
}
```

Supporting structures:

```
IPI-GuidedPlanning ::= SEQUENCE {
  waypoints       SEQUENCE (SIZE(1..50)) OF IPI-Waypoint,
  fallbackRoute   BOOLEAN DEFAULT FALSE
}

IPI-Waypoint ::= SEQUENCE {
  position        Position3D,
  targetSpeed     INTEGER (0..32767) OPTIONAL, -- 0.01 m/s units
  dwellTimeMs     INTEGER (0..10000) OPTIONAL
}

IPI-GuidedPerception ::= SEQUENCE {
  detectedObjects SEQUENCE (SIZE(1..64)) OF IPI-DetectedObject
}

IPI-DetectedObject ::= SEQUENCE {
  objId           OCTET STRING (SIZE(4)),
  classification  ENUMERATED { vehicle(0), pedestrian(1), cyclist(2), obstacle(3) },
  position        Position3D,
  velocity        INTEGER (-32768..32767) OPTIONAL, -- 0.01 m/s units
  covariance      OCTET STRING (SIZE(0..32)) OPTIONAL
}

IPI-GuidedControl ::= SEQUENCE {
  commands        SEQUENCE (SIZE(1..10)) OF IPI-ControlCommand
}

IPI-ControlCommand ::= SEQUENCE {
  axis            ENUMERATED { steering(0), throttle(1), brake(2) },
  value           INTEGER (-1000..1000) -- permille units
}
```

These extensions exist solely for advanced CAV cooperation. All other flows reuse existing J2735 types, keeping code paths familiar for OEMs and infrastructure vendors.

---

## Plane 1 – RSU Broadcast (SAE J2735)

| Direction | PSID / Message | Description | QoS |
| --- | --- | --- | --- |
| Inbound | `0x20` (BSM) | Vehicle identification + `IPI-ServiceRequest` extension. | Mandatory |
| Inbound | `0x2C` (SRM) | Priority/availability requests. | Mandatory |
| Outbound | `0x2B` (SPaT) | Signal phase & timing ≤100 ms cadence. | Mandatory |
| Outbound | `0x2A` (MAP) | Lane geometry updates. | Optional |
| Outbound | `0x2D` (SSM) | Response to SRM with granted phases. | Optional |
| Outbound | `TIM` | Pedestrian warnings (`itis` codes). | Optional |

*Workflow*
1. RSU receives BSM/SRM, validates SCMS signature, and forwards decoded frames to the 5G session orchestrator.
2. Outbound MAP/SPaT/SSM/TIM frames are generated by infrastructure logic (possibly informed by MQTT/HTTP data) and re-encoded into SAE J2735 for broadcast.

---

## Plane 2 – 5G MQTT Session Plane

* **Transport** – MQTT 5.0 over TLS 1.3. Payloads are J2735 `MessageFrame` objects encoded with UPER and Base64 in the MQTT payload.
* **Authentication** – SCMS-derived pseudonym certificates or SIM-based credentials bound to the vehicle VIN.
* **Topics** (receivers ignore unknown MQTT user properties and silently skip unpublished regional data they do not support)

| Topic | Direction | Payload | Functional Coverage |
| --- | --- | --- | --- |
| `ipi/{intersectionId}/session/register` | vehicle → infrastructure | BSM with `IPI-ServiceRequest.serviceType=registration`. | F02-0000 (init), F01-0000 |
| `ipi/{intersectionId}/session/{vehicleId}/events` | infrastructure → vehicle | `IPI-CooperativeService` (status updates) or TIM. | F02-0001 |
| `ipi/{intersectionId}/session/{sessionId}/heartbeat` | vehicle → infrastructure | BSM heartbeat (no extension) every `heartbeatIntervalSeconds`. | F02-0000 |
| `ipi/{intersectionId}/session/{sessionId}/service/request` | vehicle → infrastructure | `IPI-CooperativeService` (`guidanceStatus=request`). | F02-0000 |
| `ipi/{intersectionId}/session/{sessionId}/service/update` | infrastructure → vehicle | `IPI-CooperativeService` (`guidanceStatus=update/complete`). | F02-0001 |
| `ipi/{intersectionId}/session/{sessionId}/telemetry` | vehicle → infrastructure | BSM (primary), optional `IPI-CooperativeService` with `perceptionData`. | Enables A02-0000..A02-0006 |
| `ipi/{intersectionId}/pcv/{vehicleId}/response` | infrastructure → vehicle | SSM/TIM mirrored over MQTT for reliable delivery. | F01-0001 |

* **Session lifecycle** mirrors the earlier description but each control frame is a J2735 `MessageFrame`.

---

## Plane 3 – HTTP Backhaul (Infrastructure ↔ Cloud)

* **Base URL** – `https://cloud.example.gov/api/v1/intersections/{intersectionId}`
* **Encoding** – HTTP bodies carry Base64-encoded J2735 `MessageFrame` objects within JSON:

```json
{
  "messageType": "TIM",
  "encoding": "UPER",
  "payload": "BASE64(J2735)"
}
```

* **Endpoints**

| Method & Path | J2735 Payload | Purpose |
| --- | --- | --- |
| `POST /sessions/{sessionId}/events` | `IPI-CooperativeService` | Push session state to cloud dashboards. |
| `POST /telemetry/batch` | Array of `BSM` frames | Upload aggregated telemetry. |
| `POST /cloud/exports` | TIM (with data backlog metadata) | Request backlog jobs. |
| `GET /cloud/exports/{jobId}` | TIM response | Retrieve job status/URI. |
| `POST /interchange/outbound` | TIM | Forward data packages to peers. |
| `GET /configuration` | MAP/SPaT/TIM bundle | Pull latest signal & map configurations. |
| `PATCH /configuration` | TIM acknowledgement | Report applied configuration. |

---

## Cross-Plane Considerations

* **Correlation** – `requestId` inside `IPI-ServiceRequest` and `sessionId` inside `IPI-CooperativeService` align radio, MQTT, and HTTP records.
* **Security** – All planes use SCMS certificates. HTTP adds OAuth 2.0 or mTLS. Certificate revocation lists are synchronized over HTTP and cached at the edge.
* **Latency Targets** –
  * RSU broadcast loop: ≤ 50 ms from radio reception to local processing.
  * PCV assistance via BSM/SRM over MQTT fallback: ≤ 200 ms end-to-end.
  * CAV cooperative responses (`IPI-CooperativeService`): ≤ 300 ms for full guidance; ≤ 100 ms for acknowledgement.
  * HTTP telemetry uploads: aggregated within 1 s windows; critical events streamed immediately.
* **Privacy** – Vehicles rotate SCMS pseudonyms per policy. Registration BSM payloads include hashed VIN only when required by jurisdiction.

---

## Implementation Notes

* Vehicles without 5G connectivity rely solely on RSU broadcasts; infrastructure mirrors critical responses (SSM/TIM) over both planes when available.
* MQTT brokers should perform payload validation against J2735 schemas to catch malformed frames before routing to safety services.
* Future collective perception (CPM) efforts can be layered by defining additional `regional` extensions within `IPI-CooperativeService` without altering the transport fabric.
* Simulation harnesses should emit authentic J2735 frames (BSM, SRM, TIM, IPI-CooperativeService) to validate end-to-end latency and cooperative behaviors.

---

## Developer Enablement

* Provide reference encoder/decoder libraries (C/C++, Rust, Python) that wrap the J2735 ASN.1 definitions plus the `IPI-ServiceRequest` and `IPI-CooperativeService` extensions.
* Publish message dictionaries and example payloads (hex and JSON representations) for every supported scenario, including success and failure cases.
* Supply conformance test vectors that validate proper handling of unknown fields, signature verification, and latency budgets.
* Offer a containerized “intersection in a box” simulator that emits RSU broadcasts, accepts MQTT connections, and exposes HTTP backhaul endpoints for integration testing.

## Interoperability & Certification

* Establish an IPI extension registry where vendors reserve `ipiRegionID` subtypes and document their semantics before deployment.
* Define profile levels (e.g., Basic RSU, Enhanced 5G, Full CAV) that specify which message types, transports, and services are mandatory, optional, or unsupported.
* Require conformance testing before production onboarding; test suites should include malformed frame injection, clock drift scenarios, and replay attacks.
* Maintain backward-compatibility policies: new fields must be optional; removal of fields requires a deprecation window communicated via TIM advisories and release notes.

## Safety & Failover Practices

* Validate every inbound frame against SCMS signatures and enforce freshness using timestamp tolerances (e.g., ±1 s for safety messages).
* If message parsing fails, fall back to the last known safe state and continue broadcasting conservative guidance (e.g., deny SRM, issue caution TIM).
* Prioritize redundancy by simultaneously delivering critical responses over RSU and MQTT, and confirm acknowledgements via HTTP backhaul when possible.
* Implement rate limiting per vehicle/session to guard against flooding attacks while ensuring emergency vehicles can override limits when authorized.

## Observability & Diagnostics

* Log message provenance, `requestId`, `sessionId`, transport, and outcome (`ACCEPTED`, `REJECTED`, `IGNORED_UNKNOWN_EXTENSION`) with millisecond timestamps.
* Export metrics such as end-to-end latency, frame acceptance rates, unknown-field occurrences, heartbeat misses, and radio RSSI trends.
* Support distributed tracing by propagating `traceId` across RSU, MQTT, and HTTP flows when possible; include references in TIM cloud notifications.
* Provide remote diagnostics endpoints (HTTP) to pull recent logs, schema versions, and certificate status for maintenance crews.

## Scalability & Capacity Planning

* Dimension RSU and MQTT broker capacity for peak events (e.g., stadium egress) with 5× safety margin compared to average hourly load.
* Use hierarchical topic namespaces (e.g., district-level prefixes) and broker clustering to distribute load across multiple edge nodes.
* Cache frequently broadcast MAP/SPaT data at RSUs and employ differential updates to reduce bandwidth.
* Schedule cloud export jobs during off-peak intervals and throttle large data uploads to avoid starving real-time guidance.

---

By standardizing on SAE J2735 encodings for all routine exchanges, embracing permissive parsing, and establishing strong tooling, certification, safety, observability, and scalability practices, the IPI minimizes integration overhead while enabling advanced guidance capabilities for CAVs.

---

## Using the C++ reference implementation

This document defines the functional and message-level API. The repository also
ships a C++17 reference implementation under `cpp/` that you can use to
prototype against this design:

- Build the library and examples:

  ```bash
  cmake -S cpp -B cpp/build
  cmake --build cpp/build
  ```

- Link your application against `cpp/build/libipi.a` and include headers from
  `cpp/include`. The key namespaces mirror the concepts in this document:
  - `ipi::api` – `ReceiverApi` and `SenderApi` mirror the HTTP/MQTT planes.
  - `ipi::core` – `CooperativeServiceMessage` and related payloads.
  - `ipi::v2x` – lightweight J2735 message models (BSM, MAP, SPaT, SRM, SSM).
  - `ipi::mesh` – `MeshManager` for the Vehicle–Vehicle Local Mesh Mode.

As a simple example, an edge-side process that ingests a BSM over V2X and
rebroadcasts it over 5G MQTT could:

1. Decode the raw bytes into `ipi::j2735::BasicSafetyMessage`.
2. Pack it into an `ipi::api::J2735Payload` and wrap it in an
   `ipi::api::Envelope`.
3. Call `ipi::api::ReceiverApi::ingestV2xMessage` on the in-memory API
   implementation returned by `make_in_memory_receiver_api()`.

For vehicle-side mesh experiments, the `example_mesh_demo` executable in
`cpp/build` shows how to:

```bash
./cpp/build/example_mesh_demo
```

It constructs a `MeshManager`, feeds it local telemetry and synthetic BSMs from
neighbors, simulates loss of infrastructure heartbeats, and logs the
`IPI-CooperativeService` frames it would broadcast over a V2V mesh.
