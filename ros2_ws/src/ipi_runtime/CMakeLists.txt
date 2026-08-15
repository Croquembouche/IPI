cmake_minimum_required(VERSION 3.16)
project(ipi_runtime)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(ipi_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(IPI CONFIG REQUIRED)

add_executable(activation_manager_node src/activation_manager_node.cpp)
target_link_libraries(activation_manager_node IPI::ipi)
ament_target_dependencies(activation_manager_node ipi_msgs rclcpp sensor_msgs std_msgs)

add_executable(tiered_service_manager_node src/tiered_service_manager_node.cpp)
target_link_libraries(tiered_service_manager_node IPI::ipi)
ament_target_dependencies(tiered_service_manager_node ipi_msgs rclcpp)

add_executable(offload_decision_node src/offload_decision_node.cpp)
target_link_libraries(offload_decision_node IPI::ipi)
ament_target_dependencies(offload_decision_node ipi_msgs rclcpp)

add_executable(activation_zone_publisher_node src/activation_zone_publisher_node.cpp)
ament_target_dependencies(activation_zone_publisher_node ipi_msgs rclcpp)

install(TARGETS
  activation_manager_node
  tiered_service_manager_node
  offload_decision_node
  activation_zone_publisher_node
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY config launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(launch_testing_ament_cmake REQUIRED)
  add_launch_test(test/runtime_contract_test.py TIMEOUT 45)
endif()

ament_package()
