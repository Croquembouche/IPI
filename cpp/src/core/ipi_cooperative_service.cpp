#include "ipi/core/ipi_cooperative_service.hpp"

#include <algorithm>
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
        throw std::runtime_error("Buffer underrun while reading uint16");
    }
    std::uint16_t hi = buffer[offset++];
    std::uint16_t lo = buffer[offset++];
    return static_cast<std::uint16_t>((hi << 8) | lo);
}

void write_uint32(std::vector<std::uint8_t>& buffer, std::uint32_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

std::uint32_t read_uint32(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 4 > buffer.size()) {
        throw std::runtime_error("Buffer underrun while reading uint32");
    }
    std::uint32_t b0 = buffer[offset++];
    std::uint32_t b1 = buffer[offset++];
    std::uint32_t b2 = buffer[offset++];
    std::uint32_t b3 = buffer[offset++];
    return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

void write_double(std::vector<std::uint8_t>& buffer, double value) {
    std::uint64_t bits{};
    static_assert(sizeof(bits) == sizeof(value), "Unexpected double size");
    std::memcpy(&bits, &value, sizeof(double));
    for (int shift = 56; shift >= 0; shift -= 8) {
        buffer.push_back(static_cast<std::uint8_t>((bits >> shift) & 0xFF));
    }
}

double read_double(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 8 > buffer.size()) {
        throw std::runtime_error("Buffer underrun while reading double");
    }
    std::uint64_t bits = 0;
    for (int i = 0; i < 8; ++i) {
        bits = (bits << 8) | buffer[offset++];
    }
    double value{};
    std::memcpy(&value, &bits, sizeof(double));
    return value;
}

} // namespace

namespace ipi {

void CooperativeServiceMessage::validate() const {
    if (vehicleId.empty() || vehicleId.size() > 16) {
        throw std::invalid_argument("vehicleId must be between 1 and 16 bytes");
    }
    if (requestedHorizonMs && *requestedHorizonMs > 60000) {
        throw std::invalid_argument("requestedHorizonMs must be <= 60000");
    }
    if (confidence && *confidence > 100) {
        throw std::invalid_argument("confidence must be between 0 and 100");
    }
    if (offloadTaskId && offloadTaskId->size() > 65535) {
        throw std::invalid_argument("offloadTaskId exceeds 65535 bytes");
    }
    if (offloadPayload && offloadPayload->size() > 65535) {
        throw std::invalid_argument("offloadPayload exceeds 65535 bytes");
    }

    if (planning) {
        if (planning->waypoints.empty()) {
            throw std::invalid_argument("planning payload requires at least one waypoint");
        }
        if (planning->waypoints.size() > 50) {
            throw std::invalid_argument("planning payload exceeds 50 waypoints");
        }
        for (const auto& wp : planning->waypoints) {
            if (wp.targetSpeedMps && *wp.targetSpeedMps < 0.0) {
                throw std::invalid_argument("Waypoint targetSpeedMps must be non-negative");
            }
            if (wp.dwellTimeMs && *wp.dwellTimeMs > 10000) {
                throw std::invalid_argument("Waypoint dwellTimeMs must be <= 10000");
            }
        }
    }

    if (perception) {
        if (perception->detectedObjects.size() > 64) {
            throw std::invalid_argument("perception payload exceeds 64 detected objects");
        }
    }

    if (control) {
        if (control->commands.empty()) {
            throw std::invalid_argument("control payload requires at least one command");
        }
        if (control->commands.size() > 10) {
            throw std::invalid_argument("control payload exceeds 10 commands");
        }
        for (const auto& cmd : control->commands) {
            if (cmd.valuePermille < -1000 || cmd.valuePermille > 1000) {
                throw std::invalid_argument("control command value must be within [-1000, 1000]");
            }
        }
    }
}

std::vector<std::uint8_t> CooperativeServiceMessage::to_canonical_encoding() const {
    validate();

    std::vector<std::uint8_t> buffer;
    buffer.reserve(64);
    buffer.insert(buffer.end(), sessionId.begin(), sessionId.end());

    if (vehicleId.size() > 255) {
        throw std::invalid_argument("vehicleId overflow");
    }
    buffer.push_back(static_cast<std::uint8_t>(vehicleId.size()));
    buffer.insert(buffer.end(), vehicleId.begin(), vehicleId.end());

    buffer.push_back(static_cast<std::uint8_t>(serviceClass));
    buffer.push_back(static_cast<std::uint8_t>(guidanceStatus));

    std::uint8_t flags = 0;
    if (requestedHorizonMs) flags |= 0x01;
    if (confidence) flags |= 0x02;
    if (expirationTimeDs) flags |= 0x04;
    if (planning) flags |= 0x08;
    if (perception) flags |= 0x10;
    if (control) flags |= 0x20;
    if (offloadPayload) flags |= 0x40;
    if (offloadTaskId) flags |= 0x80;
    buffer.push_back(flags);

    if (requestedHorizonMs) {
        write_uint16(buffer, *requestedHorizonMs);
    }
    if (confidence) {
        buffer.push_back(*confidence);
    }
    if (expirationTimeDs) {
        write_uint32(buffer, *expirationTimeDs);
    }

    auto write_section = [&](const std::vector<std::uint8_t>& data) {
        if (data.size() > 65535) {
            throw std::invalid_argument("Section size exceeds 65535 bytes");
        }
        write_uint16(buffer, static_cast<std::uint16_t>(data.size()));
        buffer.insert(buffer.end(), data.begin(), data.end());
    };

    if (planning) {
        std::vector<std::uint8_t> section;
        section.push_back(planning->fallbackRoute ? 1 : 0);
        write_uint16(section, static_cast<std::uint16_t>(planning->waypoints.size()));
        for (const auto& wp : planning->waypoints) {
            write_double(section, wp.position.latitude);
            write_double(section, wp.position.longitude);
            section.push_back(wp.position.elevation ? 1 : 0);
            if (wp.position.elevation) {
                write_double(section, *wp.position.elevation);
            }
            section.push_back(wp.targetSpeedMps ? 1 : 0);
            if (wp.targetSpeedMps) {
                write_double(section, *wp.targetSpeedMps);
            }
            section.push_back(wp.dwellTimeMs ? 1 : 0);
            if (wp.dwellTimeMs) {
                write_uint16(section, *wp.dwellTimeMs);
            }
        }
        write_section(section);
    }

    if (perception) {
        std::vector<std::uint8_t> section;
        write_uint16(section, static_cast<std::uint16_t>(perception->detectedObjects.size()));
        for (const auto& obj : perception->detectedObjects) {
            section.insert(section.end(), obj.objectId.begin(), obj.objectId.end());
            section.push_back(static_cast<std::uint8_t>(obj.classification));
            write_double(section, obj.position.latitude);
            write_double(section, obj.position.longitude);
            section.push_back(obj.position.elevation ? 1 : 0);
            if (obj.position.elevation) {
                write_double(section, *obj.position.elevation);
            }
            section.push_back(obj.velocityMps ? 1 : 0);
            if (obj.velocityMps) {
                write_double(section, *obj.velocityMps);
            }
            if (obj.covariance.size() > 255) {
                throw std::invalid_argument("Covariance blob exceeds 255 bytes");
            }
            section.push_back(static_cast<std::uint8_t>(obj.covariance.size()));
            section.insert(section.end(), obj.covariance.begin(), obj.covariance.end());
        }
        write_section(section);
    }

    if (control) {
        std::vector<std::uint8_t> section;
        write_uint16(section, static_cast<std::uint16_t>(control->commands.size()));
        for (const auto& cmd : control->commands) {
            section.push_back(static_cast<std::uint8_t>(cmd.axis));
            auto encodedValue = static_cast<std::int32_t>(cmd.valuePermille) + 32768;
            write_uint16(section, static_cast<std::uint16_t>(encodedValue & 0xFFFF));
        }
        write_section(section);
    }

    if (offloadPayload) {
        write_section(*offloadPayload);
    }

    if (offloadTaskId) {
        std::vector<std::uint8_t> section;
        section.reserve(offloadTaskId->size());
        section.insert(section.end(), offloadTaskId->begin(), offloadTaskId->end());
        write_section(section);
    }

    return buffer;
}

CooperativeServiceMessage CooperativeServiceMessage::from_canonical_encoding(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < SessionId{}.size() + 4) {
        throw std::runtime_error("Buffer too small for CooperativeServiceMessage");
    }

    CooperativeServiceMessage msg;
    std::size_t offset = 0;
    std::copy_n(buffer.begin(), msg.sessionId.size(), msg.sessionId.begin());
    offset += msg.sessionId.size();

    if (offset >= buffer.size()) {
        throw std::runtime_error("Buffer underrun reading vehicleId length");
    }
    std::uint8_t vehicleIdLen = buffer[offset++];
    if (offset + vehicleIdLen > buffer.size()) {
        throw std::runtime_error("vehicleId length exceeds buffer");
    }
    msg.vehicleId.assign(buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                         buffer.begin() + static_cast<std::ptrdiff_t>(offset + vehicleIdLen));
    offset += vehicleIdLen;

    if (offset + 2 > buffer.size()) {
        throw std::runtime_error("Buffer underrun reading serviceClass/status");
    }
    msg.serviceClass = static_cast<ServiceClass>(buffer[offset++]);
    msg.guidanceStatus = static_cast<GuidanceStatus>(buffer[offset++]);

    if (offset >= buffer.size()) {
        throw std::runtime_error("Buffer underrun reading flags");
    }
    std::uint8_t flags = buffer[offset++];

    if (flags & 0x01) {
        msg.requestedHorizonMs = read_uint16(buffer, offset);
    }
    if (flags & 0x02) {
        if (offset >= buffer.size()) {
            throw std::runtime_error("Buffer underrun reading confidence");
        }
        msg.confidence = buffer[offset++];
    }
    if (flags & 0x04) {
        msg.expirationTimeDs = read_uint32(buffer, offset);
    }

    auto read_section = [&](std::vector<std::uint8_t>& out) {
        if (offset + 2 > buffer.size()) {
            throw std::runtime_error("Buffer underrun while reading section length");
        }
        auto len = read_uint16(buffer, offset);
        if (offset + len > buffer.size()) {
            throw std::runtime_error("Section length exceeds buffer");
        }
        out.assign(buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                   buffer.begin() + static_cast<std::ptrdiff_t>(offset + len));
        offset += len;
    };

    if (flags & 0x08) {
        std::vector<std::uint8_t> section;
        read_section(section);
        GuidedPlanningPayload payload;
        std::size_t sOff = 0;
        if (section.size() < 3) {
            throw std::runtime_error("Planning section too small");
        }
        payload.fallbackRoute = section[sOff++] != 0;
        auto count = read_uint16(section, sOff);
        payload.waypoints.reserve(count);
        for (std::uint16_t i = 0; i < count; ++i) {
            Waypoint wp;
            wp.position.latitude = read_double(section, sOff);
            wp.position.longitude = read_double(section, sOff);
            bool hasElevation = section[sOff++] != 0;
            if (hasElevation) {
                wp.position.elevation = read_double(section, sOff);
            }
            bool hasTargetSpeed = section[sOff++] != 0;
            if (hasTargetSpeed) {
                wp.targetSpeedMps = read_double(section, sOff);
            }
            bool hasDwell = section[sOff++] != 0;
            if (hasDwell) {
                wp.dwellTimeMs = read_uint16(section, sOff);
            }
            payload.waypoints.push_back(std::move(wp));
        }
        msg.planning = std::move(payload);
    }

    if (flags & 0x10) {
        std::vector<std::uint8_t> section;
        read_section(section);
        GuidedPerceptionPayload payload;
        std::size_t sOff = 0;
        auto count = read_uint16(section, sOff);
        payload.detectedObjects.reserve(count);
        for (std::uint16_t i = 0; i < count; ++i) {
            DetectedObject obj;
            if (sOff + obj.objectId.size() > section.size()) {
                throw std::runtime_error("Perception section truncated");
            }
            std::copy_n(section.begin() + static_cast<std::ptrdiff_t>(sOff), obj.objectId.size(), obj.objectId.begin());
            sOff += obj.objectId.size();
            obj.classification = static_cast<DetectedObject::Classification>(section[sOff++]);
            obj.position.latitude = read_double(section, sOff);
            obj.position.longitude = read_double(section, sOff);
            bool hasElevation = section[sOff++] != 0;
            if (hasElevation) {
                obj.position.elevation = read_double(section, sOff);
            }
            bool hasVelocity = section[sOff++] != 0;
            if (hasVelocity) {
                obj.velocityMps = read_double(section, sOff);
            }
            if (sOff >= section.size()) {
                throw std::runtime_error("Perception covariance length missing");
            }
            std::uint8_t covLen = section[sOff++];
            if (sOff + covLen > section.size()) {
                throw std::runtime_error("Perception covariance truncated");
            }
            obj.covariance.assign(section.begin() + static_cast<std::ptrdiff_t>(sOff),
                                   section.begin() + static_cast<std::ptrdiff_t>(sOff + covLen));
            sOff += covLen;
            payload.detectedObjects.push_back(std::move(obj));
        }
        msg.perception = std::move(payload);
    }

    if (flags & 0x20) {
        std::vector<std::uint8_t> section;
        read_section(section);
        GuidedControlPayload payload;
        std::size_t sOff = 0;
        auto count = read_uint16(section, sOff);
        payload.commands.reserve(count);
        for (std::uint16_t i = 0; i < count; ++i) {
            ControlCommand cmd;
            if (sOff >= section.size()) {
                throw std::runtime_error("Control section truncated (axis)");
            }
            cmd.axis = static_cast<ControlCommand::Axis>(section[sOff++]);
            auto rawValue = read_uint16(section, sOff);
            cmd.valuePermille = static_cast<std::int16_t>(static_cast<std::int32_t>(rawValue) - 32768);
            payload.commands.push_back(std::move(cmd));
        }
        msg.control = std::move(payload);
    }

    if (flags & 0x40) {
        std::vector<std::uint8_t> section;
        read_section(section);
        msg.offloadPayload = std::move(section);
    }

    if (flags & 0x80) {
        std::vector<std::uint8_t> section;
        read_section(section);
        msg.offloadTaskId = std::string(section.begin(), section.end());
    }

    msg.validate();
    return msg;
}

std::string CooperativeServiceMessage::to_string() const {
    std::ostringstream oss;
    oss << "CooperativeService{" << ipi::to_string(serviceClass)
        << ", status=" << ipi::to_string(guidanceStatus)
        << ", vehicleId=0x";
    for (auto byte : vehicleId) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    oss << std::dec;
    if (requestedHorizonMs) {
        oss << ", requestedHorizonMs=" << *requestedHorizonMs;
    }
    if (confidence) {
        oss << ", confidence=" << static_cast<int>(*confidence);
    }
    if (planning) {
        oss << ", planning.waypoints=" << planning->waypoints.size();
    }
    if (perception) {
        oss << ", perception.objects=" << perception->detectedObjects.size();
    }
    if (control) {
        oss << ", control.commands=" << control->commands.size();
    }
    if (offloadTaskId) {
        oss << ", offloadTaskId=" << *offloadTaskId;
    }
    if (offloadPayload) {
        oss << ", offloadPayload=" << offloadPayload->size() << " bytes";
    }
    oss << "}";
    return oss.str();
}

std::string to_string(ServiceClass serviceClass) {
    switch (serviceClass) {
        case ServiceClass::GuidedPlanning:
            return "guidedPlanning";
        case ServiceClass::GuidedPerception:
            return "guidedPerception";
        case ServiceClass::GuidedControl:
            return "guidedControl";
        default:
            return "unknown";
    }
}

std::string to_string(GuidanceStatus status) {
    switch (status) {
        case GuidanceStatus::Request:
            return "request";
        case GuidanceStatus::Update:
            return "update";
        case GuidanceStatus::Complete:
            return "complete";
        case GuidanceStatus::Reject:
            return "reject";
        default:
            return "unknown";
    }
}

std::string to_string(DetectedObject::Classification classification) {
    switch (classification) {
        case DetectedObject::Classification::Vehicle:
            return "vehicle";
        case DetectedObject::Classification::Pedestrian:
            return "pedestrian";
        case DetectedObject::Classification::Cyclist:
            return "cyclist";
        case DetectedObject::Classification::Obstacle:
            return "obstacle";
        default:
            return "unknown";
    }
}

std::string to_string(ControlCommand::Axis axis) {
    switch (axis) {
        case ControlCommand::Axis::Steering:
            return "steering";
        case ControlCommand::Axis::Throttle:
            return "throttle";
        case ControlCommand::Axis::Brake:
            return "brake";
        default:
            return "unknown";
    }
}

} // namespace ipi
