# Operator note

- Written: 2026-07-03T17:37:38
- User requested stopping the active run and treating the stopped payload as timeout.
- 2048-byte raw partial: 331 observed rows, 3 successes, 328 timeouts.
- Analysis convention: 2048-byte row in `payload_summary.csv` is 1000 attempts, 0 successes, 1000 timeouts.
- Raw data was preserved; no raw packet CSV was overwritten.
