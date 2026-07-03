# Aborted experiment 11 startup

This run was aborted during the first TCP condition before sender data was collected.

Cause: after adding threaded TCP receiver support, the edge build failed because `example_private_5g_latency_receiver` needed to link pthread/`Threads::Threads`. The failed receiver launch wrote:

`stdbuf: failed to run command './cpp/build/example_private_5g_latency_receiver': No such file or directory`

The CMake link issue was fixed after this partial startup. Use the next multiclient scalability run folder for the clean dataset.
