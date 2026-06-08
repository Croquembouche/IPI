# MQTT Payload Sweep Metadata

- Start time: 2026-05-15T15:35:36-04:00
- Payload arguments: 0 256 1024 4096 8192 16384 32768 65536 131072 262144
- Count per condition: 1000
- Interval ms: 200
- MQTT port: 1883
- Base station: 10.100.100.6
- Weather: sunny
- Mobility: stationary
- Clock sync state: unsynced
- MQTT broker: fresh threaded Python MQTT 3.1.1 QoS0 broker on base station for each MQTT payload.
- Timestamp note: sender send time is set after request encoding and immediately before MQTT publish, so sender-side packaging time is excluded from RTT.
