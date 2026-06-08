# Environment Evidence

- Run directory: /home/hydrau/Documents/Github/IPI/results/real_5g/20260515_sunny_run_1
- Timestamp local: 2026-05-15T09:06:14-04:00
- pwd: /home/hydrau/Documents/Github/IPI
- hostname: hydrau
- uname: Linux hydrau 6.8.0-111-generic #111~22.04.1-Ubuntu SMP PREEMPT_DYNAMIC Tue Apr 14 17:13:45 UTC  x86_64 x86_64 x86_64 GNU/Linux
- Current date label: 20260515
- Weather label: sunny
- Run label: run_1

## Tools
- cmake: /usr/bin/cmake
- g++: /usr/bin/g++
- make: /usr/bin/make
- ssh: /usr/bin/ssh
- scp: /usr/bin/scp
- rsync: /usr/bin/rsync
- sshpass: MISSING
- ros2: /opt/ros/humble/bin/ros2
- python3: /usr/bin/python3

## Timestamp patch evidence
cpp/include/ipi/api/private_5g_latency_probe.hpp:69:void set_private_5g_probe_request_send_time(std::vector<std::uint8_t>& encodedRequest,
cpp/include/ipi/api/private_5g_latency_probe.hpp:77:void send_private_5g_probe_packet(int socketFd, const std::vector<std::uint8_t>& packet);
cpp/examples/library/private_5g_latency_sender.cpp:488:            ipi::api::set_private_5g_probe_request_send_time(encodedRequest, request.clientSendTimeNs);
cpp/examples/library/private_5g_latency_sender.cpp:489:            ipi::api::send_private_5g_probe_packet(socketFd, encodedRequest);
cpp/examples/library/private_5g_latency_sender.cpp:523:            ipi::api::set_private_5g_probe_request_send_time(encodedRequest, request.clientSendTimeNs);
cpp/examples/library/private_5g_latency_sender.cpp:524:            client.publish(make_request_topic(args), encodedRequest);
cpp/src/api/private_5g_latency_probe.cpp:281:void set_private_5g_probe_request_send_time(std::vector<std::uint8_t>& encodedRequest,
cpp/src/api/private_5g_latency_probe.cpp:351:void send_private_5g_probe_packet(int socketFd, const std::vector<std::uint8_t>& packet) {

## Listening ports before run
State  Recv-Q Send-Q Local Address:Port  Peer Address:PortProcess                                
LISTEN 0      4096       127.0.0.1:22352      0.0.0.0:*                                          
LISTEN 0      511        127.0.0.1:22350      0.0.0.0:*                                          
LISTEN 0      16         127.0.0.1:8080       0.0.0.0:*    users:(("davos_web_ui",pid=2974,fd=3))
LISTEN 0      128        127.0.0.1:631        0.0.0.0:*                                          
LISTEN 0      128          0.0.0.0:22         0.0.0.0:*                                          
LISTEN 0      4096   127.0.0.53%lo:53         0.0.0.0:*                                          
LISTEN 0      128             [::]:22            [::]:*                                          
LISTEN 0      128            [::1]:631           [::]:*                                          
LISTEN 0      511            [::1]:22350         [::]:*                                          
