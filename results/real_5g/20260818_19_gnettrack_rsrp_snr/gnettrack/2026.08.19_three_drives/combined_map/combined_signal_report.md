# Three-drive Cisco P5G signal map

- Plateau exclusion threshold: longer than 5 seconds
- Total GPS samples: 1301
- Excluded signal-loss plateau samples: 123
- Valid NR RSRP samples: 1178
- Valid NR SINR samples recovered from verbose logs: 1178
- Ordinary G-NetTrack SNR samples: 0
- Android active subscriptions: Cisco Private only (subscription 8, hardware slot 1)

## Runs

- Drive 1: 2026.08.19_12.06.05–2026.08.19_12.13.41; 860 samples, 123 excluded
- Drive 2: 2026.08.19_12.13.43–2026.08.19_12.13.46; 7 samples, 0 excluded
- Drive 3: 2026.08.19_12.14.54–2026.08.19_12.18.32; 434 samples, 0 excluded

Long frozen radio-timestamp runs are removed in full, and map lines are broken across those intervals. Repeated values lasting five seconds or less remain because normal Samsung radio callbacks update every one to three seconds.

G-NetTrack's displayed SIM1 is a frozen duplicate CellInfo record, not an enabled Android subscription. Dual-SIM handling must remain enabled so the current record is exposed as SIM2; only `*_SIM2_*` session folders are used here.

The ordinary SNR fields are empty. SINR is recovered from each verbose log's timestamped NR `ssSinr` field and aligned to the nearest GPS row.
