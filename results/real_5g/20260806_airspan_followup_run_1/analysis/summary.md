# Airspan Follow-Up: Medium/Typical Location

- Application matrix: `passed` (24/24 samples).
- Attempts: `309000`; accepted: `307662`; failed: `1338`; success: `99.567%`.
- Corrected signal class: `medium_typical_deployment`.
- Physical placement: `mg52_on_trunk_floor_front_face_up_toward_sky`.
- Timing metric: request/response RTT; one-way fields are excluded because endpoint clocks were unsynchronized.
- Repository GNSS precision: `0.001` degree; exact coordinates and rosbags remain only in the excluded raw backup.
- Paper-facing status: pending ACP Cell 2 and MG52 radio-export alignment.

## Condition/Transport Summary

| condition | transport | attempts | accepted | success % | p50 ms | p95 ms | p99 ms | hit @100 ms % | hit @500 ms % |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| C1 | TCP | 1000 | 1000 | 100.000 | 124.844 | 150.114 | 160.265 | 0.800 | 100.000 |
| C1 | MQTT | 1000 | 1000 | 100.000 | 30.606 | 41.994 | 48.248 | 99.800 | 100.000 |
| C1 | UDP | 1000 | 1000 | 100.000 | 27.524 | 39.752 | 44.79 | 100.000 | 100.000 |
| C2 | TCP | 1000 | 1000 | 100.000 | 80.166 | 100.889 | 114.052 | 91.400 | 100.000 |
| C2 | MQTT | 1000 | 1000 | 100.000 | 51.783 | 63.062 | 69.857 | 99.800 | 100.000 |
| C2 | UDP | 1000 | 1000 | 100.000 | 49.808 | 59.398 | 64.308 | 100.000 | 100.000 |
| C3 | TCP | 1000 | 1000 | 100.000 | 330.89 | 399.674 | 425.808 | 0.000 | 99.000 |
| C3 | MQTT | 1000 | 1000 | 100.000 | 139.678 | 169.639 | 461.654 | 6.300 | 99.600 |
| C3 | UDP | 1000 | 995 | 99.500 | 139.711 | 171.522 | 181.726 | 5.000 | 99.500 |
| C4 | TCP | 100000 | 100000 | 100.000 | 250.041 | 355.865 | 607.557 | 0.004 | 97.644 |
| C4 | MQTT | 100000 | 100000 | 100.000 | 158.693 | 473.816 | 553.73 | 6.819 | 95.585 |
| C4 | UDP | 100000 | 98667 | 98.667 | 103.712 | 170.385 | 179.781 | 47.102 | 98.667 |

## C4 Fairness

| repetition | transport | accepted/attempts | success % | Jain accepted-count fairness | client success min-max % |
| ---: | --- | ---: | ---: | ---: | ---: |
| 1 | TCP | 50000/50000 | 100.000 | 1.000000 | 100.000-100.000 |
| 1 | MQTT | 50000/50000 | 100.000 | 1.000000 | 100.000-100.000 |
| 1 | UDP | 49386/50000 | 98.772 | 0.999980 | 97.800-99.600 |
| 2 | TCP | 50000/50000 | 100.000 | 1.000000 | 100.000-100.000 |
| 2 | MQTT | 50000/50000 | 100.000 | 1.000000 | 100.000-100.000 |
| 2 | UDP | 49281/50000 | 98.562 | 0.999978 | 97.400-99.800 |

## Evidence Qualifications

- Raw per-transport manifests retain the original planned `stronger/outside` fields. `metadata_corrections.json` supersedes those fields with the operator-confirmed medium/typical classification and trunk-floor, face-up placement.
- Automatic GNSS recovered at 10:39:02 EDT. Earlier samples rely on the operator-reported fixed coordinate; later samples include ROS-derived positions.
- Repository-facing coordinates are rounded to 0.001 degree, and serialized rosbags are retained only in the excluded raw backup.
- Deadline rates use every attempt as the denominator. Timeouts and other unaccepted rows are deadline misses.
- Three incomplete or transition attempts remain preserved but are excluded from the valid-repeat summaries.
- ACP Cell 2 and MG52 exports are still required before signal-related interpretation or paper-facing use.
