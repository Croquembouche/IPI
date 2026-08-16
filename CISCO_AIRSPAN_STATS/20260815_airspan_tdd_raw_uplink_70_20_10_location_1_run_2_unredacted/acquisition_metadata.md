# Private 5G Raw-Byte Uplink Payload Sweep

- Acquisition date: `2026-08-15`
- Run ID: `edge4av-real-20260815-airspan-tdd-raw-uplink-70-20-10-location-1-run-2`
- Location ID: `location_1`
- Mobility: `stationary`
- Transports: `tcp`, `mqtt`
- Application payloads: `1024 10240 102400 1048576 2097152` bytes
- Attempts per transport and size: `1000`
- Planned attempts: `10000`
- Interval: `200 ms` after each completed attempt
- Timeout: `60000 ms` per application acknowledgment
- TDD: operator-reported `70/20/10`
- Airspan Cell 1 administrative state: `unlocked` and broadcasting
- Airspan Cell 2 administrative state: `unlocked` and broadcasting
- Serving cell: operator-reported Airspan Cell 2 with no handoff
- Terminology: an administratively `locked` cell is not broadcasting; this is
  separate from the UE serving-cell state
- RSRP: operator-reported `-100 dBm`
- RSRQ: operator-reported `-13 dB`
- Clock state: `unsynced`; sender RTT only
- Timing metric: monotonic RTT from immediately before the first framed TCP or
  MQTT byte is sent through receipt and structural parsing of the complete
  application acknowledgment. The local topic/sequence/length/CRC32
  comparisons follow the timer and must not be described as included in
  `rtt_ms`; see `measurement_boundary_correction.json`.
- Payload validation: exact length plus CRC32 at the edge
- Response: compact application acknowledgment containing sequence, accepted
  state, payload length, CRC32, and server validation time

Metadata correction: after acquisition, the user corrected the originally
recorded RSRP from `-97 dBm` to `-100 dBm`. This correction changes only the
operator-reported radio context; it does not alter any traffic, RTT, telemetry,
or GPS sample. The acquisition-time runner hash below remains preserved. The
separate checkpoint hash captures the immediate metadata-only correction;
later reusable-runner changes are not acquisition-time source provenance.

The TCP application body is framed over a persistent TCP connection; TCP may
segment it into multiple network packets. MQTT carries the same raw request in
a QoS 0 publish through the experiment broker. The payload labels therefore
describe application bytes, not individual IP packets.

The validated 60-second stationary GPS evidence remains in the excluded run-1
location directory. Run 2 references that single capture rather than recording
the unchanged stationary location again.

## Protocol Qualification

- Raw probe SHA-256 on car and edge:
  `36c2cf9ed694175fa05ae7ed74eff3788ac5e8bd9fc9a6dd0a1efe862d0e84bf`.
- Acquisition runner SHA-256:
  `e049d51227d62cc2c79cb19cdce0542208887330e2715b9940edf6cf39898cfa`.
- Post-acquisition RSRP-correction runner checkpoint SHA-256:
  `d0ce8392c1e53e9fd6714d0ee734eb59844e752858ccf8daf5cff17152fe2940`.
- Host telemetry collector SHA-256:
  `0693afa4dc2d217beb814c619ef3a23fb27a0f667546563c480189502982a86d`.
- Aggregate analyzer SHA-256:
  `8bce602d7de4882c44f6ce88c3df7f3fbdf59cdf7848af6a161a988e27b6fa32`.
- Raw codec self-test: passed through 2 MiB.
- Local TCP 2 MiB loopback: 3/3 sender acknowledgments and 3/3 receiver rows.
- Local MQTT 2 MiB loopback: 3/3 sender acknowledgments and 3/3 receiver rows.
- Private-5G TCP 2 MiB smoke: 3/3 sender acknowledgments and 3/3 edge rows.
- Private-5G MQTT 2 MiB smoke: 3/3 sender acknowledgments and 3/3 edge rows.

## Excluded Pilot

Run 1 completed 1,000 IPI-probe attempts at 1 KiB and 1,000 at 10 KiB before
the current committed codec rejected the 100 KiB argument at construction time
with `offloadPayload exceeds 65535 bytes`. The zero-message 100 KiB attempt and
the two completed IPI conditions remain preserved as a protocol-qualification
pilot. They are excluded from this raw-byte sweep so all final conditions use
one payload format and timing boundary.

## Full Acquisition Result

- Workload window: `2026-08-15T12:52:02-04:00` through
  `2026-08-15T14:48:24-04:00`.
- Complete conditions: `10/10`.
- Sender attempts: `10000`; accepted: `10000`; failed: `0`.
- Matching edge receiver rows: `10000`.
- Exact application payload length and CRC32: validated for every edge row.
- Sender/edge sequence correlation: validated for every accepted sender row.
- Host telemetry: `20/20` car/edge files parsed and validated.
- Remote fetch verification: all `95/95` remote files match their local copies
  by SHA-256.
- One-way latency: excluded because endpoint clocks were unsynchronized.

The independent aggregate validator passed with no validation errors. Its
machine-readable output is in `validation_summary.json`, and the per-condition
RTT, deadline, goodput, server-processing, and host-health results are under
`analysis/`.

During TCP 2 MiB sequences 97-116, a 22.7-second read-only Git integrity scan
briefly raised car CPU to 11.2%. All 20 affected attempts succeeded, their
median RTT was consistent with the full condition, and the final condition
maximum occurred later at sequence 613 outside this window. Every row remains
included; see `known_anomalies.json`.

After all edge files were fetched and checksum-verified, the exact temporary
public-key line was removed from the edge, failed key authentication confirmed
the revocation, and the local private/public key files and empty temporary
directory were deleted. The key files are not recoverable; the local and edge
result artifacts remain preserved.
