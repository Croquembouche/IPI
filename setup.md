# IPI Setup and Deployment Guide

This guide describes how to build and deploy the current IPI reference
implementation for:

- radio-path testing with the Mocar V2X SDK
- private 5G latency testing over TCP and MQTT
- basic Edge4AV/IPI application bring-up
- synchronized experiment logging for the Edge4AV paper campaign

## 1. Deployment model

A practical lab deployment usually has these roles:

- `Build Host`: where you compile IPI
- `TX-EDGE`: Linux host that sends SPaT to the transmitting radio over TCP
- `V2X-TX-RADIO`: transmitter-side Mocar radio device
- `V2X-RX-RADIO`: receiver-side V2X radio device
- `5G Vehicle Node`: sender-side device on the private 5G network
- `5G Infrastructure Node`: receiver-side device on the private 5G network
- `MQTT Broker`: only needed for the MQTT path; may run on the 5G infrastructure
  node or another reachable host

You can collapse some roles onto one machine, but keeping them separate makes
latency measurements cleaner.

## 2. Prerequisites

Required on the build host:

- Linux
- `cmake`
- C++17 compiler
- `make`

Required for Mocar V2X deployment:

- Mocar SDK files under `third_party/mocar/`
- ability to run the J2735 bridge binary on the transmitter-side radio
- `aarch64-linux-gnu-g++` if you cross-build `ipi_spat_bridge` from an x86 host,
  or a native `g++` toolchain if you build directly on the radio

Required for MQTT deployment:

- an MQTT 3.1.1-compatible broker reachable by both 5G endpoints

Required for the proposal-facing ROS 2 reference runtime:

- ROS 2 Humble with `ament_cmake`, `rclcpp`, `sensor_msgs`, and
  `launch_testing_ament_cmake`
- the system ROS Python (`/usr/bin/python3` on the validated Humble host)

Recommended for meaningful latency measurements:

- synchronized clocks across endpoints
- GNSS-disciplined PTP if possible
- otherwise NTP/Chrony and RTT-centric analysis

## 3. Build IPI

From the repository root:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

This builds the core IPI library, the Edge4AV examples, and the private 5G
latency tools. It does not build the vendored `J2735-2020` Mocar bridge.

Important outputs:

- `cpp/build/libipi.a`
- `cpp/build/example_spat_tcp_sender`
- `cpp/build/example_private_5g_latency_sender`
- `cpp/build/example_private_5g_latency_receiver`
- `cpp/build/example_edge4av_dual_plane`

### 3.1 Install the CMake package

The library exports `IPI::ipi` for downstream packages:

```bash
cmake -S cpp -B cpp/build \
  -DIPI_ENABLE_TESTS=ON \
  -DCMAKE_INSTALL_PREFIX="$PWD/cpp/install"
cmake --build cpp/build --parallel
cmake --install cpp/build
```

Downstream CMake projects can use:

```cmake
find_package(IPI CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE IPI::ipi)
```

Add `cpp/install` to `CMAKE_PREFIX_PATH` if it is not already searchable.

## 4. Build the Mocar bridge used in the radio path

From the repository root:

```bash
make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge clean
make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge
```

This produces:

- `third_party/mocar/J2735-2020/samples/ipi_spat_bridge/ipi_spat_bridge`

The vendored bridge Makefile defaults to `aarch64-linux-gnu-g++`. If you are
building directly on the radio instead of cross-compiling from the build host,
use:

```bash
make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge CXX=g++
```

Also keep these runtime libraries available on the Mocar target:

- `third_party/mocar/J2735-2020/lib/libmocarcv2x.so`
- `third_party/mocar/J2735-2020/lib/libzlog.so`

### 4.1 Build and run the IPI PC5 exchange

The strict `IP5X` sample is separate from the existing SPaT and custom-RTT
samples. Its application payload is a complete UPER J2735 `MessageFrame` using
reserved `TestMessage00` (`DSRCmsgID` 240), local `RegionId` 200, and the typed
IPI regional value from `cpp/asn1/IPI.asn`:

```bash
make -C third_party/mocar/J2735-2020/samples/ipi_pc5_exchange clean
make -C third_party/mocar/J2735-2020/samples/ipi_pc5_exchange
```

Copy the resulting `ipi_pc5_exchange` binary and vendor shared libraries to
both radios. Start the responder before the initiator:

```bash
./ipi_pc5_exchange --role responder --node-id node-b --peer-id node-a
./ipi_pc5_exchange --role initiator --node-id node-a --peer-id node-b \
  --count 100 --body-bytes 128 --timeout-ms 1000
```

`--body-bytes` is the number of bytes in the IPI offload-content field, not the
final radio application-packet size. The default 2048-byte cap applies to the
complete `IP5X` packet, including metadata, the J2735 frame, length fields, and
CRC. An input that cannot fit is rejected before transmission.
`--max-packet-bytes` can be raised only up to the observed 4080-byte SDK
application ceiling. That larger value is deployment-specific and must be
validated again on the actual device pair.

### 4.2 Verify or prepare the J2735 regional schema

The deterministic vector verifier requires `asn1tools` 0.167.0, `pycrate`
0.7.11, and an installed USDOT J2735 pycrate package. It compiles `IPI.asn` with
both independent ASN.1 implementations, compares every regional UPER value,
then verifies the outer `MessageFrame` with the unmodified J2735 package:

```bash
python3 scripts/verify_j2735_ipi_vectors.py
```

The repository includes the SAE standard PDF but not the separately licensed
`J2735ASN_202309` module bundle. After obtaining that bundle, prepare a combined
schema in a new directory:

```bash
python3 scripts/prepare_j2735_ipi_schema.py \
  --base-dir /path/to/J2735ASN_202309 \
  --output-dir /tmp/j2735asn-202309-ipi
```

The preparation step verifies the `TestMessage00` identifier, adds the typed
IPI entry to `Reg-TestMessage00`, and records input and output hashes. Compile
the resulting directory with the licensed production ASN.1/UPER compiler; do
not edit or redistribute the source SAE modules.

## 5. Suggested runtime layout

On each runtime host, stage a small deployment directory such as:

```text
/opt/ipi/
  bin/
  lib/
  logs/
  config/
```

Suggested contents:

- `bin/`
  - `example_spat_tcp_sender`
  - `example_private_5g_latency_sender`
  - `example_private_5g_latency_receiver`
  - `ipi_spat_bridge`
- `lib/`
  - Mocar shared libraries when needed
- `logs/`
  - sender CSV files
  - receiver CSV files
  - receiver stdout/stderr logs
  - radio receive logs from `V2X-RX-RADIO`
  - run manifests keyed by `run_id` and `condition_id`

## 6. Copy artifacts to targets

Example deployment commands:

```bash
scp cpp/build/example_spat_tcp_sender user@TX-EDGE:/opt/ipi/bin/
scp cpp/build/example_private_5g_latency_sender user@5G-1:/opt/ipi/bin/
scp cpp/build/example_private_5g_latency_receiver user@5G-2:/opt/ipi/bin/

scp third_party/mocar/J2735-2020/samples/ipi_spat_bridge/ipi_spat_bridge user@V2X-TX-RADIO:/opt/ipi/bin/
scp third_party/mocar/J2735-2020/lib/libmocarcv2x.so user@V2X-TX-RADIO:/opt/ipi/lib/
scp third_party/mocar/J2735-2020/lib/libzlog.so user@V2X-TX-RADIO:/opt/ipi/lib/
```

No repo-provided receive-only binary is required on `V2X-RX-RADIO` unless you
add your own callback/logger there. That side currently depends on the vendor
receive path and whatever latency logging you already use in the lab.

## 7. Configure runtime environment

Useful environment variables:

- `MOCAR_TCP_PORT`
  - overrides the listen port for `ipi_spat_bridge`
- `IPI_DEBUG=1`
  - enables debug hex output in some sender-side helpers

If the Mocar target does not already know where to find its shared libraries,
set:

```bash
export LD_LIBRARY_PATH=/opt/ipi/lib:$LD_LIBRARY_PATH
```

For the paper experiments, standardize these runtime flags across sender,
receiver, and bridge processes:

- `--run-id`
- `--condition-id`
- `--condition-label`
- `--request-id`
- `--network-load-level`
- `--qos-profile`
- `--mobility-state`
- `--clock-sync-state`
- `--csv`

Recommended `condition-label` values:

- `radio-baseline`
- `private-5g-baseline`
- `private-5g-stressed`

## 8. Deploy by role

### 8.1 TX-EDGE

Deploy at minimum:

- `example_spat_tcp_sender`

Responsibilities:

- build or relay the SPaT payload toward `V2X-TX-RADIO`
- timestamp the sender-side start of the radio-path experiment
- emit run- and condition-tagged experiment logs

### 8.2 V2X-TX-RADIO

Deploy:

- `ipi_spat_bridge`
- Mocar shared libraries

Responsibilities:

- receive SPaT payloads from `TX-EDGE` over TCP
- forward them into the Mocar V2X transmit path
- emit RSU-side bridge logs with the same `run_id` and `condition_id`

### 8.3 V2X-RX-RADIO

Deploy:

- your receive-side Mocar callback/logger or existing vendor telemetry tooling

Responsibilities:

- receive the over-the-air SPaT broadcast
- record receive timestamps, radio logs, or callback output

The vendored `third_party/mocar/J2735-2020/samples/spat/spat_sample.c` shows
the receive callback API, but its sample `main()` also transmits SPaT. Treat it
as a callback reference, not a drop-in receive-only latency harness.

### 8.4 5G Infrastructure Node

Deploy at minimum:

- `example_private_5g_latency_receiver`
- optionally the MQTT broker
- optionally `example_edge4av_dual_plane` for API smoke testing

Responsibilities:

- listen for TCP latency probes
- subscribe to MQTT latency request topics
- host or reach the MQTT broker
- emit infrastructure-side experiment logs with condition metadata

### 8.5 Vehicle / Sender Nodes

Deploy:

- `example_private_5g_latency_sender` on the 5G sender

Responsibilities:

- drive the 5G latency experiments
- write sender-side logs and CSV files
- vary payload size, message type, and interval

## 9. Bring-up sequence

Recommended order:

1. Build and pass `ctest`.
2. Build `ipi_spat_bridge` and, if measuring radio RTT, `ipi_custom_rtt`.
3. Stage binaries on all runtime nodes.
4. Verify time synchronization.
5. Start receive logging on `V2X-RX-RADIO`.
6. Start `ipi_spat_bridge` on `V2X-TX-RADIO`.
7. Run `example_spat_tcp_sender` on `TX-EDGE`.
8. Start the 5G TCP receiver on `5G-2`.
9. Run the 5G TCP latency sender on `5G-1`.
10. Bring up the MQTT broker.
11. Run the 5G MQTT latency test.
12. Archive CSV logs and stderr summaries after each run.

## 10. Optional MQTT broker deployment

The repo does not ship a production broker.

Typical options:

- run Mosquitto on the infrastructure-side 5G node
- run a broker on a separate edge VM reachable from both 5G devices

If you use Mosquitto, the default plain TCP port is usually `1883`.

Current code boundary:

- the in-repo MQTT path is a minimal MQTT 3.1.1 client implementation
- it does not add TLS on its own

If you need broker-authenticated TLS for deployment, add that as the next step.

### 10.1 Broker-backed IPI sessions

The session transport is distinct from the latency-probe MQTT topics. A
deployment instantiates:

- `make_mqtt_session_message_broker` in each process that connects to the
  broker;
- `make_broker_session_endpoint` in the infrastructure process; and
- `make_broker_private_session_transport` in each vehicle-side client process.

Both ends exchange strict `IPIS` records for registration, heartbeat, patch,
termination, service requests and updates, events, telemetry, and PCV
responses. The shared lifecycle enforces active state, monotonic leases,
ownership, service grants, sequence/freshness, outstanding requests, and
terminal outcomes.

`MqttSessionBrokerConfig` accepts host, port, client ID, username, password,
keepalive, clean-session, allowed topic prefix, and reconnect backoff. The
configuration also bounds connect and socket I/O waits so an unavailable broker
cannot block shutdown indefinitely. The current implementation reconnects and
resubscribes after detected connection loss. It is a reference MQTT 3.1.1
client using QoS 0 and plain TCP. Do not use it as production evidence until
TLS, deployment access policy, required QoS, and real broker disconnect/restart
tests have passed.

## 11. Build and test the ROS 2 proposal runtime

The `ipi_msgs` package defines the activation, service-intent/link,
policy-decision, offload-request, and offload-decision contracts. `ipi_runtime`
provides the activation manager, tiered service manager, offload decision node,
and a parameterized reference-zone publisher.

After installing the C++ library as shown in Section 3.1:

```bash
source /opt/ros/humble/setup.bash
PATH=/opt/ros/humble/bin:/usr/bin:/bin \
colcon --log-base ros2_ws/log build \
  --base-paths ros2_ws/src/ipi_msgs ros2_ws/src/ipi_runtime \
  --build-base ros2_ws/build \
  --install-base ros2_ws/install \
  --cmake-args \
    -DCMAKE_PREFIX_PATH="$PWD/cpp/install" \
    -DPython3_EXECUTABLE=/usr/bin/python3
```

Run the launch contract test:

```bash
source ros2_ws/install/setup.bash
colcon --log-base ros2_ws/log test \
  --build-base ros2_ws/build \
  --install-base ros2_ws/install \
  --packages-select ipi_runtime \
  --event-handlers console_direct+
colcon test-result --test-result-base ros2_ws/build --verbose
```

Run the reference intersection:

```bash
source ros2_ws/install/setup.bash
ros2 launch ipi_runtime reference_intersection.launch.py
```

The launch path publishes decisions only. It does not directly call Autoware
operation-mode services or send motion/control commands. A future approved
vehicle adapter must retain local safety authority and reject stale or unsafe
requests.

## 12. Sanity checks after deployment

Use these quick checks.

On the build host:

```bash
./cpp/build/example_edge4av_dual_plane
```

On `5G-2`:

```bash
/opt/ipi/bin/example_private_5g_latency_receiver --transport tcp --port 36666 --once
```

On `5G-1`:

```bash
/opt/ipi/bin/example_private_5g_latency_sender --transport tcp --host <5G-2_IP> --port 36666 --count 1 --message service
```

For MQTT, verify:

- broker reachable on `host:port`
- sender and receiver use the same `--intersection-id`
- sender and receiver use the same `--source-id`
- all nodes in the same condition use the same `--run-id` and `--condition-id`
- the receiver or bridge side sets the intended `--rsu-id`

## 13. Current deployment limits

Be explicit about these when you deploy:

- Radio path currently uses the vendored `J2735-2020` Mocar bridge sample for
  SPaT forwarding.
- The radio-path sender host and the transmitting radio are separate roles.
- The repo ships `ipi_custom_rtt` for Mocar radio RTT. SPaT receive-side
  one-way logging still depends on the Mocar callback/logger used in the lab.
- Private 5G latency tooling supports both TCP and MQTT.
- Private-5G RTT uses a local steady clock. Wall-clock timestamps are retained
  for log correlation, and one-way latency is emitted only when clock
  synchronization is explicitly declared.
- The correlated private-5G executable and native detector schema have local
  tests, but historical result files were not relabeled; new monotonic or
  native-detector claims require recollection.
- The broker session reference path has an in-memory broker integration test;
  it has not yet passed a deployed-broker restart or TLS test.
- The `IP5X` PC5 adapter and Mocar sample compile for AArch64, but a two-radio
  field run remains required.
- The ROS 2 reference runtime has a synthetic launch test. It publishes
  activation/policy/offload decisions and intentionally does not command
  Autoware motion.
- The current tools standardize experiment logs, but AV-facing outcome metrics
  still need to be supplied by the operator or higher-level AV stack.
- MQTT support is minimal MQTT 3.1.1 over plain TCP.
- The repo provides a measurement harness, not a full production backend.
- One-way latency is only meaningful if your clocks are synchronized.
- The typed IPI regional schema, complete `TestMessage00` codec, dual-compiler
  payload vectors, and USDOT 202409 outer-frame check are implemented. Exact
  compilation and interoperability against the separately licensed SAE
  `J2735ASN_202309` modules remain pending because those files are not in this
  repository. `IP5X` itself is project framing over the vendor custom channel,
  not a standardized J2735 bearer.
- The repository does not yet carry an approved project license for external
  release; ROS package license identifiers remain a release gate.
- The optional CMake Mocar example path under `cpp/examples/device` targets a
  different SDK layout and is not the deployment path documented here.
