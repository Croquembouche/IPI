#include "ipi/api/private_session_transport.hpp"

#include <algorithm>
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace ipi::api {

namespace {

std::vector<std::string> split_topic(const std::string& topic) {
    std::vector<std::string> parts;
    std::size_t start = 0;
    while (start <= topic.size()) {
        const auto end = topic.find('/', start);
        parts.push_back(topic.substr(start, end == std::string::npos ? std::string::npos : end - start));
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    return parts;
}

class InMemoryPrivateSessionTransport final : public PrivateSessionTransport {
public:
    InMemoryPrivateSessionTransport(std::shared_ptr<ReceiverApi> receiver,
                                    std::shared_ptr<SenderApi> sender)
        : receiver_(std::move(receiver)),
          sender_(std::move(sender)) {
        if (!receiver_) {
            throw std::invalid_argument("PrivateSessionTransport requires a ReceiverApi");
        }
        if (!sender_) {
            throw std::invalid_argument("PrivateSessionTransport requires a SenderApi");
        }
    }

    SessionDescriptor register_session(const SessionRegistration& registration) override {
        auto descriptor = receiver_->registerSession(registration);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            sessionIntersectionIds_[descriptor.sessionId] = registration.metadata.intersectionId;
        }

        SessionPublication publication;
        publication.topic.kind = SessionTopicKind::Register;
        publication.topic.intersectionId = registration.metadata.intersectionId;
        publication.topic.vehicleId = descriptor.vehicleProfile.vehicleId;
        publication.metadata = registration.metadata;
        publication.metadata.sessionId = descriptor.sessionId;
        publication.payloadType = "session-registration";
        publication.payloadSize = registration.requestedServices.size();
        record_publication(std::move(publication));

        return descriptor;
    }

    Ack heartbeat(const HeartbeatUpdate& heartbeatUpdate) override {
        SessionPublication publication;
        publication.topic.kind = SessionTopicKind::Heartbeat;
        publication.topic.intersectionId = lookup_intersection_id(heartbeatUpdate.sessionId);
        publication.topic.sessionId = heartbeatUpdate.sessionId;
        publication.metadata.sessionId = heartbeatUpdate.sessionId;
        publication.metadata.intersectionId = publication.topic.intersectionId;
        publication.metadata.transport = TransportType::CELLULAR_5G;
        publication.payloadType = "heartbeat";
        publication.payloadSize = heartbeatUpdate.telemetry ? 1U : 0U;
        record_publication(std::move(publication));

        return receiver_->heartbeat(heartbeatUpdate);
    }

    Ack invoke_service(const ServiceInvocation& invocation) override {
        SessionPublication publication;
        publication.topic.kind = SessionTopicKind::ServiceRequest;
        publication.topic.intersectionId = invocation.request.metadata.intersectionId;
        publication.topic.sessionId = invocation.sessionId;
        publication.metadata = invocation.request.metadata;
        publication.metadata.sessionId = invocation.sessionId;
        publication.payloadType = "service-invocation";
        publication.payloadSize = invocation.request.data.context.size();
        record_publication(std::move(publication));

        return receiver_->invokeService(invocation);
    }

    Ack submit_telemetry(const TelemetrySubmission& submission) override {
        SessionPublication publication;
        publication.topic.kind = SessionTopicKind::Telemetry;
        publication.topic.intersectionId = lookup_intersection_id(submission.sessionId);
        publication.topic.sessionId = submission.sessionId;
        publication.metadata.sessionId = submission.sessionId;
        publication.metadata.intersectionId = publication.topic.intersectionId;
        publication.metadata.transport = TransportType::CELLULAR_5G;
        publication.payloadType = "telemetry";
        publication.payloadSize = submission.frames.size();
        record_publication(std::move(publication));

        return receiver_->submitTelemetry(submission);
    }

    std::vector<Envelope<VehicleServiceResponse>> list_session_responses(
        const SessionResponseQuery& query) const override {
        return sender_->listSessionResponses(query);
    }

    std::vector<SessionPublication> list_publications(
        const SessionPublicationQuery& query = {}) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<SessionPublication> result;
        result.reserve(std::min(query.limit, publications_.size()));

        for (auto it = publications_.rbegin(); it != publications_.rend(); ++it) {
            if (query.kind && it->topic.kind != *query.kind) {
                continue;
            }
            if (query.intersectionId && it->topic.intersectionId != *query.intersectionId) {
                continue;
            }
            if (query.sessionId && it->topic.sessionId != query.sessionId) {
                continue;
            }
            result.push_back(*it);
            if (result.size() >= query.limit) {
                break;
            }
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

private:
    std::string lookup_intersection_id(const std::string& sessionId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = sessionIntersectionIds_.find(sessionId);
        if (it != sessionIntersectionIds_.end()) {
            return it->second;
        }
        return {};
    }

    void record_publication(SessionPublication publication) {
        std::lock_guard<std::mutex> lock(mutex_);
        publications_.push_back(std::move(publication));
    }

    std::shared_ptr<ReceiverApi> receiver_;
    std::shared_ptr<SenderApi> sender_;
    mutable std::mutex mutex_;
    std::map<std::string, std::string> sessionIntersectionIds_;
    std::vector<SessionPublication> publications_;
};

} // namespace

std::string SessionTopic::to_string() const {
    switch (kind) {
        case SessionTopicKind::Register:
            return "ipi/" + intersectionId + "/session/register";
        case SessionTopicKind::Events:
            if (!vehicleId) {
                throw std::invalid_argument("events topic requires vehicleId");
            }
            return "ipi/" + intersectionId + "/session/" + *vehicleId + "/events";
        case SessionTopicKind::Heartbeat:
            if (!sessionId) {
                throw std::invalid_argument("heartbeat topic requires sessionId");
            }
            return "ipi/" + intersectionId + "/session/" + *sessionId + "/heartbeat";
        case SessionTopicKind::ServiceRequest:
            if (!sessionId) {
                throw std::invalid_argument("service request topic requires sessionId");
            }
            return "ipi/" + intersectionId + "/session/" + *sessionId + "/service/request";
        case SessionTopicKind::ServiceUpdate:
            if (!sessionId) {
                throw std::invalid_argument("service update topic requires sessionId");
            }
            return "ipi/" + intersectionId + "/session/" + *sessionId + "/service/update";
        case SessionTopicKind::Telemetry:
            if (!sessionId) {
                throw std::invalid_argument("telemetry topic requires sessionId");
            }
            return "ipi/" + intersectionId + "/session/" + *sessionId + "/telemetry";
        case SessionTopicKind::PcvResponse:
            if (!vehicleId) {
                throw std::invalid_argument("pcv response topic requires vehicleId");
            }
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

    SessionTopic parsed;
    parsed.intersectionId = parts[1];

    if (parts[2] == "session") {
        if (parts[3] == "register" && parts.size() == 4) {
            parsed.kind = SessionTopicKind::Register;
            return parsed;
        }
        if (parts.size() == 5 && parts[4] == "heartbeat") {
            parsed.kind = SessionTopicKind::Heartbeat;
            parsed.sessionId = parts[3];
            return parsed;
        }
        if (parts.size() == 5 && parts[4] == "telemetry") {
            parsed.kind = SessionTopicKind::Telemetry;
            parsed.sessionId = parts[3];
            return parsed;
        }
        if (parts.size() == 6 && parts[4] == "service" && parts[5] == "request") {
            parsed.kind = SessionTopicKind::ServiceRequest;
            parsed.sessionId = parts[3];
            return parsed;
        }
        if (parts.size() == 6 && parts[4] == "service" && parts[5] == "update") {
            parsed.kind = SessionTopicKind::ServiceUpdate;
            parsed.sessionId = parts[3];
            return parsed;
        }
        if (parts.size() == 5 && parts[4] == "events") {
            parsed.kind = SessionTopicKind::Events;
            parsed.vehicleId = parts[3];
            return parsed;
        }
    }

    if (parts[2] == "pcv" && parts.size() == 5 && parts[4] == "response") {
        parsed.kind = SessionTopicKind::PcvResponse;
        parsed.vehicleId = parts[3];
        return parsed;
    }

    throw std::invalid_argument("unsupported session topic: " + topic);
}

std::shared_ptr<PrivateSessionTransport> make_in_memory_private_session_transport(
    std::shared_ptr<ReceiverApi> receiver,
    std::shared_ptr<SenderApi> sender) {
    return std::make_shared<InMemoryPrivateSessionTransport>(std::move(receiver), std::move(sender));
}

} // namespace ipi::api
