# Mocar exp_01 consolidated 0-2KB payload sweep

This folder consolidates the successful payload datasets for the 0-2KB sweep. It is not one continuous run; each payload source is recorded below.

| payload bytes | rows | success | success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | 1000 | 1000 | 1.000 | 93.252 | 99.939 | 106.873 | 111.814 |
| 256 | 1000 | 999 | 0.999 | 98.324 | 99.980 | 106.884 | 111.691 |
| 512 | 1000 | 1000 | 1.000 | 98.848 | 99.972 | 106.691 | 114.577 |
| 1024 | 1000 | 1000 | 1.000 | 99.841 | 99.966 | 106.877 | 112.513 |
| 2048 | 1000 | 1000 | 1.000 | 98.515 | 99.958 | 111.661 | 120.562 |

Sources:

- 0 bytes: `remote OBU /root/edge4av_exp/exp_01/20260703_exp_01_payload_sweep_run_4/obu/payload_0.csv`
- 256 bytes: `results/mocar_v2x/20260703_exp_01_payload_256_full_160216/remote_obu/obu/payload_256.csv`
- 512 bytes: `results/mocar_v2x/20260703_exp_01_payload_512_full_160733/remote_obu/obu/payload_512.csv`
- 1024 bytes: `results/mocar_v2x/20260703_exp_01_payload_1024_full_161137/remote_obu/obu/payload_1024.csv`
- 2048 bytes: `results/mocar_v2x/20260703_exp_01_payload_2048_full_161542/remote_obu/obu/payload_2048.csv`
