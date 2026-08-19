# 2026-08-19 GL-X3000 TDD 40/40/20 Directional Repeat

## Scope And Validity

This run repeats the new-device TDD `70/20/10` matrix at operator-reported TDD
`40/40/20`, stationary `location_3`, and the same GL-X3000 vehicle gateway. The
fresh 59.628-second GNSS bag contains 16,102 messages; its median position is
1.85 m from the preceding `70/20/10` run. The two modem-logging segments contain
382 unique 5G-SA samples over 63.5 minutes of active capture, separated by an
operator-requested pause. Combined RSRP is `-102` to `-93 dBm` (median
`-98 dBm`), RSRQ is `-10 dB`, and SINR is `16` to `24 dB` (median `20 dB`).
Serving-cell, cell-administrative, and handoff state remain unreported.

The operator paused during the first attempt at uplink TCP 10 KiB. Its 142
accepted rows are retained as diagnostic data and excluded from every result
below. Uplink TCP 1 KiB completed before the pause; the clean TCP 10-KiB restart
and all later conditions completed after it. All final application conditions
contain exactly 500 validated exchanges.

- Uplink RTT means vehicle sends the declared payload, d1 validates its length
  and CRC32, and d1 returns a compact application acknowledgment.
- Downlink RTT means vehicle sends a compact request and ends RTT only after
  receiving and validating the complete d1 payload's length, sequence number,
  and CRC32. Validation time is separately retained and included in RTT.
- Upload bandwidth is vehicle to d1; download is d1 to vehicle.
- Payload bodies remain in memory. Payload-file HDD I/O is not included in RTT
  or goodput; only compact measurements and telemetry are persisted afterward.

## Application RTT

All 8,000 final exchanges completed successfully with zero failures.

| Direction | Transport | Payload | p50 RTT | p95 RTT | Max RTT |
|---|---|---:|---:|---:|---:|
| Uplink | TCP | 1 KiB | 39.968 ms | 52.850 ms | 65.515 ms |
| Uplink | TCP | 10 KiB | 62.977 ms | 87.639 ms | 279.610 ms |
| Uplink | TCP | 100 KiB | 170.916 ms | 268.867 ms | 475.817 ms |
| Uplink | TCP | 500 KiB | 609.652 ms | 1,149.368 ms | 2,041.143 ms |
| Uplink | MQTT | 1 KiB | 39.220 ms | 49.225 ms | 66.901 ms |
| Uplink | MQTT | 10 KiB | 59.009 ms | 87.707 ms | 490.249 ms |
| Uplink | MQTT | 100 KiB | 172.946 ms | 252.016 ms | 644.436 ms |
| Uplink | MQTT | 500 KiB | 580.942 ms | 1,104.331 ms | 2,377.005 ms |
| Downlink | TCP | 1 KiB | 38.666 ms | 49.444 ms | 89.601 ms |
| Downlink | TCP | 10 KiB | 39.606 ms | 51.624 ms | 309.568 ms |
| Downlink | TCP | 100 KiB | 51.373 ms | 95.152 ms | 487.418 ms |
| Downlink | TCP | 500 KiB | 101.529 ms | 320.891 ms | 4,050.986 ms |
| Downlink | MQTT | 1 KiB | 29.760 ms | 44.642 ms | 3,831.198 ms |
| Downlink | MQTT | 10 KiB | 34.174 ms | 49.778 ms | 1,882.287 ms |
| Downlink | MQTT | 100 KiB | 49.596 ms | 136.386 ms | 1,153.608 ms |
| Downlink | MQTT | 500 KiB | 86.417 ms | 138.168 ms | 234.548 ms |

Downlink validation p50 ranges from `0.043` to `1.335 ms`, and validation p95
ranges from `0.113` to `2.484 ms`. The multi-second downlink tail events are
therefore not caused by payload validation. They are accepted transfers that
completed late, not failed or missing responses.

## Exact 50-MiB Bandwidth

All six direction-runs transferred exactly 52,428,800 bytes and passed matching
endpoint CRC32 validation.

| Repetition | Upload, vehicle to d1 | Download, d1 to vehicle |
|---:|---:|---:|
| 1 | 7.802 Mb/s | 142.314 Mb/s |
| 2 | 12.888 Mb/s | 90.742 Mb/s |
| 3 | 12.050 Mb/s | 137.297 Mb/s |

Upload goodput averages `10.913 Mb/s` (median `12.050`, sample SD `2.727`,
95% t interval `4.140--17.687 Mb/s`, `n=3`). Download averages `123.451 Mb/s`
(median `137.297`, sample SD `28.438`, interval `52.808--194.094 Mb/s`). The
wide intervals reflect material between-repetition variability and the small
three-repetition sample.

## Previous-Device Comparison

The operator confirmed that this run and the preceding August 19 run use a
GL.iNet `GL-X3000` as the vehicle private-5G gateway. The comparison run from
August 18 used the preceding Cisco Meraki `MG52-HW` campaign gateway. Its
per-run JSON did not repeat the model field, so that identity comes from the
repository's campaign context and the reported replacement sequence.

Both runs used operator-reported TDD `40/40/20` and stationary `location_3`.
At 1 KiB, uplink p50 is effectively tied: TCP is `1.2%` higher and MQTT is
`1.0%` lower with GL-X3000. At 10, 100, and 500 KiB, GL-X3000 uplink p50 is,
respectively, `63.7%`, `88.9%`, and `92.2%` lower for TCP and `69.2%`, `89.0%`,
and `92.4%` lower for MQTT. Uplink p95 shows the same large-payload pattern,
falling `67.5--86.5%` for TCP and `69.3--88.1%` for MQTT.

Downlink does not show a uniform device advantage. GL-X3000 p50 is
`15.3--38.2%` higher at 1 and 10 KiB, is tied for TCP and `16.6%` lower for
MQTT at 100 KiB, and is `27.4--30.5%` lower at 500 KiB. Downlink p95 changes
range from `39.1%` lower to `39.9%` higher. The complete row-level comparison
is retained in `previous_device_rtt_comparison.csv`.

Available 50-MiB measurements average `10.913 Mb/s` upload with GL-X3000 versus
`0.515 Mb/s` over the two retained August 18 uploads, a `21.19x` ratio. GL-X3000
download averages `123.451 Mb/s` versus the single retained August 18 download
of `42.746 Mb/s`, a `2.89x` ratio. These values are in
`previous_device_bandwidth_comparison.csv`.

This is not a hardware-only A/B test. August 18 RSRP was operator-reported at
`-105 dBm`, then `-110 dBm`, and finally `-115 dBm`; the GL-X3000 run measured
RSRP `-102` to `-93 dBm` with median `-98 dBm`. The previous bandwidth stage
also stopped after two uploads and one download, whereas the GL-X3000 stage has
three complete pairs. The supported conclusion is that the severe August 18
large-uplink condition did not recur with GL-X3000. The current evidence cannot
separate device hardware from signal, configuration, session state, or
time-varying path effects. As a cleaner but still sequential control, the
same-signal TDD `70/20/10` comparison against August 17 found GL-X3000 mean
upload and download goodput `30.8%` and `31.0%` higher, while downlink RTT
remained mixed.

## Matched New-Device TDD Comparison

The immediately preceding new-device `70/20/10` run used the same location and
had comparable modem signal: median RSRP `-99 dBm` versus `-98 dBm` here.
Observed uplink RTT is essentially tied at 1 KiB. From 10 KiB upward,
`40/40/20` has higher p50 and p95 for both transports. At 500 KiB, its uplink
p50 is `163.9%` higher for TCP and `143.5%` higher for MQTT; p95 is `345.1%`
and `313.1%` higher, respectively.

For downlink, every TCP/MQTT payload has a higher p50 and p95 under `40/40/20`.
The p50 increase ranges from `0.3%` to `30.6%`. The p95 increase ranges from
`7.9%` to `243.6%`; the largest change is TCP 500 KiB, where rare multi-second
stalls raise p95 from `93.403` to `320.891 ms`.

Mean upload goodput is `10.913 Mb/s`, `4.778 Mb/s` or `30.4%` below the
`70/20/10` mean of `15.691 Mb/s`. Mean download is `123.451 Mb/s`, `42.655
Mb/s` or `25.7%` below `166.106 Mb/s`. Thus this sequential matched collection
observes lower application performance under `40/40/20` in both directions,
despite a slightly stronger median RSRP. The bandwidth estimate has only three
repetitions and high `40/40/20` variability, and the application matrix was
split by an operator pause. Treat the result as a matched within-deployment
observation, not a direct radio-PHY capacity measurement or a general result
for all networks using these profile labels.
