# 2026-08-16 MQTT Uplink Repeat Acquisition

- Run ID: `edge4av-real-20260816-airspan-tdd-raw-uplink-40-40-20-location-1-mqtt-repeat-run-1`
- Reported TDD profile: `40/40/20`
- Reported RSRP: `-100 dBm`
- Latest operator-reported RSRQ: `-13 dB`
- Transport: MQTT only
- Completed payloads: 1 KiB, 10 KiB, 100 KiB, and 1,024 KiB
- Attempts per completed condition: 1,000
- Post-completion interval: 200 ms
- Application acknowledgment timeout: 60 s
- Timing metric: sender monotonic complete application-acknowledgment RTT

The fresh ROS 2 bag spans 59.570 seconds and contains 16,087 messages across
all six expected NovAtel topics. All 13,063 CSV rows have valid positions. The
stationarity gate passes: median horizontal speed is 0.0025 m/s and p95 radial
spread is 0.622 m. Exact coordinates and the serialized bag remain only in the
excluded raw tree; the repository-safe coordinate is `39.664, -75.757`.

The four retained conditions ran from 09:57:26 through 11:38:01 EDT. Each has
1,000 accepted sender rows, 1,000 matching edge receiver rows, zero failures,
an exit status of zero, and a completion marker. The user stopped the 2 MiB
condition at 11:59:21 EDT after 133 validated sender/receiver pairs. It has no
completion marker and is excluded from all aggregate and repeat comparisons.
Future payload sweeps end at 1,024 KiB (1 MiB).

All 50 fetched edge files across the four complete conditions and the stopped
condition matched their remote copies under checksum dry-run comparison. The
temporary 2026-08-16 key and the stale 2026-08-15 experiment key were removed
from the edge. Subsequent authentication with the 2026-08-16 key failed, and
the local private/public key files were deleted.

Acquisition software:

- Repository commit: `2b44f352bb4579599dc51c4719314b66052e4f49`
- Raw probe SHA-256: `36c2cf9ed694175fa05ae7ed74eff3788ac5e8bd9fc9a6dd0a1efe862d0e84bf`
- Acquisition-time runner SHA-256: `11d080fc7fed58499d5c27f5a56c75d205776df1c20b68f87272507f69defdd4`
- GPS recorder SHA-256: `30a3f188846b852d2e2d7fa294308f778a4dbf01d8e61e039b9e7c6d7229abad`

The profile, Cell 2 lock/no-handoff, RSRP, and RSRQ are operator-reported
unless otherwise qualified above. No timestamp-aligned ACP or MG52 export is
stored for this block. Therefore comparisons with 2026-08-15 describe
day-to-day repeat differences, not a causal TDD effect.
