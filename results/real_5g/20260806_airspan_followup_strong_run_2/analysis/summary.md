# Airspan Follow-Up: Strong Location

- Application matrix: `passed` (12/12 samples).
- Attempts: `6000`; accepted: `6000`; failed: `0`; success: `100.000%`.
- Signal class: `strong` (operator-reported pending MG52 radio-export validation).
- Physical placement: `mg52_on_trunk_floor_front_face_up_toward_sky`.
- Timing metric: request/response RTT; one-way fields are excluded because endpoint clocks were unsynchronized.
- Repository GNSS precision: `0.001` degree; exact coordinates and rosbags remain only in the excluded raw backup.
- Paper-facing status: pending ACP Cell 2 and MG52 radio-export alignment.

## Condition/Transport Summary

| condition | transport | attempts | accepted | success % | p50 ms | p95 ms | p99 ms | hit @100 ms % | hit @500 ms % |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| C1 | TCP | 1000 | 1000 | 100.000 | 117.863 | 137.309 | 145.868 | 4.600 | 100.000 |
| C1 | MQTT | 1000 | 1000 | 100.000 | 28.016 | 39.955 | 47.855 | 100.000 | 100.000 |
| C1 | UDP | 1000 | 1000 | 100.000 | 26.632 | 39.2 | 43.923 | 100.000 | 100.000 |
| C2 | TCP | 1000 | 1000 | 100.000 | 75.388 | 93.605 | 107.415 | 97.800 | 100.000 |
| C2 | MQTT | 1000 | 1000 | 100.000 | 49.753 | 59.721 | 67.663 | 99.700 | 100.000 |
| C2 | UDP | 1000 | 1000 | 100.000 | 47.947 | 56.947 | 63.586 | 100.000 | 100.000 |

## Evidence Qualifications

- Automatic GNSS produced in-window positions for 12/12 valid samples.
- Repository-facing coordinates are rounded to 0.001 degree, and serialized rosbags are retained only in the excluded raw backup.
- Deadline rates use every attempt as the denominator. Timeouts and other unaccepted rows are deadline misses.
- ACP Cell 2 and MG52 exports are still required before signal-related interpretation or paper-facing use.
