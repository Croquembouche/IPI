# Experiment Results

This directory stores Edge4AV/IPI local-loopback and real private-5G experiment
artifacts, including command logs, sender/receiver CSVs, GPS captures, ROS bag
metadata, run summaries, environment notes, and helper scripts used during the
runs.

GitHub rejects normal Git blobs larger than 100 MB. The following original
artifacts are kept in the local workspace but stored in the repository as
compressed `.gz` files:

- `results/real_5g/20260515_sunny_run_1/gps_restarted2/gps/rosbag/rosbag_0.db3`
- `results/real_5g/20260515_sunny_run_1/gps_restarted2/gps/gps_samples.csv`

Restore them with:

```bash
gunzip -k results/real_5g/20260515_sunny_run_1/gps_restarted2/gps/rosbag/rosbag_0.db3.gz
gunzip -k results/real_5g/20260515_sunny_run_1/gps_restarted2/gps/gps_samples.csv.gz
```
