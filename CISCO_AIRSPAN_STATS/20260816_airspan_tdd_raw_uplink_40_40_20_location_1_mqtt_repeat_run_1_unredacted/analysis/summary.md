# 2026-08-16 MQTT Uplink Repeat Summary

All four retained MQTT conditions passed: 4,000 attempts, 4,000 validated
application acknowledgments, 4,000 matching edge rows, and zero failures. The
user stopped the 2 MiB condition after 133 validated exchanges; it is preserved
but excluded. Future sweeps end at 1,024 KiB (1 MiB).

| Payload | p50 ms | p95 ms | p99 ms | Max ms |
|---:|---:|---:|---:|---:|
| 1 KiB | 39.435 | 51.016 | 279.632 | 299.649 |
| 10 KiB | 63.682 | 169.247 | 263.223 | 539.404 |
| 100 KiB | 439.006 | 746.947 | 927.222 | 34,967.951 |
| 1,024 KiB | 4,549.326 | 5,608.034 | 6,193.954 | 6,700.886 |

Compared with the valid 2026-08-15 `40/40/20` MQTT conditions, today's p50
RTT was 2.36% higher at 1 KiB, 10.04% higher at 10 KiB, 102.71% higher at
100 KiB, and 256.86% higher at 1 MiB. The p95 increases were 9.22%, 60.52%,
111.27%, and 163.86%, respectively. The difference grows strongly with payload
size in these two sequential day blocks.

This is a repeat comparison, not a causal TDD result. Both profiles and radio
values are operator-reported; no timestamp-aligned ACP or MG52 export is
stored, and day/time/load may differ. The 34.968-second accepted RTT at 100 KiB
sequence 19 is retained as an unfiltered outlier.
