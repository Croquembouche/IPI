# Experiment 09 Detector-Output-To-IPI Summary

- Run ID: `edge4av-real-20260702-detector-output-to-ipi-run-1`
- Date/time: `2026-07-02`, stationary private-5G run
- Edge server: `10.100.100.6`
- Detector source: `results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/per_sample.csv`
- Detector samples used for manifest: `922`
- Payload sizing rule: `estimated_payload_bytes = 256 + pred_boxes * 96`
- Detector-output payload summary: min `13216`, p50 `19648`, p95 `22816`,
  p99 `23968`, max `25024` bytes
- Probe count: `1000` per completed condition
- Probe interval: `200` ms
- Network load: `idle`
- QoS profile: `default`
- Mobility state: `stationary`
- GNSS: recorded with NovAtel ROS 2 package under `gps/` and
  `gps_mqtt_continuation/gps/`

## Payload Set Rationale

This experiment overlaps with the earlier TCP/MQTT payload-size experiments, but
the interpretation is different: these payload sizes are derived from the
detector-output object-count distribution, not chosen as arbitrary transport
sweep points.

The automatic manifest included both p99 `23968` B and max `25024` B. These
only differ by `1056` B, so the MQTT continuation was pruned after collection
started. The core paper set is:

- `0` B baseline
- `4096` B control point from the earlier payload sweeps
- `19648` B detector p50
- `22816` B detector p95
- `23968` B detector p99 / tail point
- `60000` B IPI cap stress point

TCP also includes completed `256`, `1024`, and `25024` B optional points. MQTT
partial `256`, `1024`, and first-attempt `4096` artifacts are preserved with
`.partial_interrupted_*` suffixes and are excluded from the main results.

## Latency Results

| Transport | Payload B | Accepted | Success | p50 RTT ms | p95 RTT ms | p99 RTT ms | Mean RTT ms | Max RTT ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| MQTT | 0 | 1000/1000 | 1000/1000 | 27.023 | 49.826 | 65.177 | 29.192 | 255.648 |
| MQTT | 4096 | 1000/1000 | 1000/1000 | 43.746 | 63.268 | 77.832 | 45.903 | 165.948 |
| MQTT | 19648 | 1000/1000 | 1000/1000 | 91.986 | 157.092 | 195.864 | 102.269 | 619.513 |
| MQTT | 22816 | 1000/1000 | 1000/1000 | 101.600 | 167.841 | 197.606 | 109.903 | 231.750 |
| MQTT | 23968 | 1000/1000 | 1000/1000 | 111.672 | 183.705 | 209.486 | 120.612 | 307.806 |
| MQTT | 60000 | 1000/1000 | 1000/1000 | 243.656 | 403.790 | 681.227 | 264.616 | 957.062 |
| TCP | 0 | 1000/1000 | 1000/1000 | 120.004 | 151.696 | 185.130 | 123.546 | 526.058 |
| TCP | 256 | 1000/1000 | 1000/1000 | 119.819 | 153.022 | 181.691 | 122.512 | 365.872 |
| TCP | 1024 | 1000/1000 | 1000/1000 | 121.513 | 149.048 | 176.596 | 125.466 | 377.596 |
| TCP | 4096 | 1000/1000 | 1000/1000 | 79.857 | 105.694 | 125.650 | 81.484 | 224.320 |
| TCP | 19648 | 1000/1000 | 1000/1000 | 99.880 | 139.898 | 163.729 | 104.067 | 361.907 |
| TCP | 22816 | 1000/1000 | 1000/1000 | 129.713 | 209.762 | 249.999 | 138.991 | 658.159 |
| TCP | 23968 | 1000/1000 | 1000/1000 | 111.538 | 169.540 | 203.890 | 120.099 | 410.014 |
| TCP | 25024 | 1000/1000 | 1000/1000 | 134.948 | 213.894 | 275.649 | 146.431 | 576.022 |
| TCP | 60000 | 1000/1000 | 1000/1000 | 240.056 | 381.409 | 659.838 | 260.366 | 1205.567 |

## GNSS Snapshot

Primary capture:

- CSV samples in `gps/gps_samples.csv`: `727938`
- NovAtel `BESTPOS` samples: `33098`
- Mean `BESTPOS` latitude: `39.663545200`
- Mean `BESTPOS` longitude: `-75.757001841`
- Mean `BESTPOS` altitude: `34.064` m
- Mean `BESTPOS` horizontal speed: `0.0024` m/s
- ROS bag duration: `3309.825` s
- ROS bag messages: `893658`

MQTT continuation capture:

- CSV samples in `gps_mqtt_continuation/gps/gps_samples.csv`: `371265`
- NovAtel `BESTPOS` samples: `16876`
- Mean `BESTPOS` latitude: `39.663551692`
- Mean `BESTPOS` longitude: `-75.756998941`
- Mean `BESTPOS` altitude: `35.051` m
- Mean `BESTPOS` horizontal speed: `0.0024` m/s
- ROS bag duration: `1687.680` s
- ROS bag messages: `455672`

## Verification

- Completed main-condition sender CSVs contain `1000` samples plus header.
- Completed main-condition edge receiver CSVs under `base_station/` contain
  `1000` samples plus header.
- Local and edge receiver/broker processes were cleaned up after the run.
- `ros2 bag info` succeeded for both GNSS captures.

## Interpretation

- The detector-output p50 to p99 payloads are tightly clustered:
  `19648-23968` B. For the paper, plot these as detector-distribution points
  rather than as a generic payload sweep.
- MQTT p95 RTT grows from `63.268` ms at `4096` B to `183.705` ms at the
  detector p99 payload, and to `403.790` ms at the `60000` B cap.
- TCP p95 RTT grows from `105.694` ms at `4096` B to `169.540` ms at the
  detector p99 payload, and to `381.409` ms at the `60000` B cap.
- Use RTT for analysis because the run is marked `clock_sync_state=unsynced`;
  one-way uplink/downlink columns are not reliable without synchronized clocks.
