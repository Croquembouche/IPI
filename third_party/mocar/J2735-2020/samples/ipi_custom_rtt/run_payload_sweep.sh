#!/usr/bin/env bash
set -euo pipefail

BIN="./ipi_custom_rtt"
NODE_ID="v2x-tx-radio"
COUNT=1000
INTERVAL_MS=100
TIMEOUT_MS=1000
START_PAYLOAD_BYTES=128
MAX_PAYLOAD_BYTES=$((128 * 1024))
INCLUDE_ZERO=1
ASN_CHECK=0
OUT_DIR="mocar_radio_rtt_$(date +%Y%m%d_%H%M%S)"

usage() {
  cat <<'EOF'
Usage: ./run_payload_sweep.sh [options]

Run an initiator-side Mocar custom RTT payload sweep. Start the other Mocar
device first with:

  ./ipi_custom_rtt --role responder --node-id v2x-rx-radio

Options:
  --bin <path>                  RTT binary path (default: ./ipi_custom_rtt)
  --node-id <id>                initiator node ID (default: v2x-tx-radio)
  --count <n>                   probes per payload (default: 1000)
  --interval-ms <ms>            inter-probe interval (default: 100)
  --timeout-ms <ms>             per-probe timeout (default: 1000)
  --start-payload-bytes <n>     first nonzero payload (default: 128)
  --max-payload-bytes <n>       maximum payload (default: 131072)
  --out-dir <dir>               output directory
  --asn-check <0|1>             pass-through custom-send ASN check flag
  --include-zero                include a zero-byte payload condition (default)
  --no-zero                     skip the zero-byte payload condition
  -h, --help                    show this help

The nonzero payload sequence doubles each step: 128, 256, 512, ... up to
128 KiB by default.

Outputs include per-payload CSV files plus spreadsheet-friendly text files:
  mocar_radio_rtt_all_samples.txt
  mocar_radio_rtt_summary.txt
  mocar-rtt-payload-<bytes>_samples.txt
  mocar-rtt-payload-<bytes>_rtt_ms.txt
EOF
}

require_value() {
  local name="$1"
  local value="${2:-}"
  if [[ -z "$value" ]]; then
    echo "missing value for $name" >&2
    exit 2
  fi
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --bin)
      require_value "$1" "${2:-}"
      BIN="$2"
      shift 2
      ;;
    --node-id)
      require_value "$1" "${2:-}"
      NODE_ID="$2"
      shift 2
      ;;
    --count)
      require_value "$1" "${2:-}"
      COUNT="$2"
      shift 2
      ;;
    --interval-ms)
      require_value "$1" "${2:-}"
      INTERVAL_MS="$2"
      shift 2
      ;;
    --timeout-ms)
      require_value "$1" "${2:-}"
      TIMEOUT_MS="$2"
      shift 2
      ;;
    --start-payload-bytes)
      require_value "$1" "${2:-}"
      START_PAYLOAD_BYTES="$2"
      shift 2
      ;;
    --max-payload-bytes)
      require_value "$1" "${2:-}"
      MAX_PAYLOAD_BYTES="$2"
      shift 2
      ;;
    --out-dir)
      require_value "$1" "${2:-}"
      OUT_DIR="$2"
      shift 2
      ;;
    --asn-check)
      require_value "$1" "${2:-}"
      ASN_CHECK="$2"
      shift 2
      ;;
    --include-zero)
      INCLUDE_ZERO=1
      shift
      ;;
    --no-zero)
      INCLUDE_ZERO=0
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "unknown argument: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
done

for numeric in COUNT INTERVAL_MS TIMEOUT_MS START_PAYLOAD_BYTES MAX_PAYLOAD_BYTES INCLUDE_ZERO ASN_CHECK; do
  if ! [[ "${!numeric}" =~ ^[0-9]+$ ]]; then
    echo "$numeric must be a nonnegative integer" >&2
    exit 2
  fi
done

if [[ "$START_PAYLOAD_BYTES" -eq 0 ]]; then
  echo "--start-payload-bytes must be greater than zero; use --include-zero for the zero condition" >&2
  exit 2
fi

if [[ "$START_PAYLOAD_BYTES" -gt "$MAX_PAYLOAD_BYTES" ]]; then
  echo "--start-payload-bytes must be <= --max-payload-bytes" >&2
  exit 2
fi

if [[ "$MAX_PAYLOAD_BYTES" -gt $((128 * 1024)) ]]; then
  echo "--max-payload-bytes must be <= 131072 for ipi_custom_rtt" >&2
  exit 2
fi

if [[ "$ASN_CHECK" -ne 0 && "$ASN_CHECK" -ne 1 ]]; then
  echo "--asn-check must be 0 or 1" >&2
  exit 2
fi

if [[ ! -x "$BIN" ]]; then
  echo "RTT binary is not executable: $BIN" >&2
  echo "Build it first with: make clean && make" >&2
  exit 2
fi

payloads=()
if [[ "$INCLUDE_ZERO" -eq 1 ]]; then
  payloads+=(0)
fi

payload="$START_PAYLOAD_BYTES"
while [[ "$payload" -le "$MAX_PAYLOAD_BYTES" ]]; do
  payloads+=("$payload")
  payload=$((payload * 2))
done

mkdir -p "$OUT_DIR/commands"
COMMAND_LOG="$OUT_DIR/commands/mocar_radio_rtt_payload_sweep_commands.txt"
METADATA="$OUT_DIR/mocar_radio_rtt_payload_sweep_metadata.md"
ALL_SAMPLES_TXT="$OUT_DIR/mocar_radio_rtt_all_samples.txt"
SUMMARY_TXT="$OUT_DIR/mocar_radio_rtt_summary.txt"
: > "$COMMAND_LOG"

{
  echo "# Mocar Radio RTT Payload Sweep"
  echo
  echo "- Start time: $(date --iso-8601=seconds)"
  echo "- Initiator node ID: $NODE_ID"
  echo "- Payload bytes: ${payloads[*]}"
  echo "- Count per payload: $COUNT"
  echo "- Interval ms: $INTERVAL_MS"
  echo "- Timeout ms: $TIMEOUT_MS"
  echo "- ASN check: $ASN_CHECK"
  echo "- Binary: $BIN"
  echo "- RTT clock: initiator monotonic clock"
  echo "- Text sample output: $ALL_SAMPLES_TXT"
  echo "- Text summary output: $SUMMARY_TXT"
} > "$METADATA"

printf 'payload_bytes\tsequence\tpacket_bytes\tsuccess\trtt_ms\tdetail\n' > "$ALL_SAMPLES_TXT"
printf 'payload_bytes\tattempts\tsuccesses\tfailures\tavg_rtt_ms\tmin_rtt_ms\tmax_rtt_ms\n' > "$SUMMARY_TXT"

write_text_outputs() {
  local payload="$1"
  local condition="$2"
  local csv="$3"
  local samples_txt="$OUT_DIR/${condition}_samples.txt"
  local rtt_txt="$OUT_DIR/${condition}_rtt_ms.txt"

  awk -F, '
    BEGIN {
      OFS = "\t";
      print "sequence", "payload_bytes", "packet_bytes", "success", "rtt_ms", "detail";
    }
    NR > 1 {
      print $1, $2, $3, $4, $5, $11;
    }
  ' "$csv" > "$samples_txt"

  awk -F, '
    NR > 1 && $4 == "true" {
      print $5;
    }
  ' "$csv" > "$rtt_txt"

  awk -F, -v payload="$payload" '
    BEGIN {
      OFS = "\t";
    }
    NR > 1 {
      attempts += 1;
      if ($4 == "true") {
        successes += 1;
        rtt = $5 + 0.0;
        sum += rtt;
        if (successes == 1 || rtt < min_rtt) {
          min_rtt = rtt;
        }
        if (successes == 1 || rtt > max_rtt) {
          max_rtt = rtt;
        }
      }
    }
    END {
      failures = attempts - successes;
      if (successes > 0) {
        printf "%s\t%d\t%d\t%d\t%.6f\t%.6f\t%.6f\n",
          payload, attempts, successes, failures, sum / successes, min_rtt, max_rtt;
      } else {
        printf "%s\t%d\t%d\t%d\t\t\t\n", payload, attempts, successes, failures;
      }
    }
  ' "$csv" >> "$SUMMARY_TXT"

  awk -F, '
    BEGIN {
      OFS = "\t";
    }
    NR > 1 {
      print $2, $1, $3, $4, $5, $11;
    }
  ' "$csv" >> "$ALL_SAMPLES_TXT"
}

for payload in "${payloads[@]}"; do
  condition="mocar-rtt-payload-$payload"
  out="$OUT_DIR/${condition}.csv"
  err="$OUT_DIR/${condition}.err"
  cmd=(
    "$BIN"
    --role initiator
    --node-id "$NODE_ID"
    --count "$COUNT"
    --interval-ms "$INTERVAL_MS"
    --timeout-ms "$TIMEOUT_MS"
    --payload-bytes "$payload"
    --asn-check "$ASN_CHECK"
    --csv
  )

  echo "$(date --iso-8601=seconds) starting Mocar RTT payload $payload"
  printf 'local: ' >> "$COMMAND_LOG"
  printf '%q ' "${cmd[@]}" >> "$COMMAND_LOG"
  printf '> %q 2> %q\n' "$out" "$err" >> "$COMMAND_LOG"
  "${cmd[@]}" > "$out" 2> "$err"
  write_text_outputs "$payload" "$condition" "$out"
done

{
  echo
  echo "- End time: $(date --iso-8601=seconds)"
} >> "$METADATA"
