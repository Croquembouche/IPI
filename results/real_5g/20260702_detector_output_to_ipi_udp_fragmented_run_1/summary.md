# Experiment 09 UDP Fragmented Detector-Output Replay

## Run

- Run ID: `edge4av-real-20260702-detector-output-to-ipi-udp-fragmented-run-1`
- Date: 2026-07-02
- Edge server: `d1@10.100.100.6`
- Vehicle state: stationary
- Location class: current/good-signal location; weak-signal repeat still pending
- GNSS mean location: `39.663532736, -75.756985197`
- UDP mode: application-level fragmentation/reassembly, `--udp-max-datagram-bytes 1400`

## Why The UDP Code Was Changed

Raw UDP can carry large datagrams at the socket API, but detector-sized messages exceed the path MTU when sent as one UDP datagram. For example, `--payload-bytes 1400` encoded to a 1640-byte UDP datagram, which requires IP fragmentation on a 1500-byte MTU path. The private-5G path did not reliably deliver those IP fragments to the receiver application.

The UDP sender/receiver were updated to split one logical detector-output request across MTU-safe UDP datagrams and reassemble at the receiver. Local build and edge build both completed after the change. Validation before this full run:

- Local loopback 60 KB UDP payload: 3/3 accepted after application fragmentation.
- Edge 4096-byte payload: 5/5 accepted after application fragmentation.
- Edge 19648-byte payload: 5/5 accepted after application fragmentation.

The earlier raw-UDP diagnostic attempt is retained in `results/real_5g/20260702_detector_output_to_ipi_udp_run_1/ABORTED_DO_NOT_USE_AS_FINAL.md`.

## UDP Results

| Payload bytes | Attempts | Accepted | Success | p50 ms | p95 ms | p99 ms | Mean ms | Max ms | Receiver rows | Notes |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| 0 | 1000 | 997 | 99.70% | 25.859 | 51.661 | 64.888 | 28.643 | 159.781 | 1000 | Three sender-side ack losses/timeouts despite receiver rows. |
| 4096 | 1000 | 1000 | 100.00% | 42.987 | 66.246 | 93.827 | 45.520 | 162.908 | 1000 | Previously failed as raw UDP; fixed with application fragmentation. |
| 19648 | 1000 | 999 | 99.90% | 99.657 | 135.704 | 150.316 | 100.658 | 170.308 | 999 | Detector p50 payload size. |
| 22816 | 1000 | 997 | 99.70% | 109.767 | 147.557 | 161.252 | 111.260 | 199.878 | 998 | Detector p95 payload size. |
| 23968 | 1000 | 995 | 99.50% | 96.587 | 139.705 | 159.778 | 99.873 | 182.469 | 999 | Detector p99 payload size. |
| 60000 | 100 | 20 | 20.00% | 177.415 | 194.395 | 201.264 | 176.265 | 201.264 | 20 | Upper stress control; many fragments per logical message make message delivery fragile. |

## GNSS Verification

- ROS bag: `gps/rosbag`
- Bag duration: 2234.229 s
- Bag size: 273.7 MiB
- Bag messages: 603245
- CSV samples: 491165
- Best-position samples: 64477
- Best-position mean: `39.663532736, -75.756985197`
- Best-position radius: p95 `0.628 m`, max `0.720 m`
- Horizontal speed: mean `0.0022 m/s`, p95 `0.0044 m/s`, max `0.0142 m/s`

## Takeaways

- Experiment 09 now has current-location TCP, MQTT, and UDP detector-output replay data.
- UDP must be described as MTU-safe application-fragmented UDP, not a single raw UDP datagram per detector output.
- The main detector payload range, 4096-23968 bytes, is usable over UDP after fragmentation with 99.5-100.0% message success.
- The 60 KB upper control shows why very large UDP messages should not be treated like TCP/MQTT streams without reliability or retransmission.
- Weak-signal experiment 09 is still pending and should repeat TCP/MQTT/UDP at the weak-signal stationary location.
