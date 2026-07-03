#!/usr/bin/env bash
set -euo pipefail

RUN_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$RUN_ROOT/../../.." && pwd)"
RUN_NAME="$(basename "$RUN_ROOT")"

EDGE_USER="${EDGE_USER:-d1}"
EDGE_HOST="${EDGE_HOST:-10.100.100.6}"
EDGE_REPO="${EDGE_REPO:-/home/d1/Documents/Github/IPI}"
EDGE_TARGET="${EDGE_USER}@${EDGE_HOST}"
: "${SSHPASS:?SSHPASS must be set for password-based edge SSH.}"

RUN_ID="${RUN_ID:-edge4av-real-20260703-failure-fallback-run-1}"
REMOTE_RUN_DIR="${EDGE_REPO}/results/real_5g/${RUN_NAME}"
INTERSECTION_ID="${INTERSECTION_ID:-failure-fallback-run-1}"
SOURCE_ID="${SOURCE_ID:-veh-01}"
TCP_PORT="${TCP_PORT:-36666}"
UDP_PORT="${UDP_PORT:-36667}"
MQTT_PORT="${MQTT_PORT:-1883}"
PAYLOAD_BYTES="${PAYLOAD_BYTES:-1024}"
COUNT="${COUNT:-1000}"
INTERVAL_MS="${INTERVAL_MS:-200}"
TIMEOUT_MS="${TIMEOUT_MS:-1000}"
TRIGGER_AFTER_SEC="${TRIGGER_AFTER_SEC:-60}"
OUTAGE_SEC="${OUTAGE_SEC:-10}"
TRANSPORTS="${TRANSPORTS:-tcp udp mqtt}"
SENDER_PID=""
ulimit -n 65535 >/dev/null 2>&1 || true

SSH_OPTS=(-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null)

mkdir -p "$RUN_ROOT/base_station" "$RUN_ROOT/commands"

log() {
  printf '[%s] %s\n' "$(date -Iseconds)" "$*" | tee -a "$RUN_ROOT/run.log"
}

now_ns() {
  date +%s%N
}

record_command() {
  printf '%s\n' "$*" >> "$RUN_ROOT/commands/experiment_12_failure_fallback_commands.txt"
}

record_event() {
  local condition="$1"
  local transport="$2"
  local failure_mode="$3"
  local action="$4"
  local detail="$5"
  local event_file="$RUN_ROOT/${condition}_events.csv"
  if [[ ! -f "$event_file" ]]; then
    echo "event_time_ns,event_time_iso,condition_id,transport,failure_mode,action,detail" > "$event_file"
  fi
  printf '%s,%s,%s,%s,%s,%s,%s\n' \
    "$(now_ns)" "$(date -Iseconds)" "$condition" "$transport" "$failure_mode" "$action" "$detail" \
    >> "$event_file"
}

ssh_edge_bash() {
  local remote_script="$1"
  sshpass -e ssh "${SSH_OPTS[@]}" "$EDGE_TARGET" "bash -lc $(printf '%q' "ulimit -n 65535 >/dev/null 2>&1 || true; $remote_script")"
}

remote_cleanup() {
  ssh_edge_bash "pkill -f '[e]xample_private_5g_latency_receiver' || true; pkill -f '[e]xample_private_5g_latency_udp_receiver' || true; pkill -f '[t]hreaded_mqtt_broker.py' || true" >/dev/null 2>&1 || true
}

stop_gnss() {
  if [[ -f "$RUN_ROOT/gps_capture.pid" ]]; then
    local gps_pid
    gps_pid="$(cat "$RUN_ROOT/gps_capture.pid" 2>/dev/null || true)"
    if [[ -n "$gps_pid" ]] && kill -0 "$gps_pid" >/dev/null 2>&1; then
      kill -TERM "$gps_pid" >/dev/null 2>&1 || true
      wait "$gps_pid" >/dev/null 2>&1 || true
    fi
  fi
}

cleanup() {
  remote_cleanup
  stop_gnss
}
trap cleanup EXIT INT TERM

wait_tcp_port() {
  local host="$1"
  local port="$2"
  local label="$3"
  for _ in $(seq 1 60); do
    if timeout 1 bash -c ":</dev/tcp/${host}/${port}" >/dev/null 2>&1; then
      return 0
    fi
    sleep 1
  done
  log "ERROR: timed out waiting for ${label} on ${host}:${port}"
  return 1
}

write_environment() {
  {
    echo "# Experiment 12 Failure/Fallback"
    echo
    echo "- run_id: ${RUN_ID}"
    echo "- run_name: ${RUN_NAME}"
    echo "- edge_host: ${EDGE_HOST}"
    echo "- edge_repo: ${EDGE_REPO}"
    echo "- payload_bytes: ${PAYLOAD_BYTES}"
    echo "- count_per_condition: ${COUNT}"
    echo "- interval_ms: ${INTERVAL_MS}"
    echo "- timeout_ms: ${TIMEOUT_MS}"
    echo "- trigger_after_sec: ${TRIGGER_AFTER_SEC}"
    echo "- outage_sec: ${OUTAGE_SEC}"
    echo "- transports: ${TRANSPORTS}"
    echo "- vehicle_state: stationary"
    echo "- gnss: NovAtel ROS 2 recorder"
    echo "- failure_modes: tcp receiver restart, udp receiver restart, mqtt receiver restart, mqtt broker restart"
    echo "- collection_started: $(date -Iseconds)"
  } > "$RUN_ROOT/environment.md"
}

sync_and_build() {
  log "Building local C++ tools"
  (
    cd "$REPO_ROOT" &&
      cmake -S cpp -B cpp/build &&
      cmake --build cpp/build -j2
  ) > "$RUN_ROOT/local_build.log" 2>&1

  log "Syncing source to edge"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    --exclude '.git/' \
    --exclude 'cpp/build/' \
    --exclude 'results/' \
    "$REPO_ROOT/" "$EDGE_TARGET:$EDGE_REPO/" \
    > "$RUN_ROOT/rsync_cpp_to_base_station.log" 2>&1

  log "Building edge C++ tools"
  ssh_edge_bash "cd '$EDGE_REPO' && cmake -S cpp -B cpp/build && cmake --build cpp/build -j2" \
    > "$RUN_ROOT/base_build.log" 2>&1

  ssh_edge_bash "mkdir -p '$REMOTE_RUN_DIR/base_station' '$REMOTE_RUN_DIR/tools'"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    "$RUN_ROOT/tools/threaded_mqtt_broker.py" \
    "$EDGE_TARGET:$REMOTE_RUN_DIR/tools/threaded_mqtt_broker.py" \
    > "$RUN_ROOT/rsync_mqtt_broker_to_base_station.log" 2>&1
}

start_gnss() {
  log "Starting GNSS recorder"
  bash "$REPO_ROOT/scripts/record_gps_for_experiment.sh" "$RUN_ROOT" \
    > "$RUN_ROOT/gps_capture_stdout.log" \
    2> "$RUN_ROOT/gps_capture_stderr.log" &
  echo "$!" > "$RUN_ROOT/gps_capture.pid"
  sleep 5
}

start_tcp_receiver() {
  local condition="$1"
  local cmd
  cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport tcp --port '$TCP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label failure-fallback --rsu-id rsu-1 --network-load-level failure --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name fallback_mode --vehicle-outcome-value receiver-restart --vehicle-outcome-unit mode --csv >> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2>> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $cmd"
  ssh_edge_bash "$cmd"
  wait_tcp_port "$EDGE_HOST" "$TCP_PORT" "TCP receiver"
}

start_udp_receiver() {
  local condition="$1"
  local cmd
  cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_udp_receiver --port '$UDP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label failure-fallback --rsu-id rsu-1 --network-load-level failure --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name fallback_mode --vehicle-outcome-value receiver-restart --vehicle-outcome-unit mode --csv >> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2>> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $cmd"
  ssh_edge_bash "$cmd"
  sleep 2
}

start_mqtt_broker() {
  local condition="$1"
  local cmd
  cmd="cd '$REMOTE_RUN_DIR/tools' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.pid'; exec python3 threaded_mqtt_broker.py --host 0.0.0.0 --port '$MQTT_PORT' >> '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.log' 2>> '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.err' < /dev/null\""
  record_command "remote: $cmd"
  ssh_edge_bash "$cmd"
  wait_tcp_port "$EDGE_HOST" "$MQTT_PORT" "MQTT broker"
}

start_mqtt_receiver() {
  local condition="$1"
  local failure_mode="$2"
  local cmd
  cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port '$MQTT_PORT' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label failure-fallback --rsu-id rsu-1 --network-load-level failure --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name fallback_mode --vehicle-outcome-value '$failure_mode' --vehicle-outcome-unit mode --csv >> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2>> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $cmd"
  ssh_edge_bash "$cmd"
  sleep 2
}

fetch_condition_artifacts() {
  local condition="$1"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    "$EDGE_TARGET:$REMOTE_RUN_DIR/base_station/${condition}"'*' \
    "$RUN_ROOT/base_station/" \
    > "$RUN_ROOT/rsync_from_base_station_${condition}.log" 2>&1 || true
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    "$EDGE_TARGET:$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}"'*' \
    "$RUN_ROOT/base_station/" \
    >> "$RUN_ROOT/rsync_from_base_station_${condition}.log" 2>&1 || true
}

run_sender() {
  local transport="$1"
  local failure_mode="$2"
  local condition="$3"
  local sender_csv="$RUN_ROOT/${condition}_sender.csv"
  local sender_err="$RUN_ROOT/${condition}_sender.err"
  local cmd
  if [[ "$transport" == "udp" ]]; then
    cmd="./cpp/build/example_private_5g_latency_udp_sender --host '$EDGE_HOST' --port '$UDP_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms '$TIMEOUT_MS' --udp-max-datagram-bytes 1400 --message service --payload-bytes '$PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label failure-fallback --request-id '$condition' --av-id '$SOURCE_ID' --obu-id '$SOURCE_ID' --rsu-id rsu-1 --network-load-level failure --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name fallback_mode --vehicle-outcome-value '$failure_mode' --vehicle-outcome-unit mode --csv"
  elif [[ "$transport" == "mqtt" ]]; then
    cmd="./cpp/build/example_private_5g_latency_sender --transport mqtt --host '$EDGE_HOST' --port '$MQTT_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms '$TIMEOUT_MS' --continue-on-failure --message service --payload-bytes '$PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label failure-fallback --request-id '$condition' --av-id '$SOURCE_ID' --obu-id '$SOURCE_ID' --rsu-id rsu-1 --network-load-level failure --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name fallback_mode --vehicle-outcome-value '$failure_mode' --vehicle-outcome-unit mode --csv"
  else
    cmd="./cpp/build/example_private_5g_latency_sender --transport tcp --host '$EDGE_HOST' --port '$TCP_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms '$TIMEOUT_MS' --continue-on-failure --message service --payload-bytes '$PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label failure-fallback --request-id '$condition' --av-id '$SOURCE_ID' --obu-id '$SOURCE_ID' --rsu-id rsu-1 --network-load-level failure --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name fallback_mode --vehicle-outcome-value '$failure_mode' --vehicle-outcome-unit mode --csv"
  fi
  record_command "local: $cmd > '$sender_csv' 2> '$sender_err'"
  (
    cd "$REPO_ROOT" &&
      eval "$cmd"
  ) > "$sender_csv" 2> "$sender_err" &
  SENDER_PID="$!"
}

restart_receiver_failure() {
  local transport="$1"
  local condition="$2"
  local failure_mode="receiver-restart"
  local condition_pattern="[${condition:0:1}]${condition:1}"
  record_event "$condition" "$transport" "$failure_mode" "failure_begin" "killing ${transport} receiver"
  ssh_edge_bash "pkill -f '$condition_pattern' || true"
  sleep "$OUTAGE_SEC"
  record_event "$condition" "$transport" "$failure_mode" "restart_begin" "starting ${transport} receiver"
  if [[ "$transport" == "tcp" ]]; then
    start_tcp_receiver "$condition"
  elif [[ "$transport" == "udp" ]]; then
    start_udp_receiver "$condition"
  else
    start_mqtt_receiver "$condition" "$failure_mode"
  fi
  record_event "$condition" "$transport" "$failure_mode" "restart_complete" "${transport} receiver restarted"
}

restart_broker_failure() {
  local condition="$1"
  local failure_mode="broker-restart"
  local condition_pattern="[${condition:0:1}]${condition:1}"
  record_event "$condition" "mqtt" "$failure_mode" "failure_begin" "killing mqtt broker and receiver"
  ssh_edge_bash "pkill -f '[t]hreaded_mqtt_broker.py' || true; pkill -f '$condition_pattern' || true"
  sleep "$OUTAGE_SEC"
  record_event "$condition" "mqtt" "$failure_mode" "restart_begin" "starting mqtt broker and receiver"
  start_mqtt_broker "$condition"
  start_mqtt_receiver "$condition" "$failure_mode"
  record_event "$condition" "mqtt" "$failure_mode" "restart_complete" "mqtt broker and receiver restarted"
}

run_condition() {
  local transport="$1"
  local failure_mode="$2"
  local condition="p5g-failure-${failure_mode}-${transport}-payload-${PAYLOAD_BYTES}"
  log "Running Experiment 12 condition ${condition}"
  record_event "$condition" "$transport" "$failure_mode" "condition_start" "starting condition"
  remote_cleanup
  sleep 2

  if [[ "$transport" == "tcp" ]]; then
    start_tcp_receiver "$condition"
  elif [[ "$transport" == "udp" ]]; then
    start_udp_receiver "$condition"
  else
    start_mqtt_broker "$condition"
    start_mqtt_receiver "$condition" "$failure_mode"
  fi
  record_event "$condition" "$transport" "$failure_mode" "service_ready" "receiver stack ready"

  run_sender "$transport" "$failure_mode" "$condition"
  local sender_pid="$SENDER_PID"
  record_event "$condition" "$transport" "$failure_mode" "sender_started" "pid=${sender_pid}"

  sleep "$TRIGGER_AFTER_SEC"
  if [[ "$failure_mode" == "broker-restart" ]]; then
    restart_broker_failure "$condition"
  else
    restart_receiver_failure "$transport" "$condition"
  fi

  local status=0
  if ! wait "$sender_pid"; then
    status=1
    echo "sender_exit_status=1" >> "$RUN_ROOT/${condition}.err"
  fi
  record_event "$condition" "$transport" "$failure_mode" "sender_complete" "status=${status}"

  sleep 5
  remote_cleanup
  sleep 2
  fetch_condition_artifacts "$condition"
}

main() {
  write_environment
  sync_and_build
  start_gnss

  for transport in $TRANSPORTS; do
    if [[ "$transport" == "mqtt" ]]; then
      run_condition mqtt receiver-restart
      run_condition mqtt broker-restart
    else
      run_condition "$transport" receiver-restart
    fi
  done

  log "Experiment 12 failure/fallback run complete"
}

main "$@"
