#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: run_airspan_followup_condition.sh <C1|C2|C3|C4|C5|C6> <repetition> <tcp|mqtt|udp>

Required environment:
  EDGE_HOST          Reachable d1 edge address (kept out of result metadata)
  SSHPASS            Password used by sshpass (never written to disk)
  SIGNAL_CLASS       Operator-declared location class, such as weak or strong
  LOCATION_ID        Stable, non-sensitive location label for this run block
  PHYSICAL_PLACEMENT Actual MG52 placement/orientation label
  ACP_BIN_START      ISO-8601 start of the assigned ACP bin
  ACP_BIN_END        ISO-8601 end of the assigned ACP bin

Important optional environment:
  RUN_NAME           Default: YYYYMMDD_airspan_followup_run_1
  EDGE_USER          Default: d1
  EDGE_DEPLOY_ROOT   Default: /home/d1/edge4av_followup/ipi_2c043b3
  COUNT              Default: 500 probes per client for five-minute samples
  INTERVAL_MS        Default: 200
  MAX_TRANSPORT_SECONDS
                     Default: 260; hard sender timeout inside the ACP sample
  WORKLOAD_OFFSET_S  Default: 15 seconds after the ACP sample starts
  BIN_END_GUARD_S    Default: 15 seconds before the ACP sample ends
  ENABLE_GNSS        Default: 1
  DRY_RUN            Set to 1 to validate and print the plan without traffic
  PREFLIGHT_ONLY      Set to 1 to check both hosts without reserving paths

The assigned ACP interval must be exactly five minutes. Start the runner before
ACP_BIN_START so telemetry, GNSS, and receivers are ready by the workload
offset. C4 over MQTT needs the longest setup lead because it starts 100 logical
receivers.
EOF
}

if [[ "$#" -ne 3 ]]; then
  usage >&2
  exit 2
fi

CONDITION="${1^^}"
REPETITION="$2"
TRANSPORT="${3,,}"

if [[ ! "$REPETITION" =~ ^[1-9][0-9]*$ ]]; then
  echo "repetition must be a positive integer" >&2
  exit 2
fi
if [[ ! "$TRANSPORT" =~ ^(tcp|mqtt|udp)$ ]]; then
  echo "transport must be tcp, mqtt, or udp" >&2
  exit 2
fi

case "$CONDITION" in
  C1)
    PAYLOAD_BYTES=1024
    CLIENTS=1
    LOAD_MBPS=0
    ;;
  C2)
    PAYLOAD_BYTES=23968
    CLIENTS=1
    LOAD_MBPS=0
    ;;
  C3)
    PAYLOAD_BYTES=1024
    CLIENTS=1
    LOAD_MBPS=25
    ;;
  C4)
    PAYLOAD_BYTES=1024
    CLIENTS=100
    LOAD_MBPS=0
    ;;
  C5)
    PAYLOAD_BYTES=1024
    CLIENTS=1
    LOAD_MBPS=0
    ;;
  C6)
    PAYLOAD_BYTES=23968
    CLIENTS=1
    LOAD_MBPS=0
    ;;
  *)
    echo "condition must be C1, C2, C3, C4, C5, or C6" >&2
    exit 2
    ;;
esac

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUN_NAME="${RUN_NAME:-$(date +%Y%m%d)_airspan_followup_run_1}"
RUN_ID="${RUN_ID:-edge4av-real-${RUN_NAME//_/-}}"
EDGE_USER="${EDGE_USER:-d1}"
EDGE_DEPLOY_ROOT="${EDGE_DEPLOY_ROOT:-/home/d1/edge4av_followup/ipi_2c043b3}"
REMOTE_RUN_ROOT="${REMOTE_RUN_ROOT:-/home/d1/edge4av_followup/runs}"
COUNT="${COUNT:-500}"
INTERVAL_MS="${INTERVAL_MS:-200}"
TCP_PORT="${TCP_PORT:-36666}"
UDP_PORT="${UDP_PORT:-36667}"
MQTT_PORT="${MQTT_PORT:-1883}"
LOAD_PORT="${LOAD_PORT:-39000}"
UDP_TIMEOUT_MS="${UDP_TIMEOUT_MS:-5000}"
UDP_MAX_DATAGRAM_BYTES="${UDP_MAX_DATAGRAM_BYTES:-1400}"
MQTT_TIMEOUT_MS="${MQTT_TIMEOUT_MS:-60000}"
MAX_TRANSPORT_SECONDS="${MAX_TRANSPORT_SECONDS:-260}"
WORKLOAD_OFFSET_S="${WORKLOAD_OFFSET_S:-15}"
BIN_END_GUARD_S="${BIN_END_GUARD_S:-15}"
START_LATE_TOLERANCE_S="${START_LATE_TOLERANCE_S:-10}"
MIN_RUNTIME_HEADROOM_S="${MIN_RUNTIME_HEADROOM_S:-30}"
ENABLE_GNSS="${ENABLE_GNSS:-1}"
DRY_RUN="${DRY_RUN:-0}"
PREFLIGHT_ONLY="${PREFLIGHT_ONLY:-0}"
TDD_PROFILE="70/20/10"
FRAME_PACKING="10D4G"
AIRSPAN_CELL="2"
: "${SIGNAL_CLASS:?Set SIGNAL_CLASS to the operator-declared location class}"
: "${LOCATION_ID:?Set LOCATION_ID to a stable, non-sensitive location label}"
: "${PHYSICAL_PLACEMENT:?Set PHYSICAL_PLACEMENT to the actual MG52 placement/orientation}"
SIGNAL_PLACEMENT="$PHYSICAL_PLACEMENT"
CONDITION_LOWER="${CONDITION,,}"
CONDITION_ID="${CONDITION_LOWER}-rep${REPETITION}-${TRANSPORT}"
CONDITION_LABEL="airspan-followup-70-20-10-${SIGNAL_CLASS}"
INTERSECTION_ID="airspan-followup-${CONDITION_LOWER}-rep${REPETITION}"
if (( LOAD_MBPS > 0 )); then
  NETWORK_LOAD_LEVEL="uplink-${LOAD_MBPS}mbps"
elif (( CLIENTS > 1 )); then
  NETWORK_LOAD_LEVEL="${CLIENTS}-clients"
else
  NETWORK_LOAD_LEVEL="idle"
fi

for value_name in RUN_NAME RUN_ID CONDITION_ID SIGNAL_CLASS LOCATION_ID PHYSICAL_PLACEMENT; do
  value="${!value_name}"
  if [[ ! "$value" =~ ^[A-Za-z0-9_.-]+$ ]]; then
    echo "$value_name contains unsupported characters: $value" >&2
    exit 2
  fi
done
for number_name in \
  COUNT INTERVAL_MS TCP_PORT UDP_PORT MQTT_PORT LOAD_PORT \
  UDP_TIMEOUT_MS UDP_MAX_DATAGRAM_BYTES MQTT_TIMEOUT_MS \
  MAX_TRANSPORT_SECONDS WORKLOAD_OFFSET_S BIN_END_GUARD_S \
  START_LATE_TOLERANCE_S MIN_RUNTIME_HEADROOM_S
do
  number="${!number_name}"
  if [[ ! "$number" =~ ^[0-9]+$ ]]; then
    echo "$number_name must be an unsigned integer" >&2
    exit 2
  fi
done
if (( COUNT == 0 || INTERVAL_MS == 0 || MAX_TRANSPORT_SECONDS == 0 )); then
  echo "COUNT, INTERVAL_MS, and MAX_TRANSPORT_SECONDS must be positive" >&2
  exit 2
fi

: "${ACP_BIN_START:?Set ACP_BIN_START to the assigned ISO-8601 ACP-bin start}"
: "${ACP_BIN_END:?Set ACP_BIN_END to the assigned ISO-8601 ACP-bin end}"

BIN_START_EPOCH="$(date --date "$ACP_BIN_START" +%s)"
BIN_END_EPOCH="$(date --date "$ACP_BIN_END" +%s)"
if (( BIN_END_EPOCH <= BIN_START_EPOCH )); then
  echo "ACP_BIN_END must be later than ACP_BIN_START" >&2
  exit 2
fi
BIN_DURATION_S=$((BIN_END_EPOCH - BIN_START_EPOCH))
if (( BIN_DURATION_S != 300 )); then
  echo "The assigned ACP interval must be exactly 5 minutes" >&2
  exit 2
fi

AVAILABLE_WORKLOAD_SECONDS=$((BIN_DURATION_S - WORKLOAD_OFFSET_S - BIN_END_GUARD_S))
if (( AVAILABLE_WORKLOAD_SECONDS <= 0 )); then
  echo "WORKLOAD_OFFSET_S and BIN_END_GUARD_S consume the entire ACP interval" >&2
  exit 2
fi
if (( START_LATE_TOLERANCE_S > BIN_END_GUARD_S )); then
  echo "START_LATE_TOLERANCE_S cannot exceed BIN_END_GUARD_S" >&2
  exit 2
fi
if (( MAX_TRANSPORT_SECONDS > AVAILABLE_WORKLOAD_SECONDS )); then
  echo "MAX_TRANSPORT_SECONDS exceeds the guarded ACP workload window" >&2
  exit 2
fi
NOMINAL_INTERVAL_SPAN_MS=$(((COUNT - 1) * INTERVAL_MS))
AVAILABLE_WORKLOAD_MS=$((AVAILABLE_WORKLOAD_SECONDS * 1000))
MINIMUM_BUDGET_MS=$((NOMINAL_INTERVAL_SPAN_MS + MIN_RUNTIME_HEADROOM_S * 1000))
if (( MINIMUM_BUDGET_MS > AVAILABLE_WORKLOAD_MS )); then
  echo "COUNT and INTERVAL_MS leave less than ${MIN_RUNTIME_HEADROOM_S}s for request RTT and startup" >&2
  exit 2
fi
if (( MAX_TRANSPORT_SECONDS * 1000 < NOMINAL_INTERVAL_SPAN_MS )); then
  echo "MAX_TRANSPORT_SECONDS is shorter than the configured inter-probe schedule" >&2
  exit 2
fi

if [[ "$DRY_RUN" == "1" ]]; then
  printf 'condition=%s\n' "$CONDITION"
  printf 'repetition=%s\n' "$REPETITION"
  printf 'transport=%s\n' "$TRANSPORT"
  printf 'payload_bytes=%s\n' "$PAYLOAD_BYTES"
  printf 'clients=%s\n' "$CLIENTS"
  printf 'load_mbps=%s\n' "$LOAD_MBPS"
  printf 'signal_class=%s\n' "$SIGNAL_CLASS"
  printf 'location_id=%s\n' "$LOCATION_ID"
  printf 'physical_placement=%s\n' "$PHYSICAL_PLACEMENT"
  printf 'tdd_profile=%s\n' "$TDD_PROFILE"
  printf 'airspan_cell=%s\n' "$AIRSPAN_CELL"
  printf 'acp_bin=%s to %s\n' "$ACP_BIN_START" "$ACP_BIN_END"
  printf 'acp_bin_seconds=%s\n' "$BIN_DURATION_S"
  printf 'workload_offset_s=%s\n' "$WORKLOAD_OFFSET_S"
  printf 'bin_end_guard_s=%s\n' "$BIN_END_GUARD_S"
  printf 'max_transport_seconds=%s\n' "$MAX_TRANSPORT_SECONDS"
  printf 'nominal_interval_span_ms=%s\n' "$NOMINAL_INTERVAL_SPAN_MS"
  exit 0
fi

: "${EDGE_HOST:?Set EDGE_HOST to the reachable d1 edge address}"
: "${SSHPASS:?Set SSHPASS for password-based edge SSH}"

RUN_DIR="$REPO_ROOT/results/real_5g/$RUN_NAME"
TRANSPORT_DIR="$RUN_DIR/application/$CONDITION_LOWER/rep_${REPETITION}/$TRANSPORT"
REMOTE_TRANSPORT_DIR="$REMOTE_RUN_ROOT/$RUN_NAME/application/$CONDITION_LOWER/rep_${REPETITION}/$TRANSPORT"
REMOTE_PID_DIR="$REMOTE_TRANSPORT_DIR/pids"
REMOTE_LOG_DIR="$REMOTE_TRANSPORT_DIR/logs"
REMOTE_TELEMETRY_DIR="$REMOTE_TRANSPORT_DIR/host_telemetry"
LOCAL_TELEMETRY_DIR="$TRANSPORT_DIR/host_telemetry"
COMMAND_LOG="$TRANSPORT_DIR/commands.txt"

SSH_OPTS=(-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null)
SSH_BASE=(sshpass -e ssh "${SSH_OPTS[@]}" "$EDGE_USER@$EDGE_HOST")
RSYNC_RSH="sshpass -e ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null"

LOCAL_TELEMETRY_PID=""
LOCAL_LOAD_PID=""
GPS_PID=""
WORKLOAD_STARTED_AT=""
WORKLOAD_FINISHED_AT=""
RUN_RESERVED=0

log() {
  printf '[%s] %s\n' "$(date -Iseconds)" "$*" | tee -a "$TRANSPORT_DIR/run.log"
}

ssh_edge_bash() {
  local remote_script="$1"
  "${SSH_BASE[@]}" "bash -lc $(printf '%q' "$remote_script")"
}

record_command() {
  local command="$*"
  command="${command//$EDGE_HOST/<edge-host>}"
  printf '%s\n' "$command" >> "$COMMAND_LOG"
}

wait_tcp_port() {
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

stop_local_pid() {
  local pid="$1"
  if [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null; then
    kill -TERM "$pid" 2>/dev/null || true
    for _ in $(seq 1 20); do
      if ! kill -0 "$pid" 2>/dev/null; then
        wait "$pid" 2>/dev/null || true
        return
      fi
      sleep 0.1
    done
    kill -KILL "$pid" 2>/dev/null || true
    wait "$pid" 2>/dev/null || true
  fi
}

finish_local_load() {
  if [[ -z "$LOCAL_LOAD_PID" ]]; then
    return 0
  fi

  local deadline=$((BIN_END_EPOCH - BIN_END_GUARD_S + 5))
  local state=""
  while kill -0 "$LOCAL_LOAD_PID" 2>/dev/null; do
    state="$(ps -o stat= -p "$LOCAL_LOAD_PID" 2>/dev/null || true)"
    if [[ "$state" == Z* ]]; then
      break
    fi
    if (( $(date +%s) > deadline )); then
      stop_local_pid "$LOCAL_LOAD_PID"
      LOCAL_LOAD_PID=""
      echo "Load generator did not finish by the guarded deadline" >&2
      return 1
    fi
    sleep 0.1
  done

  local status=0
  if ! wait "$LOCAL_LOAD_PID"; then
    status=1
  fi
  LOCAL_LOAD_PID=""
  if (( status != 0 )); then
    echo "Load generator exited nonzero" >&2
  fi
  return "$status"
}

stop_remote_pids() {
  ssh_edge_bash "if test -d '$REMOTE_PID_DIR'; then for pid_file in '$REMOTE_PID_DIR'/*.pid; do test -f \"\$pid_file\" || continue; pid=\$(cat \"\$pid_file\" 2>/dev/null || true); if test -n \"\$pid\" && kill -0 \"\$pid\" 2>/dev/null; then kill -TERM \"\$pid\" 2>/dev/null || true; fi; done; sleep 1; for pid_file in '$REMOTE_PID_DIR'/*.pid; do test -f \"\$pid_file\" || continue; pid=\$(cat \"\$pid_file\" 2>/dev/null || true); if test -n \"\$pid\" && kill -0 \"\$pid\" 2>/dev/null; then kill -KILL \"\$pid\" 2>/dev/null || true; fi; done; fi" >/dev/null 2>&1 || true
}

fetch_remote_artifacts() {
  rsync -az -e "$RSYNC_RSH" \
    "$EDGE_USER@$EDGE_HOST:$REMOTE_TRANSPORT_DIR/" \
    "$TRANSPORT_DIR/edge/" \
    >> "$TRANSPORT_DIR/rsync_from_edge.log" 2>&1 || true
}

stop_gnss() {
  if [[ -n "$GPS_PID" ]]; then
    stop_local_pid "$GPS_PID"
    GPS_PID=""
  fi
}

cleanup() {
  set +e
  if [[ "$RUN_RESERVED" != "1" ]]; then
    return
  fi
  if [[ -n "$LOCAL_LOAD_PID" ]]; then
    stop_local_pid "$LOCAL_LOAD_PID"
    LOCAL_LOAD_PID=""
  fi
  if [[ -n "$LOCAL_TELEMETRY_PID" ]]; then
    stop_local_pid "$LOCAL_TELEMETRY_PID"
    LOCAL_TELEMETRY_PID=""
  fi
  stop_gnss
  stop_remote_pids
  fetch_remote_artifacts
}
trap cleanup EXIT INT TERM

write_manifest() {
  python3 -c '
import json, sys
path = sys.argv[1]
keys = [
    "run_id", "run_name", "condition", "repetition", "transport",
    "payload_bytes", "clients", "load_mbps", "signal_class", "location_id",
    "physical_placement", "signal_placement", "tdd_profile", "frame_packing", "airspan_cell",
    "acp_bin_start", "acp_bin_end", "count_per_client", "interval_ms",
    "acp_bin_seconds", "workload_offset_s", "bin_end_guard_s",
    "max_transport_seconds", "start_late_tolerance_s",
    "minimum_runtime_headroom_s", "nominal_interval_span_ms"
]
values = sys.argv[2:]
data = dict(zip(keys, values))
for key in (
    "repetition", "payload_bytes", "clients", "load_mbps", "airspan_cell",
    "count_per_client", "interval_ms", "acp_bin_seconds",
    "workload_offset_s", "bin_end_guard_s", "max_transport_seconds",
    "start_late_tolerance_s", "minimum_runtime_headroom_s",
    "nominal_interval_span_ms"
):
    data[key] = int(data[key])
data.update({
    "timing_metric": "rtt",
    "clock_sync_state": "unsynced",
    "phone_logger_used": False,
    "edge_role": "d1-wired-receiver",
    "acp_mg52_state": "operator-verified-artifacts-pending",
})
with open(path, "w", encoding="utf-8") as handle:
    json.dump(data, handle, indent=2, sort_keys=True)
    handle.write("\n")
' "$TRANSPORT_DIR/run_manifest.json" \
    "$RUN_ID" "$RUN_NAME" "$CONDITION" "$REPETITION" "$TRANSPORT" \
    "$PAYLOAD_BYTES" "$CLIENTS" "$LOAD_MBPS" "$SIGNAL_CLASS" "$LOCATION_ID" \
    "$PHYSICAL_PLACEMENT" "$SIGNAL_PLACEMENT" "$TDD_PROFILE" "$FRAME_PACKING" "$AIRSPAN_CELL" \
    "$ACP_BIN_START" "$ACP_BIN_END" "$COUNT" "$INTERVAL_MS" \
    "$BIN_DURATION_S" "$WORKLOAD_OFFSET_S" "$BIN_END_GUARD_S" \
    "$MAX_TRANSPORT_SECONDS" "$START_LATE_TOLERANCE_S" \
    "$MIN_RUNTIME_HEADROOM_S" "$NOMINAL_INTERVAL_SPAN_MS"
}

preflight() {
  local local_binary
  if [[ "$TRANSPORT" == "udp" ]]; then
    local_binary="$REPO_ROOT/cpp/build/example_private_5g_latency_udp_sender"
  else
    local_binary="$REPO_ROOT/cpp/build/example_private_5g_latency_sender"
  fi
  [[ -x "$local_binary" ]] || { echo "Missing local sender: $local_binary" >&2; return 1; }
  [[ -f "$REPO_ROOT/scripts/collect_host_telemetry.py" ]] || { echo "Missing local telemetry collector" >&2; return 1; }
  command -v sshpass rsync timeout >/dev/null
  ip route get "$EDGE_HOST" | grep -q 'dev eno2' || {
    echo "d1 is not routed through the expected car-side eno2 interface" >&2
    return 1
  }

  local remote_receiver="example_private_5g_latency_receiver"
  local planned_port="$TCP_PORT"
  if [[ "$TRANSPORT" == "udp" ]]; then
    remote_receiver="example_private_5g_latency_udp_receiver"
    planned_port="$UDP_PORT"
  elif [[ "$TRANSPORT" == "mqtt" ]]; then
    planned_port="$MQTT_PORT"
  fi
  if [[ -e "$TRANSPORT_DIR" ]]; then
    echo "Refusing to overwrite existing condition directory: $TRANSPORT_DIR" >&2
    return 1
  fi
  ssh_edge_bash "test -x '$EDGE_DEPLOY_ROOT/cpp/build/$remote_receiver'; test -f '$EDGE_DEPLOY_ROOT/scripts/collect_host_telemetry.py'; test -f '$EDGE_DEPLOY_ROOT/scripts/threaded_mqtt_broker.py'; test -f '$EDGE_DEPLOY_ROOT/scripts/edge4av_loadgen.py'; if ss -lntup 2>/dev/null | grep -Eq ':($planned_port|$LOAD_PORT)( |\$)'; then echo 'planned port already occupied' >&2; exit 1; fi; if test -e '$REMOTE_TRANSPORT_DIR'; then echo 'remote condition directory already exists' >&2; exit 1; fi"
}

reserve_paths() {
  mkdir -p "$TRANSPORT_DIR/senders" "$TRANSPORT_DIR/edge" "$LOCAL_TELEMETRY_DIR" "$TRANSPORT_DIR/gps"
  ssh_edge_bash "install -d '$REMOTE_PID_DIR' '$REMOTE_LOG_DIR' '$REMOTE_TELEMETRY_DIR'"
  RUN_RESERVED=1
}

start_remote_process() {
  local name="$1"
  shift
  local command="$*"
  local pid_file="$REMOTE_PID_DIR/$name.pid"
  local stdout_file="$REMOTE_LOG_DIR/$name.out"
  if [[ "$name" == *_receiver || "$name" == mqtt_receiver_* || "$name" == load_server ]]; then
    stdout_file="$REMOTE_LOG_DIR/$name.csv"
  fi
  local stderr_file="$REMOTE_LOG_DIR/$name.err"
  ssh_edge_bash "setsid -f bash -c \"echo \\\$\\\$ > '$pid_file'; exec $command > '$stdout_file' 2> '$stderr_file' < /dev/null\""
}

start_telemetry() {
  log "Starting car and d1 host telemetry"
  python3 "$REPO_ROOT/scripts/collect_host_telemetry.py" \
    --output "$LOCAL_TELEMETRY_DIR/car.jsonl" \
    --interval-s 1 \
    --interface eno2 \
    --process-match 'example_private_5g_latency_(udp_)?sender|edge4av_loadgen' &
  LOCAL_TELEMETRY_PID=$!

  start_remote_process telemetry \
    "python3 '$EDGE_DEPLOY_ROOT/scripts/collect_host_telemetry.py' --output '$REMOTE_TELEMETRY_DIR/d1.jsonl' --interval-s 1 --interface enp0s31f6 --process-match 'example_private_5g_latency_(udp_)?receiver|threaded_mqtt_broker|edge4av_loadgen'"
}

start_gnss() {
  if [[ "$ENABLE_GNSS" != "1" ]]; then
    return
  fi
  log "Starting GNSS recorder"
  bash "$REPO_ROOT/scripts/record_gps_for_experiment.sh" "$TRANSPORT_DIR" \
    > "$TRANSPORT_DIR/gps_capture_stdout.log" \
    2> "$TRANSPORT_DIR/gps_capture_stderr.log" &
  GPS_PID=$!
  sleep 5
  if ! kill -0 "$GPS_PID" 2>/dev/null; then
    log "GNSS recorder exited early; preserving its logs"
    GPS_PID=""
  fi
}

receiver_common="--run-id '$RUN_ID' --condition-id '$CONDITION_ID' --condition-label '$CONDITION_LABEL' --rsu-id rsu-1 --network-load-level '$NETWORK_LOAD_LEVEL' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name payload_bytes --vehicle-outcome-value '$PAYLOAD_BYTES' --vehicle-outcome-unit bytes --csv"

start_tcp_receiver() {
  start_remote_process tcp_receiver \
    "stdbuf -oL -eL '$EDGE_DEPLOY_ROOT/cpp/build/example_private_5g_latency_receiver' --transport tcp --port '$TCP_PORT' $receiver_common"
  wait_tcp_port "$TCP_PORT" "TCP receiver"
}

start_udp_receiver() {
  start_remote_process udp_receiver \
    "stdbuf -oL -eL '$EDGE_DEPLOY_ROOT/cpp/build/example_private_5g_latency_udp_receiver' --port '$UDP_PORT' $receiver_common"
  sleep 2
}

source_id() {
  printf 'veh-%03d' "$1"
}

start_mqtt_stack() {
  start_remote_process mqtt_broker \
    "python3 '$EDGE_DEPLOY_ROOT/scripts/threaded_mqtt_broker.py' --host 0.0.0.0 --port '$MQTT_PORT'"
  wait_tcp_port "$MQTT_PORT" "MQTT broker"
  for i in $(seq 1 "$CLIENTS"); do
    local src
    src="$(source_id "$i")"
    start_remote_process "mqtt_receiver_$src" \
      "stdbuf -oL -eL '$EDGE_DEPLOY_ROOT/cpp/build/example_private_5g_latency_receiver' --transport mqtt --host 127.0.0.1 --port '$MQTT_PORT' --intersection-id '$INTERSECTION_ID' --source-id '$src' $receiver_common"
  done
  sleep 3
}

wait_for_workload_window() {
  local target=$((BIN_START_EPOCH + WORKLOAD_OFFSET_S))
  local latest_start=$((target + START_LATE_TOLERANCE_S))
  local now
  now="$(date +%s)"
  if (( now >= BIN_END_EPOCH - BIN_END_GUARD_S )); then
    echo "The assigned ACP bin has already ended or is inside the end guard" >&2
    return 1
  fi
  while (( now < target )); do
    sleep 1
    now="$(date +%s)"
  done
  if (( now > latest_start )); then
    echo "Setup missed the workload start tolerance for this ACP sample; refusing a shortened run" >&2
    return 1
  fi
}

check_workload_start_deadline() {
  local target=$((BIN_START_EPOCH + WORKLOAD_OFFSET_S))
  local latest_start=$((target + START_LATE_TOLERANCE_S))
  local now
  now="$(date +%s)"
  if (( now > latest_start )); then
    echo "Workload startup crossed the allowed ACP start tolerance; this sample is invalid" >&2
    return 1
  fi
}

start_load() {
  if (( LOAD_MBPS == 0 )); then
    return
  fi
  local remaining=$((BIN_END_EPOCH - BIN_END_GUARD_S - $(date +%s)))
  if (( remaining < 30 )); then
    echo "Insufficient guarded time remains for the background load" >&2
    return 1
  fi
  local server_duration=$((remaining + 5))
  start_remote_process load_server \
    "python3 '$EDGE_DEPLOY_ROOT/scripts/edge4av_loadgen.py' --role server --protocol tcp --bind 0.0.0.0 --port '$LOAD_PORT' --duration-s '$server_duration'"
  wait_tcp_port "$LOAD_PORT" "load server"
  record_command "python3 scripts/edge4av_loadgen.py --role client --protocol tcp --host <edge-host> --port $LOAD_PORT --duration-s $remaining --target-mbps $LOAD_MBPS"
  python3 "$REPO_ROOT/scripts/edge4av_loadgen.py" \
    --role client --protocol tcp --host "$EDGE_HOST" --port "$LOAD_PORT" \
    --duration-s "$remaining" --target-mbps "$LOAD_MBPS" \
    > "$TRANSPORT_DIR/load_client.csv" \
    2> "$TRANSPORT_DIR/load_client.err" &
  LOCAL_LOAD_PID=$!
  sleep 2
}

validate_load_artifacts() {
  if (( LOAD_MBPS == 0 )); then
    return 0
  fi
  awk -F, '
    NR == 1 {
      if ($7 != "target_mbps" || $11 != "throughput_mbps") exit 1
    }
    NR > 1 {
      rows++
      if (($7 + 0) != expected || ($11 + 0) <= 0) exit 1
    }
    END {
      if (rows != 1) exit 1
    }
  ' expected="$LOAD_MBPS" "$TRANSPORT_DIR/load_client.csv"
}

run_senders() {
  local remaining=$((BIN_END_EPOCH - BIN_END_GUARD_S - $(date +%s)))
  local timeout_seconds="$MAX_TRANSPORT_SECONDS"
  if (( remaining < timeout_seconds )); then
    timeout_seconds="$remaining"
  fi
  if (( timeout_seconds < 30 )); then
    echo "Insufficient time remains in the assigned ACP bin" >&2
    return 1
  fi

  local pids=()
  local labels=()
  for i in $(seq 1 "$CLIENTS"); do
    local src
    src="$(source_id "$i")"
    local sender_csv="$TRANSPORT_DIR/senders/${CONDITION_ID}_${src}_sender.csv"
    local sender_err="$TRANSPORT_DIR/senders/${CONDITION_ID}_${src}_sender.err"
    local common_args=(
      --count "$COUNT"
      --interval-ms "$INTERVAL_MS"
      --message service
      --payload-bytes "$PAYLOAD_BYTES"
      --intersection-id "$INTERSECTION_ID"
      --source-id "$src"
      --run-id "$RUN_ID"
      --condition-id "$CONDITION_ID"
      --condition-label "$CONDITION_LABEL"
      --request-id "$CONDITION_ID-$src"
      --av-id "$src"
      --obu-id "$src"
      --rsu-id rsu-1
      --network-load-level "$NETWORK_LOAD_LEVEL"
      --qos-profile default
      --mobility-state stationary
      --clock-sync-state unsynced
      --service-success true
      --vehicle-outcome-name payload_bytes
      --vehicle-outcome-value "$PAYLOAD_BYTES"
      --vehicle-outcome-unit bytes
      --csv
    )
    local cmd=()
    if [[ "$TRANSPORT" == "tcp" ]]; then
      cmd=("$REPO_ROOT/cpp/build/example_private_5g_latency_sender" --transport tcp --host "$EDGE_HOST" --port "$TCP_PORT" --continue-on-failure "${common_args[@]}")
    elif [[ "$TRANSPORT" == "mqtt" ]]; then
      cmd=("$REPO_ROOT/cpp/build/example_private_5g_latency_sender" --transport mqtt --host "$EDGE_HOST" --port "$MQTT_PORT" --timeout-ms "$MQTT_TIMEOUT_MS" --continue-on-failure "${common_args[@]}")
    else
      cmd=("$REPO_ROOT/cpp/build/example_private_5g_latency_udp_sender" --host "$EDGE_HOST" --port "$UDP_PORT" --timeout-ms "$UDP_TIMEOUT_MS" --udp-max-datagram-bytes "$UDP_MAX_DATAGRAM_BYTES" "${common_args[@]}")
    fi
    record_command "${cmd[*]}"
    timeout --foreground "${timeout_seconds}s" "${cmd[@]}" > "$sender_csv" 2> "$sender_err" &
    pids+=("$!")
    labels+=("$src")
  done

  local status=0
  for index in "${!pids[@]}"; do
    if ! wait "${pids[$index]}"; then
      printf 'sender_group_member=%s exit_nonzero=true\n' "${labels[$index]}" \
        >> "$TRANSPORT_DIR/sender_group.err"
      status=1
    fi
  done
  return "$status"
}

validate_sender_files() {
  python3 -c '
import csv, glob, json, sys
paths = sorted(glob.glob(sys.argv[1]))
expected_files = int(sys.argv[2])
if len(paths) != expected_files:
    raise SystemExit(f"expected {expected_files} sender files, found {len(paths)}")
expected_rows = int(sys.argv[4])
summary = {
    "files": len(paths), "rows": 0, "accepted": 0, "failed": 0,
    "expected_rows_per_file": expected_rows, "complete_files": 0,
    "per_file": {}
}
for path in paths:
    with open(path, newline="", encoding="utf-8") as handle:
        rows = list(csv.DictReader(handle))
    accepted = sum(str(row.get("accepted", "")).lower() == "true" for row in rows)
    summary["rows"] += len(rows)
    summary["accepted"] += accepted
    summary["failed"] += len(rows) - accepted
    if len(rows) == expected_rows:
        summary["complete_files"] += 1
    summary["per_file"][path.rsplit("/", 1)[-1]] = {
        "rows": len(rows), "accepted": accepted,
        "complete": len(rows) == expected_rows
    }
with open(sys.argv[3], "w", encoding="utf-8") as handle:
    json.dump(summary, handle, indent=2, sort_keys=True)
    handle.write("\n")
print(json.dumps(summary, sort_keys=True))
' "$TRANSPORT_DIR/senders/*_sender.csv" "$CLIENTS" "$TRANSPORT_DIR/validation_summary.json" "$COUNT"
}

record_actual_timing() {
  python3 -c '
import json, sys
path = sys.argv[1]
with open(path, encoding="utf-8") as handle:
    data = json.load(handle)
data["workload_started_at"] = sys.argv[2]
data["workload_finished_at"] = sys.argv[3]
with open(path, "w", encoding="utf-8") as handle:
    json.dump(data, handle, indent=2, sort_keys=True)
    handle.write("\n")
' "$TRANSPORT_DIR/run_manifest.json" "$WORKLOAD_STARTED_AT" "$WORKLOAD_FINISHED_AT"
}

main() {
  printf 'Preflight for %s repetition %s over %s\n' "$CONDITION" "$REPETITION" "$TRANSPORT"
  preflight
  if [[ "$PREFLIGHT_ONLY" == "1" ]]; then
    printf 'preflight=passed\n'
    return 0
  fi
  reserve_paths
  write_manifest
  log "Preflight passed for $CONDITION repetition $REPETITION over $TRANSPORT"
  start_telemetry
  start_gnss

  case "$TRANSPORT" in
    tcp) start_tcp_receiver ;;
    mqtt) start_mqtt_stack ;;
    udp) start_udp_receiver ;;
  esac

  wait_for_workload_window
  start_load
  check_workload_start_deadline
  WORKLOAD_STARTED_AT="$(date -Iseconds)"
  log "Starting workload at $WORKLOAD_STARTED_AT"
  local sender_status=0
  if ! run_senders; then
    sender_status=1
  fi
  WORKLOAD_FINISHED_AT="$(date -Iseconds)"
  log "Workload finished at $WORKLOAD_FINISHED_AT"
  record_actual_timing

  local load_status=0
  if [[ -n "$LOCAL_LOAD_PID" ]]; then
    if (( sender_status == 0 )); then
      if ! finish_local_load; then
        load_status=1
      fi
    else
      stop_local_pid "$LOCAL_LOAD_PID"
      LOCAL_LOAD_PID=""
    fi
  fi
  if [[ -n "$LOCAL_TELEMETRY_PID" ]]; then
    stop_local_pid "$LOCAL_TELEMETRY_PID"
    LOCAL_TELEMETRY_PID=""
  fi
  stop_gnss
  stop_remote_pids
  fetch_remote_artifacts
  validate_sender_files
  if (( load_status == 0 )) && ! validate_load_artifacts; then
    log "Load-generator artifact validation failed; artifacts were preserved"
    load_status=1
  fi

  if (( sender_status != 0 )); then
    log "One or more sender processes exited nonzero; artifacts were preserved"
    return 1
  fi
  if (( load_status != 0 )); then
    log "Background load collection failed; artifacts were preserved"
    return 1
  fi
  log "$CONDITION repetition $REPETITION $TRANSPORT complete"
}

main "$@"
