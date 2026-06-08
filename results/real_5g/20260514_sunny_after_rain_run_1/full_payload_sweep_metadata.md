# Full TCP/MQTT Payload Sweep Metadata

- Start time: 2026-05-14T09:38:25-04:00
- Payload arguments: 0 256 1024 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152
- Count per condition: 1000
- Interval ms: 200
- TCP port: 36666
- MQTT port: 1883
- Base station: 10.100.100.6
- Weather: sunny after rain
- Vehicle location: moved to new location before run
- Mobility: stationary
- Clock sync state: unsynced
- MQTT broker: threaded Python MQTT 3.1.1 QoS0 broker on base station
- Timestamp note: sender send time is patched after request encoding and immediately before TCP send or MQTT publish, so sender-side packaging time is excluded from RTT.
