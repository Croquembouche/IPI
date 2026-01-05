cmake_minimum_required(VERSION 3.16)
project(ipi_cpp LANGUAGES C CXX)

option(IPI_BUILD_EXAMPLES "Build example executables" ON)
option(IPI_ENABLE_TESTS "Enable building unit tests" OFF)
option(IPI_ENABLE_ROS2_BRIDGE "Enable ROS2 MQTT bridge helpers" OFF)
option(IPI_ENABLE_MOCAR_EXAMPLES "Build examples that depend on the Mocar SDK" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
    src/v2x/j2735_messages.cpp
    src/v2x/uper_codec.cpp
)

set(IPI_MESH_SOURCES
    src/mesh/mesh_manager.cpp
)

set(IPI_API_SOURCES
    src/api/receiver.cpp
    src/api/sender.cpp
    src/api/shared_state.cpp
)

add_library(ipi STATIC
    ${IPI_CORE_SOURCES}
    ${IPI_V2X_SOURCES}
    ${IPI_MESH_SOURCES}
    ${IPI_API_SOURCES}
)

target_include_directories(ipi
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ipi PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(ipi PRIVATE /W4 /permissive-)
else()
    target_compile_options(ipi PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(IPI_ENABLE_ROS2_BRIDGE)
    target_sources(ipi PRIVATE src/v2x/ros2_bridge.cpp)
    target_compile_definitions(ipi PUBLIC IPI_ENABLE_ROS2)
    find_package(ament_cmake QUIET)
    if(NOT ament_cmake_FOUND)
        message(WARNING "IPI_ENABLE_ROS2_BRIDGE is ON but no ROS2 environment detected. Generated headers from v2x_msg may be missing.")
    endif()
endif()

if(IPI_BUILD_EXAMPLES)
    add_executable(example_build_service_request examples/library/build_service_request.cpp)
    target_link_libraries(example_build_service_request PRIVATE ipi)

    add_executable(example_v2x_roundtrip examples/library/v2x_roundtrip.cpp)
    target_link_libraries(example_v2x_roundtrip PRIVATE ipi)

    add_executable(example_spat_tcp_sender examples/library/spat_tcp_sender.cpp)
    target_link_libraries(example_spat_tcp_sender PRIVATE ipi)

    add_executable(example_mesh_demo examples/library/mesh_demo.cpp)
    target_link_libraries(example_mesh_demo PRIVATE ipi)

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
    # Placeholder for future tests
endif()

install(TARGETS ipi EXPORT IpiTargets ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT IpiTargets FILE IpiTargets.cmake NAMESPACE IPI:: DESTINATION lib/cmake/ipi)
