# August 15--18 TDD and DU Cell Comparison

Last updated: 2026-08-18

## Purpose

This analysis compares the `70/20/10` and `40/40/20`
downlink/uplink/dynamic time-division duplex (TDD) profiles using the
application records collected from August 15 through August 18, 2026, and the
Airspan DU Cell exports. It preserves four distinct measurement families:

1. Uplink-heavy round-trip time (RTT): the vehicle sends the declared payload
   to `d1`, and `d1` returns a compact acknowledgment.
2. Downlink-heavy RTT: the vehicle sends a compact request, and `d1` returns
   the declared payload to the vehicle.
3. Exact 50-MiB TCP upload: the vehicle sends 52,428,800 bytes to `d1`.
4. Exact 50-MiB TCP download: `d1` sends 52,428,800 bytes to the vehicle.

No result is pooled across direction, TDD profile, experiment family,
transport, payload size, serving cell, location, or signal condition.

## Main conclusions

1. **The August 15--16 uplink-heavy RTT data remain inconclusive for a TDD
   ranking.** The DU export confirms that the serving cell and cell state
   changed across the `40/40/20` repetitions. The August 16 `70/20/10` run has
   no DU coverage, while the referenced August 15 `40/40/20` raw run and its
   timestamps are absent. These runs remain valid measurements of their
   recorded conditions, but they do not isolate the TDD profile.
2. **The August 17 downlink-heavy data provide a clean, same-cell TDD
   comparison.** Both profiles completed 8,000 of 8,000 exchanges at the same
   location and reported signal condition. The DU export contains six complete
   five-minute Cell 2 bins within each application block, with zero reported
   cell unavailability. `40/40/20` reduces small-object RTT, but it does not
   reduce every large-object tail. Nominal TDD allocation therefore does not
   predict application RTT by itself.
3. **The August 17 exact 50-MiB result is a valid endpoint-goodput comparison,
   separated by direction.** `70/20/10` achieved higher measured application
   goodput in both upload and download. Because upload and download transfers
   alternate inside the same five-minute DU bins, the DU export cannot isolate
   each bulk direction. The endpoint byte, checksum, direction, and timing
   records remain the source for this result.
4. **The August 18 `40/40/20` repeat exposes a strong directional limit under
   weak signal.** At the matched 500-KiB, MQTT, `-115 dBm` condition, uplink
   p50 RTT is 61.5 times downlink p50 RTT, and uplink p95 RTT is 41.0 times
   downlink p95 RTT. The exact bulk runs show the same direction: two uploads
   average 0.516 Mb/s, while the one completed download reaches 42.746 Mb/s.

The deployment used one experimental user equipment (UE), one dedicated
radio, and a clean n48 channel. Thus, uncontrolled public-network clients do
not explain these observations. The August 15--16 uplink comparison remains
inconclusive because the measured platform state changed, not because of
unknown background-user contention.

## DU Cell data preparation

The seven DU exports are overlapping snapshots. Their 10,012 physical rows
collapse to 3,906 unique interval/cell records when deduplicated by:

```text
(Node Name, Managed Element ID, Cell, Start Time, End Time)
```

Repeated rows agree after normalizing Excel-formatted numeric strings such as
`="0.1"`. The August 15--18 records use five-minute intervals except for the
documented gaps and partial rows. Zero-duration rows are excluded.

The DU timestamps contain no timezone. Their alignment with the August 18
uplink and downlink traffic transitions strongly supports interpreting them as
America/New_York local time, or EDT during these experiments. This timezone is
inferred from the data and is not vendor-confirmed.

The exported throughput is derived from volume and active time. For any set of
included bins, this analysis recomputes the aggregate value as:

```text
active-time-derived throughput (kbps)
    = 1000 * sum(DRB Cell Volume (kb)) / sum(DRB Cell Time (ms))
```

It does not average the per-bin throughput column. Wall-clock cell load is:

```text
wall-clock cell load (kbps)
    = sum(DRB Cell Volume (kb)) / included wall-clock seconds
```

These DU quantities are cell-level context. They are not endpoint goodput,
radio capacity, per-request latency, or a direct measurement of the
experiment UE. The DU export has no TDD, RSRP, RSRQ, SINR, CQI, MCS, BLER,
PRB-utilization, retransmission, queue, or per-UE identifier field. TDD,
signal, serving-cell, direction, and experiment labels come from the
application and operator records.

## 1. Uplink-heavy RTT

### TCP application latency

Every listed complete condition has 1,000 accepted exchanges and zero failed
exchanges. Values are p95 RTT in milliseconds. The DU column reports the
serving cell's uplink active-time-derived throughput over complete interior
five-minute bins for that run; it is not application goodput.

| TDD profile | Date and repetition | Serving cell | RSRP | 1 KiB | 10 KiB | 100 KiB | DU uplink context |
|---|---|---:|---:|---:|---:|---:|---:|
| `70/20/10` | Aug. 15 run 2 | 2 | -100 dBm | 41.721 | 49.082 | 111.503 | 12.107 Mb/s; 22 bins |
| `70/20/10` | Aug. 16 quick run 1 | 2 | -98 dBm, carried forward | 41.975 | 49.190 | 112.947 | No DU rows after 15:15 |
| `40/40/20` | Aug. 15 reference | 2, derived context | -100 dBm, derived context | 49.886 | 71.438 | 293.285 | Cannot align; raw run and timestamps absent |
| `40/40/20` | Aug. 16 quick run 1 | 2 | -100 dBm | 51.548 | 91.421 | 789.094 | 6.555 Mb/s; 3 bins |
| `40/40/20` | Aug. 16 quick run 2 | 1 | -98 dBm | 149.399 | 187.382 | 927.334 | 5.638 Mb/s; 4 bins |
| `40/40/20` | Aug. 18 run 1 | 2 | -105 dBm | 51.497 | 269.704 | 1,873.188 | 2.542 Mb/s; 29-bin run context |

The two `70/20/10` repetitions agree closely: their p95 changes are 0.61%,
0.22%, and 1.29% at 1, 10, and 100 KiB. The `40/40/20` repetitions do not.
For example, the two same-day TCP repetitions change serving cell and produce
p95 differences of 189.83%, 104.97%, and 17.52%. The DU rows independently
confirm that quick run 1 carried traffic on Cell 2 and quick run 2 carried
traffic on Cell 1.

### MQTT application latency

Values are p95 RTT in milliseconds. The interrupted August 16 2-MiB condition
is excluded.

| TDD profile | Date and repetition | Serving cell | RSRP | 1 KiB | 10 KiB | 100 KiB | Larger payload | DU uplink context |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| `70/20/10` | Aug. 15 run 2 | 2 | -100 dBm | 40.476 | 50.225 | 111.174 | 1 MiB: 810.505 ms | Included in 12.107-Mb/s run context |
| `40/40/20` | Aug. 15 reference | 2, derived context | -100 dBm, derived context | 46.708 | 105.436 | 353.556 | 1 MiB: 2,125.408 ms | Cannot align; raw run and timestamps absent |
| `40/40/20` | Aug. 16 repeat 1 | 2 | -100 dBm | 51.016 | 169.247 | 746.947 | 1 MiB: 5,608.034 ms | 6.482 Mb/s; 18 clean bins |
| `40/40/20` | Aug. 18 run 1 | 2 | -105 to -115 dBm | 50.274 | 285.447 | 2,159.130 | 500 KiB: 9,287.828 ms | 2.542 Mb/s; 29-bin run context |

The August 16 MQTT DU aggregate excludes the `10:05--10:10` Cell 2 bin
because it reports nonzero fault unavailability. The remaining 18 complete
interior bins give 6.482 Mb/s of uplink active-time-derived throughput.

### Uplink-heavy decision

The uplink-heavy application data show repeatable `70/20/10` RTT and unstable
`40/40/20` RTT, especially as payload grows. However, the stored evidence does
not provide one timestamp-aligned, same-cell DU pair across the two profiles.
The profiles therefore cannot be ranked by uplink-heavy RTT from these runs.
The evidence does establish that large vehicle-to-edge objects become highly
sensitive to the recorded radio and platform condition.

## 2. Downlink-heavy RTT

### Matched August 17 application comparison

Both profiles used stationary location 3, reported RSRP `-100 dBm`, reported
RSRQ `-13 dB`, and serving Cell 2. Each transport/payload condition completed
1,000 of 1,000 exchanges with zero failures. Each cell is p50/p95 RTT in
milliseconds.

| Transport | Payload | `70/20/10` | `40/40/20` | Observed difference |
|---|---:|---:|---:|---|
| TCP | 1 KiB | 29.830 / 43.574 | 28.373 / 38.771 | `40/40/20` lower at p50 and p95 |
| TCP | 10 KiB | 31.655 / 44.120 | 29.518 / 39.624 | `40/40/20` lower at p50 and p95 |
| TCP | 100 KiB | 47.403 / 71.660 | 33.697 / 69.455 | `40/40/20` lower at p50 and p95 |
| TCP | 500 KiB | 68.943 / 95.330 | 69.181 / 93.619 | Effectively tied |
| MQTT | 1 KiB | 25.631 / 39.903 | 21.727 / 31.923 | `40/40/20` lower at p50 and p95 |
| MQTT | 10 KiB | 33.909 / 45.460 | 29.612 / 39.525 | `40/40/20` lower at p50 and p95 |
| MQTT | 100 KiB | 39.136 / 49.067 | 37.921 / 71.866 | `40/40/20` lower p50; `70/20/10` lower p95 |
| MQTT | 500 KiB | 72.576 / 86.316 | 76.786 / 120.984 | `70/20/10` lower at p50 and p95 |

`40/40/20` reduces p50 and p95 RTT for 1--10-KiB responses. It also reduces
TCP p50 through 100 KiB. The advantage disappears at 500-KiB TCP and reverses
in the tail for 100--500-KiB MQTT. Thus, the profile effect depends on payload,
transport, and percentile; it is not one constant latency shift.

### Matched August 17 DU Cell context

The table uses only six complete five-minute Cell 2 bins fully contained in
each downlink-heavy block. It excludes boundary bins, the gap between profile
blocks, and the later bulk transfers. Both selected windows report zero cell
unavailability.

| TDD profile | Interior window | Bins | Cell DL volume | DL wall-clock load | DL active-time-derived throughput |
|---|---|---:|---:|---:|---:|
| `70/20/10` | 17:35--18:05 EDT | 6 | 8,004,535 kb | 4.447 Mb/s | 54.463 Mb/s |
| `40/40/20` | 18:50--19:20 EDT | 6 | 8,814,798 kb | 4.897 Mb/s | 61.285 Mb/s |

Both windows are strongly downlink-heavy. Their Cell downlink-to-uplink volume
ratios are 111.19 and 110.50, respectively. Relative to `70/20/10`, the
`40/40/20` window carries 10.1% more Cell downlink volume and has 12.5% higher
active-time-derived Cell downlink throughput. These values describe the
realized cell traffic during the selected application blocks. They do not
measure maximum radio capacity or identify the mechanism behind the RTT
difference.

## 3. Exact 50-MiB TCP upload

The August 17 primary comparison contains ten exact, checksum-validated
vehicle-to-`d1` uploads per profile at stationary location 3 and reported RSRP
`-100 dBm`. Upload is kept separate from download.

| TDD profile | Valid uploads | Mean application goodput | 95% confidence interval | Range |
|---|---:|---:|---:|---:|
| `70/20/10` | 10 | 12.000 Mb/s | 11.759--12.242 Mb/s | 11.332--12.547 Mb/s |
| `40/40/20` | 10 | 2.836 Mb/s | 2.685--2.987 Mb/s | 2.546--3.168 Mb/s |

The measured `40/40/20` upload goodput is 76.4% lower, and the measured
`70/20/10` upload goodput is 4.23 times higher. This is an end-to-end
single-flow application result. It does not establish that the nominally
larger `40/40/20` uplink allocation has lower radio capacity.

At `40/40/20` and `-115 dBm` on August 18, the two completed uploads reach
0.524 and 0.507 Mb/s, for a 0.516-Mb/s mean. This is 81.8% below the August 17
`40/40/20` mean, but the signal and collection time differ. It is a weak-signal
repeat under one profile, not another TDD comparison.

## 4. Exact 50-MiB TCP download

The August 17 primary comparison contains ten exact, checksum-validated
`d1`-to-vehicle downloads per profile at stationary location 3 and reported
RSRP `-100 dBm`. Download is kept separate from upload.

| TDD profile | Valid downloads | Mean application goodput | 95% confidence interval | Range |
|---|---:|---:|---:|---:|
| `70/20/10` | 10 | 126.786 Mb/s | 106.296--147.277 Mb/s | 54.050--155.329 Mb/s |
| `40/40/20` | 10 | 99.756 Mb/s | 82.316--117.195 Mb/s | 39.596--122.804 Mb/s |

The measured `40/40/20` download goodput is 21.3% lower, and the measured
`70/20/10` download goodput is 1.27 times higher. At `40/40/20` and
`-115 dBm` on August 18, the one completed download reaches 42.746 Mb/s. This
is 57.1% below the August 17 `40/40/20` mean, but one download is not a new
multi-repetition estimate.

The exact bulk experiment alternates upload and download transfers. As a
result, the five-minute DU bins contain both directions even though the DU
columns themselves are directional. No DU bin isolates each short download,
and the `70/20/10` primary bulk block has no complete interior bin. The DU
export is therefore not used to estimate direction-specific bulk goodput.

## 5. Matched weak-signal direction comparison

The August 18 MQTT 500-KiB measurements provide a direct application-direction
comparison under the same `40/40/20` profile, stationary location, serving
cell, transport, payload, and `-115 dBm` signal condition. Uplink and downlink
remain separate measurements.

| Direction | Accepted/attempts | p50 RTT | p95 RTT |
|---|---:|---:|---:|
| Vehicle-to-`d1` 500-KiB payload plus compact acknowledgment | 250/250 | 7,647.557 ms | 9,287.828 ms |
| Compact vehicle request plus `d1`-to-vehicle 500-KiB payload | 500/500 | 124.311 ms | 226.787 ms |

The uplink-heavy p50 is 61.5 times the downlink-heavy p50, and the uplink-heavy
p95 is 41.0 times the downlink-heavy p95. This result demonstrates that
application direction can dominate the communication latency of a large CAV
object even when the TDD profile, payload, transport, serving cell, location,
and signal bin are held constant.

## Evidence decision

The evidence supports three separate statements:

1. The August 15--16 uplink-heavy RTT data are useful operating-condition
   measurements but do not isolate a TDD effect.
2. The August 17 downlink-heavy RTT and exact 50-MiB endpoint-goodput data are
   valid matched application comparisons between `70/20/10` and `40/40/20` in
   this deployment.
3. The August 18 `40/40/20` repeat shows that weak-signal, large-object uplink
   communication is much more constrained than the matched downlink path.

The data do not support one universal ranking of the two TDD profiles. The
application result depends on direction, payload size, transport, signal
condition, and the stability of the network path during collection.

## Primary artifacts

- `CISCO_AIRSPAN_STATS/DU_Cell_Stat_Log/DUCellExport_20260816_1519.csv`
- `CISCO_AIRSPAN_STATS/DU_Cell_Stat_Log/DUCellExport_20260818_1342.csv`
- `CISCO_AIRSPAN_STATS/DU_Cell_Stat_Log/download_manifest.json`
- `results/real_5g/20260817_airspan_tdd_profile_comparison_location_3/analysis/downlink_rtt_comparison.csv`
- `results/real_5g/20260817_airspan_tdd_profile_comparison_location_3/analysis/throughput_summary.csv`
- `results/real_5g/20260817_airspan_tdd_profile_comparison_location_3/analysis/uplink_rtt_comparison.csv`
- `results/real_5g/20260818_airspan_tdd_40_40_20_location_3_directional_repeat/analysis/`
- `results/real_5g/tdd_comparison_status.json`
