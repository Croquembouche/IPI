#!/usr/bin/env bash
set -euo pipefail

OBU_HOST="${OBU_HOST:-192.168.253.10}"
OBU_USER="${OBU_USER:-root}"
OBU_PASSWORD="${OBU_PASSWORD:-root}"
OBU_NM_PROFILE="${OBU_NM_PROFILE:-OBU}"
OBU_NM_DEVICE="${OBU_NM_DEVICE:-eno1}"

if command -v nmcli >/dev/null 2>&1; then
  active_profile="$(nmcli -t -f NAME,DEVICE connection show --active 2>/dev/null | grep -F ":${OBU_NM_DEVICE}" | head -n 1 || true)"
  if [ "$active_profile" != "${OBU_NM_PROFILE}:${OBU_NM_DEVICE}" ]; then
    nmcli connection up "$OBU_NM_PROFILE" ifname "$OBU_NM_DEVICE" >/dev/null 2>&1 || true
  fi
fi

SSH_OPTS=(
  -o StrictHostKeyChecking=no
  -o UserKnownHostsFile=/dev/null
  -o ConnectTimeout="${OBU_CONNECT_TIMEOUT:-10}"
  -o ServerAliveInterval="${OBU_SERVER_ALIVE_INTERVAL:-15}"
  -o ServerAliveCountMax="${OBU_SERVER_ALIVE_COUNT_MAX:-3}"
  -o TCPKeepAlive=yes
  -o HostKeyAlgorithms=+ssh-rsa
  -o PubkeyAcceptedAlgorithms=+ssh-rsa
)

if ! command -v sshpass >/dev/null 2>&1; then
  echo "sshpass is required. Install it or connect manually with:" >&2
  echo "  ssh ${SSH_OPTS[*]} ${OBU_USER}@${OBU_HOST}" >&2
  exit 1
fi

if (( $# > 0 )); then
  exec sshpass -p "$OBU_PASSWORD" ssh "${SSH_OPTS[@]}" "${OBU_USER}@${OBU_HOST}" "$@"
fi

exec sshpass -p "$OBU_PASSWORD" ssh "${SSH_OPTS[@]}" "${OBU_USER}@${OBU_HOST}"
