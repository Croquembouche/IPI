#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: run_airspan_followup_matrix.sh <physical-placement> <C1|C2|C3|C4|C5|C6> [...]

Required environment:
  EDGE_HOST          Reachable d1 edge address
  SSHPASS            Password used by sshpass; never written to results
  SIGNAL_CLASS       Operator-declared location class, such as weak or strong
  LOCATION_ID        Stable, non-sensitive location label for this run block

Optional environment:
  RUN_NAME           Default: YYYYMMDD_airspan_followup_run_1
  COUNT              Default: 500 probes per client
  INTERVAL_MS        Default: 200
  REPETITIONS        Default: 2
  SIMPLE_SETUP_LEAD_S
                     Default: 45 seconds before boundary selection
  C4_MQTT_SETUP_LEAD_S
                     Default: 180 seconds before boundary selection
  ENABLE_GNSS        Passed to the per-transport runner; default: 1
  RESUME_VALIDATED   Set to 1 to skip existing transport directories only
                     after their manifests and exact row counts revalidate

Repetition 1 uses TCP, MQTT, UDP. Repetition 2 reverses the order to UDP,
MQTT, TCP. Every transport occupies one exact five-minute ACP sample. The
matrix stops immediately if a runner fails, a sender file is incomplete, or a
workload timestamp falls outside its declared sample.
EOF
}

if (( $# < 2 )); then
  usage >&2
  exit 2
fi

PHYSICAL_PLACEMENT="$1"
shift
if [[ ! "$PHYSICAL_PLACEMENT" =~ ^[A-Za-z0-9_.-]+$ ]]; then
  echo "physical placement contains unsupported characters: $PHYSICAL_PLACEMENT" >&2
  exit 2
fi

: "${EDGE_HOST:?Set EDGE_HOST to the reachable d1 edge address}"
: "${SSHPASS:?Set SSHPASS for password-based edge SSH}"
: "${SIGNAL_CLASS:?Set SIGNAL_CLASS to the operator-declared location class}"
: "${LOCATION_ID:?Set LOCATION_ID to a stable, non-sensitive location label}"

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUNNER="$REPO_ROOT/scripts/run_airspan_followup_condition.sh"
RUN_NAME="${RUN_NAME:-$(date +%Y%m%d)_airspan_followup_run_1}"
COUNT="${COUNT:-500}"
INTERVAL_MS="${INTERVAL_MS:-200}"
REPETITIONS="${REPETITIONS:-2}"
SIMPLE_SETUP_LEAD_S="${SIMPLE_SETUP_LEAD_S:-45}"
C4_MQTT_SETUP_LEAD_S="${C4_MQTT_SETUP_LEAD_S:-180}"
ENABLE_GNSS="${ENABLE_GNSS:-1}"
RESUME_VALIDATED="${RESUME_VALIDATED:-0}"
EDGE_USER="${EDGE_USER:-d1}"

for value_name in RUN_NAME SIGNAL_CLASS LOCATION_ID; do
  value="${!value_name}"
  if [[ ! "$value" =~ ^[A-Za-z0-9_.-]+$ ]]; then
    echo "$value_name contains unsupported characters: $value" >&2
    exit 2
  fi
done
for number_name in COUNT INTERVAL_MS REPETITIONS SIMPLE_SETUP_LEAD_S C4_MQTT_SETUP_LEAD_S; do
  number="${!number_name}"
  if [[ ! "$number" =~ ^[1-9][0-9]*$ ]]; then
    echo "$number_name must be a positive integer" >&2
    exit 2
  fi
done
if (( REPETITIONS != 2 )); then
  echo "This collection plan requires exactly two repetitions" >&2
  exit 2
fi
if [[ "$RESUME_VALIDATED" != "0" && "$RESUME_VALIDATED" != "1" ]]; then
  echo "RESUME_VALIDATED must be 0 or 1" >&2
  exit 2
fi

CONDITIONS=("$@")
for condition in "${CONDITIONS[@]}"; do
  condition="${condition^^}"
  if [[ ! "$condition" =~ ^C[1-6]$ ]]; then
    echo "unsupported condition: $condition" >&2
    exit 2
  fi
done

command -v date flock jq sshpass >/dev/null
[[ -x "$RUNNER" ]] || { echo "missing runner: $RUNNER" >&2; exit 1; }

RUN_DIR="$REPO_ROOT/results/real_5g/$RUN_NAME"
SCHEDULE_FILE="$RUN_DIR/matrix_schedule.tsv"
MATRIX_LOG="$RUN_DIR/matrix_run.log"
mkdir -p "$RUN_DIR"
exec 9>"$RUN_DIR/.matrix.lock"
if ! flock -n 9; then
  echo "another matrix process holds $RUN_DIR/.matrix.lock" >&2
  exit 1
fi
if [[ ! -e "$SCHEDULE_FILE" ]]; then
  printf 'event\tcondition\trepetition\ttransport\tsignal_class\tlocation_id\tphysical_placement\tbin_start\tbin_end\ttimestamp\tstatus\n' > "$SCHEDULE_FILE"
fi

log() {
  printf '[%s] %s\n' "$(date -Iseconds)" "$*" | tee -a "$MATRIX_LOG"
}

next_boundary() {
  local lead_s="$1"
  local now target
  now="$(date +%s)"
  target=$((now + lead_s))
  printf '%s\n' "$((((target + 299) / 300) * 300))"
}

validate_completed_transport() {
  local condition="$1"
  local repetition="$2"
  local transport="$3"
  local bin_start_epoch="$4"
  local bin_end_epoch="$5"
  local condition_lower="${condition,,}"
  local transport_dir="$RUN_DIR/application/$condition_lower/rep_${repetition}/$transport"
  local expected_clients=1
  if [[ "$condition" == "C4" ]]; then
    expected_clients=100
  fi

  jq -e \
    --argjson expected_clients "$expected_clients" \
    --argjson expected_count "$COUNT" \
    '.files == $expected_clients and
     .complete_files == $expected_clients and
     .rows == ($expected_clients * $expected_count)' \
    "$transport_dir/validation_summary.json" >/dev/null

  local started finished started_epoch finished_epoch
  started="$(jq -er '.workload_started_at' "$transport_dir/run_manifest.json")"
  finished="$(jq -er '.workload_finished_at' "$transport_dir/run_manifest.json")"
  started_epoch="$(date --date "$started" +%s)"
  finished_epoch="$(date --date "$finished" +%s)"
  if (( started_epoch < bin_start_epoch || finished_epoch > bin_end_epoch )); then
    echo "workload timestamps escaped the declared ACP sample" >&2
    return 1
  fi
}

validate_existing_transport() {
  local condition="$1"
  local repetition="$2"
  local transport="$3"
  local condition_lower="${condition,,}"
  local transport_dir="$RUN_DIR/application/$condition_lower/rep_${repetition}/$transport"
  local manifest="$transport_dir/run_manifest.json"
  local validation="$transport_dir/validation_summary.json"
  [[ -f "$manifest" && -f "$validation" ]] || return 1

  jq -e \
    --arg condition "$condition" \
    --argjson repetition "$repetition" \
    --arg transport "$transport" \
    --arg signal_class "$SIGNAL_CLASS" \
    --arg location_id "$LOCATION_ID" \
    --arg physical_placement "$PHYSICAL_PLACEMENT" \
    --argjson count "$COUNT" \
    --argjson interval_ms "$INTERVAL_MS" \
    '.condition == $condition and
     .repetition == $repetition and
     .transport == $transport and
     .signal_class == $signal_class and
     .location_id == $location_id and
     .physical_placement == $physical_placement and
     .count_per_client == $count and
     .interval_ms == $interval_ms' \
    "$manifest" >/dev/null || return 1

  local bin_start bin_end bin_start_epoch bin_end_epoch
  bin_start="$(jq -er '.acp_bin_start' "$manifest")" || return 1
  bin_end="$(jq -er '.acp_bin_end' "$manifest")" || return 1
  bin_start_epoch="$(date --date "$bin_start" +%s)" || return 1
  bin_end_epoch="$(date --date "$bin_end" +%s)" || return 1
  (( bin_end_epoch - bin_start_epoch == 300 )) || return 1
  validate_completed_transport \
    "$condition" "$repetition" "$transport" "$bin_start_epoch" "$bin_end_epoch"
}

run_transport() {
  local condition="$1"
  local repetition="$2"
  local transport="$3"
  local lead_s="$SIMPLE_SETUP_LEAD_S"
  if [[ "$condition" == "C4" && "$transport" == "mqtt" ]]; then
    lead_s="$C4_MQTT_SETUP_LEAD_S"
  fi

  local bin_start_epoch bin_end_epoch bin_start bin_end
  bin_start_epoch="$(next_boundary "$lead_s")"
  bin_end_epoch=$((bin_start_epoch + 300))
  bin_start="$(date --date "@$bin_start_epoch" --iso-8601=seconds)"
  bin_end="$(date --date "@$bin_end_epoch" --iso-8601=seconds)"
  printf 'planned\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\tplanned\n' \
    "$condition" "$repetition" "$transport" "$SIGNAL_CLASS" "$LOCATION_ID" "$PHYSICAL_PLACEMENT" \
    "$bin_start" "$bin_end" "$(date -Iseconds)" >> "$SCHEDULE_FILE"
  log "Scheduled $condition repetition $repetition $transport for $bin_start to $bin_end"

  set +e
  RUN_NAME="$RUN_NAME" \
  COUNT="$COUNT" \
  INTERVAL_MS="$INTERVAL_MS" \
  SIGNAL_CLASS="$SIGNAL_CLASS" \
  LOCATION_ID="$LOCATION_ID" \
  PHYSICAL_PLACEMENT="$PHYSICAL_PLACEMENT" \
  ACP_BIN_START="$bin_start" \
  ACP_BIN_END="$bin_end" \
  ENABLE_GNSS="$ENABLE_GNSS" \
    bash "$RUNNER" "$condition" "$repetition" "$transport" \
      2>&1 | tee -a "$MATRIX_LOG"
  local runner_status="${PIPESTATUS[0]}"
  set -e

  if (( runner_status != 0 )); then
    printf 'completed\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\trc_%s\n' \
      "$condition" "$repetition" "$transport" "$SIGNAL_CLASS" "$LOCATION_ID" "$PHYSICAL_PLACEMENT" \
      "$bin_start" "$bin_end" "$(date -Iseconds)" "$runner_status" >> "$SCHEDULE_FILE"
    log "$condition repetition $repetition $transport failed with rc=$runner_status; stopping matrix"
    return "$runner_status"
  fi

  if ! validate_completed_transport "$condition" "$repetition" "$transport" "$bin_start_epoch" "$bin_end_epoch"; then
    printf 'completed\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\tvalidation_failed\n' \
      "$condition" "$repetition" "$transport" "$SIGNAL_CLASS" "$LOCATION_ID" "$PHYSICAL_PLACEMENT" \
      "$bin_start" "$bin_end" "$(date -Iseconds)" >> "$SCHEDULE_FILE"
    log "$condition repetition $repetition $transport failed post-run validation; stopping matrix"
    return 1
  fi

  printf 'completed\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\tpassed\n' \
    "$condition" "$repetition" "$transport" "$SIGNAL_CLASS" "$LOCATION_ID" "$PHYSICAL_PLACEMENT" \
    "$bin_start" "$bin_end" "$(date -Iseconds)" >> "$SCHEDULE_FILE"
  log "$condition repetition $repetition $transport passed immediate validation"
}

log "Starting matrix phase signal_class=$SIGNAL_CLASS location_id=$LOCATION_ID physical_placement=$PHYSICAL_PLACEMENT conditions=${CONDITIONS[*]} count=$COUNT interval_ms=$INTERVAL_MS"
for raw_condition in "${CONDITIONS[@]}"; do
  condition="${raw_condition^^}"
  for repetition in $(seq 1 "$REPETITIONS"); do
    if (( repetition % 2 == 1 )); then
      transports=(tcp mqtt udp)
    else
      transports=(udp mqtt tcp)
    fi
    for transport in "${transports[@]}"; do
      transport_dir="$RUN_DIR/application/${condition,,}/rep_${repetition}/$transport"
      if [[ "$RESUME_VALIDATED" == "1" && -e "$transport_dir" ]]; then
        if ! validate_existing_transport "$condition" "$repetition" "$transport"; then
          log "Existing $condition repetition $repetition $transport failed resume validation"
          exit 1
        fi
        printf 'resumed\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\tskipped_valid\n' \
          "$condition" "$repetition" "$transport" "$SIGNAL_CLASS" "$LOCATION_ID" "$PHYSICAL_PLACEMENT" \
          "$(jq -r '.acp_bin_start' "$transport_dir/run_manifest.json")" \
          "$(jq -r '.acp_bin_end' "$transport_dir/run_manifest.json")" \
          "$(date -Iseconds)" >> "$SCHEDULE_FILE"
        log "Skipping already validated $condition repetition $repetition $transport"
        continue
      fi
      run_transport "$condition" "$repetition" "$transport"
    done
  done
done
log "Matrix phase complete signal_class=$SIGNAL_CLASS location_id=$LOCATION_ID physical_placement=$PHYSICAL_PLACEMENT conditions=${CONDITIONS[*]}"
