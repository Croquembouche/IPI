# Mocar exp_01 0-2KB payload sweep

- Run ID: `20260704_exp_01_payload_sweep_0_2kb_144149`
- Payloads: `0, 256, 512, 1024, 2048`
- Attempts per payload: `1000`
- GNSS samples: `254689`
- Mean GNSS latitude: `39.666101558129114`
- Mean GNSS longitude: `-75.75784805083902`
- Latest GNSS CSV row: `1783191668703397979,1783191668.702756074,/novatel/oem7/inspva,/novatel/oem7/inspva,/novatel/oem7/inspva,39.666102334,-75.757847872,0.552803000,0.002385883,-0.001346582,-0.001775939,0.001593261,-0.001346582,,novatel_oem7_msgs.msg.InertialSolutionStatus(status=3),`

| payload bytes | rows | success | success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | 1000 | 999 | 0.999 | 98.690 | 99.978 | 108.812 | 112.730 |
| 256 | 1000 | 1000 | 1.000 | 100.022 | 99.970 | 107.879 | 113.705 |
| 512 | 1000 | 1000 | 1.000 | 99.025 | 99.975 | 108.860 | 112.662 |
| 1024 | 1000 | 1000 | 1.000 | 99.731 | 99.970 | 106.894 | 112.708 |
| 2048 | 1000 | 998 | 0.998 | 99.115 | 99.957 | 110.682 | 119.554 |
