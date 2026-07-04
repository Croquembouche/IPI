#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
SDK_ROOT="$REPO_ROOT/third_party/mocar/J2735-2020"
REMOTE_ROOT="${REMOTE_ROOT:-/root/edge4av_exp}"
RTT_BIN="$SDK_ROOT/samples/ipi_custom_rtt/ipi_custom_rtt"
RTT_SWEEP="$SDK_ROOT/samples/ipi_custom_rtt/run_payload_sweep.sh"
MAX_GLIBC_MINOR="${MAX_GLIBC_MINOR:-27}"

usage() {
  cat <<'EOF'
Usage: setup_mocar_v2x_test.sh <obu|rsu|both>

Creates the Edge4AV Mocar experiment directory layout, deploys compatible
Mocar V2X binaries, and runs short startup/send-path smoke tests.

Environment:
  OBU_HOST, OBU_USER, OBU_PASSWORD       defaults used by connectOBU_10.sh
  JUMP_PASSWORD                         required for RSU path
  RSU_PASSWORD                          required for RSU path
  REMOTE_ROOT                           default: /root/edge4av_exp
  SMOKE_SECONDS                         default: 5
  MAX_GLIBC_MINOR                       default: 27

Remote layout:
  /root/edge4av_exp/test
  /root/edge4av_exp/exp_01
  /root/edge4av_exp/exp_02
  /root/edge4av_exp/exp_03
  /root/edge4av_exp/bin/custom_sample
  /root/edge4av_exp/bin/ipi_custom_rtt
  /root/edge4av_exp/bin/run_payload_sweep.sh
  /root/edge4av_exp/lib/{libmocarcv2x.so,libzlog.so}
EOF
}

require_file() {
  local path="$1"
  if [[ ! -f "$path" ]]; then
    echo "missing required file: $path" >&2
    exit 1
  fi
}

verify_glibc_compat() {
  local path="$1"
  if ! command -v readelf >/dev/null 2>&1; then
    echo "warning: readelf not found; skipping glibc compatibility check for $path" >&2
    return
  fi

  local bad_versions
  bad_versions="$(
    {
      readelf --version-info "$path" 2>/dev/null |
        grep -o 'GLIBC_[0-9][0-9]*\.[0-9][0-9]*' || true
    } |
      sort -Vu |
      awk -v max_minor="$MAX_GLIBC_MINOR" -F'[_.]' \
        '($2 > 2) || ($2 == 2 && $3 > max_minor) { print $0 }'
  )"
  if [[ -n "$bad_versions" ]]; then
    cat >&2 <<EOF
$path requires glibc newer than 2.$MAX_GLIBC_MINOR:
$bad_versions

Rebuild it with:
  $SCRIPT_DIR/build_ipi_custom_rtt_docker.sh
EOF
    exit 1
  fi
}

make_payload() {
  local tmpdir
  tmpdir="$(mktemp -d)"
  mkdir -p "$tmpdir/bin" "$tmpdir/lib"
  cp "$SDK_ROOT/samples/custom/custom_sample" "$tmpdir/bin/custom_sample"
  cp "$RTT_BIN" "$tmpdir/bin/ipi_custom_rtt"
  cp "$RTT_SWEEP" "$tmpdir/bin/run_payload_sweep.sh"
  cp "$SDK_ROOT/lib/libmocarcv2x.so" "$tmpdir/lib/libmocarcv2x.so"
  cp "$SDK_ROOT/lib/libzlog.so" "$tmpdir/lib/libzlog.so"
  tar -C "$tmpdir" -czf - .
  rm -rf "$tmpdir"
}

remote_setup_cmd() {
  cat <<EOF
set -eu
mkdir -p '$REMOTE_ROOT/test' '$REMOTE_ROOT/exp_01' '$REMOTE_ROOT/exp_02' '$REMOTE_ROOT/exp_03' '$REMOTE_ROOT/bin' '$REMOTE_ROOT/lib'
tar -C '$REMOTE_ROOT' -xzf -
chmod +x '$REMOTE_ROOT/bin/custom_sample' '$REMOTE_ROOT/bin/ipi_custom_rtt' '$REMOTE_ROOT/bin/run_payload_sweep.sh'
EOF
}

remote_smoke_cmd() {
  local label="$1"
  local seconds="${SMOKE_SECONDS:-5}"
  cat <<EOF
set +e
LD_LIBRARY_PATH='$REMOTE_ROOT/lib' timeout '$seconds' '$REMOTE_ROOT/bin/custom_sample' > '$REMOTE_ROOT/test/${label}_custom_sample_smoke.stdout' 2> '$REMOTE_ROOT/test/${label}_custom_sample_smoke.stderr'
rc=\$?
echo "\$rc" > '$REMOTE_ROOT/test/${label}_custom_sample_smoke.status'
echo '${label}_custom_sample_rc='"\$rc"
wc -l '$REMOTE_ROOT/test/${label}_custom_sample_smoke.stdout' '$REMOTE_ROOT/test/${label}_custom_sample_smoke.stderr'
LD_LIBRARY_PATH='$REMOTE_ROOT/lib' '$REMOTE_ROOT/bin/ipi_custom_rtt' --help > '$REMOTE_ROOT/test/${label}_ipi_custom_rtt_help.stdout' 2> '$REMOTE_ROOT/test/${label}_ipi_custom_rtt_help.stderr'
rtt_rc=\$?
echo "\$rtt_rc" > '$REMOTE_ROOT/test/${label}_ipi_custom_rtt_help.status'
echo '${label}_ipi_custom_rtt_help_rc='"\$rtt_rc"
wc -l '$REMOTE_ROOT/test/${label}_ipi_custom_rtt_help.stdout' '$REMOTE_ROOT/test/${label}_ipi_custom_rtt_help.stderr'
EOF
}

setup_obu() {
  echo "Setting up OBU..."
  make_payload | "$SCRIPT_DIR/connectOBU_10.sh" "$(remote_setup_cmd)"
  "$SCRIPT_DIR/connectOBU_10.sh" "$(remote_smoke_cmd obu)"
}

setup_rsu() {
  echo "Setting up RSU..."
  : "${JUMP_PASSWORD:?Set JUMP_PASSWORD for RSU setup}"
  : "${RSU_PASSWORD:?Set RSU_PASSWORD for RSU setup}"
  make_payload | "$SCRIPT_DIR/connectRSU_40.sh" "$(remote_setup_cmd)"
  "$SCRIPT_DIR/connectRSU_40.sh" "$(remote_smoke_cmd rsu)"
}

main() {
  require_file "$SDK_ROOT/samples/custom/custom_sample"
  require_file "$RTT_BIN"
  require_file "$RTT_SWEEP"
  require_file "$SDK_ROOT/lib/libmocarcv2x.so"
  require_file "$SDK_ROOT/lib/libzlog.so"
  verify_glibc_compat "$RTT_BIN"

  case "${1:-}" in
    obu)
      setup_obu
      ;;
    rsu)
      setup_rsu
      ;;
    both)
      setup_obu
      setup_rsu
      ;;
    -h|--help)
      usage
      ;;
    *)
      usage >&2
      exit 2
      ;;
  esac
}

main "$@"
