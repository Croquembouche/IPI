# Experiment 11 Multiclient Scalability - Run 2

- Date: 2026-07-02
- Run ID: `edge4av-real-20260702-multiclient-scalability-run-2`
- Edge server: `10.100.100.6`
- Vehicle state: stationary
- Payload: 1024-byte service payload
- Probe count: 1000 per client per condition
- Client levels: 1, 2, 5, 10, 20
- Transports: TCP, fragmented UDP, MQTT
- MQTT mode: one broker on the edge and one receiver process per source ID
- GNSS: NovAtel ROS 2 bag and CSV recorded under `gps/`

## GNSS

- Mean GNSS position: 39.663549987, -75.756999844
- Best-position p95 radius: 1.262 m
- Best-position max radius: 1.324 m
- Horizontal speed p95: 0.004208 m/s
- Horizontal speed max: 0.018772 m/s
- ROS bag: 540.8 MiB, 4,414.31 s, 1,191,862 messages

## Results

| Transport | Clients | Attempts | Accepted | Success % | p50 ms | p95 ms | p99 ms | Per-client p95 range ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| TCP | 1 | 1,000 | 1,000 | 100.00 | 129.613 | 177.841 | 340.295 | 177.841-177.841 |
| TCP | 2 | 2,000 | 2,000 | 100.00 | 125.486 | 176.408 | 291.839 | 173.803-179.657 |
| TCP | 5 | 5,000 | 5,000 | 100.00 | 125.112 | 179.889 | 350.327 | 177.896-181.698 |
| TCP | 10 | 10,000 | 10,000 | 100.00 | 126.668 | 189.859 | 319.529 | 187.778-193.875 |
| TCP | 20 | 20,000 | 20,000 | 100.00 | 131.861 | 221.684 | 359.732 | 215.920-228.003 |
| UDP | 1 | 1,000 | 996 | 99.60 | 30.354 | 51.798 | 77.783 | 51.798-51.798 |
| UDP | 2 | 2,000 | 1,982 | 99.10 | 31.722 | 59.723 | 85.653 | 59.718-59.727 |
| UDP | 5 | 5,000 | 4,981 | 99.62 | 30.805 | 55.721 | 75.977 | 54.922-56.578 |
| UDP | 10 | 10,000 | 9,997 | 99.97 | 30.185 | 43.642 | 50.524 | 41.948-43.823 |
| UDP | 20 | 20,000 | 19,997 | 99.98 | 30.553 | 45.927 | 61.820 | 45.692-48.106 |
| MQTT | 1 | 1,000 | 1,000 | 100.00 | 30.446 | 43.242 | 46.308 | 43.242-43.242 |
| MQTT | 2 | 2,000 | 2,000 | 100.00 | 30.563 | 43.852 | 54.219 | 43.827-43.861 |
| MQTT | 5 | 5,000 | 5,000 | 100.00 | 30.747 | 43.741 | 48.383 | 43.023-43.858 |
| MQTT | 10 | 10,000 | 10,000 | 100.00 | 33.676 | 45.946 | 54.452 | 45.652-47.826 |
| MQTT | 20 | 20,000 | 20,000 | 100.00 | 39.668 | 89.831 | 165.962 | 82.232-92.480 |

## Notes

- TCP receiver was updated to handle concurrent clients before this run.
- Fragmented UDP was used with `--udp-max-datagram-bytes 1400`.
- MQTT at 20 clients completed with one receiver process per source ID.
- This run is the base stationary repeat. The 50- and 100-client extension is in `../20260702_multiclient_scalability_run_3/summary.md`.
