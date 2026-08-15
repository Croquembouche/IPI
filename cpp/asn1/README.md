# IPI SAE J2735 regional extension

`IPI.asn` defines the IPI cooperative-service payload carried at the
`TestMessage00` regional-extension point. It uses `DSRCmsgID` 240 and the local
deployment `RegionId` 200. The latter is in J2735's uncoordinated range; every
deployment sharing a radio domain is responsible for preventing collisions.

The file is deliberately separate from SAE's licensed ASN modules. A production
schema bundle is prepared by applying the following object-set entry to
`Reg-TestMessage00` in the licensed `REGION` module:

```asn1
{ IPI.IpiCooperativeService IDENTIFIED BY IPI.ipiRegionId }
```

Do not add a new top-level `DSRCmsgID`. J2735 reserves `TestMessage00` through
`TestMessage15` for experimental regional applications. The C++ codec uses the
same `MessageFrame` and `TestMessage00` structure and is checked against
deterministic independent-tool vectors.

`test/DSRC.asn` only supplies the `RegionId` dependency for isolated schema and
vector tests. It must not be distributed or compiled as the production J2735
base module. Production compilation requires the separately licensed SAE
J2735ASN module set and a compiler that supports information object classes,
parameterized `RegionalExtension`, open types, and unaligned PER.
