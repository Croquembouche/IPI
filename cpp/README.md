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
