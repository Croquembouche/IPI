# Experiment 11 Multiclient Scalability

- run_id: edge4av-real-20260706-multiclient-scalability-weak-signal-run-1
- run_name: 20260706_multiclient_scalability_weak_signal_run_1
- edge_host: 10.100.100.6
- edge_repo: /home/d1/Documents/Github/IPI
- payload_bytes: 1024
- count_per_client: 1000
- interval_ms: 200
- client_levels: 1 2 5 10 20 50 100
- transports: tcp udp mqtt
- vehicle_state: stationary
- location_class: weak-signal
- gnss: NovAtel ROS 2 recorder
- mqtt_mode: one MQTT receiver process per source ID behind one broker
- udp_mode: application-level fragmentation enabled with max datagram 1400 bytes
- collection_started: 2026-07-06T13:04:27-04:00
