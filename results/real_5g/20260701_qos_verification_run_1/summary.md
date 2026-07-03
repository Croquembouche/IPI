# QoS / 5QI Verification Run 1

- Date/time: `2026-07-01`
- Vehicle private-5G interface: `eno2`
- Vehicle private-5G IP: `10.120.121.35`
- Edge application server: `10.100.100.6`
- Edge TCP latency port: `36666`
- Gateway / physically connected private-5G server candidate: `10.100.100.1`
- Method: local `tcpdump` on `eno2` during successful TCP latency probes.

## What Was Verified

Two one-probe TCP latency runs were captured:

| Condition | Sender CSV | Capture Decode | Result |
| --- | --- | --- | --- |
| `qos_profile=default` | `local/qos-verification-default-success-tcp_sender.csv` | `local/default_success_tcp_36666.decoded.txt` | Successful probe, all captured packets `tos 0x0` |
| `qos_profile=5qi-mapped` | `local/qos-verification-5qi-mapped-success-tcp_sender.csv` | `local/5qi_mapped_success_tcp_36666.decoded.txt` | Successful probe, all captured packets `tos 0x0` |

TOS counts:

```text
default tos counts
     12 tos 0x0

5qi_mapped tos counts
     12 tos 0x0
```

## Interpretation

- The application metadata field `qos_profile=5qi-mapped` does not produce a
  different IP TOS/DSCP marking on packets leaving the vehicle host.
- Therefore, application-side packet marking does not verify QoS/5QI
  enforcement.
- The edge server is an application endpoint physically connected to the
  private-5G server. It is not the authoritative place to verify 5QI bearer or
  QoS-flow counters.
- `10.100.100.1` responds as the network gateway and HTTP redirects to a
  Meraki local-device page. HTTP artifacts are saved under `local/`.

## What Still Needs Private-5G Server Evidence

To claim network-enforced QoS/5QI, collect one of the following from the
private-5G server/core while a short default and `5qi-mapped` run is active:

- PDU session QoS-flow table showing the UE and a non-default 5QI flow.
- Packet/byte counters increasing on that non-default 5QI flow.
- Policy/config rule mapping this UE/application traffic to the non-default
  flow, ideally matching UE IP `10.120.121.35`, edge IP `10.100.100.6`, and the
  relevant TCP ports.

Without that core-side evidence, the correct paper wording is:

> We collected a QoS-labeled software control (`qos_profile=5qi-mapped`), but
> packet capture showed no DSCP/TOS marking difference at the vehicle host; we
> therefore do not claim verified network-enforced 5QI behavior.
