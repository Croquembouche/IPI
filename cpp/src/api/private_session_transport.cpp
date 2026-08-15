#include "ipi/api/private_session_transport.hpp"

#include "ipi/api/in_memory_api.hpp"
#include "ipi/api/session_wire_codec.hpp"

#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace ipi::api {
namespace {

std::vector<std::string> split_topic(const std::string& topic) {
    std::vector<std::string> parts;
    std::size_t start = 0;
    while (start <= topic.size()) {
        const auto end = topic.find('/', start);
        parts.push_back(topic.substr(
            start, end == std::string::npos ? std::string::npos : end - start));
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return parts;
}

void validate_topic_segment(const std::string& value, const char* label) {
    if (value.empty() || value.find_first_of("/+#") != std::string::npos ||
        value.find('\0') != std::string::npos) {
        throw std::invalid_argument(std::string("invalid ") + label + " topic segment");
    }
}

bool default_timestamp(Timestamp value) {
    return value.time_since_epoch() == Timestamp::duration::zero();
}

SourceType source_for_role(VehicleRole role) {
    switch (role) {
        case VehicleRole::PCAV: return SourceType::PCAV;
        case VehicleRole::PEDESTRIAN_DEVICE: return SourceType::PEDESTRIAN_DEVICE;
        default: return SourceType::PCV;
    }
}

Ack receiver_failure(std::string detail) {
    Ack acknowledgement;
    acknowledgement.accepted = false;
    acknowledgement.code = FailureCode::SERVICE_UNAVAILABLE;
    acknowledgement.detail = std::move(detail);
    return acknowledgement;
}

SessionTopic service_response_topic(
    const SessionSnapshot& snapshot,
    const std::string& sessionId,
    const Envelope<VehicleServiceResponse>& response) {
    if (std::holds_alternative<PCVServiceType>(response.data.serviceType)) {
        return SessionTopic{SessionTopicKind::PcvResponse,
                            snapshot.registration.metadata.intersectionId,
                            std::nullopt, response.data.vehicleId};
    }
    return SessionTopic{SessionTopicKind::ServiceUpdate,
                        snapshot.registration.metadata.intersectionId,
                        sessionId, std::nullopt};
}

class InMemoryPrivateSessionTransport final : public PrivateSessionTransport {
public:
    InMemoryPrivateSessionTransport(std::shared_ptr<ReceiverApi> receiver,
                                    std::shared_ptr<SenderApi> sender,
                                    SessionTransportOptions options)
        : receiver_(std::move(receiver)),
          sender_(std::move(sender)),
          wallClock_(options.wallClock ? options.wallClock
                                       : [] { return std::chrono::system_clock::now(); }),
          lifecycle_(std::move(options)) {
        if (!receiver_ || !sender_) {
            throw std::invalid_argument(
                "PrivateSessionTransport requires a matched ReceiverApi and SenderApi");
        }
    }

    SessionDescriptor register_session(const SessionRegistration& input) override {
        SessionRegistration registration = input;
        normalize_registration(registration);
        auto descriptor = lifecycle_.register_session(registration);

        auto receiverRegistration = registration;
        receiverRegistration.metadata.sessionId = descriptor.sessionId;
        try {
            (void)receiver_->registerSession(receiverRegistration);
        } catch (...) {
            (void)lifecycle_.terminate_session(
                SessionTermination{descriptor.sessionId,
                                   TerminationReasonCode::SERVICE_UNAVAILABLE,
                                   "receiver registration failed"});
            throw;
        }

        record(make_session_registration_wire_message(
            SessionTopic{SessionTopicKind::Register,
                         registration.metadata.intersectionId, std::nullopt, std::nullopt}
                .to_string(),
            receiverRegistration));

        EnvelopeMetadata eventMetadata = response_metadata(
            descriptor.sessionId, registration.metadata.messageId,
            registration.metadata.intersectionId);
        record(make_session_descriptor_wire_message(
            SessionTopic{SessionTopicKind::Events,
                         registration.metadata.intersectionId, std::nullopt,
                         descriptor.vehicleProfile.vehicleId}
                .to_string(),
            std::move(eventMetadata), descriptor));
        return descriptor;
    }

    Ack heartbeat(const HeartbeatUpdate& input) override {
        HeartbeatUpdate heartbeatUpdate = input;
        if (default_timestamp(heartbeatUpdate.timestamp)) {
            heartbeatUpdate.timestamp = wallClock_();
        }
        auto acknowledgement = lifecycle_.heartbeat(heartbeatUpdate);
        if (!acknowledgement.accepted) return acknowledgement;

        acknowledgement = receiver_->heartbeat(heartbeatUpdate);
        if (!acknowledgement.accepted) {
            return normalize_receiver_failure(std::move(acknowledgement));
        }

        const auto snapshot = lifecycle_.get_snapshot(heartbeatUpdate.sessionId);
        if (!snapshot) return receiver_failure("session disappeared after heartbeat");
        auto metadata = operation_metadata(*snapshot, heartbeatUpdate.sessionId);
        record(make_heartbeat_wire_message(
            SessionTopic{SessionTopicKind::Heartbeat,
                         snapshot->registration.metadata.intersectionId,
                         heartbeatUpdate.sessionId, std::nullopt}
                .to_string(),
            std::move(metadata), heartbeatUpdate));
        return acknowledgement;
    }

    Ack patch_session(const SessionPatch& patch) override {
        auto acknowledgement = lifecycle_.patch_session(patch);
        if (!acknowledgement.accepted) return acknowledgement;
        acknowledgement = receiver_->patchSession(patch);
        if (!acknowledgement.accepted) {
            return normalize_receiver_failure(std::move(acknowledgement));
        }

        const auto snapshot = lifecycle_.get_snapshot(patch.sessionId);
        if (!snapshot) return receiver_failure("session disappeared after patch");
        auto metadata = operation_metadata(*snapshot, patch.sessionId);
        record(make_session_patch_wire_message(
            SessionTopic{SessionTopicKind::Patch,
                         snapshot->registration.metadata.intersectionId,
                         patch.sessionId, std::nullopt}
                .to_string(),
            std::move(metadata), patch));
        return acknowledgement;
    }

    Ack terminate_session(const SessionTermination& termination) override {
        const auto snapshot = lifecycle_.get_snapshot(termination.sessionId);
        auto acknowledgement = lifecycle_.terminate_session(termination);
        if (!acknowledgement.accepted) return acknowledgement;
        acknowledgement = receiver_->terminateSession(termination);
        if (!acknowledgement.accepted) {
            return normalize_receiver_failure(std::move(acknowledgement));
        }
        if (snapshot) {
            auto metadata = operation_metadata(*snapshot, termination.sessionId);
            record(make_session_termination_wire_message(
                SessionTopic{SessionTopicKind::Terminate,
                             snapshot->registration.metadata.intersectionId,
                             termination.sessionId, std::nullopt}
                    .to_string(),
                std::move(metadata), termination));
        }
        return acknowledgement;
    }

    Ack invoke_service(const ServiceInvocation& input) override {
        ServiceInvocation invocation = input;
        normalize_invocation(invocation);
        auto acknowledgement = lifecycle_.authorize_service(invocation);
        if (!acknowledgement.accepted) return acknowledgement;

        acknowledgement = receiver_->invokeService(invocation);
        if (!acknowledgement.accepted) {
            return normalize_receiver_failure(std::move(acknowledgement));
        }

        const auto snapshot = lifecycle_.get_snapshot(invocation.sessionId);
        if (!snapshot) return receiver_failure("session disappeared after invocation");
        record(make_service_invocation_wire_message(
            SessionTopic{SessionTopicKind::ServiceRequest,
                         snapshot->registration.metadata.intersectionId,
                         invocation.sessionId, std::nullopt}
                .to_string(),
            invocation));

        const auto responses = sender_->listSessionResponses(
            SessionResponseQuery{invocation.sessionId});
        for (const auto& response : responses) {
            if (response.metadata.correlationId != invocation.request.metadata.correlationId) {
                continue;
            }
            const auto updateAck = deliver_service_update(invocation.sessionId, response);
            if (!updateAck.accepted && updateAck.code != FailureCode::DUPLICATE_RESPONSE) {
                return updateAck;
            }
        }
        acknowledgement.id = invocation.request.metadata.correlationId;
        return acknowledgement;
    }

    Ack submit_telemetry(const TelemetrySubmission& submission) override {
        auto acknowledgement = lifecycle_.authorize_telemetry(submission);
        if (!acknowledgement.accepted) return acknowledgement;
        acknowledgement = receiver_->submitTelemetry(submission);
        if (!acknowledgement.accepted) {
            return normalize_receiver_failure(std::move(acknowledgement));
        }

        const auto snapshot = lifecycle_.get_snapshot(submission.sessionId);
        if (!snapshot) return receiver_failure("session disappeared after telemetry");
        auto metadata = operation_metadata(*snapshot, submission.sessionId);
        record(make_telemetry_wire_message(
            SessionTopic{SessionTopicKind::Telemetry,
                         snapshot->registration.metadata.intersectionId,
                         submission.sessionId, std::nullopt}
                .to_string(),
            std::move(metadata), submission));
        return acknowledgement;
    }

    Ack deliver_service_update(
        const std::string& sessionId,
        const Envelope<VehicleServiceResponse>& response) override {
        auto acknowledgement = lifecycle_.accept_service_response(sessionId, response);
        if (!acknowledgement.accepted) return acknowledgement;

        const auto snapshot = lifecycle_.get_snapshot(sessionId);
        if (!snapshot) return receiver_failure("unknown session for service update");
        {
            std::lock_guard<std::mutex> lock(mutex_);
            validatedResponses_[sessionId].push_back(response);
        }
        record(make_service_response_wire_message(
            service_response_topic(*snapshot, sessionId, response).to_string(),
            response));
        acknowledgement.id = response.data.resultId;
        return acknowledgement;
    }

    std::optional<SessionDescriptor> get_session(
        const std::string& sessionId) const override {
        return lifecycle_.get_session(sessionId);
    }

    std::vector<Envelope<VehicleServiceResponse>> list_session_responses(
        const SessionResponseQuery& query) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto found = validatedResponses_.find(query.sessionId);
        if (found == validatedResponses_.end()) return {};
        const auto now = wallClock_();
        std::vector<Envelope<VehicleServiceResponse>> result;
        for (const auto& response : found->second) {
            if (response.data.expiresAt && *response.data.expiresAt <= now) continue;
            result.push_back(response);
        }
        return result;
    }

    std::vector<SessionPublication> list_publications(
        const SessionPublicationQuery& query = {}) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<SessionPublication> result;
        result.reserve(std::min(query.limit, publications_.size()));
        for (auto it = publications_.rbegin(); it != publications_.rend(); ++it) {
            if (query.kind && it->topic.kind != *query.kind) continue;
            if (query.intersectionId && it->topic.intersectionId != *query.intersectionId) continue;
            if (query.sessionId && it->topic.sessionId != query.sessionId) continue;
            result.push_back(*it);
            if (result.size() >= query.limit) break;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

private:
    void normalize_registration(SessionRegistration& registration) {
        if (registration.metadata.messageId.empty()) {
            registration.metadata.messageId = next_message_id();
        }
        if (default_timestamp(registration.metadata.sentAt)) {
            registration.metadata.sentAt = wallClock_();
        }
        if (registration.metadata.source.id.empty()) {
            registration.metadata.source.id = registration.vehicleProfile.vehicleId;
        }
        registration.metadata.source.type = source_for_role(registration.vehicleProfile.role);
    }

    void normalize_invocation(ServiceInvocation& invocation) {
        auto& metadata = invocation.request.metadata;
        metadata.sessionId = invocation.sessionId;
        if (metadata.messageId.empty()) metadata.messageId = next_message_id();
        if (default_timestamp(metadata.sentAt)) metadata.sentAt = wallClock_();
        if (!metadata.correlationId) metadata.correlationId = metadata.messageId;
        if (metadata.source.id.empty()) metadata.source.id = invocation.request.data.vehicleId;
    }

    EnvelopeMetadata operation_metadata(const SessionSnapshot& snapshot,
                                        const std::string& sessionId) {
        EnvelopeMetadata metadata;
        metadata.messageId = next_message_id();
        metadata.sentAt = wallClock_();
        metadata.intersectionId = snapshot.registration.metadata.intersectionId;
        metadata.transport = snapshot.descriptor.transport;
        metadata.source.type = source_for_role(snapshot.descriptor.vehicleProfile.role);
        metadata.source.id = snapshot.descriptor.vehicleProfile.vehicleId;
        metadata.sessionId = sessionId;
        return metadata;
    }

    EnvelopeMetadata response_metadata(const std::string& sessionId,
                                       const std::string& correlationId,
                                       const std::string& intersectionId) {
        EnvelopeMetadata metadata;
        metadata.messageId = next_message_id();
        metadata.sentAt = wallClock_();
        metadata.intersectionId = intersectionId;
        metadata.transport = TransportType::CELLULAR_5G;
        metadata.source.type = SourceType::INFRASTRUCTURE;
        metadata.source.id = intersectionId;
        metadata.sessionId = sessionId;
        metadata.correlationId = correlationId;
        return metadata;
    }

    std::string next_message_id() {
        std::ostringstream out;
        out << "ipi-session-message-" << ++messageCounter_;
        return out.str();
    }

    void record(const SessionWireMessage& wire) {
        SessionPublication publication;
        publication.topic = SessionTopic::parse(wire.topic);
        publication.metadata = wire.metadata;
        publication.payloadType = wire.payloadType;
        publication.payloadSize = wire.payload.size();
        publication.encodedPayload = encode_session_wire_message(wire);
        std::lock_guard<std::mutex> lock(mutex_);
        publications_.push_back(std::move(publication));
    }

    static Ack normalize_receiver_failure(Ack acknowledgement) {
        if (acknowledgement.code == FailureCode::NONE) {
            acknowledgement.code = FailureCode::SERVICE_UNAVAILABLE;
        }
        if (acknowledgement.detail.empty()) {
            acknowledgement.detail = "receiver rejected an authorized operation";
        }
        return acknowledgement;
    }

    std::shared_ptr<ReceiverApi> receiver_;
    std::shared_ptr<SenderApi> sender_;
    std::function<Timestamp()> wallClock_;
    mutable SessionLifecycle lifecycle_;
    std::atomic<std::uint64_t> messageCounter_{0};
    mutable std::mutex mutex_;
    std::unordered_map<std::string,
        std::vector<Envelope<VehicleServiceResponse>>> validatedResponses_;
    std::vector<SessionPublication> publications_;
};

} // namespace

std::string SessionTopic::to_string() const {
    validate_topic_segment(intersectionId, "intersectionId");
    switch (kind) {
        case SessionTopicKind::Register:
            return "ipi/" + intersectionId + "/session/register";
        case SessionTopicKind::Events:
            if (!vehicleId) throw std::invalid_argument("events topic requires vehicleId");
            validate_topic_segment(*vehicleId, "vehicleId");
            return "ipi/" + intersectionId + "/session/" + *vehicleId + "/events";
        case SessionTopicKind::Heartbeat:
        case SessionTopicKind::Patch:
        case SessionTopicKind::Terminate:
        case SessionTopicKind::ServiceRequest:
        case SessionTopicKind::ServiceUpdate:
        case SessionTopicKind::Telemetry:
            if (!sessionId) throw std::invalid_argument("session topic requires sessionId");
            validate_topic_segment(*sessionId, "sessionId");
            if (kind == SessionTopicKind::Heartbeat)
                return "ipi/" + intersectionId + "/session/" + *sessionId + "/heartbeat";
            if (kind == SessionTopicKind::Patch)
                return "ipi/" + intersectionId + "/session/" + *sessionId + "/patch";
            if (kind == SessionTopicKind::Terminate)
                return "ipi/" + intersectionId + "/session/" + *sessionId + "/terminate";
            if (kind == SessionTopicKind::Telemetry)
                return "ipi/" + intersectionId + "/session/" + *sessionId + "/telemetry";
            return "ipi/" + intersectionId + "/session/" + *sessionId +
                   "/service/" +
                   (kind == SessionTopicKind::ServiceRequest ? "request" : "update");
        case SessionTopicKind::PcvResponse:
            if (!vehicleId) throw std::invalid_argument("pcv response topic requires vehicleId");
            validate_topic_segment(*vehicleId, "vehicleId");
            return "ipi/" + intersectionId + "/pcv/" + *vehicleId + "/response";
        default:
            throw std::invalid_argument("unsupported session topic kind");
    }
}

SessionTopic SessionTopic::parse(const std::string& topic) {
    const auto parts = split_topic(topic);
    if (parts.size() < 4 || parts[0] != "ipi") {
        throw std::invalid_argument("invalid session topic: " + topic);
    }
    validate_topic_segment(parts[1], "intersectionId");

    SessionTopic parsed;
    parsed.intersectionId = parts[1];
    if (parts[2] == "session") {
        if (parts[3] == "register" && parts.size() == 4) {
            parsed.kind = SessionTopicKind::Register;
            return parsed;
        }
        if (parts.size() == 5 && parts[4] == "events") {
            validate_topic_segment(parts[3], "vehicleId");
            parsed.kind = SessionTopicKind::Events;
            parsed.vehicleId = parts[3];
            return parsed;
        }
        if (parts.size() == 5) {
            validate_topic_segment(parts[3], "sessionId");
            parsed.sessionId = parts[3];
            if (parts[4] == "heartbeat") parsed.kind = SessionTopicKind::Heartbeat;
            else if (parts[4] == "patch") parsed.kind = SessionTopicKind::Patch;
            else if (parts[4] == "terminate") parsed.kind = SessionTopicKind::Terminate;
            else if (parts[4] == "telemetry") parsed.kind = SessionTopicKind::Telemetry;
            else throw std::invalid_argument("unsupported session topic: " + topic);
            return parsed;
        }
        if (parts.size() == 6 && parts[4] == "service") {
            validate_topic_segment(parts[3], "sessionId");
            parsed.sessionId = parts[3];
            if (parts[5] == "request") parsed.kind = SessionTopicKind::ServiceRequest;
            else if (parts[5] == "update") parsed.kind = SessionTopicKind::ServiceUpdate;
            else throw std::invalid_argument("unsupported session topic: " + topic);
            return parsed;
        }
    }
    if (parts[2] == "pcv" && parts.size() == 5 && parts[4] == "response") {
        validate_topic_segment(parts[3], "vehicleId");
        parsed.kind = SessionTopicKind::PcvResponse;
        parsed.vehicleId = parts[3];
        return parsed;
    }
    throw std::invalid_argument("unsupported session topic: " + topic);
}

std::shared_ptr<PrivateSessionTransport> make_in_memory_private_session_transport(
    std::shared_ptr<ReceiverApi> receiver,
    std::shared_ptr<SenderApi> sender,
    SessionTransportOptions options) {
    if (!receiver && !sender) {
        auto pair = make_in_memory_api_pair();
        receiver = std::move(pair.receiver);
        sender = std::move(pair.sender);
    } else if (!receiver || !sender) {
        throw std::invalid_argument(
            "provide both receiver and sender, or neither, to preserve store isolation");
    }
    return std::make_shared<InMemoryPrivateSessionTransport>(
        std::move(receiver), std::move(sender), std::move(options));
}

} // namespace ipi::api
