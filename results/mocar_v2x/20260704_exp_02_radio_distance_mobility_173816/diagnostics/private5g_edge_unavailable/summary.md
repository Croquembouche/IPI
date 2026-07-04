# Private-5G Edge Unavailable Diagnostic

- Run ID: `edge4av-mobility-20260704T173816`
- Result scope: `exp_02_only`
- Intended private-5G payload before reclassification: `1024 B`
- Intended private-5G transports before reclassification: `tcp udp mqtt`
- Result: private-5G latency collection did not start and is not counted as exp 04. The expected edge host `10.100.100.6` was unreachable, and no experiment receiver ports were open on discovered 10.x hosts.
- Edge reachability log: `edge_reachability.csv`
- GNSS samples: `99516`
- GNSS duration s: `452.326`
- Mean GNSS latitude: `39.66566976385578`
- Mean GNSS longitude: `-75.75716311059921`
- Latitude range: `39.66378404` to `39.667027222`
- Longitude range: `-75.759350628` to `-75.755501494`
- Median speed m/s: `7.131`
- p95 speed m/s: `12.336`
- Max speed m/s: `13.090`
- ROS bag size bytes: `58105856`

## Edge Reachability

| host | samples | ping ok | ssh open | mqtt open | tcp rx open | udp rx open |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 10.100.100.1 | 13 | 13 | 0 | 0 | 0 | 0 |
| 10.100.100.2 | 13 | 13 | 13 | 0 | 0 | 0 |
| 10.100.100.5 | 13 | 13 | 13 | 0 | 0 | 0 |
| 10.100.100.6 | 13 | 0 | 0 | 0 | 0 | 0 |
