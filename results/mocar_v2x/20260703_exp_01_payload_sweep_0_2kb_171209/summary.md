# Mocar exp_01 0-2KB payload sweep

- Status: stopped at operator request; 2048-byte payload is treated as timeout for analysis.
- Run ID: `20260703_exp_01_payload_sweep_0_2kb_171209`
- Run directory: `results/mocar_v2x/20260703_exp_01_payload_sweep_0_2kb_171209`
- Payloads: `0, 256, 512, 1024, 2048`
- Attempts per payload for analysis: `1000`
- GNSS samples: `294691`
- Mean GNSS latitude: `39.6667882439804`
- Mean GNSS longitude: `-75.75736767719877`
- Latest GNSS CSV row: `1783114471371307718,1783114471.370485537,/novatel/oem7/odom,/novatel/oem7/fix,/novatel/oem7/odom,39.666788406,-75.757365915,-0.542706402,0.001270090,-0.001795095,0.000541297,0.001148967,-0.001795095,,,`

## Payload Summary

| payload bytes | analysis attempts | observed rows | analysis success | analysis timeouts | analysis success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms | note |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 0 | 1000 | 1000 | 1000 | 0 | 1.000 | 97.908 | 99.971 | 108.645 | 111.728 | observed |
| 256 | 1000 | 1000 | 999 | 1 | 0.999 | 98.706 | 99.968 | 108.903 | 113.727 | observed |
| 512 | 1000 | 1000 | 999 | 1 | 0.999 | 101.953 | 99.965 | 107.833 | 112.632 | observed |
| 1024 | 1000 | 1000 | 976 | 24 | 0.976 | 100.724 | 99.953 | 110.829 | 116.660 | observed |
| 2048 | 1000 | 331 | 0 | 1000 | 0.000 |  |  |  |  | operator_stopped_run; raw_observed_rows=331; raw_observed_success=3; raw_observed_timeouts=328; treated_as_1000_timeouts; raw_preserved_as_remote_obu/obu/payload_2048_observed_partial.csv; synthetic_file=analysis/payload_2048_treated_timeout.csv |

## Operator Note

- The original 2048-byte run was interrupted before completion. The raw partial file is preserved at `remote_obu/obu/payload_2048.csv` and copied to `remote_obu/obu/payload_2048_observed_partial.csv`.
- Raw observed 2048-byte data: `331` rows, `3` successes, `328` timeouts.
- For the paper-facing analysis artifact, 2048 bytes is counted as `1000` attempts, `0` successes, and `1000` timeouts.
- A synthetic timeout-only CSV for that convention is stored at `analysis/payload_2048_treated_timeout.csv`.
- OBU signal logs are stored under `remote_obu/signal/`; no C-V2X RSSI/SNR field was exposed by the available OBU diagnostics during this run.
