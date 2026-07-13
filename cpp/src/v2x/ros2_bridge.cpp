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
constexpr double kAccuracyScale = 0.01;    // Compact horizontal-accuracy increment.

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
        core.accelset.longitude = clamp_to_int64(*msg.accelerationMps2 / kAccelScale);
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
    if (msg.coredata.accelset.longitude != 0) {
        out.accelerationMps2 = static_cast<float>(msg.coredata.accelset.longitude * kAccelScale);
    }
    if (msg.coredata.size.length != 0) {
        out.laneId = static_cast<std::uint16_t>(msg.coredata.size.length);
    }
    return out;
}

v2x_msg::msg::PSM Ros2Bridge::to_ros(const j2735::PersonalSafetyMessage& msg) {
    msg.validate();
    v2x_msg::msg::PSM ros;
    ros.basictype = static_cast<std::int64_t>(msg.basicType);
    ros.secmark = msg.secondMarkMs;
    ros.msgcnt = msg.messageCount;
    std::ostringstream idStream;
    idStream << std::hex << std::setw(8) << std::setfill('0') << msg.temporaryId;
    ros.id = idStream.str();
    ros.position.latitude = clamp_to_int64(msg.latitude * kLatLonScale);
    ros.position.longitude = clamp_to_int64(msg.longitude * kLatLonScale);
    if (msg.elevationM) {
        ros.position.elevation = clamp_to_int64(*msg.elevationM / kElevationScale);
    }
    if (msg.horizontalAccuracyM) {
        ros.accuracy.semimajor = clamp_to_int64(*msg.horizontalAccuracyM / kAccuracyScale);
        ros.accuracy.semiminor = ros.accuracy.semimajor;
    }
    ros.speed = clamp_to_int64(msg.speedMps / kSpeedScale);
    ros.heading = clamp_to_int64(msg.headingDeg / kHeadingScale);
    if (msg.acceleration) {
        ros.accelset.longitude = clamp_to_int64(msg.acceleration->longitudinalMps2 / kAccelScale);
        ros.accelset.lat = clamp_to_int64(msg.acceleration->lateralMps2 / kAccelScale);
        ros.accelset.vert = clamp_to_int64(msg.acceleration->verticalMps2 / kAccelScale);
        ros.accelset.yaw = clamp_to_int64(msg.acceleration->yawRateDegPerSec / kAccelScale);
    }
    ros.pathhistory.crumbdata.reserve(msg.pathHistory.size());
    for (const auto& point : msg.pathHistory) {
        v2x_msg::msg::PathHistoryPoint rosPoint;
        rosPoint.latoffset = point.latitudeOffset;
        rosPoint.lonoffset = point.longitudeOffset;
        rosPoint.elevationoffset = point.elevationOffset;
        rosPoint.timeoffset = point.timeOffsetMs;
        ros.pathhistory.crumbdata.push_back(std::move(rosPoint));
    }
    if (msg.pathPrediction) {
        ros.pathprediction.radiusofcurve = msg.pathPrediction->radiusOfCurveM;
        ros.pathprediction.confidence = msg.pathPrediction->confidence;
    }
    if (msg.propulsion) {
        switch (msg.propulsion->kind) {
            case j2735::PersonalPropulsionKind::Human:
                ros.propulsion.human = msg.propulsion->subtype;
                break;
            case j2735::PersonalPropulsionKind::Animal:
                ros.propulsion.animal = msg.propulsion->subtype;
                break;
            case j2735::PersonalPropulsionKind::Motor:
                ros.propulsion.motor = msg.propulsion->subtype;
                break;
            case j2735::PersonalPropulsionKind::Unavailable:
            default:
                break;
        }
    }
    return ros;
}

j2735::PersonalSafetyMessage Ros2Bridge::from_ros(const v2x_msg::msg::PSM& msg) {
    j2735::PersonalSafetyMessage out;
    out.basicType = static_cast<j2735::PersonalDeviceUserType>(msg.basictype);
    out.secondMarkMs = static_cast<std::uint16_t>(msg.secmark);
    out.messageCount = static_cast<std::uint8_t>(msg.msgcnt);
    try {
        out.temporaryId = static_cast<std::uint32_t>(std::stoul(msg.id, nullptr, 16));
    } catch (const std::exception&) {
        out.temporaryId = 0;
    }
    out.latitude = static_cast<double>(msg.position.latitude) / kLatLonScale;
    out.longitude = static_cast<double>(msg.position.longitude) / kLatLonScale;
    if (msg.position.elevation != 0) {
        out.elevationM = static_cast<float>(msg.position.elevation * kElevationScale);
    }
    if (msg.accuracy.semimajor != 0) {
        out.horizontalAccuracyM = static_cast<float>(msg.accuracy.semimajor * kAccuracyScale);
    }
    out.speedMps = static_cast<float>(msg.speed * kSpeedScale);
    out.headingDeg = static_cast<float>(msg.heading * kHeadingScale);
    if (msg.accelset.longitude != 0 || msg.accelset.lat != 0 ||
        msg.accelset.vert != 0 || msg.accelset.yaw != 0) {
        j2735::PersonalAccelerationSet acceleration;
        acceleration.longitudinalMps2 = static_cast<float>(msg.accelset.longitude * kAccelScale);
        acceleration.lateralMps2 = static_cast<float>(msg.accelset.lat * kAccelScale);
        acceleration.verticalMps2 = static_cast<float>(msg.accelset.vert * kAccelScale);
        acceleration.yawRateDegPerSec = static_cast<float>(msg.accelset.yaw * kAccelScale);
        out.acceleration = acceleration;
    }
    out.pathHistory.reserve(msg.pathhistory.crumbdata.size());
    for (const auto& rosPoint : msg.pathhistory.crumbdata) {
        out.pathHistory.push_back({static_cast<std::int16_t>(rosPoint.latoffset),
                                   static_cast<std::int16_t>(rosPoint.lonoffset),
                                   static_cast<std::int16_t>(rosPoint.elevationoffset),
                                   static_cast<std::uint16_t>(rosPoint.timeoffset)});
    }
    if (msg.pathprediction.radiusofcurve != 0 || msg.pathprediction.confidence != 0) {
        out.pathPrediction = j2735::PersonalPathPrediction{
            static_cast<std::int16_t>(msg.pathprediction.radiusofcurve),
            static_cast<std::uint8_t>(msg.pathprediction.confidence)};
    }
    if (msg.propulsion.human != 0) {
        out.propulsion = j2735::PersonalPropelledInformation{
            j2735::PersonalPropulsionKind::Human,
            static_cast<std::uint8_t>(msg.propulsion.human)};
    } else if (msg.propulsion.animal != 0) {
        out.propulsion = j2735::PersonalPropelledInformation{
            j2735::PersonalPropulsionKind::Animal,
            static_cast<std::uint8_t>(msg.propulsion.animal)};
    } else if (msg.propulsion.motor != 0) {
        out.propulsion = j2735::PersonalPropelledInformation{
            j2735::PersonalPropulsionKind::Motor,
            static_cast<std::uint8_t>(msg.propulsion.motor)};
    }
    out.validate();
    return out;
}

v2x_msg::msg::MAP Ros2Bridge::to_ros(const j2735::MapMessage& msg) {
    v2x_msg::msg::MAP ros;
    ros.intersections.resize(1);
    auto& intersection = ros.intersections[0];
    intersection.id.intersectionid = static_cast<int64_t>(msg.intersectionId);
    intersection.revision = msg.revision;
    intersection.name = msg.name.value_or("");
    intersection.laneset.reserve(msg.lanes.size());
    for (const auto& lane : msg.lanes) {
        v2x_msg::msg::GenericLane rosLane;
        rosLane.laneid = lane.laneId;
        if (lane.ingress) {
            rosLane.ingressapproach = 1;
        } else {
            rosLane.egressapproach = 1;
        }
        intersection.laneset.push_back(std::move(rosLane));
    }
    return ros;
}

j2735::MapMessage Ros2Bridge::from_ros(const v2x_msg::msg::MAP& msg) {
    j2735::MapMessage map;
    if (!msg.intersections.empty()) {
        const auto& intersection = msg.intersections[0];
        map.intersectionId = static_cast<std::uint16_t>(intersection.id.intersectionid);
        map.revision = static_cast<std::uint8_t>(intersection.revision);
        if (!intersection.name.empty()) {
            map.name = intersection.name;
        }
        map.lanes.reserve(intersection.laneset.size());
        for (const auto& lane : intersection.laneset) {
            map.lanes.push_back(j2735::MapLane{
                static_cast<std::uint16_t>(lane.laneid), lane.egressapproach == 0});
        }
    }
    return map;
}

v2x_msg::msg::SPAT Ros2Bridge::to_ros(const j2735::SpatMessage& msg) {
    v2x_msg::msg::SPAT ros;
    ros.timestamp = msg.timestampMs;
    ros.intersections.resize(1);
    auto& intersection = ros.intersections[0];
    intersection.id.intersectionid = msg.intersectionId;
    intersection.states.reserve(msg.phases.size());
    for (const auto& phase : msg.phases) {
        v2x_msg::msg::MovementState movement;
        movement.signalgroupid = phase.signalGroup;
        movement.statetimespeed.resize(1);
        movement.statetimespeed[0].movementphasestate = static_cast<int64_t>(phase.state);
        if (phase.timeToChangeMs) {
            movement.statetimespeed[0].timing.minendtime = *phase.timeToChangeMs;
        }
        intersection.states.push_back(std::move(movement));
    }
    return ros;
}

j2735::SpatMessage Ros2Bridge::from_ros(const v2x_msg::msg::SPAT& msg) {
    j2735::SpatMessage spat;
    if (!msg.intersections.empty()) {
        const auto& intersection = msg.intersections[0];
        spat.intersectionId = static_cast<std::uint16_t>(intersection.id.intersectionid);
        spat.timestampMs = static_cast<std::uint32_t>(msg.timestamp);
        spat.phases.reserve(intersection.states.size());
        for (const auto& movement : intersection.states) {
            j2735::SpatPhaseState phase{};
            phase.signalGroup = static_cast<std::uint8_t>(movement.signalgroupid);
            if (!movement.statetimespeed.empty()) {
                phase.state = static_cast<j2735::MovementPhaseState>(
                    movement.statetimespeed[0].movementphasestate);
                if (movement.statetimespeed[0].timing.minendtime != 0) {
                    phase.timeToChangeMs = static_cast<std::uint16_t>(
                        movement.statetimespeed[0].timing.minendtime);
                }
            }
            spat.phases.push_back(std::move(phase));
        }
    }
    return spat;
}

v2x_msg::msg::SRM Ros2Bridge::to_ros(const j2735::SignalRequestMessage& msg) {
    v2x_msg::msg::SRM ros;
    ros.requests.signalrequest.requestid = msg.requestId;
    ros.requests.signalrequest.id.intersectionid = msg.intersectionId;
    ros.requests.signalrequest.inboundlane.lane = msg.requestedSignalGroup;
    ros.requestor.id.stationid = static_cast<int64_t>(msg.vehicleId);
    if (msg.estimatedArrivalMs) {
        ros.requests.second = *msg.estimatedArrivalMs;
    }
    if (msg.priorityLevel) {
        ros.requests.signalrequest.requesttype = *msg.priorityLevel;
    }
    return ros;
}

j2735::SignalRequestMessage Ros2Bridge::from_ros(const v2x_msg::msg::SRM& msg) {
    j2735::SignalRequestMessage srm;
    srm.requestId = static_cast<std::uint16_t>(msg.requests.signalrequest.requestid);
    srm.vehicleId = static_cast<std::uint32_t>(msg.requestor.id.stationid);
    srm.intersectionId = static_cast<std::uint16_t>(
        msg.requests.signalrequest.id.intersectionid);
    srm.requestedSignalGroup = static_cast<std::uint8_t>(
        msg.requests.signalrequest.inboundlane.lane);
    if (msg.requests.second != 0) {
        srm.estimatedArrivalMs = static_cast<std::uint32_t>(msg.requests.second);
    }
    if (msg.requests.signalrequest.requesttype != 0) {
        srm.priorityLevel = static_cast<std::uint8_t>(msg.requests.signalrequest.requesttype);
    }
    return srm;
}

v2x_msg::msg::SSM Ros2Bridge::to_ros(const j2735::SignalStatusMessage& msg) {
    v2x_msg::msg::SSM ros;
    v2x_msg::msg::SignalStatus signalStatus;
    signalStatus.sequencenumber = msg.requestId;
    signalStatus.id.intersectionid = msg.intersectionId;
    v2x_msg::msg::SignalStatusPackage package;
    package.requester.request = msg.requestId;
    package.inboundon.lane = msg.grantedSignalGroup;
    package.status = msg.granted ? 1 : 0;
    if (msg.estimatedServedTimeMs) {
        package.duration = *msg.estimatedServedTimeMs;
    }
    signalStatus.sigstatus.push_back(std::move(package));
    ros.status.push_back(std::move(signalStatus));
    return ros;
}

j2735::SignalStatusMessage Ros2Bridge::from_ros(const v2x_msg::msg::SSM& msg) {
    j2735::SignalStatusMessage ssm;
    if (!msg.status.empty()) {
        const auto& signalStatus = msg.status[0];
        ssm.intersectionId = static_cast<std::uint16_t>(signalStatus.id.intersectionid);
        ssm.requestId = static_cast<std::uint16_t>(signalStatus.sequencenumber);
        if (!signalStatus.sigstatus.empty()) {
            const auto& package = signalStatus.sigstatus[0];
            if (package.requester.request != 0) {
                ssm.requestId = static_cast<std::uint16_t>(package.requester.request);
            }
            ssm.grantedSignalGroup = static_cast<std::uint8_t>(package.inboundon.lane);
            ssm.granted = package.status != 0;
            if (package.duration != 0) {
                ssm.estimatedServedTimeMs = static_cast<std::uint16_t>(package.duration);
            }
        }
    }
    return ssm;
}

} // namespace ipi::v2x

#endif // IPI_ENABLE_ROS2
