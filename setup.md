# IPI Setup and Deployment Guide

This guide describes how to build and deploy the current IPI reference
implementation for:

- radio-path testing with the Mocar V2X SDK
- private 5G latency testing over TCP and MQTT
- basic Edge4AV/IPI application bring-up

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
  - receiver stdout/stderr logs
  - radio receive logs from `V2X-RX-RADIO`

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

## 8. Deploy by role

### 8.1 TX-EDGE

Deploy at minimum:

- `example_spat_tcp_sender`

Responsibilities:

- build or relay the SPaT payload toward `V2X-TX-RADIO`
- timestamp the sender-side start of the radio-path experiment

### 8.2 V2X-TX-RADIO

Deploy:

- `ipi_spat_bridge`
- Mocar shared libraries

Responsibilities:

- receive SPaT payloads from `TX-EDGE` over TCP
- forward them into the Mocar V2X transmit path

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

### 8.5 Vehicle / Sender Nodes

Deploy:

- `example_private_5g_latency_sender` on the 5G sender

Responsibilities:

- drive the 5G latency experiments
- write sender-side logs
- vary payload size, message type, and interval

## 9. Bring-up sequence

Recommended order:

1. Build and pass `ctest`.
2. Build `ipi_spat_bridge`.
3. Stage binaries on all runtime nodes.
4. Verify time synchronization.
5. Start receive logging on `V2X-RX-RADIO`.
6. Start `ipi_spat_bridge` on `V2X-TX-RADIO`.
7. Run `example_spat_tcp_sender` on `TX-EDGE`.
8. Start the 5G TCP receiver on `5G-2`.
9. Run the 5G TCP latency sender on `5G-1`.
10. Bring up the MQTT broker.
11. Run the 5G MQTT latency test.
12. Archive logs and extracted CSV output after each run.

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

## 11. Sanity checks after deployment

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

## 12. Current deployment limits

Be explicit about these when you deploy:

- Radio path currently uses the vendored `J2735-2020` Mocar bridge sample for
  SPaT forwarding.
- The radio-path sender host and the transmitting radio are separate roles.
- The repo does not yet ship a dedicated receive-only Mocar latency collector
  for `V2X-RX-RADIO`.
- Private 5G latency tooling supports both TCP and MQTT.
- MQTT support is minimal MQTT 3.1.1 over plain TCP.
- The repo provides a measurement harness, not a full production backend.
- One-way latency is only meaningful if your clocks are synchronized.
- The optional CMake Mocar example path under `cpp/examples/device` targets a
  different SDK layout and is not the deployment path documented here.
