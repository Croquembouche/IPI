# MQTT Latency MiB Resume Metadata

- Start time: 2026-05-13T16:12:53-04:00
- Payload arguments: 1048576 2097152
- Count per condition: 1000
- Interval ms: 200
- MQTT port: 1883
- Broker: threaded Python MQTT 3.1.1 QoS0 broker, used after single-thread broker stalled on first 1 MiB attempt.
- Timestamp note: sender send time is patched after request encoding and immediately before MQTT publish, so sender-side packaging time is excluded from RTT.

- End time: 2026-05-13T17:15:22-04:00
