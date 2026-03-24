#pragma once

#include "ipi/api/receiver.hpp"
#include "ipi/api/sender.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ipi::api {

enum class SessionTopicKind {
    Register,
    Events,
    Heartbeat,
    ServiceRequest,
    ServiceUpdate,
    Telemetry,
    PcvResponse
};

struct SessionTopic {
    SessionTopicKind kind{SessionTopicKind::Register};
    std::string intersectionId{};
    std::optional<std::string> sessionId{};
    std::optional<std::string> vehicleId{};

    [[nodiscard]] std::string to_string() const;
    static SessionTopic parse(const std::string& topic);
};

struct SessionPublication {
    SessionTopic topic{};
    EnvelopeMetadata metadata{};
    std::string payloadType{};
    std::size_t payloadSize{0};
};

struct SessionPublicationQuery {
    std::optional<std::string> intersectionId{};
    std::optional<std::string> sessionId{};
    std::optional<SessionTopicKind> kind{};
    std::size_t limit{50};
};

class PrivateSessionTransport {
public:
    virtual ~PrivateSessionTransport() = default;

    virtual SessionDescriptor register_session(const SessionRegistration& registration) = 0;
    virtual Ack heartbeat(const HeartbeatUpdate& heartbeat) = 0;
    virtual Ack invoke_service(const ServiceInvocation& invocation) = 0;
    virtual Ack submit_telemetry(const TelemetrySubmission& submission) = 0;

    virtual std::vector<Envelope<VehicleServiceResponse>> list_session_responses(
        const SessionResponseQuery& query) const = 0;

    virtual std::vector<SessionPublication> list_publications(
        const SessionPublicationQuery& query = {}) const = 0;
};

std::shared_ptr<PrivateSessionTransport> make_in_memory_private_session_transport(
    std::shared_ptr<ReceiverApi> receiver = make_in_memory_receiver_api(),
    std::shared_ptr<SenderApi> sender = make_in_memory_sender_api());

} // namespace ipi::api
