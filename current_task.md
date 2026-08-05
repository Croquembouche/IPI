# Current Task

Last updated: 2026-08-05

## Task

Publish a vehicle-ready private-5G follow-up runbook, record the deployment
values needed to interpret the new measurements, and define a locked-cell
`40/40/20` versus `70/20/10` TDD comparison while keeping observations,
hypotheses, and untested improvements distinct.

## Status

The static Airspan inventory is partially complete. The existing experiments
used the `40/40/20` TDD configuration, and the planned follow-up uses
`70/20/10`; both report a `10D4G` structure. The attempted `30/60/10`
configuration did not support a usable collection and produced no valid
experiment result. The user identifies the three slash-separated components,
in order, as downlink, uplink, and dynamic frame allocations. The original
profile therefore contains 40 downlink, 40 uplink, and 20 dynamic frames; the
follow-up contains 70 downlink, 20 uplink, and 10 dynamic frames. The new
profile is downlink-heavy and is not an uplink-enhanced configuration. The
network administrator clarifies that the unchanged `10D4G` value is frame
packing for LTE coexistence. Record it for reproducibility, but do not analyze
it as part of the TDD comparison.
Header-verified per-cell carrier values and current
RF power/EIRP are now available. The complete grant record and antenna
azimuth/downtilt remain to be collected. The current SAS provider is now
identified from the 2026-07-31 CBRS configuration export. The vehicle CPE is
now identified as a Cisco Meraki `MG52-HW`, and its placement, orientation,
current firmware, and wired-link state are recorded below. TDD is the only
intended changed parameter for a newly collected same-cell comparison. A
historical-to-follow-up comparison also spans collection dates and cannot be
described as the same controlled causal comparison. The user confirms that
the original experiments used DNN `cisco5g` and were assigned the default 5QI
9 treatment.
The numeric QFI, any attempted nondefault 5QI assignment, and the ACP
performance-counter scaling remain unresolved.
The signed installation analysis now documents the outdoor mounting structure
and antenna centerline height.
The follow-up configuration plan is confirmed, but no new collection is yet
recorded in the repository. The MG52 will be locked to one Airspan cell so that
no handoff occurs. The runbook requires Cell 2 because current evidence places
the original experiment traffic on Cell 2. If another cell is selected, a new
`40/40/20` reference must be collected on that same cell before `70/20/10`.

The pending experiment runbook now begins with a vehicle-day execution order.
It requires a pull/build/test preflight, R1A phone qualification, the three-bin
R1 validation, and then the two-repetition C1-C6 matrix. The `70/20/10`
R1/C1/C2 measurements can supply the follow-up side of R9 without duplicate
probes. A newly collected same-cell `40/40/20` reference is required for a
controlled causal comparison; otherwise R9 remains a historical-to-follow-up
sensitivity comparison. Historical result scripts must not be run in place
because some retain historical run IDs and output paths.

## Cisco And Meraki Information Retrieval

- The Cisco Networking Learning Hub URL supplied by the user is a general
  Meraki training catalog. It does not expose deployment configuration.
- The Meraki Dashboard can document the MX250 model, firmware, WAN/LAN status,
  active ports, VLANs, routes, visible clients, layer-2 topology, events, and
  packet traffic visible on routed MX interfaces. A read-only click path and
  evidence checklist are now recorded in `remaining_exp.md`.
- The user confirms that the vehicle gateway/CPE is a Cisco Meraki
  `MG52-HW`. In `Cellular Gateway > Monitor > Cellular gateways`, its `Uplink`
  and `Performance` views can provide the active RAT and band, APN, RSRP,
  RSRQ, link state, and time-ranged performance fields. Subscriber and device
  identifiers must remain in the excluded evidence directory.
- Meraki MX evidence cannot establish Airspan TDD/RAN configuration or an
  active QoS flow's QFI and 5QI. The user has supplied written confirmation for
  DNN `cisco5g` and default 5QI 9. Numeric QFI and nondefault QoS-flow evidence
  are needed only if the optional QoS comparison is pursued. Exact Cisco-core
  inventory and internal path decomposition are outside the current paper
  scope.
- Meraki packet captures can show packet presence, direction, DSCP/TOS, and
  timing at the selected MX interface. They cannot by themselves isolate
  over-the-air delay or prove a 5QI/QFI assignment.
- The user confirms that the application edge computer is directly connected
  to the MX250. This removes an external WAN or cloud hop after the local 5G
  system. It does not make the gNodeB-to-edge path literally zero-delay: RAN,
  core/UPF, MX250, Ethernet, and endpoint processing remain within the
  application RTT unless bounded separately. Treat the local wired segment as
  expected to be small, not as measured zero latency.

### Vehicle Gateway/CPE

- The user identifies the vehicle CPE as a Cisco Meraki `MG52-HW` with an
  internal antenna. During all original experiments, it was placed flat on the
  bottom of the vehicle trunk with its front face oriented upward; this
  placement did not change between runs. The current dashboard reports MG
  firmware `4.1.2`, modem firmware
  `M0R.115005`, host version `A0R.501056`, carrier PRI version `P0R.000566`,
  and carrier profile `Generic GCF`. The experiment-side Ethernet link reports
  `1000 Mbit/s`, full duplex. The user confirms that these firmware and link
  values match the original experiment campaigns. Report the observed wired
  link separately from vendor maximum data rates.
- Cisco documents the MG52 as a 5G SA/NSA sub-6 gateway using the Telit
  FN990A40 radio, with an internal antenna, two 2.5 GbE ports, maximum
  passthrough rates of 2 Gbit/s downlink and 300 Mbit/s uplink, and 4x4
  downlink MIMO. The 1 Gbit/s negotiated Ethernet link used by the experiments
  is below the device's port capability and is the relevant wired-link value
  for the measured system:
  <https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/Overviews_and_Datasheets/MG52%252F%252F52E_Technical_Specifications>.
- The FN990 family hardware guide lists typical conducted n48 receiver
  sensitivity at 30 kHz subcarrier spacing and 40 MHz bandwidth as -91, -91,
  -93, and -92 dBm for its four antenna ports and -97 dBm combined. These are
  modem-level conducted values measured under the guide's stated laboratory
  conditions. They are not a measured whole-device sensitivity value for the
  MG52 with its enclosure and internal antennas:
  <https://fccid.io/RI7FN990A40/User-Manual/User-Manual-7361571.pdf>.
- Cisco states that the MG52 internal antennas have a front-face bias. The
  original trunk-floor, front-face-up orientation is now recorded. Every
  follow-up run must record whether it retains or deliberately changes this
  placement:
  <https://documentation.meraki.com/SASE_and_SD-WAN/Cellular/Product_Information/MG_Antenna_Datasheets/MG52_Internal_Antenna_Datasheet>.

## Airspan Information Retrieval

- User-reported ACP configuration values recorded on 2026-07-31: the deployed
  `AS29-N48-DSC1` is an all-in-one AirSpeed 2900 with one xPU, CU-CP, CU-UP,
  DU, and RU; two enabled DU cells map one-to-one to two RU sector carriers.
  Both sector carriers inherit the DU configuration, have CBRS enabled, are
  registered as CBSD Category B, report an antenna-gain value of 17 with
  cross-polarization enabled, and
  obtain CPI information from the SAS. The clock source is GNSS, the UE
  inactivity timer is 7 s, the primary PLMN is CBRS MCC 315/MNC 010, and the
  configured slice is eMBB with SST 1 and SD 000002. `Frequency Reuse One` and
  the CBRS measurement-capability field are both reported as disabled. These
  hardware, cell, and CBRS values are now supported by header-bearing ACP CSV
  exports and the 2026-07-31 network configuration export in the connected
  Drive folder.
- Exact node and managed-element identifiers, management addresses and
  usernames, internal user IDs, cell identities, and GNSS coordinates supplied
  in the same transcription are intentionally omitted from tracked
  documentation. Retain them only in the locally excluded
  `CISCO_AIRSPAN_STATS/` evidence directory and redact them from manuscript
  material.
- The user confirms that both cells used `40/40/20` with `10D4G` during the
  original experiments. The follow-up will use `70/20/10` with `10D4G` on the
  selected serving cell, and the MG52 will be locked to that cell so no handoff
  occurs. Use Cell 2 to preserve comparability with the original campaigns.
  Current observations show that Cell 2 served the vehicle UE and carried the
  original experiment traffic; Cell 1 did not carry that traffic. The July 31
  export reports an intermediate reconfiguration state in which Cell 1 was
  active at `30/60/10` and Cell 2 was locked at `40/40/20`; do not describe
  that snapshot as either measured experiment configuration. Both CBSD entries
  report `Authorized` with successful SAS heartbeats. The software inventory
  marks `22.00-53-0.0` with platform `22.0-24-0.0` as the current package.
- Header-bearing ACP exports dated 2026-07-31 confirm that both cells use band
  n48, TDD, 40 MHz bandwidth, 30 kHz PDCCH/SSB subcarrier spacing, and a 20 ms
  SSB periodicity. Cell 1 reports NR-ARFCN 637992 and SSB frequency
  3558.720 MHz; Cell 2 reports NR-ARFCN 645334 and SSB frequency 3669.600 MHz.
  Applying the 3GPP NR-ARFCN mapping gives reference frequencies of 3569.880
  MHz and 3680.010 MHz, respectively. The 2026-07-31 network configuration
  export captured Cell 1 as active/unlocked at `30/60/10` and Cell 2 as
  locked at `40/40/20`, both with `10D4G`. This was an intermediate
  reconfiguration snapshot, not a measurement configuration. The network
  administrator identifies `10D4G` as a fixed frame-packing setting used for
  coexistence with LTE systems. It is unchanged between the configurations and
  is not an experimental variable or an explanation for measured performance.
- The same active configuration export records a maximum of one uplink layer
  and two downlink layers for each cell, with downlink 256-QAM enabled. These
  are configured cell capabilities, not measured modulation, rank, or
  throughput during an application run.
- ACP statistics collection is enabled at a 15-minute granularity. The active
  configuration enables KPI families for PRB usage, DRB UE throughput, RACH,
  QoS flows, and L1 measurements, among others. This establishes that ACP is
  configured to collect those KPI families, but it does not establish the
  exact counters, dimensions, or per-UE fields available from an export.
- The header-bearing CBSD export reports, for each cell, 40 MHz bandwidth,
  33 dBm cell transmit power, 34 dBm/MHz cell EIRP, `0/4` under
  `PAL/GAA Count (10MHz)`, one requested and authorized grant, no suspended
  grant, and a successful SAS heartbeat. The header order supports reading
  `0/4` as zero PAL and four GAA 10 MHz assignments. The 2026-07-31 CBRS
  configuration export identifies Federated Wireless as the enabled primary
  SAS connection, using CBRS configuration version 1.2, with no backup domain
  proxy enabled. It does not include the four individual grant records, so
  grant-level frequency and power evidence remains unresolved.
- The user's definitive configuration record establishes that both cells used
  `40/40/20` with `10D4G` during the original experiments. The current
  follow-up uses `70/20/10` with `10D4G` on the MG52-locked serving cell. The
  user identifies the order as downlink, uplink, and dynamic frame allocation.
  The baseline therefore has 40 downlink, 40 uplink, and 20 dynamic frames; the
  follow-up has 70 downlink, 20 uplink, and 10 dynamic frames. TDD is the sole
  intended changed parameter only when the same cell, channel, signal
  condition, placement, endpoints, and application settings are retained. A
  newly collected same-cell baseline permits that controlled interpretation;
  a comparison only with historical results does not. Retain the separate
  `10D4G` value for reproducibility, but do not treat it as part of the
  comparison.
- Two ACP screenshots added on 2026-07-31 document gNodeB network and 5QI
  properties. The earlier network-properties screenshot displays
  `RLC Acknowledge Mode Support` as disabled, DSCP 48 for NG-C and Xn-C, DSCP
  16 for management, and the NG-C SCTP heartbeat/retry timers. The
  5QI-properties view shows DSCP disabled and
  `RLC-UMBidirectional` selected for each displayed 5QI profile from 1 through
  9. These are gNodeB-side profile settings. They do not identify the QFI/5QI
  assigned to an experiment PDU session and do not prove that a Cisco
  core-side QoS policy was applied.
- The 2026-07-31 network configuration export reports `RLC Acknowledge Mode
  Support` as enabled, and the user confirms that the live setting is enabled.
  Treat `Enabled` as the current capability setting and the earlier screenshot
  as a non-current view. This setting does not establish the RLC mode used by
  the measured bearer.
- In ACP, open the deployed gNodeB configuration and then the `gNodeB Network
  Properties` view; search for the exact field `RLC Acknowledge Mode Support`.
  In the active XML export, the equivalent field is
  `/root/GnbConfigs/GnbDetailsGet/GnbProperties/NetworkConfig/RlcAcknowledgeModeSupport`,
  which currently evaluates to `Enabled`. The separate `5QI Properties` view
  contains the per-profile `RLC Mode` row.
- The screenshot's UM setting means that the displayed profiles use RLC
  Unacknowledged Mode. 3GPP TS 38.322 assigns RLC ARQ error correction only to
  Acknowledged Mode, but lower-layer HARQ and transport-layer recovery remain
  separate mechanisms. Do not attribute application loss or latency to RLC UM
  without bearer-level evidence that the measured flow used one of these
  profiles:
  <https://www.etsi.org/deliver/etsi_ts/138300_138399/138322/17.03.00_60/ts_138322v170300p.pdf>.
- The user confirms that no other users share this private deployment during
  the experiments and that no interference from other bands is present. Treat
  the measurements as an isolated-deployment condition and do not introduce
  external-user load or inter-band interference as an explanation.
- A primary-source web search did not find a public Airspan definition of the
  `40/40/20`, `30/60/10`, or `70/20/10` labels. The user provides the
  deployment-specific mapping of the slash-separated fields to downlink,
  uplink, and dynamic frame allocations. 3GPP defines a TDD pattern through
  its periodicity and counts of full downlink/uplink slots and partial
  downlink/uplink symbols; remaining symbols are flexible. The network
  administrator separately identifies `10D4G` as frame packing for LTE
  coexistence. Because it remains fixed, the follow-up does not evaluate its
  effects:
  <https://www.etsi.org/deliver/etsi_ts/138200_138299/138213/17.03.00_60/ts_138213v170300p.pdf>.
- 3GPP TS 28.552 resolves part of the DU-counter interpretation. A mean active
  UE is a UE with DRB data available for uplink, downlink, or both, not every
  attached UE. Its standard average UE-throughput metric is intended for bursts
  spanning multiple slots; a small burst contained in one initial HARQ
  transmission has zero throughput-measurement time and belongs to the
  unrestricted-volume category. Short periodic probes can therefore coexist
  with a zero mean-active-UE value or contribute weakly to the displayed DRB
  throughput. Airspan must still confirm ACP scaling, rounding, sampling, and
  whether its exported columns implement these standard measurements:
  <https://www.etsi.org/deliver/etsi_ts/128500_128599/128552/18.07.00_60/ts_128552v180700p.pdf>.
- The Airspan installation guide filed under FCC ID `PIDAS2900` maps product
  code `AS29-N48-DSC1` to the integrated-antenna, DC-powered n48 variant. It
  specifies an integrated 17 dBi dual-slant antenna with 65-degree azimuth and
  8-degree elevation beamwidth and support for two carriers as `2x 2T2R`. For
  its 40 MHz, 17 dBi antenna configuration, the guide lists equipment maxima of
  33 dBm conducted power, 44.23 dBm EIRP per 10 MHz, and 50 dBm EIRP over the
  bandwidth. These are certified product capabilities, not evidence of the
  configured transmit power or the SAS-authorized EIRP used by this deployment:
  <https://fccid.io/PIDAS2900/User-Manual/User-Manual-5885368.pdf>.
- The locally excluded signed structural analysis in `CISCO_AIRSPAN_STATS/`
  identifies the installation as a 21 ft tripod/mount structure and places one
  AirSpeed 2900 at a 20 ft antenna centerline above roof level. Its antenna
  placement sketch is not referenced to north and gives no mechanical or
  electrical downtilt. It therefore supports the outdoor placement and height
  fields, but not azimuth or downtilt. Exact site coordinates and project
  identifiers remain only in the excluded evidence file.
- The Drive coverage-prediction presentation labels the AirSpeed 2900 model
  input as 20 ft above ground level and shows predicted downlink SS-RSRP, not
  measured coverage. The signed structural analysis instead describes a 20 ft
  antenna centerline above roof level. Confirm the actual installed height
  above ground and the propagation-model height before using either value or
  the predicted map in the paper.
- The ACP software and network-function exports confirm AirSpeed 2900 product
  code `AS29-N48-DSC1`, platform version `22.0-24-0.0`, application version
  `22.00-53-0.0`, and the all-in-one xPU/CU-CP/CU-UP/DU/RU composition. The
  board value `xpu_2200` remains user-reported because it is not present in
  these exports.
- The current DU Cell CSV contains performance counters only. It does not
  establish the exact gNodeB model, CBSD category, spectrum authorization,
  channel configuration, antenna installation, CPE model, or receiver
  sensitivity.
- In ACP, use the selected gNodeB's read-only `Inventory`, `Software`, `5G
  Cells`, `Status`, `Provision`, and `Network Functions` views, plus the
  top-level `Topology` view, to record installed hardware, software, cell, and
  network-function configuration. Do not save or apply configuration changes.
- Obtain individual grant parameters, authorized frequency range/EIRP, and
  remaining Category-B installation parameters from the SAS grant and CPI
  records. These values must not be inferred from the product datasheet.
- The gNodeB and gateway/CPE models are now confirmed as an AirSpeed 2900 and
  a Meraki `MG52-HW`. The available receiver-sensitivity value is for the MG52's
  FN990A40 modem under conducted laboratory conditions; do not present it as a
  measured whole-gateway value.
- Numeric QFI and nondefault QoS-flow configuration require Cisco-side records
  if the optional QoS experiment proceeds; they are not Airspan DU Cell fields.
  Cisco-core placement is outside the current paper scope.
- The user confirms that the original experiment sessions used DNN `cisco5g`
  and were assigned default 5QI 9. No numeric QFI was provided. This confirms
  the default session treatment but does not validate the separate
  `5qi-mapped` application label as a nondefault network-enforced QoS flow.
- The connected Drive folder's physical-SIM and eSIM exports contain inventory
  and session columns, including a `UPF Name` header, but they do not provide a
  usable UPF identity, DNN/APN, 5QI, QoS-flow evidence, MX250 configuration, or
  gateway/CPE model. They do not verify a nondefault QoS assignment.
- Airspan's SR22.00 ACP release notes require the ACP server to be installed in
  UTC. This is a product requirement, not proof of how this ACP version renders
  CSV timestamps for the logged-in user; the runtime export timezone still
  requires verification.
- `All_Config.xml` is a collection of generic ACP profile templates rather
  than a record of the profile assigned to the deployed AirSpeed 2900. Do not
  use its template values as deployment facts. The separate
  `Airspan_NetworkConfigExport_20260731_1234.xml` contains the active gNodeB
  configuration and is the source for the current cell, TDD-label, clock,
  inactivity-timer, and KPI-collection settings.
- Keep raw screenshots, exports, identifiers, and vendor records under the
  locally excluded `CISCO_AIRSPAN_STATS/` directory. Only redacted,
  source-attributed values may enter the manuscript or tracked result files.

## Uplink Interpretation And TDD Pilot

- Added evidence-scoped notes for reporting bandwidth/spectrum, quantitative
  stronger/lower coverage, problems observed under the phone's highest
  signal-bar level, measured-versus-theoretical configuration, explicit
  uplink/downlink direction, and positive design implications.
- Corrected `low-latency protocol` to `low-latency QoS flow` or
  `low-latency network treatment`; no such network-enforced flow has yet been
  verified in the measured deployment.
- Recorded prospective uplink improvements: uplink-oriented TDD allocation,
  verified QoS/scheduling, exterior and distributed vehicle antennas,
  regulation-compliant power/link-budget improvements, beamforming, coordinated
  beam processing, and coverage-aware operation.
- Recorded the safety boundary: do not propose relaxing RF-exposure rules.
  Higher power, gain, EIRP, and beamforming must remain within equipment,
  certification, CBRS, and exposure limits.
- Revised R9 to compare baseline `40/40/20` with follow-up `70/20/10` on an
  MG52-locked serving cell; both configurations report `10D4G`. The
  slash-separated values are downlink, uplink, and dynamic frame allocations:
  40/40/20 for the baseline and 70/20/10 for the follow-up. The attempted
  `30/60/10` configuration produced no usable measurement. R9 is an exploratory
  directionality sensitivity study. Because `70/20/10` reduces uplink
  allocation, it must not be presented as an uplink-enhanced profile. No live
  comparison has yet been recorded.
- Added primary standards, regulatory material, measurement literature, and
  vehicular-antenna/beamforming source leads for later manuscript review.

## Active Constraints

- Use the complete current `results/real_5g/` tree rather than one run or the
  manuscript summary alone.
- Preserve negative experiments as results; do not relabel an attempted but
  unverified QoS experiment as though it never occurred.
- Distinguish end-to-end application measurements from RAN/core telemetry and
  causal diagnosis.

## Files Touched In Current Task

- `current_task.md`
- `remaining_exp.md`

## Existing Experiments Relevant To The Proposed Work

- Five stationary baseline campaigns repeated payload and protocol conditions
  at different locations. Separate signal-survey measurements were assigned to
  those run locations by nearest-point selection or spatial interpolation.
- Stronger- and weak-signal versions exist for load, detector-output replay,
  and 1--100-client scalability experiments.
- Background-load sweeps vary offered uplink load and stream count, while
  multiclient sweeps vary application concurrency.
- A QoS verification experiment captured default and `5qi-mapped` TCP traffic
  at the vehicle interface. Both captures had `tos 0x0`; the experiment did not
  establish host marking or network-enforced 5QI treatment.
- Sender logs contain client/server timestamps and RTT, but all inspected
  private-5G sender files use `clock_sync_state=unsynced` and no synchronized
  CPE, gNodeB, UPF, or core timestamps are stored.

## Follow-Up Experiment Classification

1. Per-request RSRP/RSRQ/SNR or SINR, CQI, MCS, BLER, HARQ/RLC, and serving-cell
   telemetry: **not completed**. Existing radio values come from a separate
   16-point survey and are assigned at run/location level, not recorded for each
   request.
2. Controlled signal sweep using attenuation, transmit-power control, or another
   method that holds location and workload constant: **partially completed**.
   Existing experiments compare locations and stronger/weak-signal campaigns,
   but signal was not independently controlled and day/location effects remain.
3. CPE or antenna inside-versus-outside vehicle comparison: **not completed**.
   The internal-antenna `MG52-HW` remained on the trunk floor with its front
   face upward throughout the original campaigns, but no controlled
   inside-versus-outside comparison or associated run pair is stored.
4. Synchronized path decomposition across the CPE, RAN/core, and edge host:
   **excluded from the current paper scope**. The paper reports application RTT
   over the complete vehicle-to-local-edge path and does not attribute delay to
   an internal path segment. The edge computer is directly connected to the
   MX250, and the only stored packet captures are vehicle-interface captures
   for QoS verification.
5. Verified network-enforced 5QI comparison: **attempted with a negative
   verification result**. Application labels were tested, but packet captures
   showed no TOS/DSCP difference and no core-side QoS-flow evidence is stored.
6. RAN-internal load and scheduling experiment with PRB, scheduler, queue, CQI,
   MCS, BLER, or retransmission counters: **partially completed at the
   application level only**. Background-load and multiclient sweeps exist, but
   no RAN/core counters were collected.
7. Transmit-power, TDD, scheduler, or verified QoS configuration comparison:
   **not completed**. No run family changes these network configurations.

## Validation

- On 2026-08-04, configured and built `cpp/` with tests enabled and ran all
  seven CTest targets successfully.
- Ran `bash -n` on the current load/QoS, multiclient, detector-output TCP/MQTT,
  and fragmented-UDP reference collection scripts; all passed syntax checking.
- Ran `git diff --check` on the five vehicle-documentation files; no whitespace
  errors were reported. Fetched `origin/main` and confirmed that local `HEAD`
  and the remote branch had no divergence before staging.
- Limited the vehicle publication scope to `AGENTS.md`, `agent_context.md`,
  `current_task.md`, `experiment_summary.md`, and `remaining_exp.md`. The local
  Cisco/Airspan evidence directory remains excluded, and unrelated paper map
  and chart scripts are not part of this publication.
- Verified that R9 distinguishes the historical `40/40/20` baseline from the
  planned locked-cell `70/20/10` follow-up and separates uplink and downlink
  measurements. A same-cell, newly collected baseline is required before TDD
  can be treated as the only changed parameter; a historical-only comparison
  is reported as a sensitivity comparison. One pair remains diagnostic rather
  than manuscript evidence.
- Ran `git diff --check -- remaining_exp.md current_task.md`; no whitespace
  errors were reported.
- No follow-up experiment is yet recorded in the repository. Before collection,
  verify `70/20/10` on the selected cell and verify that the MG52 is locked to
  Airspan Cell 2. Export both cell rows so nonselected-cell traffic remains
  visible. Treat the co-located phone's serving-cell record separately because
  it does not establish the MG52 serving cell.
- Updated the vehicle runbook after the configuration change: Cell 2 is the
  required locked serving cell for comparison with the original campaign. The
  condition identifier C2 remains distinct from Airspan Cell 2.
- Reviewed all private-5G experiment families in `experiment_summary.md` and
  enumerated every top-level run directory under `results/real_5g/`.
- Checked all private-5G sender CSV headers. They contain application identity,
  condition, payload, endpoint timestamps, RTT, and outcome fields, but no radio
  or RAN telemetry fields.
- Searched private-5G artifacts for RSRP, RSRQ, SNR/SINR, CQI, MCS, BLER,
  HARQ/RLC, PRB, scheduler, queue, TDD, ARFCN, power, attenuation, CPE, and
  antenna records. Radio metrics occur only in the separate signal-survey and
  derived run-level mapping artifacts; no lower-layer trace was found.
- Confirmed that no sender CSV records a mobile private-5G condition or a
  synchronized clock state.
- Confirmed that the only stored private-5G packet captures are the two local
  QoS-verification captures.
- Reviewed the locally excluded 25-page signed structural analysis, including
  its antenna-placement sketch. It supports a 20 ft antenna centerline on a
  21 ft outdoor tripod/mount structure but does not establish a north-referenced
  azimuth or mechanical/electrical downtilt.
- Accessed and inventoried all 23 files under the connected `Airspan Radio`
  Drive folder. Reviewed all 10 current ACP configuration CSVs, all five
  Airspan manuals/release notes, the DU-cell counter export, the two SIM
  inventory workbooks, the eSIM provisioning document, and the four deployment
  and procurement PDFs. The review resolves the status-table headers and the
  current per-cell RF/CBRS values recorded above. Those 23 files alone did not
  contain the TDD labels or current SAS provider; the later configuration
  exports resolve those values. User confirmation resolves the frame-allocation
  mapping. The network administrator identifies `10D4G` as an unchanged
  LTE-coexistence packing setting, so it is recorded but excluded from the
  experimental comparison. Exact Cisco-core and internal-path fields are
  outside the current paper scope.
- Reviewed the two locally added ACP screenshots at original resolution. They
  resolve the configured gNodeB 5QI/RLC template values and control-interface
  DSCP/SCTP values. The screenshots alone do not resolve the active experiment
  flow's QFI/5QI, TDD pattern, SAS provider, antenna orientation, or ACP
  performance-counter semantics.
- Recorded the user's confirmation that the CPE is an `MG52-HW`, remained flat
  on the trunk floor with its front face upward for every original run, used
  DNN `cisco5g`, and was assigned default 5QI 9. Also recorded that the edge
  computer is directly connected to the MX250 and that the current firmware and
  Ethernet-link values match the original campaigns. Numeric QFI and any
  nondefault 5QI assignment remain unverified; internal delay decomposition is
  outside the current paper scope.
- Queried the active Airspan XML export at
  `/root/GnbConfigs/GnbDetailsGet/GnbProperties/NetworkConfig/RlcAcknowledgeModeSupport`
  and confirmed that it currently returns `Enabled`; the user independently
  confirms the live setting. This capability value does not override the
  `RLC-UMBidirectional` 5QI-profile settings or establish the mode used by an
  experiment bearer.
- Downloaded and XML-validated `All_Config.xml` and
  `Airspan_NetworkConfigExport_20260731_1234.xml`, and reviewed
  `CBRSConfiguration_20260731_1230.csv`. The files remain under the locally
  excluded `CISCO_AIRSPAN_STATS/` directory. The CBRS export resolves the
  current primary SAS provider; the network export resolves the current cell
  frame labels and confirms 15-minute statistics collection, GNSS clock
  source, and the 7-second UE inactivity timer. `All_Config.xml` is a generic
  profile library and was not used as active-deployment evidence.
- Checked Cisco's MG52 product documentation, technical specification, and
  internal-antenna documentation, then checked the FN990A40 hardware guide for
  n48 modem sensitivity. Vendor maximums and modem-level conducted sensitivity
  are kept distinct from the observed 1 Gbit/s full-duplex Ethernet link and
  from whole-gateway radio performance.
- Checked primary FCC/Airspan, 3GPP/ETSI, and Cisco documentation online. The
  public AirSpeed 2900 material confirms the product and certified frequency
  range but does not define the Airspan TDD shorthand. 3GPP resolves the TDD,
  active-UE, DRB-throughput, and RLC terminology; Cisco documentation confirms
  that 5QI-to-DSCP treatment is configured through SMF/DNN QoS policy, so the
  gNodeB profile screenshot alone is not flow-assignment evidence.
- Checked current official Meraki documentation for inventory, appliance
  status, firmware, VLANs, route table, clients, topology, event log, packet
  capture, and MG cellular-gateway views. Checked Cisco's private-5G
  architecture documentation to confirm that mobile-core and subscriber/QoS
  management belong to Cisco Control Center/Private 5G management rather than
  the MX appliance pages.
- The Drive folder contains eSIM activation material and procurement records
  with financial and personal information. None of those values were copied
  into the repository. Keep the folder access-controlled; if it was shared
  outside the intended team, review access and revoke or reissue any still-valid
  activation material.

## Next Steps

- Obtain administrator approval and car-side host access, then run the
  phone/ACP validation pilot before the targeted follow-up experiments.

## Subsequent Implementation Task: PSM and Cooperative J2735 Flow

Task requested 2026-07-13: add SAE J2735 Personal Safety Message support and
make both standard J2735 payloads and IPI cooperative CAV messages available
through the same send/receive interfaces.

Status: complete in the working tree.

- Added a typed PSM model with identity, position, motion, accuracy,
  acceleration, path history/prediction, and propulsion fields.
- Added PSM profile encode/decode, byte encode/decode, MessageFrame/API traits,
  private-5G probe support, and TCP/MQTT/UDP sender selection.
- Added typed `IPI-CooperativeService` packing/unpacking to the high-level
  J2735 API.
- Added opaque pre-encoded J2735 payload ingest/list/broadcast methods for full
  standard messages not modeled by the lightweight helper types.
- Added and repaired ROS 2 conversions so the optional `v2x_msg` bridge builds
  for BSM, PSM, MAP, SPaT, SRM, and SSM.

Validation:

- Normal C++ build passed.
- All 7 CTest tests passed, including the new PSM/cooperative flow test.
- Live TCP `--message psm` sender/receiver probe passed with an accepted PSM.
- Optional ROS 2 `v2x_msg` package and IPI ROS bridge builds passed using ROS 2
  Humble with the system Python interpreter.

## Active Task Update: Coauthor Email Action Plan

Task requested 2026-07-23: read the saved 35-message Cisco/UD coauthor email
chain and identify what William must do for each manuscript comment and proposed
private-5G follow-up experiment.

Status: email analysis and downloaded-material review complete. The QoS
document, sample G-NetTrack archive, and DU Cell export have been reviewed. No
paper source or experiment artifact was changed.

### Immediate User Actions

- Keep `CISCO_AIRSPAN_STATS/` in the local working tree as requested, but retain
  its local Git exclusion so vendor documents, internal addresses, device
  identifiers, and unredacted exports are never committed or uploaded.
- Run one ACP/G-NetTrack validation pilot before the follow-up experiments. Use
  one controlled traffic source, isolate other test UEs where possible, align
  the run with ACP's 15-minute reporting boundaries, and record the timezone
  used by every logger.
- Export ACP after every new experiment day, before the approximately two-day
  retention window expires.
- For the scheduled Cisco discussion, confirm numeric QFI only if the optional
  QoS comparison proceeds. The AirSpeed 2900, Category-B/GAA configuration,
  current channel and RF values, `MG52-HW` placement and firmware continuity,
  DNN `cisco5g`, and default 5QI 9 are now recorded. Exact Cisco-core inventory
  and path decomposition are not required for the current paper.
- Ask Cisco/UD administrators for the requirements and approval to install the
  Airspan ACP on a stable UD-managed VMware server.
- Tell Cisco that the desired second QoS profile is higher-priority,
  lower-latency treatment for compact CAV request/response traffic under
  contention, compared against the current default DNN/5QI 9 path. Ask Cisco to
  recommend a supported non-default 5QI and provide core-side policy, QFI/flow,
  and counter evidence.

### Download Review

- `Dynamic QoS Rules.pdf`, `G-NetTrack_Pro_Logs 1.21.zip`, and
  `DUCellExport_20260724_1002.csv` are stored under the local
  `CISCO_AIRSPAN_STATS/` directory. The directory is excluded through
  `.git/info/exclude` because the PDF is vendor-confidential and the exports
  contain internal identifiers. The files will remain in this directory as the
  user requested and must not be added to Git.
- The QoS document supports a default-session policy and an optional dedicated
  PCC flow. A dedicated flow may use GBR or non-GBR 5QI values and may match
  protocol, endpoint, port, and traffic direction. The document's camera
  example uses a dedicated 5QI 4 flow, but that example does not establish the
  correct profile for compact CAV request/response traffic.
- Dynamic QoS must not be enabled as an exploratory change. The document states
  that the feature cannot be disabled after activation and that a saved PCC rule
  cannot be removed. It also requires confirmation that the radio supports the
  requested GBR and has sufficient resources.
- The document lists default-session 5QI values 5--9 in one parameter
  description but gives 5QI 69 in its recommended example. Cisco must resolve
  this inconsistency and provide the exact supported DNN, 5QI, GBR/non-GBR,
  bit-rate, and flow-filter values for the installed software release.
- The G-NetTrack archive contains eight sample AT&T LTE runs from 2018, not
  measurements from the UD private-5G deployment. Its raw records are
  tab-delimited text files with 243 columns; the archive also includes
  per-metric KML files. Across 6,753 records, the samples contain GNSS, speed,
  network mode, signal level and quality, SNR, CQI, LTE RSSI, and bitrate fields.
- The sample runs have median timestamp intervals between 1.0 and 3.5 seconds.
  Their ping-latency and ping-loss fields are empty. The archive therefore
  demonstrates an export format but provides neither packet-level timing nor
  evidence that the current app exposes the needed 5G NR fields on the lab
  phone.
- Before a full campaign, run a short stationary pilot on the lab private-5G
  network and verify the current export fields, timestamp behavior, sampling
  interval, NR serving-cell values, and whether ping tests can run concurrently
  with the application workload. Drop device identifiers, subscriber
  identifiers, phone numbers, and local IP addresses before any derived data is
  stored in the repository.
- The DU Cell export contains 590 rows for two cells in uninterrupted
  15-minute bins from 2026-07-21 08:15 through 2026-07-24 10:00. Its 45 columns
  cover DRB volume, active-time-derived throughput, RACH attempts and outcomes,
  cell availability, and mean active-UE/preamble counters.
- Every row is identified only by cell and time bin. The export has no UE
  identifier, serving-cell RF measurements, CQI, MCS, BLER, HARQ/RLC
  retransmissions, PRB utilization, scheduler or queue state, QFI/5QI, or packet
  latency. It therefore provides coarse cell context rather than per-request or
  per-UE radio evidence.
- The export does not overlap the stored May and 2026-07-01--06 private-5G
  campaigns, so it cannot be used to explain those results. New application
  runs and ACP exports must be collected over the same time windows.
- `Mean Number of Active UEs` is zero in all 590 rows despite nonzero DRB
  traffic, and the two cells report a simultaneous nonzero fault-unavailability
  interval around 2026-07-23 20:45--21:15. Airspan must clarify the active-UE
  counter, the availability unit and event, the export timezone, and the
  mapping from cell 1/2 to the serving radio configuration before these fields
  are interpreted.

Validation:

- Extracted all six PDF pages as text and visually inspected rendered pages
  containing the configuration template and recommended values.
- Enumerated the archive and parsed all eight raw tab-delimited logs without
  extracting identifier-bearing records into the repository.
- Parsed every DU Cell row, checked interval continuity and duplicate keys,
  measured field coverage, and confirmed that both cells have 295 consecutive
  15-minute rows with no duplicate cell/time bins.
- Confirmed that `CISCO_AIRSPAN_STATS/` is absent from `git status` and locally
  excluded from Git. Confirmed that the mounted filesystem does not enforce the
  attempted owner-only permissions.

### Historical ACP Telemetry Availability

The following windows are derived from the stored application sender
timestamps. They are expressed in America/New_York local time (EDT), rounded to
15-minute ACP boundaries, with one context bin before and after each day's
traffic:

- 2026-05-13 10:45--17:45
- 2026-05-14 09:15--18:00
- 2026-05-15 08:45--18:30
- 2026-05-18 08:45--18:15
- 2026-05-21 13:15--18:00
- 2026-05-22 13:30--17:30
- 2026-07-01 10:30--17:30
- 2026-07-02 08:45--16:15
- 2026-07-03 09:45--10:45
- 2026-07-06 08:00--16:15

Confirm ACP's timezone before matching rows. If ACP only accepts whole dates,
export the complete days May 13, 14, 15, 18, 21, and 22 and July 1, 2, 3, and
6. These historical exports can add coarse cell context only; they cannot
provide per-request or per-UE radio measurements that were not originally
recorded.

ACP currently exposes only recent data, so these historical windows can no
longer be exported. The July 21--24 CSV must not be relabeled or joined as
though it were measured during the May 13--July 6 experiments. Exclusive use of
the radio improves attribution within the CSV's actual collection window, but
it does not establish that radio load, faults, configuration, or counter values
were the same during the earlier experiments.

The current CSV may be used only as a separately dated operational snapshot of
the same deployment, subject to resolving its counter semantics. It includes a
cell-unavailability interval and reports zero mean active UEs despite nonzero
DRB traffic, so it is not yet a validated idle baseline. Historical application
results remain valid at their measured layer, but no historical ACP/RAN
telemetry is available for causal interpretation.

### Required Validation Pilot

1. Confirm the ACP export timezone, the unit of cell-unavailability time, the
   mapping from cell 1/2 to the physical radio configuration, and why the mean
   active-UE counter remains zero when DRB traffic is nonzero.
2. Ask Airspan whether ACP can export shorter intervals or per-UE records and
   which report, if any, provides RSRP/RSRQ/SINR, CQI, MCS, BLER,
   retransmission, PRB-utilization, and QoS-flow fields. Do not describe these
   fields as available until an actual export is obtained.
3. Use three clean consecutive ACP bins with other test UEs removed where
   possible: one idle bin, one controlled uplink-traffic bin, and one controlled
   downlink-traffic bin. Keep traffic inside the selected 15-minute bin, record
   exact start/end times and rates, and export ACP immediately afterward.
4. Run G-NetTrack on a private-5G-capable Android phone beside the gateway for
   the full pilot. Record the phone, experiment-host, and ACP timezone/clock
   offsets, and verify that the current app export contains 5G NR measurements
   rather than assuming the 2018 LTE sample schema applies.
5. Accept the pilot only if the traffic appears in the expected cell and time
   bins, measured DRB volume/throughput tracks the known offered traffic, the
   current G-NetTrack fields and cadence are documented, and unexplained
   counters are resolved or excluded from later analysis.

### Targeted Telemetry Recollection

Do not repeat the full private-5G campaign. Recollect a predeclared subset that
reproduces the representative points needed to relate application behavior to
contemporaneous ACP and G-NetTrack measurements:

1. Stronger-signal, idle network, 1 KiB application payload.
2. Stronger-signal, idle network, 23,968 B detector-output payload.
3. Stronger-signal, 1 KiB payload under the existing 25 Mbps uplink-load
   condition.
4. Stronger-signal, 1 KiB payload with the existing 100-client condition.
5. Weaker-signal, idle network, 1 KiB application payload.
6. Weaker-signal, idle network, 23,968 B detector-output payload.

Use the existing TCP, MQTT, and UDP parameters and working collection scripts.
Where execution time permits, run the three transports sequentially within the
same 15-minute ACP bin so they share the same cell context, while preserving
separate application logs and exact transport start/end times. Use measured
signal strength to label the two signal conditions; placement alone is not the
independent variable.

One bin per condition is sufficient only for a diagnostic check. Collect at
least two bins per condition for a synchronized follow-up dataset; three is
preferred for paper comparisons. With two bins, the six-condition matrix
requires three hours, plus the 45-minute validation pilot. Export ACP and
G-NetTrack at the end of each collection day.

Run the default-versus-new QoS comparison separately, only after Cisco approves
and verifies the second DNN/5QI configuration. Do not repeat the complete
payload sweep, every client count, every load level, failure/restart runs, or
V2X experiments solely to obtain ACP context.

### Experiment Actions

1. Per-request radio context: use a private-5G-capable Android phone with
   G-NetTrack logging beside the gateway and synchronize its clock with the
   experiment host. Treat phone measurements as a co-located proxy, not gateway
   telemetry. Ask whether ACP can export per-UE gateway statistics at finer
   granularity.
2. Controlled signal/CPE comparison: do not change gNodeB power without written
   UD authorization. Run an adjacent outside-versus-inside vehicle comparison
   at one fixed location with unchanged orientation, workload, and radio
   configuration; repeat each condition and log radio context.
3. Optional path decomposition: this is outside the current paper scope. If it
   is reopened later, measure CPE-to-edge application RTT and
   Airspan-to-MX250-connected-laptop ping RTT separately. Do not claim that the
   difference isolates over-the-air delay unless the paths and interfaces are
   shown to be comparable.
4. Verified QoS: after Cisco creates the second DNN/QoS profile, run a targeted
   default-versus-new-profile comparison under idle and controlled uplink load.
   Preserve core-side proof of the actual 5QI flow; IP TOS alone is insufficient.
5. RAN statistics: use ACP Radio DU Cell exports during targeted load,
   multiclient, and QoS comparisons. Verify export interval and per-UE support;
   align or lengthen experiment windows if ACP reports 15-minute aggregates.
   Scheduler statistics and queue occupancy are unavailable from ACP and must
   not be promised.

### Manuscript Actions After User Inputs

- Replace `Cisco 5G` and `Cisco radio/core` with vendor-accurate private-5G
  terminology and add the verified configuration details once supplied.
- Describe the existing weak-signal results without calling them a commercial
  cell edge unless Cisco supplies a deployment-specific coverage definition.
- Clarify that LTE C-V2X PC5 and the private-5G NR Uu path are independent; no
  new V2X experiment is required for this clarification.
- Keep IPI as an enabling contribution while retaining the readiness question
  and three measurement-derived insights as the paper's central logic.
- Scope private-5G conclusions to the measured deployment and add causal
  interpretation only where new radio or RAN evidence supports it.
- Add peer-reviewed DRIVE-SAFE/BREAKING-LOW work when available; the news item
  alone is motivation, not technical evidence.
- Allow coauthors to use review macros temporarily, then remove all strikeout,
  comments, credentials, internal addresses, and review annotations before
  submission.

## Active Task Update: Remaining Experiment Runbook

Task requested 2026-07-24: create a detailed root-level reference document for
every experiment and supporting action that remains.

Status: complete.

Files updated:

- `remaining_exp.md`
- `AGENTS.md`
- `agent_context.md`
- `current_task.md`

The new runbook:

- preserves the distinction between existing application results and
  unavailable historical Airspan telemetry;
- defines the Airspan/G-NetTrack validation pilot;
- reduces the synchronized follow-up to six representative private-5G
  conditions rather than repeating the full campaign;
- specifies common timing, physical controls, application parameters, artifacts,
  redaction, repetitions, acceptance criteria, and end-of-day export steps;
- defines the blocked Cisco-verified QoS and Airspan path-measurement work;
- lists experiments that do not need repetition; and
- provides a completion checklist for future updates.

Validation:

- Checked the required six-condition matrix against the existing compact,
  detector-output, load, multiclient, and weak-signal result families.
- Preserved TCP, MQTT, and UDP as separate application measurements.
- Preserved the approximately two-day Airspan retention constraint and
  15-minute aggregation boundary.
- Added `remaining_exp.md` to the approved root Markdown list and required read
  path so future repository cleanup does not remove it.

Next step:

- Complete R0 and R1 in `remaining_exp.md` before collecting the six
  paper-facing follow-up conditions.

### Execution Host Clarification

Clarification received 2026-07-24:

- `d1` is not connected through the Cisco private-5G gateway.
- Do not run the private-5G application sender, load client, or R1-R5
  experiments on `d1`.
- Run those experiments on the computer in the car behind the Cisco
  private-5G gateway.
- Use `d1` only as a wired receiver or path-comparison endpoint if its physical
  connection to the MX250/Airspan-side network is confirmed.
- A separate authenticated SSH or reverse-SSH connection to the car's
  experiment computer is required before Codex can execute the collection.

## Active Task Update: Review-Comment Closure And Android Logger

Task requested 2026-07-24: re-audit `remaining_exp.md` against the saved
coauthor email chain and specify what must be collected with the user's old
Android phone and installed NetTrack-Lite/G-NetTrack Lite app.

Status: complete.

Files updated:

- `remaining_exp.md`
- `current_task.md`

The runbook now adds:

- a coauthor-comment closure matrix;
- R1A qualification for the exact phone, Android build, private-5G attachment,
  app package/version, fields, sampling behavior, and raw export;
- a precise phone field list and a rule that Lite cannot be used for
  paper-facing collection unless it exports timestamped private-5G NR records;
- explicit separation between co-located phone radio context and gateway,
  per-request, or RAN telemetry;
- deployment/topology documentation, IPI implementation validation,
  claim/citation/submission closure, and ACP availability work;
- host and network-stack telemetry for the synchronized follow-up;
- an R8 directionality check after source review confirmed that current large
  payloads are sent in the vehicle-to-edge request while the returned
  acknowledgment does not carry an equally large response body; and
- explicit alternatives for blocked or unnecessary work, including no gNodeB
  power changes, no full campaign rerun in another vehicle, no unsupported
  over-the-air delay subtraction, and no phone speed test during application
  measurements.

Current evidence relevant to the phone:

- The official G-NetTrack Lite listing says 5G reporting can include RSRP,
  RSRQ, and PCI, while SNR, CQI, and timing advance are listed only for 4G.
- The same listing places text/KML recording and active data tests under Pro
  features while also mentioning Lite log mode, so an actual Lite export must
  be inspected.
- `CISCO_AIRSPAN_STATS/G-NetTrack_Pro_Logs 1.21.zip` is an old 2018 AT&T LTE
  schema example. It does not verify current 5G NR fields or the user's phone.
- No Android device is currently visible through local `adb`, so phone model,
  Android/baseband version, private-network attachment, and an actual export
  remain user-side inputs.

Next step:

1. On the phone, record its exact model, Android/baseband version, app
   name/package/version, and whether the app can create a raw timestamped
   text/CSV export.
2. At the car, verify that the phone attaches to the Airspan private-5G NR cell
   and reports the expected band/cell identity.
3. Collect the five-minute R1A idle qualification log and place the unredacted
   export under `CISCO_AIRSPAN_STATS/` for inspection.
4. Do not start the 45-minute R1 ACP validation pilot until R1A passes.
