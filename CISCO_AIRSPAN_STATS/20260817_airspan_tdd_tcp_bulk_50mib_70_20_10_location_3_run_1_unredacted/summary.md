# Directional 50-MiB TCP Bulk Transfer

- TDD: `70/20/10`
- Location: `location_3`, stationary and unchanged from the preceding GNSS capture
- RSRP: `-100 dBm`
- RSRQ: `-13 dB`
- Transport: one TCP stream
- Exact application bytes per direction: `52,428,800` (50 MiB)

| Direction | Sender | Receiver | Receiver elapsed | Receiver throughput |
|---|---|---|---:|---:|
| Upload | vehicle | d1 | 34.550 s | 12.140 Mb/s |
| Download | d1 | vehicle | 2.784 s | 150.631 Mb/s |

Both transfers completed with matching endpoint byte counts and CRC32
`34909490`. Under this single run, receiver-observed downlink throughput was
12.408 times receiver-observed uplink throughput. This is a single-flow TCP
host-path result under the recorded deployment context, not radio PHY capacity
and not a causal comparison with another TDD profile.
