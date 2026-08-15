#include "ipi/api/session_wire_codec.hpp"

#include "ipi/api/private_session_transport.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace ipi::api {
namespace {

constexpr std::array<std::uint8_t, 4> kMagic{{'I', 'P', 'I', 'S'}};
constexpr std::uint8_t kVersion = 1;
constexpr std::uint8_t kBodyVersion = 1;

class Writer {
public:
    void u8(std::uint8_t value) { data.push_back(value); }
    void boolean(bool value) { u8(value ? 1U : 0U); }

    void u16(std::uint16_t value) {
        data.push_back(static_cast<std::uint8_t>(value >> 8));
        data.push_back(static_cast<std::uint8_t>(value));
    }

    void u32(std::uint32_t value) {
        for (int shift = 24; shift >= 0; shift -= 8) {
            data.push_back(static_cast<std::uint8_t>(value >> shift));
        }
    }

    void u64(std::uint64_t value) {
        for (int shift = 56; shift >= 0; shift -= 8) {
            data.push_back(static_cast<std::uint8_t>(value >> shift));
        }
    }

    void i32(std::int32_t value) { u32(static_cast<std::uint32_t>(value)); }
    void i64(std::int64_t value) { u64(static_cast<std::uint64_t>(value)); }

    void f64(double value) {
        static_assert(sizeof(double) == sizeof(std::uint64_t), "unsupported double size");
        std::uint64_t bits = 0;
        std::memcpy(&bits, &value, sizeof(bits));
        u64(bits);
    }

    void string(const std::string& value) {
        if (value.size() > std::numeric_limits<std::uint16_t>::max()) {
            throw std::invalid_argument("session wire string exceeds uint16");
        }
        u16(static_cast<std::uint16_t>(value.size()));
        data.insert(data.end(), value.begin(), value.end());
    }

    void bytes(const std::vector<std::uint8_t>& value) {
        if (value.size() > std::numeric_limits<std::uint32_t>::max()) {
            throw std::invalid_argument("session wire byte vector exceeds uint32");
        }
        u32(static_cast<std::uint32_t>(value.size()));
        data.insert(data.end(), value.begin(), value.end());
    }

    void strings(const std::vector<std::string>& values) {
        if (values.size() > std::numeric_limits<std::uint16_t>::max()) {
            throw std::invalid_argument("session wire string vector exceeds uint16");
        }
        u16(static_cast<std::uint16_t>(values.size()));
        for (const auto& value : values) string(value);
    }

    std::vector<std::uint8_t> data{};
};

class Reader {
public:
    explicit Reader(const std::vector<std::uint8_t>& input) : data(input) {}

    std::uint8_t u8() {
        require(1);
        return data[offset++];
    }

    bool boolean() {
        const auto value = u8();
        if (value > 1U) throw std::runtime_error("invalid session wire boolean");
        return value != 0U;
    }

    std::uint16_t u16() {
        require(2);
        const auto value = static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(data[offset]) << 8) | data[offset + 1]);
        offset += 2;
        return value;
    }

    std::uint32_t u32() {
        require(4);
        std::uint32_t value = 0;
        for (int index = 0; index < 4; ++index) value = (value << 8) | data[offset++];
        return value;
    }

    std::uint64_t u64() {
        require(8);
        std::uint64_t value = 0;
        for (int index = 0; index < 8; ++index) value = (value << 8) | data[offset++];
        return value;
    }

    std::int32_t i32() { return static_cast<std::int32_t>(u32()); }
    std::int64_t i64() { return static_cast<std::int64_t>(u64()); }

    double f64() {
        const auto bits = u64();
        double value = 0;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    std::string string() {
        const auto length = u16();
        require(length);
        const auto first = data.begin() + static_cast<std::ptrdiff_t>(offset);
        const auto last = first + static_cast<std::ptrdiff_t>(length);
        offset += length;
        return {first, last};
    }

    std::vector<std::uint8_t> bytes() {
        const auto length = u32();
        require(length);
        const auto first = data.begin() + static_cast<std::ptrdiff_t>(offset);
        const auto last = first + static_cast<std::ptrdiff_t>(length);
        offset += length;
        return {first, last};
    }

    std::vector<std::string> strings() {
        const auto count = u16();
        std::vector<std::string> values;
        values.reserve(count);
        for (std::uint16_t index = 0; index < count; ++index) values.push_back(string());
        return values;
    }

    void finish() const {
        if (offset != data.size()) {
            throw std::runtime_error("session wire payload has trailing bytes");
        }
    }

private:
    void require(std::size_t count) const {
        if (offset > data.size() || count > data.size() - offset) {
            throw std::runtime_error("session wire buffer underrun");
        }
    }

    const std::vector<std::uint8_t>& data;
    std::size_t offset{0};
};

template <typename Enum>
Enum read_enum(Reader& reader, std::uint8_t maximum, const char* label) {
    const auto value = reader.u8();
    if (value > maximum) {
        throw std::runtime_error(std::string("unknown ") + label + " enum value");
    }
    return static_cast<Enum>(value);
}

template <typename Enum>
void validate_enum(Enum value, std::uint8_t maximum, const char* label) {
    if (static_cast<std::uint8_t>(value) > maximum) {
        throw std::invalid_argument(std::string("unknown ") + label + " enum value");
    }
}

std::int64_t timestamp_ns(Timestamp value) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(value.time_since_epoch()).count();
}

Timestamp timestamp_from_ns(std::int64_t value) {
    return Timestamp{std::chrono::duration_cast<Timestamp::duration>(
        std::chrono::nanoseconds(value))};
}

void write_timestamp(Writer& writer, Timestamp value) { writer.i64(timestamp_ns(value)); }
Timestamp read_timestamp(Reader& reader) { return timestamp_from_ns(reader.i64()); }

template <typename T, typename Write>
void write_optional(Writer& writer, const std::optional<T>& value, Write write) {
    writer.boolean(value.has_value());
    if (value) write(*value);
}

template <typename T, typename Read>
std::optional<T> read_optional(Reader& reader, Read read) {
    if (!reader.boolean()) return std::nullopt;
    return read();
}

void write_subscription(Writer& writer, const Subscription& value) {
    writer.string(value.callbackUrl);
    writer.strings(value.eventTypes);
    writer.bytes(value.secret);
}

Subscription read_subscription(Reader& reader) {
    Subscription value;
    value.callbackUrl = reader.string();
    value.eventTypes = reader.strings();
    value.secret = reader.bytes();
    return value;
}

void write_profile(Writer& writer, const VehicleProfile& value) {
    validate_enum(value.role, static_cast<std::uint8_t>(VehicleRole::PEDESTRIAN_DEVICE),
                  "vehicle-role");
    writer.string(value.vehicleId);
    write_optional<std::string>(writer, value.vin,
                                [&](const std::string& item) { writer.string(item); });
    writer.u8(static_cast<std::uint8_t>(value.role));
    write_optional<std::string>(writer, value.oem,
                                [&](const std::string& item) { writer.string(item); });
    write_optional<std::string>(writer, value.softwareVersion,
                                [&](const std::string& item) { writer.string(item); });
    writer.boolean(value.supportsStreaming);
}

VehicleProfile read_profile(Reader& reader) {
    VehicleProfile value;
    value.vehicleId = reader.string();
    value.vin = read_optional<std::string>(reader, [&] { return reader.string(); });
    value.role = read_enum<VehicleRole>(reader,
        static_cast<std::uint8_t>(VehicleRole::PEDESTRIAN_DEVICE), "vehicle-role");
    value.oem = read_optional<std::string>(reader, [&] { return reader.string(); });
    value.softwareVersion = read_optional<std::string>(reader, [&] { return reader.string(); });
    value.supportsStreaming = reader.boolean();
    return value;
}

void write_geo_point(Writer& writer, const GeoPoint& value) {
    writer.f64(value.latitude);
    writer.f64(value.longitude);
    write_optional<double>(writer, value.elevation,
                           [&](double item) { writer.f64(item); });
}

GeoPoint read_geo_point(Reader& reader) {
    GeoPoint value;
    value.latitude = reader.f64();
    value.longitude = reader.f64();
    value.elevation = read_optional<double>(reader, [&] { return reader.f64(); });
    return value;
}

void write_telemetry_frame(Writer& writer, const VehicleTelemetryFrame& value) {
    write_timestamp(writer, value.timestamp);
    write_geo_point(writer, value.pose);
    write_optional<double>(writer, value.speedMps, [&](double item) { writer.f64(item); });
    write_optional<double>(writer, value.accelerationMps2,
                           [&](double item) { writer.f64(item); });
    writer.bytes(value.context);
}

VehicleTelemetryFrame read_telemetry_frame(Reader& reader) {
    VehicleTelemetryFrame value;
    value.timestamp = read_timestamp(reader);
    value.pose = read_geo_point(reader);
    value.speedMps = read_optional<double>(reader, [&] { return reader.f64(); });
    value.accelerationMps2 = read_optional<double>(reader, [&] { return reader.f64(); });
    value.context = reader.bytes();
    return value;
}

void write_service_type(Writer& writer,
                        const std::variant<PCVServiceType, PCAVServiceType>& value) {
    if (std::holds_alternative<PCVServiceType>(value)) {
        validate_enum(std::get<PCVServiceType>(value),
                      static_cast<std::uint8_t>(PCVServiceType::LANE_KEEPING_AID),
                      "pcv-service");
        writer.u8(0);
        writer.u8(static_cast<std::uint8_t>(std::get<PCVServiceType>(value)));
    } else {
        validate_enum(std::get<PCAVServiceType>(value),
                      static_cast<std::uint8_t>(PCAVServiceType::LANE_KEEPING_AID),
                      "pcav-service");
        writer.u8(1);
        writer.u8(static_cast<std::uint8_t>(std::get<PCAVServiceType>(value)));
    }
}

std::variant<PCVServiceType, PCAVServiceType> read_service_type(Reader& reader) {
    const auto family = reader.u8();
    if (family == 0) {
        return read_enum<PCVServiceType>(reader,
            static_cast<std::uint8_t>(PCVServiceType::LANE_KEEPING_AID), "pcv-service");
    }
    if (family == 1) {
        return read_enum<PCAVServiceType>(reader,
            static_cast<std::uint8_t>(PCAVServiceType::LANE_KEEPING_AID), "pcav-service");
    }
    throw std::runtime_error("unknown vehicle service family");
}

void write_location(Writer& writer, const Location& value) {
    writer.f64(value.latitude);
    writer.f64(value.longitude);
    write_optional<double>(writer, value.elevation,
                           [&](double item) { writer.f64(item); });
}

Location read_location(Reader& reader) {
    Location value;
    value.latitude = reader.f64();
    value.longitude = reader.f64();
    value.elevation = read_optional<double>(reader, [&] { return reader.f64(); });
    return value;
}

void start_body(Writer& writer) { writer.u8(kBodyVersion); }
void require_body(Reader& reader) {
    if (reader.u8() != kBodyVersion) {
        throw std::runtime_error("unsupported session payload version");
    }
}

SessionWireMessage make_wire(SessionWireKind kind,
                             std::string topic,
                             EnvelopeMetadata metadata,
                             std::string payloadType,
                             std::vector<std::uint8_t> payload) {
    SessionWireMessage result;
    result.kind = kind;
    result.topic = std::move(topic);
    result.metadata = std::move(metadata);
    result.payloadType = std::move(payloadType);
    result.payload = std::move(payload);
    validate_session_wire_message(result);
    return result;
}

SessionTopicKind topic_kind_for(SessionWireKind kind) {
    switch (kind) {
        case SessionWireKind::REGISTER: return SessionTopicKind::Register;
        case SessionWireKind::HEARTBEAT: return SessionTopicKind::Heartbeat;
        case SessionWireKind::PATCH: return SessionTopicKind::Patch;
        case SessionWireKind::TERMINATE: return SessionTopicKind::Terminate;
        case SessionWireKind::SERVICE_REQUEST: return SessionTopicKind::ServiceRequest;
        case SessionWireKind::SERVICE_UPDATE: return SessionTopicKind::ServiceUpdate;
        case SessionWireKind::EVENT: return SessionTopicKind::Events;
        case SessionWireKind::TELEMETRY: return SessionTopicKind::Telemetry;
        case SessionWireKind::PCV_RESPONSE: return SessionTopicKind::PcvResponse;
        default: throw std::invalid_argument("unknown session wire kind");
    }
}

bool valid_wire_kind(std::uint8_t value) {
    return value >= static_cast<std::uint8_t>(SessionWireKind::REGISTER) &&
           value <= static_cast<std::uint8_t>(SessionWireKind::PCV_RESPONSE);
}

} // namespace

void validate_session_wire_message(const SessionWireMessage& message) {
    if (!valid_wire_kind(static_cast<std::uint8_t>(message.kind))) {
        throw std::invalid_argument("unknown session wire kind");
    }
    if (message.topic.empty() || message.metadata.messageId.empty() ||
        message.metadata.intersectionId.empty() || message.payloadType.empty()) {
        throw std::invalid_argument(
            "session wire topic, messageId, intersectionId, and payloadType are required");
    }
    validate_enum(message.metadata.transport,
                  static_cast<std::uint8_t>(TransportType::HTTP_BACKHAUL), "transport");
    validate_enum(message.metadata.source.type,
                  static_cast<std::uint8_t>(SourceType::INFRASTRUCTURE), "source");

    const auto topic = SessionTopic::parse(message.topic);
    if (topic.kind != topic_kind_for(message.kind)) {
        throw std::invalid_argument("session wire kind does not match topic");
    }
    if (topic.intersectionId != message.metadata.intersectionId) {
        throw std::invalid_argument("session wire intersection does not match topic");
    }
    if (topic.sessionId) {
        if (!message.metadata.sessionId || *topic.sessionId != *message.metadata.sessionId) {
            throw std::invalid_argument("session wire session does not match topic");
        }
    }

    switch (message.kind) {
        case SessionWireKind::REGISTER:
            if (message.payloadType != "session-registration-v1") {
                throw std::invalid_argument("registration wire payload type mismatch");
            }
            break;
        case SessionWireKind::HEARTBEAT:
            if (message.payloadType != "heartbeat-v1") {
                throw std::invalid_argument("heartbeat wire payload type mismatch");
            }
            break;
        case SessionWireKind::PATCH:
            if (message.payloadType != "session-patch-v1") {
                throw std::invalid_argument("patch wire payload type mismatch");
            }
            break;
        case SessionWireKind::TERMINATE:
            if (message.payloadType != "session-termination-v1") {
                throw std::invalid_argument("termination wire payload type mismatch");
            }
            break;
        case SessionWireKind::SERVICE_REQUEST:
            if (message.payloadType != "service-invocation-v1") {
                throw std::invalid_argument("service request wire payload type mismatch");
            }
            break;
        case SessionWireKind::SERVICE_UPDATE:
        case SessionWireKind::PCV_RESPONSE:
            if (message.payloadType != "service-response-v1") {
                throw std::invalid_argument("service response wire payload type mismatch");
            }
            break;
        case SessionWireKind::EVENT:
            if (message.payloadType != "session-descriptor-v1" &&
                message.payloadType != "ack-v1") {
                throw std::invalid_argument("session event wire payload type mismatch");
            }
            break;
        case SessionWireKind::TELEMETRY:
            if (message.payloadType != "telemetry-v1") {
                throw std::invalid_argument("telemetry wire payload type mismatch");
            }
            break;
        default:
            throw std::invalid_argument("unsupported session wire kind");
    }
}

std::vector<std::uint8_t> encode_session_wire_message(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.payload.size() > std::numeric_limits<std::uint32_t>::max()) {
        throw std::invalid_argument("session wire payload exceeds uint32");
    }

    Writer writer;
    writer.data.insert(writer.data.end(), kMagic.begin(), kMagic.end());
    writer.u8(kVersion);
    writer.u8(static_cast<std::uint8_t>(message.kind));
    writer.string(message.topic);
    writer.string(message.metadata.messageId);
    write_timestamp(writer, message.metadata.sentAt);
    writer.string(message.metadata.intersectionId);
    writer.u8(static_cast<std::uint8_t>(message.metadata.transport));
    writer.u8(static_cast<std::uint8_t>(message.metadata.source.type));
    writer.string(message.metadata.source.id);

    std::uint8_t flags = 0;
    if (message.metadata.sessionId) flags |= 0x01U;
    if (message.metadata.correlationId) flags |= 0x02U;
    if (message.metadata.priority) flags |= 0x04U;
    if (message.metadata.sequence) flags |= 0x08U;
    if (message.metadata.expiresAt) flags |= 0x10U;
    writer.u8(flags);
    if (message.metadata.sessionId) writer.string(*message.metadata.sessionId);
    if (message.metadata.correlationId) writer.string(*message.metadata.correlationId);
    if (message.metadata.priority) writer.string(*message.metadata.priority);
    if (message.metadata.sequence) writer.u64(*message.metadata.sequence);
    if (message.metadata.expiresAt) write_timestamp(writer, *message.metadata.expiresAt);

    writer.string(message.payloadType);
    writer.bytes(message.payload);
    return writer.data;
}

SessionWireMessage decode_session_wire_message(const std::vector<std::uint8_t>& buffer) {
    if (buffer.size() < 6 || !std::equal(kMagic.begin(), kMagic.end(), buffer.begin())) {
        throw std::runtime_error("invalid session wire magic");
    }
    Reader reader(buffer);
    for (const auto expected : kMagic) {
        if (reader.u8() != expected) throw std::runtime_error("invalid session wire magic");
    }
    if (reader.u8() != kVersion) throw std::runtime_error("unsupported session wire version");
    const auto kind = reader.u8();
    if (!valid_wire_kind(kind)) throw std::runtime_error("unknown session wire kind");

    SessionWireMessage message;
    message.kind = static_cast<SessionWireKind>(kind);
    message.topic = reader.string();
    message.metadata.messageId = reader.string();
    message.metadata.sentAt = read_timestamp(reader);
    message.metadata.intersectionId = reader.string();
    message.metadata.transport = read_enum<TransportType>(reader,
        static_cast<std::uint8_t>(TransportType::HTTP_BACKHAUL), "transport");
    message.metadata.source.type = read_enum<SourceType>(reader,
        static_cast<std::uint8_t>(SourceType::INFRASTRUCTURE), "source");
    message.metadata.source.id = reader.string();

    const auto flags = reader.u8();
    if ((flags & 0xE0U) != 0U) throw std::runtime_error("unknown session wire metadata flags");
    if (flags & 0x01U) message.metadata.sessionId = reader.string();
    if (flags & 0x02U) message.metadata.correlationId = reader.string();
    if (flags & 0x04U) message.metadata.priority = reader.string();
    if (flags & 0x08U) message.metadata.sequence = reader.u64();
    if (flags & 0x10U) message.metadata.expiresAt = read_timestamp(reader);
    message.payloadType = reader.string();
    message.payload = reader.bytes();
    reader.finish();
    validate_session_wire_message(message);
    return message;
}

SessionWireMessage make_session_registration_wire_message(
    const std::string& topic, const SessionRegistration& registration) {
    if (registration.vehicleProfile.vehicleId.empty() ||
        registration.metadata.source.id != registration.vehicleProfile.vehicleId) {
        throw std::invalid_argument(
            "registration source identity must match the non-empty vehicle identity");
    }
    Writer writer;
    start_body(writer);
    write_profile(writer, registration.vehicleProfile);
    writer.strings(registration.requestedServices);
    write_optional<std::string>(writer, registration.rsuFallback,
                                [&](const std::string& value) { writer.string(value); });
    write_optional<int>(writer, registration.minSidelinkRssi,
                        [&](int value) { writer.i32(value); });
    write_optional<Subscription>(writer, registration.inlineSubscription,
                                 [&](const Subscription& value) {
                                     write_subscription(writer, value);
                                 });
    return make_wire(SessionWireKind::REGISTER, topic, registration.metadata,
                     "session-registration-v1", std::move(writer.data));
}

SessionRegistration decode_session_registration_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::REGISTER) {
        throw std::invalid_argument("wire message is not a session registration");
    }
    Reader reader(message.payload);
    require_body(reader);
    SessionRegistration value;
    value.metadata = message.metadata;
    value.vehicleProfile = read_profile(reader);
    value.requestedServices = reader.strings();
    value.rsuFallback = read_optional<std::string>(reader, [&] { return reader.string(); });
    value.minSidelinkRssi = read_optional<int>(reader, [&] { return reader.i32(); });
    value.inlineSubscription = read_optional<Subscription>(reader,
        [&] { return read_subscription(reader); });
    reader.finish();
    if (value.vehicleProfile.vehicleId.empty() ||
        value.metadata.source.id != value.vehicleProfile.vehicleId) {
        throw std::runtime_error(
            "registration source identity does not match vehicle identity");
    }
    return value;
}

SessionWireMessage make_session_descriptor_wire_message(
    const std::string& topic, EnvelopeMetadata metadata,
    const SessionDescriptor& descriptor) {
    const auto parsedTopic = SessionTopic::parse(topic);
    if (!parsedTopic.vehicleId || descriptor.vehicleProfile.vehicleId.empty() ||
        *parsedTopic.vehicleId != descriptor.vehicleProfile.vehicleId) {
        throw std::invalid_argument(
            "descriptor event topic must match the non-empty vehicle identity");
    }
    validate_enum(descriptor.transport,
                  static_cast<std::uint8_t>(TransportType::HTTP_BACKHAUL), "transport");
    validate_enum(descriptor.state, static_cast<std::uint8_t>(SessionState::EXPIRED),
                  "session-state");
    metadata.sessionId = descriptor.sessionId;
    Writer writer;
    start_body(writer);
    writer.string(descriptor.sessionId);
    write_profile(writer, descriptor.vehicleProfile);
    writer.u8(static_cast<std::uint8_t>(descriptor.transport));
    writer.u8(static_cast<std::uint8_t>(descriptor.state));
    writer.u32(descriptor.leaseSeconds);
    writer.u32(descriptor.heartbeatIntervalSeconds);
    writer.strings(descriptor.preferredChannels);
    writer.strings(descriptor.grantedServices);
    write_timestamp(writer, descriptor.registeredAt);
    write_timestamp(writer, descriptor.lastHeartbeatAt);
    write_timestamp(writer, descriptor.expiresAt);
    write_optional<std::string>(writer, descriptor.rsuFallback,
                                [&](const std::string& value) { writer.string(value); });
    write_optional<int>(writer, descriptor.minSidelinkRssi,
                        [&](int value) { writer.i32(value); });
    write_optional<Subscription>(writer, descriptor.inlineSubscription,
                                 [&](const Subscription& value) {
                                     write_subscription(writer, value);
                                 });
    return make_wire(SessionWireKind::EVENT, topic, std::move(metadata),
                     "session-descriptor-v1", std::move(writer.data));
}

SessionDescriptor decode_session_descriptor_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::EVENT ||
        message.payloadType != "session-descriptor-v1") {
        throw std::invalid_argument("wire message is not a session descriptor");
    }
    Reader reader(message.payload);
    require_body(reader);
    SessionDescriptor value;
    value.sessionId = reader.string();
    value.vehicleProfile = read_profile(reader);
    value.transport = read_enum<TransportType>(reader,
        static_cast<std::uint8_t>(TransportType::HTTP_BACKHAUL), "transport");
    value.state = read_enum<SessionState>(reader,
        static_cast<std::uint8_t>(SessionState::EXPIRED), "session-state");
    value.leaseSeconds = reader.u32();
    value.heartbeatIntervalSeconds = reader.u32();
    value.preferredChannels = reader.strings();
    value.grantedServices = reader.strings();
    value.registeredAt = read_timestamp(reader);
    value.lastHeartbeatAt = read_timestamp(reader);
    value.expiresAt = read_timestamp(reader);
    value.rsuFallback = read_optional<std::string>(reader, [&] { return reader.string(); });
    value.minSidelinkRssi = read_optional<int>(reader, [&] { return reader.i32(); });
    value.inlineSubscription = read_optional<Subscription>(reader,
        [&] { return read_subscription(reader); });
    reader.finish();
    if (!message.metadata.sessionId || *message.metadata.sessionId != value.sessionId) {
        throw std::runtime_error("descriptor session identity mismatch");
    }
    const auto topic = SessionTopic::parse(message.topic);
    if (!topic.vehicleId || value.vehicleProfile.vehicleId.empty() ||
        *topic.vehicleId != value.vehicleProfile.vehicleId) {
        throw std::runtime_error("descriptor vehicle identity does not match event topic");
    }
    return value;
}

SessionWireMessage make_heartbeat_wire_message(
    const std::string& topic, EnvelopeMetadata metadata, const HeartbeatUpdate& heartbeat) {
    metadata.sessionId = heartbeat.sessionId;
    Writer writer;
    start_body(writer);
    writer.string(heartbeat.sessionId);
    write_timestamp(writer, heartbeat.timestamp);
    write_optional<VehicleTelemetryFrame>(writer, heartbeat.telemetry,
        [&](const VehicleTelemetryFrame& value) { write_telemetry_frame(writer, value); });
    return make_wire(SessionWireKind::HEARTBEAT, topic, std::move(metadata),
                     "heartbeat-v1", std::move(writer.data));
}

HeartbeatUpdate decode_heartbeat_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::HEARTBEAT) {
        throw std::invalid_argument("wire message is not a heartbeat");
    }
    Reader reader(message.payload);
    require_body(reader);
    HeartbeatUpdate value;
    value.sessionId = reader.string();
    value.timestamp = read_timestamp(reader);
    value.telemetry = read_optional<VehicleTelemetryFrame>(reader,
        [&] { return read_telemetry_frame(reader); });
    reader.finish();
    if (!message.metadata.sessionId || *message.metadata.sessionId != value.sessionId) {
        throw std::runtime_error("heartbeat session identity mismatch");
    }
    return value;
}

SessionWireMessage make_session_patch_wire_message(
    const std::string& topic, EnvelopeMetadata metadata, const SessionPatch& patch) {
    metadata.sessionId = patch.sessionId;
    Writer writer;
    start_body(writer);
    writer.string(patch.sessionId);
    write_optional<VehicleProfile>(writer, patch.profile,
        [&](const VehicleProfile& value) { write_profile(writer, value); });
    write_optional<std::vector<std::string>>(writer, patch.preferredChannels,
        [&](const std::vector<std::string>& value) { writer.strings(value); });
    return make_wire(SessionWireKind::PATCH, topic, std::move(metadata),
                     "session-patch-v1", std::move(writer.data));
}

SessionPatch decode_session_patch_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::PATCH) {
        throw std::invalid_argument("wire message is not a session patch");
    }
    Reader reader(message.payload);
    require_body(reader);
    SessionPatch value;
    value.sessionId = reader.string();
    value.profile = read_optional<VehicleProfile>(reader, [&] { return read_profile(reader); });
    value.preferredChannels = read_optional<std::vector<std::string>>(
        reader, [&] { return reader.strings(); });
    reader.finish();
    if (!message.metadata.sessionId || *message.metadata.sessionId != value.sessionId) {
        throw std::runtime_error("patch session identity mismatch");
    }
    return value;
}

SessionWireMessage make_session_termination_wire_message(
    const std::string& topic, EnvelopeMetadata metadata,
    const SessionTermination& termination) {
    validate_enum(termination.reasonCode,
                  static_cast<std::uint8_t>(TerminationReasonCode::PROTOCOL_ERROR),
                  "termination-reason");
    metadata.sessionId = termination.sessionId;
    Writer writer;
    start_body(writer);
    writer.string(termination.sessionId);
    writer.u8(static_cast<std::uint8_t>(termination.reasonCode));
    writer.string(termination.reason);
    return make_wire(SessionWireKind::TERMINATE, topic, std::move(metadata),
                     "session-termination-v1", std::move(writer.data));
}

SessionTermination decode_session_termination_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::TERMINATE) {
        throw std::invalid_argument("wire message is not a session termination");
    }
    Reader reader(message.payload);
    require_body(reader);
    SessionTermination value;
    value.sessionId = reader.string();
    value.reasonCode = read_enum<TerminationReasonCode>(reader,
        static_cast<std::uint8_t>(TerminationReasonCode::PROTOCOL_ERROR),
        "termination-reason");
    value.reason = reader.string();
    reader.finish();
    if (!message.metadata.sessionId || *message.metadata.sessionId != value.sessionId) {
        throw std::runtime_error("termination session identity mismatch");
    }
    return value;
}

SessionWireMessage make_service_invocation_wire_message(
    const std::string& topic, const ServiceInvocation& invocation) {
    auto metadata = invocation.request.metadata;
    metadata.sessionId = invocation.sessionId;
    Writer writer;
    start_body(writer);
    writer.string(invocation.sessionId);
    write_service_type(writer, invocation.request.data.serviceType);
    writer.string(invocation.request.data.vehicleId);
    write_optional<std::string>(writer, invocation.request.data.vin,
                                [&](const std::string& value) { writer.string(value); });
    write_location(writer, invocation.request.data.location);
    write_optional<double>(writer, invocation.request.data.speedMps,
                           [&](double value) { writer.f64(value); });
    write_optional<double>(writer, invocation.request.data.headingDegrees,
                           [&](double value) { writer.f64(value); });
    writer.bytes(invocation.request.data.context);
    return make_wire(SessionWireKind::SERVICE_REQUEST, topic, std::move(metadata),
                     "service-invocation-v1", std::move(writer.data));
}

ServiceInvocation decode_service_invocation_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::SERVICE_REQUEST) {
        throw std::invalid_argument("wire message is not a service invocation");
    }
    Reader reader(message.payload);
    require_body(reader);
    ServiceInvocation value;
    value.sessionId = reader.string();
    value.request.metadata = message.metadata;
    value.request.data.serviceType = read_service_type(reader);
    value.request.data.vehicleId = reader.string();
    value.request.data.vin = read_optional<std::string>(reader, [&] { return reader.string(); });
    value.request.data.location = read_location(reader);
    value.request.data.speedMps = read_optional<double>(reader, [&] { return reader.f64(); });
    value.request.data.headingDegrees = read_optional<double>(reader, [&] { return reader.f64(); });
    value.request.data.context = reader.bytes();
    reader.finish();
    if (!message.metadata.sessionId || *message.metadata.sessionId != value.sessionId) {
        throw std::runtime_error("service invocation session identity mismatch");
    }
    return value;
}

SessionWireMessage make_service_response_wire_message(
    const std::string& topic, const Envelope<VehicleServiceResponse>& response) {
    validate_vehicle_service_response(response.data);
    Writer writer;
    start_body(writer);
    write_service_type(writer, response.data.serviceType);
    writer.string(response.data.vehicleId);
    writer.u8(static_cast<std::uint8_t>(response.data.status));
    write_optional<Timestamp>(writer, response.data.expiresAt,
                              [&](Timestamp value) { write_timestamp(writer, value); });
    writer.bytes(response.data.guidance);
    writer.string(response.data.resultId);
    writer.u8(static_cast<std::uint8_t>(response.data.failureCode));
    write_optional<FallbackResult>(writer, response.data.fallbackResult,
        [&](FallbackResult value) { writer.u8(static_cast<std::uint8_t>(value)); });
    writer.string(response.data.detail);
    const auto kind = SessionTopic::parse(topic).kind == SessionTopicKind::PcvResponse
                          ? SessionWireKind::PCV_RESPONSE
                          : SessionWireKind::SERVICE_UPDATE;
    return make_wire(kind, topic, response.metadata,
                     "service-response-v1", std::move(writer.data));
}

Envelope<VehicleServiceResponse> decode_service_response_payload(
    const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::SERVICE_UPDATE &&
        message.kind != SessionWireKind::PCV_RESPONSE) {
        throw std::invalid_argument("wire message is not a service response");
    }
    Reader reader(message.payload);
    require_body(reader);
    Envelope<VehicleServiceResponse> value;
    value.metadata = message.metadata;
    value.data.serviceType = read_service_type(reader);
    value.data.vehicleId = reader.string();
    value.data.status = read_enum<VehicleServiceStatus>(reader,
        static_cast<std::uint8_t>(VehicleServiceStatus::FALLBACK_COMPLETED),
        "vehicle-service-status");
    value.data.expiresAt = read_optional<Timestamp>(reader, [&] { return read_timestamp(reader); });
    value.data.guidance = reader.bytes();
    value.data.resultId = reader.string();
    value.data.failureCode = read_enum<FailureCode>(reader,
        static_cast<std::uint8_t>(FailureCode::FALLBACK_FAILED), "failure-code");
    value.data.fallbackResult = read_optional<FallbackResult>(reader, [&] {
        return read_enum<FallbackResult>(reader,
            static_cast<std::uint8_t>(FallbackResult::FAILED), "fallback-result");
    });
    value.data.detail = reader.string();
    reader.finish();
    validate_vehicle_service_response(value.data);
    return value;
}

SessionWireMessage make_telemetry_wire_message(
    const std::string& topic, EnvelopeMetadata metadata,
    const TelemetrySubmission& submission) {
    metadata.sessionId = submission.sessionId;
    Writer writer;
    start_body(writer);
    writer.string(submission.sessionId);
    if (submission.frames.size() > std::numeric_limits<std::uint16_t>::max()) {
        throw std::invalid_argument("telemetry frame count exceeds uint16");
    }
    writer.u16(static_cast<std::uint16_t>(submission.frames.size()));
    for (const auto& frame : submission.frames) write_telemetry_frame(writer, frame);
    return make_wire(SessionWireKind::TELEMETRY, topic, std::move(metadata),
                     "telemetry-v1", std::move(writer.data));
}

TelemetrySubmission decode_telemetry_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::TELEMETRY) {
        throw std::invalid_argument("wire message is not telemetry");
    }
    Reader reader(message.payload);
    require_body(reader);
    TelemetrySubmission value;
    value.sessionId = reader.string();
    const auto count = reader.u16();
    value.frames.reserve(count);
    for (std::uint16_t index = 0; index < count; ++index) {
        value.frames.push_back(read_telemetry_frame(reader));
    }
    reader.finish();
    if (!message.metadata.sessionId || *message.metadata.sessionId != value.sessionId) {
        throw std::runtime_error("telemetry session identity mismatch");
    }
    return value;
}

SessionWireMessage make_ack_wire_message(
    const std::string& topic, EnvelopeMetadata metadata, const Ack& acknowledgement) {
    validate_enum(acknowledgement.code,
                  static_cast<std::uint8_t>(FailureCode::FALLBACK_FAILED), "failure-code");
    if (acknowledgement.fallbackResult) {
        validate_enum(*acknowledgement.fallbackResult,
                      static_cast<std::uint8_t>(FallbackResult::FAILED),
                      "fallback-result");
    }
    if (acknowledgement.accepted && acknowledgement.code != FailureCode::NONE) {
        throw std::invalid_argument("accepted acknowledgement cannot carry a failure code");
    }
    if (!acknowledgement.accepted && acknowledgement.code == FailureCode::NONE) {
        throw std::invalid_argument("rejected acknowledgement requires a failure code");
    }
    Writer writer;
    start_body(writer);
    writer.boolean(acknowledgement.accepted);
    write_optional<std::string>(writer, acknowledgement.id,
                                [&](const std::string& value) { writer.string(value); });
    writer.u8(static_cast<std::uint8_t>(acknowledgement.code));
    writer.string(acknowledgement.detail);
    write_optional<FallbackResult>(writer, acknowledgement.fallbackResult,
        [&](FallbackResult value) { writer.u8(static_cast<std::uint8_t>(value)); });
    return make_wire(SessionWireKind::EVENT, topic, std::move(metadata),
                     "ack-v1", std::move(writer.data));
}

Ack decode_ack_payload(const SessionWireMessage& message) {
    validate_session_wire_message(message);
    if (message.kind != SessionWireKind::EVENT || message.payloadType != "ack-v1") {
        throw std::invalid_argument("wire message is not an acknowledgement");
    }
    Reader reader(message.payload);
    require_body(reader);
    Ack value;
    value.accepted = reader.boolean();
    value.id = read_optional<std::string>(reader, [&] { return reader.string(); });
    value.code = read_enum<FailureCode>(reader,
        static_cast<std::uint8_t>(FailureCode::FALLBACK_FAILED), "failure-code");
    value.detail = reader.string();
    value.fallbackResult = read_optional<FallbackResult>(reader, [&] {
        return read_enum<FallbackResult>(reader,
            static_cast<std::uint8_t>(FallbackResult::FAILED), "fallback-result");
    });
    reader.finish();
    if (value.accepted && value.code != FailureCode::NONE) {
        throw std::runtime_error("accepted acknowledgement carries a failure code");
    }
    if (!value.accepted && value.code == FailureCode::NONE) {
        throw std::runtime_error("rejected acknowledgement lacks a failure code");
    }
    return value;
}

} // namespace ipi::api
