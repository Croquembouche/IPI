# Experiment 11 Multiclient Scalability - Run 3

- Date: 2026-07-02
- Run ID: `edge4av-real-20260702-multiclient-scalability-run-3`
- Edge server: `10.100.100.6`
- Vehicle state: stationary
- Payload: 1024-byte service payload
- Probe count: 1000 per client per condition
- Client levels: 50, 100
- Transports: TCP, fragmented UDP, MQTT
- MQTT mode: one broker on the edge and one receiver process per source ID
- GNSS: NovAtel ROS 2 bag and CSV recorded under `gps/`

## GNSS

- Mean GNSS position: 39.663557810, -75.757006269
- Best-position p95 radius: 0.556 m
- Best-position max radius: 0.652 m
- Horizontal speed p95: 0.004162 m/s
- Horizontal speed max: 0.014692 m/s
- ROS bag: 269.8 MiB, 2,202.69 s, 594,729 messages

## Results

| Transport | Clients | Attempts | Accepted | Success % | p50 ms | p95 ms | p99 ms | Per-client p95 range ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| TCP | 50 | 50,000 | 50,000 | 100.00 | 143.790 | 189.609 | 237.964 | 183.974-193.701 |
| TCP | 100 | 100,000 | 100,000 | 100.00 | 203.798 | 283.765 | 331.926 | 274.832-290.742 |
| UDP | 50 | 50,000 | 49,483 | 98.97 | 35.774 | 73.899 | 120.009 | 69.143-79.769 |
| UDP | 100 | 100,000 | 99,365 | 99.37 | 65.220 | 145.894 | 169.839 | 138.870-151.839 |
| MQTT | 50 | 50,000 | 50,000 | 100.00 | 39.816 | 81.793 | 133.568 | 77.963-85.905 |
| MQTT | 100 | 100,000 | 100,000 | 100.00 | 114.053 | 177.053 | 547.738 | 173.854-179.697 |

## Notes

- This run extends Experiment 11 beyond the original 1, 2, 5, 10, and 20 client sweep.
- The edge soft file-descriptor limit was raised inside the supplemental runner before launching edge-side processes.
- MQTT at 100 clients completed with one broker plus 100 source-specific receiver processes.
- The base 1-20 client sweep is in `../20260702_multiclient_scalability_run_2/summary.md`.
