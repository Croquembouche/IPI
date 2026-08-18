# Overrun Diagnostic — Not the Primary Condition

The TCP 500-KiB sender was originally launched with a 1,000-exchange target.
During acquisition, the operator changed both uplink 500-KiB targets to 250.
The stop completed after 260 successful exchanges because ten additional
transfers finished between the last status observation and process
interruption.

This directory preserves the untouched 260-row vehicle sender and d1 receiver
artifacts. It must not be summarized as the declared condition. The primary
`payload_512000/` condition contains exactly sequences 1 through 250; those
250 exchanges are the only rows included in its validation summary.
