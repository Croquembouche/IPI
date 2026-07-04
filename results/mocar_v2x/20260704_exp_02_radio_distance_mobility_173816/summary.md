# Experiment 02 Radio Distance Mobility - Run 1

- Run ID: `edge4av-mobility-20260704T173816`
- Condition ID: `mocar-exp02-mobility-route-los-moving-live-payload-256`
- Payload: `256 B`
- Link state: `los`
- Mobility state: `moving`
- Rows collected before operator stop: `675`
- Success: `515`
- Timeouts: `160`
- Success rate: `0.763`
- First success sequence: `1`
- p50 RTT ms: `99.610`
- p95 RTT ms: `108.421`
- p99 RTT ms: `112.436`
- GNSS source: `/home/hydrau/Documents/Github/IPI/results/mocar_v2x/20260704_exp_02_radio_distance_mobility_173816/gps`
- GNSS samples: `99516`
- GNSS duration s: `452.326`
- Median speed m/s: `7.131`

## Notes

- The run was stopped by operator instruction at the end of the first drive run.
- Successes and timeouts were interleaved across the route; the longest contiguous timeout burst was `50` samples starting at sequence `61`.
- Private-5G was not counted for this run; the edge reachability check is retained only as a diagnostic under `diagnostics/private5g_edge_unavailable/`.
