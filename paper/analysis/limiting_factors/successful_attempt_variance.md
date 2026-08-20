# Successful IPI Attempt RTT Variance

## Result

The analysis includes 1,765,930 finite RTT records from successful IPI attempts in 452 experimental conditions. Each condition has at least 20 successful responses and receives equal weight.

Because RTT spans multiple orders of magnitude, the primary variance decomposition uses log10 RTT. Differences between condition means explain 91.2% of the successful-attempt variance, while attempt-to-attempt variation within a fixed condition explains 8.8%. The successful-attempt latency regime is therefore set mainly by the experimental condition rather than by ordinary jitter within one condition.

Across conditions, the median coefficient of variation is 0.282, and the median p95/p50 ratio is 1.419. The 95th-percentile condition has a p95/p50 ratio of 5.539, and the maximum is 12.029, so a successful response can still have a highly variable completion time.

The median p95/p50 ratio is 1.446 across 427 private-5G conditions and 1.089 across 25 direct-PC5 conditions. This comparison is conditional on success: PC5 conditions with fewer than 20 replies, including complete and near-complete outages, are not part of the variance calculation.

## Method boundary

The result does not replace accepted delivery or deadline availability. It answers a separate question: when an IPI attempt succeeds, how predictable is its RTT? Raw millisecond-squared variance is retained in the CSV and JSON, but log10 RTT is used for the primary decomposition because a small number of seconds-scale tails otherwise dominate the scale.
