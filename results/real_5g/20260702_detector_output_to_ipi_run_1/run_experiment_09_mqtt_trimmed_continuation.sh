#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
RUN_DIR="$REPO_ROOT/results/real_5g/20260702_detector_output_to_ipi_run_1"
GPS_RUN_DIR="$RUN_DIR/gps_mqtt_continuation"
RUN_ID="edge4av-real-20260702-detector-output-to-ipi-run-1"
EDGE_HOST="${EDGE_HOST:-10.100.100.6}"
EDGE_USER="${EDGE_USER:-d1}"
EDGE_REPO="${EDGE_REPO:-/home/d1/Documents/Github/IPI}"
REMOTE_RUN_DIR="$EDGE_REPO/results/real_5g/20260702_detector_output_to_ipi_run_1"

COUNT="${COUNT:-1000}"
INTERVAL_MS="${INTERVAL_MS:-200}"
MQTT_PORT="${MQTT_PORT:-1883}"
INTERSECTION_ID="${INTERSECTION_ID:-detector-output-to-ipi-run-1}"
SOURCE_ID="${SOURCE_ID:-av-1}"
PAYLOADS="${PAYLOADS:-4096 19648 22816 23968 60000}"

if [ -z "${SSHPASS:-}" ]; then
  echo "SSHPASS must be set for password-based edge SSH." >&2
  exit 1
fi

mkdir -p "$RUN_DIR"/{base_station,commands,tools} "$GPS_RUN_DIR"

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

ssh_edge_bash() {
  printf 'set -euo pipefail\n%s\n' "$1" | "${SSH_BASE[@]}" "bash -s"
}

record_command() {
  printf '%s\n' "$*" >> "$RUN_DIR/commands/experiment_09_mqtt_trimmed_continuation_commands.txt"
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
  ssh_edge_bash "pkill -f '[e]xample_private_5g_latency_receiver' || true; pkill -f '[t]hreaded_mqtt_broker.py' || true" >/dev/null 2>&1 || true
}

GPS_PID=""
stop_gps() {
  if [ -z "$GPS_PID" ]; then
    return
  fi
  for pid_file in \
    "$GPS_RUN_DIR/gps/gps_topic_recorder.pid" \
    "$GPS_RUN_DIR/gps/rosbag_record.pid" \
    "$GPS_RUN_DIR/gps/novatel_driver.pid"; do
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
  stop_gps
  remote_cleanup
  rsync -az -e "$RSYNC_RSH" "$EDGE_USER@$EDGE_HOST:$REMOTE_RUN_DIR/base_station/" "$RUN_DIR/base_station/" >> "$RUN_DIR/rsync_from_base_station.log" 2>&1 || true
}
trap cleanup EXIT INT TERM

start_gps() {
  log "Starting GNSS recorder for trimmed MQTT continuation"
  bash "$REPO_ROOT/scripts/record_gps_for_experiment.sh" "$GPS_RUN_DIR" \
    > "$GPS_RUN_DIR/gps_capture_stdout.log" \
    2> "$GPS_RUN_DIR/gps_capture_stderr.log" &
  GPS_PID=$!
  echo "$GPS_PID" > "$GPS_RUN_DIR/gps_capture.pid"
  sleep 5
}

start_mqtt_stack() {
  local condition="$1"
  local payload="$2"
  local broker_cmd
  local receiver_cmd
  broker_cmd="cd '$REMOTE_RUN_DIR/tools' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.pid'; exec python3 threaded_mqtt_broker.py --host 0.0.0.0 --port '$MQTT_PORT' > '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.log' 2> '$REMOTE_RUN_DIR/base_station/threaded_mqtt_broker_${condition}.err' < /dev/null\""
  record_command "remote: $broker_cmd"
  ssh_edge_bash "$broker_cmd"
  wait_tcp_port "$EDGE_HOST" "$MQTT_PORT" "MQTT broker"
  receiver_cmd="cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port '$MQTT_PORT' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label detector-output-to-ipi --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name detector_payload_bytes --vehicle-outcome-value '$payload' --vehicle-outcome-unit bytes --csv > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "remote: $receiver_cmd"
  ssh_edge_bash "$receiver_cmd"
  sleep 2
}

stop_condition_processes() {
  remote_cleanup
  sleep 2
  rsync -az -e "$RSYNC_RSH" "$EDGE_USER@$EDGE_HOST:$REMOTE_RUN_DIR/base_station/" "$RUN_DIR/base_station/" >> "$RUN_DIR/rsync_from_base_station.log" 2>&1 || true
}

run_mqtt_condition() {
  local payload="$1"
  local condition="p5g-mqtt-detector-output-payload-${payload}"
  log "Running trimmed MQTT detector-output condition $condition"
  start_mqtt_stack "$condition" "$payload"
  local cmd
  cmd="./cpp/build/example_private_5g_latency_sender --transport mqtt --host '$EDGE_HOST' --port '$MQTT_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms 60000 --message service --payload-bytes '$payload' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --condition-id '$condition' --request-id '$condition' --run-id '$RUN_ID' --condition-label detector-output-to-ipi --av-id av-1 --obu-id obu-1 --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name detector_payload_bytes --vehicle-outcome-value '$payload' --vehicle-outcome-unit bytes --csv"
  record_command "local: $cmd > '$RUN_DIR/${condition}_sender.csv' 2> '$RUN_DIR/${condition}_sender.err'"
  bash -lc "cd '$REPO_ROOT' && $cmd" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  stop_condition_processes
}

remote_cleanup
start_gps

for payload in $PAYLOADS; do
  run_mqtt_condition "$payload"
done

log "Experiment 09 trimmed MQTT continuation complete"
