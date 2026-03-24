#pragma once

#include "ipi/api/j2735_payload_codec.hpp"
#include "ipi/api/private_session_transport.hpp"
#include "ipi/api/receiver.hpp"
#include "ipi/api/sender.hpp"
#include "ipi/core/ipi_service_request.hpp"

#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ipi::api {

struct ServiceRequestContext {
    std::string vehicleId{};
    std::optional<std::string> vin{};
    Location location{};
    std::optional<double> speedMps{};
    std::optional<double> headingDegrees{};
};

/**
 * Edge4AvInterface provides a single high-level entry point for both
 * broadcast-V2X messages and private connected-session flows. It removes the
 * repetitive packing of typed J2735 messages into payload envelopes and the
 * manual translation from IPI service requests into the private-session API.
 */
class Edge4AvInterface {
public:
    explicit Edge4AvInterface(std::shared_ptr<ReceiverApi> receiver = make_in_memory_receiver_api(),
                              std::shared_ptr<SenderApi> sender = make_in_memory_sender_api(),
                              std::shared_ptr<PrivateSessionTransport> privateSessionTransport = {},
                              v2x::UperCodec codec = {});

    template <typename MessageT>
    Ack ingest_v2x_message(EnvelopeMetadata metadata,
                           const MessageT& message,
                           std::optional<int> rssi = {},
                           std::optional<int> channel = {},
                           J2735Encoding encoding = J2735Encoding::UPER) const {
        metadata = normalize_metadata(std::move(metadata));
        return receiver_->ingestV2xMessage(
            make_j2735_envelope(metadata, message, codec_, encoding), rssi, channel);
    }

    template <typename MessageT>
    Ack request_broadcast(EnvelopeMetadata metadata,
                          BroadcastTarget target,
                          const MessageT& message,
                          J2735Encoding encoding = J2735Encoding::UPER) const {
        BroadcastRequest request;
        request.metadata = normalize_metadata(std::move(metadata));
        request.target = std::move(target);
        request.message = pack_j2735_payload(message, codec_, encoding);
        return receiver_->requestBroadcast(request);
    }

    template <typename MessageT>
    std::vector<MessageT> list_v2x_messages(std::optional<Timestamp> since = {},
                                            std::size_t limit = 50) const {
        const auto payloads = sender_->listV2xMessages(make_v2x_query<MessageT>(since, limit));
        std::vector<MessageT> decoded;
        decoded.reserve(payloads.size());
        for (const auto& payload : payloads) {
            decoded.push_back(unpack_j2735_payload<MessageT>(payload, codec_));
        }
        return decoded;
    }

    SessionDescriptor register_session(SessionRegistration registration) const;

    SessionDescriptor register_vehicle_session(EnvelopeMetadata metadata,
                                               VehicleProfile vehicleProfile,
                                               std::vector<std::string> requestedServices = {},
                                               std::optional<std::string> rsuFallback = {},
                                               std::optional<int> minSidelinkRssi = {},
                                               std::optional<Subscription> inlineSubscription = {}) const;

    Ack heartbeat(HeartbeatUpdate heartbeat) const;
    Ack heartbeat(const std::string& sessionId,
                  std::optional<VehicleTelemetryFrame> telemetry = {}) const;

    Ack submit_service_request(EnvelopeMetadata metadata,
                               const VehicleProfile& vehicleProfile,
                               const ipi::IpiServiceRequest& request,
                               ServiceRequestContext context) const;

    Ack submit_telemetry(TelemetrySubmission submission) const;
    Ack submit_telemetry(const std::string& sessionId,
                         std::vector<VehicleTelemetryFrame> frames) const;

    std::vector<Envelope<VehicleServiceResponse>> list_vehicle_responses(
        const std::string& vehicleId,
        std::optional<std::variant<PCVServiceType, PCAVServiceType>> serviceType = {},
        std::optional<Timestamp> since = {}) const;

    std::vector<Envelope<VehicleServiceResponse>> list_session_responses(
        const std::string& sessionId) const;

    [[nodiscard]] const v2x::UperCodec& codec() const noexcept;
    [[nodiscard]] const std::shared_ptr<PrivateSessionTransport>& private_session_transport() const noexcept;

private:
    std::shared_ptr<ReceiverApi> receiver_;
    std::shared_ptr<SenderApi> sender_;
    std::shared_ptr<PrivateSessionTransport> privateSessionTransport_;
    v2x::UperCodec codec_;
    mutable std::atomic<std::uint64_t> messageCounter_{0};

    EnvelopeMetadata normalize_metadata(EnvelopeMetadata metadata) const;
    [[nodiscard]] std::string make_identifier(const char* prefix) const;
};

} // namespace ipi::api
