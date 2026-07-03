# Experiment 12 Failure/Fallback - Run 1

- Date: 2026-07-03
- Run ID: `edge4av-real-20260703-failure-fallback-run-1`
- Edge server: `10.100.100.6`
- Vehicle state: stationary
- Payload: 1024-byte service payload
- Probe count: 1000 per condition
- Probe interval: 200 ms
- Sender timeout: 1000 ms
- Failure trigger: 60 s after sender start
- Configured outage: 10 s before restart command
- Transports: TCP, UDP, MQTT
- GNSS: NovAtel ROS 2 bag and CSV recorded under `gps/`

## GNSS

- Mean GNSS position: 39.663553168, -75.757035809
- Position range: latitude 39.663547650 to 39.663556660, longitude -75.757039592 to -75.757026924
- Horizontal speed median: 0.001985 m/s
- Horizontal speed p95: 0.004778 m/s
- Horizontal speed max: 0.017804 m/s
- CSV position samples: 247,105
- ROS bag: 138 MiB, 1,123.35 s, 303,308 messages

## Results

| Failure mode | Transport | Attempts | Accepted | Failed | Success % | p50 ms | p95 ms | p99 ms | Restart outage ms | First failure after trigger ms | First success after restart ms | Success gap ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Receiver restart | TCP | 1,000 | 953 | 47 | 95.3 | 126.629 | 161.464 | 361.228 | 11,371.1 | 864.1 | 57.3 | 11,714.7 |
| Receiver restart | UDP | 1,000 | 969 | 31 | 96.9 | 30.305 | 44.139 | 63.027 | 13,285.0 | 747.2 | 24.7 | 13,463.5 |
| Receiver restart | MQTT | 1,000 | 991 | 9 | 99.1 | 31.977 | 49.855 | 259.906 | 13,287.7 | 1,820.6 | 6.6 | 13,367.4 |
| Broker restart | MQTT | 1,000 | 955 | 45 | 95.5 | 31.644 | 47.871 | 90.553 | 15,021.1 | 631.9 | 50.6 | 15,100.7 |

## Failure Details

| Failure mode | Transport | Failure details |
| --- | --- | --- |
| Receiver restart | TCP | 1 peer close, 46 reconnect failures |
| Receiver restart | UDP | 31 UDP acknowledgment timeouts |
| Receiver restart | MQTT | 9 MQTT acknowledgment timeouts |
| Broker restart | MQTT | 1 peer close, 43 reconnect failures, 1 acknowledgment timeout |

## Notes

- This is the stationary no-Mocar Experiment 12 subset: receiver restart for TCP, UDP, and MQTT, plus MQTT broker restart.
- TCP and MQTT senders used `--continue-on-failure`, so they recorded failures and continued reconnecting instead of exiting at the first transport error.
- Receiver-side artifacts were fetched into `base_station/`; receiver CSVs have two headers because the restarted receiver appends to the same condition CSV.
- Weak-signal and compact-fallback failure modes remain separate follow-up runs if the paper needs those claims.
