# Operator note

- Written: 2026-07-04T14:29:53
- User requested stopping the active 1024-byte run, treating the rest of 1024 as timeout, and treating 2048 bytes as all timeout without testing it.
- 1024-byte raw partial: 900 observed rows, 0 successes, 900 timeouts.
- Analysis convention: 1024-byte and 2048-byte rows in `payload_summary.csv` are 1000 attempts, 0 successes, 1000 timeouts.
- Raw partial data was preserved; no raw packet CSV was overwritten.
