# Experiment 11 Multiclient Scalability - Run 4

- Date: 2026-07-02
- Run ID: `edge4av-real-20260702-multiclient-scalability-run-4`
- Edge server: `10.100.100.6`
- Vehicle state: stationary
- Payload: 1024-byte service payload
- Probe count: 1000 per client per condition
- Client levels: 1, 2, 5, 10, 20, 50, 100
- Transports: TCP, fragmented UDP, MQTT
- MQTT mode: one broker on the edge and one receiver process per source ID
- GNSS: NovAtel ROS 2 bag and CSV recorded under `gps/`

## GNSS

- Mean GNSS position: 39.663559686, -75.756988674
- Best-position p95 radius: 1.524 m
- Best-position max radius: 1.731 m
- Horizontal speed p95: 0.004273 m/s
- Horizontal speed max: 0.019289 m/s
- ROS bag: 744.7 MiB, 6,078.51 s, 1,641,200 messages

## Results

| Transport | Clients | Attempts | Accepted | Failed | Success % | p50 ms | p95 ms | p99 ms | Per-client p95 range ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| TCP | 1 | 1,000 | 1,000 | 0 | 100.00 | 121.132 | 143.829 | 152.695 | 143.829-143.829 |
| TCP | 2 | 2,000 | 2,000 | 0 | 100.00 | 122.019 | 145.652 | 157.832 | 145.439-145.713 |
| TCP | 5 | 5,000 | 5,000 | 0 | 100.00 | 121.877 | 145.869 | 197.953 | 145.458-145.934 |
| TCP | 10 | 10,000 | 10,000 | 0 | 100.00 | 121.758 | 143.852 | 153.908 | 142.001-145.680 |
| TCP | 20 | 20,000 | 20,000 | 0 | 100.00 | 122.399 | 145.751 | 167.820 | 143.704-147.789 |
| TCP | 50 | 50,000 | 50,000 | 0 | 100.00 | 133.845 | 166.067 | 189.781 | 161.841-169.984 |
| TCP | 100 | 100,000 | 100,000 | 0 | 100.00 | 187.687 | 264.070 | 305.725 | 255.879-272.003 |
| UDP | 1 | 1,000 | 1,000 | 0 | 100.00 | 30.383 | 40.542 | 48.232 | 40.542-40.542 |
| UDP | 2 | 2,000 | 2,000 | 0 | 100.00 | 30.352 | 41.462 | 47.181 | 40.515-42.255 |
| UDP | 5 | 5,000 | 5,000 | 0 | 100.00 | 30.373 | 41.752 | 48.068 | 41.418-41.793 |
| UDP | 10 | 10,000 | 10,000 | 0 | 100.00 | 30.213 | 41.869 | 49.775 | 40.915-41.946 |
| UDP | 20 | 20,000 | 19,999 | 1 | 99.995 | 30.707 | 43.796 | 51.733 | 41.903-45.742 |
| UDP | 50 | 50,000 | 49,986 | 14 | 99.972 | 31.966 | 53.967 | 79.634 | 51.858-57.865 |
| UDP | 100 | 100,000 | 99,965 | 35 | 99.965 | 46.097 | 113.391 | 139.843 | 110.109-116.032 |
| MQTT | 1 | 1,000 | 1,000 | 0 | 100.00 | 30.368 | 40.551 | 47.028 | 40.551-40.551 |
| MQTT | 2 | 2,000 | 2,000 | 0 | 100.00 | 30.563 | 42.278 | 47.370 | 41.754-43.172 |
| MQTT | 5 | 5,000 | 5,000 | 0 | 100.00 | 32.297 | 45.454 | 51.230 | 44.936-45.777 |
| MQTT | 10 | 10,000 | 10,000 | 0 | 100.00 | 37.954 | 47.873 | 57.642 | 46.087-49.735 |
| MQTT | 20 | 20,000 | 20,000 | 0 | 100.00 | 37.920 | 53.753 | 107.932 | 51.847-55.881 |
| MQTT | 50 | 50,000 | 50,000 | 0 | 100.00 | 37.829 | 54.056 | 82.022 | 51.879-57.799 |
| MQTT | 100 | 100,000 | 100,000 | 0 | 100.00 | 41.774 | 81.850 | 121.672 | 77.857-85.738 |

## Notes

- This is the second stationary current-location repeat for Experiment 11.
- The full TCP/UDP/MQTT sweep completed for all requested client levels.
- UDP failures are reply timeouts/drops recorded by the sender; the sender processes completed normally.
- The patched GNSS helper exited cleanly after the experiment wrapper finished.
