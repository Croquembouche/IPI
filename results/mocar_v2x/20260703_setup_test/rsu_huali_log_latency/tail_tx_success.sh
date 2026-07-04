#!/usr/bin/env bash
set -euo pipefail

log_file="/huali/log/cv2x_stack.log"
out_file="latency.txt"

tail -F "$log_file" | awk '
  BEGIN { count = 0; ts = "" }
  /\[cv2x_tx:[0-9]+\] Secure send message\[PSID:32\] start/ {
    if (match($0, /^\[([0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\.[0-9]{3})\]/, m)) {
      ts = m[1]
    }
  }
  /Tx success/ {
    if (ts != "") {
      count++
      printf "%d %s\n", count, ts >> "'"$out_file"'"
      fflush("'"$out_file"'")
    }
  }
'
