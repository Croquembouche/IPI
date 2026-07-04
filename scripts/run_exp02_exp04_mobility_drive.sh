#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DATE_TAG="$(date +%Y%m%d)"
TIME_TAG="$(date +%H%M%S)"

RUN_ID="${RUN_ID:-edge4av-mobility-${DATE_TAG}T${TIME_TAG}}"
P5G_RUN_DIR="${P5G_RUN_DIR:-${REPO_ROOT}/results/real_5g/${DATE_TAG}_exp_04_private5g_mobility_${TIME_TAG}}"
MOCAR_RUN_DIR="${MOCAR_RUN_DIR:-${REPO_ROOT}/results/mocar_v2x/${DATE_TAG}_exp_02_radio_distance_mobility_${TIME_TAG}}"
REMOTE_MOCAR_RUN="${REMOTE_MOCAR_RUN:-/root/edge4av_exp/exp_02/${RUN_ID}}"

EDGE_USER="${EDGE_USER:-d1}"
EDGE_HOST="${EDGE_HOST:-10.100.100.6}"
EDGE_REPO="${EDGE_REPO:-/home/d1/Documents/Github/IPI}"
EDGE_TARGET="${EDGE_USER}@${EDGE_HOST}"

TCP_PORT="${TCP_PORT:-36666}"
UDP_PORT="${UDP_PORT:-36667}"
MQTT_PORT="${MQTT_PORT:-1883}"
P5G_PAYLOAD_BYTES="${P5G_PAYLOAD_BYTES:-1024}"
V2X_PAYLOAD_BYTES="${V2X_PAYLOAD_BYTES:-256}"
COUNT="${COUNT:-1000}"
INTERVAL_MS="${INTERVAL_MS:-200}"
TIMEOUT_MS="${TIMEOUT_MS:-1000}"
TRANSPORTS="${TRANSPORTS:-tcp udp mqtt}"
ROUTE_LABEL="${ROUTE_LABEL:-mobility-route}"
DISTANCE_M="${DISTANCE_M:-unknown}"
LINK_STATE="${LINK_STATE:-los}"
MOBILITY_STATE="${MOBILITY_STATE:-moving}"
CLOCK_SYNC_STATE="${CLOCK_SYNC_STATE:-unsynced}"
SOURCE_ID="${SOURCE_ID:-veh-01}"
AV_ID="${AV_ID:-veh-01}"
OBU_ID="${OBU_ID:-obu-1}"
RSU_ID="${RSU_ID:-rsu-1}"
INTERSECTION_ID="${INTERSECTION_ID:-exp04-mobility}"
SYNC_BUILD="${SYNC_BUILD:-1}"

V2X_COUNT="${V2X_COUNT:-$COUNT}"
V2X_INTERVAL_MS="${V2X_INTERVAL_MS:-$INTERVAL_MS}"
V2X_TIMEOUT_MS="${V2X_TIMEOUT_MS:-500}"
V2X_RESPONDER_TIMEOUT_SEC="${V2X_RESPONDER_TIMEOUT_SEC:-7200}"
V2X_SENDER_TIMEOUT_SEC="${V2X_SENDER_TIMEOUT_SEC:-1500}"

: "${SSHPASS:?Set SSHPASS for ${EDGE_TARGET}}"

SSH_OPTS=(-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null)
GPS_PID=""
P5G_PID=""
V2X_PID=""

mkdir -p \
  "$P5G_RUN_DIR/base_station" \
  "$P5G_RUN_DIR/commands" \
  "$P5G_RUN_DIR/tools" \
  "$MOCAR_RUN_DIR/commands" \
  "$MOCAR_RUN_DIR/remote_obu" \
  "$MOCAR_RUN_DIR/remote_rsu"

log() {
  printf '[%s] %s\n' "$(date -Iseconds)" "$*" | tee -a "$P5G_RUN_DIR/run.log" "$MOCAR_RUN_DIR/run.log"
}

record_command() {
  local target="$1"
  shift
  printf '%s\n' "$*" >> "$target"
}

ssh_edge_bash() {
  local remote_script="$1"
  sshpass -e ssh "${SSH_OPTS[@]}" "$EDGE_TARGET" \
    "bash -lc $(printf '%q' "ulimit -n 65535 >/dev/null 2>&1 || true; $remote_script")"
}

edge_rsync_from() {
  local remote_glob="$1"
  local local_dir="$2"
  local log_file="$3"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    "$EDGE_TARGET:$remote_glob" "$local_dir/" > "$log_file" 2>&1 || true
}

remote_cleanup_edge() {
  ssh_edge_bash "pkill -f '[e]xample_private_5g_latency_receiver' || true; pkill -f '[e]xample_private_5g_latency_udp_receiver' || true; pkill -f '[t]hreaded_mqtt_broker.py' || true" \
    >/dev/null 2>&1 || true
}

remote_cleanup_mocar() {
  OBU_PASSWORD="${OBU_PASSWORD:-root}" "$REPO_ROOT/third_party/mocar/ssh_scripts/connectOBU_10.sh" \
    "for name in ipi_custom_rtt cv2x_rx_app cv2x_tx_app; do for pid in \$(pidof \$name 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; done" \
    > "$MOCAR_RUN_DIR/commands/cleanup_obu.log" 2>&1 || true
  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    "$REPO_ROOT/third_party/mocar/ssh_scripts/connectRSU_40.sh" \
    "for name in ipi_custom_rtt cv2x_rx_app cv2x_tx_app; do for pid in \$(pidof \$name 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; done" \
    > "$MOCAR_RUN_DIR/commands/cleanup_rsu.log" 2>&1 || true
}

restore_mocar_apps() {
  OBU_PASSWORD="${OBU_PASSWORD:-root}" "$REPO_ROOT/third_party/mocar/ssh_scripts/connectOBU_10.sh" \
    "mkdir -p '$REMOTE_MOCAR_RUN/commands'; export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; /usr/local/install/S99initapps.sh restart > '$REMOTE_MOCAR_RUN/commands/obu_restore_apps.log' 2>&1 || true; sleep 2; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor' | grep -v grep > '$REMOTE_MOCAR_RUN/commands/obu_processes_after_restore.log' || true" \
    > "$MOCAR_RUN_DIR/commands/restore_obu_apps.log" 2>&1 || true
  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    "$REPO_ROOT/third_party/mocar/ssh_scripts/connectRSU_40.sh" \
    "mkdir -p '$REMOTE_MOCAR_RUN/commands'; export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; /usr/local/install/S99initapps.sh restart > '$REMOTE_MOCAR_RUN/commands/rsu_restore_apps.log' 2>&1 || true; sleep 2; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor' | grep -v grep > '$REMOTE_MOCAR_RUN/commands/rsu_processes_after_restore.log' || true" \
    > "$MOCAR_RUN_DIR/commands/restore_rsu_apps.log" 2>&1 || true
}

stop_gnss() {
  if [[ -n "${GPS_PID:-}" ]] && kill -0 "$GPS_PID" >/dev/null 2>&1; then
    kill -TERM "$GPS_PID" >/dev/null 2>&1 || true
    wait "$GPS_PID" >/dev/null 2>&1 || true
  fi
  GPS_PID=""
}

cleanup() {
  if [[ -n "${P5G_PID:-}" ]] && kill -0 "$P5G_PID" >/dev/null 2>&1; then
    kill -TERM "$P5G_PID" >/dev/null 2>&1 || true
  fi
  if [[ -n "${V2X_PID:-}" ]] && kill -0 "$V2X_PID" >/dev/null 2>&1; then
    kill -TERM "$V2X_PID" >/dev/null 2>&1 || true
  fi
  remote_cleanup_edge
  remote_cleanup_mocar
  restore_mocar_apps
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
    echo "# Experiment 04 Private-5G Mobility"
    echo
    echo "- run_id: ${RUN_ID}"
    echo "- paired_mocar_exp02_dir: ${MOCAR_RUN_DIR}"
    echo "- edge_host: ${EDGE_HOST}"
    echo "- payload_bytes: ${P5G_PAYLOAD_BYTES}"
    echo "- count_per_transport: ${COUNT}"
    echo "- interval_ms: ${INTERVAL_MS}"
    echo "- timeout_ms: ${TIMEOUT_MS}"
    echo "- transports: ${TRANSPORTS}"
    echo "- route_label: ${ROUTE_LABEL}"
    echo "- mobility_state: ${MOBILITY_STATE}"
    echo "- gnss: NovAtel ROS 2 recorder under gps/"
    echo "- collection_started: $(date -Iseconds)"
  } > "$P5G_RUN_DIR/environment.md"

  {
    echo "# Experiment 02 Radio Distance Mobility"
    echo
    echo "- run_id: ${RUN_ID}"
    echo "- paired_private5g_exp04_dir: ${P5G_RUN_DIR}"
    echo "- remote_mocar_run: ${REMOTE_MOCAR_RUN}"
    echo "- payload_bytes: ${V2X_PAYLOAD_BYTES}"
    echo "- count_per_transport_leg: ${V2X_COUNT}"
    echo "- interval_ms: ${V2X_INTERVAL_MS}"
    echo "- timeout_ms: ${V2X_TIMEOUT_MS}"
    echo "- distance_m: ${DISTANCE_M}"
    echo "- link_state: ${LINK_STATE}"
    echo "- mobility_state: ${MOBILITY_STATE}"
    echo "- gnss_source: ${P5G_RUN_DIR}/gps"
    echo "- collection_started: $(date -Iseconds)"
  } > "$MOCAR_RUN_DIR/environment.md"
}

sync_and_build() {
  if [[ "$SYNC_BUILD" != "1" ]]; then
    log "Skipping local/edge build because SYNC_BUILD=${SYNC_BUILD}"
    return
  fi

  log "Building local C++ tools"
  (
    cd "$REPO_ROOT" &&
      cmake -S cpp -B cpp/build &&
      cmake --build cpp/build -j2
  ) > "$P5G_RUN_DIR/local_build.log" 2>&1

  log "Syncing source to edge"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    --exclude '.git/' \
    --exclude 'cpp/build/' \
    --exclude 'results/' \
    "$REPO_ROOT/" "$EDGE_TARGET:$EDGE_REPO/" \
    > "$P5G_RUN_DIR/rsync_cpp_to_base_station.log" 2>&1

  log "Building edge C++ tools"
  ssh_edge_bash "cd '$EDGE_REPO' && cmake -S cpp -B cpp/build && cmake --build cpp/build -j2" \
    > "$P5G_RUN_DIR/base_build.log" 2>&1
}

start_gnss() {
  log "Starting continuous GNSS recorder"
  "$REPO_ROOT/scripts/record_gps_for_experiment.sh" "$P5G_RUN_DIR" \
    > "$P5G_RUN_DIR/gps_capture_stdout.log" \
    2> "$P5G_RUN_DIR/gps_capture_stderr.log" &
  GPS_PID="$!"
  echo "$GPS_PID" > "$P5G_RUN_DIR/gps_capture.pid"
  {
    echo "# GNSS Reference"
    echo
    echo "GNSS for this paired mobility run is recorded once under:"
    echo
    echo "- ${P5G_RUN_DIR}/gps"
  } > "$MOCAR_RUN_DIR/gnss_reference.md"
  sleep 5
}

prepare_edge() {
  remote_cleanup_edge
  ssh_edge_bash "mkdir -p '$EDGE_REPO/results/real_5g/$(basename "$P5G_RUN_DIR")/base_station' '$EDGE_REPO/results/real_5g/$(basename "$P5G_RUN_DIR")/tools'"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    "$REPO_ROOT/scripts/threaded_mqtt_broker.py" \
    "$EDGE_TARGET:$EDGE_REPO/results/real_5g/$(basename "$P5G_RUN_DIR")/tools/threaded_mqtt_broker.py" \
    > "$P5G_RUN_DIR/rsync_mqtt_broker_to_base_station.log" 2>&1
}

prepare_mocar() {
  log "Preparing Mocar OBU/RSU for exp 02"
  OBU_PASSWORD="${OBU_PASSWORD:-root}" "$REPO_ROOT/third_party/mocar/ssh_scripts/connectOBU_10.sh" \
    "mkdir -p '$REMOTE_MOCAR_RUN/obu' '$REMOTE_MOCAR_RUN/commands' '$REMOTE_MOCAR_RUN/signal'; export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; for pid in \$(pidof ipi_custom_rtt cv2x_rx_app cv2x_tx_app 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; /usr/local/install/S99initapps.sh restart > '$REMOTE_MOCAR_RUN/commands/obu_initial_restart.log' 2>&1 || true; sleep 8; cv2x-config --get-v2x-status > '$REMOTE_MOCAR_RUN/signal/obu_v2x_status_pre.log' 2>&1 || true; cv2x-config --get-subscription-info > '$REMOTE_MOCAR_RUN/signal/obu_subscription_pre.log' 2>&1 || true" \
    > "$MOCAR_RUN_DIR/commands/obu_prepare.log" 2>&1

  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    "$REPO_ROOT/third_party/mocar/ssh_scripts/connectRSU_40.sh" \
    "set -e; mkdir -p '$REMOTE_MOCAR_RUN/rsu' '$REMOTE_MOCAR_RUN/commands'; cd /root/edge4av_exp; for pid in \$(pidof ipi_custom_rtt cv2x_rx_app cv2x_tx_app 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; cv2x-config --get-v2x-status > '$REMOTE_MOCAR_RUN/rsu/rsu_v2x_status_pre.log' 2>&1 || true; (timeout ${V2X_RESPONDER_TIMEOUT_SEC}s /root/edge4av_exp/bin/ipi_custom_rtt --role responder --node-id rsu-exp02-mobility --quiet > '$REMOTE_MOCAR_RUN/rsu/rsu_responder.stdout' 2> '$REMOTE_MOCAR_RUN/rsu/rsu_responder.stderr'; echo \$? > '$REMOTE_MOCAR_RUN/rsu/rsu_responder.status') >/dev/null 2>&1 & echo \$! > '$REMOTE_MOCAR_RUN/rsu/rsu_responder.launch_pid'" \
    > "$MOCAR_RUN_DIR/commands/rsu_prepare_start_responder.log" 2>&1
  sleep 3
}

start_tcp_receiver() {
  local condition="$1"
  local remote_run="$EDGE_REPO/results/real_5g/$(basename "$P5G_RUN_DIR")"
  local cmd
  cmd="cd '$EDGE_REPO' && mkdir -p '$remote_run/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$remote_run/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport tcp --port '$TCP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label private-5g-mobile --rsu-id '$RSU_ID' --network-load-level idle --qos-profile default --mobility-state '$MOBILITY_STATE' --clock-sync-state '$CLOCK_SYNC_STATE' --service-success true --vehicle-outcome-name route_label --vehicle-outcome-value '$ROUTE_LABEL' --vehicle-outcome-unit label --csv >> '$remote_run/base_station/${condition}_receiver.csv' 2>> '$remote_run/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "$P5G_RUN_DIR/commands/experiment_04_commands.txt" "remote: $cmd"
  ssh_edge_bash "$cmd"
  wait_tcp_port "$EDGE_HOST" "$TCP_PORT" "TCP receiver"
}

start_udp_receiver() {
  local condition="$1"
  local remote_run="$EDGE_REPO/results/real_5g/$(basename "$P5G_RUN_DIR")"
  local cmd
  cmd="cd '$EDGE_REPO' && mkdir -p '$remote_run/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$remote_run/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_udp_receiver --port '$UDP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label private-5g-mobile --rsu-id '$RSU_ID' --network-load-level idle --qos-profile default --mobility-state '$MOBILITY_STATE' --clock-sync-state '$CLOCK_SYNC_STATE' --service-success true --vehicle-outcome-name route_label --vehicle-outcome-value '$ROUTE_LABEL' --vehicle-outcome-unit label --csv >> '$remote_run/base_station/${condition}_receiver.csv' 2>> '$remote_run/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "$P5G_RUN_DIR/commands/experiment_04_commands.txt" "remote: $cmd"
  ssh_edge_bash "$cmd"
  sleep 2
}

start_mqtt_stack() {
  local condition="$1"
  local remote_run="$EDGE_REPO/results/real_5g/$(basename "$P5G_RUN_DIR")"
  local broker_cmd
  local receiver_cmd
  broker_cmd="cd '$remote_run/tools' && setsid -f bash -c \"echo \\\$\\\$ > '$remote_run/base_station/threaded_mqtt_broker_${condition}.pid'; exec python3 threaded_mqtt_broker.py --host 0.0.0.0 --port '$MQTT_PORT' >> '$remote_run/base_station/threaded_mqtt_broker_${condition}.log' 2>> '$remote_run/base_station/threaded_mqtt_broker_${condition}.err' < /dev/null\""
  record_command "$P5G_RUN_DIR/commands/experiment_04_commands.txt" "remote: $broker_cmd"
  ssh_edge_bash "$broker_cmd"
  wait_tcp_port "$EDGE_HOST" "$MQTT_PORT" "MQTT broker"

  receiver_cmd="cd '$EDGE_REPO' && mkdir -p '$remote_run/base_station' && setsid -f bash -c \"echo \\\$\\\$ > '$remote_run/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_receiver --transport mqtt --host 127.0.0.1 --port '$MQTT_PORT' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label private-5g-mobile --rsu-id '$RSU_ID' --network-load-level idle --qos-profile default --mobility-state '$MOBILITY_STATE' --clock-sync-state '$CLOCK_SYNC_STATE' --service-success true --vehicle-outcome-name route_label --vehicle-outcome-value '$ROUTE_LABEL' --vehicle-outcome-unit label --csv >> '$remote_run/base_station/${condition}_receiver.csv' 2>> '$remote_run/base_station/${condition}_receiver.err' < /dev/null\""
  record_command "$P5G_RUN_DIR/commands/experiment_04_commands.txt" "remote: $receiver_cmd"
  ssh_edge_bash "$receiver_cmd"
  sleep 2
}

start_p5g_sender() {
  local transport="$1"
  local condition="$2"
  local sender_csv="$P5G_RUN_DIR/${condition}_sender.csv"
  local sender_err="$P5G_RUN_DIR/${condition}_sender.err"
  local cmd
  if [[ "$transport" == "udp" ]]; then
    cmd="./cpp/build/example_private_5g_latency_udp_sender --host '$EDGE_HOST' --port '$UDP_PORT' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms '$TIMEOUT_MS' --udp-max-datagram-bytes 1400 --message service --payload-bytes '$P5G_PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label private-5g-mobile --request-id '$condition' --av-id '$AV_ID' --obu-id '$OBU_ID' --rsu-id '$RSU_ID' --network-load-level idle --qos-profile default --mobility-state '$MOBILITY_STATE' --clock-sync-state '$CLOCK_SYNC_STATE' --service-success true --vehicle-outcome-name route_label --vehicle-outcome-value '$ROUTE_LABEL' --vehicle-outcome-unit label --csv"
  else
    cmd="./cpp/build/example_private_5g_latency_sender --transport '$transport' --host '$EDGE_HOST' --port '$([[ "$transport" == "mqtt" ]] && echo "$MQTT_PORT" || echo "$TCP_PORT")' --count '$COUNT' --interval-ms '$INTERVAL_MS' --timeout-ms '$TIMEOUT_MS' --continue-on-failure --message service --payload-bytes '$P5G_PAYLOAD_BYTES' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --run-id '$RUN_ID' --condition-id '$condition' --condition-label private-5g-mobile --request-id '$condition' --av-id '$AV_ID' --obu-id '$OBU_ID' --rsu-id '$RSU_ID' --network-load-level idle --qos-profile default --mobility-state '$MOBILITY_STATE' --clock-sync-state '$CLOCK_SYNC_STATE' --service-success true --vehicle-outcome-name route_label --vehicle-outcome-value '$ROUTE_LABEL' --vehicle-outcome-unit label --csv"
  fi
  record_command "$P5G_RUN_DIR/commands/experiment_04_commands.txt" "local: $cmd > '$sender_csv' 2> '$sender_err'"
  (
    cd "$REPO_ROOT"
    eval "$cmd"
  ) > "$sender_csv" 2> "$sender_err" &
  P5G_PID="$!"
}

start_v2x_sender() {
  local leg="$1"
  local condition="mocar-exp02-${ROUTE_LABEL}-${LINK_STATE}-${MOBILITY_STATE}-${leg}-payload-${V2X_PAYLOAD_BYTES}"
  local remote_csv="$REMOTE_MOCAR_RUN/obu/${condition}.csv"
  local remote_err="$REMOTE_MOCAR_RUN/obu/${condition}.err"
  local remote_status="$REMOTE_MOCAR_RUN/obu/${condition}.status"
  local remote_cmd
  remote_cmd="cd /root/edge4av_exp; timeout ${V2X_SENDER_TIMEOUT_SEC}s /root/edge4av_exp/bin/ipi_custom_rtt --role initiator --node-id obu-exp02-${leg} --count $V2X_COUNT --interval-ms $V2X_INTERVAL_MS --timeout-ms $V2X_TIMEOUT_MS --payload-bytes $V2X_PAYLOAD_BYTES --csv > '$remote_csv' 2> '$remote_err'; rc=\$?; echo \$rc > '$remote_status'; echo condition=$condition rc=\$rc time=\$(date -Iseconds)"
  record_command "$MOCAR_RUN_DIR/commands/experiment_02_commands.txt" "obu: $remote_cmd"
  OBU_PASSWORD="${OBU_PASSWORD:-root}" "$REPO_ROOT/third_party/mocar/ssh_scripts/connectOBU_10.sh" "$remote_cmd" \
    > "$MOCAR_RUN_DIR/commands/${condition}_ssh.stdout" \
    2> "$MOCAR_RUN_DIR/commands/${condition}_ssh.stderr" &
  V2X_PID="$!"
}

fetch_edge_artifacts() {
  local condition="$1"
  local remote_run="$EDGE_REPO/results/real_5g/$(basename "$P5G_RUN_DIR")"
  edge_rsync_from "$remote_run/base_station/${condition}*" "$P5G_RUN_DIR/base_station" "$P5G_RUN_DIR/rsync_from_base_station_${condition}.log"
  edge_rsync_from "$remote_run/base_station/threaded_mqtt_broker_${condition}*" "$P5G_RUN_DIR/base_station" "$P5G_RUN_DIR/rsync_from_base_station_${condition}_broker.log"
}

fetch_mocar_artifacts() {
  log "Fetching Mocar OBU/RSU artifacts"
  OBU_PASSWORD="${OBU_PASSWORD:-root}" "$REPO_ROOT/third_party/mocar/ssh_scripts/connectOBU_10.sh" \
    "tar -C '$REMOTE_MOCAR_RUN' -czf - ." \
    > "$MOCAR_RUN_DIR/remote_obu.tgz" 2> "$MOCAR_RUN_DIR/commands/fetch_obu_tar.err" || true
  if [[ -s "$MOCAR_RUN_DIR/remote_obu.tgz" ]]; then
    tar -xzf "$MOCAR_RUN_DIR/remote_obu.tgz" -C "$MOCAR_RUN_DIR/remote_obu"
  fi

  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    "$REPO_ROOT/third_party/mocar/ssh_scripts/connectRSU_40.sh" \
    "tar -C '$REMOTE_MOCAR_RUN' -czf - ." \
    > "$MOCAR_RUN_DIR/remote_rsu.tgz" 2> "$MOCAR_RUN_DIR/commands/fetch_rsu_tar.err" || true
  if [[ -s "$MOCAR_RUN_DIR/remote_rsu.tgz" ]]; then
    tar -xzf "$MOCAR_RUN_DIR/remote_rsu.tgz" -C "$MOCAR_RUN_DIR/remote_rsu"
  fi
}

run_leg() {
  local transport="$1"
  local p5g_condition="p5g-${transport}-mobility-${ROUTE_LABEL}-payload-${P5G_PAYLOAD_BYTES}"
  log "Starting paired mobility leg: transport=${transport}"
  remote_cleanup_edge
  sleep 2

  if [[ "$transport" == "tcp" ]]; then
    start_tcp_receiver "$p5g_condition"
  elif [[ "$transport" == "udp" ]]; then
    start_udp_receiver "$p5g_condition"
  elif [[ "$transport" == "mqtt" ]]; then
    start_mqtt_stack "$p5g_condition"
  else
    log "ERROR: unsupported transport ${transport}"
    exit 2
  fi

  start_v2x_sender "$transport"
  local v2x_pid="$V2X_PID"
  sleep 1
  start_p5g_sender "$transport" "$p5g_condition"
  local p5g_pid="$P5G_PID"

  local p5g_status=0
  local v2x_status=0
  if ! wait "$p5g_pid"; then
    p5g_status=1
  fi
  if ! wait "$v2x_pid"; then
    v2x_status=1
  fi
  printf 'transport=%s p5g_status=%s v2x_status=%s completed_at=%s\n' \
    "$transport" "$p5g_status" "$v2x_status" "$(date -Iseconds)" >> "$P5G_RUN_DIR/leg_status.txt"

  remote_cleanup_edge
  sleep 2
  fetch_edge_artifacts "$p5g_condition"
}

summarize() {
  P5G_RUN_DIR="$P5G_RUN_DIR" MOCAR_RUN_DIR="$MOCAR_RUN_DIR" RUN_ID="$RUN_ID" TRANSPORTS="$TRANSPORTS" \
  P5G_PAYLOAD_BYTES="$P5G_PAYLOAD_BYTES" V2X_PAYLOAD_BYTES="$V2X_PAYLOAD_BYTES" COUNT="$COUNT" \
  ROUTE_LABEL="$ROUTE_LABEL" LINK_STATE="$LINK_STATE" MOBILITY_STATE="$MOBILITY_STATE" python3 - <<'PY'
import csv
import os
import pathlib
import statistics

p5g = pathlib.Path(os.environ["P5G_RUN_DIR"])
mocar = pathlib.Path(os.environ["MOCAR_RUN_DIR"])
transports = os.environ["TRANSPORTS"].split()


def pct(values, q):
    if not values:
        return ""
    values = sorted(values)
    return values[min(len(values) - 1, round((len(values) - 1) * q / 100))]


def summarize_latency_csv(path, success_field="accepted"):
    rows = list(csv.DictReader(path.open())) if path.exists() else []
    ok = []
    for row in rows:
        if row.get(success_field) == "true" and row.get("rtt_ms"):
            try:
                ok.append(float(row["rtt_ms"]))
            except ValueError:
                pass
    return {
        "rows": len(rows),
        "success": len(ok),
        "success_rate": len(ok) / len(rows) if rows else 0,
        "p50_rtt_ms": statistics.median(ok) if ok else "",
        "p95_rtt_ms": pct(ok, 95),
        "p99_rtt_ms": pct(ok, 99),
    }


def write_csv(path, fieldnames, rows):
    with path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


p5g_rows = []
for transport in transports:
    condition = f"p5g-{transport}-mobility-{os.environ['ROUTE_LABEL']}-payload-{os.environ['P5G_PAYLOAD_BYTES']}"
    stats = summarize_latency_csv(p5g / f"{condition}_sender.csv", "accepted")
    p5g_rows.append({"transport": transport, "condition_id": condition, **stats})
write_csv(
    p5g / "mobility_summary.csv",
    ["transport", "condition_id", "rows", "success", "success_rate", "p50_rtt_ms", "p95_rtt_ms", "p99_rtt_ms"],
    p5g_rows,
)

v2x_rows = []
for transport in transports:
    condition = (
        f"mocar-exp02-{os.environ['ROUTE_LABEL']}-"
        f"{os.environ['LINK_STATE']}-{os.environ['MOBILITY_STATE']}-"
        f"{transport}-payload-{os.environ['V2X_PAYLOAD_BYTES']}"
    )
    path = mocar / "remote_obu" / "obu" / f"{condition}.csv"
    stats = summarize_latency_csv(path, "success")
    v2x_rows.append({"paired_transport": transport, "condition_id": condition, **stats})
write_csv(
    mocar / "radio_mobility_summary.csv",
    ["paired_transport", "condition_id", "rows", "success", "success_rate", "p50_rtt_ms", "p95_rtt_ms", "p99_rtt_ms"],
    v2x_rows,
)

gps_path = p5g / "gps" / "gps_samples.csv"
gps_rows = list(csv.DictReader(gps_path.open())) if gps_path.exists() else []
lat = []
lon = []
speed = []
for row in gps_rows:
    try:
        if row.get("latitude_deg") and row.get("longitude_deg"):
            lat.append(float(row["latitude_deg"]))
            lon.append(float(row["longitude_deg"]))
        if row.get("horizontal_speed_mps"):
            speed.append(float(row["horizontal_speed_mps"]))
    except ValueError:
        pass
gps_lines = [
    f"- GNSS samples: `{len(gps_rows)}`",
    f"- Mean GNSS latitude: `{sum(lat) / len(lat) if lat else ''}`",
    f"- Mean GNSS longitude: `{sum(lon) / len(lon) if lon else ''}`",
]
if speed:
    gps_lines.extend([
        f"- Horizontal speed median m/s: `{statistics.median(speed):.6f}`",
        f"- Horizontal speed p95 m/s: `{pct(speed, 95):.6f}`",
        f"- Horizontal speed max m/s: `{max(speed):.6f}`",
    ])

def fmt(value):
    if value == "":
        return ""
    if isinstance(value, float):
        return f"{value:.3f}"
    return str(value)

p5g_md = [
    "# Experiment 04 Private-5G Mobility",
    "",
    f"- Run ID: `{os.environ['RUN_ID']}`",
    f"- Payload: `{os.environ['P5G_PAYLOAD_BYTES']} B`",
    f"- Paired Mocar exp 02 directory: `{mocar}`",
    *gps_lines,
    "",
    "| transport | rows | success | success rate | p50 ms | p95 ms | p99 ms |",
    "| --- | ---: | ---: | ---: | ---: | ---: | ---: |",
]
for row in p5g_rows:
    p5g_md.append(
        f"| {row['transport']} | {row['rows']} | {row['success']} | {row['success_rate']:.3f} | "
        f"{fmt(row['p50_rtt_ms'])} | {fmt(row['p95_rtt_ms'])} | {fmt(row['p99_rtt_ms'])} |"
    )
(p5g / "summary.md").write_text("\n".join(p5g_md) + "\n")

v2x_md = [
    "# Experiment 02 Radio Distance Mobility",
    "",
    f"- Run ID: `{os.environ['RUN_ID']}`",
    f"- Payload: `{os.environ['V2X_PAYLOAD_BYTES']} B`",
    f"- Paired private-5G exp 04 directory: `{p5g}`",
    f"- GNSS source: `{p5g / 'gps'}`",
    "",
    "| paired private-5G transport | rows | success | success rate | p50 ms | p95 ms | p99 ms |",
    "| --- | ---: | ---: | ---: | ---: | ---: | ---: |",
]
for row in v2x_rows:
    v2x_md.append(
        f"| {row['paired_transport']} | {row['rows']} | {row['success']} | {row['success_rate']:.3f} | "
        f"{fmt(row['p50_rtt_ms'])} | {fmt(row['p95_rtt_ms'])} | {fmt(row['p99_rtt_ms'])} |"
    )
(mocar / "summary.md").write_text("\n".join(v2x_md) + "\n")
PY
}

main() {
  log "Preparing paired exp 02/04 mobility run"
  write_environment
  sync_and_build
  prepare_edge
  prepare_mocar
  start_gnss

  log "Starting drive collection. Begin driving now if you have not already started."
  for transport in $TRANSPORTS; do
    run_leg "$transport"
  done

  fetch_mocar_artifacts
  stop_gnss
  summarize
  log "Paired mobility run complete"
  echo "$P5G_RUN_DIR"
  echo "$MOCAR_RUN_DIR"
}

main "$@"
