# 2026-08-19 GL-X3000 TDD 70/20/10 Directional Repeat

## Scope And Direction Contract

This fresh-device run uses the GL-X3000 private-5G gateway at stationary
`location_3` under operator-reported TDD `70/20/10`. The fresh GNSS bag spans
59.616 seconds with 16,101 messages; its median is 2.98 m from the August 18
location-3 reference. The modem logger retained 285 unique ten-second 5G-SA
samples over 47.5 minutes: RSRP `-104` to `-95 dBm` (median `-99 dBm`), RSRQ
constant at `-10 dB`, and SINR `15` to `23 dB` (median `20 dB`). Serving-cell,
cell-administrative, and handoff states were not freshly reported and remain
null.

- Uplink RTT means vehicle sends the declared payload to d1, d1 validates the
  complete in-memory length and CRC32, and d1 returns a compact application
  acknowledgment.
- Downlink RTT means vehicle sends a compact request, d1 returns the declared
  in-memory payload, and vehicle RTT ends after complete length, sequence, and
  CRC32 validation. Validation time is stored separately and included in RTT.
- Upload bandwidth means vehicle to d1; download means d1 to vehicle.
- Payload bodies are not written to HDD inside a timed interval. Only compact
  result rows, summaries, and telemetry are persisted after timing boundaries.

## RTT Results

All 16 conditions completed exactly 500 accepted exchanges with zero failures:
4,000 uplink and 4,000 downlink.

| Direction | Transport | Payload | p50 RTT | p95 RTT | Max RTT |
|---|---|---:|---:|---:|---:|
| Uplink | TCP | 1 KiB | 41.513 ms | 53.352 ms | 69.885 ms |
| Uplink | TCP | 10 KiB | 45.888 ms | 57.516 ms | 65.393 ms |
| Uplink | TCP | 100 KiB | 85.091 ms | 102.547 ms | 130.876 ms |
| Uplink | TCP | 500 KiB | 231.046 ms | 258.200 ms | 831.145 ms |
| Uplink | MQTT | 1 KiB | 39.411 ms | 46.948 ms | 57.586 ms |
| Uplink | MQTT | 10 KiB | 45.537 ms | 55.494 ms | 63.540 ms |
| Uplink | MQTT | 100 KiB | 89.215 ms | 109.402 ms | 129.790 ms |
| Uplink | MQTT | 500 KiB | 238.607 ms | 267.336 ms | 911.888 ms |
| Downlink | TCP | 1 KiB | 36.001 ms | 45.814 ms | 53.266 ms |
| Downlink | TCP | 10 KiB | 37.489 ms | 45.937 ms | 53.451 ms |
| Downlink | TCP | 100 KiB | 43.857 ms | 82.671 ms | 187.940 ms |
| Downlink | TCP | 500 KiB | 77.744 ms | 93.403 ms | 162.838 ms |
| Downlink | MQTT | 1 KiB | 29.662 ms | 40.001 ms | 45.099 ms |
| Downlink | MQTT | 10 KiB | 33.210 ms | 44.210 ms | 56.330 ms |
| Downlink | MQTT | 100 KiB | 44.551 ms | 83.172 ms | 111.689 ms |
| Downlink | MQTT | 500 KiB | 80.493 ms | 97.806 ms | 155.661 ms |

Across downlink conditions, payload-validation p50 ranges from 0.044 to
1.406 ms and p95 from 0.081 to 2.082 ms.

## Exact 50-MiB Bandwidth

All six direction-runs transferred exactly 52,428,800 bytes and passed matching
endpoint CRC32 validation. Receiver-observed upload throughput is 15.691 Mb/s
mean (15.647 median, 0.144 sample SD, 15.334--16.049 two-sided 95% t interval,
`n=3`). Download throughput is 166.106 Mb/s mean (172.970 median, 12.711 sample
SD, 134.531--197.681 95% t interval, `n=3`).

## Device Check

The closest same-profile/location comparison is the August 17 old-device
TDD-70/20/10 dataset at location 3 and reported RSRP `-100 dBm`. Relative to
that dataset, the new-device mean upload is 3.691 Mb/s or 30.8% higher, and mean
download is 39.320 Mb/s or 31.0% higher. Downlink RTT is mixed rather than
uniformly improved: new-device p50 changes range from 7.5% lower to 20.7%
higher, and most values remain in the same tens-of-milliseconds range.

The dramatic uplink RTT difference from August 18 is real as an observed
cross-run difference: at 500 KiB, new-device p50 is 97.1% lower for TCP and
96.9% lower for MQTT. At 100 KiB it is approximately 94.4% lower for both.
That comparison does **not** isolate the device because August 18 used TDD
`40/40/20`, weaker/mixed RSRP, and a different collection time. Moreover, the
old device had already delivered 12.000 Mb/s mean upload under TDD `70/20/10`
on August 17, so the August 18 0.5-Mb/s behavior is not evidence of an immutable
old-device hardware cap. The defensible conclusion is that the severe August
18 uplink condition did not persist during the replacement-device run and
therefore was not a stable capacity limit of the radio deployment. This
experiment alone cannot separate old-device hardware, configuration, session
state, and contemporaneous radio/path effects.
