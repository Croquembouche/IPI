# Intersection Programming Interface (IPI)

The IPI project defines a communications fabric that links intelligent
intersections with connected vehicles (CVs), connected and autonomous vehicles
(CAVs), pedestrians, peer intersections, and cloud services. This repository
captures both the specification and reference implementations needed to explore
the architecture end-to-end.

## Repository Layout

| Path | Description |
| --- | --- |
| `references/README.md` | Human-readable API design covering transport planes, topic/end-point contracts, payload schemas, rate limits, and operational guidance. |
| `instructions.md` | Operator runbook for running radio and private 5G latency measurements across dedicated device roles. |
| `setup.md` | Deployment guide for building, staging, and bringing up the current IPI reference implementation. |
| `experiment.md` | Edge4AV experiment tracker for recording run IDs, condition IDs, metrics, and paper-ready results. |
| `web/experiment-tracker/` | Dependency-free browser UI for entering experiment results, tracking readiness, and exporting JSON or Markdown snapshots. |
| `cpp/` | C++17 reference library implementing the data models, UPER encoding helpers, optional ROS 2/Mocar adapters, vehicle–vehicle mesh helpers, and in-memory receiver/sender APIs. |
| `third_party/mocar/` | Vendor SDK artifacts and samples for Mocar RSU/OBU devices (used by the optional hardware demos). |
| `v2x_msg/` | ROS 2 message definitions for V2X (required when building the ROS integration). |

## C++ Reference Implementation

The `cpp/` tree exposes several reusable components:

- `ipi::core` – strongly-typed representations of the IPI request/response data
  structures, including the BSM `IPI-ServiceRequest` extension and the
  `IPI-CooperativeService` regional payload used for guided planning/
  perception/control.
- `ipi::v2x` – simplified SAE J2735 helpers (BSM, MAP, SPaT, SRM, SSM) plus the
  `UperCodec` for ASN.1 UPER packing/unpacking and optional ROS 2 bridge
  adapters (`Ros2Bridge`) to translate between ROS messages and the lightweight
  structs.
- `ipi::api` – interfaces (`ReceiverApi`, `SenderApi`) that mirror the HTTP and
  MQTT semantics in the spec. Factory helpers produce thread-safe in-memory
  implementations so you can prototype against the contract without deploying a
  backend, and the private 5G latency probe helpers provide a concrete
  request/ack wire format for timing experiments over an IP path.
- `ipi::mesh` – a vehicle–vehicle mesh manager plus a cooperative task offloader
  used in power-outage or degraded-compute scenarios. `MeshManager` tracks
  nearby vehicles from BSMs and exchanges `IPI-CooperativeService` guidance
  frames over V2V links, while `TaskOffloader` packages computation-aid
  requests/responses on top of the same payload.

### Building

```bash
cmake -S cpp -B cpp/build
cmake --build cpp/build
```

This produces `libipi.a` and the example executables described below. Optional
integrations can be toggled via CMake flags.

## Web Experiment Tracker

The repo also ships a static browser UI for the `Edge4AV` experiment campaign at
`web/experiment-tracker/`. It mirrors the structure in `experiment.md`, stores
edits locally in the browser, and can export either JSON or Markdown snapshots.

Run it from a lightweight local server:

```bash
python3 -m http.server
```

Then open:

```text
http://localhost:8000/web/experiment-tracker/
```

If you would rather use a shorter entry point from the repo root, open:

```text
http://localhost:8000/experiment-tracker.html
```

If you started the server from inside `web/` instead of the repo root, use:

```text
http://localhost:8000/experiment-tracker/
```

### Example Programs

- `example_build_service_request`
  Demonstrates constructing and serialising an `IPI-ServiceRequest` and an
  `IPI-CooperativeService` payload. Use it as boilerplate when crafting service
  requests or cooperative guidance data.

- `example_v2x_roundtrip`
  Exercises the J2735 helpers and the `UperCodec` by encoding/decoding synthetic
  BSM, MAP, SPaT, SRM, and SSM frames. Replace the synthetic values with live
  sensor input to simulate RSU broadcasts.

- `example_private_5g_latency_receiver` and `example_private_5g_latency_sender`
  Form a concrete measurement harness for the private connected-V2X route. The
  sender can use either a raw TCP session or MQTT over the 5G IP path, sends
  framed IPI payloads, and reports RTT plus one-way uplink/downlink latency
  when the endpoints are time-synchronized. The receiver validates and
  acknowledges SPaT or
  `IPI-CooperativeService` probes. Both ends now support shared experiment
  metadata (`--run-id`, `--condition-id`, `--condition-label`, `--request-id`,
  `--network-load-level`, `--qos-profile`, `--mobility-state`,
  `--clock-sync-state`) so logs can be joined directly into Edge4AV figures.

- `example_mocar_ipi` *(requires `IPI_ENABLE_MOCAR_EXAMPLES=ON`)*
  Wraps the Mocar SDK so RSU/OBU devices can broadcast the core message types.
  Ideal for hardware bring-up when no full intersection backend exists yet.

- `example_ros2_bsm_broadcaster` *(requires both `IPI_ENABLE_ROS2_BRIDGE=ON`
  and `IPI_ENABLE_MOCAR_EXAMPLES=ON` inside a ROS 2 workspace)* Subscribes to
  `v2x_msg::msg::BSM` (e.g., generated by perception/camera pipelines), converts
  it to the lightweight model, validates it through the UPER codec, and
  broadcasts the result via the Mocar SDK.
- `example_mesh_demo`
  Uses `ipi::mesh::MeshManager` with a simple logging transport to demonstrate the vehicle–vehicle local mesh mode and `TaskOffloader`-based computation handoffs: it ingests synthetic BSMs, simulates loss of infrastructure heartbeats, and shows both cooperative guidance broadcasts and a mocked task offload lifecycle.

### Optional Integrations

- `-DIPI_ENABLE_ROS2_BRIDGE=ON` – builds the ROS 2 bridge helpers inside a ROS
  workspace where `v2x_msg` is available.
- `-DIPI_ENABLE_MOCAR_EXAMPLES=ON` – enables demos that call the Mocar SDK (RSU
  hardware); requires `third_party/mocar/libs/libmocarv2x.so`.
- Enabling both flags adds the ROS→Mocar BSM broadcaster sample described above.

## Demo: Edge→Device SPaT over TCP

This demo uses the host-built TCP sender and the J2735-2020 bridge on the
Mocar device.

### Build the pieces

1) Edge sender (host/Orin):
```bash
cmake -S cpp -B cpp/build
cmake --build cpp/build --target example_spat_tcp_sender
```

2) Device bridge (J2735-2020 SDK, cross-compile on host):
```bash
cd third_party/mocar/J2735-2020/samples/ipi_spat_bridge
make clean && make   # produces ipi_spat_bridge (aarch64)
```

### Run the demo

On the Mocar broadcasting device (with `libmocarcv2x.so` and `libzlog.so`
available at `../lib` relative to the binary):
```bash
./ipi_spat_bridge            # listens on TCP port 35555 by default
```

On the edge device (replace IP with the broadcaster’s address):
```bash
./cpp/build/example_spat_tcp_sender 192.168.253.40 35555
```

Override phases/states/timing if needed:
```bash
./cpp/build/example_spat_tcp_sender 192.168.253.40 35555 1 3 5000 2 1 5000
# state map: 0=dark,1=stopAndRemain,2=stopThenProceed,3=proceed,4=flashing
```

Enable debug hex dumps in the sender with `IPI_DEBUG=1` when troubleshooting.

## Demo: Mocar Radio RTT Probe

The `ipi_custom_rtt` sample uses the Mocar custom V2X API to send a timestamped
probe from one radio and echo it from the other radio. RTT is computed on the
initiator, so this path does not depend on synchronized clocks.

Build:
```bash
cd third_party/mocar/J2735-2020/samples/ipi_custom_rtt
make clean && make
```

Run on the echo-side radio:
```bash
./ipi_custom_rtt --role responder --node-id v2x-rx-radio
```

Run on the measuring radio:
```bash
./ipi_custom_rtt \
  --role initiator \
  --node-id v2x-tx-radio \
  --count 1000 \
  --interval-ms 100 \
  --payload-bytes 128 \
  --csv > mocar_radio_rtt.csv
```

For a payload sweep that doubles size each condition up to 128 KiB, keep the
responder running and run this on the measuring radio:
```bash
./run_payload_sweep.sh \
  --node-id v2x-tx-radio \
  --count 1000 \
  --interval-ms 100
```

The sweep writes CSV files plus tab-delimited text files for spreadsheet import:
`mocar_radio_rtt_all_samples.txt`, `mocar_radio_rtt_summary.txt`,
`mocar-rtt-payload-<bytes>_samples.txt`, and
`mocar-rtt-payload-<bytes>_rtt_ms.txt`.

## Demo: Private 5G Route Latency over TCP or MQTT

This pair is intended for your private 5G IP path. Run the receiver on the RSU,
edge node, or infrastructure host reachable over 5G, then run the sender from
the vehicle-side machine or modem host. The same binaries support both direct
TCP and MQTT transport.

### Build the pieces

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build --target example_private_5g_latency_receiver example_private_5g_latency_sender
```

### Run the receiver over TCP

```bash
./cpp/build/example_private_5g_latency_receiver \
  --transport tcp \
  --port 36666 \
  --run-id edge4av-run-001 \
  --condition-id p5g-tcp-baseline \
  --condition-label private-5g-baseline \
  --rsu-id rsu-1 \
  --network-load-level idle \
  --qos-profile default \
  --clock-sync-state ptp-synced \
  --csv > tcp_receiver.csv
```

### Run service-plane probes over TCP

```bash
./cpp/build/example_private_5g_latency_sender \
  --transport tcp \
  --host 192.168.253.40 \
  --port 36666 \
  --count 50 \
  --interval-ms 1000 \
  --message service \
  --payload-bytes 256 \
  --run-id edge4av-run-001 \
  --condition-id p5g-tcp-baseline \
  --condition-label private-5g-baseline \
  --request-id p5g-tcp-baseline \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --network-load-level idle \
  --qos-profile default \
  --clock-sync-state ptp-synced \
  --csv > tcp_service_baseline.csv
```

### Run SPaT probes over the same TCP path

```bash
./cpp/build/example_private_5g_latency_sender \
  --transport tcp \
  --host 192.168.253.40 \
  --port 36666 \
  --count 50 \
  --interval-ms 1000 \
  --message spat
```

### Run the receiver against an MQTT broker

Use any MQTT 3.1.1-compatible broker reachable over the 5G path.

```bash
./cpp/build/example_private_5g_latency_receiver \
  --transport mqtt \
  --host 192.168.253.40 \
  --port 1883 \
  --intersection-id intersection-101 \
  --source-id veh-01 \
  --run-id edge4av-run-001 \
  --condition-id p5g-mqtt-baseline \
  --condition-label private-5g-baseline \
  --rsu-id rsu-1 \
  --network-load-level idle \
  --qos-profile default \
  --clock-sync-state ptp-synced \
  --csv > mqtt_receiver.csv
```

### Run service-plane probes over MQTT

```bash
./cpp/build/example_private_5g_latency_sender \
  --transport mqtt \
  --host 192.168.253.40 \
  --port 1883 \
  --count 50 \
  --interval-ms 1000 \
  --message service \
  --payload-bytes 256 \
  --intersection-id intersection-101 \
  --source-id veh-01
```

### Run SPaT probes over MQTT

```bash
./cpp/build/example_private_5g_latency_sender \
  --transport mqtt \
  --host 192.168.253.40 \
  --port 1883 \
  --count 50 \
  --interval-ms 1000 \
  --message spat \
  --intersection-id intersection-101 \
  --source-id veh-01
```

Notes:

- `--message service` sends an `IPI-CooperativeService` request frame and is the
  closest match to the private-session control plane.
- `--message spat` sends a J2735 SPaT frame over the same selected transport path so you can
  compare radio SPaT timing against the private 5G route.
- `--transport mqtt` uses request and ack topics under
  `ipi/{intersectionId}/latency/{sourceId}/...`.
- The sender prints per-probe timing plus summary `p50/p95/p99` RTT. One-way
  uplink/downlink numbers assume the endpoints are clock-synchronized.
- Use `--csv` if you want per-probe rows for later plotting.
- The MQTT path is a dependency-free MQTT 3.1.1 implementation over plain TCP.
  It does not add TLS by itself.

## Working with the Design

Start with `references/README.md` to understand the overall architecture,
communication planes, and payload contracts. The C++ APIs mirror the same
nomenclature so you can stub or back the interfaces with your infrastructure of
choice. The mesh behaviour, power‑outage scenario, and cooperative task
offloading are documented in `cpp/LOGIC.md` and implemented in
`ipi::mesh::MeshManager` and `ipi::mesh::TaskOffloader`. For hardware-in-the-loop
experiments, reuse the optional Mocar helpers; for ROS-based perception systems,
publish detected vehicles as BSM messages and forward them using the provided
broadcaster and mesh/offload samples.

Contributions are welcome—open issues or PRs when expanding the spec, porting
encoders, or wiring production backends to the `ReceiverApi`/`SenderApi`
interfaces.
