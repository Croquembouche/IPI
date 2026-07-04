# Mocar exp_01 0-2KB payload sweep

- Status: stopped at operator request during 1024-byte payload; 1024-byte and 2048-byte payloads are treated as timeout for analysis.
- Run ID: `20260704_exp_01_payload_sweep_0_2kb_133527`
- Run directory: `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_133527`
- Payloads: `0, 256, 512, 1024, 2048`
- Attempts per payload for analysis: `1000`
- GNSS samples: `436390`
- Mean GNSS latitude: `39.66690616379369`
- Mean GNSS longitude: `-75.75697012788874`
- Latest GNSS CSV row: `1783189692550029516,1783189692.547234378,/novatel/oem7/odom,/novatel/oem7/fix,/novatel/oem7/odom,39.666906501,-75.756969855,0.432089636,0.001478506,0.000360664,-0.000500276,0.001391296,0.000360664,,,`

## Payload Summary

| payload bytes | analysis attempts | observed rows | analysis success | analysis timeouts | analysis success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms | note |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 0 | 1000 | 1000 | 1000 | 0 | 1.000 | 107.235 | 99.988 | 196.691 | 204.559 | observed |
| 256 | 1000 | 1000 | 997 | 3 | 0.997 | 100.628 | 99.983 | 109.781 | 112.814 | observed |
| 512 | 1000 | 1000 | 955 | 45 | 0.955 | 97.080 | 99.968 | 109.045 | 112.558 | observed |
| 1024 | 1000 | 900 | 0 | 1000 | 0.000 |  |  |  |  | operator_stopped_run; raw_observed_rows=900; raw_observed_success=0; raw_observed_timeouts=900; remaining_entries_and_analysis_treated_as_1000_timeouts; raw_preserved_as_remote_obu/obu/payload_1024_observed_partial.csv; synthetic_file=analysis/payload_1024_treated_timeout.csv |
| 2048 | 1000 | 0 | 0 | 1000 | 0.000 |  |  |  |  | operator_skipped_run; treated_as_1000_timeouts; synthetic_file=analysis/payload_2048_treated_timeout.csv |

## Operator Note

- The 1024-byte run was interrupted after observing timeout-only behavior. The raw partial CSV is preserved at `remote_obu/obu/payload_1024.csv` and copied to `remote_obu/obu/payload_1024_observed_partial.csv`.
- Raw observed 1024-byte data: `900` rows, `0` successes, `900` timeouts.
- For the paper-facing analysis artifact, 1024 bytes is counted as `1000` attempts, `0` successes, and `1000` timeouts.
- The 2048-byte run was skipped by operator instruction and counted as `1000` attempts, `0` successes, and `1000` timeouts.
- Synthetic timeout-only CSVs are stored under `analysis/`.
- OBU signal logs are stored under `remote_obu/signal/`; no C-V2X RSSI/SNR field was exposed by the available OBU diagnostics during this run.
