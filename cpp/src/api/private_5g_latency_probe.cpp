#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace ipi::api {

namespace {

constexpr std::array<std::uint8_t, 4> kMagic{{'I', '5', 'G', 'P'}};
constexpr std::uint8_t kVersion1 = 1;
constexpr std::uint8_t kVersion2 = 2;
constexpr std::uint8_t kRequestRecord = 1;
constexpr std::uint8_t kAckRecord = 2;

void append_uint16(std::vector<std::uint8_t>& buffer, std::uint16_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void append_uint32(std::vector<std::uint8_t>& buffer, std::uint32_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void append_uint64(std::vector<std::uint8_t>& buffer, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        buffer.push_back(static_cast<std::uint8_t>((value >> shift) & 0xFF));
    }
}

std::uint8_t read_byte(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset >= buffer.size()) {
        throw std::runtime_error("buffer underrun while reading byte");
    }
    return buffer[offset++];
}

std::uint16_t read_uint16(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 2 > buffer.size()) {
        throw std::runtime_error("buffer underrun while reading uint16");
    }
    const auto hi = static_cast<std::uint16_t>(buffer[offset++]);
    const auto lo = static_cast<std::uint16_t>(buffer[offset++]);
    return static_cast<std::uint16_t>((hi << 8) | lo);
}

std::uint32_t read_uint32(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 4 > buffer.size()) {
        throw std::runtime_error("buffer underrun while reading uint32");
    }
    std::uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        value = (value << 8) | buffer[offset++];
    }
    return value;
}

std::uint64_t read_uint64(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 8 > buffer.size()) {
        throw std::runtime_error("buffer underrun while reading uint64");
    }
    std::uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value = (value << 8) | buffer[offset++];
    }
    return value;
}

void append_string(std::vector<std::uint8_t>& buffer, const std::string& value) {
    if (value.size() > 65535U) {
        throw std::invalid_argument("string field exceeds 65535 bytes");
    }
    append_uint16(buffer, static_cast<std::uint16_t>(value.size()));
    buffer.insert(buffer.end(), value.begin(), value.end());
}

void append_optional_string(std::vector<std::uint8_t>& buffer, const std::optional<std::string>& value) {
    if (!value) {
        append_uint16(buffer, 0xFFFFU);
        return;
    }
    append_string(buffer, *value);
}

std::string read_string(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    const auto length = read_uint16(buffer, offset);
    if (offset + length > buffer.size()) {
        throw std::runtime_error("string length exceeds buffer");
    }
    const auto begin = buffer.begin() + static_cast<std::ptrdiff_t>(offset);
    const auto end = begin + static_cast<std::ptrdiff_t>(length);
    offset += length;
    return std::string(begin, end);
}

std::optional<std::string> read_optional_string(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    const auto length = read_uint16(buffer, offset);
    if (length == 0xFFFFU) {
        return std::nullopt;
    }
    if (offset + length > buffer.size()) {
        throw std::runtime_error("optional string length exceeds buffer");
    }
    const auto begin = buffer.begin() + static_cast<std::ptrdiff_t>(offset);
    const auto end = begin + static_cast<std::ptrdiff_t>(length);
    offset += length;
    return std::string(begin, end);
}

void append_frame(std::vector<std::uint8_t>& buffer, const MessageFrame& frame) {
    if (!frame.annotations.empty()) {
        throw std::invalid_argument("probe frame annotations are not supported");
    }
    if (frame.payload.size() > 0xFFFFFFFFULL) {
        throw std::invalid_argument("probe frame payload exceeds uint32");
    }
    buffer.push_back(static_cast<std::uint8_t>(frame.type));
    append_uint32(buffer, static_cast<std::uint32_t>(frame.payload.size()));
    buffer.insert(buffer.end(), frame.payload.begin(), frame.payload.end());
}

MessageFrame read_frame(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    MessageFrame frame;
    const auto type = read_byte(buffer, offset);
    if (type > static_cast<std::uint8_t>(MessageType::PSM)) {
        throw std::runtime_error("unknown probe frame type");
    }
    frame.type = static_cast<MessageType>(type);
    const auto payloadSize = read_uint32(buffer, offset);
    if (offset + payloadSize > buffer.size()) {
        throw std::runtime_error("frame payload exceeds buffer");
    }
    frame.payload.assign(buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                         buffer.begin() + static_cast<std::ptrdiff_t>(offset + payloadSize));
    offset += payloadSize;
    return frame;
}

std::uint8_t validate_prefix(const std::vector<std::uint8_t>& buffer,
                             std::uint8_t expectedRecordKind) {
    if (buffer.size() < 6) {
        throw std::runtime_error("probe packet too small");
    }
    if (!std::equal(kMagic.begin(), kMagic.end(), buffer.begin())) {
        throw std::runtime_error("invalid probe packet magic");
    }
    if (buffer[4] != kVersion1 && buffer[4] != kVersion2) {
        throw std::runtime_error("unsupported probe packet version");
    }
    if (buffer[5] != expectedRecordKind) {
        throw std::runtime_error("unexpected probe packet kind");
    }
    return buffer[4];
}

MessageFrame make_frame(MessageType type, std::vector<std::uint8_t> payload) {
    MessageFrame frame;
    frame.type = type;
    frame.payload = std::move(payload);
    return frame;
}

void recv_exact(int socketFd, void* destination, std::size_t size) {
    auto* out = static_cast<std::uint8_t*>(destination);
    std::size_t offset = 0;
    while (offset < size) {
        const auto received = ::recv(socketFd, out + offset, size - offset, 0);
        if (received == 0) {
            throw std::runtime_error("peer closed");
        }
        if (received < 0) {
            throw std::runtime_error("recv() failed");
        }
        offset += static_cast<std::size_t>(received);
    }
}

void send_exact(int socketFd, const void* source, std::size_t size) {
    const auto* data = static_cast<const std::uint8_t*>(source);
    std::size_t offset = 0;
    while (offset < size) {
        const auto sent = ::send(socketFd, data + offset, size - offset, MSG_NOSIGNAL);
        if (sent <= 0) {
            throw std::runtime_error("send() failed");
        }
        offset += static_cast<std::size_t>(sent);
    }
}

} // namespace

MessageFrame make_private_5g_probe_frame(const j2735::BasicSafetyMessage& message,
                                         const v2x::UperCodec& codec) {
    return make_frame(MessageType::BSM, codec.encode(message));
}

MessageFrame make_private_5g_probe_frame(const j2735::PersonalSafetyMessage& message,
                                         const v2x::UperCodec& codec) {
    return make_frame(MessageType::PSM, codec.encode(message));
}

MessageFrame make_private_5g_probe_frame(const j2735::MapMessage& message,
                                         const v2x::UperCodec& codec) {
    return make_frame(MessageType::MAP, codec.encode(message));
}

MessageFrame make_private_5g_probe_frame(const j2735::SpatMessage& message,
                                         const v2x::UperCodec& codec) {
    return make_frame(MessageType::SPAT, codec.encode(message));
}

MessageFrame make_private_5g_probe_frame(const j2735::SignalRequestMessage& message,
                                         const v2x::UperCodec& codec) {
    return make_frame(MessageType::SRM, codec.encode(message));
}

MessageFrame make_private_5g_probe_frame(const j2735::SignalStatusMessage& message,
                                         const v2x::UperCodec& codec) {
    return make_frame(MessageType::SSM, codec.encode(message));
}

MessageFrame make_private_5g_probe_frame(const CooperativeServiceMessage& message) {
    return make_frame(MessageType::IpiCooperativeService, message.to_canonical_encoding());
}

std::string inspect_private_5g_probe_frame(const MessageFrame& frame, const v2x::UperCodec& codec) {
    switch (frame.type) {
        case MessageType::BSM:
            return codec.decode_bsm(frame.payload).to_string();
        case MessageType::PSM:
            return codec.decode_psm(frame.payload).to_string();
        case MessageType::MAP:
            return codec.decode_map(frame.payload).to_string();
        case MessageType::SPAT:
            return codec.decode_spat(frame.payload).to_string();
        case MessageType::SRM:
            return codec.decode_srm(frame.payload).to_string();
        case MessageType::SSM:
            return codec.decode_ssm(frame.payload).to_string();
        case MessageType::IpiCooperativeService: {
            auto message = codec.decode_ipi_cooperative_service(frame.payload);
            return message.to_string();
        }
        case MessageType::TIM:
            return "TIM{payloadBytes=" + std::to_string(frame.payload.size()) + "}";
        default:
            throw std::invalid_argument("unsupported probe frame type");
    }
}

std::vector<std::uint8_t> encode_private_5g_probe_request(const Private5gProbeRequest& request) {
    std::vector<std::uint8_t> buffer;
    buffer.reserve(64 + request.frame.payload.size());
    buffer.insert(buffer.end(), kMagic.begin(), kMagic.end());
    buffer.push_back(kVersion2);
    buffer.push_back(kRequestRecord);
    append_uint64(buffer, request.sequence);
    append_uint64(buffer, request.clientSendTimeNs);
    append_uint64(buffer, request.expiresAtUnixNs);
    append_string(buffer, request.messageId);
    append_string(buffer, request.runId);
    append_string(buffer, request.conditionId);
    append_string(buffer, request.conditionLabel);
    append_string(buffer, request.requestId);
    append_string(buffer, request.serviceType);
    append_string(buffer, request.intersectionId);
    append_string(buffer, request.sourceId);
    append_optional_string(buffer, request.sessionId);
    append_string(buffer, request.networkLoadLevel);
    append_string(buffer, request.qosProfile);
    append_string(buffer, request.mobilityState);
    append_string(buffer, request.clockSyncState);
    append_frame(buffer, request.frame);
    return buffer;
}

Private5gProbeRequest decode_private_5g_probe_request(const std::vector<std::uint8_t>& buffer) {
    const auto version = validate_prefix(buffer, kRequestRecord);
    std::size_t offset = 6;

    Private5gProbeRequest request;
    request.sequence = read_uint64(buffer, offset);
    request.clientSendTimeNs = read_uint64(buffer, offset);
    if (version >= kVersion2) {
        request.expiresAtUnixNs = read_uint64(buffer, offset);
        request.messageId = read_string(buffer, offset);
    }
    request.runId = read_string(buffer, offset);
    request.conditionId = read_string(buffer, offset);
    request.conditionLabel = read_string(buffer, offset);
    request.requestId = read_string(buffer, offset);
    request.serviceType = read_string(buffer, offset);
    request.intersectionId = read_string(buffer, offset);
    request.sourceId = read_string(buffer, offset);
    request.sessionId = read_optional_string(buffer, offset);
    request.networkLoadLevel = read_string(buffer, offset);
    request.qosProfile = read_string(buffer, offset);
    request.mobilityState = read_string(buffer, offset);
    request.clockSyncState = read_string(buffer, offset);
    request.frame = read_frame(buffer, offset);

    if (offset != buffer.size()) {
        throw std::runtime_error("unexpected trailing bytes in probe request");
    }
    return request;
}

std::vector<std::uint8_t> encode_private_5g_probe_ack(const Private5gProbeAck& ack) {
    std::vector<std::uint8_t> buffer;
    buffer.reserve(64 + ack.detail.size());
    buffer.insert(buffer.end(), kMagic.begin(), kMagic.end());
    buffer.push_back(kVersion2);
    buffer.push_back(kAckRecord);
    append_uint64(buffer, ack.sequence);
    append_uint64(buffer, ack.clientSendTimeNs);
    append_uint64(buffer, ack.serverReceiveTimeNs);
    append_uint64(buffer, ack.serverSendTimeNs);
    append_uint64(buffer, ack.serverProcessingElapsedNs);
    append_string(buffer, ack.responseId);
    append_string(buffer, ack.correlationId);
    append_string(buffer, ack.requestMessageId);
    append_string(buffer, ack.requestId);
    append_optional_string(buffer, ack.sessionId);
    buffer.push_back(static_cast<std::uint8_t>(ack.frameType));
    append_uint32(buffer, ack.payloadSize);
    buffer.push_back(ack.accepted ? 1U : 0U);
    append_string(buffer, ack.detail);
    return buffer;
}

Private5gProbeAck decode_private_5g_probe_ack(const std::vector<std::uint8_t>& buffer) {
    const auto version = validate_prefix(buffer, kAckRecord);
    std::size_t offset = 6;

    Private5gProbeAck ack;
    ack.sequence = read_uint64(buffer, offset);
    ack.clientSendTimeNs = read_uint64(buffer, offset);
    ack.serverReceiveTimeNs = read_uint64(buffer, offset);
    ack.serverSendTimeNs = read_uint64(buffer, offset);
    if (version >= kVersion2) {
        ack.serverProcessingElapsedNs = read_uint64(buffer, offset);
        ack.responseId = read_string(buffer, offset);
        ack.correlationId = read_string(buffer, offset);
        ack.requestMessageId = read_string(buffer, offset);
        ack.requestId = read_string(buffer, offset);
        ack.sessionId = read_optional_string(buffer, offset);
    } else if (ack.serverSendTimeNs >= ack.serverReceiveTimeNs) {
        ack.serverProcessingElapsedNs = ack.serverSendTimeNs - ack.serverReceiveTimeNs;
    }
    const auto frameType = read_byte(buffer, offset);
    if (frameType > static_cast<std::uint8_t>(MessageType::PSM)) {
        throw std::runtime_error("unknown probe acknowledgement frame type");
    }
    ack.frameType = static_cast<MessageType>(frameType);
    ack.payloadSize = read_uint32(buffer, offset);
    const auto accepted = read_byte(buffer, offset);
    if (accepted > 1U) {
        throw std::runtime_error("invalid probe acknowledgement accepted flag");
    }
    ack.accepted = accepted != 0;
    ack.detail = read_string(buffer, offset);

    if (offset != buffer.size()) {
        throw std::runtime_error("unexpected trailing bytes in probe ack");
    }
    return ack;
}

void send_private_5g_probe_packet(int socketFd, const std::vector<std::uint8_t>& packet) {
    if (packet.empty() || packet.size() > 0xFFFFFFFFULL) {
        throw std::runtime_error("invalid probe packet length");
    }
    const auto packetSize = static_cast<std::uint32_t>(packet.size());
    const auto packetSizeBe = htonl(packetSize);
    send_exact(socketFd, &packetSizeBe, sizeof(packetSizeBe));
    send_exact(socketFd, packet.data(), packet.size());
}

std::vector<std::uint8_t> recv_private_5g_probe_packet(int socketFd, std::size_t maxPayloadBytes) {
    std::uint32_t packetSizeBe = 0;
    recv_exact(socketFd, &packetSizeBe, sizeof(packetSizeBe));
    const auto packetSize = static_cast<std::size_t>(ntohl(packetSizeBe));
    if (packetSize == 0 || packetSize > maxPayloadBytes) {
        throw std::runtime_error("invalid probe packet length");
    }

    std::vector<std::uint8_t> packet(packetSize);
    recv_exact(socketFd, packet.data(), packet.size());
    return packet;
}

std::uint64_t current_unix_time_ns() {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());
}

Private5gProbeAck make_private_5g_probe_ack(
    const Private5gProbeRequest& request,
    std::uint64_t serverReceiveTimeNs,
    std::uint64_t serverSendTimeNs,
    std::uint64_t serverProcessingElapsedNs,
    bool accepted,
    std::string detail,
    std::string responseId) {
    Private5gProbeAck acknowledgement;
    acknowledgement.sequence = request.sequence;
    acknowledgement.clientSendTimeNs = request.clientSendTimeNs;
    acknowledgement.serverReceiveTimeNs = serverReceiveTimeNs;
    acknowledgement.serverSendTimeNs = serverSendTimeNs;
    acknowledgement.serverProcessingElapsedNs = serverProcessingElapsedNs;
    acknowledgement.responseId = std::move(responseId);
    acknowledgement.correlationId = request.requestId;
    acknowledgement.requestMessageId = request.messageId;
    acknowledgement.requestId = request.requestId;
    acknowledgement.sessionId = request.sessionId;
    acknowledgement.frameType = request.frame.type;
    acknowledgement.payloadSize = static_cast<std::uint32_t>(request.frame.payload.size());
    acknowledgement.accepted = accepted;
    acknowledgement.detail = std::move(detail);
    return acknowledgement;
}

Private5gProbeAckValidation validate_private_5g_probe_ack(
    const Private5gProbeRequest& request,
    const Private5gProbeAck& acknowledgement,
    const std::unordered_set<std::string>& completedResponseIds,
    bool arrivedAfterDeadline,
    std::uint64_t currentUnixTimeNs) {
    auto result = [](Private5gProbeAckDisposition disposition, std::string detail) {
        return Private5gProbeAckValidation{disposition, std::move(detail)};
    };
    if (request.messageId.empty() || acknowledgement.requestMessageId != request.messageId) {
        return result(Private5gProbeAckDisposition::MISMATCHED_MESSAGE,
                      "acknowledgement request message identity mismatch");
    }
    if (request.requestId.empty() || acknowledgement.requestId != request.requestId ||
        acknowledgement.correlationId != request.requestId) {
        return result(Private5gProbeAckDisposition::MISMATCHED_REQUEST,
                      "acknowledgement request/correlation identity mismatch");
    }
    if (acknowledgement.sessionId != request.sessionId) {
        return result(Private5gProbeAckDisposition::MISMATCHED_SESSION,
                      "acknowledgement session identity mismatch");
    }
    if (acknowledgement.sequence != request.sequence ||
        acknowledgement.clientSendTimeNs != request.clientSendTimeNs) {
        return result(Private5gProbeAckDisposition::MISMATCHED_SEQUENCE,
                      "acknowledgement sequence or echoed send time mismatch");
    }
    if (acknowledgement.frameType != request.frame.type) {
        return result(Private5gProbeAckDisposition::MISMATCHED_RESULT,
                      "acknowledgement result/frame type mismatch");
    }
    if (acknowledgement.payloadSize != request.frame.payload.size()) {
        return result(Private5gProbeAckDisposition::MISMATCHED_PAYLOAD,
                      "acknowledgement payload size mismatch");
    }
    if (acknowledgement.responseId.empty() ||
        completedResponseIds.find(acknowledgement.responseId) != completedResponseIds.end()) {
        return result(Private5gProbeAckDisposition::DUPLICATE,
                      "missing or duplicate response identity");
    }
    if (currentUnixTimeNs == 0) currentUnixTimeNs = current_unix_time_ns();
    if (request.expiresAtUnixNs != 0 && currentUnixTimeNs >= request.expiresAtUnixNs) {
        return result(Private5gProbeAckDisposition::STALE,
                      "acknowledgement arrived after request expiration");
    }
    if (arrivedAfterDeadline) {
        return result(Private5gProbeAckDisposition::LATE,
                      "acknowledgement arrived after the local monotonic deadline");
    }
    return result(Private5gProbeAckDisposition::MATCHED, "matched");
}

Private5gProbeAckValidation Private5gProbeResponseTracker::validate(
    const Private5gProbeRequest& request,
    const Private5gProbeAck& acknowledgement,
    bool arrivedAfterDeadline,
    std::uint64_t currentUnixTimeNs) {
    auto validation = validate_private_5g_probe_ack(
        request, acknowledgement, completedResponseIds_, arrivedAfterDeadline,
        currentUnixTimeNs);
    if (validation.matched()) completedResponseIds_.insert(acknowledgement.responseId);
    return validation;
}

void Private5gProbeResponseTracker::reset() {
    completedResponseIds_.clear();
}

std::string to_string(Private5gProbeAckDisposition disposition) {
    switch (disposition) {
        case Private5gProbeAckDisposition::MATCHED: return "matched";
        case Private5gProbeAckDisposition::MISMATCHED_MESSAGE: return "mismatched-message";
        case Private5gProbeAckDisposition::MISMATCHED_REQUEST: return "mismatched-request";
        case Private5gProbeAckDisposition::MISMATCHED_SESSION: return "mismatched-session";
        case Private5gProbeAckDisposition::MISMATCHED_SEQUENCE: return "mismatched-sequence";
        case Private5gProbeAckDisposition::MISMATCHED_RESULT: return "mismatched-result";
        case Private5gProbeAckDisposition::MISMATCHED_PAYLOAD: return "mismatched-payload";
        case Private5gProbeAckDisposition::DUPLICATE: return "duplicate";
        case Private5gProbeAckDisposition::STALE: return "stale";
        case Private5gProbeAckDisposition::LATE: return "late";
        default: return "unknown";
    }
}

Private5gLatencyMetrics compute_private_5g_latency_metrics(
    const Private5gProbeAck& ack,
    std::int64_t localRoundTripNs,
    bool oneWayClockSynchronized,
    std::optional<std::uint64_t> clientReceiveWallTimeNs) {
    if (localRoundTripNs < 0) {
        throw std::invalid_argument("local monotonic round-trip duration must be non-negative");
    }
    if (ack.serverProcessingElapsedNs >
        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
        throw std::invalid_argument("server processing duration exceeds int64");
    }
    Private5gLatencyMetrics metrics;
    metrics.roundTripNs = localRoundTripNs;
    metrics.serverProcessingNs = static_cast<std::int64_t>(ack.serverProcessingElapsedNs);

    if (oneWayClockSynchronized && clientReceiveWallTimeNs) {
        if (ack.serverReceiveTimeNs >= ack.clientSendTimeNs &&
            ack.serverReceiveTimeNs - ack.clientSendTimeNs <=
                static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
            metrics.uplinkNs = static_cast<std::int64_t>(
                ack.serverReceiveTimeNs - ack.clientSendTimeNs);
        }
        if (*clientReceiveWallTimeNs >= ack.serverSendTimeNs &&
            *clientReceiveWallTimeNs - ack.serverSendTimeNs <=
                static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
            metrics.downlinkNs = static_cast<std::int64_t>(
                *clientReceiveWallTimeNs - ack.serverSendTimeNs);
        }
    }
    return metrics;
}

} // namespace ipi::api
