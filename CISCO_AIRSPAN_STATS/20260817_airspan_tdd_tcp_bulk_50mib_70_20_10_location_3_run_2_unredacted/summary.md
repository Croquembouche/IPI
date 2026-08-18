# Ten-Repetition Directional 50-MiB TCP Throughput

- TDD: `70/20/10`
- Location: `location_3`, stationary and unchanged
- RSRP: `-100 dBm`
- RSRQ: `-13 dB`
- Transport: one TCP stream per direction
- Repetitions: `10` new matched upload/download pairs
- Exact application bytes: `52,428,800` per direction per repetition
- Validation: `20/20` direction-runs passed endpoint byte-count and CRC32 checks

| Receiver-observed direction | Mean | Median | Sample SD | 95% t CI for mean | Min--max |
|---|---:|---:|---:|---:|---:|
| Vehicle to d1 upload | 12.000 Mb/s | 12.098 Mb/s | 0.338 Mb/s | 11.759--12.242 Mb/s | 11.332--12.547 Mb/s |
| d1 to vehicle download | 126.786 Mb/s | 136.166 Mb/s | 28.643 Mb/s | 106.296--147.277 Mb/s | 54.050--155.329 Mb/s |

Upload was stable across the ten repetitions (2.815% coefficient of
variation). Download varied substantially (22.592% coefficient of variation),
including one validated 54.050-Mb/s repetition. The paired download/upload
ratio had a mean of 10.596 and median of 11.466. The earlier single run remains
a pilot and is excluded from these primary `n=10` estimates.

These are single-flow TCP host-path measurements under one recorded deployment
condition. They do not measure radio PHY capacity and do not isolate a causal
TDD effect without a matched second-profile experiment.
