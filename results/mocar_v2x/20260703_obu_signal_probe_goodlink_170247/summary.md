# OBU signal metric probe

This probe tested whether the OBU can expose C-V2X received signal strength during successful RSU-to-OBU traffic.

Traffic check:

| mode | RTT rows | successes |
|---|---:|---:|
| raw | 30 | 30 |
| timestamp/frame | 30 | 30 |
| totals | 30 | 30 |
| SCH filter | 30 | 30 |

Signal capture check:

| capture | output |
|---|---:|
| `diag_rssi_raw.log` | 0 bytes |
| `diag_rssi_tf.log` | 0 bytes |
| `diag_rssi_total.log` | 0 bytes |
| `diag_rssi_sch.log` | 0 bytes |

Conclusion: the installed OBU image does not expose usable C-V2X RSSI/SNR/RSRP/RSRQ through `diag-rssi`, `cv2x-config`, or the public Mocar SDK callback. The available indicators are C-V2X RX/TX active status, pool active status, message counters in OBU logs, and application-level packet success/RTT/loss. The `RTW: rssi` entries in syslog are from the Realtek Wi-Fi driver and should not be used as C-V2X signal strength.
