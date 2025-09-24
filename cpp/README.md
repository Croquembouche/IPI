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
- Canonical byte-level encoders/decoders (non-ASN.1) suitable for early
  integration testing.
- Validation helpers that enforce the bounds and cardinality constraints
  documented in the API design.
- Example executable demonstrating round-trip encoding/decoding.

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
target environment provides a compatible `libmocarv2x.so`:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_MOCAR_EXAMPLES=ON
```

The Mocar-based demo expects the SDK under `third_party/mocar`. Adjust
`MOCAR_LOG_CFG` (or edit the `DEFAULT_LOG_CONFIG` macro) before running on the
device:

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
