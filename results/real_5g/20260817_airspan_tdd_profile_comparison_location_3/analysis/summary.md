# Private-5G TDD Profile Comparison

This derived result keeps three experiment families separate:

1. The August 15 TCP/MQTT runs measure uplink-heavy RTT: the vehicle sends the
   declared object to d1 and waits for a compact acknowledgment.
2. The August 17 TCP/MQTT runs measure downlink-heavy RTT: the vehicle sends a
   compact request and waits for the declared object from d1.
3. The August 17 exact 50-MiB TCP runs measure single-flow application goodput
   in both directions. They are not pooled with either RTT family.

## Uplink-heavy RTT

The primary August 15 comparison contains 1,000 accepted messages in every
profile, transport, and payload condition. `70/20/10` has lower p50 and p95 RTT
in all 16 percentile comparisons. Depending on transport, payload, and
percentile, its RTT is 13.3%--68.6% below `40/40/20`, measured relative to the
slower profile. The August 16 uplink repetitions are retained separately and
are not pooled because serving-cell and cell-broadcast context changed.

## Downlink-heavy RTT

The August 17 comparison contains 1,000 accepted responses in every profile,
transport, and payload condition. For 1--10 KiB, `40/40/20` lowers p50 and p95
RTT by 4.9%--20.0%. TCP at 100 KiB has a 28.9% lower p50 and a 3.1% lower p95
under `40/40/20`; TCP at 500 KiB is effectively tied. MQTT at 100 KiB has a
3.1% lower p50 but a 31.7% higher p95 under `40/40/20`. At 500 KiB, MQTT under
`70/20/10` has a 5.5% lower p50 and a 28.7% lower p95.

## Exact 50-MiB TCP bandwidth

Each profile contains 10 validated upload/download pairs. All 40 direction-runs
match exactly 52,428,800 bytes and the endpoint CRC32.

| Direction | `70/20/10` mean | `40/40/20` mean | Difference |
|---|---:|---:|---:|
| Upload, vehicle to d1 | 12.000 Mb/s | 2.836 Mb/s | `40/40/20` is 9.164 Mb/s or 76.4% lower; `70/20/10` is 4.23x higher |
| Download, d1 to vehicle | 126.786 Mb/s | 99.756 Mb/s | `40/40/20` is 27.031 Mb/s or 21.3% lower; `70/20/10` is 1.27x higher |

The operator confirmed that TDD was the only radio-side setting changed and
that the other host applications introduced negligible additional work relative
to the experiment. These are measured end-to-end application RTT and
single-flow TCP-goodput effects in this deployment, not direct radio-PHY
capacity measurements. There is no pooled overall latency value across the
uplink-heavy and downlink-heavy RTT families.
