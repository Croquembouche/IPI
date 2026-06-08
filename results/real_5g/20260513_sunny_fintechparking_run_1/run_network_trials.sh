#!/usr/bin/env bash
set -euo pipefail

RUN_ID="20260513_sunny_fintechparking_run_1"
PAPER_RUN_ID="edge4av-real-20260513-sunny-fintechparking-run-1"
REPO="/home/hydrau/Documents/Github/IPI"
RUN_DIR="$REPO/results/real_5g/$RUN_ID"
KEY="/tmp/ipi-ssh-$RUN_ID/key"
REMOTE="d1@10.100.100.6"
REMOTE_REPO="/home/d1/Documents/Github/IPI"
REMOTE_RUN="$REMOTE_REPO/results/real_5g/$RUN_ID"
COUNT=1000
INTERVAL_MS=200
TCP_PORT=36666
MQTT_PORT=1883
COMMON_ARGS=(
  --run-id "$PAPER_RUN_ID"
  --condition-label private-5g-sunny-fintechparking
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
: > "$RUN_DIR/commands/network_trial_commands.txt"

log_cmd() {
  printf '%s\n' "$*" >> "$RUN_DIR/commands/network_trial_commands.txt"
}

ssh_remote() {
  ssh -i "$KEY" -o BatchMode=yes "$REMOTE" "$@"
}

ssh_launch_detached() {
  local cmd="$1"
  timeout 5s ssh -i "$KEY" -o BatchMode=yes "$REMOTE" "$cmd" \
    >> "$RUN_DIR/remote_launch_detach.log" 2>&1 || true
}

cleanup_remote_receivers() {
  ssh_remote "pkill -f '[e]xample_private_5g_latency_receiver.*($TCP_PORT|$MQTT_PORT)' || true" || true
}

stop_remote_pid() {
  local pid_file="$1"
  ssh_remote "if test -f '$pid_file'; then kill \$(cat '$pid_file') 2>/dev/null || true; fi" || true
}

start_tcp_receiver() {
  local condition="$1"
  local out="$REMOTE_RUN/${condition}_receiver.csv"
  local err="$REMOTE_RUN/${condition}_receiver.err"
  local pid="$REMOTE_RUN/${condition}_receiver.pid"
  cleanup_remote_receivers
  local cmd="cd '$REMOTE_REPO' && mkdir -p '$REMOTE_RUN' && nohup stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport tcp --port $TCP_PORT --run-id '$PAPER_RUN_ID' --condition-id '$condition' --condition-label private-5g-sunny-fintechparking --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv > '$out' 2> '$err' < /dev/null & echo \$! > '$pid'"
  log_cmd "remote: $cmd"
  ssh_launch_detached "$cmd"
  for _ in $(seq 1 30); do
    if ssh_remote "ss -ltn | grep -q ':$TCP_PORT '"; then
      return 0
    fi
    sleep 1
  done
  ssh_remote "cat '$err' 2>/dev/null || true"
  return 1
}

run_tcp_service_payload() {
  local payload="$1"
  local condition="p5g-tcp-service-payload-$payload"
  start_tcp_receiver "$condition"
  local cmd=(./cpp/build/example_private_5g_latency_sender
    --transport tcp --host 10.100.100.6 --port "$TCP_PORT"
    --count "$COUNT" --interval-ms "$INTERVAL_MS"
    --message service --payload-bytes "$payload"
    --condition-id "$condition" --request-id "$condition"
    "${COMMON_ARGS[@]}"
    --csv)
  log_cmd "local: ${cmd[*]} > $RUN_DIR/${condition}_sender.csv 2> $RUN_DIR/${condition}_sender.err"
  "${cmd[@]}" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  stop_remote_pid "$REMOTE_RUN/${condition}_receiver.pid"
}

run_tcp_spat() {
  local condition="p5g-tcp-spat-baseline"
  start_tcp_receiver "$condition"
  local cmd=(./cpp/build/example_private_5g_latency_sender
    --transport tcp --host 10.100.100.6 --port "$TCP_PORT"
    --count "$COUNT" --interval-ms "$INTERVAL_MS"
    --message spat
    --condition-id "$condition" --request-id "$condition"
    "${COMMON_ARGS[@]}"
    --csv)
  log_cmd "local: ${cmd[*]} > $RUN_DIR/${condition}_sender.csv 2> $RUN_DIR/${condition}_sender.err"
  "${cmd[@]}" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  stop_remote_pid "$REMOTE_RUN/${condition}_receiver.pid"
}

start_mqtt_broker() {
  scp -i "$KEY" -q "$RUN_DIR/tools/minimal_mqtt_broker.py" "$REMOTE:$REMOTE_RUN/minimal_mqtt_broker.py"
  ssh_remote "pkill -f '[m]inimal_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT' || true"
  local cmd="cd '$REMOTE_RUN' && nohup python3 minimal_mqtt_broker.py --host 0.0.0.0 --port $MQTT_PORT > mqtt_broker.log 2> mqtt_broker.err < /dev/null & echo \$! > mqtt_broker.pid"
  log_cmd "remote: $cmd"
  ssh_launch_detached "$cmd"
  for _ in $(seq 1 20); do
    if ssh_remote "ss -ltn | grep -q ':$MQTT_PORT '"; then
      return 0
    fi
    sleep 1
  done
  ssh_remote "cat '$REMOTE_RUN/mqtt_broker.err' 2>/dev/null || true"
  return 1
}

start_mqtt_receiver() {
  local condition="$1"
  local out="$REMOTE_RUN/${condition}_receiver.csv"
  local err="$REMOTE_RUN/${condition}_receiver.err"
  local pid="$REMOTE_RUN/${condition}_receiver.pid"
  ssh_remote "pkill -f '[e]xample_private_5g_latency_receiver.*--transport mqtt' || true" || true
  local cmd="cd '$REMOTE_REPO' && mkdir -p '$REMOTE_RUN' && nohup stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port $MQTT_PORT --intersection-id fintechparking --source-id av-1 --run-id '$PAPER_RUN_ID' --condition-id '$condition' --condition-label private-5g-sunny-fintechparking --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --csv > '$out' 2> '$err' < /dev/null & echo \$! > '$pid'"
  log_cmd "remote: $cmd"
  ssh_launch_detached "$cmd"
  sleep 2
  ssh_remote "kill -0 \$(cat '$pid')"
}

run_mqtt_service_payload() {
  local payload="$1"
  local condition="p5g-mqtt-service-payload-$payload"
  start_mqtt_receiver "$condition"
  local cmd=(./cpp/build/example_private_5g_latency_sender
    --transport mqtt --host 10.100.100.6 --port "$MQTT_PORT"
    --count "$COUNT" --interval-ms "$INTERVAL_MS" --timeout-ms 5000
    --message service --payload-bytes "$payload"
    --intersection-id fintechparking --source-id av-1
    --condition-id "$condition" --request-id "$condition"
    "${COMMON_ARGS[@]}"
    --csv)
  log_cmd "local: ${cmd[*]} > $RUN_DIR/${condition}_sender.csv 2> $RUN_DIR/${condition}_sender.err"
  "${cmd[@]}" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  stop_remote_pid "$REMOTE_RUN/${condition}_receiver.pid"
}

run_mqtt_spat() {
  local condition="p5g-mqtt-spat-baseline"
  start_mqtt_receiver "$condition"
  local cmd=(./cpp/build/example_private_5g_latency_sender
    --transport mqtt --host 10.100.100.6 --port "$MQTT_PORT"
    --count "$COUNT" --interval-ms "$INTERVAL_MS" --timeout-ms 5000
    --message spat
    --intersection-id fintechparking --source-id av-1
    --condition-id "$condition" --request-id "$condition"
    "${COMMON_ARGS[@]}"
    --csv)
  log_cmd "local: ${cmd[*]} > $RUN_DIR/${condition}_sender.csv 2> $RUN_DIR/${condition}_sender.err"
  "${cmd[@]}" > "$RUN_DIR/${condition}_sender.csv" 2> "$RUN_DIR/${condition}_sender.err"
  stop_remote_pid "$REMOTE_RUN/${condition}_receiver.pid"
}

{
  echo "# Network Trial Metadata"
  echo
  echo "- Start time: $(date --iso-8601=seconds)"
  echo "- Count per condition: $COUNT"
  echo "- Interval ms: $INTERVAL_MS"
  echo "- TCP port: $TCP_PORT"
  echo "- MQTT port: $MQTT_PORT"
  echo "- Base station: 10.100.100.6"
  echo "- Mobility: stationary"
  echo "- Clock sync state: unsynced"
  echo "- MQTT broker: temporary minimal Python MQTT 3.1.1 QoS0 broker on base station"
} > "$RUN_DIR/network_trial_metadata.md"

cleanup_remote_receivers
for payload in 0 256 1024 4096; do
  echo "$(date --iso-8601=seconds) starting TCP service payload $payload"
  run_tcp_service_payload "$payload"
done

echo "$(date --iso-8601=seconds) starting TCP SPaT"
run_tcp_spat

echo "$(date --iso-8601=seconds) starting MQTT broker"
start_mqtt_broker
for payload in 0 256 1024 4096; do
  echo "$(date --iso-8601=seconds) starting MQTT service payload $payload"
  run_mqtt_service_payload "$payload"
done

echo "$(date --iso-8601=seconds) starting MQTT SPaT"
run_mqtt_spat

ssh_remote "pkill -f '[e]xample_private_5g_latency_receiver.*($TCP_PORT|$MQTT_PORT)' || true; if test -f '$REMOTE_RUN/mqtt_broker.pid'; then kill \$(cat '$REMOTE_RUN/mqtt_broker.pid') 2>/dev/null || true; fi"
{
  echo
  echo "- End time: $(date --iso-8601=seconds)"
} >> "$RUN_DIR/network_trial_metadata.md"
