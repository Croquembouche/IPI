# Large Payload Sweep Metadata

- Start time: 2026-05-13T11:56:02-04:00
- Payload arguments: 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432
- Count per condition: 1000
- Interval ms: 200
- TCP port: 36666
- MQTT port: 1883
- Base station: 10.100.100.6
- Mobility: stationary
- Clock sync state: unsynced
- MQTT broker: temporary minimal Python MQTT 3.1.1 QoS0 broker on base station
- Note: sender interval is applied after each request/ack cycle; wall time grows with RTT and serialization cost.
