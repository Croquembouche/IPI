# Remaining Experiment Plan

Last updated: 2026-08-04

## Purpose

This document defines the experiments and evidence-closure actions that remain
before the Edge4AV paper can answer the current coauthor comments. It is a
runbook and decision record. Use it to prepare each collection, name and
preserve the artifacts, determine whether a run is valid, and prevent
unnecessary repetition of experiments that are already complete.

## Vehicle Collection Start Order

Use this section at the vehicle. The detailed controls, artifacts, and pass
conditions remain in R1A through R9 below.

### 1. Pull And Validate The Experiment Host

Run these commands on the experiment computer in the vehicle, while it is
connected through the MG52 private-5G gateway:

```bash
git pull --ff-only origin main
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build -j2
ctest --test-dir cpp/build --output-on-failure
command -v sshpass rsync timeout
```

Confirm that the vehicle computer can reach the existing MX250-connected edge
host. Set `EDGE_HOST`, `EDGE_USER`, `EDGE_REPO`, and `SSHPASS` only in the local
shell environment. Do not write a password into a script, run manifest, or Git
file. Record the vehicle-host, edge-host, phone, and ACP times before collecting
data.

Historical scripts under `results/real_5g/` are implementation references.
Do not run them in their original directories: several scripts hardcode their
historical run ID or output path and could mix new data with an existing result.
Every new collection must use a new directory named
`results/real_5g/YYYYMMDD_airspan_followup_run_N/` and a matching unique
`run_id`. Before launching a reused script, verify its run directory, remote run
directory, condition labels, payload list, transport list, ports, and edge-host
settings.

### 2. Qualify The Phone Before Application Runs

Run R1A first. Collect the five-minute stationary phone log and verify that the
phone is attached to the Airspan private-5G NR cell and exports timestamps,
serving-cell identity, RSRP, and RSRQ. Stop if R1A fails. Do not substitute
screenshots or displayed signal bars for the timestamped export.

### 3. Validate ACP And Logger Alignment

Run R1 across three consecutive 15-minute ACP bins:

1. idle;
2. one controlled 25 Mbps vehicle-to-edge uplink stream; and
3. one controlled 25 Mbps edge-to-vehicle downlink stream.

Export ACP and the phone log immediately afterward. Stop before R2-R4 if the
traffic cannot be assigned to Airspan Cell 2 and the expected bins, the phone
log has gaps, or achieved traffic differs materially from the intended
direction or rate. Export both cells so that Cell 1's lack of experiment traffic
can be verified rather than assumed.

### 4. Run The Required Follow-Up Matrix

After R1 passes, collect C1-C6 in the two-repetition order in the R2-R4 section.
The minimum paper-facing matrix is 12 ACP bins: stronger/weaker signal at 1 KiB
and 23,968 B, stronger signal under 25 Mbps uplink load, and stronger signal
with 100 clients. Keep TCP, MQTT, and UDP separate. Target 1,000 probes per
transport and condition. Together with R1, the minimum collection occupies 15
ACP bins, or 3 hours 45 minutes, excluding setup and placement transitions.

Both cells will use `30/60/10`: 30 downlink, 60 uplink, and 10 dynamic frames.
Both cells remain administratively unlocked. Record the configuration and the
phone-observed serving cell in every new run manifest. Current observations
show that the vehicle UE is served by Airspan Cell 2 and that all experiment
traffic occurs on Cell 2. Airspan Cell 2 is the radio cell; it is unrelated to
experiment condition C2. The R1
idle/uplink/downlink bins and the first valid C1 and C2 TCP runs also satisfy the
traffic portion of the R9 diagnostic; do not repeat those probes solely for R9.
They remain follow-up measurements under `30/60/10`, not relabeled repetitions
of the historical `40/40/20` runs.

### 5. Do Not Run Blocked Or Unnecessary Work

- R5 remains blocked until Cisco provides and verifies a nondefault QoS flow.
- R6 is excluded from the current paper.
- R7 is collected opportunistically through the ACP exports accompanying R1-R4.
- R8 requires a response-payload-capable harness. Do not claim bidirectional
  large-payload results from the existing request-payload scripts.
- Do not repeat V2X, full payload sweeps, failure/restart, or every client-count
  experiment for this follow-up.

Complete the end-of-day checklist before leaving the vehicle. In particular,
export ACP before its short retention window expires, preserve unredacted files
only under `CISCO_AIRSPAN_STATS/`, and verify every sender and receiver file is
present and nonempty.

The goal is not to repeat the full private-5G campaign. The existing results
already cover payload size, TCP/MQTT/UDP, background load, detector-output
replay, application deadlines, multiclient scaling, weak signal, and
failure/restart behavior. The remaining measurement work is a targeted
follow-up that collects application results, Airspan cell statistics, and
Android radio measurements over the same time windows. Several comments do not
require a new experiment. They instead require verified deployment facts,
architecture clarification, IPI implementation evidence, careful claim scope,
and relevant citations. Those actions are included here so that completing the
experiment matrix alone is not mistaken for completing the paper.

## Controlling Evidence Rules

1. Keep every measurement attached to its actual collection date and time.
   Never treat the July 21-24 Airspan export as though it was collected during
   the May 13-July 6 application experiments.
2. Existing application measurements remain valid at the layer at which they
   were collected. Historical Airspan/RAN telemetry was not retained.
3. The new runs are follow-up measurements of representative conditions. They
   do not replace or relabel the original runs.
4. Airspan DU Cell rows are 15-minute, cell-level aggregates. They are not
   per-request measurements and currently contain no UE identifier, RSRP,
   RSRQ, SINR, CQI, MCS, BLER, HARQ/RLC retransmission, PRB utilization,
   scheduler state, queue state, QFI/5QI, or packet latency.
5. Android radio measurements from a phone beside the gateway are a co-located
   proxy. They are not measurements from the vehicle gateway/CPE, gNodeB, UPF,
   or the application packets.
6. Continue reporting private-5G request/response timing as RTT. Do not report
   one-way latency unless the relevant clocks are verified as synchronized.
7. Label signal conditions using measured signal strength. Outside/inside
   placement may be used to create a signal difference, but placement itself is
   not evidence of signal strength.
8. Do not change gNodeB transmit power or production radio configuration
   without written UD/Cisco/Airspan approval.
9. Do not enable Dynamic QoS experimentally. The supplied document states that
   activation cannot be disabled and a saved rule cannot be removed.
10. Do not use a phone for paper-facing measurements until its hardware,
    private-5G connection, app fields, sampling behavior, and raw export have
    passed R1A. A screen display or screenshot is not a time-aligned dataset.
11. Phone samples cannot be described as per-request radio telemetry when the
    phone samples more slowly than the application sender. Report them as
    time-aligned, co-located radio context at the measured sampling interval.
12. Do not run phone-based ping, upload, or download tests during the
    application measurements. Those tests create an additional workload and
    would confound the intended idle, load, and multiclient conditions.
13. Use one qualified phone and fixed orientation for all paired comparisons.
    Without calibration, do not equate its absolute RSRP/RSRQ with the
    gateway/CPE receiver's values or treat the coauthor's approximate 8 dB
    vehicle loss as a measured result.

## Data Location And Git Handling

- Keep confidential and unredacted Cisco/Airspan/G-NetTrack material under
  `CISCO_AIRSPAN_STATS/`.
- `CISCO_AIRSPAN_STATS/` is locally excluded through `.git/info/exclude`.
- Do not force-add that directory or upload its raw contents to GitHub.
- Raw exports may contain node names, internal addresses, device identifiers,
  subscriber identifiers, phone numbers, and local IP addresses.
- In G-NetTrack-style logs, treat `IMEI`, `IMSI`, `MSISDN`, `IP`, raw
  cell/network identifiers, and exact GNSS coordinates as sensitive.
- Store only redacted, experiment-aligned derived files under
  `results/real_5g/`.
- Export Airspan data after every collection day. The currently available
  history is approximately two days.

## Remaining Work Summary

| ID | Experiment or action | Priority | Current status | Required for |
|---|---|---:|---|---|
| R0 | Resolve Airspan counter and configuration questions | Required | Partially complete; active configuration and SAS provider retained | Correct interpretation of every new Airspan export |
| R1A | Qualify the Android phone and installed radio app | Required before R1 | Not run | Establish whether the phone can observe and export the private-5G NR fields |
| R1 | ACP and Android radio-logger validation pilot | Required | Not run | Validate cell mapping, timing, traffic attribution, and current phone fields |
| R2 | Stronger/weaker-signal payload follow-up | Required | Not run | Relate signal and payload size to application and cell-level behavior |
| R3 | Background-load follow-up | Required | Not run | Relate application tail growth to contemporaneous cell traffic |
| R4 | 100-client follow-up | Required | Not run | Add cell context to the representative scalability endpoint |
| R5 | Verified default-versus-new QoS comparison | Required only if Cisco enables it | Blocked on Cisco configuration | Determine whether network-enforced QoS changes results |
| R6 | Path-segment measurements | Optional | Excluded from the current paper scope | Future delay decomposition; the current paper reports complete-path application RTT |
| R7 | Additional ACP/per-UE counter export | Recommended | Availability unknown | Improve radio/RAN interpretation beyond DU Cell aggregates |
| R8 | Validate payload direction and measure selected downlink responses | Required for bidirectional payload claims | Not run | Distinguish large vehicle-to-edge requests from large edge-to-vehicle responses |
| R9 | Compare the historical `40/40/20` results with the `30/60/10` follow-up | Exploratory sensitivity study | Both-cell configuration plan established; follow-up not run | Quantify whether and by how much the changed TDD allocation affects application outcomes, then derive implications for future vehicular-radio and 6G design |
| P1 | Complete deployment and path documentation | Required | Not complete | Vendor attribution, reproducibility, equipment scope, and V2X/5G separation |
| P2 | Validate and document the IPI contribution | Required | Partly implemented; final validation not recorded | Answer the request to strengthen IPI without changing the paper's central logic |
| P3 | Close manuscript claims, citations, and submission checks | Required | Not complete | Qualified conclusions, related work, anonymity, format, and evidence traceability |
| O1 | Establish reliable ACP access and retention | Recommended | Current ACP is externally hosted and may go down | Prevent loss of the short-retention Airspan evidence |

No new V2X experiment is required solely for these private-5G follow-ups.

## Coauthor Comment Closure Matrix

| Comment | Required response or evidence | Closure item |
|---|---|---|
| Cisco supplied the edge/core equipment and Airspan supplied the radio | Verify exact models and use supplier names only in the first deployment description; use `private 5G` or `5G network` afterward | R0, P1 |
| State the band, CBSD category, spectrum authorization, and radio configuration | Obtain written configuration values and a redacted configuration record | R0, P1 |
| Identify the CPE, antenna placement, and receiver sensitivity | The CPE is an internal-antenna `MG52-HW`; all original runs used the trunk-floor, front-face-up placement, and modem-level n48 sensitivity is documented. Record both controlled positions for R2 | R0, R2, P1 |
| Clarify whether the data represent a commercial cell edge | Report measured RSRP/RSRQ and avoid `cell edge` unless a deployment-specific threshold is supplied | R1A, R1-R4, P3 |
| Clarify whether V2X is implemented using 5G | State that the measured LTE C-V2X PC5 OBU-RSU path and private-5G NR Uu vehicle-edge path are independent; NR-V2X sidelink was not measured | P1 |
| Strengthen IPI | Tie the API, state transitions, required/optional fields, request/response example, failure semantics, and enabled analysis to the current implementation and tests | P2 |
| Qualify conclusions to the equipment and explain what failed | Separate observed endpoint behavior from unverified radio causes; scope conclusions to the measured deployment and identify improvements as requirements unless a follow-up validates a mechanism | R1-R5, R7, P3 |
| Consider the BREAKING-LOW/DRIVE-SAFE work | Read and cite relevant technical publications; use the project page only as motivation, not as experimental evidence | P3 |
| Use G-NetTrack Pro for continuous logging and exports | First qualify the installed Lite build and phone. Use Pro if Lite cannot export a usable raw, timestamped log | R1A, R1 |
| Change signal strength by reducing gNodeB power | Do not perform this intervention because UD prohibits power adjustment; use a passive, fixed-location inside/outside comparison | R2 |
| Compare gateway/antenna placement inside and outside the vehicle | Run only the representative paired conditions, with fixed location/orientation and measured signal values | R2 |
| Use other campus vehicles | Not required for the current paper. Add only if the research question changes to vehicle-body generalization | No current experiment |
| Ping from Airspan toward the MX250-connected laptop | Optional and outside the current paper scope; do not call the result over-the-air latency unless the compared paths isolate that segment | R6 |
| Configure a nondefault 5QI | Use a second Cisco-provisioned DNN with core-side proof; do not infer 5QI from IP TOS or an application label | R5 |
| Collect RAN load counters | Use available ACP/per-UE exports. Record scheduler state and queue occupancy as unavailable unless Airspan provides another supported interface | R1, R7 |
| Move ACP to stable UD VMware infrastructure | Coordinate installation or, until then, export after every collection day before the approximately two-day history expires | O1 |
| Establish that 5G can carry large CAV responses, not only requests | The current probe sends the selected payload toward the edge and returns a compact acknowledgment. Run the targeted reverse-direction experiment or narrow every payload claim to the measured direction | R8, P3 |

## R0. Questions To Resolve Before Interpreting Airspan Data

Ask Cisco/Airspan for written answers or documentation for the following:

### Deployment Values Reported From ACP

The following values were transcribed by the user from ACP on 2026-07-24:

| Field | Reported value |
|---|---|
| gNodeB model | Airspan AirSpeed 2900 |
| gNodeB description | `GNB Outdoor 3.55-3.7GHz n48` |
| Board number | `xpu_2200` |
| Platform version | `22.0-24-0.0` |
| Application version | `22.00-53-0.0` |

These values answer the Airspan hardware and software portion of Question 11.
Retain an ACP inventory/software screenshot or export under
`CISCO_AIRSPAN_STATS/` as supporting evidence before using the values in the
manuscript.

Three configuration files retained on 2026-07-31 add the following evidence:

- `Airspan_NetworkConfigExport_20260731_1234.xml` is a gNodeB configuration
  export captured during reconfiguration. It records GNSS as the clock source,
  a 7-second UE inactivity timer, enabled 15-minute statistics collection,
  Cell 1 at `30/60/10` with `10D4G`, and Cell 2 at `40/40/20` with `10D4G`.
  This intermediate state is neither the original experiment configuration nor
  the planned follow-up configuration. The export
  also records one maximum uplink layer, two maximum downlink layers, and
  downlink 256-QAM enabled for each cell. These are configuration values, not
  measurements of the rank or modulation used by an experiment. The export
  confirms the TDD labels. The user identifies the components, in order, as
  downlink, uplink, and dynamic frame allocations. The original profile has 40
  downlink, 40 uplink, and 20 dynamic frames; the follow-up has 30 downlink, 60
  uplink, and 10 dynamic frames.
- `CBRSConfiguration_20260731_1230.csv` identifies Federated Wireless as the
  enabled primary SAS connection, CBRS configuration version 1.2, and no
  enabled backup domain proxy. It does not contain the individual SAS grant
  records.
- `All_Config.xml` is a generic ACP profile library. It does not establish
  which profile is assigned to the deployed gNodeB and must not be used as an
  active-configuration source.

Two ACP screenshots added on 2026-07-31 provide the following additional
configuration evidence:

- The earlier gNodeB network-properties screenshot displays `RLC Acknowledge
  Mode Support` as disabled, DSCP 48 for NG-C and Xn-C, DSCP 16 for management,
  and NG-C SCTP heartbeat/retry settings. The active configuration export and
  the user's live check establish that the current capability setting is
  enabled. These are configuration values, not application user-plane latency
  measurements or proof of the measured bearer's RLC mode.
- The 5QI-properties view shows DSCP disabled and
  `RLC-UMBidirectional` selected for each displayed 5QI profile from 1 through
  9. The page documents available gNodeB-side profiles, not which QFI/5QI the
  Cisco core assigned to an experiment PDU session.
- In ACP, open the deployed gNodeB configuration and the `gNodeB Network
  Properties` view, then locate `RLC Acknowledge Mode Support`. The active XML
  export stores the same field at
  `/root/GnbConfigs/GnbDetailsGet/GnbProperties/NetworkConfig/RlcAcknowledgeModeSupport`
  and reports `Enabled`, which the user confirms matches the live setting. The
  earlier disabled screenshot is not the current value. Capability support
  does not establish the RLC mode used by a bearer. The `5QI Properties`
  view's `RLC Mode` row currently shows
  `RLC-UMBidirectional` for profiles 1 through 9.
- The user confirms that the deployment had no other users and no interference
  from other bands during the experiments. External-user traffic and inter-band
  interference are therefore not candidate explanations for the measured
  results.

Online primary sources resolve terminology but not deployment-specific fields.
3GPP TS 38.213 defines an NR TDD pattern using periodicity, full downlink and
uplink slots, partial downlink and uplink symbols, and remaining flexible
symbols. No public primary Airspan source was found that maps `40/40/20` or
`30/60/10` to those fields. The network administrator identifies `10D4G` as a
fixed frame-packing setting used for coexistence with LTE systems. It is
unchanged between the original and follow-up configurations, so retain it in
the configuration record without treating it as an experimental variable or a
cause of measured performance. 3GPP TS 28.552 defines an active UE as
one with DRB data available for transmission and defines the standard average
UE-throughput metric for bursts spanning multiple slots. Small one-slot bursts
may instead be counted as unrestricted volume. These definitions explain why
an attached UE sending short periodic probes need not produce a nonzero mean
active-UE average or a representative DRB-throughput value. Airspan-specific
scaling, rounding, and counter implementation still require confirmation.

1. Airspan's SR22.00 release notes require the ACP server to use UTC. Verify
   whether this deployment's CSV export timestamps are UTC or are rendered in
   the logged-in user's local timezone.
2. What is the unit of `Cell Unavailability Time Fault`, `Cell Unavailability
   Time Manual`, and `Cell Unavailability Time (Total)`?
3. What caused the simultaneous cell-unavailability values around July 23,
   20:45-21:15 in the current export?
4. How do `Cell 1` and `Cell 2` map to the physical radio, sector, PCI,
   NR-ARFCN, and antenna configuration?
5. Does ACP implement `Mean Number of Active UEs` according to 3GPP TS 28.552,
   what sampling period and scale does it use, and does the CSV round values
   below one active UE to zero?
6. Does the ACP `DRB UE Throughput` column implement `DRB.UEThpUl` and
   `DRB.UEThpDl` from 3GPP TS 28.552, and does ACP expose the corresponding
   unrestricted-volume counters for short one-slot bursts?
7. Can ACP export intervals shorter than 15 minutes?
8. Can ACP export per-UE records for the gateway/CPE?
9. Which ACP report, if any, provides RSRP, RSRQ, SINR, CQI, MCS, BLER,
   HARQ/RLC retransmissions, PRB utilization, or QoS-flow information?
10. Which scheduler and queue counters are unavailable, and which related load
    counters are available?
11. The Airspan gNodeB is an AirSpeed 2900 and the vehicle CPE is a Cisco
    Meraki `MG52-HW`. The current MG52 dashboard reports MG firmware `4.1.2`,
    modem firmware `M0R.115005`, host version `A0R.501056`, carrier PRI
    version `P0R.000566`, carrier profile `Generic GCF`, and a `1000 Mbit/s`
    full-duplex Ethernet link. The user confirms that these values match the
    original experiment campaigns. Exact Cisco-core inventory and path
    decomposition are outside the current paper scope.
12. Header-bearing ACP exports confirm band n48 and 40 MHz bandwidth. Cell 1
    reports NR-ARFCN 637992 (3569.880 MHz reference frequency), and Cell 2
    reports NR-ARFCN 645334 (3680.010 MHz reference frequency). Retain the SAS
    grant records if exact authorized channel edges are needed.
13. Header-bearing ACP exports confirm CBSD Category B, 17 dBi antenna gain,
    33 dBm cell transmit power, 34 dBm/MHz cell EIRP, SAS-sourced CPI data, and
    `0/4` PAL/GAA 10 MHz assignments for each cell. The CBRS configuration
    export identifies Federated Wireless as the enabled primary SAS provider.
    Individual grant records and installation azimuth/downtilt remain
    unresolved.
14. The signed structural analysis records an outdoor AirSpeed 2900 at a 20 ft
    antenna centerline on a 21 ft tripod/mount structure. Verify that the field
    installation matches that design, then record the compass azimuth,
    mechanical/electrical downtilt, and sector/cell mapping; those values are
    not present in the structural analysis. Also reconcile the structural
    report's height above roof level with the coverage model's `20 ft AGL`
    assumption and record the actual installed height above ground.
15. The CPE is an internal-antenna Meraki `MG52-HW` using the Telit FN990A40
    modem. The modem guide gives typical conducted n48 sensitivity at 30 kHz
    SCS and 40 MHz bandwidth as -91, -91, -93, and -92 dBm at the four antenna
    ports and -97 dBm combined. These are modem-level laboratory values, not a
    whole-gateway over-the-air sensitivity measurement. All original runs used
    the same placement: flat on the bottom of the trunk with the MG52 front
    face oriented upward. Record the placement used by each new condition.
    Sources:
    <https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/Overviews_and_Datasheets/MG52%252F%252F52E_Technical_Specifications>,
    <https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/MG_Antenna_Datasheets/MG52_Internal_Antenna_Datasheet>, and
    <https://fccid.io/RI7FN990A40/User-Manual/User-Manual-7361571.pdf>.
16. The application edge computer is directly connected to the MX250, so the
    measured path contains no external WAN or cloud segment after the local 5G
    system. Exact mobile-core placement and internal path decomposition are
    outside the current paper scope. Direct Ethernet attachment does not
    establish zero gNodeB-to-edge latency; all local processing and forwarding
    costs remain inside the reported application RTT.
17. The user confirms that the experiment sessions used DNN `cisco5g` and were
    assigned default 5QI 9. The numeric QFI was not provided. This confirmation
    applies to the default session and does not prove that the attempted
    `5qi-mapped` application condition received a nondefault 5QI.
18. What RSRP, RSRQ, or SINR range does the operator use to define the usable
    coverage boundary for this deployment, if any?
19. Can the ACP instance be moved to UD-managed VMware, and what retention,
    backup, and export schedule will apply there?

Unresolved counters must be excluded from causal interpretation. They may still
be archived as raw observations.

### Meraki Dashboard Evidence Collection

The Cisco Networking Learning Hub is a training catalog. It does not expose the
deployment configuration. Use the Meraki Dashboard itself to document the
MX250 and the enterprise-side portion of the measured path. Use read-only views
and do not save or apply configuration changes.

| Evidence needed | Meraki Dashboard path | Record or export | Limitation |
|---|---|---|---|
| MX250 identity | `Organization > Configure > Inventory` | Filter for the MX250, enable all relevant columns, and download the inventory CSV. Record model and assigned network; retain serial and MAC only in the private evidence directory. | Inventory describes Meraki-managed devices, not the Airspan radio or Cisco mobile core. |
| MX250 status and interfaces | `Security & SD-WAN > Monitor > Appliance status` | Capture model, configured firmware, configuration status, WAN assignment method, and the active-port diagram. Redact addresses, Dynamic DNS names, serials, and exact location before manuscript use. | This page describes the MX appliance and its uplinks; it does not identify the NR radio bearer or 5QI. |
| Installed MX firmware | `Organization > Monitor > Firmware upgrades > All networks` | Record the current firmware version for the network. Do not schedule an upgrade while collecting evidence. | The organization page reports the configured Meraki firmware, not Cisco edge-core software. |
| VLAN and static-route configuration | `Security & SD-WAN > Configure > Addressing & VLANs` | Record routed or passthrough mode, the VLAN used by the edge endpoint, relevant subnets, and relevant static routes. View only; do not edit or save. | These are enterprise-side network settings. |
| Active route state | `Security & SD-WAN > Monitor > Route table` | Record the status, VLAN, route type, destination role, and next-hop role for routes used by the experiment. Keep raw addresses private. | The route table cannot identify the internal RAN/core segment that produced delay. |
| Edge endpoint and other visible clients | `Network-wide > Monitor > Clients` | Find the MX250-connected experiment laptop and any visible Cisco edge component. Record hostname or assigned role, VLAN, connection, and sent/received usage. Keep IP and MAC addresses private. | Dashboard client classification can be inferred and stale; verify device roles from labels or administrator records. |
| Layer-2 topology | `Network-wide > Monitor > Topology` | Save a screenshot and note every visible MX, switch, edge host, and discovered LLDP/CDP neighbor. Verify each link physically or with an administrator. | Non-Meraki devices or links without LLDP/CDP may be missing; the diagram is not a complete private-5G topology. |
| MX events and configuration history | `Network-wide > Monitor > Event log` and `Organization > Monitor > Change log` | Filter to the experiment window and retain MX connectivity, reboot, route, and configuration-change events. | These logs do not record Airspan radio events or Cisco core session policy. |
| Traffic visible at the MX250 | `Network-wide > Monitor > Packet Capture` | Select `for security appliances`, the MX250, and the relevant LAN or routed interface. During a controlled request or ping, download a `.pcap` and record the capture start/end time and timezone. | An MX capture can establish packet presence, direction, IP DSCP/TOS, and timing at that interface. It cannot by itself isolate over-the-air delay or prove QFI/5QI assignment. MX captures also omit ordinary LAN-to-LAN traffic that the appliance only switches locally. |

The exact gateway/CPE is now confirmed as a Meraki `MG52-HW`. Open
`Cellular Gateway > Monitor > Cellular gateways`, select that device, and save
the `Uplink` and `Performance` views. Record its active SIM slot, RAT, serving
band, provider, MCC/MNC, APN, data-session MTU, and time-ranged RSRP and RSRQ.
Keep the data-session address and all device/subscriber identifiers private.
The dashboard already resolves the model and current firmware fields. The
original installation is also established: the gateway remained flat on the
trunk floor with its front face upward throughout the original campaigns.

If the optional nondefault-QoS experiment proceeds, obtain the subscriber DNN,
active QFI/5QI, SMF/DNN QoS profile, flow match, and enforcement counters from
Cisco Control Center, the Cisco Private 5G Management Platform, a redacted
mobile-core export, or written Cisco confirmation. Exact core inventory and
path decomposition are not required for the current paper. Obtain Airspan TDD,
scheduler, RAN counters, SAS grants, and per-UE radio telemetry from ACP or
Airspan rather than the Meraki Dashboard.

Store the inventory CSV, redacted screenshots, event records, and packet
captures under `CISCO_AIRSPAN_STATS/`. Do not upload raw dashboard exports or
screenshots containing serial numbers, MAC/IP addresses, subscriber data, or
exact site information to GitHub.

## R1A. Android Phone And App Qualification

Complete this qualification before scheduling R1. An old Android phone is
usable only if its modem supports the deployed private-5G network and the
installed app exports the required fields. The phone's age or the presence of a
5G icon is not sufficient evidence.

The current official G-NetTrack Lite listing identifies package
`com.gyokovsolutions.gnettracklite`. It states that 5G reporting can include
RSRP, RSRQ, and PCI, while SNR, CQI, and timing advance are listed only for 4G.
The same listing describes text/KML recording and active ping/upload/download
tests as Pro features, while also mentioning a Lite log mode. Because this
description is not sufficient to establish the installed build's export
behavior, inspect an actual export. If the installed app has a different package
name, record its exact name and do not assume it has G-NetTrack capabilities.

Official references:

- `https://play.google.com/store/apps/details?id=com.gyokovsolutions.gnettracklite`
- `https://www.gyokovsolutions.com/manuals/G-NetTrack_Pro-manual.pdf`

The attached `G-NetTrack_Pro_Logs 1.21.zip` contains historical 2018 AT&T LTE
examples. It is useful for understanding the tab-delimited schema, file marks,
neighbor-cell fields, and sensitive identifiers, but it does not validate 5G
NR reporting on the user's current phone.

### Phone And Network Eligibility

Record these facts once in the raw collection notes:

1. Phone manufacturer and model.
2. Android version, security-patch level, baseband version, and modem/chipset
   when it can be identified.
3. App name, package name, version, and whether it is Lite or Pro.
4. SIM profile used for the private network and the configured APN/DNN, with
   subscriber identifiers redacted from repository artifacts.
5. Manufacturer or modem documentation showing support for band n48 and 5G
   standalone operation, if the deployment uses standalone NR.
6. The network type reported by Android and the app while connected at the
   test site.
7. Serving PLMN, cell/NCI, PCI, TAC, band, and NR-ARFCN, then confirmation that
   these values map to the Airspan deployment rather than a public carrier.

The phone fails qualification if it cannot attach to the private-5G network, is
camped only on a public carrier or LTE anchor that does not represent the
measured private-5G NR cell, or cannot export timestamped records.

### Fields To Collect

The raw phone log should contain, when the phone and Android API expose them:

- Timestamp with documented timezone and subsecond precision if available.
- Latitude, longitude, GNSS accuracy, speed, and bearing.
- Radio-access technology and mode, including NR SA/NSA status if reported.
- Serving PLMN, cell/NCI, PCI, TAC, band, NR-ARFCN, and frequency.
- 5G RSRP and RSRQ.
- 5G SINR only if the app explicitly reports a valid NR SINR field; do not
  relabel an LTE SNR field as 5G SINR.
- Serving-cell changes and neighbor-cell identifiers/levels.
- Downlink and uplink system bitrate only if exported by the app. Because the
  phone is a separate UE, these values describe phone traffic, not gateway/CPE
  or cell throughput; use them to detect unintended phone traffic rather than
  to measure the application workload.
- App log state, configured sampling trigger, and location-source/accuracy
  fields.

CQI, MCS, BLER, HARQ/RLC retransmissions, PRB use, QFI, and 5QI are desirable
from Airspan or core exports but are not required phone fields. Do not fill
these columns from assumptions or from LTE-only displays.

### Logger Configuration

1. Grant precise-location, phone-state, and file/media permissions required by
   the app, and enable Android location services.
2. Disable battery optimization for the logger and keep the screen in the mode
   required by the app to prevent suspended collection.
3. Use time-based logging for stationary experiments. Select the shortest
   stable interval exposed by the app, preferably one second, and record both
   the configured interval and achieved timestamp spacing.
4. Disable distance-triggered logging for stationary runs when the app permits
   it. A stationary phone must still produce periodic records.
5. Turn off Wi-Fi and confirm that the displayed serving cell is the private-5G
   cell. Do not run maps, ads, speed tests, or other phone traffic during the
   application measurements.
6. Place the phone beside the gateway/CPE antenna in a marked position and
   orientation. Record whether both are inside or outside the vehicle.
7. Keep the phone connected for all compared conditions and record it as an
   additional UE on the cell.

### Qualification Capture

1. Collect at least five minutes of stationary idle data.
2. Record the phone, car experiment host, and ACP displayed times at the start
   and end.
3. Add an in-app file mark if supported; otherwise record exact log start/end
   times in the run manifest.
4. Export every raw format the app supports, including timestamped text/CSV and
   KML where available.
5. Inspect the raw headers and at least 20 consecutive records. Record which
   target fields are populated, their units, timestamp resolution, sampling
   interval, repeated/stale-value behavior, and missing-value representation.
6. Confirm that records continue while the app is in its intended background
   state and that there is no unexplained multi-minute gap.
7. Move between two passive signal conditions for a short diagnostic check and
   confirm that RSRP/RSRQ update rather than remaining frozen.
8. Redact subscriber, device, phone-number, local-address, and precise-location
   fields before placing a derivative under `results/`.

R1A passes only if the phone attaches to the correct private-5G NR cell and
produces a raw, timestamped export with stable periodic samples, cell identity,
RSRP, and RSRQ. If Lite cannot do this, install G-NetTrack Pro or another
validated logger and repeat R1A. Screenshots and manually transcribed readings
do not pass.

## Common Collection Protocol

Apply this protocol to R1-R6 unless an experiment below overrides it.

### Required Execution Hosts

- Run R1-R5 application senders and load clients on the experiment computer in
  the car while it is connected through the Cisco private-5G gateway.
- `d1` is not connected through the private-5G gateway and must not be used as
  the vehicle-side sender for private-5G application measurements.
- `d1` may be used as a wired receiver or path-comparison endpoint only if its
  physical connection to the MX250/Airspan-side network is confirmed and
  documented.
- Preserve the existing edge-side receivers when reproducing an existing
  result condition. Changing the receiver or route creates a different path and
  must be recorded as a new condition.
- Remote execution on the car requires an authenticated SSH or reverse-SSH
  connection to the car's experiment computer, not merely a tunnel to `d1`.
- The qualified Android phone is a separate UE and radio proxy. It must not
  replace the car experiment computer as the application sender.

### Time Alignment

1. Confirm the ACP timezone before the first run.
2. Record the displayed time on the application host, Android phone, and ACP at
   the beginning and end of the collection.
3. Record clock offsets rather than assuming synchronization.
4. Align each condition to one ACP 15-minute bin.
5. For a bin from `HH:00` to `HH:15`, begin the workload near `HH:01` and finish
   by approximately `HH:13`. Leave time for receiver startup, transport
   transition, and log flushing without crossing the bin boundary.
6. Record exact start and end timestamps for every transport and background
   traffic process.

### Network And Physical Controls

1. Keep other test UEs disconnected where possible.
2. Record every UE that remains connected.
3. Keep the gateway, antenna, phone, vehicle, and edge host unchanged within a
   paired comparison except for the intended intervention.
4. Keep the vehicle location and orientation fixed for the stronger/weaker
   signal comparison.
5. Keep the phone at a marked position beside the gateway/CPE antenna and
   record its orientation.
6. Record whether the gateway/CPE, antenna, and phone are inside or outside the
   vehicle.
7. Record weather and any temporary obstruction, but do not use weather as a
   causal explanation without evidence.
8. Do not call a condition "cell edge" unless Cisco/Airspan provides a
   deployment-specific definition and the measurements satisfy it.
9. Keep phone data tests, speed tests, automatic uploads/downloads, and
   nonessential apps disabled. The phone should record radio context without
   creating the experimental load.

### Application Parameters

Reuse the parameters and working scripts from the existing result family being
replicated:

- Compact application payload: sender parameter `1024` B.
- Representative detector-output payload: sender parameter `23968` B.
- Background load: existing controlled `25 Mbps` uplink condition.
- Client-scale endpoint: existing `100-client`, `1024` B condition.
- Transports: TCP, MQTT, and UDP.
- Target: 1000 probes per transport/condition when the existing script uses
  that count.
- Preserve the existing send interval, timeout, message type, and transport
  implementation unless a documented defect requires a change.

The application sender parameter and serialized frame size are different
quantities. Preserve both values in the resulting logs and paper tables.

For multiclient collection, reuse the working commands that produced
`results/real_5g/20260702_multiclient_scalability_run_4/` and
`results/real_5g/20260706_multiclient_scalability_weak_signal_run_1/`. Do not
substitute a generic starter that lacks the existing MQTT multiclient behavior.

### Host And Network-Stack Telemetry

Collect lightweight telemetry on both the car experiment computer and edge
host during R1-R5 and R8:

- CPU utilization per core and for the sender/receiver/broker/load processes.
- Memory use, swap activity, and process restarts.
- Network-interface bytes, packets, drops, errors, and overruns before and after
  each bin.
- TCP retransmission/socket statistics for TCP and MQTT conditions when
  available.
- Same-host receiver processing time from request receipt to response send.
- System clock status and any NTP/PTP/chrony offset report.

Use a sampling interval that does not materially load the hosts; one-second
host samples are preferred. Host telemetry is required for the 100-client,
bulk-payload, and loaded conditions because endpoint saturation can otherwise
be mistaken for a radio or core effect.

### Per-Bin Collection

For each non-multiclient condition:

1. Start the receiver and verify readiness before the ACP bin begins.
2. Start the R1A-qualified Android radio logger and mark the condition.
3. Start any required background load.
4. Run TCP, MQTT, and UDP sequentially within the same ACP bin when they fit.
5. Keep separate sender and receiver files for every transport.
6. Stop background load after the final application transport.
7. Record any timeout, restart, manual correction, or unexpected traffic.
8. Keep the Android radio logger running through the end of the bin.

If all three transports cannot finish in one bin, assign a separate bin to each
transport. Do not allow a transport to cross an ACP boundary without recording
that fact. Counterbalance transport order across repetitions so that one
transport is not always measured first or last. With three repetitions, rotate
the starting transport; with two, reverse the order.

### Required Artifacts

For every condition and repetition, preserve:

- Application sender CSV for each transport.
- Application receiver CSV for each transport.
- Load-generator client/server CSV when background load is used.
- Car-host and edge-host CPU, memory, process, clock, and interface telemetry.
- Exact condition start/end times.
- Airspan DU Cell export covering at least one bin before, every experiment
  bin, and one bin after the collection.
- Raw timestamped Android radio-log export for the same window and KML when
  supported.
- A redacted Airspan derivative with node/internal identifiers removed.
- A redacted Android radio-log derivative with device, subscriber,
  phone-number, local-IP, and precise-location fields removed or anonymized.
- Condition metadata: location identifier, vehicle orientation, gateway and
  antenna placement, measured signal statistics, protocol order, payload,
  client count, offered load, DNN, 5QI/QFI when applicable, phone/app versions,
  configured and achieved phone sampling interval, and operator notes.

Recommended result layout:

```text
results/real_5g/YYYYMMDD_airspan_followup_run_N/
  run_manifest.json
  application/
  load/
  host_telemetry/
  airspan_redacted.csv
  gnet_redacted.tsv
  validation_summary.json
  notes.txt
```

Keep unredacted originals in `CISCO_AIRSPAN_STATS/`, not in this result folder.

## R1. ACP And Android Radio-Logger Validation Pilot

### Purpose

Validate the measurement path before collecting paper-facing follow-up data.
This pilot determines whether known traffic appears in the correct Airspan cell
and time bin and whether the R1A-qualified phone provides usable, time-aligned
private-5G NR context throughout the workload. The expected serving cell is
Airspan Cell 2. Export both cells and verify that Cell 1 does not carry the
experiment traffic.

### Duration

Three consecutive ACP bins, approximately 45 minutes.

### Conditions

| Bin | Condition | Traffic |
|---|---|---|
| V1 | Idle | No intentional application or load-generator traffic |
| V2 | Controlled uplink | One known-rate uplink stream using the existing load generator |
| V3 | Controlled downlink | One known-rate downlink stream using the existing load generator |

Use a rate that the existing path can sustain without saturating it. The
existing `25 Mbps` rate is preferred if it remains stable. Record the actual
achieved rate.

### Validation Checks

The pilot passes only if all of the following are true:

1. The application-host and ACP time windows can be aligned unambiguously.
2. The serving Airspan cell is identified.
3. The idle bin is distinguishable from the controlled-traffic bins.
4. Uplink traffic increases the expected uplink DRB volume/time counters.
5. Downlink traffic increases the expected downlink DRB volume/time counters.
6. The observed Airspan volume is consistent with the offered rate and
   duration after accounting for protocol overhead.
7. Cell 2 carries the experiment traffic; any Cell 1 traffic is explained.
8. The phone export contains the private-5G serving-cell identity, RSRP, RSRQ,
   usable timestamps, and records throughout all three bins.
9. The phone's configured and achieved sampling intervals, missing-value
   behavior, and stale-value behavior are documented.
10. The zero active-UE counter and cell-unavailability units are either
    explained or explicitly excluded.
11. Phone-generated ping/upload/download traffic is absent from all three bins.
12. The phone is reported as co-located run-level radio context rather than
    gateway or per-request telemetry.

Do not proceed to paper-facing R2-R5 interpretation if traffic cannot be mapped
to the correct ACP bin and cell.

## R2-R4. Targeted Follow-Up Matrix

### Required Conditions

| Condition | Signal condition | Network load | Application payload | Client count | Transports |
|---|---|---|---:|---:|---|
| C1 | Stronger measured signal | Idle | 1024 B | 1 | TCP, MQTT, UDP |
| C2 | Stronger measured signal | Idle | 23968 B | 1 | TCP, MQTT, UDP |
| C3 | Stronger measured signal | 25 Mbps uplink | 1024 B | 1 | TCP, MQTT, UDP |
| C4 | Stronger measured signal | Existing test background only | 1024 B | 100 | TCP, MQTT, UDP |
| C5 | Weaker measured signal | Idle | 1024 B | 1 | TCP, MQTT, UDP |
| C6 | Weaker measured signal | Idle | 23968 B | 1 | TCP, MQTT, UDP |

### Repetitions

- One ACP bin per condition is sufficient only for diagnosis.
- Two bins per condition are the minimum synchronized follow-up dataset.
- Three bins per condition are preferred for paper-facing comparisons.
- Keep the original per-condition target of 1000 application probes per
  transport where execution time permits.

With two bins per condition:

- Six conditions require 12 bins, or 3 hours.
- The R1 validation pilot adds 45 minutes.
- Total minimum collection time is approximately 3 hours 45 minutes, excluding
  setup and transitions.

With three bins per condition, total time including the pilot is approximately
5 hours 15 minutes.

### Signal Pair

Create the stronger/weaker signal pair at one fixed vehicle location and
orientation. The planned intervention is:

- Stronger condition: gateway/CPE antenna in the marked outside position.
- Weaker condition: gateway/CPE antenna in the marked inside position.

The MG52 uses integrated internal antennas, so mark the complete gateway
position and front-face orientation for both conditions and record that moving
the gateway is the required intervention. Keep its cables, power, computer,
software, vehicle location, and vehicle orientation unchanged. Keep the phone
in the corresponding marked position beside the gateway. Do not use a
different vehicle for this pair.

Use the original trunk-floor, front-face-up placement as the inside-vehicle
baseline unless the paired-run protocol explicitly selects another documented
inside position.

Collect a short idle baseline after each placement change. The comparison is
valid only if RSRP/RSRQ show a repeatable separation between placements. If the
radio distributions overlap substantially, do not label the runs as
stronger/weaker signal; adjust the passive obstruction method or location
without changing gNodeB power.

Use an interleaved order when practical. For two repetitions of a paired
condition, prefer stronger-weaker-weaker-stronger rather than collecting all
stronger runs on one day and all weaker runs on another day.

### Suggested Two-Repetition Order

| Bin group | Condition |
|---|---|
| 1-3 | R1 validation: idle, uplink, downlink |
| 4 | C1 stronger, 1024 B |
| 5 | C5 weaker, 1024 B |
| 6 | C5 weaker, 1024 B repeat |
| 7 | C1 stronger, 1024 B repeat |
| 8 | C2 stronger, 23968 B |
| 9 | C6 weaker, 23968 B |
| 10 | C6 weaker, 23968 B repeat |
| 11 | C2 stronger, 23968 B repeat |
| 12-13 | C3 stronger, loaded, two repetitions |
| 14-15 | C4 stronger, 100 clients, two repetitions |

This order requires 15 ACP bins, or 3 hours 45 minutes. Add setup and placement
transition time outside the assigned bins.

### Per-Condition Evaluation

For every transport and repetition, compute:

- Attempts, successes, failures, and success rate.
- RTT p50, p95, and p99.
- Deadline-hit rates for the deadlines already used in the paper analysis.
- Achieved load and load-generator loss/errors where applicable.
- For 100 clients: per-client success, p50/p95/p99, aggregate success, and
  fairness.
- Co-located phone RSRP/RSRQ statistics and serving-cell identity over the
  application interval, with NR SINR included only if R1A validates that field.
- Airspan uplink/downlink DRB volume, active time, derived throughput, RACH
  counters, and availability for both cells. Use Cell 2 for the application
  comparison after confirming the phone serving-cell record and the absence of
  experiment traffic on Cell 1.

Do not correlate a request with an individual Airspan row. The valid unit is an
application condition within a 15-minute cell-level bin.

### Acceptance Criteria

1. At least two valid bins exist for every required condition.
2. Every application record maps to one declared ACP bin.
3. Every bin maps to Airspan Cell 2; any serving-cell change or Cell 1 traffic
   is identified and reported.
4. TCP, MQTT, and UDP retain separate application results.
5. The stronger/weaker labels are supported by measured radio values.
6. The load condition reaches and records its intended offered rate.
7. The 100-client condition starts the declared number of clients and reports
   every failed or missing client.
8. No unexplained restart, outage, cross-bin workload, or additional UE affects
   the bin.
9. New application results are compared with the corresponding existing result
   family to determine whether the representative behavior reproduced.
10. Any failure to reproduce is reported as a result, not silently discarded.
11. Host telemetry shows whether sender, receiver, broker, or interface
    saturation occurred; unexplained host saturation invalidates a network-only
    interpretation.

## R5. Verified Network-Enforced QoS Experiment

### Status

Blocked until Cisco provisions and verifies a second DNN/QoS profile.

The existing QoS-label experiment is a negative verification result: both
vehicle-interface captures showed IP TOS `0x0`, and no core-side QFI/5QI
evidence was stored. It does not establish network-enforced QoS treatment.

### Preconditions

Do not run this experiment until all items are complete:

1. Cisco identifies the exact second DNN.
2. Cisco specifies the supported default-session and dedicated-flow 5QI.
3. Cisco states whether the flow is GBR or non-GBR.
4. Cisco supplies uplink/downlink GBR and maximum-bit-rate values when needed.
5. Cisco supplies the exact protocol/endpoint/port flow filter.
6. Airspan confirms radio support and available resources.
7. Cisco provides a maintenance window and recovery procedure.
8. Cisco confirms how the applied DNN, QFI, 5QI, and counters will be captured.
9. The configuration discrepancy in the supplied document is resolved: one
   section lists default-session 5QI 5-9, while the recommended example uses
   5QI 69.

### Conditions

Use a 1024 B application payload and the same signal condition, placement,
transport order, host, and background load for all comparisons:

| Condition | DNN/QoS treatment | Background load |
|---|---|---|
| Q1 | Existing default DNN/profile | Idle |
| Q2 | Approved new DNN/profile | Idle |
| Q3 | Existing default DNN/profile | 25 Mbps uplink |
| Q4 | Approved new DNN/profile | 25 Mbps uplink |

Run TCP, MQTT, and UDP. Use at least two valid bins per condition; three are
preferred. Interleave default and new treatment where the operational
configuration permits it.

### Required Evidence

- Application sender/receiver logs.
- Airspan export for every bin.
- Core-side proof of the DNN, QoS flow, QFI, and applied 5QI.
- Configuration snapshot identifying the tested policy without credentials or
  internal addresses in the repository.
- Packet capture where allowed, recognizing that IP TOS alone is not proof of
  5QI treatment.
- Admission failure or inability to apply the policy must be preserved as a
  negative result.

### Interpretation Rule

Claim a QoS effect only if the network treatment is independently verified and
the application comparison holds signal, load, payload, and transport
conditions constant. If network treatment cannot be verified, describe the run
as an attempted QoS experiment, not a QoS comparison.

## R6. Optional Path-Segment Measurements

### Status

This experiment is excluded from the current paper scope. The edge
computer-to-MX250 connection is confirmed, and the paper will report
application RTT over the complete vehicle-to-local-edge path without assigning
delay to the radio, core, or wired segments. The procedure below is retained
only for a future decomposition study.

### Purpose

Measure RTT over identifiable path segments to bound where delay occurs. This
experiment must not claim exact over-the-air latency unless Cisco/Airspan
confirms that the measured paths are nested and comparable.

### Topology Information Required If Reopened

- The application edge computer is directly connected to the MX250; retain the
  relevant MX interface and route as local evidence.
- Gateway/CPE interface and route through the 5G system to the application
  edge host.
- gNodeB/Airspan interface available for testing.
- `d1` interface and route, if `d1` is the MX250-connected laptop.
- UPF and core placement.
- Whether each probe traverses the radio, transport network, core, and edge
  service.
- Firewall, NAT, tunneling, and traffic-shaping behavior on each path.

Direct attachment to the MX250 means that the path has no external WAN or
cloud segment after the local 5G system. It does not establish zero delay
between the gNodeB and edge computer. RAN, core/UPF, MX250, Ethernet, and edge
host processing remain part of the measured application RTT until separately
bounded.

### Measurements

1. ICMP RTT and application RTT from the car computer through the
   gateway/CPE to the same edge-side endpoint.
2. Same-clock receiver processing time between request receipt and response
   transmission on the edge host, obtained from application timestamps or an
   allowed edge-host packet capture.
3. Edge-host loopback application RTT to bound responder and host-stack cost.
4. Ping RTT from the Airspan-accessible interface to `d1` only after confirming
   that `d1` is the MX250-connected laptop and documenting the interface and
   route.
5. Wired RTT between `d1`, the MX250-side endpoint, and the edge host when
   those routes are supported and documented.
6. Additional CPE, UPF, or core-boundary RTTs only where endpoints and routes
   are documented.
7. Repeat the measurements that include the radio link under the stronger and
   weaker measured signal conditions.

Use the same probe count and interval where possible. RTT measurements do not
require cross-host clock subtraction. Receiver processing time is valid because
both timestamps come from one host; it must not be subtracted from an unrelated
path measurement to create an unsupported radio-delay estimate.

### Interpretation Rule

Do not subtract two RTTs and call the difference radio latency unless the paths
are proven to differ only by the radio segment. Otherwise, report each path RTT
separately and explain its endpoints. A roughly 20 ms Airspan-originated ping is
not, by itself, proof that the application RTT is almost entirely over the air.

## R7. Additional Airspan/Per-UE Export

This is a data-availability task rather than a standalone application
experiment.

Request an actual sample export before promising any of these fields:

- Per-UE serving cell and UE identifier that can be safely pseudonymized.
- RSRP, RSRQ, and SINR.
- CQI and MCS.
- BLER and retransmission counters.
- RLC/PDCP volume and retransmission counters.
- PRB utilization.
- Applied DNN, QFI, and 5QI.
- Report interval and counter reset behavior.

The 2026-07-31 network configuration export confirms that ACP statistics are
enabled at a 15-minute granularity and that KPI families for PRB usage, DRB UE
throughput, RACH, QoS flows, and L1 measurements are enabled. This does not
show which counters or per-UE dimensions the current ACP interface can export.
Use the configuration as a guide for locating the report, not as a substitute
for a sample data export.

If unavailable, state that Airspan evidence is limited to 15-minute cell-level
DRB, RACH, and availability counters. Do not infer unavailable lower-layer
metrics from application RTT.

## R8. Payload-Direction Validation

### Existing Boundary

The current TCP/MQTT/UDP probe places the selected application payload in
`Private5gProbeRequest.frame.payload`. `Private5gProbeAck` returns sequence and
timing metadata, the request payload-size value, acceptance state, and detail
text; it does not return a response body of the selected size. The existing
payload sweeps therefore measure a large vehicle-to-edge request followed by a
compact edge-to-vehicle acknowledgment. The reported RTT is round trip, but
the payload-size intervention is primarily uplink.

Before final paper revision, choose and document one of these valid paths:

1. Run the targeted response-payload experiment below and report request and
   response direction separately.
2. If the experiment cannot be run, narrow all payload-size claims, tables, and
   workload mappings to vehicle-to-edge request transfer with a compact
   acknowledgment. Do not claim that the current sweeps measured equally sized
   edge-to-vehicle responses or symmetric transfers.

### Harness Requirements

Extend the existing probe without changing its correlation and timing model:

- Add independent `request_payload_bytes` and `response_payload_bytes`
  parameters.
- Return an actual response body of the declared size.
- Fill response bytes deterministically and validate their length and checksum
  at the sender.
- Log requested and encoded bytes in each direction, response validation,
  timeout/failure state, sender RTT, and same-host server processing time.
- Preserve TCP, MQTT over TCP, and UDP as separate protocol conditions.
- Use the existing fragmented/reassembled UDP path when a response exceeds one
  safe datagram; do not compare raw oversized UDP against stream transports as
  though the representations were equivalent.
- Add local loopback tests before the field run to verify exact byte counts,
  framing, correlation, and failure handling.

### Targeted Conditions

Use the stronger measured signal condition, an otherwise idle cell, the same
car sender and edge endpoint, and the R1A/R1/O1 collection protocol.

| Direction condition | Request payload | Response payload | Purpose |
|---|---:|---:|---|
| D1 compact request/response | 1024 B | 1024 B | Confirm the new response path at compact size |
| D2 detector request | 23968 B | 0 B plus acknowledgment metadata | Reproduce the measured uplink-oriented detector condition |
| D3 detector response | 256 B | 23968 B | Compare the detector-size object in the reverse direction |
| D4 mid-size request | 262144 B | 0 B plus acknowledgment metadata | Representative uplink map/perception transfer |
| D5 mid-size response | 256 B | 262144 B | Representative downlink map/perception transfer |
| D6 bulk request | 1048576 B | 0 B plus acknowledgment metadata | Bound the measured bulk uplink condition |
| D7 bulk response | 256 B | 1048576 B | Bound bulk downlink delivery |

Run TCP and MQTT for D1-D7. Run UDP for D1-D3 with the same
fragmentation/reassembly policy in both directions; run larger UDP conditions
only if the harness has a prevalidated bounded-fragment implementation.

Use 1000 attempts for D1-D3 where runtime permits. Predeclare a smaller but
identical attempt count for each paired D4/D5 and D6/D7 condition when
seconds-scale transfers make 1000 attempts impractical. Do not report p99 from
fewer than 100 successful responses. Use at least two paired repetitions, with
three preferred, and alternate direction order between repetitions.

### Required Analysis

- Attempts, validated responses, failures, and response availability.
- RTT p50/p95/p99 only where the successful sample count supports the
  percentile.
- Miss rates at the paper's 100, 500, and 1000 ms evaluation budgets.
- Encoded request/response bytes and achieved uplink/downlink byte rate.
- Phone RSRP/RSRQ and serving cell during each condition.
- Matching Airspan uplink/downlink DRB volume and active-time counters.
- Host CPU, memory, interface drops/errors, and server processing time.

R8 supports a directional comparison only when the paired conditions use the
same application size, protocol, signal condition, endpoints, and attempt
policy. Directional asymmetry must be reported as an observed property of the
measured deployment and TDD configuration, not as a universal 5G property.

## R9. Provisional TDD Configuration Pilot

### Status And Interpretation

The user confirms that the existing experiment campaigns used the `40/40/20`
TDD configuration and that the planned follow-up will use `30/60/10`. Both
configurations report a `10D4G` structure. This configuration record is
definitive: TDD is the only parameter changed between the original and
follow-up configurations. No additional configuration-diff evidence is
required. The slash-separated values are frame allocations ordered as
downlink, uplink, and dynamic.

The network administrator identifies `10D4G` as an LTE-coexistence
frame-packing setting. Because it is fixed across both configurations, R9
neither varies nor evaluates it. The run manifests retain the value only for
configuration reproducibility.

This is an exploratory sensitivity experiment. The user does not expect the
allocation change to alter the results substantially, and the experiment is
not presented as a proposed fix. Its purpose is to measure whether the change
affects application latency, availability, or throughput, quantify the size of
that effect, and determine what the result implies for future vehicular-radio
and 6G uplink/downlink design.

The user confirms that both cells used `40/40/20` during the original
experiments. For the follow-up, both cells use `30/60/10` and remain
administratively unlocked. The July 31 export captured an intermediate
reconfiguration state and does not define either measured configuration. No
additional historical log export is required.

The baseline therefore uses 40 downlink, 40 uplink, and 20 dynamic frames on
both cells; the follow-up uses 30 downlink, 60 uplink, and 10 dynamic frames on
both cells. Before the first measurement bin, export or capture the live cell
status and verify that both cells are enabled, administratively unlocked, and
using `30/60/10`. Record the selected TDD profile and the serving cell in the
run metadata. Do not change either cell during the collection.

### Quick Diagnostic

Run the pilot first at one fixed stronger-signal location. Record quantitative
RSRP, RSRQ, and NR SINR where available; a phone signal-bar display is not an
experimental signal metric.

For the `30/60/10` follow-up:

1. collect a two-minute idle interval;
2. collect 60 seconds of sustained vehicle-to-edge uplink throughput;
3. collect 60 seconds of sustained edge-to-vehicle downlink throughput;
4. send 1000 TCP requests with a 1 KiB request and compact acknowledgment at
   the existing 200 ms interval; and
5. send at least 100 TCP requests with a 23,968 B request and compact
   acknowledgment.

Keep the vehicle location and orientation, gateway and antenna placement,
channel bandwidth, application host, traffic endpoints, request interval,
timeout, DNN, and 5QI unchanged. Capture application logs, host telemetry,
Android radio measurements, and Airspan DU Cell statistics for both cells
covering the complete comparison. Use the phone serving-cell record to map each
interval. Current observations indicate that Cell 2 carries all experiment
traffic. Record any cell change, handover, or unexpected Cell 1 traffic.

One execution is a diagnostic result only. Use at least three repetitions of
the follow-up configuration. If the stronger-signal runs show a repeatable
difference from the historical results, repeat the follow-up at one
quantitatively defined lower-signal condition to test whether the difference
depends on coverage.

The comparison treats TDD as the sole changed parameter. Because the original
and follow-up runs occur on different dates, report signal conditions, cell
load, weather, vehicle placement, and all fixed controls for each repetition.
These observations quantify run-to-run variation without reopening the
established configuration comparison.

### Analysis

Report uplink and downlink throughput separately. For the application probes,
report attempts, response availability, RTT p50/p95, and 100/500/1000 ms miss
rates. For each metric, report the absolute and relative difference from the
historical configuration with uncertainty, not only whether the direction is
better or worse. Interpret practical importance against the application
deadlines used elsewhere in the paper. Attribute a repeatable measured
difference to the TDD change within this deployment, while avoiding a universal
claim about all private 5G deployments.

Either outcome is informative. A small effect would show that changing the
frame allocation alone does not materially change the measured application
outcomes and would motivate investigation of other uplink constraints,
including link budget, vehicle antennas, scheduling, and QoS. A large effect
would identify frame allocation as an important deployment and future-radio
design lever. An effect that varies by payload, load, or signal condition would
motivate adaptive, application-aware allocation. The experiment can inform a
future vehicular-radio or 6G design argument, but it cannot by itself establish
that a new radio generation is required.

## P1. Deployment And Path Documentation

This is required even if no additional network experiment can be run. Complete
one source-backed deployment inventory from vendor documentation, redacted
configuration exports, and written Cisco/Airspan confirmation.

The inventory must include:

- Airspan gNodeB, MX250, gateway/CPE, application sender, edge host, and the
  software/firmware versions that affect the measured endpoints.
- Band n48, channel bandwidth, channel frequency or NR-ARFCN, CBSD Category A
  or B, GAA/PAL status, TDD pattern, transmit power/EIRP, and gNodeB antenna
  configuration.
- Gateway/CPE model and firmware, the scope of any receiver-sensitivity value,
  integrated/external antenna, antenna/cable details, and inside/outside
  placement and orientation for every condition. For this deployment, record
  the internal-antenna `MG52-HW`, its trunk-floor/front-face-up placement, and
  distinguish the FN990A40 modem's conducted sensitivity from whole-gateway
  performance.
- The measured system boundary from the vehicle sender and MG52 through the
  private-5G path to the MX250-connected edge host. State that the reported RTT
  includes the complete internal path and is not decomposed by segment.
- Default DNN and verified 5QI, number of active test UEs, traffic isolation,
  send interval, timeout, payload/frame-size distinction, and endpoint clock
  state.
- Raw station locations for experiment alignment and anonymized map locations
  for the double-blind manuscript.

The first manuscript description should identify the suppliers once, for
example: `a private 5G deployment comprising a Cisco-supplied local mobile core
and an Airspan 5G NR gNodeB operating in CBRS GAA spectrum`. Exact Cisco-core
inventory is not needed because the paper reports end-to-end application RTT
and makes no internal core-delay claim. Later references should use `5G
network` or `5G path`, not `Cisco 5G`.

The architecture description must state that:

- The V2X measurements use direct LTE C-V2X PC5 communication between the OBU
  and RSU.
- The 5G measurements use the NR Uu path from the vehicle gateway/CPE through
  the Airspan gNodeB and Cisco mobile core to an edge-side endpoint.
- The two paths are measured independently.
- The paper does not evaluate NR-V2X sidelink and does not implement the LTE
  C-V2X OBU-RSU exchange through the private-5G network.

P1 is complete only when each manuscript configuration value has a stored
source and every unverified value is omitted or labeled unknown.

## P2. IPI Contribution Evidence And Validation

Strengthening IPI does not require another radio campaign, but it does require
the paper's interface claims to match executable code.

Complete the following:

1. Map every API operation, type, required/optional field, state transition,
   and status named in Section 3 to the current declarations under
   `cpp/include/ipi/api/` and implementations under `cpp/src/api/`.
2. Verify that update, session registration/refresh/termination, service
   request, correlated response, status, freshness/expiration, timeout,
   missing-response, and rejection semantics are either implemented and tested
   or explicitly identified as representable but not exercised.
3. Generate one compact request/response example from the real encoder or
   example program. Do not invent a wire format in the manuscript.
4. Map IPI identifiers to the stored experiment columns that produce attempt
   denominators, response availability, correlated RTT, deadline misses,
   stale-response classification, and restart outcomes.
5. Explain the concrete difference from ordinary packet logs: packet captures
   provide endpoints, timestamps, protocols, and bytes, whereas IPI preserves
   application request identity, response correlation, status, freshness, and
   session context.
6. Preserve the paper's contribution hierarchy. IPI is the first contribution;
   the communication measurements and three measurement-derived insights remain
   the other central contribution. Do not recast Edge4AV as only an IPI paper.
7. Keep the measured boundary explicit. Implemented session or workflow
   operations that were not measured end to end cannot be presented as
   experimental results.

Validation commands:

```bash
cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON
cmake --build cpp/build
ctest --test-dir cpp/build --output-on-failure
```

Add focused tests before finalizing the interface claim if the existing suite
does not exercise a claimed state transition or failure distinction.

## P3. Manuscript, Citation, And Submission Closure

### Claim And Reasoning Review

- Build a claim-to-evidence table for every numerical result and each of the
  three insights. Each row must name the source result directory, analysis
  script, metric definition, and manuscript location.
- Scope 5G conclusions to the measured Cisco/Airspan deployment and operating
  conditions. Do not generalize an unverified deployment behavior to all 5G NR
  networks.
- Distinguish observations from causes. Payload-, signal-, load-, concurrency-,
  and restart-associated changes are observed at the endpoints. Scheduler,
  queue, HARQ, or core causes require R5-R7 evidence.
- Describe what the measured deployment failed to satisfy in application terms:
  response availability, time budget, or payload transfer requirement.
- State network improvements as requirements or research directions unless they
  are experimentally validated. Examples include verified QoS treatment,
  deadline-aware scheduling, radio/resource visibility, payload reduction, and
  path adaptation.
- Do not describe any condition as `commercial cell edge` without a verified
  operator definition. Report the measured RSRP/RSRQ range instead.
- Distinguish probe attempts from independent repetitions. Thousands of
  requests within one run improve percentile resolution but do not replace
  independent run/bin repetitions.
- Report each new repetition before any pooled summary. Preserve the
  attempt-level failure denominator and use run-aware uncertainty intervals
  when comparing paired conditions; do not pool heterogeneous days, signals,
  protocols, or client counts into one unlabeled population.
- Report p99 only when the number of successful responses supports it, and
  state when a timeout censors the observed RTT distribution.
- Ensure the introduction asks the readiness question, the experiments answer
  it, and the three insights follow from the results. Do not write as though
  the experiments were selected to prove conclusions known in advance.

### Deferred Uplink Interpretation And Design Notes

These notes are candidates for the testbed, measurement-scope, discussion, and
open-question sections. They are not manuscript claims until their required
evidence is collected.

#### Measurement And Claim Boundaries

- Report the exact n48 channel bandwidth, assigned channel or NR-ARFCN, TDD
  pattern, gNodeB software, gateway/CPE, antenna placement, DNN, and verified
  5QI. A theoretical NR configuration is not the measured configuration.
- Do not assume homogeneous coverage. Define stronger- and lower-signal
  conditions from measured RSRP, RSRQ, and NR SINR distributions and report
  the range for each condition.
- The user has observed application problems while the phone displayed its
  highest signal-bar level. Record this as an observation to validate, not a
  result. Android signal bars are an abstract, configurable level; a paper
  claim requires time-aligned radio values and application outcomes.
- Use `low-latency QoS flow` or `low-latency network treatment`, not
  `low-latency protocol`. TCP, MQTT over TCP, and UDP are the measured protocol
  conditions. The current deployment used the default DNN/5QI path, and no
  network-enforced low-latency QoS flow has been verified.
- State direction explicitly. Existing large-payload sweeps place the selected
  payload in the vehicle-to-edge request and return a compact acknowledgment.
  They primarily stress the uplink even though RTT includes both directions.
- Scope conclusions to the measured Cisco/Airspan deployment and configuration.
  A wider channel, different TDD pattern, verified QoS flow, different
  scheduler, denser deployment, or improved vehicle antenna system may produce
  different results.
- Frame the contribution around what the measurements reveal: application
  requirements expose an uplink-sensitive operating regime and identify
  network, radio, and vehicle-design changes that should be evaluated. Do not
  present the study only as a list of network failures.

#### Uplink Improvement Directions

1. **Uplink-oriented TDD resources.** More uplink slots or shorter uplink
   opportunities may improve uplink throughput or waiting time, but they trade
   against downlink resources and require synchronization and interference
   coordination in production deployments. R9 tests this direction only in the
   measured private network.
2. **Verified QoS and scheduling.** A provisioned QoS flow, appropriate 5QI,
   configured scheduling behavior, or deadline-aware scheduling may protect
   compact CAV requests under contention. An application label alone does not
   provide this treatment.
3. **Vehicle-exterior antennas.** A roof-mounted or otherwise exterior antenna
   can reduce vehicle-body penetration and self-blockage. Multiple separated
   antenna modules or a vehicular distributed antenna system can improve
   directional coverage, diversity, and MIMO performance.
4. **Uplink power and EIRP.** Higher transmit power can improve the uplink link
   budget, but it increases transmitter energy demand and is constrained by UE
   capability, equipment certification, CBRS power control, EIRP limits, and
   RF-exposure requirements. A vehicle power supply reduces handset-battery
   constraints; it does not make higher RF power consume less energy.
5. **Beamforming and spatial processing.** A vehicle-mounted antenna array
   could direct energy toward the serving gNodeB, suppress radiation and
   interference in other directions, or improve receive combining. Beamforming
   is not exclusive to millimeter wave, although narrow-beam FR2 systems depend
   on it more heavily and face blockage and beam-tracking overhead. Any n48
   design must remain within the authorized EIRP and be supported by the CPE.
6. **Beam coordination.** Coordinated beamforming, beam tracking, interference
   cancellation, and multi-point reception are research directions for
   overlapping beams, mobility, blockage, and inter-cell interference. They
   were not implemented or measured in the current deployment.
7. **Coverage-aware operation.** CAV applications and networks can adapt
   payload, direction, path, scheduling, or service placement using measured
   radio quality rather than assuming uniform coverage.

Do not propose relaxing human-safety requirements. Use the term `RF exposure`
and describe higher power, antenna gain, roof placement, and beamforming only
within applicable exposure, certification, spectrum-authorization, and EIRP
limits.

These directions connect the paper to multiple communities: CAV researchers
need application-aware availability and deadline measurements; networking
researchers need uplink-aware scheduling and QoS; RF and antenna researchers
need vehicle-integrated arrays and link-budget improvements; and signal
processing researchers need mobile beam tracking and interference management.
A possible 6G implication is that vehicle-edge systems require explicit
uplink-oriented operation rather than assuming consumer-style downlink-heavy
traffic.

### Uplink And RF Source Leads

Verify the exact claim and bibliographic metadata before adding any source to
the manuscript:

- 3GPP TS 38.213 defines TDD periods and downlink, uplink, and flexible
  slots/symbols:
  <https://www.etsi.org/deliver/etsi_ts/138200_138299/138213/17.01.00_60/ts_138213v170100p.pdf>.
- NGMN's *5G TDD Uplink* white paper explains why vertical applications may
  require balanced or uplink-oriented patterns and why differing production
  frame structures require coordination:
  <https://www.ngmn.org/wp-content/uploads/220117-5G-TDD-Uplink-White-Paper-v1.0.pdf>.
- Raffeck et al., *Parameterizing 5G New Radio*, measures the effects of
  bandwidth, TDD periodicity, and DL:UL allocation on uplink/downlink delay and
  throughput:
  <https://opendl.ifip-tc6.org/db/conf/cnsm/cnsm2024/1571045800.pdf>.
- 3GPP TS 23.501 defines 5QI as QoS-flow characteristics, including priority,
  packet-delay budget, and packet-error rate:
  <https://www.etsi.org/deliver/etsi_TS/123500_123599/123501/18.05.00_60/ts_123501v180500p.pdf>.
- 3GPP TS 38.101-1 lists UE transmitter power classes and lists n48 with the
  23 dBm default power class:
  <https://www.etsi.org/deliver/etsi_TS/138100_138199/13810101/18.05.00_60/ts_13810101v180500p.pdf>.
- Current 47 CFR 96.41 limits CBRS End User Devices to 23 dBm EIRP per 10 MHz
  and requires power control:
  <https://www.ecfr.gov/current/title-47/part-96/section-96.41>.
- Android documents signal bars as an abstract five-level value whose NR
  thresholds may use SS-RSRP, SS-RSRQ, and SS-SINR:
  <https://source.android.com/docs/core/connect/signal-strength>.
- 3GPP TR 37.828 studies high-power UE operation for fixed-wireless and
  vehicle-mounted use cases:
  <https://portal.3gpp.org/desktopmodules/Specifications/SpecificationDetails.aspx?specificationId=3949>.
- The 5GAA vehicular-DAS report discusses roof/exterior antenna placement,
  vehicle self-blockage, diversity, and multi-antenna designs:
  <https://5gaa.org/content/uploads/2022/12/5GAA_White-Paper-DAS.pdf>.
- Artner et al., *Automotive Antenna Roof for Cooperative Connected Driving*,
  evaluates distributed roof antennas and MIMO separation:
  <https://doi.org/10.1109/ACCESS.2019.2897219>.
- Alkhateeb et al., *Deep Learning Coordinated Beamforming for Highly-Mobile
  Millimeter Wave Systems*, studies beam prediction and coordination under
  mobility and blockage:
  <https://doi.org/10.1109/ACCESS.2018.2850226>.

### Literature Review

Read the underlying technical publications before adding citations. Relevant
candidates from the coauthor's suggested research program include:

- `Teleoperating Autonomous Vehicles over Commercial 5G Networks: Are We There
  Yet?`
- `An In-Depth Measurement Analysis of 5G mmWave PHY Latency and Its Impact on
  End-to-End Delay`
- `A Comprehensive Real-World Evaluation of 5G Improvements Over 4G in Low-
  and Mid-Bands`

Use a peer-reviewed paper or technical report for a technical claim. The
BREAKING-LOW/DRIVE-SAFE project and news pages may establish motivation and
project context, but they do not establish measured performance. Verify
publication status, title, authors, venue, year, DOI, and the exact sentence
supported before editing `paper/references.bib`.

### Submission Review

- Remove all coauthor review macros, strikeouts, comments, credentials,
  internal addresses, exact private deployment coordinates, and hidden
  identifying metadata.
- Use the current anonymous ACM `sigconf`/MobiCom format.
- Keep the reviewed main content within 12 pages and verify references begin on
  page 13 under the current submission plan.
- Compile the main paper and standalone appendix separately.
- Keep hyperlink generation disabled and verify the final PDFs contain no link
  annotations.
- Check figure readability, float placement, bibliography integrity,
  cross-references, overfull boxes, and undefined references.
- Perform a final terminology, sentence-logic, paragraph-link, meta-commentary,
  and claim-strength review against `AGENTS.md`.

## O1. ACP Availability And Retention

Cisco reports that the current ACP instance runs on an external NUC that can go
down and recommends a UD-managed VMware deployment. Coordinate that installation
if it can be completed without delaying the immediate collection.

Until stable hosting is available:

1. Confirm ACP access before starting each experiment day.
2. Export the required statistics immediately after the final bin.
3. Preserve one bin before and one bin after the experiment window.
4. Back up the raw export in `CISCO_AIRSPAN_STATS/` the same day.
5. Do not assume that more than approximately two days of history can be
   recovered.
6. Record every ACP outage and exclude affected bins unless their completeness
   can be verified.

## Experiments That Do Not Need Repetition

Do not repeat these solely to obtain Airspan context:

- Complete private-5G payload sweep.
- Every detector-output payload size.
- Every background-load level and stream count.
- Every client count from 1 to 100.
- Failure/restart and fallback experiments.
- Derived deadline analysis.
- Dataset workload generation and detector benchmarks.
- Existing V2X stationary payload, mobility, and signal experiments.
- The complete private-5G campaign in another vehicle or golf cart.
- Phone throughput, ping, or data-sequence tests during application
  measurements.
- Any gNodeB power-change experiment while UD prohibits configuration changes.

Repeat one of these only if a stored artifact is invalid, a central result fails
to reproduce in R2-R4, or the user explicitly changes the paper's evidence
requirements.

## End-Of-Day Procedure

Complete this checklist before leaving the test site:

- [ ] Stop every sender, receiver, load generator, and packet capture.
- [ ] Record the final application-host, phone, and ACP times.
- [ ] Export Airspan data covering one bin before through one bin after the
      day's experiments.
- [ ] Export the Android logger's raw timestamped data and KML when supported.
- [ ] Verify the phone log contains the expected serving cell, RSRP, RSRQ, and
      records across every experiment interval.
- [ ] Verify that every expected sender and receiver CSV is nonempty.
- [ ] Verify expected attempt counts and client counts.
- [ ] Verify no condition crossed an undeclared ACP boundary.
- [ ] Copy raw confidential exports to `CISCO_AIRSPAN_STATS/`.
- [ ] Confirm `git status` does not list `CISCO_AIRSPAN_STATS/`.
- [ ] Generate redacted derivatives for `results/real_5g/`.
- [ ] Record anomalies before memory of the run is lost.
- [ ] Back up the raw files before the Airspan retention window expires.

## Completion Definition

The remaining experiment and paper-evidence work is complete when:

1. R0 questions required for interpretation are answered or explicitly marked
   unavailable.
2. R1A and R1 pass.
3. R2-R4 contain at least two valid, synchronized bins per required condition,
   with three preferred.
4. Every condition has matching application, Airspan, Android radio-log, and
   metadata artifacts.
5. Stronger/weaker signal labels are measurement-backed.
6. The new representative application behavior is compared against the existing
   result family.
7. R5 is either completed with verified network treatment or retained as a
   documented blocked/negative result.
8. R6 remains excluded while the paper reports complete-path application RTT
   and makes no internal delay-decomposition claim.
9. R7 is obtained or its unavailable fields are documented.
10. R8 is completed for bidirectional payload claims, or the paper explicitly
    limits its payload conclusions to large vehicle-to-edge requests followed
    by compact acknowledgments.
11. R9 is completed before reporting the measured effect of the TDD change;
    the analysis states that TDD was the only changed parameter and reports
    field conditions and repetition-to-repetition variation.
12. P1 contains source-backed deployment and path facts.
13. P2 validates every IPI claim against the implementation and tests.
14. P3 closes claim scope, technical citations, anonymity, format, and PDF
    validation.
15. ACP availability and retention are controlled under O1 or every collection
    is exported and backed up the same day.
16. Raw confidential files remain outside Git.
17. `experiment_summary.md` and the paper are updated only after validation.

## Current Checklist

- [ ] R0 Airspan counter/configuration questions answered.
- [ ] R1A phone/app/private-5G qualification completed.
- [ ] R1 ACP/Android radio-logger validation pilot completed.
- [ ] C1 stronger signal, idle, 1024 B completed.
- [ ] C2 stronger signal, idle, 23968 B completed.
- [ ] C3 stronger signal, loaded, 1024 B completed.
- [ ] C4 stronger signal, 100 clients, 1024 B completed.
- [ ] C5 weaker signal, idle, 1024 B completed.
- [ ] C6 weaker signal, idle, 23968 B completed.
- [ ] R5 Cisco-approved QoS comparison completed or formally blocked.
- [x] R6 excluded; the paper reports complete-path application RTT.
- [ ] R7 additional Airspan export obtained or documented unavailable.
- [ ] R8 directional payload experiment completed or manuscript scope narrowed.
- [ ] R9 both-cell configuration verified and repeated `30/60/10` comparison
      completed.
- [ ] P1 deployment inventory and V2X/5G path description completed.
- [ ] P2 IPI implementation mapping and test validation completed.
- [ ] P3 claim/citation/submission review completed.
- [ ] O1 stable ACP hosting established or same-day export procedure confirmed.
- [ ] All raw and redacted artifacts validated.
- [ ] `experiment_summary.md` updated.
- [ ] Paper claims updated and rechecked against the final evidence.
