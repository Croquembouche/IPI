# Experiment 11 Weak-Signal Multiclient Scalability

- Date: 2026-07-06
- Run ID: `edge4av-real-20260706-multiclient-scalability-weak-signal-run-1`
- Edge server: `10.100.100.6`
- Vehicle state: stationary weak-signal private-5G location
- Payload: `1024` B service payload
- Probe count: `1000` per client per condition
- Client levels: `1`, `2`, `5`, `10`, `20`, `50`, `100`
- Transports: TCP, fragmented UDP, MQTT
- MQTT mode: one broker on the edge and one receiver process per source ID
- GNSS: NovAtel ROS 2 CSV and bag under `gps/`

## GNSS

- CSV samples: `2158805`
- BESTPOS samples: `283771`
- Mean BESTPOS position: `39.662946885, -75.757351763`
- Mean horizontal speed: `0.0021` m/s

## Results

| Transport | Clients | Attempts | Accepted | Failed | Success % | p50 ms | p95 ms | p99 ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| TCP | 1 | 1000 | 1000 | 0 | 100.000 | 129.801 | 215.653 | 312.652 |
| TCP | 2 | 2000 | 2000 | 0 | 100.000 | 129.829 | 222.610 | 299.352 |
| TCP | 5 | 5000 | 5000 | 0 | 100.000 | 129.579 | 219.582 | 295.778 |
| TCP | 10 | 10000 | 10000 | 0 | 100.000 | 129.818 | 259.796 | 329.905 |
| TCP | 20 | 20000 | 20000 | 0 | 100.000 | 139.955 | 288.742 | 357.700 |
| TCP | 50 | 50000 | 50000 | 0 | 100.000 | 316.558 | 769.911 | 1593.879 |
| TCP | 100 | 100000 | 100000 | 0 | 100.000 | 405.818 | 2908.420 | 6416.896 |
| UDP | 1 | 1000 | 995 | 5 | 99.500 | 39.477 | 82.047 | 114.188 |
| UDP | 2 | 2000 | 1998 | 2 | 99.900 | 39.772 | 88.844 | 126.086 |
| UDP | 5 | 5000 | 4972 | 28 | 99.440 | 39.559 | 93.048 | 140.379 |
| UDP | 10 | 10000 | 9965 | 35 | 99.650 | 37.924 | 104.357 | 139.962 |
| UDP | 20 | 20000 | 19868 | 132 | 99.340 | 37.696 | 129.373 | 164.718 |
| UDP | 50 | 50000 | 48364 | 1636 | 96.728 | 83.560 | 169.870 | 180.362 |
| UDP | 100 | 100000 | 90811 | 9189 | 90.811 | 101.949 | 172.500 | 181.912 |
| MQTT | 1 | 1000 | 1000 | 0 | 100.000 | 39.870 | 91.782 | 129.790 |
| MQTT | 2 | 2000 | 2000 | 0 | 100.000 | 39.982 | 89.926 | 149.817 |
| MQTT | 5 | 5000 | 5000 | 0 | 100.000 | 40.010 | 112.105 | 161.794 |
| MQTT | 10 | 10000 | 10000 | 0 | 100.000 | 41.768 | 129.995 | 175.859 |
| MQTT | 20 | 20000 | 20000 | 0 | 100.000 | 47.686 | 175.844 | 529.779 |
| MQTT | 50 | 50000 | 49998 | 2 | 99.996 | 173.945 | 1117.707 | 1949.906 |
| MQTT | 100 | 100000 | 99499 | 501 | 99.499 | 909.719 | 5135.984 | 21776.949 |

## Verification

- 564 sender CSVs were produced, matching TCP/UDP/MQTT across `1+2+5+10+20+50+100 = 188` client files per transport.
- Every sender CSV contains `1000` samples plus header.
- 202 edge receiver CSVs were pulled under `base_station/`; TCP and UDP use one receiver per condition, while MQTT uses one receiver per source ID.
- GNSS CSV and ROS bag were recorded.
- Local and edge experiment processes were cleaned up after the run.

## Notes

- This completes the weak-signal repeat of Experiment 11.
- TCP remained at `100%` accepted probes, but high-client RTT tails grew sharply under weak signal.
- UDP acceptance degraded with scale, reaching `90.811%` at 100 clients.
- MQTT retained high acceptance, but 50 and 100 clients had large latency tails; the 100-client condition showed intermittent 30-second ACK timeouts before recovering and completing.
