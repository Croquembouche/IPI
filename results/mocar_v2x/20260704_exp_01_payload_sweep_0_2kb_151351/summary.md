# Mocar exp_01 0-2KB payload sweep

- Run ID: `20260704_exp_01_payload_sweep_0_2kb_151351`
- Payloads: `0, 256, 512, 1024, 2048`
- Attempts per payload: `1000`
- GNSS samples: `455879`
- Mean GNSS latitude: `39.66551609743672`
- Mean GNSS longitude: `-75.75907085644465`
- Latest GNSS CSV row: `1783194506328834643,1783194506.328065679,/novatel/oem7/inspva,/novatel/oem7/inspva,/novatel/oem7/inspva,39.665512812,-75.759065333,-1.420362118,0.002423291,0.001942898,-0.000037253,0.002423005,0.001942898,,novatel_oem7_msgs.msg.InertialSolutionStatus(status=3),`

| payload bytes | rows | success | success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | 1000 | 1000 | 1.000 | 98.049 | 99.969 | 107.746 | 112.612 |
| 256 | 1000 | 1000 | 1.000 | 99.616 | 99.977 | 107.805 | 113.649 |
| 512 | 1000 | 998 | 0.998 | 101.220 | 99.967 | 107.815 | 110.843 |
| 1024 | 1000 | 979 | 0.979 | 97.127 | 99.965 | 109.777 | 114.644 |
| 2048 | 1000 | 8 | 0.008 | 69.609 | 74.950 | 106.210 | 106.210 |
