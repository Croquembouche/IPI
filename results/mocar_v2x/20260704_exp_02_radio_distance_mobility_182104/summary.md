# Experiment 02 Radio Distance Mobility

- Run ID: `edge4av-exp02-mobility-20260704T182104`
- Condition ID: `mocar-exp02-mobility-route-los-moving-run3-payload-256`
- Payload: `256 B`
- Timeout threshold: `500 ms`
- Rows: `1000`
- Success: `591`
- Timeouts: `409`
- Success rate: `0.591`
- p50 RTT ms: `29.204`
- p95 RTT ms: `43.923`
- p99 RTT ms: `54.317`
- GNSS samples: `96720`
- OBU-local clock offset ns: ``
- Clock alignment median RTT ns: ``
- V2X/GNSS send-time join: `/home/hydrau/Documents/Github/IPI/results/mocar_v2x/20260704_exp_02_radio_distance_mobility_182104/v2x_gnss_by_send_time.csv`

## Timing Note

- In-run SSH clock probes did not return parseable device epoch values because the probe used a Python timestamp call that is not supported on the device Python.
- The V2X CSV still records `origin_send_epoch_ns` and `decision_epoch_ns` from the OBU for every row, and `v2x_gnss_by_send_time.csv` joins GNSS using the OBU send epoch timestamp.
- The collection script has been patched after this run to use shell `date +%s%N` plus `/proc/uptime` for future clock alignment probes.
