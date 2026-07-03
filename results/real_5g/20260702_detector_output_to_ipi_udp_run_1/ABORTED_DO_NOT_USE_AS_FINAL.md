# Aborted diagnostic UDP run

This run was interrupted during the experiment 09 UDP collection.

- Payload 0 completed and reached the UDP receiver.
- Payload 4096 produced repeated sender-side `udp ack timeout` rows while the edge receiver was still listening and its receiver CSV had only the header.
- Payloads 19648 and 22816 were created only as empty/partial files while stopping the script.

Use this folder only as diagnostic evidence that raw UDP detector-size datagrams were not reaching the application on this private 5G path. The clean UDP collection is in the next `detector_output_to_ipi_udp` run folder.
