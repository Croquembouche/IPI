# Signal Strength Map Summary

Generated from `results/real_5g/Signal.ods`.

Assumptions:

- ID `0` is the base station and is plotted as a marker, but it is not used as a measured signal sample because its RSRP, RSRQ, and SNR cells are empty.
- SNR ranges are represented by their midpoint before interpolation.
- SNR is a dimensionless ratio; the survey reports its logarithmic value in dB.
- Interpolation uses Delaunay triangulation with barycentric linear interpolation on a `100 x 100` grid.
- Interpolated cells outside the triangulated measurement area are masked.
- OpenStreetMap Standard tiles are cached locally and embedded in the OSM overlay SVGs at zoom `18`.
- OpenStreetMap data attribution: `(C) OpenStreetMap contributors`.

Generated files:

- `results/real_5g/signal_measurements_parsed.csv`
- `results/real_5g/run_locations_parsed.csv`
- `results/real_5g/signal_map_rsrp.svg`
- `results/real_5g/signal_map_rsrq.svg`
- `results/real_5g/signal_map_snr.svg`
- `results/real_5g/signal_strength_maps.svg`
- `results/real_5g/signal_osm_rsrp.svg`
- `results/real_5g/signal_osm_runs_rsrp.svg`
- `results/real_5g/signal_osm_rsrq.svg`
- `results/real_5g/signal_osm_runs_rsrq.svg`
- `results/real_5g/signal_osm_snr.svg`
- `results/real_5g/signal_osm_runs_snr.svg`
- `results/real_5g/signal_osm_overlay_maps.svg`
- `results/real_5g/signal_osm_runs_overlay_maps.svg`

Measurement summary:

| Metric | Samples | Min | Mean | Max | Color scale |
|---|---:|---:|---:|---:|---|
| RSRP (dBm) | 16 | -121.00 | -106.88 | -91.00 | -125 to -85 dBm |
| RSRQ (dB) | 16 | -19.00 | -11.00 | -10.00 | -20 to -8 dB |
| SNR (dB) | 16 | 3.50 | 16.44 | 27.50 | 0 to 30 dB |

Base station: ID `0` at `39.66729000, -75.75751000`.

Signal-mapped run-location markers:

| Label | Latitude | Longitude | GNSS samples | Source |
|---|---:|---:|---:|---|
| Run 1 | 39.663793229 | -75.757002704 | 531 | `results/real_5g/20260513_sunny_fintechparking_run_1/gps/gps_samples.csv` |
| Run 2 | 39.666699491 | -75.757007496 | 1111 | `results/real_5g/20260514_sunny_after_rain_run_1/gps/gps_samples.csv` |
| Run 3 | 39.662797359 | -75.757436528 | 1266 | `results/real_5g/20260515_sunny_run_1/gps/gps_samples.csv` |
| Run 4 | 39.665710840 | -75.756101486 | 13100 | `results/real_5g/20260522_cloudy_run_1/gps_20260522/gps/gps_samples.csv` |
| Run 5 | 39.663549611 | -75.756997286 | 10117 | `results/real_5g/20260521_small_rain_run_1/gps_20260521/gps/gps_samples.csv` |
