# Alternating TCP/MQTT Metadata

- Start time: 2026-05-22T13:55:37-04:00
- Weather: cloudy
- Payload arguments: 0 256 1024 4096 8192 16384 32768 65536 131072 262144 524288 1048576
- Count per condition: 1000
- Interval ms: 200
- Order: TCP then MQTT for each payload
- Base station: 10.100.100.6
- TCP port: 36666
- MQTT port: 1883
- Mobility: stationary
- Clock sync state: unsynced
- Timestamp note: sender send time is set after request encoding and immediately before TCP send or MQTT publish, so sender-side packaging time is excluded from RTT.

- End time: 2026-05-22T17:04:05-04:00
