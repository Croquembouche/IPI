# IPI PC5 exchange sample

This device sample binds the strict `IP5X` request/response envelope to the
Mocar custom-message channel (`0x1b`). The application value inside the
envelope is a complete UPER J2735 `MessageFrame`: reserved `TestMessage00`
(`DSRCmsgID` 240), local `RegionId` 200, and the typed IPI regional value from
`cpp/asn1/IPI.asn`. Both endpoints decode that frame. The sample also validates
CRC, request identity, session, sequence, expiration, duplicate responses,
packet size, and a local monotonic RTT deadline before accepting a result.

The default operational maximum is 2048 bytes for the complete application
packet. `--body-bytes` controls only the IPI offload-content field; metadata,
the J2735 frame, `IP5X` framing, and CRC must also fit. A deployment can
explicitly raise `--max-packet-bytes` up to the observed 4080-byte SDK
application ceiling, but that is not a portable PC5 guarantee and requires a
new hardware validation sweep.

Build on the host for the AArch64 Mocar target:

```bash
make -C third_party/mocar/J2735-2020/samples/ipi_pc5_exchange
```

Run the responder first, then the initiator:

```bash
./ipi_pc5_exchange --role responder --node-id node-b --peer-id node-a
./ipi_pc5_exchange --role initiator --node-id node-a --peer-id node-b \
  --count 100 --body-bytes 128 --timeout-ms 1000
```

The IPI payload has deterministic UPER vectors from two independent ASN.1
compilers, and its outer frame was checked with the USDOT J2735 202409 package.
The `IP5X` wrapper is still an IPI integration protocol transported through the
vendor custom-message path; it is not a standardized J2735 bearer. Exact
SEP2023 module compilation still requires the separately licensed SAE
`J2735ASN_202309` files, and hardware interoperability remains a deployment
check.
