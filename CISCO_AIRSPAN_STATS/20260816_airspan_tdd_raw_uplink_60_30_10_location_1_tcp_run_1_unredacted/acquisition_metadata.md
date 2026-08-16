# 2026-08-16 TCP Uplink Payload Acquisition

- Run ID retained from acquisition: `edge4av-real-20260816-airspan-tdd-raw-uplink-60-30-10-location-1-tcp-run-1`
- Corrected reported TDD profile: `60/20/20`
- Acquisition-time TDD entry: `60/30/10`, superseded by `tdd_profile_correction.json`
- Carried-forward RSRP: `-98 dBm`
- Carried-forward RSRQ: `-13 dB`
- Airspan Cell 1: locked/not broadcasting
- Airspan Cell 2: unlocked/broadcasting
- Serving context: MG52 selected Cell 2 with no reported handoff
- Transport: TCP exact raw application objects with compact application acknowledgments
- Payloads: 1 KiB, 10 KiB, 100 KiB, and 1,024 KiB
- Post-completion interval: 200 ms

The 1 KiB, 10 KiB, and 100 KiB conditions each completed 1,000/1,000
validated exchanges. The user stopped the 1,024 KiB condition after exactly
500 completed exchanges. Its 500 sender rows match 500 edge rows and pass
sequence, payload-length, and CRC32 correlation; it retains sender status 143
and no completion marker, so it is reported separately from the complete
three-condition matrix.

The run reused the validated same-day 59.570-second stationary GPS capture.
No new bag was requested, and physical-location continuity was not
independently reverified. Exact coordinates and the bag remain in excluded raw
evidence.

All 28 fetched edge files matched their remote copies before access revocation.
The temporary key with fingerprint
`SHA256:5WrjHKS3UZuVte0dZqCiVf6Yw4m7R8r4kPQRhYiCa84` was removed from the
edge, authentication with it then failed, and its local key material was
deleted. No experiment process or planned test port remained active.

Acquisition software:

- Repository commit: `2b44f352bb4579599dc51c4719314b66052e4f49`
- Raw probe SHA-256: `36c2cf9ed694175fa05ae7ed74eff3788ac5e8bd9fc9a6dd0a1efe862d0e84bf`
- Runner SHA-256: `afaceeee4d597fd8156f81a589bbece59b3c093139eb80195467b097badba8b0`
- Correction-aware analyzer SHA-256: `9f8f0f31a5b87115ac360991d74ec7ef00f0743a80980cea07b1221657b50536`

The corrected TDD, cell state, serving-cell context, RSRP, and RSRQ lack
timestamp-aligned ACP/MG52 exports. This block measures its own application-ACK
RTT distribution and does not by itself establish a causal TDD effect.
