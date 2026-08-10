# Airspan Follow-Up: Weak Location

- Application matrix: `passed` (12/12 samples).
- Attempts: `6000`; accepted: `5994`; failed: `6`; success: `99.900%`.
- Signal class: `weak` (operator-reported pending MG52 radio-export validation).
- Physical placement: `mg52_on_trunk_floor_front_face_up_toward_sky`.
- Timing metric: request/response RTT; one-way fields are excluded because endpoint clocks were unsynchronized.
- Repository GNSS precision: `0.001` degree; exact coordinates and rosbags remain only in the excluded raw backup.
- Paper-facing status: pending ACP Cell 2 and MG52 radio-export alignment.

## Condition/Transport Summary

| condition | transport | attempts | accepted | success % | p50 ms | p95 ms | p99 ms | hit @100 ms % | hit @500 ms % |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| C1 | TCP | 1000 | 1000 | 100.000 | 126.758 | 150.429 | 230.408 | 0.500 | 100.000 |
| C1 | MQTT | 1000 | 1000 | 100.000 | 32.471 | 46.106 | 83.856 | 99.100 | 100.000 |
| C1 | UDP | 1000 | 1000 | 100.000 | 31.715 | 43.859 | 61.847 | 99.500 | 100.000 |
| C2 | TCP | 1000 | 1000 | 100.000 | 152.528 | 188.673 | 235.952 | 0.000 | 99.700 |
| C2 | MQTT | 1000 | 1000 | 100.000 | 125.968 | 162.864 | 242.489 | 4.400 | 99.700 |
| C2 | UDP | 1000 | 994 | 99.400 | 120.623 | 148.558 | 160.248 | 5.000 | 99.400 |

## Failure Observations

| sample | failed/attempts | recorded detail |
| --- | ---: | --- |
| C2-rep1-udp | 3/500 | udp ack timeout: 3 |
| C2-rep2-udp | 3/500 | udp ack timeout: 3 |

## Evidence Qualifications

- Automatic GNSS produced in-window positions for 12/12 valid samples.
- Repository-facing coordinates are rounded to 0.001 degree, and serialized rosbags are retained only in the excluded raw backup.
- Deadline rates use every attempt as the denominator. Timeouts and other unaccepted rows are deadline misses.
- ACP Cell 2 and MG52 exports are still required before signal-related interpretation or paper-facing use.
