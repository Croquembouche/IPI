#!/usr/bin/env bash
set -uo pipefail

RUN_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$RUN_ROOT/../../.." && pwd)"
RUN_NAME="$(basename "$RUN_ROOT")"

EDGE_USER="${EDGE_USER:-d1}"
EDGE_HOST="${EDGE_HOST:-10.100.100.6}"
EDGE_REPO="${EDGE_REPO:-/home/d1/Documents/Github/IPI}"
EDGE_TARGET="${EDGE_USER}@${EDGE_HOST}"
: "${SSHPASS:?SSHPASS must be set for password-based edge SSH.}"

RUN_ID="${RUN_ID:-edge4av-real-20260702-detector-output-to-ipi-udp-fragmented-run-1}"
REMOTE_RUN_DIR="${EDGE_REPO}/results/real_5g/${RUN_NAME}"
UDP_PORT="${UDP_PORT:-36667}"
INTERSECTION_ID="${INTERSECTION_ID:-detector-output-to-ipi-udp-fragmented-run-1}"
SOURCE_ID="${SOURCE_ID:-av-1}"
UDP_MAX_DATAGRAM_BYTES="${UDP_MAX_DATAGRAM_BYTES:-1400}"

SSH_OPTS=(-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null)

mkdir -p "$RUN_ROOT/base_station" "$RUN_ROOT/commands"

log() {
  printf '[%s] %s\n' "$(date -Iseconds)" "$*" | tee -a "$RUN_ROOT/run.log"
}

ssh_edge_bash() {
  local remote_script="$1"
  sshpass -e ssh "${SSH_OPTS[@]}" "$EDGE_TARGET" "bash -lc $(printf '%q' "$remote_script")"
}

remote_cleanup() {
  ssh_edge_bash "pkill -TERM -f '[e]xample_private_5g_latency_udp_receiver' || true" >/dev/null 2>&1 || true
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

write_environment() {
  {
    echo "# Experiment 09 fragmented UDP collection"
    echo
    echo "- run_id: ${RUN_ID}"
    echo "- run_name: ${RUN_NAME}"
    echo "- edge_host: ${EDGE_HOST}"
    echo "- edge_repo: ${EDGE_REPO}"
    echo "- udp_port: ${UDP_PORT}"
    echo "- udp_max_datagram_bytes: ${UDP_MAX_DATAGRAM_BYTES}"
    echo "- collection_started: $(date -Iseconds)"
    echo "- vehicle_state: stationary"
    echo "- location_class: current/good-signal location; weak-signal repeat is separate"
    echo "- note: UDP detector outputs are application-fragmented and reassembled to avoid IP fragmentation over the private 5G path"
  } > "$RUN_ROOT/environment.md"
}

sync_and_build() {
  log "Building local C++ latency tools"
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

  log "Building edge C++ latency tools"
  ssh_edge_bash "cd '$EDGE_REPO' && cmake -S cpp -B cpp/build && cmake --build cpp/build -j2" \
    > "$RUN_ROOT/base_build.log" 2>&1
}

start_gnss() {
  log "Starting GNSS recorder"
  bash "$REPO_ROOT/scripts/record_gps_for_experiment.sh" "$RUN_ROOT" \
    > "$RUN_ROOT/gps_capture_stdout.log" \
    2> "$RUN_ROOT/gps_capture_stderr.log" &
  echo "$!" > "$RUN_ROOT/gps_capture.pid"
  sleep 5
}

start_udp_receiver() {
  local condition="$1"
  local payload="$2"

  remote_cleanup
  ssh_edge_bash "cd '$EDGE_REPO' && mkdir -p '$REMOTE_RUN_DIR/base_station' && rm -f '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid' && setsid -f bash -c \"echo \\\$\\\$ > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.pid'; exec stdbuf -oL -eL ./cpp/build/example_private_5g_latency_udp_receiver --port '$UDP_PORT' --run-id '$RUN_ID' --condition-id '$condition' --condition-label detector-output-to-ipi --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name detector_payload_bytes --vehicle-outcome-value '$payload' --vehicle-outcome-unit bytes --csv > '$REMOTE_RUN_DIR/base_station/${condition}_receiver.csv' 2> '$REMOTE_RUN_DIR/base_station/${condition}_receiver.err' < /dev/null\""
  sleep 2
}

fetch_udp_receiver() {
  local condition="$1"
  sshpass -e rsync -az \
    -e "ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null" \
    "$EDGE_TARGET:$REMOTE_RUN_DIR/base_station/${condition}_receiver."* \
    "$RUN_ROOT/base_station/" \
    > "$RUN_ROOT/rsync_from_base_station_${condition}.log" 2>&1 || true
}

run_udp_condition() {
  local payload="$1"
  local count="$2"
  local interval_ms="$3"
  local timeout_ms="$4"
  local max_seconds="$5"
  local condition="p5g-udp-detector-output-payload-${payload}"
  local sender_csv="$RUN_ROOT/${condition}_sender.csv"
  local sender_err="$RUN_ROOT/${condition}_sender.err"

  log "Running UDP condition ${condition} count=${count} interval_ms=${interval_ms} timeout_ms=${timeout_ms} max_seconds=${max_seconds}"
  start_udp_receiver "$condition" "$payload"

  {
    echo "./cpp/build/example_private_5g_latency_udp_sender --host '$EDGE_HOST' --port '$UDP_PORT' --count '$count' --interval-ms '$interval_ms' --timeout-ms '$timeout_ms' --udp-max-datagram-bytes '$UDP_MAX_DATAGRAM_BYTES' --message service --payload-bytes '$payload' --intersection-id '$INTERSECTION_ID' --source-id '$SOURCE_ID' --condition-id '$condition' --request-id '$condition' --run-id '$RUN_ID' --condition-label detector-output-to-ipi --av-id av-1 --obu-id obu-1 --rsu-id rsu-1 --network-load-level idle --qos-profile default --mobility-state stationary --clock-sync-state unsynced --service-success true --vehicle-outcome-name detector_payload_bytes --vehicle-outcome-value '$payload' --vehicle-outcome-unit bytes --csv"
  } >> "$RUN_ROOT/commands/experiment_09_udp_commands.txt"

  (
    cd "$REPO_ROOT" &&
      timeout --foreground "${max_seconds}s" \
        ./cpp/build/example_private_5g_latency_udp_sender \
          --host "$EDGE_HOST" \
          --port "$UDP_PORT" \
          --count "$count" \
          --interval-ms "$interval_ms" \
          --timeout-ms "$timeout_ms" \
          --udp-max-datagram-bytes "$UDP_MAX_DATAGRAM_BYTES" \
          --message service \
          --payload-bytes "$payload" \
          --intersection-id "$INTERSECTION_ID" \
          --source-id "$SOURCE_ID" \
          --condition-id "$condition" \
          --request-id "$condition" \
          --run-id "$RUN_ID" \
          --condition-label detector-output-to-ipi \
          --av-id av-1 \
          --obu-id obu-1 \
          --rsu-id rsu-1 \
          --network-load-level idle \
          --qos-profile default \
          --mobility-state stationary \
          --clock-sync-state unsynced \
          --service-success true \
          --vehicle-outcome-name detector_payload_bytes \
          --vehicle-outcome-value "$payload" \
          --vehicle-outcome-unit bytes \
          --csv
  ) > "$sender_csv" 2> "$sender_err"
  local sender_status=$?
  if [[ "$sender_status" -ne 0 ]]; then
    echo "sender_exit_status=${sender_status}" >> "$sender_err"
  fi

  sleep 1
  remote_cleanup
  sleep 1
  fetch_udp_receiver "$condition"
}

main() {
  write_environment
  sync_and_build
  start_gnss

  # Main detector-output-to-IPI UDP conditions. The 60000-byte upper control is
  # kept shorter because it may lose at message level when many fragments are required.
  run_udp_condition 0 1000 200 3000 420
  run_udp_condition 4096 1000 200 5000 720
  run_udp_condition 19648 1000 200 5000 900
  run_udp_condition 22816 1000 200 5000 900
  run_udp_condition 23968 1000 200 5000 900
  run_udp_condition 60000 100 200 8000 900

  log "Fragmented UDP experiment complete"
}

main "$@"
