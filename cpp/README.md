# IPI C++ Layout

- `include/`, `src/`: core IPI library (J2735 helpers, UPER codec, API façade).
- `examples/`
  - `library/`: host-side sanity tools that do not require hardware
    (`example_build_service_request`, `example_v2x_roundtrip`).
  - `device/`: hardware-facing binaries (`example_mocar_ipi`, `example_ros2_bsm_broadcaster`).

## Build

```bash
cmake -S cpp -B cpp/build
cmake --build cpp/build
```

Toggles:

- `-DIPI_BUILD_EXAMPLES=ON` (default) builds the apps above.
- `-DIPI_ENABLE_ROS2_BRIDGE=ON` turns on ROS2 conversions and the ROS2 broadcaster.
- `-DIPI_ENABLE_MOCAR_EXAMPLES=ON` enables Mocar-backed examples. By default it
  expects the SDK under `third_party/mocar/new_V2X_64bit`; override with
  `-DMOCAR_SDK_ROOT=/path/to/new_V2X_64bit`.

The static library is produced at `cpp/build/libipi.a`, and public headers live
under `cpp/include`. In your own CMake project, you can either add this tree as
a subdirectory or install the library and consume it via `find_package`.

## Running the examples

After building as above, the example binaries live in `cpp/build`:

```bash
# From the repo root, after cmake --build cpp/build
./cpp/build/example_build_service_request
./cpp/build/example_v2x_roundtrip
./cpp/build/example_spat_tcp_sender <rsu_ip> <port>
./cpp/build/example_mesh_demo
```

- `example_build_service_request` prints an `IPI-ServiceRequest` and matching
  `IPI-CooperativeService` encoded into their canonical byte formats.
- `example_v2x_roundtrip` shows J2735 BSM/MAP/SPaT/SRM/SSM encode/decode using
  the lightweight models and `UperCodec`.
- `example_spat_tcp_sender` opens a TCP connection to an RSU device and sends
  SPaT frames; see the top-level `README.md` “Edge→Device SPaT over TCP” section
  for a full end-to-end walkthrough.
- `example_mesh_demo` exercises `ipi::mesh::MeshManager` by feeding it a local
  vehicle pose and a synthetic neighbor, simulating loss of SPaT/MAP/5G
  heartbeats so the mesh mode activates, and showcasing the
  `ipi::mesh::TaskOffloader` helper sending/receiving computation-aid payloads.
