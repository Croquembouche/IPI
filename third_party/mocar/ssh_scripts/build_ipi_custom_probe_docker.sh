#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
SAMPLE_DIR="third_party/mocar/J2735-2020/samples/ipi_custom_probe"
BIN="$REPO_ROOT/$SAMPLE_DIR/ipi_custom_probe"
IMAGE="${MOCAR_PROBE_BUILD_IMAGE:-ubuntu:18.04}"
MAX_GLIBC_MINOR="${MAX_GLIBC_MINOR:-27}"

if ! command -v docker >/dev/null 2>&1; then
  echo "docker is required for the isolated OBU-compatible build" >&2
  exit 1
fi

uid="$(id -u)"
gid="$(id -g)"

docker run --rm \
  -e DEBIAN_FRONTEND=noninteractive \
  -v "$REPO_ROOT:/work" \
  -w "/work/$SAMPLE_DIR" \
  "$IMAGE" \
  bash -lc "apt-get update >/tmp/apt-update.log && \
    apt-get install -y --no-install-recommends \
      make gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu libc6-dev-arm64-cross \
      >/tmp/apt-install.log && \
    make clean && make && \
    chown $uid:$gid ipi_custom_probe ipi_custom_probe.o"

if command -v readelf >/dev/null 2>&1; then
  bad_versions="$(
    {
      readelf --version-info "$BIN" 2>/dev/null |
        grep -o 'GLIBC_[0-9][0-9]*\.[0-9][0-9]*' || true
    } |
      sort -Vu |
      awk -v max_minor="$MAX_GLIBC_MINOR" -F'[_.]' \
        '($2 > 2) || ($2 == 2 && $3 > max_minor) { print $0 }'
  )"
  if [[ -n "$bad_versions" ]]; then
    echo "rebuilt binary is not compatible with glibc 2.$MAX_GLIBC_MINOR:" >&2
    echo "$bad_versions" >&2
    exit 1
  fi
fi

echo "built $BIN"
