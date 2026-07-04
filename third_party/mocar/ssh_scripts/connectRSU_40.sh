#!/usr/bin/env bash
set -euo pipefail

JUMP_HOST="${JUMP_HOST:-128.4.178.240}"
JUMP_USER="${JUMP_USER:-d1}"
RSU_HOST="${RSU_HOST:-192.168.253.40}"
RSU_USER="${RSU_USER:-root}"

: "${JUMP_PASSWORD:?Set JUMP_PASSWORD for ${JUMP_USER}@${JUMP_HOST}}"
: "${RSU_PASSWORD:?Set RSU_PASSWORD for ${RSU_USER}@${RSU_HOST}}"

OUTER_SSH_OPTS=(
  -o StrictHostKeyChecking=no
  -o UserKnownHostsFile=/dev/null
  -o ConnectTimeout="${RSU_CONNECT_TIMEOUT:-10}"
  -o ServerAliveInterval="${RSU_SERVER_ALIVE_INTERVAL:-15}"
  -o ServerAliveCountMax="${RSU_SERVER_ALIVE_COUNT_MAX:-3}"
  -o TCPKeepAlive=yes
  -o HostKeyAlgorithms=+ssh-rsa
  -o PubkeyAcceptedAlgorithms=+ssh-rsa
)

INNER_SSH_OPTS=(
  -o StrictHostKeyChecking=no
  -o UserKnownHostsFile=/dev/null
  -o ConnectTimeout="${RSU_CONNECT_TIMEOUT:-10}"
  -o ServerAliveInterval="${RSU_SERVER_ALIVE_INTERVAL:-15}"
  -o ServerAliveCountMax="${RSU_SERVER_ALIVE_COUNT_MAX:-3}"
  -o TCPKeepAlive=yes
  -o HostKeyAlgorithms=+ssh-rsa
  -o PubkeyAcceptedKeyTypes=+ssh-rsa
)

if ! command -v sshpass >/dev/null 2>&1; then
  echo "sshpass is required on the local machine." >&2
  exit 1
fi

remote_command=()
outer_tty=()
inner_tty=()
if (( $# > 0 )); then
  quoted=''
  printf -v quoted '%q ' "$@"
  remote_command=( "$quoted" )
elif [[ -t 0 ]]; then
  outer_tty=( -tt )
  inner_tty=( -tt )
fi

inner_cmd=(
  sshpass -p "$RSU_PASSWORD"
  ssh "${inner_tty[@]}" "${INNER_SSH_OPTS[@]}"
  "${RSU_USER}@${RSU_HOST}"
)

exec sshpass -p "$JUMP_PASSWORD" ssh "${outer_tty[@]}" "${OUTER_SSH_OPTS[@]}" "${JUMP_USER}@${JUMP_HOST}" \
  "$(printf '%q ' "${inner_cmd[@]}")${remote_command[*]:-}"
