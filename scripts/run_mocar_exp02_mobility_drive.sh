#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DATE_TAG="$(date +%Y%m%d)"
TIME_TAG="$(date +%H%M%S)"

RUN_ID="${RUN_ID:-edge4av-exp02-mobility-${DATE_TAG}T${TIME_TAG}}"
MOCAR_RUN_DIR="${MOCAR_RUN_DIR:-${REPO_ROOT}/results/mocar_v2x/${DATE_TAG}_exp_02_radio_distance_mobility_${TIME_TAG}}"
REMOTE_MOCAR_RUN="${REMOTE_MOCAR_RUN:-/root/edge4av_exp/exp_02/${RUN_ID}}"

V2X_PAYLOAD_BYTES="${V2X_PAYLOAD_BYTES:-256}"
COUNT="${COUNT:-1000}"
INTERVAL_MS="${INTERVAL_MS:-200}"
TIMEOUT_MS="${TIMEOUT_MS:-500}"
ROUTE_LABEL="${ROUTE_LABEL:-mobility-route}"
LINK_STATE="${LINK_STATE:-los}"
MOBILITY_STATE="${MOBILITY_STATE:-moving}"
RUN_LABEL="${RUN_LABEL:-run}"
RESPONDER_TIMEOUT_SEC="${RESPONDER_TIMEOUT_SEC:-7200}"
SENDER_TIMEOUT_SEC="${SENDER_TIMEOUT_SEC:-1500}"

GPS_PID=""

mkdir -p "$MOCAR_RUN_DIR/commands" "$MOCAR_RUN_DIR/remote_obu" "$MOCAR_RUN_DIR/remote_rsu"

log() {
  printf '[%s] %s\n' "$(date -Iseconds)" "$*" | tee -a "$MOCAR_RUN_DIR/run.log"
}

connect_obu() {
  OBU_PASSWORD="${OBU_PASSWORD:-root}" "$REPO_ROOT/third_party/mocar/ssh_scripts/connectOBU_10.sh" "$1"
}

connect_rsu() {
  JUMP_PASSWORD="${JUMP_PASSWORD:-mist1234}" RSU_PASSWORD="${RSU_PASSWORD:-root}" \
    "$REPO_ROOT/third_party/mocar/ssh_scripts/connectRSU_40.sh" "$1"
}

remote_time_cmd() {
  cat <<'EOF'
epoch="$(date +%s%N)"
case "$epoch" in
  *N*) epoch="$(($(date +%s) * 1000000000))" ;;
esac
mono="$(awk '{printf "%.0f", $1 * 1000000000}' /proc/uptime)"
printf '%s,%s\n' "$epoch" "$mono"
EOF
}

record_clock_alignment() {
  local device="$1"
  local label="$2"
  local out_file="$MOCAR_RUN_DIR/clock_alignment.csv"
  local before after remote_output remote_line remote_epoch remote_mono midpoint offset rtt

  if [[ ! -f "$out_file" ]]; then
    echo "device,label,local_send_epoch_ns,device_epoch_ns,device_mono_ns,local_recv_epoch_ns,rtt_ns,estimated_device_minus_local_epoch_ns" > "$out_file"
  fi

  before="$(date +%s%N)"
  if [[ "$device" == "obu" ]]; then
    remote_output="$(connect_obu "$(remote_time_cmd)" 2>/dev/null || true)"
  else
    remote_output="$(connect_rsu "$(remote_time_cmd)" 2>/dev/null || true)"
  fi
  after="$(date +%s%N)"
  remote_line="$(printf '%s\n' "$remote_output" | awk -F, '/^[0-9]+,[0-9]+$/ {line=$0} END {print line}')"

  if [[ -z "$remote_line" ]]; then
    printf '%s,%s,%s,,,%s,,\n' "$device" "$label" "$before" "$after" >> "$out_file"
    return
  fi

  remote_epoch="${remote_line%%,*}"
  remote_mono="${remote_line#*,}"
  midpoint="$(((before + after) / 2))"
  offset="$((remote_epoch - midpoint))"
  rtt="$((after - before))"
  printf '%s,%s,%s,%s,%s,%s,%s,%s\n' "$device" "$label" "$before" "$remote_epoch" "$remote_mono" "$after" "$rtt" "$offset" >> "$out_file"
}

cleanup_remote_mocar() {
  connect_obu \
    "for name in ipi_custom_rtt cv2x_rx_app cv2x_tx_app; do for pid in \$(pidof \$name 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; done" \
    > "$MOCAR_RUN_DIR/commands/cleanup_obu.log" 2>&1 || true
  connect_rsu \
    "for name in ipi_custom_rtt cv2x_rx_app cv2x_tx_app; do for pid in \$(pidof \$name 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; done" \
    > "$MOCAR_RUN_DIR/commands/cleanup_rsu.log" 2>&1 || true
}

restore_mocar_apps() {
  connect_obu \
    "mkdir -p '$REMOTE_MOCAR_RUN/commands'; export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; /usr/local/install/S99initapps.sh restart > '$REMOTE_MOCAR_RUN/commands/obu_restore_apps.log' 2>&1 || true; sleep 2; ps -ef | grep -E 'cv2x_stack|cv2x_app|cv2x_monitor' | grep -v grep > '$REMOTE_MOCAR_RUN/commands/obu_processes_after_restore.log' || true" \
    > "$MOCAR_RUN_DIR/commands/restore_obu_apps.log" 2>&1 || true
  connect_rsu \
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
  cleanup_remote_mocar
  restore_mocar_apps
  stop_gnss
}
trap cleanup EXIT INT TERM

write_environment() {
  {
    echo "# Experiment 02 Radio Distance Mobility"
    echo
    echo "- run_id: ${RUN_ID}"
    echo "- remote_mocar_run: ${REMOTE_MOCAR_RUN}"
    echo "- payload_bytes: ${V2X_PAYLOAD_BYTES}"
    echo "- count: ${COUNT}"
    echo "- interval_ms: ${INTERVAL_MS}"
    echo "- timeout_ms: ${TIMEOUT_MS}"
    echo "- route_label: ${ROUTE_LABEL}"
    echo "- link_state: ${LINK_STATE}"
    echo "- mobility_state: ${MOBILITY_STATE}"
    echo "- run_label: ${RUN_LABEL}"
    echo "- gnss_source: ${MOCAR_RUN_DIR}/gps"
    echo "- clock_alignment: ${MOCAR_RUN_DIR}/clock_alignment.csv"
    echo "- collection_started: $(date -Iseconds)"
  } > "$MOCAR_RUN_DIR/environment.md"
}

start_gnss() {
  log "Starting continuous GNSS recorder"
  "$REPO_ROOT/scripts/record_gps_for_experiment.sh" "$MOCAR_RUN_DIR" \
    > "$MOCAR_RUN_DIR/gps_capture_stdout.log" \
    2> "$MOCAR_RUN_DIR/gps_capture_stderr.log" &
  GPS_PID="$!"
  echo "$GPS_PID" > "$MOCAR_RUN_DIR/gps_capture.pid"
  sleep 5
}

prepare_mocar() {
  log "Preparing Mocar OBU/RSU for exp 02"
  connect_obu \
    "mkdir -p '$REMOTE_MOCAR_RUN/obu' '$REMOTE_MOCAR_RUN/commands' '$REMOTE_MOCAR_RUN/signal'; export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin; for pid in \$(pidof ipi_custom_rtt cv2x_rx_app cv2x_tx_app 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; /usr/local/install/S99initapps.sh restart > '$REMOTE_MOCAR_RUN/commands/obu_initial_restart.log' 2>&1 || true; sleep 8; cv2x-config --get-v2x-status > '$REMOTE_MOCAR_RUN/signal/obu_v2x_status_pre.log' 2>&1 || true; cv2x-config --get-subscription-info > '$REMOTE_MOCAR_RUN/signal/obu_subscription_pre.log' 2>&1 || true" \
    > "$MOCAR_RUN_DIR/commands/obu_prepare.log" 2>&1

  connect_rsu \
    "set -e; mkdir -p '$REMOTE_MOCAR_RUN/rsu' '$REMOTE_MOCAR_RUN/commands'; cd /root/edge4av_exp; for pid in \$(pidof ipi_custom_rtt cv2x_rx_app cv2x_tx_app 2>/dev/null); do kill -9 \$pid 2>/dev/null || true; done; cv2x-config --get-v2x-status > '$REMOTE_MOCAR_RUN/rsu/rsu_v2x_status_pre.log' 2>&1 || true; (timeout ${RESPONDER_TIMEOUT_SEC}s /root/edge4av_exp/bin/ipi_custom_rtt --role responder --node-id rsu-exp02-mobility --quiet > '$REMOTE_MOCAR_RUN/rsu/rsu_responder.stdout' 2> '$REMOTE_MOCAR_RUN/rsu/rsu_responder.stderr'; echo \$? > '$REMOTE_MOCAR_RUN/rsu/rsu_responder.status') >/dev/null 2>&1 & echo \$! > '$REMOTE_MOCAR_RUN/rsu/rsu_responder.launch_pid'" \
    > "$MOCAR_RUN_DIR/commands/rsu_prepare_start_responder.log" 2>&1
  sleep 3
}

run_v2x_sender() {
  local condition="mocar-exp02-${ROUTE_LABEL}-${LINK_STATE}-${MOBILITY_STATE}-${RUN_LABEL}-payload-${V2X_PAYLOAD_BYTES}"
  local remote_csv="$REMOTE_MOCAR_RUN/obu/${condition}.csv"
  local remote_err="$REMOTE_MOCAR_RUN/obu/${condition}.err"
  local remote_status="$REMOTE_MOCAR_RUN/obu/${condition}.status"
  local remote_cmd

  remote_cmd="cd /root/edge4av_exp; timeout ${SENDER_TIMEOUT_SEC}s /root/edge4av_exp/bin/ipi_custom_rtt --role initiator --node-id obu-exp02-${RUN_LABEL} --count $COUNT --interval-ms $INTERVAL_MS --timeout-ms $TIMEOUT_MS --payload-bytes $V2X_PAYLOAD_BYTES --csv > '$remote_csv' 2> '$remote_err'; rc=\$?; echo \$rc > '$remote_status'; echo condition=$condition rc=\$rc time=\$(date -Iseconds)"
  echo "obu: $remote_cmd" > "$MOCAR_RUN_DIR/commands/experiment_02_commands.txt"
  echo "CONDITION_ID=$condition" > "$MOCAR_RUN_DIR/live_context.env"

  log "Starting OBU V2X initiator condition=${condition}"
  record_clock_alignment obu before_sender
  connect_obu "$remote_cmd" \
    > "$MOCAR_RUN_DIR/commands/${condition}_ssh.stdout" \
    2> "$MOCAR_RUN_DIR/commands/${condition}_ssh.stderr"
  record_clock_alignment obu after_sender
}

fetch_mocar_artifacts() {
  log "Fetching Mocar OBU/RSU artifacts"
  connect_obu "tar -C '$REMOTE_MOCAR_RUN' -czf - ." \
    > "$MOCAR_RUN_DIR/remote_obu.tgz" 2> "$MOCAR_RUN_DIR/commands/fetch_obu_tar.err" || true
  if [[ -s "$MOCAR_RUN_DIR/remote_obu.tgz" ]]; then
    tar -xzf "$MOCAR_RUN_DIR/remote_obu.tgz" -C "$MOCAR_RUN_DIR/remote_obu"
  fi

  connect_rsu "tar -C '$REMOTE_MOCAR_RUN' -czf - ." \
    > "$MOCAR_RUN_DIR/remote_rsu.tgz" 2> "$MOCAR_RUN_DIR/commands/fetch_rsu_tar.err" || true
  if [[ -s "$MOCAR_RUN_DIR/remote_rsu.tgz" ]]; then
    tar -xzf "$MOCAR_RUN_DIR/remote_rsu.tgz" -C "$MOCAR_RUN_DIR/remote_rsu"
  fi
}

summarize() {
  MOCAR_RUN_DIR="$MOCAR_RUN_DIR" RUN_ID="$RUN_ID" V2X_PAYLOAD_BYTES="$V2X_PAYLOAD_BYTES" \
  COUNT="$COUNT" TIMEOUT_MS="$TIMEOUT_MS" ROUTE_LABEL="$ROUTE_LABEL" LINK_STATE="$LINK_STATE" \
  MOBILITY_STATE="$MOBILITY_STATE" RUN_LABEL="$RUN_LABEL" python3 - <<'PY'
import bisect
import csv
import os
import pathlib
import statistics

run_dir = pathlib.Path(os.environ["MOCAR_RUN_DIR"])
condition = (
    f"mocar-exp02-{os.environ['ROUTE_LABEL']}-"
    f"{os.environ['LINK_STATE']}-{os.environ['MOBILITY_STATE']}-"
    f"{os.environ['RUN_LABEL']}-payload-{os.environ['V2X_PAYLOAD_BYTES']}"
)
v2x_path = run_dir / "remote_obu" / "obu" / f"{condition}.csv"
gps_path = run_dir / "gps" / "gps_samples.csv"
clock_path = run_dir / "clock_alignment.csv"


def pct(values, q):
    if not values:
        return ""
    values = sorted(values)
    return values[min(len(values) - 1, round((len(values) - 1) * q / 100))]


def read_rows(path):
    return list(csv.DictReader(path.open())) if path.exists() else []


v2x_rows = read_rows(v2x_path)
ok = [float(row["rtt_ms"]) for row in v2x_rows if row.get("success") == "true" and row.get("rtt_ms")]
timeouts = sum(1 for row in v2x_rows if row.get("detail") == "timeout")

clock_rows = read_rows(clock_path)
offsets = []
rtts = []
for row in clock_rows:
    if row.get("device") == "obu" and row.get("estimated_device_minus_local_epoch_ns"):
        try:
            offsets.append(int(row["estimated_device_minus_local_epoch_ns"]))
            rtts.append(int(row["rtt_ns"]))
        except ValueError:
            pass
clock_offset_ns = int(statistics.median(offsets)) if offsets else 0
clock_rtt_ns = int(statistics.median(rtts)) if rtts else ""

gps_rows = read_rows(gps_path)
gps_samples = []
for row in gps_rows:
    try:
        ts = int(row["system_time_ns"])
    except (KeyError, ValueError):
        continue
    gps_samples.append((ts, row))
gps_samples.sort(key=lambda item: item[0])
gps_times = [item[0] for item in gps_samples]

joined_path = run_dir / "v2x_gnss_by_send_time.csv"
joined_fields = []
if v2x_rows:
    joined_fields.extend(v2x_rows[0].keys())
joined_fields.extend([
    "send_local_epoch_ns",
    "nearest_gnss_system_time_ns",
    "gnss_delta_ms",
    "send_latitude_deg",
    "send_longitude_deg",
    "send_altitude_m",
    "send_horizontal_speed_mps",
    "send_position_source",
    "send_speed_source",
    "clock_offset_obu_minus_local_ns",
])
with joined_path.open("w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=joined_fields)
    writer.writeheader()
    for row in v2x_rows:
        out = dict(row)
        send_local = ""
        nearest_ts = ""
        delta_ms = ""
        gps = {}
        try:
            origin_send_epoch = int(row.get("origin_send_epoch_ns") or "0")
        except ValueError:
            origin_send_epoch = 0
        if origin_send_epoch and gps_times:
            send_local_int = origin_send_epoch - clock_offset_ns
            index = bisect.bisect_left(gps_times, send_local_int)
            candidates = []
            if index < len(gps_times):
                candidates.append(index)
            if index > 0:
                candidates.append(index - 1)
            best = min(candidates, key=lambda i: abs(gps_times[i] - send_local_int))
            nearest_ts = gps_times[best]
            gps = gps_samples[best][1]
            send_local = send_local_int
            delta_ms = (nearest_ts - send_local_int) / 1_000_000.0
        out.update({
            "send_local_epoch_ns": send_local,
            "nearest_gnss_system_time_ns": nearest_ts,
            "gnss_delta_ms": f"{delta_ms:.3f}" if delta_ms != "" else "",
            "send_latitude_deg": gps.get("latitude_deg", ""),
            "send_longitude_deg": gps.get("longitude_deg", ""),
            "send_altitude_m": gps.get("altitude_m", ""),
            "send_horizontal_speed_mps": gps.get("horizontal_speed_mps", ""),
            "send_position_source": gps.get("position_source", ""),
            "send_speed_source": gps.get("speed_source", ""),
            "clock_offset_obu_minus_local_ns": clock_offset_ns if offsets else "",
        })
        writer.writerow(out)

summary_row = {
    "condition_id": condition,
    "rows": len(v2x_rows),
    "success": len(ok),
    "timeouts": timeouts,
    "success_rate": len(ok) / len(v2x_rows) if v2x_rows else 0,
    "p50_rtt_ms": statistics.median(ok) if ok else "",
    "p95_rtt_ms": pct(ok, 95),
    "p99_rtt_ms": pct(ok, 99),
    "timeout_ms": os.environ["TIMEOUT_MS"],
    "gnss_samples": len(gps_rows),
    "clock_offset_obu_minus_local_ns": clock_offset_ns if offsets else "",
    "clock_alignment_median_rtt_ns": clock_rtt_ns,
}
with (run_dir / "radio_mobility_summary.csv").open("w", newline="") as f:
    fields = list(summary_row.keys())
    writer = csv.DictWriter(f, fieldnames=fields)
    writer.writeheader()
    writer.writerow(summary_row)

def fmt(value):
    if value == "":
        return ""
    if isinstance(value, float):
        return f"{value:.3f}"
    return str(value)

md = [
    "# Experiment 02 Radio Distance Mobility",
    "",
    f"- Run ID: `{os.environ['RUN_ID']}`",
    f"- Condition ID: `{condition}`",
    f"- Payload: `{os.environ['V2X_PAYLOAD_BYTES']} B`",
    f"- Timeout threshold: `{os.environ['TIMEOUT_MS']} ms`",
    f"- Rows: `{len(v2x_rows)}`",
    f"- Success: `{len(ok)}`",
    f"- Timeouts: `{timeouts}`",
    f"- Success rate: `{summary_row['success_rate']:.3f}`",
    f"- p50 RTT ms: `{fmt(summary_row['p50_rtt_ms'])}`",
    f"- p95 RTT ms: `{fmt(summary_row['p95_rtt_ms'])}`",
    f"- p99 RTT ms: `{fmt(summary_row['p99_rtt_ms'])}`",
    f"- GNSS samples: `{len(gps_rows)}`",
    f"- OBU-local clock offset ns: `{summary_row['clock_offset_obu_minus_local_ns']}`",
    f"- Clock alignment median RTT ns: `{summary_row['clock_alignment_median_rtt_ns']}`",
    f"- V2X/GNSS send-time join: `{joined_path}`",
]
(run_dir / "summary.md").write_text("\n".join(md) + "\n")
print("\n".join(md))
PY
}

main() {
  log "Preparing exp 02 mobility run"
  write_environment
  record_clock_alignment obu before_prepare
  record_clock_alignment rsu before_prepare
  prepare_mocar
  start_gnss
  run_v2x_sender
  record_clock_alignment rsu after_sender
  fetch_mocar_artifacts
  stop_gnss
  summarize
  log "Exp 02 mobility run complete"
  echo "$MOCAR_RUN_DIR"
}

main "$@"
