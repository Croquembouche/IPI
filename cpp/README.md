# IPI C++ Reference Library

This directory contains the C++17 IPI reference implementation. It now covers
the common message contract, an enforced private-session lifecycle, correlated
measurement helpers, a bounded PC5 binding, and the transport-neutral Year 2
activation, service-policy, and offload decision core.

## Module map

- `include/ipi/core`, `src/core`: strict IPI service request and cooperative
  service models.
- `include/ipi/api`, `src/api`: common API types, session lifecycle, per-topic
  wire codec, in-memory and broker-backed session transports, private-5G
  measurement helpers, detector-result schema, and PC5 adapter.
- `include/ipi/activation`, `src/activation`: activation-zone geometry,
  hysteresis, dwell, validity, and transition state machine.
- `include/ipi/policy`, `src/policy`: tier-aware service decisions with explicit
  safety-critical local fallback behavior.
- `include/ipi/offload`, `src/offload`: local/edge completion prediction,
  deadline checks, confidence checks, and fallback selection.
- `include/ipi/mesh`, `src/mesh`: mesh management and identity-, state-, and
  deadline-aware task lifecycle.
- `include/ipi/v2x`, `src/v2x`: the formal IPI `TestMessage00` regional UPER
  codec, legacy lightweight J2735 field helpers, and the optional ROS 2
  conversion bridge.
- `tests`: deterministic contract, error-path, boundary, loopback, and decision
  tests.

## Configure, build, and test

From the repository root:

```bash
cmake -S cpp -B cpp/build \
  -DIPI_ENABLE_TESTS=ON \
  -DIPI_BUILD_EXAMPLES=ON
cmake --build cpp/build --parallel
ctest --test-dir cpp/build --output-on-failure
```

Useful options:

- `-DIPI_BUILD_EXAMPLES=ON` builds the host-side tools.
- `-DIPI_ENABLE_TESTS=ON` builds all contract and loopback tests.
- `-DIPI_ENABLE_ROS2_BRIDGE=ON` builds the legacy `v2x_msg` conversions when a
  ROS 2 environment is sourced.
- `-DIPI_ENABLE_MOCAR_EXAMPLES=ON` builds SDK-dependent device examples when a
  compatible Mocar SDK is available. Override its location with
  `-DMOCAR_SDK_ROOT=/path/to/new_V2X_64bit`.

## Install and consume with CMake

The project exports `IPI::ipi` and a package configuration:

```bash
cmake -S cpp -B cpp/build \
  -DIPI_ENABLE_TESTS=ON \
  -DCMAKE_INSTALL_PREFIX=/opt/ipi
cmake --build cpp/build --parallel
cmake --install cpp/build
```

A consumer can then use:

```cmake
find_package(IPI CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE IPI::ipi)
```

Set `CMAKE_PREFIX_PATH=/opt/ipi` if the prefix is not already searchable.

## Session transports

`SessionLifecycle` is the transport-neutral authority for registration,
heartbeat activation, monotonic lease expiry, patch, termination, service
authorization, telemetry authorization, outstanding requests, and terminal
results. The in-memory and broker-backed transports use the same rules.

The broker path consists of a client-side `PrivateSessionTransport`, a
broker-hosted `SessionBrokerEndpoint`, and a publish/subscribe
`SessionMessageBroker`. `make_mqtt_session_message_broker` supplies the current
MQTT 3.1.1 implementation, including username/password CONNECT fields,
bounded connect/I/O waits, keepalive, reconnect, and resubscription. It currently
uses QoS 0 and plain TCP; TLS, durable QoS, production access policy, and
deployed broker restart tests remain deployment work.

## Proposal-facing ROS 2 runtime

The separate `ros2_ws/src/ipi_msgs` and `ros2_ws/src/ipi_runtime` packages
publish activation transitions, tiered service decisions, and offload
decisions. They deliberately do not command Autoware motion or operation mode.
See `ros2_ws/src/ipi_runtime/README.md` and the top-level `setup.md` for build,
launch, and launch-test commands.

## Formal J2735 IPI regional profile

`asn1/IPI.asn` defines `IpiCooperativeService` as a typed SAE J2735 regional
extension. The outer frame uses reserved `TestMessage00` (`DSRCmsgID` 240) and
local, uncoordinated `RegionId` 200. `J2735IpiRegionalCodec` encodes and decodes
the complete UPER `MessageFrame`; `UperCodec` delegates its cooperative-service
overload to that implementation. The older BSM, PSM, MAP, SPaT, SRM, and SSM
helper overloads remain project field profiles and are not complete generated
SAE codecs.

The checked-in vectors cover planning, perception, control, a minimal request,
all optional fields, fixed-point boundaries, malformed input, unexpected
message and region identifiers, extensions, padding, and size limits. They were
regenerated independently with `asn1tools` and `pycrate`; the outer frame was
also decoded by the USDOT J2735 202409 reference package:

```bash
python3 scripts/verify_j2735_ipi_vectors.py
```

SAE distributes its normative ASN.1 modules separately from the standard PDF.
When a licensed `J2735ASN_202309` module directory is available, create a new
combined bundle without modifying the licensed source directory:

```bash
python3 scripts/prepare_j2735_ipi_schema.py \
  --base-dir /path/to/J2735ASN_202309 \
  --output-dir /tmp/j2735asn-202309-ipi
```

The script verifies the reserved message identifier, inserts the IPI binding
into `Reg-TestMessage00`, copies `IPI.asn`, and writes a SHA-256 manifest. Exact
compilation with that licensed SEP2023 bundle remains a release gate until the
module files are supplied to the build environment.

## PC5 device sample

`third_party/mocar/J2735-2020/samples/ipi_pc5_exchange` binds the versioned
`IP5X` envelope to the vendor custom-message channel. Its `J2735Payload` is now
the formal `TestMessage00`/IPI UPER `MessageFrame`, and both endpoints decode it
before accepting a result. `IP5X` remains a project correlation, deadline, and
CRC envelope; it is not a new J2735 message or a standardized PC5 bearer.

The default complete application-packet cap is 2048 bytes. `--body-bytes`
controls only the IPI offload-content bytes, so the full J2735 and `IP5X`
framing must also fit under that cap. A deployment can explicitly raise the
complete-packet cap to the observed SDK-specific 4080-byte ceiling only after
its own hardware sweep.
