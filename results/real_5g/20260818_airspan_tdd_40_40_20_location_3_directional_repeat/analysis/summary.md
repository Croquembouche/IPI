# 2026-08-18 TDD 40/40/20 Directional Repeat

The stationary location-3 acquisition completed all declared TCP/MQTT RTT
conditions with zero failed exchanges. The bandwidth stage stopped at the
operator-requested boundary and contains one complete upload/download pair
plus one additional upload.

## Direction Contract

- Uplink RTT: the vehicle sends the declared payload to d1, and d1 returns a
  compact acknowledgment.
- Downlink RTT: the vehicle sends a compact request, and d1 returns the
  declared payload to the vehicle. RTT ends after the vehicle validates the
  complete length, sequence number, and CRC32.
- Upload bandwidth: vehicle to d1.
- Download bandwidth: d1 to vehicle.

## RTT Summary

The uplink matrix contains 6,500 accepted exchanges and zero failures. TCP and
MQTT p50 RTT is approximately 39.5 ms at 1 KiB, 173--192 ms at 10 KiB,
1.54--1.58 s at 100 KiB, and 7.65--7.86 s at 500 KiB.

The downlink matrix contains 7,000 accepted exchanges and zero failures at
RSRP -115 dBm. Across TCP and MQTT, p50 RTT is 21.5--29.6 ms at 1 KiB,
29.6--31.7 ms at 10 KiB, 51.4--59.5 ms at 100 KiB, and 124.3--139.9 ms at
500 KiB. Vehicle-side payload-validation p50 ranges from 0.035 to 1.364 ms and
p95 from 0.048 to 1.700 ms. Validation is included in RTT.

At the matched MQTT 500-KiB, -115-dBm condition, downlink p50 RTT is about
61.5 times lower than uplink p50 RTT. Do not generalize that ratio across all
uplink conditions because their RSRP context differs.

## Bandwidth Summary

- Repetition 1 upload, vehicle to d1: 0.524 Mb/s.
- Repetition 1 download, d1 to vehicle: 42.746 Mb/s.
- Repetition 2 upload, vehicle to d1: 0.507 Mb/s.

Every retained direction-run transferred exactly 52,428,800 bytes and passed
endpoint CRC32 validation. Repetition 2 download and repetitions 3--5 did not
start, so these data are not a five-pair estimate.

## Radio Context

TDD remained 40/40/20 and location remained stationary. Operator-reported RSRP
was -105 dBm before 11:07 EDT, -110 dBm from 11:07 through 11:17:59, and
-115 dBm from 11:18 onward. MQTT uplink 100 KiB crossed all three periods: 89,
378, and 533 messages respectively. All downlink and bandwidth measurements
were collected at -115 dBm. Fresh RSRQ and cell context were not reported for
this run; retained values remain explicitly identified as carried forward.
