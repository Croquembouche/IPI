#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
RUN_DIR="$REPO_ROOT/results/real_5g/20260701_load_qos_run_1"
RUN_ID="edge4av-real-20260701-load-qos-run-1"
EDGE_HOST="${EDGE_HOST:-10.100.100.6}"
EDGE_USER="${EDGE_USER:-d1}"
EDGE_REPO="${EDGE_REPO:-/home/d1/Documents/Github/IPI}"
REMOTE_RUN_DIR="$EDGE_REPO/results/real_5g/20260701_load_qos_run_1"

COUNT="${COUNT:-1000}"
INTERVAL_MS="${INTERVAL_MS:-200}"
PAYLOAD_BYTES="${PAYLOAD_BYTES:-1024}"
LOAD_MBPS="${LOAD_MBPS:-25}"
LOAD_PORT="${LOAD_PORT:-39000}"
TCP_PORT="${TCP_PORT:-36666}"
MQTT_PORT="${MQTT_PORT:-1883}"
INTERSECTION_ID="${INTERSECTION_ID:-load-qos-run-1}"
SOURCE_ID="${SOURCE_ID:-av-1}"

if [ -z "${SSHPASS:-}" ]; then
  echo "SSHPASS must be set for password-based edge SSH." >&2
  exit 1
fi

mkdir -p "$RUN_DIR"/{base_station,commands,gps,tools}

SSH_BASE=(
  sshpass -e ssh
  -o StrictHostKeyChecking=no
  -o UserKnownHostsFile=/dev/null
  "$EDGE_USER@$EDGE_HOST"
)
RSYNC_RSH="sshpass -e ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null"

log() {
  printf '[%s] %s\n' "$(date --iso-8601=seconds)" "$*" | tee -a "$RUN_DIR/run.log"
}

ssh_edge() {
  "${SSH_BASE[@]}" "$@"
}

ssh_edge_bash() {
  printf 'set -euo pipefail\n%s\n' "$1" | "${SSH_BASE[@]}" "bash -s"
}

wait_tcp_port() {
  local host="$1"
  local port="$2"
  local label="$3"
  for _ in $(seq 1 60); do
    if timeout 1 bash -c ":</dev/tcp/$host/$port" >/dev/null 2>&1; then
      return 0
    fi
    sleep 1
  done
  echo "Timed out waiting for $label on $host:$port" >&2
  return 1
}

remote_cleanup() {
  ssh_edge_bash "pkill -f '[e]xample_private_5g_latency_receiver' || true; pkill -f '[e]dge4av_loadgen.py --role server' || true; pkill -f '[t]hreaded_mqtt_broker.py' || true" >/dev/null 2>&1 || true
}

GPS_PID=""
LOCAL_LOAD_PID=""
stop_gps() {
  if [ -z "$GPS_PID" ]; then
    return
  fi

  for pid_file in \
    "$RUN_DIR/gps/gps_topic_recorder.pid" \
    "$RUN_DIR/gps/rosbag_record.pid" \
    "$RUN_DIR/gps/novatel_driver.pid"; do
    if [ -s "$pid_file" ]; then
      kill -INT "$(cat "$pid_file")" 2>/dev/null || true
    fi
  done

  kill -INT "$GPS_PID" 2>/dev/null || true
  for _ in $(seq 1 20); do
    if ! kill -0 "$GPS_PID" 2>/dev/null; then
      wait "$GPS_PID" 2>/dev/null || true
      GPS_PID=""
      return
    fi
    sleep 1
  done

  kill -TERM "$GPS_PID" 2>/dev/null || true
  sleep 2
  kill -KILL "$GPS_PID" 2>/dev/null || true
  wait "$GPS_PID" 2>/dev/null || true
  GPS_PID=""
}

cleanup() {
  set +e
  if [ -n "$LOCAL_LOAD_PID" ]; then
    kill "$LOCAL_LOAD_PID" 2>/dev/null
    wait "$LOCAL_LOAD_PID" 2>/dev/null
  fi
  stop_gps
  remote_cleanup
  rsync -az -e "$RSYNC_RSH" "$EDGE_USER@$EDGE_HOST:$REMOTE_RUN_DIR/base_station/" "$RUN_DIR/base_station/" >> "$RUN_DIR/rsync_from_base_station.log" 2>&1 || true
}
trap cleanup EXIT INT TERM

write_environment() {
  {
    printf '# Experiment 06 Load/QoS Private-5G Run\n\n'
    printf -- '- Run id: `%s`\n' "$RUN_ID"
    printf -- '- Local start time: `%s`\n' "$(date --iso-8601=seconds)"
    printf -- '- Local host: `%s`\n' "$(hostname)"
    printf -- '- Edge host: `%s@%s`\n' "$EDGE_USER" "$EDGE_HOST"
    printf -- '- Run directory: `%s`\n' "$RUN_DIR"
    printf -- '- Edge run directory: `%s`\n' "$REMOTE_RUN_DIR"
    printf -- '- Vehicle state: `stationary`\n'
    printf -- '- Payload bytes: `%s`\n' "$PAYLOAD_BYTES"
    printf -- '- Probe count: `%s`\n' "$COUNT"
    printf -- '- Probe interval ms: `%s`\n' "$INTERVAL_MS"
    printf -- '- Background load: `uplink %s Mbps`, generated locally toward edge\n' "$LOAD_MBPS"
    printf -- '- QoS label: `default`; no special 5QI profile is asserted by this script\n'
    printf -- '- GNSS: `scripts/record_gps_for_experiment.sh`\n'
  } > "$RUN_DIR/environment.md"
}

record_command() {
  printf '%s\n' "$*" >> "$RUN_DIR/commands/experiment_06_commands.txt"
}

start_gps() {
  log "Starting GNSS recorder"
  bash "$REPO_ROOT/scripts/record_gps_for_experiment.sh" "$RUN_DIR" \
    > "$RUN_DIR/gps_capture_stdout.log" \
    2> "$RUN_DIR/gps_capture_stderr.log" &
  GPS_PID=$!
  echo "$GPS_PID" > "$RUN_DIR/gps_capture.pid"
  sleep 5
  if ! kill -0 "$GPS_PID" 2>/dev/null; then
    log "GNSS recorder exited early; continuing and preserving GPS logs"
  fi
}

stage_edge() {
  log "Syncing code and tools to edge"
  rsync -az --delete --exclude build -e "$RSYNC_RSH" "$REPO_ROOT/cpp/" "$EDGE_USER@$EDGE_HOST:$EDGE_REPO/cpp/" > "$RUN_DIR/rsync_cpp_to_base_station.log" 2>&1
  ssh_edge_bash "cd '$EDGE_REPO' && rm -rf cpp/build && cmake -S cpp -B cpp/build && cmake --build cpp/build -j\$(nproc)" > "$RUN_DIR/base_build.log" 2>&1
  ssh_edge_bash "mkdir -p '$REMOTE_RUN_DIR/base_station' '$REMOTE_RUN_DIR/tools' '$REMOTE_RUN_DIR/commands'"
  rsync -az -e "$RSYNC_RSH" "$RUN_DIR/tools/" "$EDGE_USER@$EDGE_HOST:$REMOTE_RUN_DIR/tools/" > "$RUN_DIR/rsync_tools_to_base_station.log" 2>&1
}

start_tcp_receiver() {
  local condition="$1"
  local load_level="$2"
  local remote_cmd
  remote_cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport tcp --port '$TCP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label private-5g-load-qos --rsu-id rsu-1 --network-load-level '$load_level' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $remote_cmd"
  ssh_edge_bash "$remote_cmd"
  wait_tcp_port "$EDGE_HOST" "$TCP_PORT" "TCP receiver"
}

start_mqtt_stack() {
  local condition="$1"
  local load_level="$2"
  local broker_cmd
  local receiver_cmd
  broker_cmd="cd '$REMOTE_RUN_DIR/tools' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.pid'; exec python3 threaded_mqtt_broker.py --host 0.0.0.0 --port '$MQTT_PORT' > '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.log' 2> '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.err' < /dev/null\""
  record_command "remote: $broker_cmd"
  ssh_edge_bash "$broker_cmd"
  wait_tcp_port "$EDGE_HOST" "$MQTT_PORT" "MQTT broker"
  receiver_cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port '$MQTT_PORT' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label private-5g-load-qos --rsu-id rsu-1 --network-load-level '$load_level' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $receiver_cmd"
  ssh_edge_bash "$receiver_cmd"
  sleep 2
}

start_load_server() {
  local condition="$1"
  local duration_s="$2"
  local server_cmd
  server_cmd="cd '$REMOTE_RUN_DIR/tools' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_load_server.pid'; exec python3 edge4av_loadgen.py --role server --protocol tcp --bind 0.0.0.0 --port '$LOAD_PORT' --duration-s '$duration_s' > '$REMOTE_RUN_DIR/base_station/${condition}_load_server.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_load_server.err' < /dev/null\""
  record_command "remote: $server_cmd"
  ssh_edge_bash "$server_cmd"
  wait_tcp_port "$EDGE_HOST" "$LOAD_PORT" "load server"
}

start_local_load_client() {
  local condition="$1"
  local duration_s="$2"
  local cmd
  cmd="python3 '$RUN_DIR/tools/edge4av_loadgen.py' --role client --protocol tcp --host '$EDGE_HOST' --port '$LOAD_PORT' --duration-s '$duration_s' --target-mbps '$LOAD_MBPS'"
  record_command "local: $cmd > '$RUN_DIR/${condition}_load_client.csv' 2> '$RUN_DIR/${condition}_load_client.err'"
  bash -lc "$cmd" > "$RUN_DIR/${condition}_load_client.csv" 2> "$RUN_DIR/${condition}_load_client.err" &
  LOCAL_LOAD_PID=$!
  sleep 2
}

stop_condition_processes() {
  if [ -n "$LOCAL_LOAD_PID" ]; then
    wait "$LOCAL_LOAD_PID" 2>/dev/null || true
    LOCAL_LOAD_PID=""
  fi
  remote_cleanup
  sleep 2
  rsync -az -e "$RSYNC_RSH" "$EDGE_USER@$EDGE_HOST:$REMOTE_RUN_DIR/base_station/" "$RUN_DIR/base_station/" >> "$RUN_DIR/rsync_from_base_station.log" 2>&1 || true
}

run_tcp_condition() {
  local condition="$1"
  local load_level="$2"
  local load_mbps="$3"
  log "Running TCP condition $condition"
  start_tcp_receiver "$condition" "$load_level"
  if [ "$load_mbps" != "0" ]; then
    start_load_server "$condition" 260
    start_local_load_client "$condition" 230
  fi
  local cmd
  cmd="./cpp/build/example_private_5g_latency_sender --transport tcp --host '$EDGE_HOST' --port '$TCP_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --message service --payload-bytes '$PAYLOAD_BYTES' --condition-id '$condition' --request-id '$condition' --run-id '$RUN_ID' --condition-label private-5g-load-qos --av-id av-1 --obu-id obu-1 --rsu-id rsu-1 --network-load-level '$load_level' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv"
  record_command "local: $cmd > '$RUN_DIR/${condition}_sender.csv' 2> '$RUN_DIR/${condition}_sender.err'"
  bash -lc "cd '$REPO_ROOT' && $cmd" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  stop_condition_processes
}

run_mqtt_condition() {
  local condition="$1"
  local load_level="$2"
  local load_mbps="$3"
  log "Running MQTT condition $condition"
  start_mqtt_stack "$condition" "$load_level"
  if [ "$load_mbps" != "0" ]; then
    start_load_server "$condition" 260
    start_local_load_client "$condition" 230
  fi
  local cmd
  cmd="./cpp/build/example_private_5g_latency_sender --transport mqtt --host '$EDGE_HOST' --port '$MQTT_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms 60000 --message service --payload-bytes '$PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --condition-id '$condition' --request-id '$condition' --run-id '$RUN_ID' --condition-label private-5g-load-qos --av-id av-1 --obu-id obu-1 --rsu-id rsu-1 --network-load-level '$load_level' --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv"
  record_command "local: $cmd > '$RUN_DIR/${condition}_sender.csv' 2> '$RUN_DIR/${condition}_sender.err'"
  bash -lc "cd '$REPO_ROOT' && $cmd" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  stop_condition_processes
}

write_environment
remote_cleanup
stage_edge
start_gps

run_tcp_condition "p5g-tcp-loadqos-payload-${PAYLOAD_BYTES}-idle" "idle" "0"
run_tcp_condition "p5g-tcp-loadqos-payload-${PAYLOAD_BYTES}-uplink-${LOAD_MBPS}mbps" "heavy" "$LOAD_MBPS"
run_mqtt_condition "p5g-mqtt-loadqos-payload-${PAYLOAD_BYTES}-idle" "idle" "0"
run_mqtt_condition "p5g-mqtt-loadqos-payload-${PAYLOAD_BYTES}-uplink-${LOAD_MBPS}mbps" "heavy" "$LOAD_MBPS"

log "Experiment 06 run complete"
