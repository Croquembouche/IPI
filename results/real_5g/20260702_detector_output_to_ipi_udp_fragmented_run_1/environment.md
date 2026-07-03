# Experiment 09 fragmented UDP collection

- run_id: edge4av-real-20260702-detector-output-to-ipi-udp-fragmented-run-1
- run_name: 20260702_detector_output_to_ipi_udp_fragmented_run_1
- edge_host: 10.100.100.6
- edge_repo: /home/d1/Documents/Github/IPI
- udp_port: 36667
- udp_max_datagram_bytes: 1400
- collection_started: 2026-07-02T11:15:45-04:00
- vehicle_state: stationary
- location_class: current/good-signal location; weak-signal repeat is separate
- note: UDP detector outputs are application-fragmented and reassembled to avoid IP fragmentation over the private 5G path
