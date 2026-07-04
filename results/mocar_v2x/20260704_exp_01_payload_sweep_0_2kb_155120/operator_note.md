# Operator note

- Written: 2026-07-04T16:38:22
- User requested stopping the active 2048-byte run, treating the rest as timeout, and pulling results locally.
- 2048-byte raw partial: 910 observed rows, 0 successes, 910 timeouts.
- Analysis convention: 2048-byte row in `payload_summary.csv` is 1000 attempts, 0 successes, 1000 timeouts.
- Raw partial data was preserved; no raw packet CSV was overwritten.
