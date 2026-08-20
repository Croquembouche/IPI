# Private-5G Experiment Summary

This document summarizes the retained fifth-generation New Radio (5G NR)
experiments in `results/real_5g/`. The experiments measured whether one real
autonomous vehicle could exchange both compact connected-vehicle (CV) messages
and larger connected and automated vehicle (CAV) workloads with an on-site edge
host. They also identified the payload sizes and controlled network conditions
at which the measured path stopped satisfying 100-ms, 500-ms, and 1,000-ms
application deadlines.

## TDD result

Every private-5G application campaign before the August 15--16 diagnostic used
`70/20/10`. Earlier `40/40/20` labels for those campaigns are superseded. The
August 15--16 application round-trip-time (RTT) runs do not support a profile
ranking because the hardware and software path changed during
reconfiguration. Route availability, cell administrative state, and
serving-cell selection varied, while the `40/40/20` response tails changed
substantially across repetitions.

The uplink-capacity control gives a separate result. `40/40/20` assigns twice
the fixed uplink share of `70/20/10` and should provide more uplink capacity
under stable conditions. With a 25-Mbit/s offered vehicle-to-edge TCP rate,
`70/20/10` achieved 13.118 Mbit/s and the coauthor-reported `40/40/20` control
achieved 25.000 Mbit/s. The machine-readable status record is
`results/real_5g/tdd_comparison_status.json`, and the detailed comparison is
`paper/analysis/tdd_experiment_update.md`.

## Experimental definition

| Item | Experimental definition |
|---|---|
| Measured path | The autonomous-vehicle computer connected by 1-Gbit/s Ethernet to a Cisco Meraki MG52-HW gateway. The MG52 used the 5G NR radio interface between user equipment and the gNodeB (Uu) to reach a dedicated Airspan AirSpeed 2900 radio and the Cisco private-5G core. An on-site MX250 connected the core to the `d1` workload responder. The remote MX68CW served network-management and security functions and was not the application endpoint. |
| Controlled environment | The CAV gateway was the sole physical user equipment (UE). The Airspan radio and the full 40-MHz n48 channel were dedicated to the experiment, and the n48 band was clean. Thus, the baseline had no ambient users or uncontrolled contention. Every load stream and logical client reported below was deliberately introduced. |
| Radio configuration | 5G NR band n48; 40-MHz channel; 30-kHz subcarrier spacing; time-division duplexing (TDD); 20-ms synchronization-block period; 33-dBm cell transmit power; 34 dBm/MHz effective isotropic radiated power; and 17-dBi integrated antenna. |
| TDD and signal contexts | All application campaigns before August 15--16 used the operator-reported 70/20/10 downlink/uplink/dynamic allocation with 10D4G frame packing. The August 15--16 diagnostic changed between 40/40/20 and 70/20/10, but hardware/software path state did not remain stable enough to estimate an application-latency effect. The unsupported 30/60/10 attempt produced no valid experiment. |
| Application workload | The vehicle placed the selected workload bytes in an Intersection Programming Interface (IPI) cooperative-service request. The edge host decoded the frame and immediately returned the corresponding IPI response. For example, a 1,024-B workload produced a 1,096-B encoded service frame in the load experiments. |
| Workload direction | The selected CV or CAV workload traveled from the vehicle to the edge host. The return path carried the corresponding response. Therefore, the large-payload results evaluate vehicle-to-edge workload requests, not equally large edge-to-vehicle objects. |
| Timing boundary | Round-trip time (RTT) began after request encoding, immediately before the send or publish call, and ended when the vehicle received the corresponding response. The clocks were not synchronized, so RTT is the valid communication-latency measure. Stored one-way columns must not be interpreted as uplink or downlink latency. |
| Edge contribution | The edge host was directly connected to the MX250 and returned the response immediately after decoding. The experiments did not insert a perception, planning, or remote-assistance computation workload. Consequently, the results measure the communication exchange rather than edge-processing time. |
| Transports | Transmission Control Protocol (TCP), Message Queuing Telemetry Transport (MQTT) 3.1.1 over TCP, raw User Datagram Protocol (UDP), and application-fragmented UDP with each datagram kept below the maximum transmission unit (MTU). |
| Main trial cadence | The principal stationary campaigns issued 1,000 sequential requests per condition with a 200-ms interval. The locked-cell follow-up issued 500 sequential requests per repetition and used two repetitions per transport and condition. Multiclient trials issued 1,000 requests per logical client. |
| Response success | A successful attempt returned the expected response. Success percentages use every issued attempt. RTT percentiles use successful responses only. |
| Deadline availability | An attempt is available by deadline `B` only when it returned the expected response and its RTT was no greater than `B`. Timeouts and responses later than `B` are both deadline misses. |
| Logical demand | A logical client is an independent application flow behind the same vehicle computer and MG52. Increasing the logical-client count increases application demand but does not add independently scheduled radio UEs. |
| Signal evidence | A separate iPhone collected the 16-point reference signal received power (RSRP), reference signal received quality (RSRQ), and signal-to-noise ratio (SNR) survey. These measurements provide spatial context; they are not per-request MG52 radio telemetry. |
| Quality-of-service evidence | `5qi-mapped` was an application metadata label. Vehicle packet captures showed Internet Protocol (IP) Type of Service (TOS) `0x0` for both default and labeled traffic. Thus, the data do not establish a network-enforced Quality of Service (QoS) treatment through a standardized 5G QoS Identifier (5QI). |

## Experiment coverage

| Experiment family | Controlled variables | Main question |
|---|---|---|
| Stationary baseline and payload sweeps | TCP/MQTT, 0 B to 2 MiB, five collection campaigns | How does communication latency change from compact CV messages to large CAV state? |
| Signal survey and stationary locations | Sixteen handset survey points and Global Navigation Satellite System (GNSS) run locations | How much spatial radio variation existed around the measured deployment? |
| Deliberate uplink load | Idle versus achieved background traffic; one, two, and four streams | Does competing vehicle-to-edge traffic change foreground response latency? |
| Quality-of-service label control | Default versus `5qi-mapped` application label and packet capture | Was a nondefault bearer or packet treatment verified? |
| Detector-output replay | Workloads derived from 922 detector outputs; TCP, MQTT, raw UDP, and fragmented UDP | Can the path carry representative object-list sizes, and how does message formation affect delivery? |
| Deadline analysis | 100-, 120-, 400-, 500-, 1,000-, and 5,000-ms thresholds | Which accepted exchanges arrive before the application must act? |
| Logical-client demand | 1, 2, 5, 10, 20, 50, and 100 application clients behind one UE | How does concurrent application demand affect latency and response loss? |
| Failure and restart | TCP/UDP/MQTT receiver restart and MQTT broker restart | How long does the communication service remain unavailable after a controlled interruption? |
| TDD application and capacity diagnostics | August 15--16 vehicle-to-edge IPI request/acknowledgment runs; 25-Mbit/s uplink capacity controls | Can the two profiles be ranked by application latency, and does the larger uplink share improve sustained capacity? |
| Locked-cell location follow-up | Four workload conditions at common, weak-candidate, and strong-candidate locations | Are response latency and availability stable across controlled stationary placements? |

## Deadline availability across the collected workload

This table is the most direct answer to the application-readiness question.
Every percentage is the share of all issued attempts that both returned a
response and completed within the stated deadline. A high response-success
rate does not imply a high deadline-availability rate.

| Evidence group | Attempts | Response success | p95 RTT (ms) | Within 100 ms | Within 500 ms | Within 1,000 ms |
|---|---:|---:|---:|---:|---:|---:|
| Signal Phase and Timing (SPaT) / state mirror | 2,000 | 100.00% | 130.323 | 53.40% | 100.00% | 100.00% |
| Compact service, at most 4 KiB | 60,000 | 100.00% | 413.725 | 37.76% | 96.29% | 99.45% |
| 1-KiB service under deliberate load or QoS-label conditions | 34,000 | 100.00% | 602.354 | 24.12% | 93.47% | 99.04% |
| Detector-output replay | 20,100 | 99.54% | 245.879 | 42.86% | 99.32% | 99.54% |
| Multiclient 1-KiB service | 564,000 | 99.79% | 233.349 | 50.92% | 99.51% | 99.73% |
| Mid-sized payload, 8--64 KiB | 40,100 | 99.78% | 199.793 | 70.37% | 99.66% | 99.78% |
| Large state, 128--512 KiB | 18,000 | 100.00% | 667.975 | 0.99% | 88.36% | 99.27% |
| Bulk state, at least 1 MiB | 14,000 | 100.00% | 3,122.273 | 0.00% | 0.00% | 37.39% |

The path eventually returned every TCP/MQTT response in many large-payload
conditions. Nevertheless, the communication time increased from tens or
hundreds of milliseconds for compact objects to seconds for MiB-scale objects.
The application's deadline therefore separates an eventual transfer from a
usable exchange.

## Stationary payload envelope

The pooled baseline table shows the payload transition without mixing it with
deliberate load, logical clients, or restart events.

| Requested workload | TCP p95 RTT (ms) | MQTT p95 RTT (ms) | Measured deadline boundary |
|---:|---:|---:|---|
| 0 B | 141.499 | 39.724 | MQTT commonly completed by 100 ms; TCP required a longer deadline. |
| 1 KiB | 156.069 | 46.768 | The pooled p95 was below 500 ms for both transports and below 100 ms for MQTT. |
| 4 KiB | 119.639 | 59.905 | The pooled p95 was below 500 ms for both transports and below 100 ms for MQTT. |
| 16 KiB | 146.132 | 104.834 | The 100-ms boundary became marginal; the 500-ms envelope remained feasible. |
| 64 KiB | 209.497 | 187.816 | The pooled p95 exceeded 100 ms but remained below 500 ms. |
| 128 KiB | 321.223 | 309.048 | The pooled p95 remained below 500 ms. |
| 256 KiB | 697.092 | 962.579 | The p95 moved beyond 500 ms and approached 1 s. |
| 512 KiB | 2,949.865 | 1,031.485 | The 1-s deadline was no longer reliable. |
| 1 MiB | 1,867.666 | 2,702.546 | RTT was seconds-scale. |
| 2 MiB | 3,632.799 | 2,536.359 | RTT was seconds-scale. |

The five collections also show that payload size is not the only determinant.
The same payload experienced very different service times at different
stationary placements and collection periods.

| Collection | Compact 0--4-KiB p95 range, MQTT / TCP (ms) | 256-KiB p95, MQTT / TCP (ms) | 512-KiB p95, MQTT / TCP (ms) | 1-MiB p95, MQTT / TCP (ms) | 2-MiB p95, MQTT / TCP (ms) |
|---|---:|---:|---:|---:|---:|
| May 13 | 36.1--49.5 / 99.8--140.2 | 578.218 / 466.062 | 1,039.419 / 834.545 | 1,455.390 / 2,061.018 | 2,536.359 / 3,632.799 |
| May 14 | 53.7--124.4 / 119.8--155.7 | 1,154.463 / 1,297.066 | 2,672.859 / 2,848.504 | 5,013.245 / 4,872.325 | not completed / 10,767.201 |
| May 15, variable path | 509.7--1,485.8 / 1,028.7--2,044.6 | 43,835.616 / 9,465.676 | 41,816.808 / 24,493.791 | not completed | not completed |
| May 21 | 40.3--79.7 / 136.3--171.0 | 1,156.083 / 784.454 | 1,085.127 / 3,598.768 | 3,018.356 / 1,255.798 | not tested |
| May 22 | 38.0--47.7 / 119.6--155.3 | 378.398 / 420.511 | 761.387 / 982.803 | 1,406.789 / 1,465.084 | not tested |

The May 14 1-MiB MQTT row was also a partial collection with 98 responses from
99 attempts. The May 15 256-KiB MQTT condition accepted 999 of 1,000 attempts.
Its 512-KiB MQTT and TCP rows were partial collections with 309 responses from
309 attempts and 291 responses from 292 attempts, respectively. These rows
characterize their collected intervals but are not full 1,000-attempt
conditions.

## Representative detector-output workload

The detector workload was derived from 922 samples using
`256 + 96 * predicted_object_count` bytes. The resulting distribution was
13,216 B minimum, 19,648 B p50, 22,816 B p95, 23,968 B p99, and 25,024 B
maximum. Thus, this campaign evaluated a measured object-list distribution
rather than an arbitrary set of packet sizes.

| Location / formation | Transport | 0-B responses; p95 | 23,968-B responses; p95 | 60,000-B responses; p95 |
|---|---|---:|---:|---:|
| Favorable stationary | MQTT | 1,000/1,000; 49.826 ms | 1,000/1,000; 183.705 ms | 1,000/1,000; 403.790 ms |
| Favorable stationary | TCP | 1,000/1,000; 151.696 ms | 1,000/1,000; 169.540 ms | 1,000/1,000; 381.409 ms |
| Favorable stationary, application-fragmented | UDP | 997/1,000; 51.661 ms | 995/1,000; 139.705 ms | 20/100; 194.395 ms |
| Weak-location repeat | MQTT | 1,000/1,000; 71.866 ms | 1,000/1,000; 423.936 ms | 1,000/1,000; 770.058 ms |
| Weak-location repeat | TCP | 1,000/1,000; 205.873 ms | 1,000/1,000; 301.906 ms | 1,000/1,000; 661.814 ms |
| Weak-location repeat, application-fragmented | UDP | 999/1,000; 69.966 ms | 948/1,000; 180.396 ms | 0/100; -- |

For a 500-ms p95 application budget, every evaluated TCP and MQTT detector
condition through 19,648 B remained below the deadline at both the favorable
and weak locations. Weak-location MQTT reached 539.780 ms at 22,816 B, and the
larger detector points varied non-monotonically. Approximately 20 KiB of
application content before transport framing is therefore the conservative
measured ceiling for a monotonic 500-ms p95 guarantee on this path. The result
was obtained with one UE, a dedicated 40-MHz n48 channel, and no ambient
contention.

Raw UDP and application-fragmented UDP produced different outcomes. The
encoded 1,400-B workload became an approximately 1,640-B IP datagram, which
exceeded the 1,500-B path maximum transmission unit (MTU). Raw UDP returned all
0-B and 1,024-B probes but returned 0/45 at 1,400 B, 0/100 at 4,096 B, and
0/100 at 19,648 B. Splitting each logical object into MTU-safe datagrams
restored 99.5--100.0% response success through 23,968 B at the favorable
location. However, the number of fragments made the 60,000-B object fragile:
success fell to 20/100 at the favorable location and 0/100 at the weak-location
repeat.

## Deliberate load

The load generator targeted 25 Mbit/s, but the foreground result must be
paired with the achieved rate. TCP backpressure and the measured path reduced
the vehicle-side rate substantially below the configured target.

| Campaign | Transport | Idle p95 RTT (ms) | Loaded p95 RTT (ms) | Achieved vehicle-side background rate |
|---|---|---:|---:|---:|
| Favorable repeat 1 | TCP | 193.832 | 419.831 | 1.320 Mbit/s |
| Favorable repeat 1 | MQTT | 60.233 | 173.777 | 1.624 Mbit/s |
| Favorable repeat 2 | TCP | 161.851 | 387.992 | 2.265 Mbit/s |
| Favorable repeat 2 | MQTT | 52.063 | 175.905 | 2.267 Mbit/s |
| Favorable 1--4-stream sweep | TCP | -- | 391.450--409.887 | 4.371--5.602 Mbit/s |
| Favorable 1--4-stream sweep | MQTT | -- | 169.640--467.762 | 1.990--5.743 Mbit/s |
| Weak-location repeat 1, default label | TCP | 668.347 | 681.939--933.823 | 0--0.262 Mbit/s in completed client summaries |
| Weak-location repeat 1, default label | MQTT | 551.800 | 637.832--769.656 | 0--0.251 Mbit/s in completed client summaries |
| Weak-location repeat 2, default label | TCP | 150.083 | 629.612--758.947 | Preserved load logs; no consolidated achieved-rate row |
| Weak-location repeat 2, default label | MQTT | 159.750 | 460.498--707.914 | Preserved load logs; no consolidated achieved-rate row |

All foreground probes in these load rows returned responses. The change is in
communication latency: deliberate competing traffic increased p95 RTT even
though the radio served only one physical UE. The stream-count sweep was not
monotonic in achieved rate, so measured Mbit/s, rather than the configured
stream count alone, defines each load condition.

The `5qi-mapped` sweeps produced p95 RTTs of 430.775--771.833 ms for TCP and
376.994--529.321 ms for MQTT. However, both the default and `5qi-mapped`
packet captures showed TOS `0x0`. These rows are load observations with an
application label; they are not evidence of nondefault 5QI treatment.

## Logical-client demand behind one physical UE

Each logical client sent a 1-KiB workload through the same vehicle computer and
MG52. This design tests concurrent CAV application demand while holding the
physical UE count at one.

| Placement | Transport | Logical clients | Responses / attempts | p50 RTT (ms) | p95 RTT (ms) | p99 RTT (ms) |
|---|---|---:|---:|---:|---:|---:|
| Favorable | MQTT | 1 | 1,000/1,000 | 30.368 | 40.551 | 47.028 |
| Favorable | MQTT | 50 | 50,000/50,000 | 37.829 | 54.056 | 82.022 |
| Favorable | MQTT | 100 | 100,000/100,000 | 41.774 | 81.850 | 121.672 |
| Favorable | TCP | 1 | 1,000/1,000 | 121.132 | 143.829 | 152.695 |
| Favorable | TCP | 50 | 50,000/50,000 | 133.845 | 166.067 | 189.781 |
| Favorable | TCP | 100 | 100,000/100,000 | 187.687 | 264.070 | 305.725 |
| Favorable | UDP | 1 | 1,000/1,000 | 30.383 | 40.542 | 48.232 |
| Favorable | UDP | 50 | 49,986/50,000 | 31.966 | 53.967 | 79.634 |
| Favorable | UDP | 100 | 99,965/100,000 | 46.097 | 113.391 | 139.843 |
| Weak-location | MQTT | 1 | 1,000/1,000 | 39.870 | 91.782 | 129.790 |
| Weak-location | MQTT | 50 | 49,998/50,000 | 173.945 | 1,117.707 | 1,949.906 |
| Weak-location | MQTT | 100 | 99,499/100,000 | 909.719 | 5,135.984 | 21,776.949 |
| Weak-location | TCP | 1 | 1,000/1,000 | 129.801 | 215.653 | 312.652 |
| Weak-location | TCP | 50 | 50,000/50,000 | 316.558 | 769.911 | 1,593.879 |
| Weak-location | TCP | 100 | 100,000/100,000 | 405.818 | 2,908.420 | 6,416.896 |
| Weak-location | UDP | 1 | 995/1,000 | 39.477 | 82.047 | 114.188 |
| Weak-location | UDP | 50 | 48,364/50,000 | 83.560 | 169.870 | 180.362 |
| Weak-location | UDP | 100 | 90,811/100,000 | 101.949 | 172.500 | 181.912 |

The favorable run carried 100 logical clients with high response success, but
the weak-location run exposed different failure modes. MQTT and TCP retained
most or all responses while their tails expanded into seconds. Fragmented UDP
kept lower received-response RTT but lost 9,189 of 100,000 responses at 100
clients. Thus, received-response latency and response availability must be
reported together.

## TDD application latency and uplink capacity

The corrected profile record places every application campaign before August
15--16 under `70/20/10`. Their location-associated results remain part of the
signal analysis and no longer enter a TDD comparison.

The August 15--16 diagnostic uploaded exact 1-, 10-, and 100-KiB IPI requests
over TCP and returned compact correlated acknowledgments. The `70/20/10` p95
RTTs changed by only 0.61%, 0.22%, and 1.29% across the two dates. The
`40/40/20` TCP p95 changes between the August 15 reference and the first August
16 run were 3.33%, 27.97%, and 169.05%. A second August 16 run changed those
tails by another 189.83%, 104.97%, and 17.52%. MQTT p95 under `40/40/20`
changed by 9.22--163.86% as the payload increased from 1 KiB to 1 MiB.

The radio and software path did not remain stable across these measurements.
The route required recovery, Cell 1 changed administrative state, and the
second `40/40/20` TCP repetition used Cell 1 instead of Cell 2. The
application RTT observations therefore cannot identify which TDD allocation
provides lower communication latency.

The capacity control isolates a different property. At a 25-Mbit/s offered
vehicle-to-edge TCP rate, `70/20/10` achieved 13.118 Mbit/s. The
coauthor-reported `40/40/20` control achieved 25.000 Mbit/s. This direction is
consistent with the fixed uplink allocation increasing from 20 to 40 percent.
The raw `70/20/10` trace is stored under
`results/real_5g/20260805_airspan_r1_run_1/`; the raw `40/40/20` trace is not
present in the current repository.

## Controlled interruption

Each condition sent 1,000 1-KiB requests, triggered one failure 60 s after the
sender started, and waited 10 s before issuing the restart command.

| Failure mode | Transport | Responses / attempts | p50 RTT (ms) | p95 RTT (ms) | p99 RTT (ms) | Gap between successful responses |
|---|---|---:|---:|---:|---:|---:|
| Receiver restart | TCP | 953/1,000 | 126.629 | 161.464 | 361.228 | 11,714.7 ms |
| Receiver restart | UDP | 969/1,000 | 30.305 | 44.139 | 63.027 | 13,463.5 ms |
| Receiver restart | MQTT | 991/1,000 | 31.977 | 49.855 | 259.906 | 13,367.4 ms |
| Broker restart | MQTT | 955/1,000 | 31.644 | 47.871 | 90.553 | 15,100.7 ms |

The p50 values stayed near their steady-state levels because most attempts
occurred outside the injected event. The all-attempt response counts and the
11.7--15.1-s success gaps expose the interruption directly.

## Locked-cell location follow-up

The follow-up repeated two idle workloads at three stationary placements:
C1 was a 1-KiB request from one logical client, and C2 was a 23,968-B request
from one logical client. The medium/typical block also included C3, a 1-KiB
request under a 25-Mbit/s offered uplink load, and C4, 100 logical 1-KiB
clients. Each transport and condition used two 500-attempt repetitions.

| Location block | Conditions | Valid samples | Attempts | Responses | Failed | Response success |
|---|---|---:|---:|---:|---:|---:|
| Medium/typical | C1--C4 | 24/24 | 309,000 | 307,662 | 1,338 | 99.566990% |
| Weak candidate | C1--C2 | 12/12 | 6,000 | 5,994 | 6 | 99.900000% |
| Common/typical repeat | C1--C2 | 12/12 | 6,000 | 5,999 | 1 | 99.983333% |
| Strong candidate | C1--C2 | 12/12 | 6,000 | 6,000 | 0 | 100.000000% |

The medium C4 condition returned every one of the 100,000 TCP and 100,000 MQTT
responses and 98,667 of 100,000 UDP responses. The six weak-candidate failures
and the one common-repeat failure were C2 UDP response timeouts.

Combining the two common C1/C2 blocks as a descriptive reference, the
strong-candidate placement had lower p95 RTT in every matched aggregate:

| Workload | Transport | Common p95 RTT (ms) | Strong-candidate p95 RTT (ms) | Change |
|---|---|---:|---:|---:|
| C1, 1 KiB | TCP | 149.913 | 137.309 | -8.41% |
| C1, 1 KiB | MQTT | 41.632 | 39.955 | -4.03% |
| C1, 1 KiB | UDP | 40.347 | 39.200 | -2.84% |
| C2, 23,968 B | TCP | 106.971 | 93.605 | -12.49% |
| C2, 23,968 B | MQTT | 75.752 | 59.721 | -21.16% |
| C2, 23,968 B | UDP | 70.541 | 56.947 | -19.27% |

The location labels came from a separate iPhone survey, not the MG52. The
matched application results establish spatial variation in the measured RTT;
they do not assign that variation to a specific MG52 RSRP, modulation and
coding scheme, scheduler decision, or retransmission count.

## Separate-UE signal survey

| Metric | Samples | Minimum | Mean | Maximum |
|---|---:|---:|---:|---:|
| RSRP | 16 | -121.00 dBm | -106.88 dBm | -91.00 dBm |
| RSRQ | 16 | -19.00 dB | -11.00 dB | -10.00 dB |
| SNR | 16 | 3.50 dB | 16.44 dB | 27.50 dB |

These measurements document the deployment's spatial signal range. They must
not be joined to individual MG52 requests as if they were synchronized vehicle
radio measurements.

## Directly supported findings

| Comparison | Result shown by the experiment |
|---|---|
| Compact versus large application state | The clean single-UE path returned compact IPI workloads in tens to hundreds of milliseconds. MiB-scale requests consumed seconds of communication time. Consequently, the same path can support a compact update yet miss the deadline of a larger stateful exchange. |
| Eventual response versus deadline completion | Several large-payload groups achieved 100% response success while failing most short deadlines. Application support must therefore use all-attempt deadline availability, not response success or received-response RTT alone. |
| Message representation | Raw UDP failed once the encoded datagram exceeded the path MTU. Application fragmentation restored detector-sized delivery, but the 60-KiB object remained loss-prone because every fragment was required to reconstruct one logical request. |
| Competing application traffic | Deliberately introduced uplink traffic increased foreground p95 RTT even though no ambient users shared the network. One UE can therefore generate internal contention among simultaneous CAV applications. |
| Physical UE count versus service demand | Increasing logical clients behind the sole UE expanded MQTT/TCP tails and UDP response loss. A network sees one radio UE while the vehicle may expose many concurrent deadline-bound application flows. |
| TDD application latency and uplink capacity | Hardware/software path changes and large same-profile tail variation make the August 15--16 application RTT runs unusable for TDD ranking. The capacity controls followed the expected uplink-allocation direction: 13.118 Mbit/s under 70/20/10 and 25.000 Mbit/s under 40/40/20 against the same 25-Mbit/s offer. |
| Interruption | A configured 10-s restart produced 11.7--15.1-s gaps between successful responses. Recovery time, rather than steady-state median RTT, determines whether the communication service remains usable during failure. |
| Spatial placement | Matched C1/C2 aggregates changed across stationary placements. Because the survey handset was a separate UE, the experiments establish location-associated application variation without identifying a specific MG52 radio-layer cause. |

## Source artifacts

| Evidence | Stored artifact |
|---|---|
| Consolidated experiment map | `experiment_summary.md` |
| TDD decision and comparison | `results/real_5g/tdd_comparison_status.json` and `paper/analysis/tdd_experiment_update.md` |
| Stationary payload campaigns | `results/real_5g/20260513_sunny_fintechparking_run_1/`, `20260514_sunny_after_rain_run_1/`, `20260515_sunny_run_1/`, `20260521_small_rain_run_1/`, and `20260522_cloudy_run_1/` |
| Signal survey and run locations | `results/real_5g/signal_maps_summary.md`, `signal_measurements_parsed.csv`, and `run_locations_parsed.csv` |
| Load and QoS-label campaigns | `results/real_5g/20260701_load_qos_*/`, `20260706_load_qos_weak_signal_run_2/`, and `20260701_qos_verification_run_1/` |
| Detector-output TCP/MQTT | `results/real_5g/20260702_detector_output_to_ipi_run_1/summary.md` and `20260706_detector_output_to_ipi_weak_signal_tcp_mqtt_run_1/summary.md` |
| Raw and fragmented UDP | `results/real_5g/20260702_detector_output_to_ipi_udp_run_2/`, `20260702_detector_output_to_ipi_udp_fragmented_run_1/summary.md`, and `20260706_detector_output_to_ipi_weak_signal_udp_fragmented_run_1/summary.md` |
| Deadline analysis | `results/real_5g/20260702_end_to_end_deadline_analysis_run_1/summary.md` |
| Logical-client demand | `results/real_5g/20260702_multiclient_scalability_run_2/`, `run_3/`, `run_4/`, and `20260706_multiclient_scalability_weak_signal_run_1/` |
| Controlled interruption | `results/real_5g/20260703_failure_fallback_run_1/summary.md` |
| Direction and locked-cell follow-up | `results/real_5g/20260805_airspan_r1_run_1/` and the four `results/real_5g/20260806_airspan_followup_*/` folders |
| August 15--16 TDD application diagnostic | `results/real_5g/20260815_airspan_tdd_raw_uplink_70_20_10_location_1_run_2/` and the `results/real_5g/20260816_airspan_tdd_raw_uplink_40_40_20_*` and `20260816_airspan_tdd_raw_uplink_70_20_10_*` folders |
| Cross-condition machine-readable metrics | `paper/analysis/limiting_factors/condition_level_metrics.csv` |
