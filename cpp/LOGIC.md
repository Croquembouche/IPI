# IPI C++ Reference Library

This directory contains a C++17 reference implementation of the core data
structures described in the Intersection Programming Interface (IPI) message
fabric. The goal is to provide a lightweight starting point for integrating the
IPI-specific regional extensions into production J2735 pipelines.

```
cpp/
  include/
    ipi/
      core/   # IPI service request/response models
      v2x/    # J2735 helpers, UPER codec, optional ROS2 bridge adapters
  src/
    core/
    v2x/
  examples/
    library/  # host-side sanity tools
    device/   # hardware-facing demos (Mocar, ROS2 broadcaster)
```

## Features

- Canonical representations for `IPI-ServiceRequest` and `IPI-CooperativeService`
  payloads.
- Basic J2735 V2X message helpers for BSM, MAP, SPaT, SRM, and SSM frames with
  canonical byte encoders.
- `ipi::v2x::UperCodec` façade implementing ASN.1 UPER packing for the helper
  structs, ready to be swapped with a production encoder when available.
- Optional ROS 2 bridge helpers (`IPI_ENABLE_ROS2_BRIDGE=ON`) that translate
  between the lightweight models and the `v2x_msg` ROS message types used on the
  MQTT/ROS buses.
- `ipi::api::ReceiverApi` / `ipi::api::SenderApi` interfaces that implement the
  HTTP/MQTT semantics described in `api/README.md`, with in-memory reference
  implementations for rapid prototyping.
- Canonical byte-level encoders/decoders (non-ASN.1) suitable for early
  integration testing.
- Edge-to-device bridge plan: a minimal TCP link between the edge unit and the
  on-device process over Ethernet. The on-device process listens on a TCP port,
  receives Base64-encoded UPER `MessageFrame` payloads, and forwards them to
  the Mocar SDK (and vice versa). Keep framing simple (length-prefixed messages
  with a short header carrying `sessionId`/`requestId`), and ensure the edge
  side can fall back to local MQTT/HTTP when the TCP link drops.
- Validation helpers that enforce the bounds and cardinality constraints
  documented in the API design.
- Example executable demonstrating round-trip encoding/decoding.

## Edge ↔ Device TCP Link (sketch)

TCP over Ethernet is reasonable here: it preserves order, retries on loss, and
keeps the bridge simpler than introducing another MQTT broker hop. UDP would
only be needed if we find TCP head-of-line blocking hurts latency; start with
TCP and measure.

- Port and roles: device listens on `0.0.0.0:35555` (configurable via
  `IPI_DEVICE_TCP_PORT`); edge initiates and reconnects on drop with exponential
  backoff capped at ~5 s.
- Framing: network-byte-order `uint32` length prefix for the whole frame, then
  a fixed header:

```
struct LinkHeader {
  uint8_t version;      // e.g., 1
  uint8_t msg_type;     // 0=data, 1=heartbeat, 2=ack, 3=error
  uint8_t flags;        // bit0 = requires_ack, bit1 = is_response
  uint8_t reserved;     // align to 4 bytes
  uint32_t request_id;  // correlates to IPI requestId; 0 when not applicable
  uint8_t session_id[16];
};
```

  Payload that follows: Base64-encoded UPER `MessageFrame` for `msg_type=data`,
  empty for heartbeat/ack/error (or short ASCII error string for errors).
- Heartbeats: edge sends `msg_type=heartbeat` every 1–2 s when idle; device
  replies with `ack`. Close and reconnect after N (e.g., 3) missed heartbeats.
- Integration paths:
  - Device side: TCP listener → parse header → Base64-decode + UPER-decode via
    `ipi::v2x::UperCodec` → handoff to Mocar SDK send API (downlink) or to
    uplink queue for edge consumption.
  - Edge side: feed received frames into local MQTT/HTTP pipelines; source
    Mocar-bound traffic from the orchestrator, UPER-encode + Base64-wrap, and
    emit over TCP.
- Backpressure and errors: if write fails or read stalls > timeout, drop the
  socket and reconnect. Use `msg_type=error` to surface recoverable issues
  (e.g., schema mismatch).
- Security: default to the trusted VLAN between edge and device; optionally
  wrap the socket with TLS + client certs or require a pre-shared key HMAC on
  the header if the link leaves the trusted domain.

## Vehicle–Vehicle Local Mesh Mode (Power Outage Scenario)

Recent events highlighted that some AVs rely heavily on external SPaT/MAP and
cloud guidance. In a city‑wide power outage where intersections and backhaul
may be offline, we want vehicles within proximity to form a local mesh so they
can continue exchanging safety and perception data.

High‑level goals:

- Allow AVs/CVs within a configurable radius to discover one another and
  establish an ad‑hoc local area network (LAN) without relying on powered
  infrastructure.
- Reuse the existing J2735 + IPI encodings (BSM + `IPI-ServiceRequest` and
  `IPI-CooperativeService`) so vendors do not need new message families.
- Keep the mesh logic orthogonal to the RSU/edge logic so vehicle software can
  reuse it in any environment.

Planned design sketch:

- **Transport** – use whatever direct V2V link is available on the platform
  (C‑V2X sidelink, 802.11p/DSRC OBU‑to‑OBU, or Wi‑Fi Direct). From the IPI
  library’s perspective this is abstracted behind a small "mesh link" adapter
  that exposes `send(MessageFrame)` / subscription callbacks.

- **Discovery** – vehicles periodically emit a BSM with an
  `IPI-ServiceRequest` extension indicating "mesh_capable" and their desired
  mesh radius. Peers that receive compatible advertisements add each other to
  an in‑memory neighbor table keyed by `temp_id`/`vehicleId`.

- **Mesh session** – peers use the existing `IPI-CooperativeService` payload
  as their shared data frame for:
  - minimal cooperative planning hints (e.g., reserved gap, planned stop line),
  - perception overlays (e.g., pedestrian or obstacle detections),
  - control hints (only for advisory, never hard override).

- **Failure mode switch** – when a vehicle detects loss of upstream SPaT/MAP
  (e.g., no valid intersection broadcasts for N seconds) and cannot establish
  a 5G session, it enters **mesh mode**:
  - keep emitting BSM as usual for basic V2X compatibility,
  - additionally publish periodic `IPI-CooperativeService` frames to neighbors
    over the mesh link with local perception summaries and intent.

- **Integration with IPI C++ library** – a future `ipi::mesh` module will wrap
  this behavior so vehicle applications can:
  - pass decoded BSMs (from Mocar or other OBUs) into a mesh manager,
  - register callbacks for neighbor updates and cooperative guidance,
  - send `CooperativeServiceMessage` instances out over the selected mesh
    transport.

This design keeps the mesh behavior consistent with the rest of IPI: same J2735
frame family, same IPI extensions, but with peers acting as both senders and
receivers when infrastructure is unavailable.

## Cooperative Task Offloading Function

In addition to the mesh mode, the C++ stack now exposes a *task offloading*
function that lets a vehicle or intersection offload compute-heavy workloads
to nearby peers or to the edge/cloud when resources are constrained:

- **Trigger** – any participant detects a degraded compute budget (CPU thermal
  throttling, inference backlog, perception stack failure, etc.) and emits an
  `IPI-ServiceRequest` with `serviceType=computationAid`. The request may flow
  over RSU broadcast, 5G MQTT, or—when in mesh mode—directly between vehicles.
- **Negotiation** – infrastructure or mesh peers respond with an
  `IPI-CooperativeService` message whose `serviceClass=GuidedControl /
  GuidedPerception / GuidedPlanning` and includes a `GuidedPlanningPayload` or
  `GuidedPerceptionPayload` describing the work they will assume. When the
  request targets pure compute (e.g., re-running a perception model), the
  payload’s `additionalData` field carries the serialized task descriptor
  (sensor frames, HD map delta, etc.).
- **Data movement** – the requester streams supporting telemetry frames (BSM,
  cooperative perception slices, etc.) through the same plane used for the
  negotiation. When only a subset of data is required, the `CooperativeService`
  payload may include `requestedHorizonMs` and `expirationTime` fields to scope
  how long offloading remains valid.
- **Completion** – once the assisting node finishes the offloaded computation,
  it emits an `IPI-CooperativeService` with `guidanceStatus=Complete` (or
  `Reject` if it could not honor the request). Both parties log the outcome in
  session state for auditability.

The offloading helper builds on existing message families, so no additional
J2735 types are necessary. Vehicles can use the same `ipi::mesh::MeshManager`
or `ipi::api::ReceiverApi` pathways to route the requests; the only difference
is the `serviceType` advertised and the richer cooperative payload that flows
back when another node accepts the task.

## Building

```bash
cmake -S cpp -B cpp/build
cmake --build cpp/build
```

The build creates `libipi.a` plus the core sample executables
`cpp/build/example_build_service_request` and
`cpp/build/example_v2x_roundtrip`. Optional demos are enabled through the
configuration flags described below.

To use the ROS 2 bridge, configure the build inside a ROS 2 workspace and
enable the option:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_ROS2_BRIDGE=ON
```

The bridge compilation assumes the `v2x_msg` package has been built so that the
generated headers are discoverable by the compiler.

Mocar SDK integration is optional. Enable `IPI_ENABLE_MOCAR_EXAMPLES` when the
target environment provides a compatible `libmocarcv2x.so`. The current SDK
drop lives under `third_party/mocar/new_V2X_64bit`, with binaries in
`third_party/mocar/new_V2X_64bit/aarch64_libs` and sample code in
`third_party/mocar/new_V2X_64bit/code/mde_64cv2x`:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_MOCAR_EXAMPLES=ON
```

The Mocar-based demo assumes the SDK root is available at build time; override
`MOCAR_SDK_ROOT` if you stage it elsewhere:

```bash
MOCAR_SDK_ROOT=/opt/mocar/new_V2X_64bit cmake -S cpp -B cpp/build -DIPI_ENABLE_MOCAR_EXAMPLES=ON
```

Adjust `MOCAR_LOG_CFG` (or edit the `DEFAULT_LOG_CONFIG` macro) before running
on the device:

```bash
MOCAR_LOG_CFG=/path/to/mocar_log.conf ./cpp/build/example_mocar_ipi
```

When both `IPI_ENABLE_ROS2_BRIDGE` and `IPI_ENABLE_MOCAR_EXAMPLES` are enabled
inside a ROS 2 workspace, the sample `example_ros2_bsm_broadcaster` subscribes
to a `v2x_msg::msg::BSM` topic and rebroadcasts those detections over the Mocar
hardware using the UPER codec and bridge helpers.

## Next Steps

- Replace the canonical encoders with bindings to the project’s chosen J2735
  ASN.1 toolchain (e.g., asn1c) while retaining the validation logic.
- Add unit tests (e.g., using GoogleTest or Catch2) to exercise error paths and
  boundary conditions.
- Extend the library with transport adapters that embed the encoded payloads in
  RSU, MQTT, or HTTP flows as outlined in the message architecture.
