# Mocar exp_01 0-2KB payload sweep

- Status: stopped at operator request during 2048-byte payload; remaining 2048-byte attempts are treated as timeout for analysis.
- Run ID: `20260704_exp_01_payload_sweep_0_2kb_155120`
- Run directory: `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_155120`
- Payloads: `0, 256, 512, 1024, 2048`
- Attempts per payload for analysis: `1000`
- GNSS samples: `603787`
- Mean GNSS latitude: `39.663789349432875`
- Mean GNSS longitude: `-75.75855746369622`
- Latest GNSS CSV row: `1783197427496390701,1783197427.495673249,/novatel/oem7/odom,/novatel/oem7/fix,/novatel/oem7/odom,39.663811117,-75.758487502,0.059086378,1.598475236,-0.001326571,1.598224130,0.028332116,-0.001326571,,,`

## Payload Summary

| payload bytes | analysis attempts | observed rows | analysis success | analysis timeouts | analysis success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms | note |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 0 | 1000 | 1000 | 999 | 1 | 0.999 | 94.054 | 99.954 | 106.976 | 112.627 | observed |
| 256 | 1000 | 1000 | 991 | 9 | 0.991 | 97.824 | 99.969 | 107.847 | 112.593 | observed |
| 512 | 1000 | 1000 | 969 | 31 | 0.969 | 98.883 | 99.969 | 110.741 | 115.691 | observed |
| 1024 | 1000 | 1000 | 171 | 829 | 0.171 | 79.212 | 98.911 | 111.730 | 115.700 | observed |
| 2048 | 1000 | 910 | 0 | 1000 | 0.000 |  |  |  |  | operator_stopped_run; raw_observed_rows=910; raw_observed_success=0; raw_observed_timeouts=910; remaining_entries_and_analysis_treated_as_1000_timeouts; raw_preserved_as_remote_obu/obu/payload_2048_observed_partial.csv; synthetic_file=analysis/payload_2048_treated_timeout.csv |

## Operator Note

- The 2048-byte run was interrupted at operator request after timeout-dominant behavior.
- Raw observed 2048-byte data: `910` rows, `0` successes, `910` timeouts.
- For the paper-facing analysis artifact, 2048 bytes is counted as `1000` attempts, `0` successes, and `1000` timeouts.
- The raw partial CSV is preserved at `remote_obu/obu/payload_2048.csv` and copied to `remote_obu/obu/payload_2048_observed_partial.csv`.
- A synthetic timeout-only CSV is stored at `analysis/payload_2048_treated_timeout.csv`.
