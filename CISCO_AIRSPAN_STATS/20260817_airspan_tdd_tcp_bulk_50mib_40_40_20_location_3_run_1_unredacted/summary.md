# Directional 50-MiB TCP Throughput — `40/40/20`

This run contains 10 sequential repetitions at stationary `location_3`, RSRP
`-100 dBm`, and RSRQ `-13 dB`. Each repetition first transfers exactly
52,428,800 bytes from the vehicle to d1, then transfers exactly 52,428,800
bytes from d1 to the vehicle. All 20 direction-runs passed endpoint byte-count
and CRC32 validation.

| Direction | Mean Mb/s | Median Mb/s | Sample SD | 95% t interval | Min--max Mb/s |
|---|---:|---:|---:|---:|---:|
| Upload, vehicle to d1 | 2.836 | 2.923 | 0.211 | 2.685--2.987 | 2.546--3.168 |
| Download, d1 to vehicle | 99.756 | 103.433 | 24.379 | 82.316--117.195 | 39.596--122.804 |

The mean paired download/upload ratio is 35.033. This is receiver-observed,
single-flow TCP application goodput, not direct radio-PHY capacity. The
operator confirmed that TDD was the only radio-side setting changed relative
to the `70/20/10` comparison and that the other host applications introduced
negligible additional work relative to the experiment.
