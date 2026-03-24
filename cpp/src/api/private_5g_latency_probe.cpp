#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace ipi::api {

namespace {

constexpr std::array<std::uint8_t, 4> kMagic{{'I', '5', 'G', 'P'}};
constexpr std::uint8_t kVersion = 1;
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
    frame.type = static_cast<MessageType>(read_byte(buffer, offset));
    const auto payloadSize = read_uint32(buffer, offset);
    if (offset + payloadSize > buffer.size()) {
        throw std::runtime_error("frame payload exceeds buffer");
    }
    frame.payload.assign(buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                         buffer.begin() + static_cast<std::ptrdiff_t>(offset + payloadSize));
    offset += payloadSize;
    return frame;
}

void validate_prefix(const std::vector<std::uint8_t>& buffer, std::uint8_t expectedRecordKind) {
    if (buffer.size() < 6) {
        throw std::runtime_error("probe packet too small");
    }
    if (!std::equal(kMagic.begin(), kMagic.end(), buffer.begin())) {
        throw std::runtime_error("invalid probe packet magic");
    }
    if (buffer[4] != kVersion) {
        throw std::runtime_error("unsupported probe packet version");
    }
    if (buffer[5] != expectedRecordKind) {
        throw std::runtime_error("unexpected probe packet kind");
    }
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
        const auto sent = ::send(socketFd, data + offset, size - offset, 0);
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
        case MessageType::MAP:
            return codec.decode_map(frame.payload).to_string();
        case MessageType::SPAT:
            return codec.decode_spat(frame.payload).to_string();
        case MessageType::SRM:
            return codec.decode_srm(frame.payload).to_string();
        case MessageType::SSM:
            return codec.decode_ssm(frame.payload).to_string();
        case MessageType::IpiCooperativeService: {
            auto message = CooperativeServiceMessage::from_canonical_encoding(frame.payload);
            message.validate();
            return message.to_string();
        }
        default:
            throw std::invalid_argument("unsupported probe frame type");
    }
}

std::vector<std::uint8_t> encode_private_5g_probe_request(const Private5gProbeRequest& request) {
    std::vector<std::uint8_t> buffer;
    buffer.reserve(64 + request.frame.payload.size());
    buffer.insert(buffer.end(), kMagic.begin(), kMagic.end());
    buffer.push_back(kVersion);
    buffer.push_back(kRequestRecord);
    append_uint64(buffer, request.sequence);
    append_uint64(buffer, request.clientSendTimeNs);
    append_string(buffer, request.intersectionId);
    append_string(buffer, request.sourceId);
    append_optional_string(buffer, request.sessionId);
    append_frame(buffer, request.frame);
    return buffer;
}

Private5gProbeRequest decode_private_5g_probe_request(const std::vector<std::uint8_t>& buffer) {
    validate_prefix(buffer, kRequestRecord);
    std::size_t offset = 6;

    Private5gProbeRequest request;
    request.sequence = read_uint64(buffer, offset);
    request.clientSendTimeNs = read_uint64(buffer, offset);
    request.intersectionId = read_string(buffer, offset);
    request.sourceId = read_string(buffer, offset);
    request.sessionId = read_optional_string(buffer, offset);
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
    buffer.push_back(kVersion);
    buffer.push_back(kAckRecord);
    append_uint64(buffer, ack.sequence);
    append_uint64(buffer, ack.clientSendTimeNs);
    append_uint64(buffer, ack.serverReceiveTimeNs);
    append_uint64(buffer, ack.serverSendTimeNs);
    buffer.push_back(static_cast<std::uint8_t>(ack.frameType));
    append_uint32(buffer, ack.payloadSize);
    buffer.push_back(ack.accepted ? 1U : 0U);
    append_string(buffer, ack.detail);
    return buffer;
}

Private5gProbeAck decode_private_5g_probe_ack(const std::vector<std::uint8_t>& buffer) {
    validate_prefix(buffer, kAckRecord);
    std::size_t offset = 6;

    Private5gProbeAck ack;
    ack.sequence = read_uint64(buffer, offset);
    ack.clientSendTimeNs = read_uint64(buffer, offset);
    ack.serverReceiveTimeNs = read_uint64(buffer, offset);
    ack.serverSendTimeNs = read_uint64(buffer, offset);
    ack.frameType = static_cast<MessageType>(read_byte(buffer, offset));
    ack.payloadSize = read_uint32(buffer, offset);
    ack.accepted = read_byte(buffer, offset) != 0;
    ack.detail = read_string(buffer, offset);

    if (offset != buffer.size()) {
        throw std::runtime_error("unexpected trailing bytes in probe ack");
    }
    return ack;
}

void send_private_5g_probe_packet(int socketFd, const std::vector<std::uint8_t>& packet) {
    const auto packetSize = static_cast<std::uint32_t>(packet.size());
    const auto packetSizeBe = htonl(packetSize);
    send_exact(socketFd, &packetSizeBe, sizeof(packetSizeBe));
    if (!packet.empty()) {
        send_exact(socketFd, packet.data(), packet.size());
    }
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

Private5gLatencyMetrics compute_private_5g_latency_metrics(const Private5gProbeAck& ack,
                                                           std::uint64_t clientReceiveTimeNs) {
    Private5gLatencyMetrics metrics;
    metrics.roundTripNs = static_cast<std::int64_t>(clientReceiveTimeNs - ack.clientSendTimeNs);
    metrics.serverProcessingNs = static_cast<std::int64_t>(ack.serverSendTimeNs - ack.serverReceiveTimeNs);

    const auto uplink = static_cast<std::int64_t>(ack.serverReceiveTimeNs - ack.clientSendTimeNs);
    const auto downlink = static_cast<std::int64_t>(clientReceiveTimeNs - ack.serverSendTimeNs);
    if (uplink >= 0) {
        metrics.uplinkNs = uplink;
    }
    if (downlink >= 0) {
        metrics.downlinkNs = downlink;
    }
    return metrics;
}

} // namespace ipi::api
