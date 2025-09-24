#pragma once

#include "ipi/v2x/j2735_messages.hpp"

#ifdef IPI_ENABLE_ROS2
#include <v2x_msg/msg/bsm.hpp>
#include <v2x_msg/msg/map.hpp>
#include <v2x_msg/msg/spat.hpp>
#include <v2x_msg/msg/srm.hpp>
#include <v2x_msg/msg/ssm.hpp>
#endif

namespace ipi::v2x {

#ifdef IPI_ENABLE_ROS2

/**
 * Utility conversions between the lightweight J2735 helper structs and the ROS2
 * message types published in the `v2x_msg` package. Only a subset of the fields
 * is populated; callers can augment the ROS messages before publishing if more
 * detail is required.
 */
struct Ros2Bridge {
    static v2x_msg::msg::BSM to_ros(const j2735::BasicSafetyMessage& msg);
    static j2735::BasicSafetyMessage from_ros(const v2x_msg::msg::BSM& msg);

    static v2x_msg::msg::MAP to_ros(const j2735::MapMessage& msg);
    static j2735::MapMessage from_ros(const v2x_msg::msg::MAP& msg);

    static v2x_msg::msg::SPAT to_ros(const j2735::SpatMessage& msg);
    static j2735::SpatMessage from_ros(const v2x_msg::msg::SPAT& msg);

    static v2x_msg::msg::SRM to_ros(const j2735::SignalRequestMessage& msg);
    static j2735::SignalRequestMessage from_ros(const v2x_msg::msg::SRM& msg);

    static v2x_msg::msg::SSM to_ros(const j2735::SignalStatusMessage& msg);
    static j2735::SignalStatusMessage from_ros(const v2x_msg::msg::SSM& msg);
};

#endif // IPI_ENABLE_ROS2

} // namespace ipi::v2x
