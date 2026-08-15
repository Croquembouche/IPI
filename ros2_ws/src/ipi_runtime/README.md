# IPI ROS 2 runtime

This package is the proposal-facing ROS 2 adapter for the transport-neutral
IPI C++ activation, policy, and offload modules.

It deliberately publishes decisions instead of commanding Autoware motion or
operation mode. An approved vehicle-side adapter must consume those decisions,
retain local safety authority, and reject unsafe or stale requests.

Topics:

- `/ipi/infrastructure/activation_zones` (`ipi_msgs/ActivationZone`), reliable
  and transient-local.
- `/ipi/vehicle/fix` (`sensor_msgs/NavSatFix`).
- `/ipi/vehicle/activation_event` (`ipi_msgs/ActivationEvent`), emitted once
  for each state transition.
- `/ipi/vehicle/link_status` (`ipi_msgs/LinkStatus`).
- `/ipi/vehicle/service_intent` and `/ipi/vehicle/policy_decision`.
- `/ipi/vehicle/offload_request` and `/ipi/vehicle/offload_decision`.

Install the C++ library before building the workspace:

```bash
cmake -S cpp -B cpp/build \
  -DIPI_ENABLE_TESTS=ON \
  -DCMAKE_INSTALL_PREFIX="$PWD/cpp/install"
cmake --build cpp/build --parallel
cmake --install cpp/build
source /opt/ros/humble/setup.bash
PATH=/opt/ros/humble/bin:/usr/bin:/bin \
colcon --log-base ros2_ws/log build \
  --base-paths ros2_ws/src/ipi_msgs ros2_ws/src/ipi_runtime \
  --build-base ros2_ws/build \
  --install-base ros2_ws/install \
  --cmake-args \
    -DCMAKE_PREFIX_PATH="$PWD/cpp/install" \
    -DPython3_EXECUTABLE=/usr/bin/python3
```

Run the launch contract test, including clean-exit assertions for every process:

```bash
source ros2_ws/install/setup.bash
colcon --log-base ros2_ws/log test \
  --build-base ros2_ws/build \
  --install-base ros2_ws/install \
  --packages-select ipi_runtime \
  --event-handlers console_direct+
colcon test-result --test-result-base ros2_ws/build --verbose
```

The reference launch starts the configured-zone publisher and the three runtime
decision nodes:

```bash
source ros2_ws/install/setup.bash
ros2 launch ipi_runtime reference_intersection.launch.py
```

The package currently uses `LicenseRef-IPI-Research-Only` as a placeholder.
Select and add the approved repository license before an external software
release.
