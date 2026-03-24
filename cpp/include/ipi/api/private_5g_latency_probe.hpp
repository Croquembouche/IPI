#pragma once

#include "ipi/core/ipi_cooperative_service.hpp"
#include "ipi/core/message_frame.hpp"
#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ipi::api {

struct Private5gProbeRequest {
    std::uint64_t sequence{0};
    std::uint64_t clientSendTimeNs{0};
    std::string intersectionId{};
    std::string sourceId{};
    std::optional<std::string> sessionId{};
    MessageFrame frame{};
};

struct Private5gProbeAck {
    std::uint64_t sequence{0};
    std::uint64_t clientSendTimeNs{0};
    std::uint64_t serverReceiveTimeNs{0};
    std::uint64_t serverSendTimeNs{0};
    MessageType frameType{MessageType::SPAT};
    std::uint32_t payloadSize{0};
    bool accepted{true};
    std::string detail{};
};

struct Private5gLatencyMetrics {
    std::int64_t roundTripNs{0};
    std::int64_t serverProcessingNs{0};
    std::optional<std::int64_t> uplinkNs{};
    std::optional<std::int64_t> downlinkNs{};
};

[[nodiscard]] MessageFrame make_private_5g_probe_frame(const j2735::BasicSafetyMessage& message,
                                                       const v2x::UperCodec& codec);
[[nodiscard]] MessageFrame make_private_5g_probe_frame(const j2735::MapMessage& message,
                                                       const v2x::UperCodec& codec);
[[nodiscard]] MessageFrame make_private_5g_probe_frame(const j2735::SpatMessage& message,
                                                       const v2x::UperCodec& codec);
[[nodiscard]] MessageFrame make_private_5g_probe_frame(const j2735::SignalRequestMessage& message,
                                                       const v2x::UperCodec& codec);
[[nodiscard]] MessageFrame make_private_5g_probe_frame(const j2735::SignalStatusMessage& message,
                                                       const v2x::UperCodec& codec);
[[nodiscard]] MessageFrame make_private_5g_probe_frame(const CooperativeServiceMessage& message);

[[nodiscard]] std::string inspect_private_5g_probe_frame(const MessageFrame& frame,
                                                         const v2x::UperCodec& codec);

[[nodiscard]] std::vector<std::uint8_t> encode_private_5g_probe_request(
    const Private5gProbeRequest& request);
[[nodiscard]] Private5gProbeRequest decode_private_5g_probe_request(
    const std::vector<std::uint8_t>& buffer);

[[nodiscard]] std::vector<std::uint8_t> encode_private_5g_probe_ack(const Private5gProbeAck& ack);
[[nodiscard]] Private5gProbeAck decode_private_5g_probe_ack(const std::vector<std::uint8_t>& buffer);

void send_private_5g_probe_packet(int socketFd, const std::vector<std::uint8_t>& packet);
[[nodiscard]] std::vector<std::uint8_t> recv_private_5g_probe_packet(int socketFd,
                                                                     std::size_t maxPayloadBytes = 1024U * 1024U);

[[nodiscard]] std::uint64_t current_unix_time_ns();

[[nodiscard]] Private5gLatencyMetrics compute_private_5g_latency_metrics(
    const Private5gProbeAck& ack,
    std::uint64_t clientReceiveTimeNs);

} // namespace ipi::api
