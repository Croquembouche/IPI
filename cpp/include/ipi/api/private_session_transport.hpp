#pragma once

#include "ipi/api/receiver.hpp"
#include "ipi/api/sender.hpp"
#include "ipi/api/session_lifecycle.hpp"

#include <memory>
#include <optional>
#include <string>
#include <chrono>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <vector>

namespace ipi::api {

enum class SessionTopicKind {
    Register,
    Events,
    Heartbeat,
    Patch,
    Terminate,
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
    std::vector<std::uint8_t> encodedPayload{};
};

struct SessionPublicationQuery {
    std::optional<std::string> intersectionId{};
    std::optional<std::string> sessionId{};
    std::optional<SessionTopicKind> kind{};
    std::size_t limit{50};
};

using SessionTransportOptions = SessionLifecycleOptions;

class SessionTransportException : public std::runtime_error {
public:
    SessionTransportException(FailureCode code, const std::string& message)
        : std::runtime_error(message), code_(code) {}

    [[nodiscard]] FailureCode code() const noexcept { return code_; }

private:
    FailureCode code_;
};

class SessionMessageBroker {
public:
    using Handler = std::function<void(const std::string&,
                                       const std::vector<std::uint8_t>&)>;

    virtual ~SessionMessageBroker() = default;
    virtual void publish(const std::string& topic,
                         const std::vector<std::uint8_t>& payload) = 0;
    virtual std::uint64_t subscribe(const std::string& topicFilter,
                                    Handler handler) = 0;
    virtual void unsubscribe(std::uint64_t subscriptionId) = 0;
};

struct MqttSessionBrokerConfig {
    std::string host{"127.0.0.1"};
    std::uint16_t port{1883};
    std::string clientId{"ipi-session-transport"};
    std::optional<std::string> username{};
    std::optional<std::string> password{};
    std::uint16_t keepAliveSeconds{30};
    bool cleanSession{true};
    std::string allowedTopicPrefix{"ipi/"};
    std::chrono::milliseconds reconnectBackoff{250};
    std::chrono::milliseconds ioTimeout{std::chrono::seconds(5)};
};

class SessionBrokerEndpoint {
public:
    virtual ~SessionBrokerEndpoint() = default;
};

class PrivateSessionTransport {
public:
    virtual ~PrivateSessionTransport() = default;

    virtual SessionDescriptor register_session(const SessionRegistration& registration) = 0;
    virtual Ack heartbeat(const HeartbeatUpdate& heartbeat) = 0;
    virtual Ack patch_session(const SessionPatch& patch) = 0;
    virtual Ack terminate_session(const SessionTermination& termination) = 0;
    virtual Ack invoke_service(const ServiceInvocation& invocation) = 0;
    virtual Ack submit_telemetry(const TelemetrySubmission& submission) = 0;
    virtual Ack deliver_service_update(
        const std::string& sessionId,
        const Envelope<VehicleServiceResponse>& response) = 0;

    virtual std::optional<SessionDescriptor> get_session(const std::string& sessionId) const = 0;

    virtual std::vector<Envelope<VehicleServiceResponse>> list_session_responses(
        const SessionResponseQuery& query) const = 0;

    virtual std::vector<SessionPublication> list_publications(
        const SessionPublicationQuery& query = {}) const = 0;
};

std::shared_ptr<PrivateSessionTransport> make_in_memory_private_session_transport(
    std::shared_ptr<ReceiverApi> receiver = {},
    std::shared_ptr<SenderApi> sender = {},
    SessionTransportOptions options = {});

std::shared_ptr<SessionMessageBroker> make_mqtt_session_message_broker(
    MqttSessionBrokerConfig config);

std::shared_ptr<PrivateSessionTransport> make_broker_private_session_transport(
    std::shared_ptr<SessionMessageBroker> broker,
    SessionTransportOptions options = {},
    std::chrono::milliseconds responseTimeout = std::chrono::seconds(5));

std::shared_ptr<SessionBrokerEndpoint> make_broker_session_endpoint(
    std::shared_ptr<SessionMessageBroker> broker,
    std::shared_ptr<ReceiverApi> receiver = {},
    std::shared_ptr<SenderApi> sender = {},
    SessionTransportOptions options = {});

} // namespace ipi::api
