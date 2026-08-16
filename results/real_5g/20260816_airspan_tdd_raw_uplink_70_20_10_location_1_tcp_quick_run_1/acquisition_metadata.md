# 2026-08-16 Quick TCP Uplink Latency Acquisition

- Run ID: `edge4av-real-20260816-airspan-tdd-raw-uplink-70-20-10-location-1-tcp-quick-run-1`
- Reported TDD profile: `70/20/10`
- Carried-forward RSRP: `-98 dBm`
- Carried-forward RSRQ: `-13 dB`
- Airspan Cell 1 administrative state: `locked`, therefore not broadcasting
- Airspan Cell 2 administrative state: `unlocked` and broadcasting
- Serving cell: user-reported MG52 selection locked onto Cell 2 with no reported handoff
- Terminology: Airspan administrative `locked` means not broadcasting and is distinct from MG52 serving-cell selection
- Per-condition radio record: every condition manifest and application-summary row records TDD `70/20/10`, RSRP `-98 dBm`, and RSRQ `-13 dB`
- Transport: TCP only
- Payloads: 1 KiB, 10 KiB, and 100 KiB
- Attempts per condition: 1,000
- Post-completion interval: 200 ms
- Application acknowledgment timeout: 60 s
- Timing metric: sender monotonic complete application-acknowledgment receipt-and-parse RTT

This quick latency follow-up reused the validated stationary GPS evidence
collected earlier on 2026-08-16 rather than recording a new bag. That ROS 2
bag spans 59.570 seconds, contains 16,087 messages across all six expected
NovAtel topics, and has 13,063 valid position rows. Its median horizontal speed
is 0.0025 m/s and p95 radial spread is 0.622 m, so the stationarity gate passes
for that capture. Exact coordinates and the bag remain under the excluded
MQTT-repeat raw evidence; this run stores only their provenance and stationarity
summary. Physical-location continuity with this application run was not
independently reverified.

The three TCP conditions ran from 15:55:49 through 16:08:56 EDT. Each has
1,000 accepted sender rows, 1,000 matching edge receiver rows, zero failures,
an exit status of zero, and a completion marker. Payload sizes describe exact
application objects, not individual TCP or IP packets; TCP may segment an
object across multiple packets.

All 21 fetched edge files matched their remote copies under per-condition
checksum dry-run comparison. No experiment probe process or planned test port
remained active after the run. The per-run key with fingerprint
`SHA256:/q7OiYQCnOoI38vKveojUjc/zcIqMrVDnyEwfD5ynps` was removed from the edge,
subsequent authentication with it failed, and its local private/public key
files were deleted.

Acquisition software:

- Repository commit: `2b44f352bb4579599dc51c4719314b66052e4f49`
- Raw probe SHA-256: `36c2cf9ed694175fa05ae7ed74eff3788ac5e8bd9fc9a6dd0a1efe862d0e84bf`
- Acquisition-time runner SHA-256: `69f17a568b7d235ef8928c8d86e8df5453f3f59cace515307ab17660237dcd68`

The TDD profile, Cell administrative states, serving Cell 2/no-handoff state,
RSRP, and RSRQ are not verified by timestamp-aligned ACP or MG52 exports in
this block. TDD and cell context were reported immediately before acquisition;
RSRP and RSRQ were explicitly carried forward from the most recent user report.
This single-location, single-profile run measures its own TCP latency
distribution; it does not establish a causal TDD or cell-state effect.
