cmake_minimum_required(VERSION 3.8)
project(ipi_msgs)

find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/ActivationZone.msg"
  "msg/ActivationEvent.msg"
  "msg/ServiceIntent.msg"
  "msg/LinkStatus.msg"
  "msg/PolicyDecision.msg"
  "msg/OffloadRequest.msg"
  "msg/OffloadDecision.msg"
  DEPENDENCIES builtin_interfaces
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
