#!/usr/bin/env bash
set -euo pipefail

RUN_ID="20260515_sunny_run_1"
PAPER_RUN_ID="edge4av-real-20260515-sunny-run-1"
REPO="/home/hydrau/Documents/Github/IPI"
RUN_DIR="$REPO/results/real_5g/$RUN_ID"
KEY="/tmp/ipi-ssh-$RUN_ID/key"
REMOTE="d1@10.100.100.6"
REMOTE_REPO="/home/d1/Documents/Github/IPI"
REMOTE_RUN="$REMOTE_REPO/results/real_5g/$RUN_ID"
MQTT_PORT=1883
INTERVAL_MS=200
COMMON_ARGS=(
  --run-id "$PAPER_RUN_ID"
  --condition-label private-5g-sunny
  --av-id av-1
  --obu-id obu-1
  --rsu-id rsu-1
  --network-load-level idle
  --qos-profile default
  --mobility-state stationary
  --clock-sync-state unsynced
  --service-success true
)

mkdir -p "$RUN_DIR/commands"
COMMAND_LOG="$RUN_DIR/commands/mqtt_resume_20260518_commands.txt"
: > "$COMMAND_LOG"

log_cmd() {
  printf '%s\n' "$*" >> "$COMMAND_LOG"
}

ssh_remote() {
  ssh -i "$KEY" -o BatchMode=yes "$REMOTE" "$@"
}

ssh_launch_detached() {
  local cmd="$1"
  timeout 5s ssh -i "$KEY" -o BatchMode=yes "$REMOTE" "$cmd" \
    >> "$RUN_DIR/remote_mqtt_resume_20260518_launch_detach.log" 2>&1 || true
}

cleanup_remote() {
  ssh_remote "pkill -f '[e]xample_private_5g_latency_receiver.*--transport mqtt' || true; pkill -f '[t]hreaded_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT' || true; pkill -f '[m]inimal_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT' || true" || true
}

start_mqtt_broker() {
  local label="$1"
  scp -i "$KEY" -q "$RUN_DIR/tools/threaded_mqtt_broker.py" "$REMOTE:$REMOTE_RUN/threaded_mqtt_broker.py"
  ssh_remote "mkdir -p '$REMOTE_RUN'; pkill -f '[t]hreaded_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT' || true; pkill -f '[m]inimal_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT' || true"
  local cmd="cd '$REMOTE_RUN' && nohup python3 threaded_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT > threaded_mqtt_broker_${label}.log 2> threaded_mqtt_broker_${label}.err < /dev/null & echo \$! > threaded_mqtt_broker_${label}.pid"
  log_cmd "remote: $cmd"
  ssh_launch_detached "$cmd"
  for _ in $(seq 1 20); do
    if ssh_remote "ss -ltn | grep -q ':$MQTT_PORT '"; then
      return 0
    fi
    sleep 1
  done
  ssh_remote "cat '$REMOTE_RUN/threaded_mqtt_broker_${label}.err' 2>/dev/null || true"
  return 1
}

start_mqtt_receiver() {
  local condition="$1"
  local label="$2"
  local out="$REMOTE_RUN/${label}_receiver.csv"
  local err="$REMOTE_RUN/${label}_receiver.err"
  local pid="$REMOTE_RUN/${label}_receiver.pid"
  ssh_remote "pkill -f '[e]xample_private_5g_latency_receiver.*--transport mqtt' || true" || true
  local cmd="cd '$REMOTE_REPO' && mkdir -p '$REMOTE_RUN' && nohup stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port $MQTT_PORT --intersection-id sunny-run-1 --source-id av-1 --run-id '$PAPER_RUN_ID' --condition-id '$condition' --condition-label private-5g-sunny --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv > '$out' 2> '$err' < /dev/null & echo \$! > '$pid'"
  log_cmd "remote: $cmd"
  ssh_launch_detached "$cmd"
  sleep 2
  ssh_remote "kill -0 \$(cat '$pid')"
}

run_mqtt_payload() {
  local payload="$1"
  local count="$2"
  local label="$3"
  local condition="p5g-mqtt-service-payload-$payload"
  echo "$(date --iso-8601=seconds) starting MQTT $label payload $payload count $count"
  start_mqtt_broker "$label"
  start_mqtt_receiver "$condition" "$label"
  local cmd=(./cpp/build/example_private_5g_latency_sender
    --transport mqtt --host 10.100.100.6 --port "$MQTT_PORT"
    --count "$count" --interval-ms "$INTERVAL_MS" --timeout-ms 60000
    --message service --payload-bytes "$payload"
    --intersection-id sunny-run-1 --source-id av-1
    --condition-id "$condition" --request-id "$label"
    "${COMMON_ARGS[@]}"
    --csv)
  log_cmd "local: ${cmd[*]} > $RUN_DIR/${label}_sender.csv 2> $RUN_DIR/${label}_sender.err"
  "${cmd[@]}" > "$RUN_DIR/${label}_sender.csv" 2> "$RUN_DIR/${label}_sender.err"
  cleanup_remote
}

{
  echo "# MQTT Resume Metadata"
  echo
  echo "- Start time: $(date --iso-8601=seconds)"
  echo "- Resume date: 2026-05-18"
  echo "- Run folder retained from original sunny stationary run: $RUN_ID"
  echo "- Weather/location note: operator reported same location and sunny weather."
  echo "- 131072-byte resume count: 416, completing the 584 valid rows from the interrupted 2026-05-15 file."
  echo "- 262144-byte count: 1000"
  echo "- Interval ms: $INTERVAL_MS"
  echo "- Clock sync state: unsynced"
  echo "- Timestamp note: sender send time is set after request encoding and immediately before MQTT publish, so sender-side packaging time is excluded from RTT."
} > "$RUN_DIR/mqtt_resume_20260518_metadata.md"

cleanup_remote
run_mqtt_payload 131072 416 p5g-mqtt-service-payload-131072_resume_20260518
run_mqtt_payload 262144 1000 p5g-mqtt-service-payload-262144
cleanup_remote

{
  echo
  echo "- End time: $(date --iso-8601=seconds)"
} >> "$RUN_DIR/mqtt_resume_20260518_metadata.md"
