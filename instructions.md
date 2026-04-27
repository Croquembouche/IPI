# IPI Latency Test Instructions

This guide shows how to run the current latency tooling with:

- 2 V2X radios for the over-the-air path
- 1 Linux edge host attached to the transmitting radio
- 2 private 5G devices for the IP path

The examples below use these role names:

- `TX-EDGE`: Linux host that runs the SPaT sender and is wired to the
  transmitting radio
- `V2X-TX-RADIO`: the transmitting Mocar V2X radio device
- `V2X-RX-RADIO`: the receiving V2X radio device
- `5G-1`: vehicle-side sender host
- `5G-2`: infrastructure-side receiver host

If one machine can serve multiple roles in your lab, that is fine. The commands
do not require 5 physical boxes, but the role split below is the cleanest setup.

## 1. Build the binaries on the build host

From the repository root:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

Build the transmitter-side Mocar bridge separately:

```bash
make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge
```

If you are compiling directly on the transmitting radio instead of cross-building
from a host, override the compiler:

```bash
make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge CXX=g++
```

Important binaries:

- `cpp/build/example_spat_tcp_sender`
- `cpp/build/example_private_5g_latency_sender`
- `cpp/build/example_private_5g_latency_receiver`
- `third_party/mocar/J2735-2020/samples/ipi_spat_bridge/ipi_spat_bridge`

## 2. Sync clocks before measuring

If you want one-way latency, synchronize the endpoints first.

Recommended:

- GNSS-disciplined PTP if available
- otherwise NTP/Chrony and use RTT as the main metric

Without synchronized clocks, the scripts still give valid RTT, but uplink and
downlink one-way numbers are not trustworthy.

## 2.1 Shared experiment metadata

The current sender, receiver, and bridge tools support a common logging schema.
Use the same identifiers for all processes participating in one condition:

- `--run-id`: one campaign or paper run
- `--condition-id`: one concrete baseline, scaling, or stress condition
- `--condition-label`: recommended values are `radio-baseline`,
  `private-5g-baseline`, or `private-5g-stressed`
- `--request-id`: request id or request-id base used in logs
- `--network-load-level`: for example `idle`, `moderate`, `heavy`,
  `near-saturation`
- `--qos-profile`: for example `default`, `fifo`, `5qi-mapped`
- `--mobility-state`: for example `stationary`, `moving`
- `--clock-sync-state`: for example `ptp-synced`, `ntp-synced`, `unsynced`
- `--csv`: emit structured CSV rows

Recommended condition labels for the paper:

- `radio-baseline`
- `private-5g-baseline`
- `private-5g-stressed`

## 3. Radio Path: 2 V2X devices

This path measures SPaT latency using the existing TCP-to-Mocar bridge.

### Device roles

- `TX-EDGE`: runs `example_spat_tcp_sender`
- `V2X-TX-RADIO`: runs `ipi_spat_bridge`, receives the TCP payload from its edge
  host, and injects the SPaT into the Mocar SDK for radio broadcast
- `V2X-RX-RADIO`: receives the over-the-air SPaT and logs it through the Mocar
  receive path or your existing vendor latency tooling

The actual flow is:

```text
TX-EDGE -> example_spat_tcp_sender -> V2X-TX-RADIO / ipi_spat_bridge
        -> mde_v2x_spat_send(...) -> over-the-air broadcast -> V2X-RX-RADIO
```

### 3.1 Deploy

Copy or install:

- `cpp/build/example_spat_tcp_sender` to `TX-EDGE`
- `third_party/mocar/J2735-2020/samples/ipi_spat_bridge/ipi_spat_bridge` to
  `V2X-TX-RADIO`
- Mocar shared libraries to `V2X-TX-RADIO` if they are not already staged:
  - `third_party/mocar/J2735-2020/lib/libmocarcv2x.so`
  - `third_party/mocar/J2735-2020/lib/libzlog.so`
- a receive-side Mocar sample, callback, or your existing radio log collector on
  `V2X-RX-RADIO`

### 3.2 Start the bridge on `V2X-TX-RADIO`

On the transmitting Mocar device:

```bash
cd /path/to/ipi_spat_bridge
./ipi_spat_bridge \
  --run-id edge4av-run-001 \
  --condition-id radio-spat-baseline \
  --condition-label radio-baseline \
  --request-id radio-spat-baseline \
  --rsu-id rsu-1 \
  --clock-sync-state ptp-synced \
  --csv > radio_bridge.csv
```

Default listen port is `35555`.

This binary is the transmitter-side bridge. It does not receive the radio packet
back from the air interface.

### 3.3 Start radio receive logging on `V2X-RX-RADIO`

Run the receive-side tool you use to confirm SPaT arrival on the second radio.
In the vendored Mocar SDK, the relevant receive hook is
`mde_v2x_spat_recv_handle_register(...)`, shown in
`third_party/mocar/J2735-2020/samples/spat/spat_sample.c`.

Do not treat `spat_sample.c` as a receive-only harness without reviewing it
first. Its current `main()` also transmits SPaT in a loop. Use it as an API
reference for the callback path, or use your existing receive-side vendor
logging.

### 3.4 Start the sender on `TX-EDGE`

```bash
./example_spat_tcp_sender \
  --run-id edge4av-run-001 \
  --condition-id radio-spat-baseline \
  --condition-label radio-baseline \
  --request-id radio-spat-baseline \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --clock-sync-state ptp-synced \
  --csv \
  <V2X-TX-RADIO_IP> 35555 > radio_sender.csv
```

Example:

```bash
./example_spat_tcp_sender \
  --run-id edge4av-run-001 \
  --condition-id radio-spat-baseline \
  --condition-label radio-baseline \
  --request-id radio-spat-baseline \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --clock-sync-state ptp-synced \
  --csv \
  192.168.253.40 35555 > radio_sender.csv
```

Optional custom phases:

```bash
./example_spat_tcp_sender 192.168.253.40 35555 1 3 5000 2 1 5000
```

### 3.5 What to record

- sender stdout on `TX-EDGE`
- bridge stdout on `V2X-TX-RADIO`
- receive callback output or vendor logs on `V2X-RX-RADIO`
- any Mocar radio logs you already use for latency extraction

## 4. Private 5G Path over TCP: 2 5G devices

This path uses the IPI latency probe pair directly over the private 5G IP path.

### Device roles

- `5G-1`: runs `example_private_5g_latency_sender`
- `5G-2`: runs `example_private_5g_latency_receiver`

### 4.1 Start the receiver on `5G-2`

```bash
./example_private_5g_latency_receiver \
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

### 4.2 Run service-plane latency from `5G-1`

```bash
./example_private_5g_latency_sender \
  --transport tcp \
  --host <5G-2_IP> \
  --port 36666 \
  --count 100 \
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

### 4.3 Run SPaT latency over the same 5G TCP path

```bash
./example_private_5g_latency_sender \
  --transport tcp \
  --host <5G-2_IP> \
  --port 36666 \
  --count 100 \
  --interval-ms 1000 \
  --message spat \
  --run-id edge4av-run-001 \
  --condition-id p5g-tcp-spat-baseline \
  --condition-label private-5g-baseline \
  --request-id p5g-tcp-spat-baseline \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --network-load-level idle \
  --qos-profile default \
  --clock-sync-state ptp-synced \
  --csv > tcp_spat_baseline.csv
```

### 4.4 Capture a stressed TCP condition

```bash
./example_private_5g_latency_sender \
  --transport tcp \
  --host <5G-2_IP> \
  --port 36666 \
  --count 100 \
  --interval-ms 1000 \
  --message service \
  --payload-bytes 256 \
  --run-id edge4av-run-001 \
  --condition-id p5g-tcp-heavy-qos \
  --condition-label private-5g-stressed \
  --request-id p5g-tcp-heavy-qos \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --network-load-level heavy \
  --qos-profile 5qi-mapped \
  --clock-sync-state ptp-synced \
  --csv > tcp_service_heavy_qos.csv
```

## 5. Private 5G Path over MQTT: the same 2 5G devices

This path uses the same sender and receiver, but with `--transport mqtt`.

Assumption:

- an MQTT 3.1.1 broker is reachable from both `5G-1` and `5G-2`
- the broker can run on `5G-2` or on another infrastructure host in the same
  private 5G segment

The current repo does not ship a production broker. Use your lab broker or a
broker such as Mosquitto.

### 5.1 Start the MQTT receiver on `5G-2`

```bash
./example_private_5g_latency_receiver \
  --transport mqtt \
  --host <BROKER_IP> \
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

### 5.2 Run service-plane latency from `5G-1`

```bash
./example_private_5g_latency_sender \
  --transport mqtt \
  --host <BROKER_IP> \
  --port 1883 \
  --count 100 \
  --interval-ms 1000 \
  --message service \
  --payload-bytes 256 \
  --intersection-id intersection-101 \
  --source-id veh-01 \
  --run-id edge4av-run-001 \
  --condition-id p5g-mqtt-baseline \
  --condition-label private-5g-baseline \
  --request-id p5g-mqtt-baseline \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --network-load-level idle \
  --qos-profile default \
  --clock-sync-state ptp-synced \
  --csv > mqtt_service_baseline.csv
```

### 5.3 Run SPaT latency over MQTT

```bash
./example_private_5g_latency_sender \
  --transport mqtt \
  --host <BROKER_IP> \
  --port 1883 \
  --count 100 \
  --interval-ms 1000 \
  --message spat \
  --intersection-id intersection-101 \
  --source-id veh-01 \
  --run-id edge4av-run-001 \
  --condition-id p5g-mqtt-spat-baseline \
  --condition-label private-5g-baseline \
  --request-id p5g-mqtt-spat-baseline \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --network-load-level idle \
  --qos-profile default \
  --clock-sync-state ptp-synced \
  --csv > mqtt_spat_baseline.csv
```

### 5.4 Capture a stressed MQTT condition

```bash
./example_private_5g_latency_sender \
  --transport mqtt \
  --host <BROKER_IP> \
  --port 1883 \
  --count 100 \
  --interval-ms 1000 \
  --message service \
  --payload-bytes 256 \
  --intersection-id intersection-101 \
  --source-id veh-01 \
  --run-id edge4av-run-001 \
  --condition-id p5g-mqtt-near-sat \
  --condition-label private-5g-stressed \
  --request-id p5g-mqtt-near-sat \
  --av-id av-1 \
  --obu-id obu-1 \
  --rsu-id rsu-1 \
  --network-load-level near-saturation \
  --qos-profile fifo \
  --clock-sync-state ptp-synced \
  --csv > mqtt_service_near_sat.csv
```

## 6. Result interpretation

Each sender run prints:

- per-probe CSV or structured text rows with shared condition metadata
- RTT
- uplink latency
- receiver processing time
- downlink latency
- summary `p50`, `p95`, `p99`, and success rate

Use these comparisons:

- Radio SPaT vs 5G TCP SPaT
- 5G TCP service vs 5G MQTT service
- 5G TCP SPaT vs 5G MQTT SPaT

## 7. Recommended test matrix

Minimum matrix:

1. Radio SPaT: `TX-EDGE -> V2X-TX-RADIO -> V2X-RX-RADIO`
2. 5G TCP service: `5G-1 -> 5G-2`
3. 5G TCP SPaT: `5G-1 -> 5G-2`
4. 5G MQTT service: `5G-1 -> broker -> 5G-2`
5. 5G MQTT SPaT: `5G-1 -> broker -> 5G-2`

For each case, collect at least:

- `100` probes
- sender CSV
- receiver logs
- network notes: RSSI/RSRP/SINR, load, broker location, time sync status

## 8. Troubleshooting

- If TCP sender times out, verify IP reachability and that the receiver port is open.
- If MQTT sender times out, verify the broker is running and both sender and receiver use the same:
  - `--intersection-id`
  - `--source-id`
  - broker IP and port
- If uplink or downlink numbers look negative or unstable, treat only RTT as valid and fix clock sync.
- If the Mocar bridge does not transmit, verify `libmocarcv2x.so` and `libzlog.so` are available relative to the binary.
- If `ipi_spat_bridge` fails to build on the host, verify `aarch64-linux-gnu-g++`
  is installed, or rebuild natively on the radio with `make CXX=g++`.
