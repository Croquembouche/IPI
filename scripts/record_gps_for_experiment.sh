#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: scripts/record_gps_for_experiment.sh <run-results-dir> [duration-sec]

Starts the NovAtel OEM7 ROS 2 network driver, records GPS topics to a ROS bag,
and writes CSV samples containing latest GPS position and speed.

Default receiver:
  ros2 launch novatel_oem7_driver oem7_net.launch.py oem7_ip_addr:=192.168.3.100

Outputs under <run-results-dir>/gps/:
  gps_samples.csv
  gps_latest.csv
  rosbag/
  novatel_driver.log
  rosbag_record.log
  gps_topic_recorder.log
  gps_metadata.md

If duration-sec is omitted, the script runs until it receives SIGINT or SIGTERM.
EOF
}

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ] || [ "$#" -lt 1 ]; then
  usage
  exit 0
fi

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUN_DIR="$(readlink -f "$1")"
DURATION_SEC="${2:-}"
GPS_DIR="$RUN_DIR/gps"
ROS_WS_SETUP="$REPO_ROOT/ros2_ws/install/setup.bash"
ROS_DISTRO_SETUP="/opt/ros/${ROS_DISTRO:-humble}/setup.bash"
OEM7_IP_ADDR="${OEM7_IP_ADDR:-192.168.3.100}"
OEM7_LAUNCH_PACKAGE="${OEM7_LAUNCH_PACKAGE:-novatel_oem7_driver}"
OEM7_LAUNCH_FILE="${OEM7_LAUNCH_FILE:-oem7_net.launch.py}"

TOPICS=(
  /novatel/oem7/bestpos
  /novatel/oem7/bestvel
  /novatel/oem7/inspva
  /novatel/oem7/fix
  /novatel/oem7/gps
  /novatel/oem7/odom
)

mkdir -p "$GPS_DIR"

set +u
if [ -f "$ROS_DISTRO_SETUP" ]; then
  # shellcheck disable=SC1090
  source "$ROS_DISTRO_SETUP"
fi

if [ -f "$ROS_WS_SETUP" ]; then
  # shellcheck disable=SC1090
  source "$ROS_WS_SETUP"
fi
set -u

if ! command -v ros2 >/dev/null 2>&1; then
  echo "ros2 command not found after sourcing ROS setup files" >&2
  exit 1
fi

DRIVER_PID=""
BAG_PID=""
CSV_PID=""

cleanup() {
  set +e
  if [ -n "$CSV_PID" ]; then kill "$CSV_PID" 2>/dev/null; fi
  if [ -n "$BAG_PID" ]; then kill "$BAG_PID" 2>/dev/null; fi
  if [ -n "$DRIVER_PID" ]; then kill "$DRIVER_PID" 2>/dev/null; fi
  wait "$CSV_PID" "$BAG_PID" "$DRIVER_PID" 2>/dev/null
}
trap cleanup EXIT INT TERM

{
  printf '# GPS Recording Metadata\n\n'
  printf -- '- Start time: `%s`\n' "$(date --iso-8601=seconds)"
  printf -- '- Hostname: `%s`\n' "$(hostname)"
  printf -- '- Run directory: `%s`\n' "$RUN_DIR"
  printf -- '- Driver command: `ros2 launch %s %s oem7_ip_addr:=%s`\n' "$OEM7_LAUNCH_PACKAGE" "$OEM7_LAUNCH_FILE" "$OEM7_IP_ADDR"
  printf -- '- Recorded topics: `%s`\n' "${TOPICS[*]}"
  if [ -n "$DURATION_SEC" ]; then
    printf -- '- Requested duration: `%s` seconds\n' "$DURATION_SEC"
  else
    printf -- '- Requested duration: until stopped by parent experiment script or operator\n'
  fi
} > "$GPS_DIR/gps_metadata.md"

ros2 launch "$OEM7_LAUNCH_PACKAGE" "$OEM7_LAUNCH_FILE" "oem7_ip_addr:=$OEM7_IP_ADDR" \
  > "$GPS_DIR/novatel_driver.log" 2>&1 &
DRIVER_PID=$!
echo "$DRIVER_PID" > "$GPS_DIR/novatel_driver.pid"

for _ in $(seq 1 20); do
  if ros2 topic list 2>/dev/null | grep -Eq '^/novatel/oem7/(bestpos|fix|inspva)$'; then
    break
  fi
  sleep 1
done

ros2 bag record -o "$GPS_DIR/rosbag" "${TOPICS[@]}" \
  > "$GPS_DIR/rosbag_record.log" 2>&1 &
BAG_PID=$!
echo "$BAG_PID" > "$GPS_DIR/rosbag_record.pid"

python3 "$REPO_ROOT/scripts/gps_topic_recorder.py" \
  --output "$GPS_DIR/gps_samples.csv" \
  --snapshot "$GPS_DIR/gps_latest.csv" \
  > "$GPS_DIR/gps_topic_recorder.log" 2>&1 &
CSV_PID=$!
echo "$CSV_PID" > "$GPS_DIR/gps_topic_recorder.pid"

if [ -n "$DURATION_SEC" ]; then
  sleep "$DURATION_SEC"
else
  while true; do
    sleep 3600
  done
fi
