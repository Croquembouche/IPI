#include "ipi/core/ipi_service_request.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

constexpr std::size_t kMaxAdditionalData = 65535;

void write_uint16(std::vector<std::uint8_t>& buffer, std::uint16_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

std::uint16_t read_uint16(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 2 > buffer.size()) {
        throw std::runtime_error("Buffer underrun while reading uint16");
    }
    std::uint16_t high = buffer[offset++];
    std::uint16_t low = buffer[offset++];
    return static_cast<std::uint16_t>((high << 8) | low);
}

} // namespace

namespace ipi {

void IpiServiceRequest::validate() const {
    if (static_cast<std::uint8_t>(serviceType) >
        static_cast<std::uint8_t>(ServiceType::HdMapUpdate)) {
        throw std::invalid_argument("unknown IPI service type");
    }
    if (additionalData.size() > kMaxAdditionalData) {
        throw std::invalid_argument("additionalData exceeds 65535 bytes");
    }
    if (desiredHorizonMs && *desiredHorizonMs > 10000) {
        throw std::invalid_argument("desiredHorizonMs must be <= 10000");
    }
}

std::vector<std::uint8_t> IpiServiceRequest::to_canonical_encoding() const {
    validate();

    std::vector<std::uint8_t> buffer;
    buffer.reserve(8 + additionalData.size());

    buffer.push_back(static_cast<std::uint8_t>(serviceType));
    write_uint16(buffer, requestId);

    std::uint8_t flags = 0;
    if (desiredHorizonMs.has_value()) {
        flags |= 0x01;
    }
    buffer.push_back(flags);

    if (desiredHorizonMs) {
        write_uint16(buffer, *desiredHorizonMs);
    }

    write_uint16(buffer, static_cast<std::uint16_t>(additionalData.size()));
    buffer.insert(buffer.end(), additionalData.begin(), additionalData.end());

    return buffer;
}

IpiServiceRequest IpiServiceRequest::from_canonical_encoding(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < 5) {
        throw std::runtime_error("Buffer too small for IpiServiceRequest");
    }

    std::size_t offset = 0;
    IpiServiceRequest request;

    const auto serviceType = buffer[offset++];
    if (serviceType > static_cast<std::uint8_t>(ServiceType::HdMapUpdate)) {
        throw std::runtime_error("Unknown service type in IpiServiceRequest");
    }
    request.serviceType = static_cast<ServiceType>(serviceType);
    request.requestId = read_uint16(buffer, offset);

    std::uint8_t flags = buffer[offset++];
    if ((flags & 0xFEU) != 0U) {
        throw std::runtime_error("Unknown flags in IpiServiceRequest");
    }

    if (flags & 0x01) {
        request.desiredHorizonMs = read_uint16(buffer, offset);
    }

    auto dataLen = read_uint16(buffer, offset);
    if (offset + dataLen > buffer.size()) {
        throw std::runtime_error("Invalid data length in IpiServiceRequest");
    }

    request.additionalData.assign(buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                                   buffer.begin() + static_cast<std::ptrdiff_t>(offset + dataLen));
    offset += dataLen;
    if (offset != buffer.size()) {
        throw std::runtime_error("Unexpected trailing bytes in IpiServiceRequest");
    }

    request.validate();
    return request;
}

std::string IpiServiceRequest::to_string() const {
    std::ostringstream oss;
    oss << "IPI-ServiceRequest{type=" << ipi::to_string(serviceType)
        << ", requestId=" << requestId;
    if (desiredHorizonMs) {
        oss << ", desiredHorizonMs=" << *desiredHorizonMs;
    }
    oss << ", additionalData=" << additionalData.size() << " bytes}";
    return oss.str();
}

std::string to_string(ServiceType type) {
    switch (type) {
        case ServiceType::LaneKeepingAid:
            return "laneKeepingAid";
        case ServiceType::UnprotectedLeftAvailability:
            return "unprotectedLeftAvailability";
        case ServiceType::PerceptionAid:
            return "perceptionAid";
        case ServiceType::PlanningAid:
            return "planningAid";
        case ServiceType::ControlAid:
            return "controlAid";
        case ServiceType::ComputationAid:
            return "computationAid";
        case ServiceType::HdMapUpdate:
            return "hdMapUpdate";
        default:
            return "unknown";
    }
}

} // namespace ipi
