#include "ipi/v2x/uper_codec.hpp"

#include "ipi/common/debug.hpp"
#include "ipi/v2x/j2735_ipi_regional_codec.hpp"

#include <cmath>
#include <stdexcept>

namespace {

class BitWriter {
public:
    void write_uint(std::uint64_t value, std::size_t bits) {
        for (std::size_t i = 0; i < bits; ++i) {
            const std::size_t bit_index = bits - 1 - i;
            bool bit = (value >> bit_index) & 0x1;
            write_bit(bit);
        }
    }

    void write_bool(bool value) { write_bit(value); }

    void write_bytes(const std::vector<std::uint8_t>& data) {
        align();
        buffer.insert(buffer.end(), data.begin(), data.end());
    }

    void align() {
        if (bit_offset != 0) {
            current_byte <<= (8 - bit_offset);
            buffer.push_back(current_byte);
            current_byte = 0;
            bit_offset = 0;
        }
    }

    [[nodiscard]] std::vector<std::uint8_t> finish() {
        align();
        return buffer;
    }

private:
    void write_bit(bool value) {
        current_byte = (current_byte << 1) | (value ? 1 : 0);
        ++bit_offset;
        if (bit_offset == 8) {
            buffer.push_back(current_byte);
            current_byte = 0;
            bit_offset = 0;
        }
    }

    std::vector<std::uint8_t> buffer{};
    std::uint8_t current_byte{0};
    std::uint8_t bit_offset{0};
};

class BitReader {
public:
    explicit BitReader(const std::vector<std::uint8_t>& data) : buffer(data) {}

    std::uint64_t read_uint(std::size_t bits) {
        if (bits > 64) {
            throw std::invalid_argument("read_uint cannot exceed 64 bits");
        }
        std::uint64_t value = 0;
        for (std::size_t i = 0; i < bits; ++i) {
            value = (value << 1) | (read_bit() ? 1 : 0);
        }
        return value;
    }

    bool read_bool() { return read_bit(); }

    std::vector<std::uint8_t> read_bytes(std::size_t length) {
        align();
        if (byte_offset + length > buffer.size()) {
            throw std::runtime_error("Buffer underrun reading bytes");
        }
        std::vector<std::uint8_t> out(buffer.begin() + static_cast<std::ptrdiff_t>(byte_offset),
                                      buffer.begin() + static_cast<std::ptrdiff_t>(byte_offset + length));
        byte_offset += length;
        return out;
    }

    void align() {
        if (bit_offset != 0) {
            bit_offset = 0;
            ++byte_offset;
        }
    }

    [[nodiscard]] bool exhausted() const {
        return byte_offset >= buffer.size();
    }

private:
    bool read_bit() {
        if (byte_offset >= buffer.size()) {
            throw std::runtime_error("Buffer underrun reading bit");
        }
        std::uint8_t current = buffer[byte_offset];
        bool bit = (current >> (7 - bit_offset)) & 0x1;
        ++bit_offset;
        if (bit_offset == 8) {
            bit_offset = 0;
            ++byte_offset;
        }
        return bit;
    }

    const std::vector<std::uint8_t>& buffer;
    std::size_t byte_offset{0};
    std::uint8_t bit_offset{0};
};

void write_signed(BitWriter& writer, std::int64_t value, std::size_t bits) {
    if (bits == 0 || bits >= 64) {
        throw std::invalid_argument("write_signed requires between 1 and 63 bits");
    }
    const auto mask = (std::uint64_t{1} << bits) - 1U;
    writer.write_uint(static_cast<std::uint64_t>(value) & mask, bits);
}

std::int64_t read_signed(BitReader& reader, std::size_t bits) {
    if (bits == 0 || bits >= 64) {
        throw std::invalid_argument("read_signed requires between 1 and 63 bits");
    }
    const auto raw = reader.read_uint(bits);
    const auto signBit = std::uint64_t{1} << (bits - 1U);
    if ((raw & signBit) == 0) {
        return static_cast<std::int64_t>(raw);
    }
    return static_cast<std::int64_t>(raw - (std::uint64_t{1} << bits));
}

inline std::int32_t scale_lat(double degrees) {
    return static_cast<std::int32_t>(std::llround(degrees * 1e7));
}

inline double unscale_lat(std::int32_t scaled) {
    return static_cast<double>(scaled) * 1e-7;
}

inline std::uint32_t scale_speed(float speed_mps) {
    return static_cast<std::uint32_t>(std::round(speed_mps / 0.02f));
}

inline float unscale_speed(std::uint32_t scaled) {
    return static_cast<float>(scaled) * 0.02f;
}

inline std::uint32_t scale_heading(float heading_deg) {
    return static_cast<std::uint32_t>(std::round(heading_deg / 0.0125f));
}

inline float unscale_heading(std::uint32_t scaled) {
    return static_cast<float>(scaled) * 0.0125f;
}

inline std::int32_t scale_accel(float accel_mps2) {
    return static_cast<std::int32_t>(std::round(accel_mps2 / 0.01f));
}

inline float unscale_accel(std::int32_t scaled) {
    return static_cast<float>(scaled) * 0.01f;
}

inline std::int32_t scale_tenths(float value) {
    return static_cast<std::int32_t>(std::round(value * 10.0F));
}

inline float unscale_tenths(std::int32_t scaled) {
    return static_cast<float>(scaled) * 0.1F;
}

inline std::uint32_t scale_hundredths(float value) {
    return static_cast<std::uint32_t>(std::round(value * 100.0F));
}

inline float unscale_hundredths(std::uint32_t scaled) {
    return static_cast<float>(scaled) * 0.01F;
}

inline std::uint16_t scale_time_ms(std::uint16_t value) { return value; }
inline std::uint16_t unscale_time_ms(std::uint16_t value) { return value; }

} // namespace

namespace ipi::v2x {

std::vector<std::uint8_t> UperCodec::encode(const j2735::BasicSafetyMessage& msg) const {
    j2735::BasicSafetyMessage copy = msg;
    copy.validate();

    BitWriter writer;
    writer.write_uint(msg.vehicleId, 32);
    writer.write_uint(static_cast<std::uint32_t>(scale_lat(msg.latitude)), 32);
    writer.write_uint(static_cast<std::uint32_t>(scale_lat(msg.longitude)), 32);
    writer.write_uint(scale_speed(msg.speedMps), 16);
    writer.write_uint(scale_heading(msg.headingDeg), 16);
    writer.write_bool(msg.accelerationMps2.has_value());
    if (msg.accelerationMps2) {
        writer.write_uint(static_cast<std::uint32_t>(scale_accel(*msg.accelerationMps2)), 16);
    }
    writer.write_bool(msg.laneId.has_value());
    if (msg.laneId) {
        writer.write_uint(*msg.laneId, 16);
    }
    auto out = writer.finish();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][BSM] encode ", msg.to_string(), " bytes=", out.size(),
                        " hex=", ipi::debug::hex(out));
    }
    return out;
}

j2735::BasicSafetyMessage UperCodec::decode_bsm(const std::vector<std::uint8_t>& buffer) const {
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][BSM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    BitReader reader(buffer);
    j2735::BasicSafetyMessage msg;
    msg.vehicleId = static_cast<std::uint32_t>(reader.read_uint(32));
    msg.latitude = unscale_lat(static_cast<std::int32_t>(reader.read_uint(32)));
    msg.longitude = unscale_lat(static_cast<std::int32_t>(reader.read_uint(32)));
    msg.speedMps = unscale_speed(static_cast<std::uint32_t>(reader.read_uint(16)));
    msg.headingDeg = unscale_heading(static_cast<std::uint32_t>(reader.read_uint(16)));
    if (reader.read_bool()) {
        msg.accelerationMps2 = unscale_accel(static_cast<std::int32_t>(read_signed(reader, 16)));
    }
    if (reader.read_bool()) {
        msg.laneId = static_cast<std::uint16_t>(reader.read_uint(16));
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][BSM] decoded ", msg.to_string());
    }
    return msg;
}

std::vector<std::uint8_t> UperCodec::encode(const j2735::PersonalSafetyMessage& msg) const {
    msg.validate();

    BitWriter writer;
    writer.write_uint(static_cast<std::uint8_t>(msg.basicType), 3);
    writer.write_uint(msg.secondMarkMs, 16);
    writer.write_uint(msg.messageCount, 7);
    writer.write_uint(msg.temporaryId, 32);
    writer.write_uint(static_cast<std::uint32_t>(scale_lat(msg.latitude)), 32);
    writer.write_uint(static_cast<std::uint32_t>(scale_lat(msg.longitude)), 32);
    writer.write_uint(scale_speed(msg.speedMps), 13);
    writer.write_uint(scale_heading(msg.headingDeg), 15);

    writer.write_bool(msg.elevationM.has_value());
    writer.write_bool(msg.horizontalAccuracyM.has_value());
    writer.write_bool(msg.acceleration.has_value());
    writer.write_bool(msg.pathPrediction.has_value());
    writer.write_bool(msg.propulsion.has_value());

    if (msg.elevationM) {
        write_signed(writer, scale_tenths(*msg.elevationM), 17);
    }
    if (msg.horizontalAccuracyM) {
        writer.write_uint(scale_hundredths(*msg.horizontalAccuracyM), 16);
    }
    if (msg.acceleration) {
        write_signed(writer, scale_accel(msg.acceleration->longitudinalMps2), 12);
        write_signed(writer, scale_accel(msg.acceleration->lateralMps2), 12);
        write_signed(writer, scale_accel(msg.acceleration->verticalMps2), 12);
        write_signed(writer, scale_accel(msg.acceleration->yawRateDegPerSec), 16);
    }
    if (msg.pathPrediction) {
        write_signed(writer, msg.pathPrediction->radiusOfCurveM, 16);
        writer.write_uint(msg.pathPrediction->confidence, 7);
    }
    if (msg.propulsion) {
        writer.write_uint(static_cast<std::uint8_t>(msg.propulsion->kind), 2);
        writer.write_uint(msg.propulsion->subtype, 8);
    }

    writer.write_uint(msg.pathHistory.size(), 5);
    for (const auto& point : msg.pathHistory) {
        write_signed(writer, point.latitudeOffset, 16);
        write_signed(writer, point.longitudeOffset, 16);
        write_signed(writer, point.elevationOffset, 16);
        writer.write_uint(point.timeOffsetMs, 16);
    }

    auto out = writer.finish();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][PSM] encode ", msg.to_string(), " bytes=", out.size(),
                        " hex=", ipi::debug::hex(out));
    }
    return out;
}

j2735::PersonalSafetyMessage UperCodec::decode_psm(const std::vector<std::uint8_t>& buffer) const {
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][PSM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }

    BitReader reader(buffer);
    j2735::PersonalSafetyMessage msg;
    msg.basicType = static_cast<j2735::PersonalDeviceUserType>(reader.read_uint(3));
    msg.secondMarkMs = static_cast<std::uint16_t>(reader.read_uint(16));
    msg.messageCount = static_cast<std::uint8_t>(reader.read_uint(7));
    msg.temporaryId = static_cast<std::uint32_t>(reader.read_uint(32));
    msg.latitude = unscale_lat(static_cast<std::int32_t>(reader.read_uint(32)));
    msg.longitude = unscale_lat(static_cast<std::int32_t>(reader.read_uint(32)));
    msg.speedMps = unscale_speed(static_cast<std::uint32_t>(reader.read_uint(13)));
    msg.headingDeg = unscale_heading(static_cast<std::uint32_t>(reader.read_uint(15)));

    const bool hasElevation = reader.read_bool();
    const bool hasAccuracy = reader.read_bool();
    const bool hasAcceleration = reader.read_bool();
    const bool hasPathPrediction = reader.read_bool();
    const bool hasPropulsion = reader.read_bool();

    if (hasElevation) {
        msg.elevationM = unscale_tenths(static_cast<std::int32_t>(read_signed(reader, 17)));
    }
    if (hasAccuracy) {
        msg.horizontalAccuracyM = unscale_hundredths(static_cast<std::uint32_t>(reader.read_uint(16)));
    }
    if (hasAcceleration) {
        j2735::PersonalAccelerationSet values;
        values.longitudinalMps2 = unscale_accel(static_cast<std::int32_t>(read_signed(reader, 12)));
        values.lateralMps2 = unscale_accel(static_cast<std::int32_t>(read_signed(reader, 12)));
        values.verticalMps2 = unscale_accel(static_cast<std::int32_t>(read_signed(reader, 12)));
        values.yawRateDegPerSec = unscale_accel(static_cast<std::int32_t>(read_signed(reader, 16)));
        msg.acceleration = values;
    }
    if (hasPathPrediction) {
        j2735::PersonalPathPrediction prediction;
        prediction.radiusOfCurveM = static_cast<std::int16_t>(read_signed(reader, 16));
        prediction.confidence = static_cast<std::uint8_t>(reader.read_uint(7));
        msg.pathPrediction = prediction;
    }
    if (hasPropulsion) {
        j2735::PersonalPropelledInformation propulsion;
        propulsion.kind = static_cast<j2735::PersonalPropulsionKind>(reader.read_uint(2));
        propulsion.subtype = static_cast<std::uint8_t>(reader.read_uint(8));
        msg.propulsion = propulsion;
    }

    const auto historyCount = static_cast<std::size_t>(reader.read_uint(5));
    msg.pathHistory.reserve(historyCount);
    for (std::size_t i = 0; i < historyCount; ++i) {
        j2735::PersonalPathHistoryPoint point;
        point.latitudeOffset = static_cast<std::int16_t>(read_signed(reader, 16));
        point.longitudeOffset = static_cast<std::int16_t>(read_signed(reader, 16));
        point.elevationOffset = static_cast<std::int16_t>(read_signed(reader, 16));
        point.timeOffsetMs = static_cast<std::uint16_t>(reader.read_uint(16));
        msg.pathHistory.push_back(point);
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][PSM] decoded ", msg.to_string());
    }
    return msg;
}

std::vector<std::uint8_t> UperCodec::encode(const j2735::MapMessage& msg) const {
    j2735::MapMessage copy = msg;
    copy.validate();
    BitWriter writer;
    writer.write_uint(msg.intersectionId, 16);
    writer.write_uint(msg.revision, 8);
    writer.write_bool(msg.name.has_value());
    if (msg.name) {
        writer.write_uint(static_cast<std::uint64_t>(msg.name->size()), 8);
        for (char c : *msg.name) {
            writer.write_uint(static_cast<std::uint8_t>(c), 8);
        }
    }
    writer.write_uint(msg.lanes.size(), 8);
    for (const auto& lane : msg.lanes) {
        writer.write_uint(lane.laneId, 16);
        writer.write_bool(lane.ingress);
    }
    auto out = writer.finish();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][MAP] encode ", msg.to_string(), " bytes=", out.size(),
                        " hex=", ipi::debug::hex(out));
    }
    return out;
}

j2735::MapMessage UperCodec::decode_map(const std::vector<std::uint8_t>& buffer) const {
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][MAP] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    BitReader reader(buffer);
    j2735::MapMessage msg;
    msg.intersectionId = static_cast<std::uint16_t>(reader.read_uint(16));
    msg.revision = static_cast<std::uint8_t>(reader.read_uint(8));
    if (reader.read_bool()) {
        auto len = static_cast<std::size_t>(reader.read_uint(8));
        std::string text;
        text.reserve(len);
        for (std::size_t i = 0; i < len; ++i) {
            text.push_back(static_cast<char>(reader.read_uint(8)));
        }
        msg.name = text;
    }
    auto laneCount = static_cast<std::size_t>(reader.read_uint(8));
    msg.lanes.reserve(laneCount);
    for (std::size_t i = 0; i < laneCount; ++i) {
        j2735::MapLane lane{};
        lane.laneId = static_cast<std::uint16_t>(reader.read_uint(16));
        lane.ingress = reader.read_bool();
        msg.lanes.push_back(lane);
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][MAP] decoded ", msg.to_string());
    }
    return msg;
}

std::vector<std::uint8_t> UperCodec::encode(const j2735::SpatMessage& msg) const {
    j2735::SpatMessage copy = msg;
    copy.validate();
    BitWriter writer;
    writer.write_uint(msg.intersectionId, 16);
    writer.write_uint(msg.timestampMs, 32);
    writer.write_uint(msg.phases.size(), 8);
    for (const auto& phase : msg.phases) {
        writer.write_uint(phase.signalGroup, 8);
        writer.write_uint(static_cast<std::uint8_t>(phase.state), 3);
        writer.write_bool(phase.timeToChangeMs.has_value());
        if (phase.timeToChangeMs) {
            writer.write_uint(scale_time_ms(*phase.timeToChangeMs), 16);
        }
    }
    auto out = writer.finish();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SPaT] encode ", msg.to_string(), " bytes=", out.size(),
                        " hex=", ipi::debug::hex(out));
    }
    return out;
}

j2735::SpatMessage UperCodec::decode_spat(const std::vector<std::uint8_t>& buffer) const {
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SPaT] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    BitReader reader(buffer);
    j2735::SpatMessage msg;
    msg.intersectionId = static_cast<std::uint16_t>(reader.read_uint(16));
    msg.timestampMs = static_cast<std::uint32_t>(reader.read_uint(32));
    auto phaseCount = static_cast<std::size_t>(reader.read_uint(8));
    msg.phases.reserve(phaseCount);
    for (std::size_t i = 0; i < phaseCount; ++i) {
        j2735::SpatPhaseState phase{};
        phase.signalGroup = static_cast<std::uint8_t>(reader.read_uint(8));
        phase.state = static_cast<j2735::MovementPhaseState>(reader.read_uint(3));
        if (reader.read_bool()) {
            phase.timeToChangeMs = unscale_time_ms(static_cast<std::uint16_t>(reader.read_uint(16)));
        }
        msg.phases.push_back(phase);
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SPaT] decoded ", msg.to_string());
    }
    return msg;
}

std::vector<std::uint8_t> UperCodec::encode(const j2735::SignalRequestMessage& msg) const {
    j2735::SignalRequestMessage copy = msg;
    copy.validate();
    BitWriter writer;
    writer.write_uint(msg.requestId, 16);
    writer.write_uint(msg.vehicleId, 32);
    writer.write_uint(msg.intersectionId, 16);
    writer.write_uint(msg.requestedSignalGroup, 8);
    writer.write_bool(msg.estimatedArrivalMs.has_value());
    if (msg.estimatedArrivalMs) {
        writer.write_uint(*msg.estimatedArrivalMs, 32);
    }
    writer.write_bool(msg.priorityLevel.has_value());
    if (msg.priorityLevel) {
        writer.write_uint(*msg.priorityLevel, 3);
    }
    auto out = writer.finish();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SRM] encode ", msg.to_string(), " bytes=", out.size(),
                        " hex=", ipi::debug::hex(out));
    }
    return out;
}

j2735::SignalRequestMessage UperCodec::decode_srm(const std::vector<std::uint8_t>& buffer) const {
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SRM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    BitReader reader(buffer);
    j2735::SignalRequestMessage msg;
    msg.requestId = static_cast<std::uint16_t>(reader.read_uint(16));
    msg.vehicleId = static_cast<std::uint32_t>(reader.read_uint(32));
    msg.intersectionId = static_cast<std::uint16_t>(reader.read_uint(16));
    msg.requestedSignalGroup = static_cast<std::uint8_t>(reader.read_uint(8));
    if (reader.read_bool()) {
        msg.estimatedArrivalMs = static_cast<std::uint32_t>(reader.read_uint(32));
    }
    if (reader.read_bool()) {
        msg.priorityLevel = static_cast<std::uint8_t>(reader.read_uint(3));
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SRM] decoded ", msg.to_string());
    }
    return msg;
}

std::vector<std::uint8_t> UperCodec::encode(const j2735::SignalStatusMessage& msg) const {
    j2735::SignalStatusMessage copy = msg;
    copy.validate();
    BitWriter writer;
    writer.write_uint(msg.requestId, 16);
    writer.write_uint(msg.intersectionId, 16);
    writer.write_uint(msg.grantedSignalGroup, 8);
    writer.write_bool(msg.granted);
    writer.write_bool(msg.estimatedServedTimeMs.has_value());
    if (msg.estimatedServedTimeMs) {
        writer.write_uint(*msg.estimatedServedTimeMs, 16);
    }
    auto out = writer.finish();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SSM] encode ", msg.to_string(), " bytes=", out.size(),
                        " hex=", ipi::debug::hex(out));
    }
    return out;
}

j2735::SignalStatusMessage UperCodec::decode_ssm(const std::vector<std::uint8_t>& buffer) const {
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SSM] decode bytes=", buffer.size(), " hex=", ipi::debug::hex(buffer));
    }
    BitReader reader(buffer);
    j2735::SignalStatusMessage msg;
    msg.requestId = static_cast<std::uint16_t>(reader.read_uint(16));
    msg.intersectionId = static_cast<std::uint16_t>(reader.read_uint(16));
    msg.grantedSignalGroup = static_cast<std::uint8_t>(reader.read_uint(8));
    msg.granted = reader.read_bool();
    if (reader.read_bool()) {
        msg.estimatedServedTimeMs = static_cast<std::uint16_t>(reader.read_uint(16));
    }
    msg.validate();
    if (ipi::debug::enabled()) {
        ipi::debug::log("[UPER][SSM] decoded ", msg.to_string());
    }
    return msg;
}

std::vector<std::uint8_t> UperCodec::encode(const CooperativeServiceMessage& msg) const {
    const J2735IpiRegionalCodec codec;
    auto out = codec.encode_message_frame(msg);
    if (ipi::debug::enabled()) {
        ipi::debug::log("[J2735-UPER][TestMessage00][IPI-CooperativeService] encode ",
                        msg.to_string(),
                        " bytes=", out.size(), " hex=", ipi::debug::hex(out));
    }
    return out;
}

CooperativeServiceMessage UperCodec::decode_ipi_cooperative_service(
    const std::vector<std::uint8_t>& buffer) const {
    if (ipi::debug::enabled()) {
        ipi::debug::log("[J2735-UPER][TestMessage00][IPI-CooperativeService] decode bytes=",
                        buffer.size(),
                        " hex=", ipi::debug::hex(buffer));
    }
    const J2735IpiRegionalCodec codec;
    return codec.decode_message_frame(buffer);
}

} // namespace ipi::v2x
