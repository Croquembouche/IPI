#pragma once

#include "ipi/core/ipi_cooperative_service.hpp"
#include "ipi/core/message_frame.hpp"
#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace ipi::api {

struct Private5gProbeRequest {
    std::uint64_t sequence{0};
    std::uint64_t clientSendTimeNs{0};
    std::uint64_t expiresAtUnixNs{0};
    std::string messageId{};
    std::string runId{};
    std::string conditionId{};
    std::string conditionLabel{};
    std::string requestId{};
    std::string serviceType{};
    std::string intersectionId{};
    std::string sourceId{};
    std::optional<std::string> sessionId{};
    std::string networkLoadLevel{};
    std::string qosProfile{};
    std::string mobilityState{};
    std::string clockSyncState{};
    MessageFrame frame{};
};

struct Private5gProbeAck {
    std::uint64_t sequence{0};
    std::uint64_t clientSendTimeNs{0};
    std::uint64_t serverReceiveTimeNs{0};
    std::uint64_t serverSendTimeNs{0};
    std::uint64_t serverProcessingElapsedNs{0};
    std::string responseId{};
    std::string correlationId{};
    std::string requestMessageId{};
    std::string requestId{};
    std::optional<std::string> sessionId{};
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

enum class Private5gProbeAckDisposition {
    MATCHED,
    MISMATCHED_MESSAGE,
    MISMATCHED_REQUEST,
    MISMATCHED_SESSION,
    MISMATCHED_SEQUENCE,
    MISMATCHED_RESULT,
    MISMATCHED_PAYLOAD,
    DUPLICATE,
    STALE,
    LATE
};

struct Private5gProbeAckValidation {
    Private5gProbeAckDisposition disposition{Private5gProbeAckDisposition::MATCHED};
    std::string detail{};

    [[nodiscard]] bool matched() const noexcept {
        return disposition == Private5gProbeAckDisposition::MATCHED;
    }
};

class Private5gProbeResponseTracker {
public:
    [[nodiscard]] Private5gProbeAckValidation validate(
        const Private5gProbeRequest& request,
        const Private5gProbeAck& acknowledgement,
        bool arrivedAfterDeadline = false,
        std::uint64_t currentUnixTimeNs = 0);

    void reset();

private:
    std::unordered_set<std::string> completedResponseIds_{};
};

[[nodiscard]] MessageFrame make_private_5g_probe_frame(const j2735::BasicSafetyMessage& message,
                                                       const v2x::UperCodec& codec);
[[nodiscard]] MessageFrame make_private_5g_probe_frame(const j2735::PersonalSafetyMessage& message,
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

[[nodiscard]] Private5gProbeAck make_private_5g_probe_ack(
    const Private5gProbeRequest& request,
    std::uint64_t serverReceiveTimeNs,
    std::uint64_t serverSendTimeNs,
    std::uint64_t serverProcessingElapsedNs,
    bool accepted,
    std::string detail,
    std::string responseId);

[[nodiscard]] Private5gProbeAckValidation validate_private_5g_probe_ack(
    const Private5gProbeRequest& request,
    const Private5gProbeAck& acknowledgement,
    const std::unordered_set<std::string>& completedResponseIds = {},
    bool arrivedAfterDeadline = false,
    std::uint64_t currentUnixTimeNs = 0);

[[nodiscard]] std::string to_string(Private5gProbeAckDisposition disposition);

[[nodiscard]] Private5gLatencyMetrics compute_private_5g_latency_metrics(
    const Private5gProbeAck& ack,
    std::int64_t localRoundTripNs,
    bool oneWayClockSynchronized = false,
    std::optional<std::uint64_t> clientReceiveWallTimeNs = {});

} // namespace ipi::api
