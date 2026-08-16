# 2026-08-16 Quick TCP Uplink Latency Acquisition — Run 2

- Run ID: `edge4av-real-20260816-airspan-tdd-raw-uplink-40-40-20-location-1-tcp-quick-run-2`
- User-reported current TDD profile: `40/40/20`
- User-reported current RSRP: `-98 dBm`
- RSRQ carried forward from the latest operator report: `-13 dB`; not freshly
  measured for this run
- Airspan Cell 1 administrative state: user-reported `unlocked` and broadcasting
- Airspan Cell 2 administrative state: user-reported `unlocked` and broadcasting
- Corrected serving context: the user clarified after acquisition that the
  MG52 was locked onto Cell 1, with no reported handoff. The earlier carried-
  forward Cell 2 entry is superseded; no timestamp-aligned MG52 export is stored
- Terminology: administrative `locked` means not broadcasting and is distinct
  from UE serving-cell selection
- Corrected radio record: every derived application-summary row records TDD
  `40/40/20`, RSRP `-98 dBm`, RSRQ `-13 dB`, both cells' administrative/
  broadcast states, and serving Cell 1. Acquisition-time condition manifests
  retain the original Cell 2 entry and are explicitly superseded by
  `serving_cell_selection_correction.json` for serving-cell interpretation
- Transport: TCP only
- Payloads: 1 KiB, 10 KiB, and 100 KiB
- Attempts per condition: 1,000
- Post-completion interval: 200 ms
- Application acknowledgment timeout: 60 s
- Timing metric: sender monotonic complete application-acknowledgment
  receipt-and-parse RTT

The user requested another quick TCP latency run, so this run reused the
validated stationary GPS evidence collected earlier on 2026-08-16 rather than
recording a new bag. That ROS 2 bag spans 59.570 seconds, contains 16,087
messages across all six expected NovAtel topics, and has 13,063 valid position
rows. Its median horizontal speed is 0.0025 m/s and p95 radial spread is
0.622 m. The referenced capture passed its stationarity gate, but continuity
between that capture and this run was not independently reverified. Exact
coordinates and the bag remain under the excluded MQTT-repeat raw evidence;
this run stores only their provenance and stationarity summary.

The three TCP conditions ran from 14:34:56 through 14:56:45 EDT. Each has
1,000 accepted sender rows, 1,000 matching edge receiver rows, zero failures,
an exit status of zero, and a completion marker. Payload sizes describe exact
application objects, not individual TCP or IP packets; TCP may segment an
object across multiple packets.

All 21 fetched edge files matched their remote copies under full SHA-256 list
comparison, performed separately for each condition before access revocation.
The per-run key with fingerprint
`SHA256:DcAgriY87GFMtudQTCnYPt6XQS1GkSArYmOrjakY5tY` was removed from the edge,
subsequent authentication with it failed, and its local private/public key
files were deleted.

Acquisition software:

- Repository commit: `2b44f352bb4579599dc51c4719314b66052e4f49`
- Raw probe SHA-256: `36c2cf9ed694175fa05ae7ed74eff3788ac5e8bd9fc9a6dd0a1efe862d0e84bf`
- Acquisition-time runner SHA-256: `69f17a568b7d235ef8928c8d86e8df5453f3f59cace515307ab17660237dcd68`

No timestamp-aligned Airspan/ACP or MG52 export is stored for this block. This
single-location, single-profile run measures its own TCP RTT distribution; it
does not establish a causal TDD, RSRP, or both-cells-broadcasting effect.
