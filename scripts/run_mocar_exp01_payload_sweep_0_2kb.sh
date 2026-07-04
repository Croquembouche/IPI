#!/usr/bin/env bash
set -euo pipefail

PAYLOADS="${PAYLOADS:-0 256 512 1024 2048}"
COUNT="${COUNT:-1000}"
INTERVAL_MS="${INTERVAL_MS:-100}"
TIMEOUT_MS="${TIMEOUT_MS:-1000}"
REMOTE_PAYLOAD_TIMEOUT_SEC="${REMOTE_PAYLOAD_TIMEOUT_SEC:-1300}"
LOCAL_PAYLOAD_TIMEOUT_SEC="${LOCAL_PAYLOAD_TIMEOUT_SEC:-1500}"
MAX_PAYLOAD="${MAX_PAYLOAD:-2048}"
DATE_TAG="$(date +%Y%m%d)"
BASE_DIR="${BASE_DIR:-results/mocar_v2x}"
RUN_DIR="${RUN_DIR:-${BASE_DIR}/${DATE_TAG}_exp_01_payload_sweep_0_2kb_$(date +%H%M%S)}"
RUN_ID="$(basename "$RUN_DIR")"
REMOTE_RUN="/root/edge4av_exp/exp_01/$RUN_ID"
GPS_PID=""

mkdir -p "$RUN_DIR/commands" "$RUN_DIR/remote_obu" "$RUN_DIR/remote_rsu"

cat > "$RUN_DIR/run_context.env" <<EOF
RUN_DIR=$RUN_DIR
RUN_ID=$RUN_ID
REMOTE_RUN=$REMOTE_RUN
PAYLOADS=$PAYLOADS
COUNT=$COUNT
INTERVAL_MS=$INTERVAL_MS
TIMEOUT_MS=$TIMEOUT_MS
REMOTE_PAYLOAD_TIMEOUT_SEC=$REMOTE_PAYLOAD_TIMEOUT_SEC
LOCAL_PAYLOAD_TIMEOUT_SEC=$LOCAL_PAYLOAD_TIMEOUT_SEC
MAX_PAYLOAD=$MAX_PAYLOAD
METHOD=continuous_gnss_full_obu_restart_per_payload
EOF

cleanup() {
  set +e
  if [ -n "${GPS_PID:-}" ]; then
    kill "$GPS_PID" 2>/dev/null || true
    wait "$GPS_PID" 2>/dev/null || true
  fi
  OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
    "for name in ipi_custom_rtt diag-rssi cv2x_rx_app cv2x_tx_app; do for pid in \$(pidof \$name 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; done" \
    > "$RUN_DIR/commands/cleanup_obu.log" 2>&1 || true
  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    third_party/mocar/ssh_scripts/connectRSU_40.sh \
    "for name in ipi_custom_rtt cv2x_rx_app cv2x_tx_app; do for pid in \$(pidof \$name 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; done" \
    > "$RUN_DIR/commands/cleanup_rsu.log" 2>&1 || true
}
trap cleanup EXIT INT TERM

echo "[$(date --iso-8601=seconds)] run_dir=$RUN_DIR"
scripts/record_gps_for_experiment.sh "$RUN_DIR" \
  > "$RUN_DIR/gps_record_stdout.log" 2> "$RUN_DIR/gps_record_stderr.log" &
GPS_PID=$!
echo "$GPS_PID" > "$RUN_DIR/gps_record.pid"
echo "[$(date --iso-8601=seconds)] gps_pid=$GPS_PID"

OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
  "mkdir -p '$REMOTE_RUN/obu' '$REMOTE_RUN/signal' '$REMOTE_RUN/commands'" \
  > "$RUN_DIR/commands/obu_initial_prepare.log" 2>&1
JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
  third_party/mocar/ssh_scripts/connectRSU_40.sh \
  "mkdir -p '$REMOTE_RUN/rsu' '$REMOTE_RUN/commands'" \
  > "$RUN_DIR/commands/rsu_initial_prepare.log" 2>&1

for payload in $PAYLOADS; do
  if ! [[ "$payload" =~ ^[0-9]+$ ]] || [ "$payload" -gt "$MAX_PAYLOAD" ]; then
    echo "invalid payload in sweep: $payload" >&2
    exit 2
  fi

  echo "[$(date --iso-8601=seconds)] payload=$payload preparing obu"
  OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
    "export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; export LD_LIBRARY_PATH=/usr/local/lib:\${LD_LIBRARY_PATH:-}; for pid in \$(pidof ipi_custom_rtt diag-rssi cv2x_rx_app cv2x_tx_app 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; /usr/local/install/S99initapps.sh restart > '$REMOTE_RUN/commands/obu_full_restart_payload_${payload}.log' 2>&1 || true; sleep 8; cv2x-config --get-v2x-status > '$REMOTE_RUN/signal/obu_v2x_status_pre_payload_${payload}.log' 2>&1 || true; cv2x-config --get-subscription-info > '$REMOTE_RUN/signal/obu_subscription_pre_payload_${payload}.log' 2>&1 || true; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor|gpsd|chronyd' | grep -v grep > '$REMOTE_RUN/commands/obu_processes_pre_payload_${payload}.log' || true" \
    > "$RUN_DIR/commands/obu_prepare_payload_${payload}.log" 2>&1

  echo "[$(date --iso-8601=seconds)] payload=$payload starting rsu responder"
  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    third_party/mocar/ssh_scripts/connectRSU_40.sh \
    "set -e; cd /root/edge4av_exp; for pid in \$(pidof ipi_custom_rtt cv2x_rx_app cv2x_tx_app 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; cv2x-config --get-v2x-status > '$REMOTE_RUN/rsu/rsu_v2x_status_pre_payload_${payload}.log' 2>&1 || true; (timeout 2400s /root/edge4av_exp/bin/ipi_custom_rtt --role responder --node-id rsu-exp01-sweep --quiet > '$REMOTE_RUN/rsu/rsu_responder_payload_${payload}.stdout' 2> '$REMOTE_RUN/rsu/rsu_responder_payload_${payload}.stderr'; echo \$? > '$REMOTE_RUN/rsu/rsu_responder_payload_${payload}.status') >/dev/null 2>&1 & echo \$! > '$REMOTE_RUN/rsu/rsu_responder_payload_${payload}.launch_pid'" \
    > "$RUN_DIR/commands/rsu_prepare_payload_${payload}.log" 2>&1
  sleep 3

  echo "[$(date --iso-8601=seconds)] payload=$payload running"
  set +e
  OBU_PASSWORD="${OBU_PASSWORD:-root}" timeout "${LOCAL_PAYLOAD_TIMEOUT_SEC}s" third_party/mocar/ssh_scripts/connectOBU_10.sh \
    "cd /root/edge4av_exp; timeout ${REMOTE_PAYLOAD_TIMEOUT_SEC}s /root/edge4av_exp/bin/ipi_custom_rtt --role initiator --node-id obu-exp01-sweep --count $COUNT --interval-ms $INTERVAL_MS --timeout-ms $TIMEOUT_MS --payload-bytes $payload --csv > '$REMOTE_RUN/obu/payload_${payload}.csv' 2> '$REMOTE_RUN/obu/payload_${payload}.err'; rc=\$?; echo \$rc > '$REMOTE_RUN/obu/payload_${payload}.status'; lines=\$(wc -l < '$REMOTE_RUN/obu/payload_${payload}.csv' 2>/dev/null || echo 0); echo payload_done=$payload rc=\$rc lines=\$lines time=\$(date -Iseconds)" \
    | tee "$RUN_DIR/commands/obu_payload_${payload}.stdout"
  payload_ssh_rc=${PIPESTATUS[0]}
  set -e
  echo "local_payload_command_rc=$payload_ssh_rc time=$(date --iso-8601=seconds)" >> "$RUN_DIR/commands/obu_payload_${payload}.stdout"

  OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
    "cv2x-config --get-v2x-status > '$REMOTE_RUN/signal/obu_v2x_status_post_payload_${payload}.log' 2>&1 || true; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor' | grep -v grep > '$REMOTE_RUN/commands/obu_processes_post_payload_${payload}.log' || true; (logread 2>/dev/null || cat /var/log/messages 2>/dev/null || true) | grep -Ei 'rssi|snr|sinr|rsrp|rsrq|diag|cv2x' > '$REMOTE_RUN/signal/obu_signal_syslog_post_payload_${payload}.log' 2>/dev/null || true" \
    > "$RUN_DIR/commands/obu_post_payload_${payload}.log" 2>&1 || true
  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    third_party/mocar/ssh_scripts/connectRSU_40.sh \
    "for pid in \$(pidof ipi_custom_rtt 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor|ipi_custom_rtt' | grep -v grep > '$REMOTE_RUN/commands/rsu_processes_post_payload_${payload}.log' || true" \
    > "$RUN_DIR/commands/rsu_stop_payload_${payload}.log" 2>&1 || true
done

echo "[$(date --iso-8601=seconds)] fetching remote logs"
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

RUN_DIR="$RUN_DIR" PAYLOADS="$PAYLOADS" COUNT="$COUNT" python3 - <<'PY'
import csv
import os
import pathlib
import statistics

run = pathlib.Path(os.environ["RUN_DIR"])
payloads = [int(x) for x in os.environ["PAYLOADS"].split()]
count = os.environ["COUNT"]
summary = []
for payload in payloads:
    path = run / "remote_obu" / "obu" / f"payload_{payload}.csv"
    rows = list(csv.DictReader(path.open())) if path.exists() else []
    ok = [float(r["rtt_ms"]) for r in rows if r.get("success") == "true"]
    vals = sorted(ok)
    def pct(q):
        if not vals:
            return ""
        return vals[min(len(vals) - 1, round((len(vals) - 1) * q / 100))]
    summary.append({
        "payload_bytes": payload,
        "rows": len(rows),
        "success": len(ok),
        "success_rate": len(ok) / len(rows) if rows else 0,
        "avg_rtt_ms": sum(ok) / len(ok) if ok else "",
        "p50_rtt_ms": statistics.median(ok) if ok else "",
        "p95_rtt_ms": pct(95),
        "p99_rtt_ms": pct(99),
    })

gps_path = run / "gps" / "gps_samples.csv"
gps_latest = run / "gps" / "gps_latest.csv"
gps_rows = list(csv.DictReader(gps_path.open())) if gps_path.exists() else []
lat_values = [float(r.get("latitude_deg") or r.get("latitude")) for r in gps_rows if r.get("latitude_deg") or r.get("latitude")]
lon_values = [float(r.get("longitude_deg") or r.get("longitude")) for r in gps_rows if r.get("longitude_deg") or r.get("longitude")]
mean_lat = sum(lat_values) / len(lat_values) if lat_values else ""
mean_lon = sum(lon_values) / len(lon_values) if lon_values else ""
latest_line = ""
if gps_latest.exists():
    lines = gps_latest.read_text().strip().splitlines()
    latest_line = lines[-1] if len(lines) > 1 else ""

with (run / "payload_summary.csv").open("w", newline="") as f:
    fields = list(summary[0].keys())
    writer = csv.DictWriter(f, fieldnames=fields)
    writer.writeheader()
    writer.writerows(summary)

lines = [
    "# Mocar exp_01 0-2KB payload sweep",
    "",
    f"- Run ID: `{run.name}`",
    f"- Payloads: `{', '.join(str(p) for p in payloads)}`",
    f"- Attempts per payload: `{count}`",
    f"- GNSS samples: `{len(gps_rows)}`",
    f"- Mean GNSS latitude: `{mean_lat}`",
    f"- Mean GNSS longitude: `{mean_lon}`",
]
if latest_line:
    lines.append(f"- Latest GNSS CSV row: `{latest_line}`")
lines.extend([
    "",
    "| payload bytes | rows | success | success rate | avg RTT ms | p50 RTT ms | p95 RTT ms | p99 RTT ms |",
    "|---:|---:|---:|---:|---:|---:|---:|---:|",
])
for row in summary:
    def fmt(value):
        if value == "":
            return ""
        if isinstance(value, float):
            return f"{value:.3f}"
        return str(value)
    lines.append(
        f"| {row['payload_bytes']} | {row['rows']} | {row['success']} | "
        f"{row['success_rate']:.3f} | {fmt(row['avg_rtt_ms'])} | "
        f"{fmt(row['p50_rtt_ms'])} | {fmt(row['p95_rtt_ms'])} | {fmt(row['p99_rtt_ms'])} |"
    )
(run / "summary.md").write_text("\n".join(lines) + "\n")
print((run / "summary.md").read_text())
PY

OBU_PASSWORD="${OBU_PASSWORD:-root}" third_party/mocar/ssh_scripts/connectOBU_10.sh \
  "export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; /usr/local/install/S99initapps.sh restart cv2x_app >/tmp/ipi_exp01_sweep_restore.log 2>&1 || true; sleep 2; cat /tmp/ipi_exp01_sweep_restore.log; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor' | grep -v grep || true" \
  > "$RUN_DIR/commands/obu_restore_cv2x_app.log" 2>&1 || true

echo "$RUN_DIR"
