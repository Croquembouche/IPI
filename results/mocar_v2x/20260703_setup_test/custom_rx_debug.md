# Custom RX Debug Notes

Date: 2026-07-03 local / 2026-07-04 device logs

## What was changed

- Added `third_party/mocar/J2735-2020/samples/ipi_custom_probe/`.
- Added `third_party/mocar/ssh_scripts/build_ipi_custom_probe_docker.sh`.
- Built the probe in the same Ubuntu 18.04 aarch64 Docker flow used for the
  other Mocar binaries.
- Rebuilt `ipi_custom_probe` and `ipi_custom_rtt` after identifying that the
  installed SDK's public `mde_v2x_custom_send()` symbol is a no-op on this
  build.
- Patched both tools to send custom traffic through the exported SDK packet
  sender: `v2x_packet_data_send(payload, len, 0x1b)`.
- Deployed the rebuilt binaries to both devices:
  `/root/edge4av_exp/bin/ipi_custom_probe` and
  `/root/edge4av_exp/bin/ipi_custom_rtt`.

## Confirmed

- RSU `cv2x_stack` receives PSID 32 frames:
  `Receive message[PSID:32] len=40`.
- Before the patch, RSU `cv2x_stack` repeatedly reported:
  `g_stack_udp_sockfd send error 111`, and the custom callback did not emit
  `rxmsg-CUS`.
- Disassembly of `libmocarcv2x.so` showed:
  - `mde_v2x_custom_send()` returns success without sending.
  - `mde_v2x_custom_recv_handle_register()` only stores the callback pointer.
  - The normal SDK RX thread already dispatches custom packets when the SDK
    packet type is `2`.
  - `v2x_packet_data_send(buffer, len, 0x1b)` builds a custom packet and sends
    it to `/tmp/unix_domain_cv2x_stack_rx`.
- After the patch, custom RX callbacks work in both directions.
- `strace` shows the Mocar SDK app path binds fixed Unix datagram sockets:
  `/tmp/unix_domain_cv2x_app_tx` and `/tmp/unix_domain_cv2x_app_rx`.
- `cv2x_app` also owns those same fixed app socket paths when it is running, so
  it conflicts with standalone SDK test binaries.
- Temporarily starting RSU `cv2x_app` did not fix custom callback delivery.
  The wildcard subscription remained `dest_port=9000`, while `cv2x_app` bound
  UDP `10011`.
- Attempting to change wildcard subscription to `10011` was rejected by the
  modem and the subscription stayed at `9000`.
- Final process state after validation:
  - OBU: `cv2x_app` and `cv2x_stack` running.
  - RSU: only `cv2x_stack` running.
  - Both devices: wildcard subscription still `dest_port=9000`.

## Local result folders

- `custom_probe_20260703_143512/`: OBU RX probe, RSU TX probe. TX succeeded;
  OBU did not emit `rxmsg-CUS`.
- `custom_probe_rsu_app_20260703_143937/`: RSU RX probe with temporary
  `cv2x_app`, OBU TX probe. TX succeeded; RSU did not emit `rxmsg-CUS`.
- `custom_raw_rx_rsu_20260703_144505/`: bounded RSU low-level RX capture and
  stack PSID 32 tail.
- `custom_probe_retry_20260703_150300/`: cleaned probe deployed to both
  devices, but still using the SDK no-op sender. TX logs showed success; no
  callback was emitted.
- `custom_probe_obu_no_app_20260703_150547/`: OBU `cv2x_app` stopped
  temporarily; RSU sent 20 custom messages through the SDK no-op sender; OBU
  still did not emit `rxmsg-CUS`.
- `custom_probe_direct_send_20260703_151224/`: patched direct-send probe.
  RSU-to-OBU recorded 20/20 `txmsg-CUS` and 20/20 `rxmsg-CUS`. OBU-to-RSU
  recorded 20/20 `txmsg-CUS` and 20/20 `rxmsg-CUS`.
- `custom_rtt_direct_send_20260703_151348/`: patched RTT tool. RSU initiator
  to OBU responder succeeded 10/10, RTT avg `97.262 ms` with min `71.196 ms`
  and max `102.850 ms`. OBU initiator to RSU responder succeeded 10/10, RTT
  avg `100.712 ms` with min `84.300 ms` and max `114.288 ms`.

## Current Status

- Custom RX is now working.
- The remaining caveat is operational: standalone SDK test binaries temporarily
  own the fixed SDK app socket paths. Restart `cv2x_app` after OBU-side SDK
  probe/RTT runs when returning the OBU to its normal application state.
