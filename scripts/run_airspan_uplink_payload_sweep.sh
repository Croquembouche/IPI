#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: scripts/run_airspan_uplink_payload_sweep.sh

Required environment:
  EDGE_HOST           Reachable d1 edge address
  SSH_KEY             Private key authorized for EDGE_USER on the edge

Optional environment:
  EDGE_USER                 Default: d1
  EDGE_DEPLOY_ROOT          Default: /home/d1/edge4av_followup/ipi_2c043b3
  REMOTE_RUN_ROOT           Default: /home/d1/edge4av_followup/runs
  RUN_NAME                  Default: 20260815_airspan_tdd_raw_uplink_70_20_10_location_1_run_2
  GPS_EVIDENCE_ROOT         Default: the validated run-1 raw result root
  LOCATION_ID               Default: location_1
  TDD_PROFILE               40/40/20, 60/20/20, or 70/20/10; default: 70/20/10
  TDD_STATE                 Default: operator-reported
  RSRP_DBM                  Integer dBm or unreported; default: unreported
  RSRQ_DB                   Integer dB or unreported; default: unreported
  RADIO_STATE               Default: unreported
  CELL_1_ADMIN_STATE        locked, unlocked, or unreported; default: unreported
  CELL_2_ADMIN_STATE        locked, unlocked, or unreported; default: unreported
  SERVING_CELL              1, 2, or unreported; default: unreported
  HANDOFF_STATE             no-reported-handoff, handoff-observed, or unreported;
                            default: unreported
  COUNT                     Default: 1000
  PAYLOAD_512000_COUNT      Default: COUNT; permits a shorter 500-KiB condition
  PAYLOAD_1048576_COUNT     Default: COUNT; permits a shorter 1,024-KiB condition
  INTERVAL_MS               Default: 200
  MQTT_TIMEOUT_MS           Default: 60000
  MAX_CONDITION_SECONDS     Default: 14400
  TRANSPORTS                Default: "tcp mqtt"
  PAYLOADS                  Default: "1024 10240 102400 1048576"
  APPLICATION_DIRECTION     uplink or downlink; default: uplink
  PREFLIGHT_ONLY            Set to 1 to validate without reserving paths

In uplink mode, the script sends an exact raw application body and waits for a
compact acknowledgment. In downlink mode, it sends a compact request and waits
for an exact response body whose length, sequence, and CRC32 are validated. It
writes exact artifacts only below the excluded CISCO_AIRSPAN_STATS tree. It
refuses to overwrite an incomplete condition. A completed condition is skipped
on a later invocation, allowing safe continuation after an interruption
between conditions.
EOF
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  usage
  exit 0
fi
if [[ "$#" -ne 0 ]]; then
  usage >&2
  exit 2
fi

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUN_NAME="${RUN_NAME:-20260815_airspan_tdd_raw_uplink_70_20_10_location_1_run_2}"
RUN_ID="${RUN_ID:-edge4av-real-${RUN_NAME//_/-}}"
RAW_RUN_DIR="${RAW_RUN_DIR:-$REPO_ROOT/CISCO_AIRSPAN_STATS/${RUN_NAME}_unredacted}"
GPS_EVIDENCE_ROOT="${GPS_EVIDENCE_ROOT:-$REPO_ROOT/CISCO_AIRSPAN_STATS/20260815_airspan_tdd_uplink_70_20_10_location_1_run_1_unredacted}"
LOCATION_ID="${LOCATION_ID:-location_1}"
EDGE_USER="${EDGE_USER:-d1}"
EDGE_DEPLOY_ROOT="${EDGE_DEPLOY_ROOT:-/home/d1/edge4av_followup/ipi_2c043b3}"
REMOTE_RUN_ROOT="${REMOTE_RUN_ROOT:-/home/d1/edge4av_followup/runs}"
REMOTE_RUN_DIR="$REMOTE_RUN_ROOT/$RUN_NAME"
APPLICATION_DIRECTION="${APPLICATION_DIRECTION:-uplink}"
if [[ "$APPLICATION_DIRECTION" == "downlink" ]]; then
  LOCAL_PROBE="$REPO_ROOT/scripts/private_5g_raw_downlink_probe.py"
else
  LOCAL_PROBE="$REPO_ROOT/scripts/private_5g_raw_bulk_probe.py"
fi
REMOTE_PROBE="$REMOTE_RUN_DIR/tools/$(basename "$LOCAL_PROBE")"
COUNT="${COUNT:-1000}"
PAYLOAD_512000_COUNT="${PAYLOAD_512000_COUNT:-$COUNT}"
PAYLOAD_1048576_COUNT="${PAYLOAD_1048576_COUNT:-$COUNT}"
INTERVAL_MS="${INTERVAL_MS:-200}"
TCP_PORT="${TCP_PORT:-36666}"
MQTT_PORT="${MQTT_PORT:-1883}"
MQTT_TIMEOUT_MS="${MQTT_TIMEOUT_MS:-60000}"
MAX_CONDITION_SECONDS="${MAX_CONDITION_SECONDS:-14400}"
TRANSPORT_TEXT="${TRANSPORTS:-tcp mqtt}"
PAYLOAD_TEXT="${PAYLOADS:-1024 10240 102400 1048576}"
PREFLIGHT_ONLY="${PREFLIGHT_ONLY:-0}"
TDD_PROFILE="${TDD_PROFILE:-70/20/10}"
TDD_STATE="${TDD_STATE:-operator-reported}"
RSRP_DBM="${RSRP_DBM:-unreported}"
RSRQ_DB="${RSRQ_DB:-unreported}"
RADIO_STATE="${RADIO_STATE:-unreported}"
CELL_1_ADMIN_STATE="${CELL_1_ADMIN_STATE:-unreported}"
CELL_2_ADMIN_STATE="${CELL_2_ADMIN_STATE:-unreported}"
SERVING_CELL="${SERVING_CELL:-unreported}"
HANDOFF_STATE="${HANDOFF_STATE:-unreported}"

: "${EDGE_HOST:?Set EDGE_HOST to the reachable d1 edge address}"
: "${SSH_KEY:?Set SSH_KEY to the authorized per-run private key}"

read -r -a TRANSPORT_LIST <<< "$TRANSPORT_TEXT"
read -r -a PAYLOAD_LIST <<< "$PAYLOAD_TEXT"

for name in RUN_NAME RUN_ID LOCATION_ID; do
  value="${!name}"
  if [[ ! "$value" =~ ^[A-Za-z0-9_.-]+$ ]]; then
    echo "$name contains unsupported characters" >&2
    exit 2
  fi
done
for name in COUNT PAYLOAD_512000_COUNT PAYLOAD_1048576_COUNT INTERVAL_MS TCP_PORT MQTT_PORT MQTT_TIMEOUT_MS MAX_CONDITION_SECONDS; do
  value="${!name}"
  if [[ ! "$value" =~ ^[1-9][0-9]*$ ]]; then
    echo "$name must be a positive integer" >&2
    exit 2
  fi
done
case "${TRANSPORT_LIST[*]}" in
  tcp|mqtt|"tcp mqtt") ;;
  *)
    echo 'TRANSPORTS must be "tcp", "mqtt", or "tcp mqtt"' >&2
    exit 2
    ;;
esac
case "${PAYLOAD_LIST[*]}" in
  "1024 10240 102400"|"1024 10240 102400 1048576"|"1024 10240 102400 512000")
    ;;
  *)
    echo 'PAYLOADS must be the 1-100 KiB prefix or the four-size sweep ending at 500 KiB or 1 MiB' >&2
    exit 2
    ;;
esac
if [[ "$APPLICATION_DIRECTION" != "uplink" && "$APPLICATION_DIRECTION" != "downlink" ]]; then
  echo 'APPLICATION_DIRECTION must be uplink or downlink' >&2
  exit 2
fi
if (( COUNT > 1000 )) || [[ "$INTERVAL_MS" != "200" ]]; then
  echo 'COUNT must not exceed 1000 and INTERVAL_MS must remain 200 for this acquisition' >&2
  exit 2
fi
if (( PAYLOAD_512000_COUNT > COUNT || PAYLOAD_1048576_COUNT > COUNT )); then
  echo 'Per-payload count cannot exceed COUNT' >&2
  exit 2
fi
if [[ "$TDD_PROFILE" != "40/40/20" && "$TDD_PROFILE" != "60/20/20" && "$TDD_PROFILE" != "70/20/10" ]]; then
  echo 'TDD_PROFILE must be 40/40/20, 60/20/20, or 70/20/10' >&2
  exit 2
fi
for name in RSRP_DBM RSRQ_DB; do
  value="${!name}"
  if [[ "$value" != "unreported" && ! "$value" =~ ^-?[0-9]+$ ]]; then
    echo "$name must be an integer or unreported" >&2
    exit 2
  fi
done
for name in CELL_1_ADMIN_STATE CELL_2_ADMIN_STATE; do
  value="${!name}"
  if [[ "$value" != "locked" && "$value" != "unlocked" && "$value" != "unreported" ]]; then
    echo "$name must be locked, unlocked, or unreported" >&2
    exit 2
  fi
done
if [[ "$SERVING_CELL" != "1" && "$SERVING_CELL" != "2" && "$SERVING_CELL" != "unreported" ]]; then
  echo 'SERVING_CELL must be 1, 2, or unreported' >&2
  exit 2
fi
if [[ "$HANDOFF_STATE" != "no-reported-handoff" && "$HANDOFF_STATE" != "handoff-observed" && "$HANDOFF_STATE" != "unreported" ]]; then
  echo 'HANDOFF_STATE must be no-reported-handoff, handoff-observed, or unreported' >&2
  exit 2
fi
if [[ "$SERVING_CELL" == "1" || "$SERVING_CELL" == "2" ]]; then
  serving_state_name="CELL_${SERVING_CELL}_ADMIN_STATE"
else
  serving_state_name=""
fi
if [[ -n "$serving_state_name" && "${!serving_state_name}" == "locked" ]]; then
  echo 'The serving cell must be administratively unlocked and broadcasting' >&2
  exit 2
fi
if [[ ! -f "$SSH_KEY" ]]; then
  echo "SSH key does not exist: $SSH_KEY" >&2
  exit 2
fi

SSH_OPTS=(
  -i "$SSH_KEY"
  -o BatchMode=yes
  -o IdentitiesOnly=yes
  -o StrictHostKeyChecking=no
  -o UserKnownHostsFile=/dev/null
)
SSH_BASE=(ssh "${SSH_OPTS[@]}" "$EDGE_USER@$EDGE_HOST")
RSYNC_RSH="ssh -i $SSH_KEY -o BatchMode=yes -o IdentitiesOnly=yes -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null"

CURRENT_LOCAL_DIR=""
CURRENT_REMOTE_DIR=""
CURRENT_REMOTE_PID_DIR=""
LOCAL_TELEMETRY_PID=""
CONDITION_RESERVED=0

log() {
  printf '[%s] %s\n' "$(date --iso-8601=seconds)" "$*"
}

ssh_edge_bash() {
  local remote_script="$1"
  "${SSH_BASE[@]}" "bash -lc $(printf '%q' "$remote_script")"
}

wait_edge_port() {
  local port="$1"
  local label="$2"
  for _ in $(seq 1 60); do
    if timeout 1 bash -c ":</dev/tcp/$EDGE_HOST/$port" >/dev/null 2>&1; then
      return 0
    fi
    sleep 1
  done
  echo "Timed out waiting for $label" >&2
  return 1
}

stop_local_telemetry() {
  if [[ -n "$LOCAL_TELEMETRY_PID" ]] && kill -0 "$LOCAL_TELEMETRY_PID" 2>/dev/null; then
    kill -TERM "$LOCAL_TELEMETRY_PID" 2>/dev/null || true
    wait "$LOCAL_TELEMETRY_PID" 2>/dev/null || true
  fi
  LOCAL_TELEMETRY_PID=""
}

stop_remote_processes() {
  if [[ -z "$CURRENT_REMOTE_PID_DIR" ]]; then
    return 0
  fi
  ssh_edge_bash "if test -d '$CURRENT_REMOTE_PID_DIR'; then for f in '$CURRENT_REMOTE_PID_DIR'/*.pid; do test -f \"\$f\" || continue; p=\$(cat \"\$f\" 2>/dev/null || true); if test -n \"\$p\" && kill -0 \"\$p\" 2>/dev/null; then kill -TERM \"\$p\" 2>/dev/null || true; fi; done; sleep 1; for f in '$CURRENT_REMOTE_PID_DIR'/*.pid; do test -f \"\$f\" || continue; p=\$(cat \"\$f\" 2>/dev/null || true); if test -n \"\$p\" && kill -0 \"\$p\" 2>/dev/null; then kill -KILL \"\$p\" 2>/dev/null || true; fi; done; fi" >/dev/null 2>&1 || true
}

fetch_remote_artifacts() {
  if [[ -z "$CURRENT_LOCAL_DIR" || -z "$CURRENT_REMOTE_DIR" ]]; then
    return 0
  fi
  mkdir -p "$CURRENT_LOCAL_DIR/edge"
  rsync -az -e "$RSYNC_RSH" \
    "$EDGE_USER@$EDGE_HOST:$CURRENT_REMOTE_DIR/" \
    "$CURRENT_LOCAL_DIR/edge/" \
    >> "$CURRENT_LOCAL_DIR/rsync_from_edge.log" 2>&1 || true
}

cleanup_condition() {
  set +e
  if [[ "$CONDITION_RESERVED" == "1" ]]; then
    stop_local_telemetry
    stop_remote_processes
    fetch_remote_artifacts
  fi
  CONDITION_RESERVED=0
  CURRENT_LOCAL_DIR=""
  CURRENT_REMOTE_DIR=""
  CURRENT_REMOTE_PID_DIR=""
  set -e
}
trap cleanup_condition EXIT INT TERM

start_remote_process() {
  local name="$1"
  local stdout_name="$2"
  shift 2
  local command="$*"
  local pid_file="$CURRENT_REMOTE_PID_DIR/$name.pid"
  local stdout_file="$CURRENT_REMOTE_DIR/logs/$stdout_name"
  local stderr_file="$CURRENT_REMOTE_DIR/logs/$name.err"
  ssh_edge_bash "setsid -f bash -c \"echo \\\$\\\$ > '$pid_file'; exec $command > '$stdout_file' 2> '$stderr_file' < /dev/null\""
}

write_manifest() {
  local transport="$1"
  local payload="$2"
  local condition_id="$3"
  local condition_count="$4"
  python3 -c '
import json, pathlib, sys
path = pathlib.Path(sys.argv[1])
def optional_int(value):
    return None if value == "unreported" else int(value)
def optional_broadcasting(value):
    return None if value == "unreported" else value == "unlocked"
data = {
    "schema": "edge4av-airspan-raw-" + sys.argv[21] + "-payload-v1",
    "status": "running",
    "run_id": sys.argv[2],
    "run_name": sys.argv[3],
    "condition_id": sys.argv[4],
    "transport": sys.argv[5],
    "application_direction": sys.argv[21] + "-oriented",
    "application_payload_bytes": int(sys.argv[6]),
    "count": int(sys.argv[7]),
    "interval_ms": int(sys.argv[8]),
    "mqtt_timeout_ms": int(sys.argv[9]),
    "outer_timeout_s": int(sys.argv[10]),
    "tdd_profile": sys.argv[11],
    "tdd_state": sys.argv[18],
    "airspan_cell": optional_int(sys.argv[14]),
    "serving_cell": optional_int(sys.argv[14]),
    "handoff_state": None if sys.argv[15] == "unreported" else sys.argv[15],
    "cell_administrative_lock_definition": "locked means not broadcasting",
    "cell_1_administrative_state": sys.argv[16],
    "cell_1_broadcasting": optional_broadcasting(sys.argv[16]),
    "cell_2_administrative_state": sys.argv[17],
    "cell_2_broadcasting": optional_broadcasting(sys.argv[17]),
    "rsrp_dbm": optional_int(sys.argv[12]),
    "rsrq_db": optional_int(sys.argv[13]),
    "radio_state": sys.argv[19],
    "location_id": sys.argv[20],
    "mobility_state": "stationary",
    "physical_placement": "not-reconfirmed",
    "dnn": "cisco5g",
    "five_qi": 9,
    "qos_profile": "default",
    "clock_sync_state": "unsynced",
    "timing_metric": "monotonic_complete_application_" + ("ack" if sys.argv[21] == "uplink" else "response") + "_rtt",
    "timing_boundary": ("immediately before first framed TCP or MQTT byte is sent through complete correlated application acknowledgment validation" if sys.argv[21] == "uplink" else "immediately before the compact request is sent through receipt and validation of the complete correlated downlink response"),
    "payload_validation_metric": (None if sys.argv[21] == "uplink" else "vehicle monotonic time for response length, sequence, and CRC32 validation"),
    "payload_validation_included_in_rtt": (None if sys.argv[21] == "uplink" else True),
    "payload_protocol": ("exact raw application body with length and CRC32 validation" if sys.argv[21] == "uplink" else "compact request followed by exact response body with length, sequence, and CRC32 validation"),
    "application_acknowledgment": sys.argv[21] == "uplink",
    "tcp_acknowledgment_only": False,
}
path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n", encoding="utf-8")
' "$CURRENT_LOCAL_DIR/run_manifest.json" "$RUN_ID" "$RUN_NAME" "$condition_id" \
    "$transport" "$payload" "$condition_count" "$INTERVAL_MS" "$MQTT_TIMEOUT_MS" \
    "$MAX_CONDITION_SECONDS" "$TDD_PROFILE" "$RSRP_DBM" "$RSRQ_DB" \
    "$SERVING_CELL" "$HANDOFF_STATE" "$CELL_1_ADMIN_STATE" "$CELL_2_ADMIN_STATE" \
    "$TDD_STATE" "$RADIO_STATE" "$LOCATION_ID" "$APPLICATION_DIRECTION"
}

record_timing() {
  local start="$1"
  local finish="$2"
  python3 -c '
import json, pathlib, sys
path = pathlib.Path(sys.argv[1])
data = json.loads(path.read_text(encoding="utf-8"))
data["workload_started_at"] = sys.argv[2]
data["workload_finished_at"] = sys.argv[3]
path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n", encoding="utf-8")
' "$CURRENT_LOCAL_DIR/run_manifest.json" "$start" "$finish"
}

validate_condition() {
  local transport="$1"
  local payload="$2"
  local condition_id="$3"
  local condition_count="$4"
  python3 - "$CURRENT_LOCAL_DIR" "$transport" "$payload" "$condition_id" "$condition_count" "$APPLICATION_DIRECTION" <<'PY'
import csv
import json
import math
import pathlib
import sys

root = pathlib.Path(sys.argv[1])
transport = sys.argv[2]
payload = int(sys.argv[3])
condition_id = sys.argv[4]
expected = int(sys.argv[5])
direction = sys.argv[6]

with (root / "sender.csv").open(newline="", encoding="utf-8") as handle:
    rows = list(csv.DictReader(handle))

accepted = [row for row in rows if row.get("accepted", "").lower() == "true"]
failed = len(rows) - len(accepted)
rtts = sorted(float(row["rtt_ms"]) for row in accepted if row.get("rtt_ms"))
payload_validation = sorted(
    float(row["payload_validation_ms"])
    for row in accepted
    if row.get("payload_validation_ms")
)
sequences = [int(row["sequence"]) for row in rows]
wire_sizes = [int(row["wire_request_bytes"]) for row in accepted if row.get("wire_request_bytes")]

receiver_path = root / "edge" / "logs" / "receiver.csv"
receiver_rows = []
if receiver_path.is_file():
    with receiver_path.open(newline="", encoding="utf-8") as handle:
        receiver_rows = list(csv.DictReader(handle))

def percentile(values, fraction):
    if not values:
        return None
    return values[max(0, math.ceil(fraction * len(values)) - 1)]

errors = []
if len(rows) != expected:
    errors.append(f"sender rows {len(rows)} != {expected}")
if sequences != list(range(1, expected + 1)):
    errors.append("sender sequence is not exactly 1..count")
if any(row.get("transport") != transport for row in rows):
    errors.append("sender transport mismatch")
if any(row.get("condition_id") != condition_id for row in rows):
    errors.append("sender condition_id mismatch")
if any(int(row.get("application_payload_bytes", -1)) != payload for row in rows):
    errors.append("sender application payload length mismatch")
expected_detail = (
    "validated compact application acknowledgment" if direction == "uplink" else
    "validated complete downlink response length, sequence, and CRC32"
)
if any(row.get("detail") != expected_detail for row in accepted):
    errors.append("sender complete application response validation mismatch")
if direction == "downlink" and len(payload_validation) != len(accepted):
    errors.append("sender payload-validation timing count mismatch")
if len(receiver_rows) < len(accepted):
    errors.append("receiver rows fewer than accepted sender rows")
if any(int(row.get("application_payload_bytes", -1)) != payload for row in receiver_rows):
    errors.append("receiver application payload length mismatch")

summary = {
    "status": "complete" if not errors else "invalid",
    "transport": transport,
    "application_direction": direction,
    "application_payload_bytes": payload,
    "attempts": len(rows),
    "accepted": len(accepted),
    "failed": failed,
    "receiver_rows": len(receiver_rows),
    "rtt_count": len(rtts),
    "rtt_p50_ms": percentile(rtts, 0.50),
    "rtt_p95_ms": percentile(rtts, 0.95),
    "rtt_p99_ms": percentile(rtts, 0.99),
    "rtt_max_ms": max(rtts) if rtts else None,
    "wire_request_bytes_min": min(wire_sizes) if wire_sizes else None,
    "wire_request_bytes_max": max(wire_sizes) if wire_sizes else None,
    "deadline_misses_100ms": expected - sum(v <= 100 for v in rtts),
    "deadline_misses_500ms": expected - sum(v <= 500 for v in rtts),
    "deadline_misses_1000ms": expected - sum(v <= 1000 for v in rtts),
    "errors": errors,
}
if direction == "downlink":
    summary.update({
        "payload_validation_count": len(payload_validation),
        "payload_validation_p50_ms": percentile(payload_validation, 0.50),
        "payload_validation_p95_ms": percentile(payload_validation, 0.95),
        "payload_validation_p99_ms": percentile(payload_validation, 0.99),
        "payload_validation_max_ms": max(payload_validation) if payload_validation else None,
        "payload_validation_included_in_rtt": True,
    })
(root / "validation_summary.json").write_text(
    json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8"
)
print(json.dumps(summary, sort_keys=True))
if errors:
    raise SystemExit(1)

manifest_path = root / "run_manifest.json"
manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
manifest["status"] = "complete"
manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")
(root / "complete.marker").touch()
PY
}

preflight() {
  command -v ssh rsync timeout python3 sha256sum >/dev/null
  [[ -x "$LOCAL_PROBE" ]]
  [[ -f "$REPO_ROOT/scripts/collect_host_telemetry.py" ]]
  [[ -f "$GPS_EVIDENCE_ROOT/location/gps/rosbag/metadata.yaml" ]]
  [[ -f "$GPS_EVIDENCE_ROOT/location/gps/gps_samples.csv" ]]
  ip route get "$EDGE_HOST" | grep -q 'dev eno2' || {
    echo "Edge traffic is not routed through eno2" >&2
    return 1
  }
  "${SSH_BASE[@]}" true
  ssh_edge_bash "test -f '$EDGE_DEPLOY_ROOT/scripts/threaded_mqtt_broker.py'; test -f '$EDGE_DEPLOY_ROOT/scripts/collect_host_telemetry.py'; install -d '$REMOTE_RUN_DIR/tools'"
  rsync -az -e "$RSYNC_RSH" "$LOCAL_PROBE" "$EDGE_USER@$EDGE_HOST:$REMOTE_PROBE"
  local local_hash remote_hash
  local_hash="$(sha256sum "$LOCAL_PROBE" | awk '{print $1}')"
  remote_hash="$(ssh_edge_bash "sha256sum '$REMOTE_PROBE'" | awk '{print $1}')"
  if [[ "$local_hash" != "$remote_hash" ]]; then
    echo "Raw probe source mismatch after edge deployment" >&2
    return 1
  fi
  ssh_edge_bash "python3 '$REMOTE_PROBE' --self-test >/dev/null"

  for transport in "${TRANSPORT_LIST[@]}"; do
    for payload in "${PAYLOAD_LIST[@]}"; do
      local local_dir="$RAW_RUN_DIR/application/$transport/payload_${payload}"
      local remote_dir="$REMOTE_RUN_DIR/application/$transport/payload_${payload}"
      if [[ -e "$local_dir" && ! -f "$local_dir/complete.marker" ]]; then
        echo "Refusing to overwrite incomplete local condition: $local_dir" >&2
        return 1
      fi
      if [[ ! -f "$local_dir/complete.marker" ]]; then
        ssh_edge_bash "if test -e '$remote_dir'; then echo 'remote condition already exists' >&2; exit 1; fi"
      fi
    done
  done
}

reserve_condition() {
  local transport="$1"
  local payload="$2"
  CURRENT_LOCAL_DIR="$RAW_RUN_DIR/application/$transport/payload_${payload}"
  CURRENT_REMOTE_DIR="$REMOTE_RUN_DIR/application/$transport/payload_${payload}"
  CURRENT_REMOTE_PID_DIR="$CURRENT_REMOTE_DIR/pids"
  mkdir -p "$CURRENT_LOCAL_DIR/host_telemetry" "$CURRENT_LOCAL_DIR/edge"
  ssh_edge_bash "install -d '$CURRENT_REMOTE_PID_DIR' '$CURRENT_REMOTE_DIR/logs' '$CURRENT_REMOTE_DIR/host_telemetry'"
  CONDITION_RESERVED=1
}

start_telemetry() {
  python3 "$REPO_ROOT/scripts/collect_host_telemetry.py" \
    --output "$CURRENT_LOCAL_DIR/host_telemetry/car.jsonl" \
    --interval-s 1 \
    --interface eno2 \
    --process-match 'private_5g_raw_(bulk|downlink)_probe.py.*--role sender' &
  LOCAL_TELEMETRY_PID=$!
  start_remote_process telemetry telemetry.out \
    "python3 '$EDGE_DEPLOY_ROOT/scripts/collect_host_telemetry.py' --output '$CURRENT_REMOTE_DIR/host_telemetry/d1.jsonl' --interval-s 1 --interface enp0s31f6 --process-match 'private_5g_raw_(bulk|downlink)_probe.py.*--role receiver|threaded_mqtt_broker.py'"
}

start_stack() {
  local transport="$1"
  local payload="$2"
  local condition_id="$3"
  local common="--role receiver --run-id '$RUN_ID' --condition-id '$condition_id' --source-id veh-001 --intersection-id airspan-tdd-$APPLICATION_DIRECTION-$LOCATION_ID --max-payload-bytes 134217728"
  if [[ "$transport" == "tcp" ]]; then
    start_remote_process receiver receiver.csv \
      "python3 -u '$REMOTE_PROBE' --transport tcp --port '$TCP_PORT' $common"
    wait_edge_port "$TCP_PORT" "TCP receiver"
  else
    start_remote_process broker broker.out \
      "python3 '$EDGE_DEPLOY_ROOT/scripts/threaded_mqtt_broker.py' --host 0.0.0.0 --port '$MQTT_PORT'"
    wait_edge_port "$MQTT_PORT" "MQTT broker"
    start_remote_process receiver receiver.csv \
      "python3 -u '$REMOTE_PROBE' --transport mqtt --host 127.0.0.1 --port '$MQTT_PORT' $common"
    sleep 3
    ssh_edge_bash "p=\$(cat '$CURRENT_REMOTE_PID_DIR/receiver.pid'); kill -0 \"\$p\""
  fi
}

run_condition() {
  local transport="$1"
  local payload="$2"
  local condition_id="${APPLICATION_DIRECTION}-${transport}-payload-${payload}"
  local condition_count="$COUNT"
  if [[ "$payload" == "512000" ]]; then
    condition_count="$PAYLOAD_512000_COUNT"
  elif [[ "$payload" == "1048576" ]]; then
    condition_count="$PAYLOAD_1048576_COUNT"
  fi
  local local_dir="$RAW_RUN_DIR/application/$transport/payload_${payload}"
  if [[ -f "$local_dir/complete.marker" ]]; then
    log "Skipping completed $transport payload $payload"
    return 0
  fi

  local port="$TCP_PORT"
  if [[ "$transport" == "mqtt" ]]; then
    port="$MQTT_PORT"
  fi
  ssh_edge_bash "if ss -ltnp 2>/dev/null | grep -Eq ':$port( |$)'; then echo 'planned edge port occupied' >&2; exit 1; fi"

  reserve_condition "$transport" "$payload"
  write_manifest "$transport" "$payload" "$condition_id" "$condition_count"
  printf '%s\n' "$(basename "$LOCAL_PROBE") --role sender --transport $transport --host <edge-host> --port $port --count $condition_count --interval-ms $INTERVAL_MS --payload-bytes $payload" > "$CURRENT_LOCAL_DIR/commands.txt"
  start_telemetry
  start_stack "$transport" "$payload" "$condition_id"

  local args=(
    --role sender
    --transport "$transport"
    --host "$EDGE_HOST"
    --port "$port"
    --count "$condition_count"
    --interval-ms "$INTERVAL_MS"
    --payload-bytes "$payload"
    --intersection-id "airspan-tdd-$APPLICATION_DIRECTION-$LOCATION_ID"
    --source-id veh-001
    --run-id "$RUN_ID"
    --condition-id "$condition_id"
    --max-payload-bytes 134217728
    --timeout-ms "$MQTT_TIMEOUT_MS"
  )

  local started finished sender_status
  started="$(date --iso-8601=seconds)"
  log "Starting $transport payload $payload at $started"
  set +e
  timeout --foreground --signal=TERM --kill-after=30s "${MAX_CONDITION_SECONDS}s" \
    python3 -u "$LOCAL_PROBE" \
    "${args[@]}" > "$CURRENT_LOCAL_DIR/sender.csv" 2> "$CURRENT_LOCAL_DIR/sender.err"
  sender_status=$?
  set -e
  finished="$(date --iso-8601=seconds)"
  record_timing "$started" "$finished"
  printf '%s\n' "$sender_status" > "$CURRENT_LOCAL_DIR/sender.exit_status"

  stop_local_telemetry
  stop_remote_processes
  fetch_remote_artifacts
  CONDITION_RESERVED=0

  if (( sender_status != 0 )); then
    echo "$transport payload $payload sender exited with status $sender_status" >&2
    return 1
  fi
  validate_condition "$transport" "$payload" "$condition_id" "$condition_count"
  log "Completed $transport payload $payload at $finished"
  CURRENT_LOCAL_DIR=""
  CURRENT_REMOTE_DIR=""
  CURRENT_REMOTE_PID_DIR=""
}

main() {
  log "Running $APPLICATION_DIRECTION payload-sweep preflight"
  preflight
  if [[ "$PREFLIGHT_ONLY" == "1" ]]; then
    log "Preflight passed"
    return 0
  fi
  mkdir -p "$RAW_RUN_DIR/application"
  for transport in "${TRANSPORT_LIST[@]}"; do
    for payload in "${PAYLOAD_LIST[@]}"; do
      run_condition "$transport" "$payload"
    done
  done
  log "All $((${#TRANSPORT_LIST[@]} * ${#PAYLOAD_LIST[@]})) payload conditions complete"
}

main
