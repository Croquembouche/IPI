#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <payload-bytes> [count]" >&2
  exit 2
fi

PAYLOAD="$1"
COUNT="${2:-1000}"
MAX_PAYLOAD="${MAX_PAYLOAD:-2048}"
INTERVAL_MS="${INTERVAL_MS:-100}"
TIMEOUT_MS="${TIMEOUT_MS:-1000}"
DATE_TAG="$(date +%Y%m%d)"
BASE_DIR="${BASE_DIR:-results/mocar_v2x}"
RUN_DIR="${BASE_DIR}/${DATE_TAG}_exp_01_payload_${PAYLOAD}_full_$(date +%H%M%S)"
RUN_ID="$(basename "$RUN_DIR")"
REMOTE_RUN="/root/edge4av_exp/exp_01/$RUN_ID"
GPS_PID=""

if ! [[ "$PAYLOAD" =~ ^[0-9]+$ ]]; then
  echo "payload-bytes must be an integer" >&2
  exit 2
fi
if [ "$PAYLOAD" -gt "$MAX_PAYLOAD" ]; then
  echo "payload-bytes must be <= $MAX_PAYLOAD for Mocar exp_01 sweeps" >&2
  exit 2
fi

mkdir -p "$RUN_DIR/commands" "$RUN_DIR/obu" "$RUN_DIR/rsu" "$RUN_DIR/signal" \
  "$RUN_DIR/remote_obu" "$RUN_DIR/remote_rsu"

cat > "$RUN_DIR/run_context.env" <<EOF
RUN_DIR=$RUN_DIR
RUN_ID=$RUN_ID
REMOTE_RUN=$REMOTE_RUN
PAYLOAD=$PAYLOAD
COUNT=$COUNT
INTERVAL_MS=$INTERVAL_MS
TIMEOUT_MS=$TIMEOUT_MS
MAX_PAYLOAD=$MAX_PAYLOAD
METHOD=single_payload_full_obu_restart
EOF

cleanup() {
  set +e
  if [ -n "${GPS_PID:-}" ]; then
    kill "$GPS_PID" 2>/dev/null || true
    wait "$GPS_PID" 2>/dev/null || true
  fi
  OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
    "for name in ipi_custom_rtt diag-rssi; do for pid in \$(pidof \$name 2>/dev/null); do ppid=\$(awk '/^PPid:/ {print \$2}' /proc/\$pid/status 2>/dev/null); kill -9 \$pid 2>/dev/null || true; [ -n \"\$ppid\" ] && [ \"\$ppid\" != 1 ] && kill -9 \$ppid 2>/dev/null || true; done; done" \
    > "$RUN_DIR/commands/cleanup_obu.log" 2>&1 || true
  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    third_party/mocar/ssh_scripts/connectRSU_40.sh \
    "for pid in \$(pidof ipi_custom_rtt 2>/dev/null); do ppid=\$(awk '/^PPid:/ {print \$2}' /proc/\$pid/status 2>/dev/null); kill -9 \$pid 2>/dev/null || true; [ -n \"\$ppid\" ] && [ \"\$ppid\" != 1 ] && kill -9 \$ppid 2>/dev/null || true; done" \
    > "$RUN_DIR/commands/cleanup_rsu.log" 2>&1 || true
}
trap cleanup EXIT INT TERM

echo "[$(date --iso-8601=seconds)] run_dir=$RUN_DIR payload=$PAYLOAD"

scripts/record_gps_for_experiment.sh "$RUN_DIR" \
  > "$RUN_DIR/gps_record_stdout.log" 2> "$RUN_DIR/gps_record_stderr.log" &
GPS_PID=$!
echo "$GPS_PID" > "$RUN_DIR/gps_record.pid"
echo "[$(date --iso-8601=seconds)] gps_pid=$GPS_PID"

OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
  "export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; export LD_LIBRARY_PATH=/usr/local/lib:\${LD_LIBRARY_PATH:-}; mkdir -p '$REMOTE_RUN/obu' '$REMOTE_RUN/signal' '$REMOTE_RUN/commands'; /usr/local/install/S99initapps.sh restart > '$REMOTE_RUN/commands/obu_full_restart.log' 2>&1 || true; sleep 8; cv2x-config --get-v2x-status > '$REMOTE_RUN/signal/obu_v2x_status_pre.log' 2>&1 || true; cv2x-config --get-subscription-info > '$REMOTE_RUN/signal/obu_subscription_pre.log' 2>&1 || true; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor|gpsd|chronyd' | grep -v grep > '$REMOTE_RUN/commands/obu_processes_pre.log' || true; diag-rssi --help > '$REMOTE_RUN/signal/diag_rssi_help.log' 2>&1 || true; (timeout 2400s diag-rssi -T -t -f -l 0 -d 1 > '$REMOTE_RUN/signal/obu_diag_rssi.log' 2> '$REMOTE_RUN/signal/obu_diag_rssi.err' & echo \$! > '$REMOTE_RUN/signal/obu_diag_rssi.pid')" \
  > "$RUN_DIR/commands/obu_prepare.log" 2>&1
echo "[$(date --iso-8601=seconds)] obu prepared"

JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
  third_party/mocar/ssh_scripts/connectRSU_40.sh \
  "set -e; cd /root/edge4av_exp; mkdir -p '$REMOTE_RUN/rsu' '$REMOTE_RUN/commands'; for pid in \$(pidof ipi_custom_rtt 2>/dev/null); do ppid=\$(awk '/^PPid:/ {print \$2}' /proc/\$pid/status 2>/dev/null); kill -9 \$pid 2>/dev/null || true; [ -n \"\$ppid\" ] && [ \"\$ppid\" != 1 ] && kill -9 \$ppid 2>/dev/null || true; done; cv2x-config --get-v2x-status > '$REMOTE_RUN/rsu/rsu_v2x_status_pre.log' 2>&1 || true; (timeout 2400s /root/edge4av_exp/bin/ipi_custom_rtt --role responder --node-id rsu-exp01-responder --quiet > '$REMOTE_RUN/rsu/rsu_responder.stdout' 2> '$REMOTE_RUN/rsu/rsu_responder.stderr'; echo \$? > '$REMOTE_RUN/rsu/rsu_responder.status') >/dev/null 2>&1 & echo \$! > '$REMOTE_RUN/rsu/rsu_responder.launch_pid'" \
  > "$RUN_DIR/commands/rsu_prepare.log" 2>&1
echo "[$(date --iso-8601=seconds)] rsu responder started"
sleep 3

OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
  "cd /root/edge4av_exp; /root/edge4av_exp/bin/ipi_custom_rtt --role initiator --node-id obu-exp01-full --count $COUNT --interval-ms $INTERVAL_MS --timeout-ms $TIMEOUT_MS --payload-bytes $PAYLOAD --csv > '$REMOTE_RUN/obu/payload_${PAYLOAD}.csv' 2> '$REMOTE_RUN/obu/payload_${PAYLOAD}.err'; rc=\$?; echo \$rc > '$REMOTE_RUN/obu/payload_${PAYLOAD}.status'; lines=\$(wc -l < '$REMOTE_RUN/obu/payload_${PAYLOAD}.csv' 2>/dev/null || echo 0); echo payload_done=$PAYLOAD rc=\$rc lines=\$lines time=\$(date -Iseconds)" \
  | tee "$RUN_DIR/commands/obu_payload_${PAYLOAD}.stdout"
echo "[$(date --iso-8601=seconds)] payload command completed"

OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
  "kill \$(cat '$REMOTE_RUN/signal/obu_diag_rssi.pid' 2>/dev/null) 2>/dev/null || true; for name in ipi_custom_rtt diag-rssi; do for pid in \$(pidof \$name 2>/dev/null); do ppid=\$(awk '/^PPid:/ {print \$2}' /proc/\$pid/status 2>/dev/null); kill -9 \$pid 2>/dev/null || true; [ -n \"\$ppid\" ] && [ \"\$ppid\" != 1 ] && kill -9 \$ppid 2>/dev/null || true; done; done; cv2x-config --get-v2x-status > '$REMOTE_RUN/signal/obu_v2x_status_post.log' 2>&1 || true; (logread 2>/dev/null || cat /var/log/messages 2>/dev/null || true) | grep -Ei 'rssi|snr|sinr|rsrp|rsrq|diag' > '$REMOTE_RUN/signal/obu_signal_syslog_post.log' 2>/dev/null || true; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor' | grep -v grep > '$REMOTE_RUN/commands/obu_processes_post.log' || true" \
  > "$RUN_DIR/commands/obu_stop_post.log" 2>&1 || true
JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
  third_party/mocar/ssh_scripts/connectRSU_40.sh \
  "for pid in \$(pidof ipi_custom_rtt 2>/dev/null); do ppid=\$(awk '/^PPid:/ {print \$2}' /proc/\$pid/status 2>/dev/null); kill -9 \$pid 2>/dev/null || true; [ -n \"\$ppid\" ] && [ \"\$ppid\" != 1 ] && kill -9 \$ppid 2>/dev/null || true; done; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor|ipi_custom_rtt' | grep -v grep > '$REMOTE_RUN/commands/rsu_processes_post.log' || true" \
  > "$RUN_DIR/commands/rsu_stop_post.log" 2>&1 || true

OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
  "tar -C '$REMOTE_RUN' -czf - ." \
  > "$RUN_DIR/remote_obu.tgz" 2> "$RUN_DIR/commands/fetch_obu_tar.err" || true
[ -s "$RUN_DIR/remote_obu.tgz" ] && tar -xzf "$RUN_DIR/remote_obu.tgz" -C "$RUN_DIR/remote_obu"
JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
  third_party/mocar/ssh_scripts/connectRSU_40.sh \
  "tar -C '$REMOTE_RUN' -czf - ." \
  > "$RUN_DIR/remote_rsu.tgz" 2> "$RUN_DIR/commands/fetch_rsu_tar.err" || true
[ -s "$RUN_DIR/remote_rsu.tgz" ] && tar -xzf "$RUN_DIR/remote_rsu.tgz" -C "$RUN_DIR/remote_rsu"

if [ -n "${GPS_PID:-}" ]; then
  kill "$GPS_PID" 2>/dev/null || true
  wait "$GPS_PID" 2>/dev/null || true
  GPS_PID=""
fi

python3 - <<PY
import csv, pathlib, statistics
payload = "$PAYLOAD"
path = pathlib.Path("$RUN_DIR/remote_obu/obu/payload_${PAYLOAD}.csv")
rows = list(csv.DictReader(path.open()))
ok = [float(r["rtt_ms"]) for r in rows if r["success"] == "true"]
print("summary payload", payload, "rows", len(rows), "success", len(ok),
      "avg", (sum(ok) / len(ok) if ok else 0),
      "p50", (statistics.median(ok) if ok else 0))
PY

OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
  "export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; export LD_LIBRARY_PATH=/usr/local/lib:\${LD_LIBRARY_PATH:-}; /usr/local/install/S99initapps.sh restart cv2x_app >/tmp/ipi_exp01_payload_${PAYLOAD}_restore.log 2>&1 || true; sleep 2; cat /tmp/ipi_exp01_payload_${PAYLOAD}_restore.log; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor' | grep -v grep || true" \
  > "$RUN_DIR/commands/obu_restore_cv2x_app.log" 2>&1 || true

echo "$RUN_DIR"
