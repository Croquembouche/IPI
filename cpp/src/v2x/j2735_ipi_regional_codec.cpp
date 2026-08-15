#include "ipi/v2x/j2735_ipi_regional_codec.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace ipi::v2x {
namespace {

constexpr std::int64_t kLatitudeMinimum = -900000000LL;
constexpr std::int64_t kLatitudeMaximum = 900000000LL;
constexpr std::int64_t kLongitudeMinimum = -1800000000LL;
constexpr std::int64_t kLongitudeMaximum = 1800000000LL;
constexpr std::int64_t kElevationMinimum = -40960LL;
constexpr std::int64_t kElevationMaximum = 614390LL;
constexpr std::int64_t kSpeedMinimum = 0LL;
constexpr std::int64_t kSpeedMaximum = 65535LL;
constexpr std::int64_t kVelocityMinimum = -32768LL;
constexpr std::int64_t kVelocityMaximum = 32767LL;

std::size_t bits_for_range(std::int64_t lower, std::int64_t upper) {
    if (lower > upper) throw std::logic_error("invalid constrained range");
    const auto range = static_cast<std::uint64_t>(upper - lower) + 1U;
    if (range <= 1U) return 0;
    std::size_t bits = 0;
    std::uint64_t capacity = 1;
    while (capacity < range) {
        capacity <<= 1U;
        ++bits;
    }
    return bits;
}

class BitWriter {
public:
    void write_bit(bool value) {
        currentByte_ = static_cast<std::uint8_t>((currentByte_ << 1U) | (value ? 1U : 0U));
        ++bitOffset_;
        if (bitOffset_ == 8U) flush_byte();
    }

    void write_uint(std::uint64_t value, std::size_t bits) {
        if (bits > 64U) throw std::logic_error("UPER integer exceeds 64 bits");
        if (bits < 64U && value >= (std::uint64_t{1} << bits) && bits != 0U) {
            throw std::invalid_argument("UPER integer does not fit field width");
        }
        if (bits == 0U && value != 0U) {
            throw std::invalid_argument("non-zero value in zero-width UPER field");
        }
        for (std::size_t index = 0; index < bits; ++index) {
            const auto shift = bits - index - 1U;
            write_bit(((value >> shift) & 1U) != 0U);
        }
    }

    void write_constrained(std::int64_t value, std::int64_t lower, std::int64_t upper) {
        if (value < lower || value > upper) {
            throw std::invalid_argument("value outside ASN.1 constraint");
        }
        write_uint(static_cast<std::uint64_t>(value - lower), bits_for_range(lower, upper));
    }

    void write_octets(const std::uint8_t* data, std::size_t size) {
        if (data == nullptr && size != 0U) throw std::invalid_argument("null octet source");
        for (std::size_t index = 0; index < size; ++index) write_uint(data[index], 8U);
    }

    void write_octets(const std::vector<std::uint8_t>& data) {
        write_octets(data.data(), data.size());
    }

    void write_open_type(const std::vector<std::uint8_t>& value) {
        if (value.size() < 128U) {
            write_uint(value.size(), 8U);
        } else if (value.size() < 16384U) {
            write_uint(0x8000U | value.size(), 16U);
        } else {
            throw std::invalid_argument("fragmented UPER open types are outside the IPI profile");
        }
        write_octets(value);
    }

    [[nodiscard]] std::vector<std::uint8_t> finish() {
        if (bitOffset_ != 0U) {
            currentByte_ = static_cast<std::uint8_t>(currentByte_ << (8U - bitOffset_));
            flush_byte();
        }
        return std::move(bytes_);
    }

private:
    void flush_byte() {
        bytes_.push_back(currentByte_);
        currentByte_ = 0;
        bitOffset_ = 0;
    }

    std::vector<std::uint8_t> bytes_{};
    std::uint8_t currentByte_{0};
    std::uint8_t bitOffset_{0};
};

class BitReader {
public:
    explicit BitReader(const std::vector<std::uint8_t>& bytes) : bytes_(bytes) {}

    [[nodiscard]] bool read_bit() {
        if (bitPosition_ >= bytes_.size() * 8U) {
            throw std::runtime_error("truncated UPER bit stream");
        }
        const auto byteIndex = bitPosition_ / 8U;
        const auto bitIndex = 7U - (bitPosition_ % 8U);
        ++bitPosition_;
        return ((bytes_[byteIndex] >> bitIndex) & 1U) != 0U;
    }

    [[nodiscard]] std::uint64_t read_uint(std::size_t bits) {
        if (bits > 64U) throw std::logic_error("UPER integer exceeds 64 bits");
        std::uint64_t value = 0;
        for (std::size_t index = 0; index < bits; ++index) {
            value = (value << 1U) | (read_bit() ? 1U : 0U);
        }
        return value;
    }

    [[nodiscard]] std::int64_t read_constrained(std::int64_t lower, std::int64_t upper) {
        const auto bits = bits_for_range(lower, upper);
        const auto offset = read_uint(bits);
        const auto range = static_cast<std::uint64_t>(upper - lower) + 1U;
        if (offset >= range) throw std::runtime_error("invalid spare value in UPER field");
        return lower + static_cast<std::int64_t>(offset);
    }

    [[nodiscard]] std::vector<std::uint8_t> read_octets(std::size_t size) {
        if (size > remaining_bits() / 8U) throw std::runtime_error("truncated UPER octet string");
        std::vector<std::uint8_t> value;
        value.reserve(size);
        for (std::size_t index = 0; index < size; ++index) {
            value.push_back(static_cast<std::uint8_t>(read_uint(8U)));
        }
        return value;
    }

    [[nodiscard]] std::vector<std::uint8_t> read_open_type() {
        const auto first = static_cast<std::uint8_t>(read_uint(8U));
        std::size_t length = 0;
        if ((first & 0x80U) == 0U) {
            length = first;
        } else if ((first & 0xC0U) == 0x80U) {
            const auto second = static_cast<std::uint8_t>(read_uint(8U));
            length = (static_cast<std::size_t>(first & 0x3FU) << 8U) | second;
        } else {
            throw std::runtime_error("fragmented UPER open type is outside the IPI profile");
        }
        return read_octets(length);
    }

    [[nodiscard]] std::size_t remaining_bits() const noexcept {
        return (bytes_.size() * 8U) - bitPosition_;
    }

    void require_zero_padding() {
        while (remaining_bits() != 0U) {
            if (read_bit()) throw std::runtime_error("non-zero or trailing data in UPER padding");
        }
    }

private:
    const std::vector<std::uint8_t>& bytes_;
    std::size_t bitPosition_{0};
};

std::int64_t scale_value(double value,
                         double scale,
                         std::int64_t lower,
                         std::int64_t upper,
                         const char* field) {
    if (!std::isfinite(value)) throw std::invalid_argument(std::string(field) + " is not finite");
    const auto scaledDouble = value * scale;
    if (scaledDouble < static_cast<double>(lower) - 0.5 ||
        scaledDouble > static_cast<double>(upper) + 0.5) {
        throw std::invalid_argument(std::string(field) + " is outside the ASN.1 range");
    }
    const auto scaled = static_cast<std::int64_t>(std::llround(scaledDouble));
    if (scaled < lower || scaled > upper) {
        throw std::invalid_argument(std::string(field) + " is outside the ASN.1 range");
    }
    return scaled;
}

void write_position(BitWriter& writer, const Position3D& position) {
    writer.write_bit(position.elevation.has_value());
    writer.write_constrained(
        scale_value(position.latitude, 10000000.0, kLatitudeMinimum, kLatitudeMaximum, "latitude"),
        kLatitudeMinimum,
        kLatitudeMaximum);
    writer.write_constrained(
        scale_value(position.longitude, 10000000.0, kLongitudeMinimum, kLongitudeMaximum, "longitude"),
        kLongitudeMinimum,
        kLongitudeMaximum);
    if (position.elevation) {
        writer.write_constrained(
            scale_value(*position.elevation, 10.0, kElevationMinimum, kElevationMaximum, "elevation"),
            kElevationMinimum,
            kElevationMaximum);
    }
}

Position3D read_position(BitReader& reader) {
    Position3D position;
    const bool hasElevation = reader.read_bit();
    position.latitude = static_cast<double>(reader.read_constrained(
                            kLatitudeMinimum, kLatitudeMaximum)) /
                        10000000.0;
    position.longitude = static_cast<double>(reader.read_constrained(
                             kLongitudeMinimum, kLongitudeMaximum)) /
                         10000000.0;
    if (hasElevation) {
        position.elevation = static_cast<double>(reader.read_constrained(
                                 kElevationMinimum, kElevationMaximum)) /
                             10.0;
    }
    return position;
}

void write_waypoint(BitWriter& writer, const Waypoint& waypoint) {
    writer.write_bit(waypoint.targetSpeedMps.has_value());
    writer.write_bit(waypoint.dwellTimeMs.has_value());
    write_position(writer, waypoint.position);
    if (waypoint.targetSpeedMps) {
        writer.write_constrained(scale_value(*waypoint.targetSpeedMps,
                                              100.0,
                                              kSpeedMinimum,
                                              kSpeedMaximum,
                                              "targetSpeedMps"),
                                 kSpeedMinimum,
                                 kSpeedMaximum);
    }
    if (waypoint.dwellTimeMs) writer.write_constrained(*waypoint.dwellTimeMs, 0, 10000);
}

Waypoint read_waypoint(BitReader& reader) {
    Waypoint waypoint;
    const bool hasTargetSpeed = reader.read_bit();
    const bool hasDwell = reader.read_bit();
    waypoint.position = read_position(reader);
    if (hasTargetSpeed) {
        waypoint.targetSpeedMps =
            static_cast<double>(reader.read_constrained(kSpeedMinimum, kSpeedMaximum)) / 100.0;
    }
    if (hasDwell) {
        waypoint.dwellTimeMs = static_cast<std::uint16_t>(reader.read_constrained(0, 10000));
    }
    return waypoint;
}

void write_planning(BitWriter& writer, const GuidedPlanningPayload& planning) {
    writer.write_bit(planning.fallbackRoute);
    writer.write_constrained(static_cast<std::int64_t>(planning.waypoints.size()), 1, 50);
    for (const auto& waypoint : planning.waypoints) write_waypoint(writer, waypoint);
}

GuidedPlanningPayload read_planning(BitReader& reader) {
    GuidedPlanningPayload planning;
    planning.fallbackRoute = reader.read_bit();
    const auto count = static_cast<std::size_t>(reader.read_constrained(1, 50));
    planning.waypoints.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        planning.waypoints.push_back(read_waypoint(reader));
    }
    return planning;
}

void write_detected_object(BitWriter& writer, const DetectedObject& object) {
    writer.write_bit(object.velocityMps.has_value());
    writer.write_octets(object.objectId.data(), object.objectId.size());
    writer.write_constrained(static_cast<std::uint8_t>(object.classification), 0, 3);
    write_position(writer, object.position);
    if (object.velocityMps) {
        writer.write_constrained(scale_value(*object.velocityMps,
                                              100.0,
                                              kVelocityMinimum,
                                              kVelocityMaximum,
                                              "velocityMps"),
                                 kVelocityMinimum,
                                 kVelocityMaximum);
    }
    if (object.covariance.size() > 255U) {
        throw std::invalid_argument("covariance exceeds ASN.1 size constraint");
    }
    writer.write_constrained(static_cast<std::int64_t>(object.covariance.size()), 0, 255);
    writer.write_octets(object.covariance);
}

DetectedObject read_detected_object(BitReader& reader) {
    DetectedObject object;
    const bool hasVelocity = reader.read_bit();
    const auto objectId = reader.read_octets(object.objectId.size());
    std::copy(objectId.begin(), objectId.end(), object.objectId.begin());
    object.classification = static_cast<DetectedObject::Classification>(
        reader.read_constrained(0, 3));
    object.position = read_position(reader);
    if (hasVelocity) {
        object.velocityMps = static_cast<double>(reader.read_constrained(
                                 kVelocityMinimum, kVelocityMaximum)) /
                             100.0;
    }
    const auto covarianceSize = static_cast<std::size_t>(reader.read_constrained(0, 255));
    object.covariance = reader.read_octets(covarianceSize);
    return object;
}

void write_perception(BitWriter& writer, const GuidedPerceptionPayload& perception) {
    writer.write_constrained(static_cast<std::int64_t>(perception.detectedObjects.size()), 0, 64);
    for (const auto& object : perception.detectedObjects) write_detected_object(writer, object);
}

GuidedPerceptionPayload read_perception(BitReader& reader) {
    GuidedPerceptionPayload perception;
    const auto count = static_cast<std::size_t>(reader.read_constrained(0, 64));
    perception.detectedObjects.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        perception.detectedObjects.push_back(read_detected_object(reader));
    }
    return perception;
}

void write_control(BitWriter& writer, const GuidedControlPayload& control) {
    writer.write_constrained(static_cast<std::int64_t>(control.commands.size()), 1, 10);
    for (const auto& command : control.commands) {
        writer.write_constrained(static_cast<std::uint8_t>(command.axis), 0, 2);
        writer.write_constrained(command.valuePermille, -1000, 1000);
    }
}

GuidedControlPayload read_control(BitReader& reader) {
    GuidedControlPayload control;
    const auto count = static_cast<std::size_t>(reader.read_constrained(1, 10));
    control.commands.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        ControlCommand command;
        command.axis = static_cast<ControlCommand::Axis>(reader.read_constrained(0, 2));
        command.valuePermille = static_cast<std::int16_t>(reader.read_constrained(-1000, 1000));
        control.commands.push_back(command);
    }
    return control;
}

std::vector<std::uint8_t> encode_regional_value_impl(
    const CooperativeServiceMessage& message,
    const J2735IpiRegionalProfile& profile) {
    message.validate();
    if (message.offloadPayload && message.offloadPayload->size() > 2048U) {
        throw std::invalid_argument("offloadPayload exceeds ASN.1 size constraint");
    }
    if (message.offloadTaskId &&
        (message.offloadTaskId->empty() || message.offloadTaskId->size() > 255U)) {
        throw std::invalid_argument("offloadTaskId exceeds ASN.1 size constraint");
    }

    const bool hasServicePayload = message.planning || message.perception || message.control;

    BitWriter writer;
    writer.write_bit(false); // No extension additions in profile version 1.
    writer.write_bit(message.requestedHorizonMs.has_value());
    writer.write_bit(message.confidence.has_value());
    writer.write_bit(message.expirationTimeDs.has_value());
    writer.write_bit(hasServicePayload);
    writer.write_bit(message.offloadPayload.has_value());
    writer.write_bit(message.offloadTaskId.has_value());

    writer.write_constrained(profile.profileVersion, 1, 255);
    writer.write_octets(message.sessionId.data(), message.sessionId.size());
    writer.write_constrained(static_cast<std::int64_t>(message.vehicleId.size()), 1, 16);
    writer.write_octets(message.vehicleId);
    writer.write_constrained(static_cast<std::uint8_t>(message.serviceClass), 0, 2);
    writer.write_constrained(static_cast<std::uint8_t>(message.guidanceStatus), 0, 3);

    if (message.requestedHorizonMs) {
        writer.write_constrained(*message.requestedHorizonMs, 0, 60000);
    }
    if (message.confidence) writer.write_constrained(*message.confidence, 0, 100);
    if (message.expirationTimeDs) {
        writer.write_constrained(*message.expirationTimeDs,
                                 0,
                                 std::numeric_limits<std::uint32_t>::max());
    }

    if (hasServicePayload) {
        if (message.planning) {
            writer.write_constrained(0, 0, 2);
            write_planning(writer, *message.planning);
        } else if (message.perception) {
            writer.write_constrained(1, 0, 2);
            write_perception(writer, *message.perception);
        } else {
            writer.write_constrained(2, 0, 2);
            write_control(writer, *message.control);
        }
    }

    if (message.offloadPayload) {
        writer.write_constrained(static_cast<std::int64_t>(message.offloadPayload->size()), 0, 2048);
        writer.write_octets(*message.offloadPayload);
    }
    if (message.offloadTaskId) {
        writer.write_constrained(static_cast<std::int64_t>(message.offloadTaskId->size()), 1, 255);
        writer.write_octets(reinterpret_cast<const std::uint8_t*>(message.offloadTaskId->data()),
                            message.offloadTaskId->size());
    }
    return writer.finish();
}

CooperativeServiceMessage decode_regional_value_impl(
    const std::vector<std::uint8_t>& encoded,
    const J2735IpiRegionalProfile& profile) {
    if (encoded.empty()) throw std::runtime_error("empty IPI regional value");
    BitReader reader(encoded);
    if (reader.read_bit()) throw std::runtime_error("unsupported IPI ASN.1 extension addition");

    const bool hasHorizon = reader.read_bit();
    const bool hasConfidence = reader.read_bit();
    const bool hasExpiration = reader.read_bit();
    const bool hasServicePayload = reader.read_bit();
    const bool hasOffloadPayload = reader.read_bit();
    const bool hasOffloadTaskId = reader.read_bit();

    const auto profileVersion = static_cast<std::uint8_t>(reader.read_constrained(1, 255));
    if (profileVersion != profile.profileVersion) {
        throw std::runtime_error("unsupported IPI regional profile version");
    }

    CooperativeServiceMessage message;
    const auto sessionId = reader.read_octets(message.sessionId.size());
    std::copy(sessionId.begin(), sessionId.end(), message.sessionId.begin());
    const auto vehicleIdSize = static_cast<std::size_t>(reader.read_constrained(1, 16));
    message.vehicleId = reader.read_octets(vehicleIdSize);
    message.serviceClass = static_cast<ServiceClass>(reader.read_constrained(0, 2));
    message.guidanceStatus = static_cast<GuidanceStatus>(reader.read_constrained(0, 3));

    if (hasHorizon) {
        message.requestedHorizonMs = static_cast<std::uint16_t>(reader.read_constrained(0, 60000));
    }
    if (hasConfidence) {
        message.confidence = static_cast<std::uint8_t>(reader.read_constrained(0, 100));
    }
    if (hasExpiration) {
        message.expirationTimeDs = static_cast<std::uint32_t>(reader.read_constrained(
            0, std::numeric_limits<std::uint32_t>::max()));
    }

    if (hasServicePayload) {
        switch (reader.read_constrained(0, 2)) {
            case 0:
                message.planning = read_planning(reader);
                break;
            case 1:
                message.perception = read_perception(reader);
                break;
            case 2:
                message.control = read_control(reader);
                break;
            default:
                throw std::logic_error("unreachable IPI payload choice");
        }
    }

    if (hasOffloadPayload) {
        const auto size = static_cast<std::size_t>(reader.read_constrained(0, 2048));
        message.offloadPayload = reader.read_octets(size);
    }
    if (hasOffloadTaskId) {
        const auto size = static_cast<std::size_t>(reader.read_constrained(1, 255));
        const auto bytes = reader.read_octets(size);
        message.offloadTaskId = std::string(bytes.begin(), bytes.end());
    }

    reader.require_zero_padding();
    message.validate();
    return message;
}

std::vector<std::uint8_t> encode_test_message(
    const std::vector<std::uint8_t>& regionalValue,
    const J2735IpiRegionalProfile& profile) {
    BitWriter writer;
    writer.write_bit(false); // TestMessage00 extension marker.
    writer.write_bit(false); // Header absent.
    writer.write_bit(true);  // Regional value present.
    writer.write_constrained(profile.regionId, 0, 255);
    writer.write_open_type(regionalValue);
    return writer.finish();
}

std::vector<std::uint8_t> decode_test_message(
    const std::vector<std::uint8_t>& encoded,
    const J2735IpiRegionalProfile& profile) {
    BitReader reader(encoded);
    if (reader.read_bit()) throw std::runtime_error("unsupported TestMessage00 extension addition");
    const bool hasHeader = reader.read_bit();
    const bool hasRegional = reader.read_bit();
    if (hasHeader) throw std::runtime_error("IPI TestMessage00 profile requires an absent header");
    if (!hasRegional) throw std::runtime_error("IPI TestMessage00 regional value is missing");
    const auto regionId = static_cast<std::uint8_t>(reader.read_constrained(0, 255));
    if (regionId != profile.regionId) throw std::runtime_error("unexpected IPI RegionId");
    auto regionalValue = reader.read_open_type();
    reader.require_zero_padding();
    return regionalValue;
}

std::vector<std::uint8_t> encode_frame(const std::vector<std::uint8_t>& testMessage) {
    BitWriter writer;
    writer.write_bit(false); // MessageFrame extension marker.
    writer.write_constrained(J2735IpiRegionalProfile::kTestMessageId, 0, 32767);
    writer.write_open_type(testMessage);
    return writer.finish();
}

std::vector<std::uint8_t> decode_frame(const std::vector<std::uint8_t>& encoded) {
    BitReader reader(encoded);
    if (reader.read_bit()) throw std::runtime_error("unsupported MessageFrame extension addition");
    const auto messageId = static_cast<std::uint16_t>(reader.read_constrained(0, 32767));
    if (messageId != J2735IpiRegionalProfile::kTestMessageId) {
        throw std::runtime_error("MessageFrame is not TestMessage00");
    }
    auto testMessage = reader.read_open_type();
    reader.require_zero_padding();
    return testMessage;
}

} // namespace

void J2735IpiRegionalProfile::validate() const {
    if (regionId < 128U) throw std::invalid_argument("IPI RegionId must use the local range 128..255");
    if (profileVersion != kSupportedProfileVersion) {
        throw std::invalid_argument("unsupported IPI regional profile version");
    }
    if (maxFrameBytes < 8U || maxFrameBytes >= 16384U) {
        throw std::invalid_argument("IPI regional frame limit must be in [8, 16383]");
    }
}

J2735IpiRegionalCodec::J2735IpiRegionalCodec(J2735IpiRegionalProfile profile)
    : profile_(std::move(profile)) {
    profile_.validate();
}

const J2735IpiRegionalProfile& J2735IpiRegionalCodec::profile() const noexcept {
    return profile_;
}

std::vector<std::uint8_t> J2735IpiRegionalCodec::encode_regional_value(
    const CooperativeServiceMessage& message) const {
    auto encoded = encode_regional_value_impl(message, profile_);
    if (encoded.size() > profile_.maxFrameBytes) {
        throw std::invalid_argument("IPI regional value exceeds configured frame limit");
    }
    return encoded;
}

CooperativeServiceMessage J2735IpiRegionalCodec::decode_regional_value(
    const std::vector<std::uint8_t>& encoded) const {
    if (encoded.size() > profile_.maxFrameBytes) {
        throw std::runtime_error("IPI regional value exceeds configured frame limit");
    }
    return decode_regional_value_impl(encoded, profile_);
}

std::vector<std::uint8_t> J2735IpiRegionalCodec::encode_message_frame(
    const CooperativeServiceMessage& message) const {
    const auto regionalValue = encode_regional_value_impl(message, profile_);
    const auto testMessage = encode_test_message(regionalValue, profile_);
    auto frame = encode_frame(testMessage);
    if (frame.size() > profile_.maxFrameBytes) {
        throw std::invalid_argument("J2735 IPI MessageFrame exceeds configured frame limit");
    }
    return frame;
}

CooperativeServiceMessage J2735IpiRegionalCodec::decode_message_frame(
    const std::vector<std::uint8_t>& encoded) const {
    if (encoded.empty()) throw std::runtime_error("empty J2735 IPI MessageFrame");
    if (encoded.size() > profile_.maxFrameBytes) {
        throw std::runtime_error("J2735 IPI MessageFrame exceeds configured frame limit");
    }
    const auto testMessage = decode_frame(encoded);
    const auto regionalValue = decode_test_message(testMessage, profile_);
    return decode_regional_value_impl(regionalValue, profile_);
}

} // namespace ipi::v2x
