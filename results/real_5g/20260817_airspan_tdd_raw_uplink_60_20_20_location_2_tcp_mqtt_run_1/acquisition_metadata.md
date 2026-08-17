# 2026-08-17 Location-2 TCP/MQTT Uplink Payload Acquisition

- Run ID: `edge4av-real-20260817-airspan-tdd-raw-uplink-pending-location-2-tcp-mqtt-run-1`
- User-reported TDD profile: `60/20/20`
- User-reported RSRP: `-100 dBm`
- Carried-forward RSRQ: `-13 dB`
- Location: new stationary `location_2`
- Fresh GNSS: 59.599 seconds, 16,098 ROS messages; stationarity gate passed
- Transports: TCP and MQTT exact raw application objects with compact correlated application acknowledgments
- Payloads: 1 KiB, 10 KiB, 100 KiB, and 1,024 KiB
- Post-completion interval: 200 ms

TCP 1, 10, and 100 KiB each completed 1,000/1,000 validated exchanges. The
user stopped TCP 1,024 KiB after 227/227 matching sender/receiver rows; it
retains exit status 143 and no completion marker. MQTT 1, 10, and 100 KiB each
completed 1,000/1,000. MQTT 1,024 KiB completed its user-declared 100/100.

TDD and RSRP were initially carried forward provisionally so collection could
start before the user's radio report. During the run, the user reported TDD
`60/20/20` and RSRP `-100 dBm`. The timestamped radio-context update preserves
that chronology without rewriting earlier acquisition manifests.

All 68 edge files matched their remote copies before the temporary authorization
was revoked. No experiment process or planned test port remained active.

The cell administrative state, serving-cell selection, handoff state, and RSRQ
are carried forward rather than freshly verified. No timestamp-aligned ACP or
MG52 export is stored. This run is a one-location application observation and
does not establish a causal TDD or signal-strength effect.
