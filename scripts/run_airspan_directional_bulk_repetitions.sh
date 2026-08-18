#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EDGE_HOST="${EDGE_HOST:?Set EDGE_HOST}"
SSH_KEY="${SSH_KEY:?Set SSH_KEY}"
EDGE_USER="${EDGE_USER:-d1}"
RUN_NAME="${RUN_NAME:-20260817_airspan_tdd_tcp_bulk_50mib_70_20_10_location_3_run_2}"
RAW_RUN_DIR="${RAW_RUN_DIR:-$REPO_ROOT/CISCO_AIRSPAN_STATS/${RUN_NAME}_unredacted}"
REMOTE_ROOT="${REMOTE_ROOT:-/home/d1/edge4av_followup/runs/$RUN_NAME}"
PROBE="$REPO_ROOT/scripts/private_5g_bulk_throughput.py"
REMOTE_PROBE="$REMOTE_ROOT/tools/private_5g_bulk_throughput.py"
REPETITIONS="${REPETITIONS:-10}"
TOTAL_BYTES="${TOTAL_BYTES:-52428800}"
BLOCK_BYTES="${BLOCK_BYTES:-65536}"
UPLOAD_PORT="${UPLOAD_PORT:-39050}"
DOWNLOAD_PORT="${DOWNLOAD_PORT:-39051}"
CONDITION_TIMEOUT_S="${CONDITION_TIMEOUT_S:-300}"

for value in "$REPETITIONS" "$TOTAL_BYTES" "$BLOCK_BYTES" "$UPLOAD_PORT" "$DOWNLOAD_PORT"; do
  [[ "$value" =~ ^[1-9][0-9]*$ ]] || { echo "positive integer required: $value" >&2; exit 2; }
done
[[ -f "$SSH_KEY" ]] || { echo "SSH key not found" >&2; exit 2; }
[[ "$TOTAL_BYTES" == "52428800" ]] || {
  echo "This acquisition requires exactly 50 MiB per direction" >&2
  exit 2
}

SSH_OPTS=(-i "$SSH_KEY" -o BatchMode=yes -o IdentitiesOnly=yes -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null)
SSH=(ssh "${SSH_OPTS[@]}" "$EDGE_USER@$EDGE_HOST")
RSYNC_RSH="ssh -i $SSH_KEY -o BatchMode=yes -o IdentitiesOnly=yes -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null"

log() { printf '[%s] %s\n' "$(date --iso-8601=seconds)" "$*"; }
ssh_bash() { "${SSH[@]}" "bash -lc $(printf '%q' "$1")"; }

cleanup_remote_condition() {
  local remote_dir="$1"
  ssh_bash "if test -f '$remote_dir/telemetry.pid'; then p=\$(cat '$remote_dir/telemetry.pid'); kill \"\$p\" 2>/dev/null || true; fi; if test -f '$remote_dir/server.pid'; then p=\$(cat '$remote_dir/server.pid'); kill \"\$p\" 2>/dev/null || true; fi" >/dev/null 2>&1 || true
}

preflight() {
  command -v python3 rsync ssh sha256sum timeout >/dev/null
  python3 "$PROBE" --self-test >/dev/null
  ip route get "$EDGE_HOST" | grep -q 'dev eno2' || { echo "edge route is not eno2" >&2; exit 1; }
  "${SSH[@]}" true
  ssh_bash "if ss -ltn | grep -Eq ':(39050|39051)( |$)'; then echo planned-port-occupied >&2; exit 1; fi; install -d '$REMOTE_ROOT/tools'"
  rsync -az -e "$RSYNC_RSH" "$PROBE" "$EDGE_USER@$EDGE_HOST:$REMOTE_PROBE"
  local local_hash remote_hash
  local_hash="$(sha256sum "$PROBE" | awk '{print $1}')"
  remote_hash="$(ssh_bash "sha256sum '$REMOTE_PROBE'" | awk '{print $1}')"
  [[ "$local_hash" == "$remote_hash" ]] || { echo "probe hash mismatch" >&2; exit 1; }
  ssh_bash "python3 '$REMOTE_PROBE' --self-test >/dev/null"
  mkdir -p "$RAW_RUN_DIR/tools"
  rsync -a "$PROBE" "$RAW_RUN_DIR/tools/private_5g_bulk_throughput.py"
  printf '%s\n' "$local_hash" > "$RAW_RUN_DIR/tools/private_5g_bulk_throughput.sha256"
}

start_remote_server() {
  local direction="$1" port="$2" remote_dir="$3"
  ssh_bash "install -d '$remote_dir'; nohup python3 /home/d1/edge4av_followup/ipi_2c043b3/scripts/collect_host_telemetry.py --output '$remote_dir/telemetry.jsonl' --interval-s 1 --interface enp0s31f6 --process-match 'private_5g_bulk_throughput.py' >'$remote_dir/telemetry.out' 2>'$remote_dir/telemetry.err' </dev/null & echo \$! >'$remote_dir/telemetry.pid'; nohup bash -c \"python3 -u '$REMOTE_PROBE' --role server --direction '$direction' --port '$port' --total-bytes '$TOTAL_BYTES' --block-bytes '$BLOCK_BYTES' >'$remote_dir/server.json' 2>'$remote_dir/server.err'; status=\\\$?; printf '%s\\n' \\\$status >'$remote_dir/server.exit_status'\" >'$remote_dir/server_wrapper.out' 2>'$remote_dir/server_wrapper.err' </dev/null & echo \$! >'$remote_dir/server.pid'"
  for _ in $(seq 1 30); do
    if ssh_bash "ss -ltn | grep -Eq ':$port( |$)'" >/dev/null 2>&1; then return 0; fi
    sleep 1
  done
  echo "remote $direction server did not listen" >&2
  return 1
}

validate_condition() {
  local local_dir="$1" direction="$2"
  python3 - "$local_dir" "$direction" "$TOTAL_BYTES" <<'PY'
import json, pathlib, sys
root=pathlib.Path(sys.argv[1]); direction=sys.argv[2]; total=int(sys.argv[3])
client=json.loads((root/'car/client.json').read_text())
server=json.loads((root/'d1/server.json').read_text())
errors=[]
for role,data in [('client',client),('server',server)]:
    if data.get('direction') != direction: errors.append(f'{role} direction mismatch')
    if data.get('total_bytes') != total: errors.append(f'{role} byte count mismatch')
    if data.get('validated_exact_bytes') is not True: errors.append(f'{role} did not validate')
if client.get('payload_crc32') != server.get('payload_crc32'): errors.append('endpoint CRC mismatch')
if (root/'d1/server.exit_status').read_text().strip() != '0': errors.append('server exit status nonzero')
summary={
    'status':'complete' if not errors else 'invalid', 'direction':direction,
    'total_bytes':total, 'payload_crc32':client.get('payload_crc32'),
    'client_local_data_elapsed_s':client.get('local_data_elapsed_s'),
    'client_local_data_throughput_mbps':client.get('local_data_throughput_mbps'),
    'server_local_data_elapsed_s':server.get('local_data_elapsed_s'),
    'server_local_data_throughput_mbps':server.get('local_data_throughput_mbps'),
    'receiver_throughput_mbps':server.get('local_data_throughput_mbps') if direction=='upload' else client.get('local_data_throughput_mbps'),
    'errors':errors,
}
(root/'validation_summary.json').write_text(json.dumps(summary,indent=2,sort_keys=True)+'\n')
print(json.dumps(summary,sort_keys=True))
if errors: raise SystemExit(1)
(root/'complete.marker').touch()
PY
}

run_condition() {
  local repetition="$1" direction="$2" port="$3"
  local rep_name local_dir remote_dir telemetry_pid started finished client_status
  printf -v rep_name 'repetition_%02d' "$repetition"
  local_dir="$RAW_RUN_DIR/$rep_name/$direction"
  remote_dir="$REMOTE_ROOT/$rep_name/$direction"
  if [[ -f "$local_dir/complete.marker" ]]; then log "Skipping completed $rep_name $direction"; return 0; fi
  [[ ! -e "$local_dir" ]] || { echo "refusing incomplete local condition $local_dir" >&2; exit 1; }
  ssh_bash "if test -e '$remote_dir'; then echo remote-condition-exists >&2; exit 1; fi"
  mkdir -p "$local_dir/car" "$local_dir/d1"
  start_remote_server "$direction" "$port" "$remote_dir"
  python3 "$REPO_ROOT/scripts/collect_host_telemetry.py" --output "$local_dir/car/telemetry.jsonl" --interval-s 1 --interface eno2 --process-match 'private_5g_bulk_throughput.py' >"$local_dir/car/telemetry.out" 2>"$local_dir/car/telemetry.err" &
  telemetry_pid=$!
  started="$(date --iso-8601=seconds)"
  log "Starting $rep_name $direction"
  set +e
  timeout --foreground --signal=TERM --kill-after=10s "${CONDITION_TIMEOUT_S}s" python3 "$PROBE" --role client --direction "$direction" --host "$EDGE_HOST" --port "$port" --total-bytes "$TOTAL_BYTES" --block-bytes "$BLOCK_BYTES" >"$local_dir/car/client.json" 2>"$local_dir/car/client.err"
  client_status=$?
  set -e
  finished="$(date --iso-8601=seconds)"
  kill "$telemetry_pid" 2>/dev/null || true
  wait "$telemetry_pid" 2>/dev/null || true
  printf '%s\n' "$started" >"$local_dir/started_at.txt"
  printf '%s\n' "$finished" >"$local_dir/finished_at.txt"
  printf '%s\n' "$client_status" >"$local_dir/car/client.exit_status"
  for _ in $(seq 1 30); do
    if ssh_bash "test -f '$remote_dir/server.exit_status'" >/dev/null 2>&1; then break; fi
    sleep 1
  done
  cleanup_remote_condition "$remote_dir"
  rsync -az -e "$RSYNC_RSH" "$EDGE_USER@$EDGE_HOST:$remote_dir/" "$local_dir/d1/"
  (( client_status == 0 )) || { echo "$rep_name $direction client status $client_status" >&2; exit 1; }
  validate_condition "$local_dir" "$direction"
  log "Completed $rep_name $direction"
  sleep 2
}

main() {
  preflight
  for repetition in $(seq 1 "$REPETITIONS"); do
    run_condition "$repetition" upload "$UPLOAD_PORT"
    run_condition "$repetition" download "$DOWNLOAD_PORT"
  done
  log "All $REPETITIONS upload/download repetitions complete"
}

main
