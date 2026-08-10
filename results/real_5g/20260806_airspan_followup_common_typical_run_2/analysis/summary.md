# Airspan Follow-Up: Medium/Typical Location

- Application matrix: `passed` (12/12 samples).
- Attempts: `6000`; accepted: `5999`; failed: `1`; success: `99.983%`.
- Signal class: `medium_typical_deployment` (operator-corrected from a reported RSRP comparison; MG52 artifact import pending).
- Physical placement: `mg52_on_trunk_floor_front_face_up_toward_sky`.
- Timing metric: request/response RTT; one-way fields are excluded because endpoint clocks were unsynchronized.
- Repository GNSS precision: `0.001` degree; exact coordinates and rosbags remain only in the excluded raw backup.
- Paper-facing status: pending ACP Cell 2 and MG52 radio-export alignment.

## Condition/Transport Summary

| condition | transport | attempts | accepted | success % | p50 ms | p95 ms | p99 ms | hit @100 ms % | hit @500 ms % |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| C1 | TCP | 1000 | 1000 | 100.000 | 122.602 | 149.607 | 163.753 | 1.200 | 100.000 |
| C1 | MQTT | 1000 | 1000 | 100.000 | 30.389 | 41.358 | 57.186 | 99.800 | 100.000 |
| C1 | UDP | 1000 | 1000 | 100.000 | 30.149 | 40.706 | 47.717 | 99.500 | 100.000 |
| C2 | TCP | 1000 | 1000 | 100.000 | 90.217 | 109.833 | 191.95 | 81.200 | 100.000 |
| C2 | MQTT | 1000 | 1000 | 100.000 | 66.861 | 77.944 | 102.46 | 98.900 | 100.000 |
| C2 | UDP | 1000 | 999 | 99.900 | 60.753 | 72.847 | 80.34 | 99.400 | 99.900 |

## Failure Observations

| sample | failed/attempts | recorded detail |
| --- | ---: | --- |
| C2-rep2-udp | 1/500 | udp ack timeout: 1 |

## Evidence Qualifications

- `metadata_corrections.json` supersedes inconsistent per-transport planning fields with the run-level operator context.
- Automatic GNSS produced in-window positions for 12/12 valid samples.
- Repository-facing coordinates are rounded to 0.001 degree, and serialized rosbags are retained only in the excluded raw backup.
- Deadline rates use every attempt as the denominator. Timeouts and other unaccepted rows are deadline misses.
- ACP Cell 2 and MG52 exports are still required before signal-related interpretation or paper-facing use.
