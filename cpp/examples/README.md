# Example Apps Layout

- `library/` – developer utilities and sanity checks that exercise the core IPI
  library without hardware: `build_service_request`, `v2x_roundtrip`,
  `spat_tcp_sender`, `mesh_demo`.
- `device/` – binaries that interface with V2X hardware/ROS2: `mocar_ipi_demo`
  (Mocar SDK), `ros2_bsm_broadcaster` (ROS2→Mocar rebroadcast),
  `spat_tcp_bridge` (device-side SPaT TCP listener).

Notes:
- `mocar_ipi_demo` supports runtime SPaT overrides via a plain text file set in
  `MOCAR_SPAT_FILE` (lines: `<signal_group> <phase_state_int> <likely_time_ms>`);
  edits take effect on the next transmit tick without restart.

## Building the examples

From the repository root:

```bash
cmake -S cpp -B cpp/build
cmake --build cpp/build
```

Build toggles:

- `-DIPI_BUILD_EXAMPLES=ON` (default) builds all library-side examples.
- `-DIPI_ENABLE_MOCAR_EXAMPLES=ON` enables the Mocar SDK–backed device demos.
- `-DIPI_ENABLE_ROS2_BRIDGE=ON` enables ROS2-based examples inside a ROS2 workspace.

## Running the library examples

After building, run from the repo root:

```bash
./cpp/build/example_build_service_request
./cpp/build/example_v2x_roundtrip
./cpp/build/example_spat_tcp_sender <rsu_ip> <port>
./cpp/build/example_mesh_demo
```

- `example_build_service_request` prints an `IPI-ServiceRequest` plus
  `IPI-CooperativeService` payload, useful as a template for your own
  requests/guidance.
- `example_v2x_roundtrip` generates synthetic BSM/MAP/SPaT/SRM/SSM frames,
  encodes them using the J2735 helpers and `UperCodec`, and decodes them again.
- `example_spat_tcp_sender` connects to a remote RSU or bridge process over TCP
  and sends SPaT frames; see the root `README.md` “Edge→Device SPaT over TCP”
  section for full instructions and parameter examples.
- `example_mesh_demo` simulates the Vehicle–Vehicle Local Mesh Mode: it feeds
  `ipi::mesh::MeshManager` local telemetry plus a synthetic neighbor, waits for
  infrastructure heartbeats to lapse, and logs the cooperative frames it would
  broadcast over a V2V link.

## Running the device examples

The device-side examples depend on the Mocar SDK and, optionally, ROS2:

- `example_mocar_ipi` (built when `IPI_ENABLE_MOCAR_EXAMPLES=ON`) brings up the
  Mocar RSU/OBU stack and uses the IPI library to send core J2735 messages.
  Configure `MOCAR_SDK_ROOT` at CMake configure time if the SDK is not under
  `third_party/mocar/new_V2X_64bit`.
- `example_spat_tcp_bridge` (also under the Mocar examples) listens on a TCP
  port and forwards SPaT frames to the on-device J2735 encoder; see the root
  `README.md` demo for a typical edge↔device setup.
- `example_ros2_bsm_broadcaster` (requires both `IPI_ENABLE_MOCAR_EXAMPLES` and
  `IPI_ENABLE_ROS2_BRIDGE`) subscribes to a `v2x_msg::msg::BSM` topic, uses the
  bridge helpers to convert into the lightweight BSM model, and rebroadcasts
  the encoded frames via the Mocar SDK.
