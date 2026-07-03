# Experiment 12 Failure/Fallback

- run_id: edge4av-real-20260703-failure-fallback-run-1
- run_name: 20260703_failure_fallback_run_1
- edge_host: 10.100.100.6
- edge_repo: /home/d1/Documents/Github/IPI
- payload_bytes: 1024
- count_per_condition: 1000
- interval_ms: 200
- timeout_ms: 1000
- trigger_after_sec: 60
- outage_sec: 10
- transports: tcp udp mqtt
- vehicle_state: stationary
- gnss: NovAtel ROS 2 recorder
- failure_modes: tcp receiver restart, udp receiver restart, mqtt receiver restart, mqtt broker restart
- collection_started: 2026-07-03T10:10:55-04:00
