#include "ipi/v2x/ros2_bridge.hpp"

#ifdef IPI_ENABLE_ROS2

#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>

namespace ipi::v2x {

namespace {
constexpr double kLatLonScale = 1e7;       // J2735 lat/lon unit = 1e-7 degrees.
constexpr double kSpeedScale = 0.02;       // 0.02 m/s increment.
constexpr double kHeadingScale = 0.0125;   // 0.0125 degrees increment.
constexpr double kAccelScale = 0.01;       // 0.01 m/s^2 increment.
constexpr double kElevationScale = 0.1;    // 0.1 meters per unit.

inline std::int64_t clamp_to_int64(double value) {
    if (value > static_cast<double>(std::numeric_limits<std::int64_t>::max())) {
        return std::numeric_limits<std::int64_t>::max();
    }
    if (value < static_cast<double>(std::numeric_limits<std::int64_t>::min())) {
        return std::numeric_limits<std::int64_t>::min();
    }
    return static_cast<std::int64_t>(std::llround(value));
}

} // namespace

v2x_msg::msg::BSM Ros2Bridge::to_ros(const j2735::BasicSafetyMessage& msg) {
    v2x_msg::msg::BSM ros;
    auto& core = ros.coredata;
    core.msgcnt = static_cast<int64_t>(msg.vehicleId & 0x7F);
    std::ostringstream idStream;
    idStream << std::hex << std::setw(8) << std::setfill('0') << msg.vehicleId;
    core.id = idStream.str();
    core.lat = clamp_to_int64(msg.latitude * kLatLonScale);
    core.longitude = clamp_to_int64(msg.longitude * kLatLonScale);
    core.speed = clamp_to_int64(msg.speedMps / kSpeedScale);
    core.heading = clamp_to_int64(msg.headingDeg / kHeadingScale);
    if (msg.accelerationMps2) {
        core.accelset.longitudinal = clamp_to_int64(*msg.accelerationMps2 / kAccelScale);
    }
    if (msg.laneId) {
        core.size.length = static_cast<int64_t>(*msg.laneId);
    }
    core.elev = clamp_to_int64(0.0 / kElevationScale); // default 0.
    return ros;
}

j2735::BasicSafetyMessage Ros2Bridge::from_ros(const v2x_msg::msg::BSM& msg) {
    j2735::BasicSafetyMessage out;
    out.vehicleId = static_cast<std::uint32_t>(msg.coredata.msgcnt & 0xFFFFFFFF);
    out.latitude = static_cast<double>(msg.coredata.lat) / kLatLonScale;
    out.longitude = static_cast<double>(msg.coredata.longitude) / kLatLonScale;
    out.speedMps = static_cast<float>(msg.coredata.speed * kSpeedScale);
    out.headingDeg = static_cast<float>(msg.coredata.heading * kHeadingScale);
    if (msg.coredata.accelset.longitudinal != 0) {
        out.accelerationMps2 = static_cast<float>(msg.coredata.accelset.longitudinal * kAccelScale);
    }
    if (msg.coredata.size.length != 0) {
        out.laneId = static_cast<std::uint16_t>(msg.coredata.size.length);
    }
    return out;
}

v2x_msg::msg::MAP Ros2Bridge::to_ros(const j2735::MapMessage& msg) {
    v2x_msg::msg::MAP ros;
    ros.intersections.resize(1);
    auto& intersection = ros.intersections[0];
    intersection.id.id = static_cast<int64_t>(msg.intersectionId);
    intersection.revision = msg.revision;
    intersection.name = msg.name.value_or("");
    intersection.enabledlanes.reserve(msg.lanes.size());
    for (const auto& lane : msg.lanes) {
        v2x_msg::msg::LaneID laneId;
        laneId.id = lane.laneId;
        intersection.enabledlanes.push_back(laneId);
    }
    return ros;
}

j2735::MapMessage Ros2Bridge::from_ros(const v2x_msg::msg::MAP& msg) {
    j2735::MapMessage map;
    if (!msg.intersections.empty()) {
        const auto& intersection = msg.intersections[0];
        map.intersectionId = static_cast<std::uint16_t>(intersection.id.id);
        map.revision = static_cast<std::uint8_t>(intersection.revision);
        if (!intersection.name.empty()) {
            map.name = intersection.name;
        }
        map.lanes.reserve(intersection.enabledlanes.size());
        for (const auto& lane : intersection.enabledlanes) {
            map.lanes.push_back(j2735::MapLane{static_cast<std::uint16_t>(lane.id), true});
        }
    }
    return map;
}

v2x_msg::msg::SPAT Ros2Bridge::to_ros(const j2735::SpatMessage& msg) {
    v2x_msg::msg::SPAT ros;
    ros.timestamp = msg.timestampMs;
    ros.intersections.resize(1);
    auto& intersection = ros.intersections[0];
    intersection.id.id = msg.intersectionId;
    intersection.states.reserve(msg.phases.size());
    for (const auto& phase : msg.phases) {
        v2x_msg::msg::MovementState movement;
        movement.signalgroup = phase.signalGroup;
        movement.state_time_speed.resize(1);
        movement.state_time_speed[0].eventstate = static_cast<int64_t>(phase.state);
        if (phase.timeToChangeMs) {
            movement.state_time_speed[0].timing.minendtime = *phase.timeToChangeMs;
        }
        intersection.states.push_back(std::move(movement));
    }
    return ros;
}

j2735::SpatMessage Ros2Bridge::from_ros(const v2x_msg::msg::SPAT& msg) {
    j2735::SpatMessage spat;
    if (!msg.intersections.empty()) {
        const auto& intersection = msg.intersections[0];
        spat.intersectionId = static_cast<std::uint16_t>(intersection.id.id);
        spat.timestampMs = static_cast<std::uint32_t>(msg.timestamp);
        spat.phases.reserve(intersection.states.size());
        for (const auto& movement : intersection.states) {
            j2735::SpatPhaseState phase{};
            phase.signalGroup = static_cast<std::uint8_t>(movement.signalgroup);
            if (!movement.state_time_speed.empty()) {
                phase.state = static_cast<j2735::MovementPhaseState>(movement.state_time_speed[0].eventstate);
                if (movement.state_time_speed[0].timing.minendtime != 0) {
                    phase.timeToChangeMs = static_cast<std::uint16_t>(movement.state_time_speed[0].timing.minendtime);
                }
            }
            spat.phases.push_back(std::move(phase));
        }
    }
    return spat;
}

v2x_msg::msg::SRM Ros2Bridge::to_ros(const j2735::SignalRequestMessage& msg) {
    v2x_msg::msg::SRM ros;
    ros.request.request_id.id = msg.requestId;
    ros.request.vehicle_id.id = static_cast<int64_t>(msg.vehicleId);
    ros.request.requested_lane = msg.requestedSignalGroup;
    ros.request.inboundlane = msg.intersectionId;
    if (msg.estimatedArrivalMs) {
        ros.request.request_time = *msg.estimatedArrivalMs;
    }
    if (msg.priorityLevel) {
        ros.request.priority = *msg.priorityLevel;
    }
    return ros;
}

j2735::SignalRequestMessage Ros2Bridge::from_ros(const v2x_msg::msg::SRM& msg) {
    j2735::SignalRequestMessage srm;
    srm.requestId = static_cast<std::uint16_t>(msg.request.request_id.id);
    srm.vehicleId = static_cast<std::uint32_t>(msg.request.vehicle_id.id);
    srm.intersectionId = static_cast<std::uint16_t>(msg.request.inboundlane);
    srm.requestedSignalGroup = static_cast<std::uint8_t>(msg.request.requested_lane);
    if (msg.request.request_time != 0) {
        srm.estimatedArrivalMs = static_cast<std::uint32_t>(msg.request.request_time);
    }
    if (msg.request.priority != 0) {
        srm.priorityLevel = static_cast<std::uint8_t>(msg.request.priority);
    }
    return srm;
}

v2x_msg::msg::SSM Ros2Bridge::to_ros(const j2735::SignalStatusMessage& msg) {
    v2x_msg::msg::SSM ros;
    v2x_msg::msg::SignalStatusPackage package;
    package.requester.request_id.id = msg.requestId;
    package.intersection_id.id = msg.intersectionId;
    package.status.signalgroup = msg.grantedSignalGroup;
    package.status.eventstate = msg.granted ? 1 : 0;
    if (msg.estimatedServedTimeMs) {
        package.status.timedue = *msg.estimatedServedTimeMs;
    }
    ros.status.push_back(std::move(package));
    return ros;
}

j2735::SignalStatusMessage Ros2Bridge::from_ros(const v2x_msg::msg::SSM& msg) {
    j2735::SignalStatusMessage ssm;
    if (!msg.status.empty()) {
        const auto& package = msg.status[0];
        ssm.requestId = static_cast<std::uint16_t>(package.requester.request_id.id);
        ssm.intersectionId = static_cast<std::uint16_t>(package.intersection_id.id);
        ssm.grantedSignalGroup = static_cast<std::uint8_t>(package.status.signalgroup);
        ssm.granted = package.status.eventstate != 0;
        if (package.status.timedue != 0) {
            ssm.estimatedServedTimeMs = static_cast<std::uint16_t>(package.status.timedue);
        }
    }
    return ssm;
}

} // namespace ipi::v2x

#endif // IPI_ENABLE_ROS2
