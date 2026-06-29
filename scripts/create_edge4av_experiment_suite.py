#!/usr/bin/env python3
"""Create starter scripts and instructions for the full Edge4AV experiment suite."""

from __future__ import annotations

import argparse
import csv
import json
import os
import stat
import textwrap
from datetime import datetime, timezone
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_RESULTS_ROOT = REPO_ROOT / "results" / "edge4av_top_tier"


EXPERIMENTS = [
    {
        "id": "01_broadcast_message_baseline",
        "title": "Broadcast V2X message baseline",
        "minimum_repeats": 5,
        "target_probes": "1000 per message/rate condition",
        "purpose": "Measure compact broadcast-plane latency and PRR for SPaT, MAP, BSM, and TIM where lab tooling is available.",
    },
    {
        "id": "02_radio_distance_mobility",
        "title": "Radio distance, LOS/NLOS, and mobility",
        "minimum_repeats": 5,
        "target_probes": "1000 per distance/mobility condition",
        "purpose": "Measure how radio RTT/PRR changes across distance, obstruction, and vehicle motion.",
    },
    {
        "id": "03_broadcast_contention",
        "title": "Broadcast contention and mixed-message load",
        "minimum_repeats": 5,
        "target_probes": "1000 target probes while background senders run",
        "purpose": "Quantify compact-awareness robustness under mixed SPaT, BSM, MAP, TIM, and custom-channel load.",
    },
    {
        "id": "04_private5g_mobility",
        "title": "Private-5G mobility drive-through",
        "minimum_repeats": 10,
        "target_probes": "1000 per transport/payload/route pass",
        "purpose": "Measure private-5G RTT tails while the vehicle approaches, stops at, and leaves the intersection.",
    },
    {
        "id": "05_handover_weak_signal",
        "title": "Handover and weak-signal behavior",
        "minimum_repeats": 10,
        "target_probes": "1000 per edge-of-coverage or handover pass",
        "purpose": "Capture timeout, loss, and recovery behavior near the weakest signal regions or during handover.",
    },
    {
        "id": "06_load_qos_stress",
        "title": "Controlled private-5G load and QoS stress",
        "minimum_repeats": 5,
        "target_probes": "1000 latency probes per load/QoS condition",
        "purpose": "Measure RTT under idle, uplink load, downlink load, bidirectional load, and near-saturation.",
    },
    {
        "id": "07_transport_comparison",
        "title": "TCP, MQTT, and UDP transport comparison",
        "minimum_repeats": 5,
        "target_probes": "1000 per transport/payload condition",
        "purpose": "Compare common session implementations while holding the IPI payload model constant.",
    },
    {
        "id": "08_end_to_end_deadline",
        "title": "End-to-end application deadline experiment",
        "minimum_repeats": 10,
        "target_probes": "1000 service requests per deadline/application condition",
        "purpose": "Measure deadline-hit rate for a concrete guided-planning or signal-advisory service.",
    },
    {
        "id": "09_detector_output_to_ipi",
        "title": "Detector-output-to-IPI payload experiment",
        "minimum_repeats": 5,
        "target_probes": "1000 per detector-output payload condition",
        "purpose": "Transport detector-output summaries rather than raw sensor artifacts and compare deadline feasibility.",
    },
    {
        "id": "10_edge_offload_tradeoff",
        "title": "Edge offload tradeoff",
        "minimum_repeats": 5,
        "target_probes": "At least 100 detector/application samples per compute-placement condition",
        "purpose": "Compare onboard-only, edge-only, and hybrid compute-placement choices.",
    },
    {
        "id": "11_multiclient_scalability",
        "title": "Multi-client scalability and fairness",
        "minimum_repeats": 5,
        "target_probes": "1000 probes per client at each concurrency level",
        "purpose": "Measure fairness, tail growth, receiver pressure, and broker behavior as vehicle sessions scale.",
    },
    {
        "id": "12_failure_fallback",
        "title": "Failure and fallback behavior",
        "minimum_repeats": 5,
        "target_probes": "1000 probes per failure/fallback condition",
        "purpose": "Measure behavior during broker restart, receiver outage, weak signal, and fallback to compact/broadcast modes.",
    },
]


def sh_header(title: str) -> str:
    return textwrap.dedent(
        f"""\
        #!/usr/bin/env bash
        set -euo pipefail

        # {title}
        # Edit the ROLE/IP/condition variables below for your lab before running.

        REPO_ROOT="${{REPO_ROOT:-{REPO_ROOT}}}"
        RUN_ID="${{RUN_ID:-edge4av-$(date -u +%Y%m%dT%H%M%SZ)}}"
        OUT_ROOT="${{OUT_ROOT:-$REPO_ROOT/results/edge4av_top_tier/$RUN_ID}}"
        mkdir -p "$OUT_ROOT"
        cd "$REPO_ROOT"

        """
    )


def write_script(path: Path, body: str) -> None:
    path.write_text(body, encoding="utf-8")
    mode = path.stat().st_mode
    path.chmod(mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)


def common_private5g_vars() -> str:
    return textwrap.dedent(
        """\
        RECEIVER_HOST="${RECEIVER_HOST:-10.100.100.6}"
        TCP_PORT="${TCP_PORT:-36666}"
        UDP_PORT="${UDP_PORT:-36667}"
        MQTT_HOST="${MQTT_HOST:-10.100.100.6}"
        MQTT_PORT="${MQTT_PORT:-1883}"
        COUNT="${COUNT:-1000}"
        INTERVAL_MS="${INTERVAL_MS:-200}"
        PAYLOAD_BYTES="${PAYLOAD_BYTES:-1024}"
        CLOCK_SYNC_STATE="${CLOCK_SYNC_STATE:-unsynced}"
        AV_ID="${AV_ID:-av-1}"
        OBU_ID="${OBU_ID:-obu-1}"
        RSU_ID="${RSU_ID:-rsu-1}"
        SOURCE_ID="${SOURCE_ID:-veh-01}"
        INTERSECTION_ID="${INTERSECTION_ID:-intersection-101}"

        latency_host() {
          if [[ "$TRANSPORT" == "mqtt" ]]; then
            echo "$MQTT_HOST"
          else
            echo "$RECEIVER_HOST"
          fi
        }

        latency_port() {
          if [[ "$TRANSPORT" == "mqtt" ]]; then
            echo "$MQTT_PORT"
          elif [[ "$TRANSPORT" == "udp" ]]; then
            echo "$UDP_PORT"
          else
            echo "$TCP_PORT"
          fi
        }

        """
    )


def script_01() -> str:
    return sh_header("01 broadcast message baseline") + textwrap.dedent(
        """\
        ROLE="${ROLE:-initiator}" # responder|initiator|spat_sender
        RADIO_NODE_ID="${RADIO_NODE_ID:-v2x-tx-radio}"
        PEER_NODE_ID="${PEER_NODE_ID:-v2x-rx-radio}"
        COUNT="${COUNT:-1000}"
        INTERVAL_MS="${INTERVAL_MS:-100}"
        PAYLOAD_BYTES="${PAYLOAD_BYTES:-128}"
        BRIDGE_HOST="${BRIDGE_HOST:-192.168.253.40}"
        BRIDGE_PORT="${BRIDGE_PORT:-35555}"
        DISTANCE_M="${DISTANCE_M:-0}"
        MESSAGE_CLASS="${MESSAGE_CLASS:-spat}"
        CONDITION_ID="${CONDITION_ID:-broadcast-${MESSAGE_CLASS}-${DISTANCE_M}m-baseline}"

        case "$ROLE" in
          responder)
            cd third_party/mocar/J2735-2020/samples/ipi_custom_rtt
            exec ./ipi_custom_rtt --role responder --node-id "$PEER_NODE_ID" 2>&1 | tee "$OUT_ROOT/${CONDITION_ID}_responder.log"
            ;;
          initiator)
            cd third_party/mocar/J2735-2020/samples/ipi_custom_rtt
            ./ipi_custom_rtt --role initiator --node-id "$RADIO_NODE_ID" --count "$COUNT" --interval-ms "$INTERVAL_MS" --payload-bytes "$PAYLOAD_BYTES" --csv \
              > "$OUT_ROOT/${CONDITION_ID}_rtt.csv"
            ;;
          spat_sender)
            ./cpp/build/example_spat_tcp_sender --host "$BRIDGE_HOST" --port "$BRIDGE_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" \
              --condition-label radio-baseline --request-id "$CONDITION_ID" --network-load-level idle --mobility-state stationary --clock-sync-state ptp-synced \
              --csv > "$OUT_ROOT/${CONDITION_ID}_spat_sender.csv"
            ;;
          *)
            echo "ROLE must be responder, initiator, or spat_sender" >&2
            exit 2
            ;;
        esac
        """
    )


def script_02() -> str:
    return sh_header("02 radio distance mobility") + textwrap.dedent(
        """\
        ROLE="${ROLE:-initiator}" # responder|initiator
        RADIO_NODE_ID="${RADIO_NODE_ID:-v2x-tx-radio}"
        PEER_NODE_ID="${PEER_NODE_ID:-v2x-rx-radio}"
        DISTANCE_M="${DISTANCE_M:-50}"
        LINK_STATE="${LINK_STATE:-los}" # los|nlos
        MOBILITY_STATE="${MOBILITY_STATE:-stationary}" # stationary|moving-5mph|moving-15mph
        COUNT="${COUNT:-1000}"
        INTERVAL_MS="${INTERVAL_MS:-100}"
        PAYLOAD_BYTES="${PAYLOAD_BYTES:-128}"
        CONDITION_ID="${CONDITION_ID:-radio-rtt-${DISTANCE_M}m-${LINK_STATE}-${MOBILITY_STATE}}"

        cd third_party/mocar/J2735-2020/samples/ipi_custom_rtt
        if [[ "$ROLE" == "responder" ]]; then
          exec ./ipi_custom_rtt --role responder --node-id "$PEER_NODE_ID" 2>&1 | tee "$OUT_ROOT/${CONDITION_ID}_responder.log"
        fi
        ./ipi_custom_rtt --role initiator --node-id "$RADIO_NODE_ID" --count "$COUNT" --interval-ms "$INTERVAL_MS" --payload-bytes "$PAYLOAD_BYTES" --csv \
          > "$OUT_ROOT/${CONDITION_ID}_samples.csv"
        cat > "$OUT_ROOT/${CONDITION_ID}_metadata.json" <<META
        {"run_id":"$RUN_ID","condition_id":"$CONDITION_ID","distance_m":"$DISTANCE_M","link_state":"$LINK_STATE","mobility_state":"$MOBILITY_STATE","payload_bytes":"$PAYLOAD_BYTES"}
        META
        """
    )


def script_03() -> str:
    return sh_header("03 broadcast contention") + textwrap.dedent(
        """\
        ROLE="${ROLE:-target_initiator}" # responder|target_initiator|background_initiator
        RADIO_NODE_ID="${RADIO_NODE_ID:-v2x-tx-radio}"
        PEER_NODE_ID="${PEER_NODE_ID:-v2x-rx-radio}"
        BACKGROUND_LABEL="${BACKGROUND_LABEL:-mixed-spat-bsm-map-tim}"
        COUNT="${COUNT:-1000}"
        INTERVAL_MS="${INTERVAL_MS:-100}"
        TARGET_PAYLOAD_BYTES="${TARGET_PAYLOAD_BYTES:-128}"
        BACKGROUND_PAYLOAD_BYTES="${BACKGROUND_PAYLOAD_BYTES:-1024}"
        CONDITION_ID="${CONDITION_ID:-broadcast-contention-${BACKGROUND_LABEL}}"

        cd third_party/mocar/J2735-2020/samples/ipi_custom_rtt
        if [[ "$ROLE" == "responder" ]]; then
          exec ./ipi_custom_rtt --role responder --node-id "$PEER_NODE_ID" 2>&1 | tee "$OUT_ROOT/${CONDITION_ID}_responder.log"
        elif [[ "$ROLE" == "background_initiator" ]]; then
          ./ipi_custom_rtt --role initiator --node-id "${RADIO_NODE_ID}-bg" --count "$COUNT" --interval-ms "$INTERVAL_MS" --payload-bytes "$BACKGROUND_PAYLOAD_BYTES" --csv \
            > "$OUT_ROOT/${CONDITION_ID}_background_${BACKGROUND_PAYLOAD_BYTES}.csv"
        else
          ./ipi_custom_rtt --role initiator --node-id "$RADIO_NODE_ID" --count "$COUNT" --interval-ms "$INTERVAL_MS" --payload-bytes "$TARGET_PAYLOAD_BYTES" --csv \
            > "$OUT_ROOT/${CONDITION_ID}_target.csv"
        fi
        """
    )


def script_04() -> str:
    return sh_header("04 private 5G mobility") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-sender}" # receiver|sender|gps
        TRANSPORT="${TRANSPORT:-tcp}" # tcp|mqtt|udp
        ROUTE_LABEL="${ROUTE_LABEL:-approach-stop-leave}"
        CONDITION_ID="${CONDITION_ID:-p5g-${TRANSPORT}-mobility-${ROUTE_LABEL}-${PAYLOAD_BYTES}}"

        if [[ "$ROLE" == "gps" ]]; then
          exec scripts/record_gps_for_experiment.sh "$OUT_ROOT/gps_${CONDITION_ID}"
        elif [[ "$ROLE" == "receiver" && "$TRANSPORT" == "tcp" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport tcp --port "$TCP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-mobile --rsu-id "$RSU_ID" --mobility-state moving --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-mobile --rsu-id "$RSU_ID" --mobility-state moving --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_udp_receiver.csv"
        elif [[ "$ROLE" == "receiver" && "$TRANSPORT" == "mqtt" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host "$MQTT_HOST" --port "$MQTT_PORT" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-mobile --rsu-id "$RSU_ID" --mobility-state moving --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_mqtt_receiver.csv"
        elif [[ "$TRANSPORT" == "udp" ]]; then
          ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-mobile --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --network-load-level idle --mobility-state moving --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_udp_sender.csv"
        else
          ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$(latency_host)" --port "$(latency_port)" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-mobile --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --network-load-level idle --mobility-state moving --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_${TRANSPORT}_sender.csv"
        fi
        """
    )


def script_05() -> str:
    return sh_header("05 handover weak signal") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-sender}" # receiver|sender|gps
        TRANSPORT="${TRANSPORT:-tcp}"
        SIGNAL_ZONE="${SIGNAL_ZONE:-edge-of-coverage}" # edge-of-coverage|handover|nlos
        CONDITION_ID="${CONDITION_ID:-p5g-${TRANSPORT}-${SIGNAL_ZONE}-${PAYLOAD_BYTES}}"
        export MOBILITY_STATE="handover"
        export NETWORK_LOAD_LEVEL="idle"
        if [[ "$ROLE" == "gps" ]]; then
          exec scripts/record_gps_for_experiment.sh "$OUT_ROOT/gps_${CONDITION_ID}"
        elif [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-weak-signal --rsu-id "$RSU_ID" --network-load-level idle --mobility-state handover --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "receiver" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --host "$MQTT_HOST" --port "$(latency_port)" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-weak-signal --rsu-id "$RSU_ID" --network-load-level idle --mobility-state handover --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$TRANSPORT" == "udp" ]]; then
          ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-weak-signal --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --network-load-level idle --mobility-state handover --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        else
          ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$(latency_host)" --port "$(latency_port)" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-weak-signal --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --network-load-level idle --mobility-state handover --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        fi
        """
    )


def script_06() -> str:
    return sh_header("06 load qos stress") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-latency_sender}" # load_server|load_client|latency_receiver|latency_sender
        TRANSPORT="${TRANSPORT:-tcp}"
        LOAD_DIRECTION="${LOAD_DIRECTION:-uplink}" # uplink|downlink|bidirectional
        LOAD_MBPS="${LOAD_MBPS:-25}"
        QOS_PROFILE="${QOS_PROFILE:-default}"
        LOAD_PROTOCOL="${LOAD_PROTOCOL:-tcp}"
        LOAD_PORT="${LOAD_PORT:-39000}"
        CONDITION_ID="${CONDITION_ID:-p5g-${TRANSPORT}-load-${LOAD_DIRECTION}-${LOAD_MBPS}mbps-${QOS_PROFILE}-${PAYLOAD_BYTES}}"

        case "$ROLE" in
          load_server)
            exec python3 scripts/edge4av_loadgen.py --role server --protocol "$LOAD_PROTOCOL" --port "$LOAD_PORT" --duration-s "$(( (COUNT * INTERVAL_MS / 1000) + 120 ))" > "$OUT_ROOT/${CONDITION_ID}_load_server.csv"
            ;;
          load_client)
            python3 scripts/edge4av_loadgen.py --role client --protocol "$LOAD_PROTOCOL" --host "$RECEIVER_HOST" --port "$LOAD_PORT" --duration-s "$(( (COUNT * INTERVAL_MS / 1000) + 60 ))" --target-mbps "$LOAD_MBPS" > "$OUT_ROOT/${CONDITION_ID}_load_client.csv"
            ;;
          latency_receiver)
            if [[ "$TRANSPORT" == "udp" ]]; then
              exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-stressed --rsu-id "$RSU_ID" --network-load-level "$LOAD_DIRECTION-$LOAD_MBPS" --qos-profile "$QOS_PROFILE" --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
            fi
            exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --host "$MQTT_HOST" --port "$(latency_port)" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-stressed --rsu-id "$RSU_ID" --network-load-level "$LOAD_DIRECTION-$LOAD_MBPS" --qos-profile "$QOS_PROFILE" --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
            ;;
          latency_sender)
            if [[ "$TRANSPORT" == "udp" ]]; then
              ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-stressed --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --network-load-level "$LOAD_DIRECTION-$LOAD_MBPS" --qos-profile "$QOS_PROFILE" --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
              exit 0
            fi
            ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$(latency_host)" --port "$(latency_port)" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label private-5g-stressed --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --network-load-level "$LOAD_DIRECTION-$LOAD_MBPS" --qos-profile "$QOS_PROFILE" --clock-sync-state "$CLOCK_SYNC_STATE" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
            ;;
        esac
        """
    )


def script_07() -> str:
    return sh_header("07 transport comparison") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-sender}" # receiver|sender
        TRANSPORT="${TRANSPORT:-tcp}" # tcp|mqtt|udp
        CONDITION_ID="${CONDITION_ID:-transport-${TRANSPORT}-payload-${PAYLOAD_BYTES}}"
        if [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label transport-comparison --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "receiver" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --host "$MQTT_HOST" --port "$(latency_port)" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label transport-comparison --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$TRANSPORT" == "udp" ]]; then
          ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label transport-comparison --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        else
          ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$(latency_host)" --port "$(latency_port)" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label transport-comparison --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        fi
        """
    )


def script_08() -> str:
    return sh_header("08 end-to-end deadline") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-sender}" # receiver|sender|analyze
        TRANSPORT="${TRANSPORT:-mqtt}"
        DEADLINE_MS="${DEADLINE_MS:-250}"
        APP_SERVICE="${APP_SERVICE:-guided-planning}"
        CONDITION_ID="${CONDITION_ID:-e2e-${APP_SERVICE}-${TRANSPORT}-${DEADLINE_MS}ms-${PAYLOAD_BYTES}}"
        if [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label e2e-deadline --rsu-id "$RSU_ID" --vehicle-outcome-name deadline_ms --vehicle-outcome-value "$DEADLINE_MS" --vehicle-outcome-unit ms --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "receiver" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --host "$MQTT_HOST" --port "$(latency_port)" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label e2e-deadline --rsu-id "$RSU_ID" --vehicle-outcome-name deadline_ms --vehicle-outcome-value "$DEADLINE_MS" --vehicle-outcome-unit ms --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "analyze" ]]; then
          python3 scripts/analyze_edge4av_deadlines.py "$OUT_ROOT/${CONDITION_ID}_sender.csv" --deadline-ms "$DEADLINE_MS" --output "$OUT_ROOT/${CONDITION_ID}_deadline_summary.json"
        elif [[ "$TRANSPORT" == "udp" ]]; then
          ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label e2e-deadline --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --vehicle-outcome-name deadline_ms --vehicle-outcome-value "$DEADLINE_MS" --vehicle-outcome-unit ms --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        else
          ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$(latency_host)" --port "$(latency_port)" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label e2e-deadline --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --vehicle-outcome-name deadline_ms --vehicle-outcome-value "$DEADLINE_MS" --vehicle-outcome-unit ms --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        fi
        """
    )


def script_09() -> str:
    return sh_header("09 detector output to IPI") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-build_payloads}" # build_payloads|receiver|sender
        TRANSPORT="${TRANSPORT:-tcp}"
        DETECTOR_CSV="${DETECTOR_CSV:-results/v2x_benchmarks/v2x-radar-detector-benchmark-20260629T182624Z/per_sample.csv}"
        PAYLOAD_MANIFEST="$OUT_ROOT/detector_output_ipi_payloads.json"
        CONDITION_PREFIX="${CONDITION_PREFIX:-detector-output-${TRANSPORT}}"

        if [[ "$ROLE" == "build_payloads" ]]; then
          python3 scripts/build_detector_output_ipi_payloads.py --input "$DETECTOR_CSV" --output "$PAYLOAD_MANIFEST"
        elif [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_PREFIX" --condition-label detector-output --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_PREFIX}_receiver.csv"
        elif [[ "$ROLE" == "receiver" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --host "$MQTT_HOST" --port "$(latency_port)" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_PREFIX" --condition-label detector-output --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_PREFIX}_receiver.csv"
        else
          export TRANSPORT MQTT_HOST RECEIVER_HOST MQTT_PORT TCP_PORT UDP_PORT COUNT INTERVAL_MS INTERSECTION_ID SOURCE_ID RUN_ID AV_ID OBU_ID RSU_ID OUT_ROOT
          python3 - "$PAYLOAD_MANIFEST" "$CONDITION_PREFIX" <<'PY'
        import json, os, subprocess, sys
        manifest, prefix = sys.argv[1], sys.argv[2]
        transport = os.environ["TRANSPORT"]
        host = os.environ["MQTT_HOST"] if transport == "mqtt" else os.environ["RECEIVER_HOST"]
        port = os.environ["MQTT_PORT"] if transport == "mqtt" else (os.environ["UDP_PORT"] if transport == "udp" else os.environ["TCP_PORT"])
        out_root = os.environ["OUT_ROOT"]
        payloads = json.load(open(manifest))["representative_ipi_payload_bytes"]
        for payload in payloads:
            condition = f"{prefix}-payload-{payload}"
            if transport == "udp":
                cmd = [
                    "./cpp/build/example_private_5g_latency_udp_sender", "--host", host, "--port", port,
                    "--count", os.environ["COUNT"], "--interval-ms", os.environ["INTERVAL_MS"], "--message", "service",
                    "--payload-bytes", str(payload), "--run-id", os.environ["RUN_ID"], "--condition-id", condition,
                    "--condition-label", "detector-output", "--request-id", condition, "--av-id", os.environ["AV_ID"],
                    "--obu-id", os.environ["OBU_ID"], "--rsu-id", os.environ["RSU_ID"], "--csv",
                ]
            else:
                cmd = [
                    "./cpp/build/example_private_5g_latency_sender", "--transport", transport, "--host", host, "--port", port,
                    "--count", os.environ["COUNT"], "--interval-ms", os.environ["INTERVAL_MS"], "--message", "service",
                    "--payload-bytes", str(payload), "--intersection-id", os.environ["INTERSECTION_ID"], "--source-id", os.environ["SOURCE_ID"],
                    "--run-id", os.environ["RUN_ID"], "--condition-id", condition, "--condition-label", "detector-output",
                    "--request-id", condition, "--av-id", os.environ["AV_ID"], "--obu-id", os.environ["OBU_ID"], "--rsu-id", os.environ["RSU_ID"], "--csv",
                ]
            with open(f"{out_root}/{condition}_sender.csv", "w") as out:
                subprocess.run(cmd, check=True, stdout=out)
        PY
        fi
        """
    )


def script_10() -> str:
    return sh_header("10 edge offload tradeoff") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-network_transfer}" # receiver|local_detector|network_transfer|deadline_analyze
        PLACEMENT="${PLACEMENT:-edge-only}" # onboard-only|edge-only|hybrid
        TRANSPORT="${TRANSPORT:-tcp}"
        PAYLOAD_BYTES="${PAYLOAD_BYTES:-60000}"
        DEADLINE_MS="${DEADLINE_MS:-500}"
        CONDITION_ID="${CONDITION_ID:-offload-${PLACEMENT}-${TRANSPORT}-${PAYLOAD_BYTES}}"
        if [[ "$ROLE" == "local_detector" ]]; then
          CUDA_VISIBLE_DEVICES="${CUDA_VISIBLE_DEVICES:-0,1,2,3}" conda run -n mmdet3d python scripts/run_v2x_radar_detector_benchmark.py --max-samples "${MAX_SAMPLES:-100}" --sample-mode spread --gpus "${GPUS:-0,1,2,3}" --run-id "$CONDITION_ID"
        elif [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label edge-offload --rsu-id "$RSU_ID" --vehicle-outcome-name placement --vehicle-outcome-value "$PLACEMENT" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "receiver" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --host "$MQTT_HOST" --port "$(latency_port)" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label edge-offload --rsu-id "$RSU_ID" --vehicle-outcome-name placement --vehicle-outcome-value "$PLACEMENT" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "deadline_analyze" ]]; then
          python3 scripts/analyze_edge4av_deadlines.py "$OUT_ROOT/${CONDITION_ID}_sender.csv" --deadline-ms "$DEADLINE_MS" --output "$OUT_ROOT/${CONDITION_ID}_deadline_summary.json"
        elif [[ "$TRANSPORT" == "udp" ]]; then
          ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label edge-offload --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --vehicle-outcome-name placement --vehicle-outcome-value "$PLACEMENT" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        else
          ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$(latency_host)" --port "$(latency_port)" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label edge-offload --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --vehicle-outcome-name placement --vehicle-outcome-value "$PLACEMENT" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        fi
        """
    )


def script_11() -> str:
    return sh_header("11 multiclient scalability") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-sender_group}" # receiver|sender_group
        TRANSPORT="${TRANSPORT:-tcp}"
        CLIENTS="${CLIENTS:-10}"
        PAYLOAD_BYTES="${PAYLOAD_BYTES:-1024}"
        CONDITION_ID="${CONDITION_ID:-scale-${TRANSPORT}-${CLIENTS}clients-${PAYLOAD_BYTES}}"
        if [[ "$TRANSPORT" == "mqtt" ]]; then
          echo "This starter does not support MQTT multi-client with distinct source IDs because the current receiver subscribes to one source topic. Use TRANSPORT=tcp or TRANSPORT=udp, or run one MQTT receiver per source ID." >&2
          exit 2
        fi
        if [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label scale --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "receiver" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --port "$TCP_PORT" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label scale --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        fi
        pids=()
        for i in $(seq 1 "$CLIENTS"); do
          SRC="veh-$(printf '%02d' "$i")"
          if [[ "$TRANSPORT" == "udp" ]]; then
            ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label scale --request-id "${CONDITION_ID}-${SRC}" --av-id "$SRC" --obu-id "$SRC" --rsu-id "$RSU_ID" --network-load-level "${CLIENTS}-clients" --csv > "$OUT_ROOT/${CONDITION_ID}_${SRC}_sender.csv" &
          else
            ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$RECEIVER_HOST" --port "$TCP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SRC" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label scale --request-id "${CONDITION_ID}-${SRC}" --av-id "$SRC" --obu-id "$SRC" --rsu-id "$RSU_ID" --network-load-level "${CLIENTS}-clients" --csv > "$OUT_ROOT/${CONDITION_ID}_${SRC}_sender.csv" &
          fi
          pids+=("$!")
        done
        for pid in "${pids[@]}"; do wait "$pid"; done
        """
    )


def script_12() -> str:
    return sh_header("12 failure fallback") + common_private5g_vars() + textwrap.dedent(
        """\
        ROLE="${ROLE:-sender}" # receiver|sender|event
        TRANSPORT="${TRANSPORT:-tcp}"
        FAILURE_MODE="${FAILURE_MODE:-receiver-restart}" # receiver-restart|broker-restart|weak-signal|compact-fallback
        CONDITION_ID="${CONDITION_ID:-failure-${FAILURE_MODE}-${TRANSPORT}-${PAYLOAD_BYTES}}"
        if [[ "$ROLE" == "event" ]]; then
          echo "event_time_ns,event,detail" > "$OUT_ROOT/${CONDITION_ID}_events.csv"
          echo "$(date +%s%N),$FAILURE_MODE,operator-triggered" >> "$OUT_ROOT/${CONDITION_ID}_events.csv"
        elif [[ "$ROLE" == "receiver" && "$TRANSPORT" == "udp" ]]; then
          exec ./cpp/build/example_private_5g_latency_udp_receiver --port "$UDP_PORT" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label failure-fallback --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$ROLE" == "receiver" ]]; then
          exec ./cpp/build/example_private_5g_latency_receiver --transport "$TRANSPORT" --host "$MQTT_HOST" --port "$(latency_port)" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label failure-fallback --rsu-id "$RSU_ID" --csv > "$OUT_ROOT/${CONDITION_ID}_receiver.csv"
        elif [[ "$TRANSPORT" == "udp" ]]; then
          ./cpp/build/example_private_5g_latency_udp_sender --host "$RECEIVER_HOST" --port "$UDP_PORT" --count "$COUNT" --interval-ms "$INTERVAL_MS" --timeout-ms "${TIMEOUT_MS:-1000}" --message service --payload-bytes "$PAYLOAD_BYTES" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label failure-fallback --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --vehicle-outcome-name fallback_mode --vehicle-outcome-value "$FAILURE_MODE" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        else
          ./cpp/build/example_private_5g_latency_sender --transport "$TRANSPORT" --host "$(latency_host)" --port "$(latency_port)" --count "$COUNT" --interval-ms "$INTERVAL_MS" --timeout-ms "${TIMEOUT_MS:-1000}" --message service --payload-bytes "$PAYLOAD_BYTES" --intersection-id "$INTERSECTION_ID" --source-id "$SOURCE_ID" --run-id "$RUN_ID" --condition-id "$CONDITION_ID" --condition-label failure-fallback --request-id "$CONDITION_ID" --av-id "$AV_ID" --obu-id "$OBU_ID" --rsu-id "$RSU_ID" --vehicle-outcome-name fallback_mode --vehicle-outcome-value "$FAILURE_MODE" --csv > "$OUT_ROOT/${CONDITION_ID}_sender.csv"
        fi
        """
    )


SCRIPT_BUILDERS = [
    script_01,
    script_02,
    script_03,
    script_04,
    script_05,
    script_06,
    script_07,
    script_08,
    script_09,
    script_10,
    script_11,
    script_12,
]


def write_matrix(path: Path) -> None:
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=["id", "title", "minimum_repeats", "target_probes", "purpose"])
        writer.writeheader()
        writer.writerows(EXPERIMENTS)


def write_runbook(path: Path, run_id: str) -> None:
    lines = [
        "# Edge4AV MobiCom/MobiSys Experiment Suite",
        "",
        f"Run suite: `{run_id}`",
        "",
        "## Before Running",
        "",
        "1. Build the C++ tools on the build host:",
        "",
        "   ```bash",
        "   cmake -S cpp -B cpp/build -DIPI_ENABLE_TESTS=ON",
        "   cmake --build cpp/build",
        "   ctest --test-dir cpp/build --output-on-failure",
        "   make -C third_party/mocar/J2735-2020/samples/ipi_custom_rtt",
        "   make -C third_party/mocar/J2735-2020/samples/ipi_spat_bridge",
        "   ```",
        "",
        "2. Copy the relevant binaries and this suite directory to the device roles.",
        "3. Use the same `RUN_ID` for every role in one data-collection pass.",
        "4. Store GPS, signal, receiver, sender, load, and event logs under the same `OUT_ROOT`.",
        "",
        "## Repetition Rule",
        "",
        "Use at least the `minimum_repeats` listed in `experiment_matrix.csv`. For top-tier submission quality, prefer 10 independent repeats for mobility, handover, end-to-end deadline, and failure/fallback experiments. A repeat means a fresh route pass or condition run, not rerunning summary scripts on the same data.",
        "",
        "## Experiments",
        "",
    ]
    for exp in EXPERIMENTS:
        script_name = f"{exp['id']}.sh"
        lines.extend(
            [
                f"### {exp['id']}: {exp['title']}",
                "",
                exp["purpose"],
                "",
                f"- Starter script: `start_scripts/{script_name}`",
                f"- Minimum repeats: `{exp['minimum_repeats']}`",
                f"- Target samples: `{exp['target_probes']}`",
                "",
                "Start by opening the script, setting role/IP/condition variables at the top, and running the required roles in separate terminals or devices.",
                "",
            ]
        )
    lines.extend(
        [
            "## Data Quality Checklist",
            "",
            "- Every condition has sender CSV, receiver CSV/log, metadata, and GPS when vehicle-side movement is involved.",
            "- Every condition records `run_id`, `condition_id`, `request_id`, `transport`, `payload_bytes`, `network_load_level`, `qos_profile`, `mobility_state`, and `clock_sync_state`.",
            "- One-way latency is used only when clocks are synchronized; otherwise report RTT.",
            "- Keep failed and partial runs. Mark them in metadata instead of deleting them.",
            "- For radio experiments, record distance, LOS/NLOS, antenna placement, channel/config, message rate, and whether background senders were active.",
            "",
        ]
    )
    path.write_text("\n".join(lines), encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--results-root", type=Path, default=DEFAULT_RESULTS_ROOT)
    parser.add_argument("--run-id", default="")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    run_id = args.run_id or "edge4av-suite-" + datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    suite_dir = args.results_root / run_id
    scripts_dir = suite_dir / "start_scripts"
    scripts_dir.mkdir(parents=True, exist_ok=True)

    for exp, builder in zip(EXPERIMENTS, SCRIPT_BUILDERS):
        write_script(scripts_dir / f"{exp['id']}.sh", builder())

    write_matrix(suite_dir / "experiment_matrix.csv")
    write_runbook(suite_dir / "INSTRUCTIONS.md", run_id)
    (suite_dir / "suite_manifest.json").write_text(
        json.dumps(
            {
                "run_id": run_id,
                "generated_utc": datetime.now(timezone.utc).isoformat(),
                "experiments": EXPERIMENTS,
                "start_scripts": [f"start_scripts/{exp['id']}.sh" for exp in EXPERIMENTS],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    print(suite_dir)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
