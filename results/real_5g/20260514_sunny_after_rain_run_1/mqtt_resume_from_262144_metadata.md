# MQTT Resume From 262144 Metadata

- Start time: 2026-05-14T16:37:13-04:00
- Payload arguments: 262144 524288 1048576 2097152
- Count per condition: 1000
- Interval ms: 200
- MQTT port: 1883
- Broker: fresh threaded Python MQTT 3.1.1 QoS0 broker for each resumed payload.
- Timestamp note: sender send time is patched after request encoding and immediately before MQTT publish, so sender-side packaging time is excluded from RTT.
