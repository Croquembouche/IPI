cmake_minimum_required(VERSION 3.16)
project(ipi_cpp VERSION 0.2.0 LANGUAGES C CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(IPI_BUILD_EXAMPLES "Build example executables" ON)
option(IPI_ENABLE_TESTS "Enable building unit tests" OFF)
option(IPI_ENABLE_ROS2_BRIDGE "Enable ROS2 MQTT bridge helpers" OFF)
option(IPI_ENABLE_MOCAR_EXAMPLES "Build examples that depend on the Mocar SDK" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Threads REQUIRED)

# Mocar SDK layout (defaults to new_V2X_64bit drop). Override with
# -DMOCAR_SDK_ROOT=/path/to/new_V2X_64bit if staged elsewhere.
set(MOCAR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/mocar/new_V2X_64bit)
if(DEFINED MOCAR_SDK_ROOT)
    set(MOCAR_ROOT ${MOCAR_SDK_ROOT})
endif()
set(MOCAR_INCLUDE_DIR ${MOCAR_ROOT}/code/mde_64cv2x/location/include)
set(MOCAR_LIB_DIR ${MOCAR_ROOT}/aarch64_libs)

set(IPI_CORE_SOURCES
    src/core/ipi_service_request.cpp
    src/core/ipi_cooperative_service.cpp
    src/core/message_frame.cpp
    src/core/validation.cpp
)

set(IPI_V2X_SOURCES
    src/v2x/j2735_ipi_regional_codec.cpp
    src/v2x/j2735_messages.cpp
    src/v2x/uper_codec.cpp
)

set(IPI_MESH_SOURCES
    src/mesh/mesh_manager.cpp
    src/mesh/task_offloader.cpp
)

set(IPI_PROPOSAL_SOURCES
    src/activation/activation.cpp
    src/policy/service_policy.cpp
    src/offload/offload_decision.cpp
)

set(IPI_API_SOURCES
    src/api/detector_result.cpp
    src/api/edge4av_interface.cpp
    src/api/minimal_mqtt_client.cpp
    src/api/mqtt_session_transport.cpp
    src/api/pc5_ipi_adapter.cpp
    src/api/private_5g_latency_probe.cpp
    src/api/private_session_transport.cpp
    src/api/receiver.cpp
    src/api/session_lifecycle.cpp
    src/api/session_wire_codec.cpp
    src/api/sender.cpp
    src/api/shared_state.cpp
    src/api/types.cpp
)

add_library(ipi STATIC
    ${IPI_CORE_SOURCES}
    ${IPI_V2X_SOURCES}
    ${IPI_MESH_SOURCES}
    ${IPI_PROPOSAL_SOURCES}
    ${IPI_API_SOURCES}
)

target_include_directories(ipi
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ipi PUBLIC cxx_std_17)
target_link_libraries(ipi PUBLIC Threads::Threads)

if(MSVC)
    target_compile_options(ipi PRIVATE /W4 /permissive-)
else()
    target_compile_options(ipi PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(IPI_ENABLE_ROS2_BRIDGE)
    target_sources(ipi PRIVATE src/v2x/ros2_bridge.cpp)
    target_compile_definitions(ipi PUBLIC IPI_ENABLE_ROS2)
    find_package(ament_cmake REQUIRED)
    find_package(v2x_msg REQUIRED)
    ament_target_dependencies(ipi v2x_msg)
endif()

if(IPI_BUILD_EXAMPLES)
    add_executable(example_build_service_request examples/library/build_service_request.cpp)
    target_link_libraries(example_build_service_request PRIVATE ipi)

    add_executable(example_v2x_roundtrip examples/library/v2x_roundtrip.cpp)
    target_link_libraries(example_v2x_roundtrip PRIVATE ipi)

    add_executable(example_spat_tcp_sender examples/library/spat_tcp_sender.cpp)
    target_link_libraries(example_spat_tcp_sender PRIVATE ipi)

    add_executable(example_private_5g_latency_sender examples/library/private_5g_latency_sender.cpp)
    target_link_libraries(example_private_5g_latency_sender PRIVATE ipi)

    add_executable(example_private_5g_latency_receiver examples/library/private_5g_latency_receiver.cpp)
    target_link_libraries(example_private_5g_latency_receiver PRIVATE ipi Threads::Threads)

    add_executable(example_private_5g_latency_udp_sender examples/library/private_5g_latency_udp_sender.cpp)
    target_link_libraries(example_private_5g_latency_udp_sender PRIVATE ipi)

    add_executable(example_private_5g_latency_udp_receiver examples/library/private_5g_latency_udp_receiver.cpp)
    target_link_libraries(example_private_5g_latency_udp_receiver PRIVATE ipi)

    add_executable(example_mesh_demo examples/library/mesh_demo.cpp)
    target_link_libraries(example_mesh_demo PRIVATE ipi)

    add_executable(example_edge4av_dual_plane examples/library/edge4av_dual_plane.cpp)
    target_link_libraries(example_edge4av_dual_plane PRIVATE ipi)

    if(IPI_ENABLE_MOCAR_EXAMPLES)
        if(EXISTS ${MOCAR_LIB_DIR}/libmocarv2x.so)
            add_executable(example_mocar_ipi examples/device/mocar_ipi_demo.c)
            target_include_directories(example_mocar_ipi PRIVATE ${MOCAR_INCLUDE_DIR})
            target_link_libraries(example_mocar_ipi PRIVATE ${MOCAR_LIB_DIR}/libmocarv2x.so pthread m)
            set_target_properties(example_mocar_ipi PROPERTIES BUILD_RPATH "${MOCAR_LIB_DIR}" LINKER_LANGUAGE C)

            add_executable(example_spat_tcp_bridge examples/device/spat_tcp_bridge.cpp)
            target_include_directories(example_spat_tcp_bridge PRIVATE ${MOCAR_INCLUDE_DIR})
            target_link_libraries(example_spat_tcp_bridge PRIVATE ipi ${MOCAR_LIB_DIR}/libmocarv2x.so pthread m)
            set_target_properties(example_spat_tcp_bridge PROPERTIES BUILD_RPATH "${MOCAR_LIB_DIR}")
        else()
            message(WARNING "Mocar SDK library not found at ${MOCAR_LIB_DIR}/libmocarv2x.so; skipping example_mocar_ipi")
        endif()
    endif()

    if(IPI_ENABLE_ROS2_BRIDGE AND IPI_ENABLE_MOCAR_EXAMPLES)
        find_package(rclcpp QUIET)
        find_package(v2x_msg QUIET)
        if(rclcpp_FOUND AND v2x_msg_FOUND AND EXISTS ${MOCAR_LIB_DIR}/libmocarv2x.so)
            add_executable(example_ros2_bsm_broadcaster examples/device/ros2_bsm_broadcaster.cpp)
            target_include_directories(example_ros2_bsm_broadcaster PRIVATE ${MOCAR_INCLUDE_DIR})
            target_compile_definitions(example_ros2_bsm_broadcaster PRIVATE IPI_ENABLE_ROS2)
            target_link_libraries(example_ros2_bsm_broadcaster
                PRIVATE
                    ipi
                    rclcpp::rclcpp
                    v2x_msg::v2x_msg
                    ${MOCAR_LIB_DIR}/libmocarv2x.so
                    pthread
                    m)
            set_target_properties(example_ros2_bsm_broadcaster PROPERTIES BUILD_RPATH "${MOCAR_LIB_DIR}")
        else()
            message(WARNING "ROS2 or Mocar prerequisites missing; skipping example_ros2_bsm_broadcaster")
        endif()
    endif()
endif()

if(IPI_ENABLE_TESTS)
    enable_testing()
    find_package(Threads REQUIRED)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    add_executable(test_edge4av_interface tests/edge4av_interface_test.cpp)
    target_link_libraries(test_edge4av_interface PRIVATE ipi)
    add_test(NAME edge4av_interface COMMAND test_edge4av_interface)

    add_executable(test_private_session_transport tests/private_session_transport_test.cpp)
    target_link_libraries(test_private_session_transport PRIVATE ipi)
    add_test(NAME private_session_transport COMMAND test_private_session_transport)

    add_executable(test_private_5g_latency_probe tests/private_5g_latency_probe_test.cpp)
    target_link_libraries(test_private_5g_latency_probe PRIVATE ipi)
    add_test(NAME private_5g_latency_probe COMMAND test_private_5g_latency_probe)

    add_executable(test_j2735_message_flow tests/j2735_message_flow_test.cpp)
    target_link_libraries(test_j2735_message_flow PRIVATE ipi)
    add_test(NAME j2735_message_flow COMMAND test_j2735_message_flow)

    add_executable(test_j2735_ipi_regional_codec tests/j2735_ipi_regional_codec_test.cpp)
    target_link_libraries(test_j2735_ipi_regional_codec PRIVATE ipi)
    add_test(NAME j2735_ipi_regional_codec COMMAND test_j2735_ipi_regional_codec)

    add_test(
        NAME j2735_schema_prepare
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/j2735_schema_prepare_test.py
            --script ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/prepare_j2735_ipi_schema.py
            --schema ${CMAKE_CURRENT_SOURCE_DIR}/asn1/IPI.asn)

    add_executable(test_experiment_logging tests/experiment_logging_test.cpp)
    target_link_libraries(test_experiment_logging PRIVATE ipi)
    add_test(NAME experiment_logging COMMAND test_experiment_logging)

    add_executable(test_private_5g_latency_loopback tests/private_5g_latency_loopback_test.cpp)
    target_link_libraries(test_private_5g_latency_loopback PRIVATE ipi Threads::Threads)
    add_test(NAME private_5g_latency_loopback COMMAND test_private_5g_latency_loopback)

    add_executable(test_private_5g_latency_mqtt_loopback tests/private_5g_latency_mqtt_loopback_test.cpp)
    target_link_libraries(test_private_5g_latency_mqtt_loopback PRIVATE ipi Threads::Threads)
    add_test(NAME private_5g_latency_mqtt_loopback COMMAND test_private_5g_latency_mqtt_loopback)

    add_executable(test_session_lifecycle tests/session_lifecycle_test.cpp)
    target_link_libraries(test_session_lifecycle PRIVATE ipi)
    add_test(NAME session_lifecycle COMMAND test_session_lifecycle)

    add_executable(test_session_wire_codec tests/session_wire_codec_test.cpp)
    target_link_libraries(test_session_wire_codec PRIVATE ipi)
    add_test(NAME session_wire_codec COMMAND test_session_wire_codec)

    add_executable(test_broker_session_transport tests/broker_session_transport_test.cpp)
    target_link_libraries(test_broker_session_transport PRIVATE ipi Threads::Threads)
    add_test(NAME broker_session_transport COMMAND test_broker_session_transport)

    add_executable(test_pc5_ipi_adapter tests/pc5_ipi_adapter_test.cpp)
    target_link_libraries(test_pc5_ipi_adapter PRIVATE ipi)
    add_test(NAME pc5_ipi_adapter COMMAND test_pc5_ipi_adapter)

    add_executable(test_detector_result tests/detector_result_test.cpp)
    target_link_libraries(test_detector_result PRIVATE ipi)
    add_test(NAME detector_result COMMAND test_detector_result)

    add_executable(test_activation_zone tests/activation_zone_test.cpp)
    target_link_libraries(test_activation_zone PRIVATE ipi)
    add_test(NAME activation_zone COMMAND test_activation_zone)

    add_executable(test_service_policy tests/service_policy_test.cpp)
    target_link_libraries(test_service_policy PRIVATE ipi)
    add_test(NAME service_policy COMMAND test_service_policy)

    add_executable(test_offload_decision tests/offload_decision_test.cpp)
    target_link_libraries(test_offload_decision PRIVATE ipi)
    add_test(NAME offload_decision COMMAND test_offload_decision)

    add_executable(test_task_offloader tests/task_offloader_test.cpp)
    target_link_libraries(test_task_offloader PRIVATE ipi)
    add_test(NAME task_offloader COMMAND test_task_offloader)
endif()

configure_package_config_file(
    cmake/IpiConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/IPIConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ipi)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/IPIConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(TARGETS ipi EXPORT IpiTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES asn1/IPI.asn DESTINATION ${CMAKE_INSTALL_DATADIR}/ipi/asn1)
install(EXPORT IpiTargets
    FILE IpiTargets.cmake
    NAMESPACE IPI::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ipi)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/IPIConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/IPIConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ipi)
