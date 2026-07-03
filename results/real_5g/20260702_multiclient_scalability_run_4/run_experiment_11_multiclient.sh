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

RUN_ID="${RUN_ID:-edge4av-real-20260702-multiclient-scalability-run-4}"
REMOTE_RUN_DIR="${EDGE_REPO}/results/real_5g/${RUN_NAME}"
INTERSECTION_ID="${INTERSECTION_ID:-multiclient-scalability-run-4}"
TCP_PORT="${TCP_PORT:-36666}"
UDP_PORT="${UDP_PORT:-36667}"
MQTT_PORT="${MQTT_PORT:-1883}"
PAYLOAD_BYTES="${PAYLOAD_BYTES:-1024}"
COUNT="${COUNT:-1000}"
INTERVAL_MS="${INTERVAL_MS:-200}"
UDP_TIMEOUT_MS="${UDP_TIMEOUT_MS:-5000}"
UDP_MAX_DATAGRAM_BYTES="${UDP_MAX_DATAGRAM_BYTES:-1400}"
MQTT_TIMEOUT_MS="${MQTT_TIMEOUT_MS:-30000}"
CLIENT_LEVELS="${CLIENT_LEVELS:-1 2 5 10 20 50 100}"
TRANSPORTS="${TRANSPORTS:-tcp udp mqtt}"
ulimit -n 65535 >/dev/null 2>&1 || true

SSH_OPTS=(-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null)

mkdir -p "$RUN_ROOT/base_station" "$RUN_ROOT/commands"

log() {
  printf '[%s] %s\n' "$(date -Iseconds)" "$*" | tee -a "$RUN_ROOT/run.log"
}

record_command() {
  printf '%s\n' "$*" >> "$RUN_ROOT/commands/experiment_11_multiclient_commands.txt"
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
    echo "# Experiment 11 Multiclient Scalability"
    echo
    echo "- run_id: ${RUN_ID}"
    echo "- run_name: ${RUN_NAME}"
    echo "- edge_host: ${EDGE_HOST}"
    echo "- edge_repo: ${EDGE_REPO}"
    echo "- payload_bytes: ${PAYLOAD_BYTES}"
    echo "- count_per_client: ${COUNT}"
    echo "- interval_ms: ${INTERVAL_MS}"
    echo "- client_levels: ${CLIENT_LEVELS}"
    echo "- transports: ${TRANSPORTS}"
    echo "- vehicle_state: stationary"
    echo "- gnss: NovAtel ROS 2 recorder"
    echo "- mqtt_mode: one MQTT receiver process per source ID behind one broker"
    echo "- udp_mode: application-level fragmentation enabled with max datagram ${UDP_MAX_DATAGRAM_BYTES} bytes"
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

source_id() {
  printf 'veh-%02d' "$1"
}

start_tcp_receiver() {
  local condition="$1"
  remote_cleanup
  local cmd
  cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport tcp --port '$TCP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label scale --rsu-id rsu-1 --network-load-level '${CLIENTS}-clients' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name clients --vehicle-outcome-value '$CLIENTS' --vehicle-outcome-unit count --csv > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $cmd"
  ssh_edge_bash "$cmd"
  wait_tcp_port "$EDGE_HOST" "$TCP_PORT" "TCP receiver"
}

start_udp_receiver() {
  local condition="$1"
  remote_cleanup
  local cmd
  cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_udp_receiver --port '$UDP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label scale --rsu-id rsu-1 --network-load-level '${CLIENTS}-clients' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name clients --vehicle-outcome-value '$CLIENTS' --vehicle-outcome-unit count --csv > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $cmd"
  ssh_edge_bash "$cmd"
  sleep 2
}

start_mqtt_stack() {
  local condition="$1"
  remote_cleanup
  local broker_cmd
  broker_cmd="cd '$REMOTE_RUN_DIR/tools' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.pid'; exec python3 threaded_mqtt_broker.py --host 0.0.0.0 --port '$MQTT_PORT' > '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.log' 2> '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.err' < /dev/null\""
  record_command "remote: $broker_cmd"
  ssh_edge_bash "$broker_cmd"
  wait_tcp_port "$EDGE_HOST" "$MQTT_PORT" "MQTT broker"

  for i in $(seq 1 "$CLIENTS"); do
    local src
    src="$(source_id "$i")"
    local receiver_cmd
    receiver_cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_${src}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port '$MQTT_PORT' --intersection-id '$INTERSECTION_ID' --source-id '$src' --run-id '$RUN_ID' --condition-id '$condition' --condition-label scale --rsu-id rsu-1 --network-load-level '${CLIENTS}-clients' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name clients --vehicle-outcome-value '$CLIENTS' --vehicle-outcome-unit count --csv > '$REMOTE_RUN_DIR/base_station/${condition}_${src}_receiver.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_${src}_receiver.err' < /dev/null\""
    record_command "remote: $receiver_cmd"
    ssh_edge_bash "$receiver_cmd"
  done
  sleep 3
}

fetch_condition_artifacts() {
  local condition="$1"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    "$EDGE_TARGET:$REMOTE_RUN_DIR/base_station/${condition}"'*' \
    "$RUN_ROOT/base_station/" \
    > "$RUN_ROOT/rsync_from_base_station_${condition}.log" 2>&1 || true
}

run_sender_group() {
  local transport="$1"
  local condition="$2"
  local pids=()
  local status=0

  for i in $(seq 1 "$CLIENTS"); do
    local src
    src="$(source_id "$i")"
    local sender_csv="$RUN_ROOT/${condition}_${src}_sender.csv"
    local sender_err="$RUN_ROOT/${condition}_${src}_sender.err"
    local cmd
    if [[ "$transport" == "udp" ]]; then
      cmd="./cpp/build/example_private_5g_latency_udp_sender --host '$EDGE_HOST' --port '$UDP_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms '$UDP_TIMEOUT_MS' --udp-max-datagram-bytes '$UDP_MAX_DATAGRAM_BYTES' --message service --payload-bytes '$PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$src' --run-id '$RUN_ID' --condition-id '$condition' --condition-label scale --request-id '${condition}-${src}' --av-id '$src' --obu-id '$src' --rsu-id rsu-1 --network-load-level '${CLIENTS}-clients' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name clients --vehicle-outcome-value '$CLIENTS' --vehicle-outcome-unit count --csv"
    elif [[ "$transport" == "mqtt" ]]; then
      cmd="./cpp/build/example_private_5g_latency_sender --transport mqtt --host '$EDGE_HOST' --port '$MQTT_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms '$MQTT_TIMEOUT_MS' --message service --payload-bytes '$PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$src' --run-id '$RUN_ID' --condition-id '$condition' --condition-label scale --request-id '${condition}-${src}' --av-id '$src' --obu-id '$src' --rsu-id rsu-1 --network-load-level '${CLIENTS}-clients' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name clients --vehicle-outcome-value '$CLIENTS' --vehicle-outcome-unit count --csv"
    else
      cmd="./cpp/build/example_private_5g_latency_sender --transport tcp --host '$EDGE_HOST' --port '$TCP_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --message service --payload-bytes '$PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$src' --run-id '$RUN_ID' --condition-id '$condition' --condition-label scale --request-id '${condition}-${src}' --av-id '$src' --obu-id '$src' --rsu-id rsu-1 --network-load-level '${CLIENTS}-clients' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name clients --vehicle-outcome-value '$CLIENTS' --vehicle-outcome-unit count --csv"
    fi
    record_command "local: $cmd > '$sender_csv' 2> '$sender_err'"
    (
      cd "$REPO_ROOT" &&
        eval "$cmd"
    ) > "$sender_csv" 2> "$sender_err" &
    pids+=("$!")
  done

  for pid in "${pids[@]}"; do
    if ! wait "$pid"; then
      status=1
    fi
  done
  return "$status"
}

run_condition() {
  local transport="$1"
  CLIENTS="$2"
  local condition="p5g-scale-${transport}-payload-${PAYLOAD_BYTES}-clients-${CLIENTS}"
  export CLIENTS

  log "Running Experiment 11 condition ${condition}"
  if [[ "$transport" == "tcp" ]]; then
    start_tcp_receiver "$condition"
  elif [[ "$transport" == "udp" ]]; then
    start_udp_receiver "$condition"
  else
    start_mqtt_stack "$condition"
  fi

  if ! run_sender_group "$transport" "$condition"; then
    echo "sender_group_exit_status=1" >> "$RUN_ROOT/${condition}.err"
  fi

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
    for clients in $CLIENT_LEVELS; do
      run_condition "$transport" "$clients"
    done
  done

  log "Experiment 11 multiclient scalability run complete"
}

main "$@"
