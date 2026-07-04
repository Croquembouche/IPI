# Mocar V2X Setup Test - 2026-07-03

## Scope

- OBU management address: `192.168.253.10`
- RSU jump host: `128.4.178.240`
- RSU management address from jump host: `192.168.253.40`
- Remote experiment root: `/root/edge4av_exp`
- Experiment folders requested: `test`, `exp_01`, `exp_02`, `exp_03`

## OBU Result

- SSH to OBU succeeded.
- Created `/root/edge4av_exp/test`, `/root/edge4av_exp/exp_01`,
  `/root/edge4av_exp/exp_02`, and `/root/edge4av_exp/exp_03`.
- Deployed compatible SDK sample:
  `/root/edge4av_exp/bin/custom_sample`.
- Deployed rebuilt RTT binary:
  `/root/edge4av_exp/bin/ipi_custom_rtt`.
- Deployed RTT sweep helper:
  `/root/edge4av_exp/bin/run_payload_sweep.sh`.
- Deployed libraries:
  `/root/edge4av_exp/lib/libmocarcv2x.so` and
  `/root/edge4av_exp/lib/libzlog.so`.
- Custom sample smoke test command ran for 2 seconds and exited by timeout as
  expected.
- OBU smoke stderr recorded 20 lines of `txmsg-CUS: send msg successed!`.
- `ipi_custom_rtt --help` started successfully on the OBU with status `0`.

Local mirror:

- `obu_test/obu_custom_sample_smoke.status`
- `obu_test/obu_custom_sample_smoke.stdout`
- `obu_test/obu_custom_sample_smoke.stderr`
- `obu_test/obu_ipi_custom_rtt_help.status`
- `obu_test/obu_ipi_custom_rtt_help.stdout`
- `obu_test/obu_ipi_custom_rtt_help.stderr`

## RSU Result

- SSH to the jump host `128.4.178.240` succeeded.
- SSH from the jump host to the RSU at `192.168.253.40` succeeded.
- Created `/root/edge4av_exp/test`, `/root/edge4av_exp/exp_01`,
  `/root/edge4av_exp/exp_02`, and `/root/edge4av_exp/exp_03`.
- Deployed compatible SDK sample:
  `/root/edge4av_exp/bin/custom_sample`.
- Deployed rebuilt RTT binary:
  `/root/edge4av_exp/bin/ipi_custom_rtt`.
- Deployed RTT sweep helper:
  `/root/edge4av_exp/bin/run_payload_sweep.sh`.
- Deployed libraries:
  `/root/edge4av_exp/lib/libmocarcv2x.so` and
  `/root/edge4av_exp/lib/libzlog.so`.
- Custom sample smoke test command ran for 2 seconds and exited by timeout as
  expected.
- RSU smoke stderr recorded 20 lines of `txmsg-CUS: send msg successed!`.
- `ipi_custom_rtt --help` started successfully on the RSU with status `0`.

Local mirror:

- `rsu_test/rsu_custom_sample_smoke.status`
- `rsu_test/rsu_custom_sample_smoke.stdout`
- `rsu_test/rsu_custom_sample_smoke.stderr`
- `rsu_test/rsu_ipi_custom_rtt_help.status`
- `rsu_test/rsu_ipi_custom_rtt_help.stdout`
- `rsu_test/rsu_ipi_custom_rtt_help.stderr`

## OBU-RSU Link Tests

- Custom payload simultaneous test:
  - OBU and RSU each transmitted successfully for 12 seconds.
  - OBU stderr recorded 120 `txmsg-CUS: send msg successed!` lines.
  - RSU stderr recorded 120 `txmsg-CUS: send msg successed!` lines.
  - No `rxmsg-CUS` callback lines were recorded on either side.
- SPaT simultaneous test:
  - OBU and RSU each transmitted successfully for 12 seconds.
  - OBU stderr recorded 12 `txmsg-SPAT: send msg successed!` lines.
  - RSU stderr recorded 12 `txmsg-SPAT: send msg successed!` lines.
  - No `rxmsg-SPAT` callback lines were recorded on either side.
- IPI SPaT bridge test:
  - Rebuilt `ipi_spat_bridge` in an Ubuntu 18.04 Docker container so it only
    requires `GLIBC_2.17` and `GLIBCXX_3.4.21`.
  - Deployed `ipi_spat_bridge` to the RSU and ran it on TCP port `35555`.
  - Sent 10 IPI-encoded SPaT frames through `example_spat_tcp_sender` via an
    SSH tunnel through the jump host.
  - RSU bridge accepted and forwarded 10/10 SPaT frames using
    `mde_v2x_spat_send`.
  - A direct OBU SPaT receive logger recorded 10/10 `rxmsg-SPAT` callbacks.
  - This path is standard SPaT, not the Mocar custom-message API.
- BSM simultaneous test:
  - Bidirectional OBU-RSU V2X reception was verified.
  - OBU recorded 3 received BSM messages from RSU vehicle ID `40`.
  - RSU recorded 4 received BSM messages from OBU vehicle ID `10`.
  - RSU also recorded other ambient BSM messages from vehicle ID `2170064722`.
- RTT custom-message test:
  - Initial runs with the SDK public `mde_v2x_custom_send()` symbol sent no
    effective custom payloads, even though the function returned success.
  - After patching `ipi_custom_rtt` to use
    `v2x_packet_data_send(payload, len, 0x1b)`, bidirectional RTT works.
  - RSU initiator to OBU responder: 10/10 replies, average RTT `97.262 ms`,
    min `71.196 ms`, max `102.850 ms`.
  - OBU initiator to RSU responder: 10/10 replies, average RTT `100.712 ms`,
    min `84.300 ms`, max `114.288 ms`.

Local mirror:

- `obu_test/obu_rsu_link_test_stdbuf.*`
- `rsu_test/rsu_obu_link_test_stdbuf.*`
- `obu_test/obu_rsu_spat_test.*`
- `rsu_test/rsu_obu_spat_test.*`
- `local_spat_bridge_sender_rxlogger/spat_sender_*.csv`
- `obu_test/obu_ipi_spat_bridge_rx_logger.*`
- `rsu_test/rsu_ipi_spat_bridge_run2.*`
- `obu_test/obu_rsu_bsm_test.*`
- `rsu_test/rsu_obu_bsm_test.*`
- `obu_test/obu_ipi_custom_rtt_initiator.*`
- `obu_test/obu_ipi_custom_rtt_initiator_asn1.*`
- `obu_test/obu_ipi_custom_rtt_responder_rev.*`
- `rsu_test/rsu_ipi_custom_rtt_responder.*`
- `rsu_test/rsu_ipi_custom_rtt_responder_asn1.*`
- `rsu_test/rsu_ipi_custom_rtt_initiator_rev.*`
- `custom_probe_direct_send_20260703_151224/`
- `custom_rtt_direct_send_20260703_151348/`

## RSU Historical Latency Logs

- Found historical RSU timing traces in `/huali/log`.
- Mirrored locally under `rsu_huali_log_latency/`.
- Copied `latency.txt`, `tail_tx_success.sh`, and 28
  `rsu_latency_send_*.txt` scenario files.
- The scenario names cover 1 m obstacle/no-obstacle cases and 50 m, 100 m,
  and 200 m no-obstacle cases at 10 ms, 20 ms, 50 ms, and 100 ms send periods.
- The files contain `sequence_number timestamp` records from
  `tail_tx_success.sh`. That script records the transmit-start timestamp when
  `/huali/log/cv2x_stack.log` later reports `Tx success`; it does not by itself
  compute one-way latency or RTT.
- Mirrored trace line count: 131789 total lines across `latency.txt` and the
  28 scenario files.

## Custom RX Follow-Up

- Added and built `ipi_custom_probe`, a finite custom TX/RX probe that uses the
  installed Mocar SDK custom APIs and flushes callback logs immediately.
- RSU lower stack receives custom-service traffic:
  `Receive message[PSID:32] len=40`.
- Disassembly showed the installed SDK's `mde_v2x_custom_send()` symbol is a
  no-op on this build, so `ipi_custom_probe` and `ipi_custom_rtt` now send via
  `v2x_packet_data_send(payload, len, 0x1b)`.
- Patched custom probe validation succeeded in both directions:
  20/20 `rxmsg-CUS` callbacks on OBU from RSU and 20/20 `rxmsg-CUS`
  callbacks on RSU from OBU.
- `strace` shows standalone SDK test binaries and `cv2x_app` use the same fixed
  Unix datagram socket paths under `/tmp`, so they cannot both own the Mocar
  app RX socket cleanly.
- Final process state after testing: OBU `cv2x_app` and `cv2x_stack` running;
  RSU `cv2x_stack` running with no `cv2x_app`; both wildcard subscriptions
  still `dest_port=9000`.
- Detailed notes: `custom_rx_debug.md`.

## Tooling Notes

- `third_party/mocar/ssh_scripts/build_ipi_custom_rtt_docker.sh` rebuilds
  `ipi_custom_rtt` in a disposable Ubuntu 18.04 Docker container and verifies
  that the resulting binary does not require glibc newer than 2.27.
- `third_party/mocar/ssh_scripts/build_ipi_spat_bridge_docker.sh` rebuilds
  `ipi_spat_bridge` in a disposable Ubuntu 18.04 Docker container and verifies
  target-compatible glibc/libstdc++ symbol versions.
- `third_party/mocar/J2735-2020/samples/ipi_spat_rx_logger/` contains a small
  OBU-side SPaT receive logger that prints callbacks directly to stdout.
- `third_party/mocar/ssh_scripts/connectOBU_10.sh` connects to the OBU.
- `third_party/mocar/ssh_scripts/connectRSU_40.sh` connects to the RSU through
  the jump host and supports interactive login when the jump host is reachable.
- `third_party/mocar/ssh_scripts/setup_mocar_v2x_test.sh` creates the remote
  directory layout, deploys `custom_sample`, `ipi_custom_rtt`, the RTT sweep
  helper, and records smoke-test logs.
- The working RSU login used the same root credential as the OBU; the earlier
  RSU credential was rejected.
- The rebuilt `ipi_custom_rtt` binary now only requires `GLIBC_2.17` symbols
  and has runpath `$ORIGIN/../lib:$ORIGIN/../../lib:/usr/lib:/usr/local/lib`.
