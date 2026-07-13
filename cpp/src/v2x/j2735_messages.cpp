#include "ipi/v2x/j2735_messages.hpp"

#include "ipi/common/debug.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

void write_uint16(std::vector<std::uint8_t>& buffer, std::uint16_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

std::uint16_t read_uint16(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 2 > buffer.size()) {
        throw std::runtime_error("Buffer underrun reading uint16");
    }
    auto high = buffer[offset++];
    auto low = buffer[offset++];
    return static_cast<std::uint16_t>((high << 8) | low);
}

void write_uint32(std::vector<std::uint8_t>& buffer, std::uint32_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

std::uint32_t read_uint32(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 4 > buffer.size()) {
        throw std::runtime_error("Buffer underrun reading uint32");
    }
    std::uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        value = (value << 8) | buffer[offset++];
    }
    return value;
}

void write_float(std::vector<std::uint8_t>& buffer, float value) {
    static_assert(sizeof(float) == 4, "Unexpected float size");
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(float));
    write_uint32(buffer, bits);
}

float read_float(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    auto bits = read_uint32(buffer, offset);
    float value;
    std::memcpy(&value, &bits, sizeof(float));
    return value;
}

void write_double(std::vector<std::uint8_t>& buffer, double value) {
    static_assert(sizeof(double) == 8, "Unexpected double size");
    std::uint64_t bits;
    std::memcpy(&bits, &value, sizeof(double));
    for (int shift = 56; shift >= 0; shift -= 8) {
        buffer.push_back(static_cast<std::uint8_t>((bits >> shift) & 0xFF));
    }
}

double read_double(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 8 > buffer.size()) {
        throw std::runtime_error("Buffer underrun reading double");
    }
    std::uint64_t bits = 0;
    for (int i = 0; i < 8; ++i) {
        bits = (bits << 8) | buffer[offset++];
    }
    double value;
    std::memcpy(&value, &bits, sizeof(double));
    return value;
}

void write_string(std::vector<std::uint8_t>& buffer, const std::string& value) {
    if (value.size() > 255) {
        throw std::invalid_argument("String exceeds 255 bytes");
    }
    buffer.push_back(static_cast<std::uint8_t>(value.size()));
    buffer.insert(buffer.end(), value.begin(), value.end());
}

std::string read_string(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset >= buffer.size()) {
        throw std::runtime_error("Buffer underrun reading string length");
    }
    std::uint8_t len = buffer[offset++];
    if (offset + len > buffer.size()) {
        throw std::runtime_error("String length exceeds buffer");
    }
    std::string value(buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                      buffer.begin() + static_cast<std::ptrdiff_t>(offset + len));
    offset += len;
    return value;
}

} // namespace

namespace ipi::j2735 {

void BasicSafetyMessage::validate() const {
    if (latitude < -90.0 || latitude > 90.0) {
        throw std::invalid_argument("latitude out of range");
    }
    if (longitude < -180.0 || longitude > 180.0) {
        throw std::invalid_argument("longitude out of range");
    }
    if (speedMps < 0.0F) {
        throw std::invalid_argument("speedMps must be non-negative");
    }
    if (headingDeg < 0.0F || headingDeg >= 360.0F) {
        throw std::invalid_argument("headingDeg must be within [0, 360)");
    }
    if (accelerationMps2 && (*accelerationMps2 < -20.0F || *accelerationMps2 > 20.0F)) {
        throw std::invalid_argument("accelerationMps2 out of range");
    }
}

std::vector<std::uint8_t> BasicSafetyMessage::to_bytes() const {
    validate();
    std::vector<std::uint8_t> buffer;
    buffer.reserve(32);
    write_uint32(buffer, vehicleId);
    write_double(buffer, latitude);
    write_double(buffer, longitude);
    write_float(buffer, speedMps);
    write_float(buffer, headingDeg);
    std::uint8_t flags = 0;
    if (accelerationMps2) flags |= 0x01;
    if (laneId) flags |= 0x02;
    buffer.push_back(flags);
    if (accelerationMps2) {
        write_float(buffer, *accelerationMps2);
    }
    if (laneId) {
        write_uint16(buffer, *laneId);
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[BSM] encode ", to_string(), " bytes=", buffer.size(),
                        " hex=", ipi::debug::hex(buffer));
    }
    return buffer;
}

BasicSafetyMessage BasicSafetyMessage::from_bytes(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < 4 + 8 + 8 + 4 + 4 + 1) {
        throw std::runtime_error("Buffer too small for BasicSafetyMessage");
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[BSM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    BasicSafetyMessage msg;
    std::size_t offset = 0;
    msg.vehicleId = read_uint32(buffer, offset);
    msg.latitude = read_double(buffer, offset);
    msg.longitude = read_double(buffer, offset);
    msg.speedMps = read_float(buffer, offset);
    msg.headingDeg = read_float(buffer, offset);
    std::uint8_t flags = buffer[offset++];
    if (flags & 0x01) {
        msg.accelerationMps2 = read_float(buffer, offset);
    }
    if (flags & 0x02) {
        msg.laneId = read_uint16(buffer, offset);
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[BSM] decoded ", msg.to_string());
        auto roundtrip = msg.to_bytes();
        if (roundtrip != buffer) {
            ipi::debug::log("[BSM] round-trip differs hex=", ipi::debug::hex(roundtrip));
        }
    }
    return msg;
}

std::string BasicSafetyMessage::to_string() const {
    std::ostringstream oss;
    oss << "BSM{vehicleId=" << vehicleId << ", lat=" << std::fixed << std::setprecision(6) << latitude
        << ", lon=" << longitude << ", speedMps=" << speedMps << ", headingDeg=" << headingDeg;
    if (accelerationMps2) {
        oss << ", accel=" << *accelerationMps2;
    }
    if (laneId) {
        oss << ", laneId=" << *laneId;
    }
    oss << "}";
    return oss.str();
}

void PersonalSafetyMessage::validate() const {
    const auto basicTypeValue = static_cast<std::uint8_t>(basicType);
    if (basicTypeValue > static_cast<std::uint8_t>(PersonalDeviceUserType::Animal)) {
        throw std::invalid_argument("PSM basicType is not supported");
    }
    if (messageCount > 127) {
        throw std::invalid_argument("PSM messageCount must be <= 127");
    }
    if (!std::isfinite(latitude) || latitude < -90.0 || latitude > 90.0) {
        throw std::invalid_argument("PSM latitude out of range");
    }
    if (!std::isfinite(longitude) || longitude < -180.0 || longitude > 180.0) {
        throw std::invalid_argument("PSM longitude out of range");
    }
    if (!std::isfinite(speedMps) || speedMps < 0.0F || speedMps > 163.82F) {
        throw std::invalid_argument("PSM speedMps must be within [0, 163.82]");
    }
    if (!std::isfinite(headingDeg) || headingDeg < 0.0F || headingDeg >= 360.0F) {
        throw std::invalid_argument("PSM headingDeg must be within [0, 360)");
    }
    if (elevationM &&
        (!std::isfinite(*elevationM) || *elevationM < -409.5F || *elevationM > 6143.9F)) {
        throw std::invalid_argument("PSM elevationM out of range");
    }
    if (horizontalAccuracyM &&
        (!std::isfinite(*horizontalAccuracyM) || *horizontalAccuracyM < 0.0F ||
         *horizontalAccuracyM > 655.35F)) {
        throw std::invalid_argument("PSM horizontalAccuracyM out of range");
    }
    if (acceleration) {
        const auto in_range = [](float value, float min, float max) {
            return std::isfinite(value) && value >= min && value <= max;
        };
        if (!in_range(acceleration->longitudinalMps2, -20.0F, 20.0F) ||
            !in_range(acceleration->lateralMps2, -20.0F, 20.0F) ||
            !in_range(acceleration->verticalMps2, -20.0F, 20.0F) ||
            !in_range(acceleration->yawRateDegPerSec, -327.67F, 327.67F)) {
            throw std::invalid_argument("PSM acceleration field out of range");
        }
    }
    if (pathHistory.size() > 23) {
        throw std::invalid_argument("PSM pathHistory supports at most 23 points");
    }
    if (pathPrediction && pathPrediction->confidence > 100) {
        throw std::invalid_argument("PSM pathPrediction confidence must be <= 100");
    }
    if (propulsion && static_cast<std::uint8_t>(propulsion->kind) >
                          static_cast<std::uint8_t>(PersonalPropulsionKind::Motor)) {
        throw std::invalid_argument("PSM propulsion kind is not supported");
    }
}

std::vector<std::uint8_t> PersonalSafetyMessage::to_bytes() const {
    validate();
    std::vector<std::uint8_t> buffer;
    buffer.reserve(40 + pathHistory.size() * 8);
    buffer.push_back(static_cast<std::uint8_t>(basicType));
    write_uint16(buffer, secondMarkMs);
    buffer.push_back(messageCount);
    write_uint32(buffer, temporaryId);
    write_double(buffer, latitude);
    write_double(buffer, longitude);
    write_float(buffer, speedMps);
    write_float(buffer, headingDeg);

    std::uint8_t flags = 0;
    if (elevationM) flags |= 0x01;
    if (horizontalAccuracyM) flags |= 0x02;
    if (acceleration) flags |= 0x04;
    if (pathPrediction) flags |= 0x08;
    if (propulsion) flags |= 0x10;
    buffer.push_back(flags);

    if (elevationM) write_float(buffer, *elevationM);
    if (horizontalAccuracyM) write_float(buffer, *horizontalAccuracyM);
    if (acceleration) {
        write_float(buffer, acceleration->longitudinalMps2);
        write_float(buffer, acceleration->lateralMps2);
        write_float(buffer, acceleration->verticalMps2);
        write_float(buffer, acceleration->yawRateDegPerSec);
    }
    if (pathPrediction) {
        write_uint16(buffer, static_cast<std::uint16_t>(pathPrediction->radiusOfCurveM));
        buffer.push_back(pathPrediction->confidence);
    }
    if (propulsion) {
        buffer.push_back(static_cast<std::uint8_t>(propulsion->kind));
        buffer.push_back(propulsion->subtype);
    }

    buffer.push_back(static_cast<std::uint8_t>(pathHistory.size()));
    for (const auto& point : pathHistory) {
        write_uint16(buffer, static_cast<std::uint16_t>(point.latitudeOffset));
        write_uint16(buffer, static_cast<std::uint16_t>(point.longitudeOffset));
        write_uint16(buffer, static_cast<std::uint16_t>(point.elevationOffset));
        write_uint16(buffer, point.timeOffsetMs);
    }

    if (ipi::debug::enabled()) {
        ipi::debug::log("[PSM] encode ", to_string(), " bytes=", buffer.size(),
                        " hex=", ipi::debug::hex(buffer));
    }
    return buffer;
}

PersonalSafetyMessage PersonalSafetyMessage::from_bytes(const std::vector<std::uint8_t>& buffer) {
    constexpr std::size_t kMinimumSize = 1 + 2 + 1 + 4 + 8 + 8 + 4 + 4 + 1 + 1;
    if (buffer.size() < kMinimumSize) {
        throw std::runtime_error("Buffer too small for PersonalSafetyMessage");
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[PSM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }

    PersonalSafetyMessage msg;
    std::size_t offset = 0;
    msg.basicType = static_cast<PersonalDeviceUserType>(buffer[offset++]);
    msg.secondMarkMs = read_uint16(buffer, offset);
    msg.messageCount = buffer[offset++];
    msg.temporaryId = read_uint32(buffer, offset);
    msg.latitude = read_double(buffer, offset);
    msg.longitude = read_double(buffer, offset);
    msg.speedMps = read_float(buffer, offset);
    msg.headingDeg = read_float(buffer, offset);
    const std::uint8_t flags = buffer[offset++];

    if (flags & 0x01) msg.elevationM = read_float(buffer, offset);
    if (flags & 0x02) msg.horizontalAccuracyM = read_float(buffer, offset);
    if (flags & 0x04) {
        PersonalAccelerationSet values;
        values.longitudinalMps2 = read_float(buffer, offset);
        values.lateralMps2 = read_float(buffer, offset);
        values.verticalMps2 = read_float(buffer, offset);
        values.yawRateDegPerSec = read_float(buffer, offset);
        msg.acceleration = values;
    }
    if (flags & 0x08) {
        PersonalPathPrediction prediction;
        prediction.radiusOfCurveM = static_cast<std::int16_t>(read_uint16(buffer, offset));
        if (offset >= buffer.size()) {
            throw std::runtime_error("Buffer underrun reading PSM path prediction confidence");
        }
        prediction.confidence = buffer[offset++];
        msg.pathPrediction = prediction;
    }
    if (flags & 0x10) {
        if (offset + 2 > buffer.size()) {
            throw std::runtime_error("Buffer underrun reading PSM propulsion");
        }
        PersonalPropelledInformation propelled;
        propelled.kind = static_cast<PersonalPropulsionKind>(buffer[offset++]);
        propelled.subtype = buffer[offset++];
        msg.propulsion = propelled;
    }
    if (offset >= buffer.size()) {
        throw std::runtime_error("Buffer underrun reading PSM path history count");
    }
    const auto historyCount = static_cast<std::size_t>(buffer[offset++]);
    msg.pathHistory.reserve(historyCount);
    for (std::size_t i = 0; i < historyCount; ++i) {
        PersonalPathHistoryPoint point;
        point.latitudeOffset = static_cast<std::int16_t>(read_uint16(buffer, offset));
        point.longitudeOffset = static_cast<std::int16_t>(read_uint16(buffer, offset));
        point.elevationOffset = static_cast<std::int16_t>(read_uint16(buffer, offset));
        point.timeOffsetMs = read_uint16(buffer, offset);
        msg.pathHistory.push_back(point);
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[PSM] decoded ", msg.to_string());
    }
    return msg;
}

std::string PersonalSafetyMessage::to_string() const {
    std::ostringstream oss;
    oss << "PSM{type=" << ipi::j2735::to_string(basicType)
        << ", temporaryId=" << temporaryId
        << ", msgCount=" << static_cast<unsigned>(messageCount)
        << ", secMarkMs=" << secondMarkMs
        << ", lat=" << std::fixed << std::setprecision(7) << latitude
        << ", lon=" << longitude
        << ", speedMps=" << speedMps
        << ", headingDeg=" << headingDeg
        << ", pathPoints=" << pathHistory.size();
    if (horizontalAccuracyM) {
        oss << ", accuracyM=" << *horizontalAccuracyM;
    }
    if (propulsion) {
        oss << ", propulsion=" << ipi::j2735::to_string(propulsion->kind)
            << ':' << static_cast<unsigned>(propulsion->subtype);
    }
    oss << '}';
    return oss.str();
}

void MapMessage::validate() const {
    if (lanes.empty()) {
        throw std::invalid_argument("MAP requires at least one lane");
    }
    if (lanes.size() > 64) {
        throw std::invalid_argument("MAP supports at most 64 lanes in this profile");
    }
}

std::vector<std::uint8_t> MapMessage::to_bytes() const {
    validate();
    std::vector<std::uint8_t> buffer;
    buffer.reserve(16 + lanes.size() * 4);
    write_uint16(buffer, intersectionId);
    buffer.push_back(revision);
    std::uint8_t flags = 0;
    if (name) flags |= 0x01;
    buffer.push_back(flags);
    if (name) {
        write_string(buffer, *name);
    }
    buffer.push_back(static_cast<std::uint8_t>(lanes.size()));
    for (const auto& lane : lanes) {
        write_uint16(buffer, lane.laneId);
        buffer.push_back(lane.ingress ? 1 : 0);
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[MAP] encode ", to_string(), " bytes=", buffer.size(),
                        " hex=", ipi::debug::hex(buffer));
    }
    return buffer;
}

MapMessage MapMessage::from_bytes(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < 4) {
        throw std::runtime_error("Buffer too small for MAP");
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[MAP] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    MapMessage map;
    std::size_t offset = 0;
    map.intersectionId = read_uint16(buffer, offset);
    map.revision = buffer[offset++];
    std::uint8_t flags = buffer[offset++];
    if (flags & 0x01) {
        map.name = read_string(buffer, offset);
    }
    if (offset >= buffer.size()) {
        throw std::runtime_error("Buffer underrun reading lane count");
    }
    std::uint8_t laneCount = buffer[offset++];
    map.lanes.reserve(laneCount);
    for (std::uint8_t i = 0; i < laneCount; ++i) {
        auto laneId = read_uint16(buffer, offset);
        if (offset >= buffer.size()) {
            throw std::runtime_error("Buffer underrun reading lane ingress flag");
        }
        bool ingress = buffer[offset++] != 0;
        map.lanes.push_back(MapLane{laneId, ingress});
    }
    map.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[MAP] decoded ", map.to_string());
        auto roundtrip = map.to_bytes();
        if (roundtrip != buffer) {
            ipi::debug::log("[MAP] round-trip differs hex=", ipi::debug::hex(roundtrip));
        }
    }
    return map;
}

std::string MapMessage::to_string() const {
    std::ostringstream oss;
    oss << "MAP{intersection=" << intersectionId << ", revision=" << static_cast<int>(revision)
        << ", lanes=" << lanes.size();
    if (name) {
        oss << ", name='" << *name << "'";
    }
    oss << "}";
    return oss.str();
}

void SpatMessage::validate() const {
    if (phases.empty()) {
        throw std::invalid_argument("SPaT requires at least one phase state");
    }
    if (phases.size() > 32) {
        throw std::invalid_argument("SPaT supports at most 32 phases in this profile");
    }
}

std::vector<std::uint8_t> SpatMessage::to_bytes() const {
    validate();
    std::vector<std::uint8_t> buffer;
    buffer.reserve(12 + phases.size() * 6);
    write_uint16(buffer, intersectionId);
    write_uint32(buffer, timestampMs);
    buffer.push_back(static_cast<std::uint8_t>(phases.size()));
    for (const auto& phase : phases) {
        buffer.push_back(phase.signalGroup);
        buffer.push_back(static_cast<std::uint8_t>(phase.state));
        buffer.push_back(phase.timeToChangeMs ? 1 : 0);
        if (phase.timeToChangeMs) {
            write_uint16(buffer, *phase.timeToChangeMs);
        }
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SPaT] encode ", to_string(), " bytes=", buffer.size(),
                        " hex=", ipi::debug::hex(buffer));
    }
    return buffer;
}

SpatMessage SpatMessage::from_bytes(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < 7) {
        throw std::runtime_error("Buffer too small for SPaT");
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SPaT] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    SpatMessage spat;
    std::size_t offset = 0;
    spat.intersectionId = read_uint16(buffer, offset);
    spat.timestampMs = read_uint32(buffer, offset);
    if (offset >= buffer.size()) {
        throw std::runtime_error("Buffer underrun reading phase count");
    }
    std::uint8_t phaseCount = buffer[offset++];
    spat.phases.reserve(phaseCount);
    for (std::uint8_t i = 0; i < phaseCount; ++i) {
        if (offset + 3 > buffer.size()) {
            throw std::runtime_error("SPaT phase truncated");
        }
        SpatPhaseState phase{};
        phase.signalGroup = buffer[offset++];
        phase.state = static_cast<MovementPhaseState>(buffer[offset++]);
        bool hasTime = buffer[offset++] != 0;
        if (hasTime) {
            phase.timeToChangeMs = read_uint16(buffer, offset);
        }
        spat.phases.push_back(std::move(phase));
    }
    spat.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SPaT] decoded ", spat.to_string());
        auto roundtrip = spat.to_bytes();
        if (roundtrip != buffer) {
            ipi::debug::log("[SPaT] round-trip differs hex=", ipi::debug::hex(roundtrip));
        }
    }
    return spat;
}

std::string SpatMessage::to_string() const {
    std::ostringstream oss;
    oss << "SPaT{intersection=" << intersectionId << ", phases=" << phases.size() << "}";
    return oss.str();
}

void SignalRequestMessage::validate() const {
    if (requestedSignalGroup == 0) {
        throw std::invalid_argument("requestedSignalGroup must be non-zero");
    }
    if (priorityLevel && *priorityLevel > 7) {
        throw std::invalid_argument("priorityLevel must be <= 7");
    }
}

std::vector<std::uint8_t> SignalRequestMessage::to_bytes() const {
    validate();
    std::vector<std::uint8_t> buffer;
    buffer.reserve(16);
    write_uint16(buffer, requestId);
    write_uint32(buffer, vehicleId);
    write_uint16(buffer, intersectionId);
    buffer.push_back(requestedSignalGroup);
    std::uint8_t flags = 0;
    if (estimatedArrivalMs) flags |= 0x01;
    if (priorityLevel) flags |= 0x02;
    buffer.push_back(flags);
    if (estimatedArrivalMs) {
        write_uint32(buffer, *estimatedArrivalMs);
    }
    if (priorityLevel) {
        buffer.push_back(*priorityLevel);
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SRM] encode ", to_string(), " bytes=", buffer.size(),
                        " hex=", ipi::debug::hex(buffer));
    }
    return buffer;
}

SignalRequestMessage SignalRequestMessage::from_bytes(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < 2 + 4 + 2 + 1 + 1) {
        throw std::runtime_error("Buffer too small for SRM");
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SRM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    SignalRequestMessage msg;
    std::size_t offset = 0;
    msg.requestId = read_uint16(buffer, offset);
    msg.vehicleId = read_uint32(buffer, offset);
    msg.intersectionId = read_uint16(buffer, offset);
    msg.requestedSignalGroup = buffer[offset++];
    std::uint8_t flags = buffer[offset++];
    if (flags & 0x01) {
        msg.estimatedArrivalMs = read_uint32(buffer, offset);
    }
    if (flags & 0x02) {
        if (offset >= buffer.size()) {
            throw std::runtime_error("Buffer underrun reading priorityLevel");
        }
        msg.priorityLevel = buffer[offset++];
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SRM] decoded ", msg.to_string());
        auto roundtrip = msg.to_bytes();
        if (roundtrip != buffer) {
            ipi::debug::log("[SRM] round-trip differs hex=", ipi::debug::hex(roundtrip));
        }
    }
    return msg;
}

std::string SignalRequestMessage::to_string() const {
    std::ostringstream oss;
    oss << "SRM{requestId=" << requestId << ", vehicleId=" << vehicleId
        << ", intersection=" << intersectionId << ", group=" << static_cast<int>(requestedSignalGroup);
    if (estimatedArrivalMs) {
        oss << ", etaMs=" << *estimatedArrivalMs;
    }
    if (priorityLevel) {
        oss << ", priority=" << static_cast<int>(*priorityLevel);
    }
    oss << "}";
    return oss.str();
}

void SignalStatusMessage::validate() const {
    if (grantedSignalGroup == 0) {
        throw std::invalid_argument("grantedSignalGroup must be non-zero");
    }
}

std::vector<std::uint8_t> SignalStatusMessage::to_bytes() const {
    validate();
    std::vector<std::uint8_t> buffer;
    buffer.reserve(12);
    write_uint16(buffer, requestId);
    write_uint16(buffer, intersectionId);
    buffer.push_back(grantedSignalGroup);
    buffer.push_back(granted ? 1 : 0);
    if (estimatedServedTimeMs) {
        buffer.push_back(1);
        write_uint16(buffer, *estimatedServedTimeMs);
    } else {
        buffer.push_back(0);
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SSM] encode ", to_string(), " bytes=", buffer.size(),
                        " hex=", ipi::debug::hex(buffer));
    }
    return buffer;
}

SignalStatusMessage SignalStatusMessage::from_bytes(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < 2 + 2 + 1 + 1 + 1) {
        throw std::runtime_error("Buffer too small for SSM");
    }
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SSM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    SignalStatusMessage msg;
    std::size_t offset = 0;
    msg.requestId = read_uint16(buffer, offset);
    msg.intersectionId = read_uint16(buffer, offset);
    msg.grantedSignalGroup = buffer[offset++];
    msg.granted = buffer[offset++] != 0;
    bool hasTime = buffer[offset++] != 0;
    if (hasTime) {
        msg.estimatedServedTimeMs = read_uint16(buffer, offset);
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[SSM] decoded ", msg.to_string());
        auto roundtrip = msg.to_bytes();
        if (roundtrip != buffer) {
            ipi::debug::log("[SSM] round-trip differs hex=", ipi::debug::hex(roundtrip));
        }
    }
    return msg;
}

std::string SignalStatusMessage::to_string() const {
    std::ostringstream oss;
    oss << "SSM{requestId=" << requestId << ", intersection=" << intersectionId
        << ", group=" << static_cast<int>(grantedSignalGroup)
        << ", granted=" << std::boolalpha << granted;
    if (estimatedServedTimeMs) {
        oss << ", servedInMs=" << *estimatedServedTimeMs;
    }
    oss << "}";
    return oss.str();
}

std::string to_string(MovementPhaseState state) {
    switch (state) {
        case MovementPhaseState::Dark:
            return "dark";
        case MovementPhaseState::StopAndRemain:
            return "stopAndRemain";
        case MovementPhaseState::StopThenProceed:
            return "stopThenProceed";
        case MovementPhaseState::Proceed:
            return "proceed";
        case MovementPhaseState::Flashing:
            return "flashing";
        default:
            return "unknown";
    }
}

std::string to_string(PersonalDeviceUserType type) {
    switch (type) {
        case PersonalDeviceUserType::Unavailable:
            return "unavailable";
        case PersonalDeviceUserType::Pedestrian:
            return "pedestrian";
        case PersonalDeviceUserType::PedalCyclist:
            return "pedalCyclist";
        case PersonalDeviceUserType::PublicSafetyWorker:
            return "publicSafetyWorker";
        case PersonalDeviceUserType::Animal:
            return "animal";
        default:
            return "unknown";
    }
}

std::string to_string(PersonalPropulsionKind kind) {
    switch (kind) {
        case PersonalPropulsionKind::Unavailable:
            return "unavailable";
        case PersonalPropulsionKind::Human:
            return "human";
        case PersonalPropulsionKind::Animal:
            return "animal";
        case PersonalPropulsionKind::Motor:
            return "motor";
        default:
            return "unknown";
    }
}

} // namespace ipi::j2735
