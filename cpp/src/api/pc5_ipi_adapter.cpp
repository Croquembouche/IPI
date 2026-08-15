#include "ipi/api/pc5_ipi_adapter.hpp"

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

constexpr std::array<std::uint8_t, 4> kMagic{{'I', 'P', '5', 'X'}};
constexpr std::uint8_t kVersion = 1;
constexpr std::uint8_t kRequestKind = 1;
constexpr std::uint8_t kResponseKind = 2;

class Writer {
public:
    void u8(std::uint8_t value) { bytes.push_back(value); }
    void boolean(bool value) { u8(value ? 1U : 0U); }
    void u16(std::uint16_t value) {
        bytes.push_back(static_cast<std::uint8_t>(value >> 8));
        bytes.push_back(static_cast<std::uint8_t>(value));
    }
    void u32(std::uint32_t value) {
        for (int shift = 24; shift >= 0; shift -= 8) {
            bytes.push_back(static_cast<std::uint8_t>(value >> shift));
        }
    }
    void u64(std::uint64_t value) {
        for (int shift = 56; shift >= 0; shift -= 8) {
            bytes.push_back(static_cast<std::uint8_t>(value >> shift));
        }
    }
    void i64(std::int64_t value) { u64(static_cast<std::uint64_t>(value)); }
    void string(const std::string& value) {
        if (value.size() > std::numeric_limits<std::uint16_t>::max()) {
            throw std::invalid_argument("PC5 IPI string exceeds uint16");
        }
        u16(static_cast<std::uint16_t>(value.size()));
        bytes.insert(bytes.end(), value.begin(), value.end());
    }
    void optional_string(const std::optional<std::string>& value) {
        boolean(value.has_value());
        if (value) string(*value);
    }
    void payload(const std::vector<std::uint8_t>& value) {
        if (value.size() > std::numeric_limits<std::uint32_t>::max()) {
            throw std::invalid_argument("PC5 IPI payload exceeds uint32");
        }
        u32(static_cast<std::uint32_t>(value.size()));
        bytes.insert(bytes.end(), value.begin(), value.end());
    }
    std::vector<std::uint8_t> bytes{};
};

class Reader {
public:
    explicit Reader(const std::vector<std::uint8_t>& input, std::size_t dataEnd)
        : bytes(input), end(dataEnd) {}
    std::uint8_t u8() { require(1); return bytes[offset++]; }
    bool boolean() {
        const auto value = u8();
        if (value > 1U) throw std::runtime_error("invalid PC5 IPI boolean");
        return value != 0U;
    }
    std::uint16_t u16() {
        require(2);
        const auto value = static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(bytes[offset]) << 8) | bytes[offset + 1]);
        offset += 2;
        return value;
    }
    std::uint32_t u32() {
        require(4);
        std::uint32_t value = 0;
        for (int index = 0; index < 4; ++index) value = (value << 8) | bytes[offset++];
        return value;
    }
    std::uint64_t u64() {
        require(8);
        std::uint64_t value = 0;
        for (int index = 0; index < 8; ++index) value = (value << 8) | bytes[offset++];
        return value;
    }
    std::int64_t i64() { return static_cast<std::int64_t>(u64()); }
    std::string string() {
        const auto size = u16();
        require(size);
        const auto first = bytes.begin() + static_cast<std::ptrdiff_t>(offset);
        const auto last = first + static_cast<std::ptrdiff_t>(size);
        offset += size;
        return {first, last};
    }
    std::optional<std::string> optional_string() {
        if (!boolean()) return std::nullopt;
        return string();
    }
    std::vector<std::uint8_t> payload() {
        const auto size = u32();
        require(size);
        const auto first = bytes.begin() + static_cast<std::ptrdiff_t>(offset);
        const auto last = first + static_cast<std::ptrdiff_t>(size);
        offset += size;
        return {first, last};
    }
    void finish() const {
        if (offset != end) throw std::runtime_error("PC5 IPI packet has trailing bytes");
    }
private:
    void require(std::size_t size) const {
        if (offset > end || size > end - offset) {
            throw std::runtime_error("PC5 IPI packet is truncated");
        }
    }
    const std::vector<std::uint8_t>& bytes;
    std::size_t end;
    std::size_t offset{0};
};

void validate_limits(Pc5IpiCodecLimits limits) {
    if (limits.maxApplicationBytes == 0 ||
        limits.maxApplicationBytes > kMocarDeployedApplicationHardMaxBytes) {
        throw std::invalid_argument(
            "PC5 IPI maxApplicationBytes must be between 1 and the deployed 4080-byte hard limit");
    }
}

std::int64_t timestamp_ns(Timestamp value) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(value.time_since_epoch()).count();
}

Timestamp timestamp_from_ns(std::int64_t value) {
    return Timestamp{std::chrono::duration_cast<Timestamp::duration>(
        std::chrono::nanoseconds(value))};
}

std::uint32_t crc32(const std::uint8_t* data, std::size_t size) {
    std::uint32_t crc = 0xFFFFFFFFU;
    for (std::size_t index = 0; index < size; ++index) {
        crc ^= data[index];
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ (0xEDB88320U & (0U - (crc & 1U)));
        }
    }
    return ~crc;
}

void append_crc(Writer& writer) {
    const auto value = crc32(writer.bytes.data(), writer.bytes.size());
    writer.u32(value);
}

void validate_packet(const std::vector<std::uint8_t>& packet,
                     Pc5IpiCodecLimits limits,
                     std::uint8_t expectedKind) {
    validate_limits(limits);
    if (packet.size() > limits.maxApplicationBytes) {
        throw std::runtime_error("PC5 IPI packet exceeds configured application limit");
    }
    if (packet.size() < 10 || !std::equal(kMagic.begin(), kMagic.end(), packet.begin())) {
        throw std::runtime_error("invalid PC5 IPI packet magic or length");
    }
    if (packet[4] != kVersion) throw std::runtime_error("unsupported PC5 IPI version");
    if (packet[5] != expectedKind) throw std::runtime_error("unexpected PC5 IPI record kind");
    std::uint32_t stored = 0;
    for (std::size_t index = packet.size() - 4; index < packet.size(); ++index) {
        stored = (stored << 8) | packet[index];
    }
    const auto actual = crc32(packet.data(), packet.size() - 4);
    if (stored != actual) throw std::runtime_error("PC5 IPI CRC mismatch");
}

void begin(Writer& writer, std::uint8_t kind) {
    writer.bytes.insert(writer.bytes.end(), kMagic.begin(), kMagic.end());
    writer.u8(kVersion);
    writer.u8(kind);
}

void skip_prefix(Reader& reader) {
    for (const auto expected : kMagic) {
        if (reader.u8() != expected) throw std::runtime_error("invalid PC5 IPI magic");
    }
    (void)reader.u8();
    (void)reader.u8();
}

void write_j2735(Writer& writer, const J2735Payload& payload) {
    using TypeValue = std::underlying_type_t<J2735MessageType>;
    using EncodingValue = std::underlying_type_t<J2735Encoding>;
    const auto type = static_cast<TypeValue>(payload.type);
    const auto encoding = static_cast<EncodingValue>(payload.encoding);
    if (type < TypeValue{0} ||
        type > static_cast<TypeValue>(J2735MessageType::IPI_COOPERATIVE_SERVICE) ||
        encoding < EncodingValue{0} ||
        encoding > static_cast<EncodingValue>(J2735Encoding::BYTES)) {
        throw std::invalid_argument("unknown PC5 IPI payload enum");
    }
    writer.u8(static_cast<std::uint8_t>(type));
    writer.u8(static_cast<std::uint8_t>(encoding));
    writer.boolean(payload.frameCounter.has_value());
    if (payload.frameCounter) writer.u32(*payload.frameCounter);
    writer.payload(payload.payload);
}

J2735Payload read_j2735(Reader& reader) {
    J2735Payload payload;
    const auto type = reader.u8();
    const auto encoding = reader.u8();
    if (type > static_cast<std::uint8_t>(J2735MessageType::IPI_COOPERATIVE_SERVICE) ||
        encoding > static_cast<std::uint8_t>(J2735Encoding::BYTES)) {
        throw std::runtime_error("unknown PC5 IPI payload enum");
    }
    payload.type = static_cast<J2735MessageType>(type);
    payload.encoding = static_cast<J2735Encoding>(encoding);
    if (reader.boolean()) payload.frameCounter = reader.u32();
    payload.payload = reader.payload();
    return payload;
}

void enforce_encoded_limit(const std::vector<std::uint8_t>& packet,
                           Pc5IpiCodecLimits limits) {
    validate_limits(limits);
    if (packet.size() > limits.maxApplicationBytes) {
        throw std::length_error(
            "encoded PC5 IPI packet exceeds configured application limit");
    }
}

} // namespace

std::vector<std::uint8_t> encode_pc5_ipi_envelope(
    const Pc5IpiEnvelope& envelope, Pc5IpiCodecLimits limits) {
    const auto& metadata = envelope.message.metadata;
    if (metadata.messageId.empty() || !metadata.correlationId || !metadata.sessionId ||
        !metadata.sequence || !metadata.expiresAt || metadata.intersectionId.empty() ||
        metadata.source.id.empty() || envelope.message.data.payload.empty()) {
        throw std::invalid_argument(
            "PC5 IPI request requires message, correlation, session, sequence, expiration, source, intersection, and payload");
    }
    if (*metadata.expiresAt <= metadata.sentAt) {
        throw std::invalid_argument("PC5 IPI expiration must be later than sentAt");
    }
    if (static_cast<std::uint8_t>(metadata.transport) >
            static_cast<std::uint8_t>(TransportType::HTTP_BACKHAUL) ||
        static_cast<std::uint8_t>(metadata.source.type) >
            static_cast<std::uint8_t>(SourceType::INFRASTRUCTURE)) {
        throw std::invalid_argument("unknown PC5 IPI transport or source enum");
    }

    Writer writer;
    begin(writer, kRequestKind);
    writer.string(metadata.messageId);
    writer.string(*metadata.correlationId);
    writer.optional_string(metadata.sessionId);
    writer.u64(*metadata.sequence);
    writer.i64(timestamp_ns(metadata.sentAt));
    writer.i64(timestamp_ns(*metadata.expiresAt));
    writer.string(metadata.intersectionId);
    writer.u8(static_cast<std::uint8_t>(metadata.transport));
    writer.u8(static_cast<std::uint8_t>(metadata.source.type));
    writer.string(metadata.source.id);
    write_j2735(writer, envelope.message.data);
    append_crc(writer);
    enforce_encoded_limit(writer.bytes, limits);
    return writer.bytes;
}

Pc5IpiEnvelope decode_pc5_ipi_envelope(
    const std::vector<std::uint8_t>& packet, Pc5IpiCodecLimits limits) {
    validate_packet(packet, limits, kRequestKind);
    Reader reader(packet, packet.size() - 4);
    skip_prefix(reader);
    Pc5IpiEnvelope envelope;
    auto& metadata = envelope.message.metadata;
    metadata.messageId = reader.string();
    metadata.correlationId = reader.string();
    metadata.sessionId = reader.optional_string();
    metadata.sequence = reader.u64();
    metadata.sentAt = timestamp_from_ns(reader.i64());
    metadata.expiresAt = timestamp_from_ns(reader.i64());
    metadata.intersectionId = reader.string();
    const auto transport = reader.u8();
    const auto source = reader.u8();
    if (transport > static_cast<std::uint8_t>(TransportType::HTTP_BACKHAUL) ||
        source > static_cast<std::uint8_t>(SourceType::INFRASTRUCTURE)) {
        throw std::runtime_error("unknown PC5 IPI transport or source enum");
    }
    metadata.transport = static_cast<TransportType>(transport);
    metadata.source.type = static_cast<SourceType>(source);
    metadata.source.id = reader.string();
    envelope.message.data = read_j2735(reader);
    reader.finish();
    if (metadata.messageId.empty() || !metadata.correlationId ||
        metadata.correlationId->empty() || !metadata.sessionId || metadata.sessionId->empty() ||
        metadata.intersectionId.empty() || metadata.source.id.empty() ||
        envelope.message.data.payload.empty() || *metadata.expiresAt <= metadata.sentAt) {
        throw std::runtime_error("decoded PC5 IPI request violates required-field invariants");
    }
    return envelope;
}

std::vector<std::uint8_t> encode_pc5_ipi_response(
    const Pc5IpiResponse& response, Pc5IpiCodecLimits limits) {
    if (response.responseId.empty() || response.requestMessageId.empty() ||
        response.correlationId.empty() || !response.sessionId || response.sessionId->empty() ||
        response.result.payload.empty()) {
        throw std::invalid_argument("PC5 IPI response is missing required identity or result fields");
    }
    if (response.accepted && response.failureCode != FailureCode::NONE) {
        throw std::invalid_argument("accepted PC5 IPI response cannot carry a failure code");
    }
    if (!response.accepted && response.failureCode == FailureCode::NONE) {
        throw std::invalid_argument("rejected PC5 IPI response requires a failure code");
    }
    if (static_cast<std::uint8_t>(response.failureCode) >
        static_cast<std::uint8_t>(FailureCode::FALLBACK_FAILED)) {
        throw std::invalid_argument("unknown PC5 IPI failure code");
    }

    Writer writer;
    begin(writer, kResponseKind);
    writer.string(response.responseId);
    writer.string(response.requestMessageId);
    writer.string(response.correlationId);
    writer.optional_string(response.sessionId);
    writer.u64(response.sequence);
    writer.i64(timestamp_ns(response.expiresAt));
    writer.boolean(response.accepted);
    writer.u8(static_cast<std::uint8_t>(response.failureCode));
    writer.string(response.detail);
    write_j2735(writer, response.result);
    append_crc(writer);
    enforce_encoded_limit(writer.bytes, limits);
    return writer.bytes;
}

Pc5IpiResponse decode_pc5_ipi_response(
    const std::vector<std::uint8_t>& packet, Pc5IpiCodecLimits limits) {
    validate_packet(packet, limits, kResponseKind);
    Reader reader(packet, packet.size() - 4);
    skip_prefix(reader);
    Pc5IpiResponse response;
    response.responseId = reader.string();
    response.requestMessageId = reader.string();
    response.correlationId = reader.string();
    response.sessionId = reader.optional_string();
    response.sequence = reader.u64();
    response.expiresAt = timestamp_from_ns(reader.i64());
    response.accepted = reader.boolean();
    const auto failure = reader.u8();
    if (failure > static_cast<std::uint8_t>(FailureCode::FALLBACK_FAILED)) {
        throw std::runtime_error("unknown PC5 IPI failure code");
    }
    response.failureCode = static_cast<FailureCode>(failure);
    response.detail = reader.string();
    response.result = read_j2735(reader);
    reader.finish();
    if (response.responseId.empty() || response.requestMessageId.empty() ||
        response.correlationId.empty() || !response.sessionId || response.sessionId->empty() ||
        response.result.payload.empty() ||
        (response.accepted && response.failureCode != FailureCode::NONE) ||
        (!response.accepted && response.failureCode == FailureCode::NONE)) {
        throw std::runtime_error("decoded PC5 IPI response violates required-field invariants");
    }
    return response;
}

std::size_t pc5_ipi_encoded_size(const Pc5IpiEnvelope& envelope,
                                 Pc5IpiCodecLimits limits) {
    return encode_pc5_ipi_envelope(envelope, limits).size();
}

std::size_t pc5_ipi_encoded_size(const Pc5IpiResponse& response,
                                 Pc5IpiCodecLimits limits) {
    return encode_pc5_ipi_response(response, limits).size();
}

Pc5IpiResponseValidation validate_pc5_ipi_response(
    const Pc5IpiEnvelope& request,
    const Pc5IpiResponse& response,
    const std::unordered_set<std::string>& completedResponseIds,
    bool arrivedAfterDeadline,
    std::optional<Timestamp> observedAt) {
    auto result = [](Pc5IpiResponseDisposition disposition, std::string detail) {
        return Pc5IpiResponseValidation{disposition, std::move(detail)};
    };
    const auto& metadata = request.message.metadata;
    if (response.requestMessageId != metadata.messageId) {
        return result(Pc5IpiResponseDisposition::MISMATCHED_MESSAGE,
                      "PC5 response request message identity mismatch");
    }
    if (!metadata.correlationId || response.correlationId != *metadata.correlationId) {
        return result(Pc5IpiResponseDisposition::MISMATCHED_REQUEST,
                      "PC5 response correlation identity mismatch");
    }
    if (response.sessionId != metadata.sessionId) {
        return result(Pc5IpiResponseDisposition::MISMATCHED_SESSION,
                      "PC5 response session identity mismatch");
    }
    if (!metadata.sequence || response.sequence != *metadata.sequence) {
        return result(Pc5IpiResponseDisposition::MISMATCHED_SEQUENCE,
                      "PC5 response sequence mismatch");
    }
    if (response.result.type != request.message.data.type ||
        response.result.encoding != request.message.data.encoding ||
        response.result.frameCounter != request.message.data.frameCounter) {
        return result(Pc5IpiResponseDisposition::MISMATCHED_RESULT,
                      "PC5 response result type, encoding, or frame counter mismatch");
    }
    if (response.responseId.empty() ||
        completedResponseIds.find(response.responseId) != completedResponseIds.end()) {
        return result(Pc5IpiResponseDisposition::DUPLICATE,
                      "PC5 response identity is missing or duplicate");
    }
    const auto now = observedAt.value_or(std::chrono::system_clock::now());
    if ((metadata.expiresAt && now >= *metadata.expiresAt) || now >= response.expiresAt) {
        return result(Pc5IpiResponseDisposition::STALE,
                      "PC5 request or response is expired");
    }
    if (arrivedAfterDeadline) {
        return result(Pc5IpiResponseDisposition::LATE,
                      "PC5 response arrived after the local monotonic deadline");
    }
    return result(Pc5IpiResponseDisposition::MATCHED, "matched");
}

std::string to_string(Pc5IpiResponseDisposition disposition) {
    switch (disposition) {
        case Pc5IpiResponseDisposition::MATCHED: return "matched";
        case Pc5IpiResponseDisposition::MISMATCHED_MESSAGE: return "mismatched-message";
        case Pc5IpiResponseDisposition::MISMATCHED_REQUEST: return "mismatched-request";
        case Pc5IpiResponseDisposition::MISMATCHED_SESSION: return "mismatched-session";
        case Pc5IpiResponseDisposition::MISMATCHED_SEQUENCE: return "mismatched-sequence";
        case Pc5IpiResponseDisposition::MISMATCHED_RESULT: return "mismatched-result";
        case Pc5IpiResponseDisposition::DUPLICATE: return "duplicate";
        case Pc5IpiResponseDisposition::STALE: return "stale";
        case Pc5IpiResponseDisposition::LATE: return "late";
        default: return "unknown";
    }
}

} // namespace ipi::api
