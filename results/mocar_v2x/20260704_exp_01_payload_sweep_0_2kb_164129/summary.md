# Mocar exp_01 0-2KB payload sweep

- Status: final exp_01 location; stopped at operator request during 256-byte payload after timeout-dominant behavior. Payloads 256/512/1024/2048 are treated as timeout for analysis.
- Run ID: `20260704_exp_01_payload_sweep_0_2kb_164129`
- Run directory: `results/mocar_v2x/20260704_exp_01_payload_sweep_0_2kb_164129`
- Payloads: `0, 256, 512, 1024, 2048`
- Attempts per payload for analysis: `1000`
- GNSS samples: `342487`
- Mean GNSS latitude: `39.6629052667121`
- Mean GNSS longitude: `-75.75741299926972`
- Latest GNSS CSV row: `1783199249193220950,1783199249.192735254,/novatel/oem7/odom,/novatel/oem7/fix,/novatel/oem7/odom,39.662898773,-75.757423428,0.298159172,0.002739645,0.002203668,-0.001483315,-0.002303352,0.002203668,,,`

## Payload Summary

| payload bytes | analysis attempts | observed rows | analysis success | analysis timeouts | analysis success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms | note |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 0 | 1000 | 1000 | 0 | 1000 | 0.000 |  |  |  |  | observed |
| 256 | 1000 | 332 | 0 | 1000 | 0.000 |  |  |  |  | operator_stopped_run; raw_observed_rows=332; raw_observed_success=0; raw_observed_timeouts=332; remaining_entries_and_analysis_treated_as_1000_timeouts; raw_preserved_as_remote_obu/obu/payload_256_observed_partial.csv; synthetic_file=analysis/payload_256_treated_timeout.csv |
| 512 | 1000 | 0 | 0 | 1000 | 0.000 |  |  |  |  | operator_skipped_run; treated_as_1000_timeouts; synthetic_file=analysis/payload_512_treated_timeout.csv |
| 1024 | 1000 | 0 | 0 | 1000 | 0.000 |  |  |  |  | operator_skipped_run; treated_as_1000_timeouts; synthetic_file=analysis/payload_1024_treated_timeout.csv |
| 2048 | 1000 | 0 | 0 | 1000 | 0.000 |  |  |  |  | operator_skipped_run; treated_as_1000_timeouts; synthetic_file=analysis/payload_2048_treated_timeout.csv |

## Operator Note

- Payload 0 completed normally and is preserved as observed raw data.
- The 256-byte run was interrupted at operator request after timeout-dominant behavior.
- Raw observed 256-byte data: `332` rows, `0` successes, `332` timeouts.
- For paper-facing analysis, 256, 512, 1024, and 2048 bytes are each counted as `1000` attempts, `0` successes, and `1000` timeouts.
- Raw partial CSVs are preserved where available, and synthetic timeout-only CSVs are stored under `analysis/`.
