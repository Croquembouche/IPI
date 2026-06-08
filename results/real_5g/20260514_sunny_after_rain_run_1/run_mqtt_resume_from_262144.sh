#!/usr/bin/env bash
set -euo pipefail

RUN_ID="20260514_sunny_after_rain_run_1"
PAPER_RUN_ID="edge4av-real-20260514-sunny-after-rain-run-1"
REPO="/home/hydrau/Documents/Github/IPI"
RUN_DIR="$REPO/results/real_5g/$RUN_ID"
KEY="/tmp/ipi-ssh-$RUN_ID/key"
REMOTE="d1@10.100.100.6"
REMOTE_REPO="/home/d1/Documents/Github/IPI"
REMOTE_RUN="$REMOTE_REPO/results/real_5g/$RUN_ID"
MQTT_PORT=1883
COUNT=1000
INTERVAL_MS=200
PAYLOADS=(262144 524288 1048576 2097152)
COMMON_ARGS=(
  --run-id "$PAPER_RUN_ID"
  --condition-label private-5g-sunny-after-rain
  --av-id av-1
  --obu-id obu-1
  --rsu-id rsu-1
  --network-load-level idle
  --qos-profile default
  --mobility-state stationary
  --clock-sync-state unsynced
  --service-success true
)

mkdir -p "$RUN_DIR/commands" "$RUN_DIR/base_station"
: > "$RUN_DIR/commands/mqtt_resume_from_262144_commands.txt"

log_cmd() {
  printf '%s\n' "$*" >> "$RUN_DIR/commands/mqtt_resume_from_262144_commands.txt"
}

ssh_remote() {
  ssh -i "$KEY" -o BatchMode=yes "$REMOTE" "$@"
}

ssh_launch_detached() {
  local cmd="$1"
  timeout 5s ssh -i "$KEY" -o BatchMode=yes "$REMOTE" "$cmd" \
    >> "$RUN_DIR/mqtt_resume_remote_launch_detach.log" 2>&1 || true
}

cleanup_remote() {
  ssh_remote "pkill -f '[e]xample_private_5g_latency_receiver.*--transport mqtt' || true; pkill -f '[t]hreaded_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT' || true" || true
}

start_mqtt_broker() {
  scp -i "$KEY" -q "$RUN_DIR/tools/threaded_mqtt_broker.py" "$REMOTE:$REMOTE_RUN/threaded_mqtt_broker.py"
  cleanup_remote
  local cmd="cd '$REMOTE_RUN' && nohup python3 threaded_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT > threaded_mqtt_broker_resume.log 2> threaded_mqtt_broker_resume.err < /dev/null & echo \$! > threaded_mqtt_broker_resume.pid"
  log_cmd "remote: $cmd"
  ssh_launch_detached "$cmd"
  for _ in $(seq 1 20); do
    if ssh_remote "ss -ltn | grep -q ':$MQTT_PORT '"; then
      return 0
    fi
    sleep 1
  done
  ssh_remote "cat '$REMOTE_RUN/threaded_mqtt_broker_resume.err' 2>/dev/null || true"
  return 1
}

start_mqtt_receiver() {
  local condition="$1"
  local out="$REMOTE_RUN/${condition}_receiver.csv"
  local err="$REMOTE_RUN/${condition}_receiver.err"
  local pid="$REMOTE_RUN/${condition}_receiver.pid"
  ssh_remote "pkill -f '[e]xample_private_5g_latency_receiver.*--transport mqtt' || true" || true
  local cmd="cd '$REMOTE_REPO' && mkdir -p '$REMOTE_RUN' && nohup stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port $MQTT_PORT --intersection-id moved-location --source-id av-1 --run-id '$PAPER_RUN_ID' --condition-id '$condition' --condition-label private-5g-sunny-after-rain --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv > '$out' 2> '$err' < /dev/null & echo \$! > '$pid'"
  log_cmd "remote: $cmd"
  ssh_launch_detached "$cmd"
  sleep 2
  ssh_remote "kill -0 \$(cat '$pid')"
}

run_mqtt_payload() {
  local payload="$1"
  local condition="p5g-mqtt-service-payload-$payload"
  echo "$(date --iso-8601=seconds) starting fresh threaded MQTT broker for payload $payload"
  start_mqtt_broker
  start_mqtt_receiver "$condition"
  local cmd=(./cpp/build/example_private_5g_latency_sender
    --transport mqtt --host 10.100.100.6 --port "$MQTT_PORT"
    --count "$COUNT" --interval-ms "$INTERVAL_MS" --timeout-ms 60000
    --message service --payload-bytes "$payload"
    --intersection-id moved-location --source-id av-1
    --condition-id "$condition" --request-id "$condition"
    "${COMMON_ARGS[@]}"
    --csv)
  log_cmd "local: ${cmd[*]} > $RUN_DIR/${condition}_sender.csv 2> $RUN_DIR/${condition}_sender.err"
  "${cmd[@]}" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  cleanup_remote
}

{
  echo "# MQTT Resume From 262144 Metadata"
  echo
  echo "- Start time: $(date --iso-8601=seconds)"
  echo "- Payload arguments: ${PAYLOADS[*]}"
  echo "- Count per condition: $COUNT"
  echo "- Interval ms: $INTERVAL_MS"
  echo "- MQTT port: $MQTT_PORT"
  echo "- Broker: fresh threaded Python MQTT 3.1.1 QoS0 broker for each resumed payload."
  echo "- Timestamp note: sender send time is patched after request encoding and immediately before MQTT publish, so sender-side packaging time is excluded from RTT."
} > "$RUN_DIR/mqtt_resume_from_262144_metadata.md"

for payload in "${PAYLOADS[@]}"; do
  echo "$(date --iso-8601=seconds) starting MQTT service payload $payload"
  run_mqtt_payload "$payload"
done

cleanup_remote
{
  echo
  echo "- End time: $(date --iso-8601=seconds)"
} >> "$RUN_DIR/mqtt_resume_from_262144_metadata.md"
