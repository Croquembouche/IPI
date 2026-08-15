#include "ipi/api/receiver.hpp"

#include "shared_state.hpp"

#include <chrono>
#include <random>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace ipi::api {

namespace {

std::string make_identifier(detail::SharedState& state)
{
    std::ostringstream oss;
    oss << "ipi-" << ++state.messageCounter;
    return oss.str();
}

Envelope<VehicleServiceResponse> make_response(const Envelope<VehicleServiceRequest>& request,
                                               VehicleServiceStatus status,
                                               detail::SharedState& state)
{
    Envelope<VehicleServiceResponse> response;
    response.metadata = request.metadata;
    response.metadata.messageId = make_identifier(state);
    response.metadata.sentAt = std::chrono::system_clock::now();
    response.data.serviceType = request.data.serviceType;
    response.data.vehicleId = request.data.vehicleId;
    response.data.status = status;
    response.data.expiresAt = request.metadata.expiresAt;
    response.data.guidance = request.data.context;
    response.data.resultId = make_identifier(state);
    response.data.failureCode = FailureCode::NONE;
    validate_vehicle_service_response(response.data);
    return response;
}

Ack reject(FailureCode code, std::string detail)
{
    Ack acknowledgement;
    acknowledgement.accepted = false;
    acknowledgement.code = code;
    acknowledgement.detail = std::move(detail);
    return acknowledgement;
}

class InMemoryReceiverApi final : public ReceiverApi {
public:
    explicit InMemoryReceiverApi(std::shared_ptr<detail::SharedState> state)
        : state_(std::move(state)) {}

    Ack ingestV2xMessage(const Envelope<J2735Payload>& message,
                         std::optional<int> rssi,
                         std::optional<int> channel) override
    {
        std::lock_guard lock(state_->mutex);
        state_->v2xMessages.push_back({message, rssi, channel});
        return Ack{};
    }

    Ack requestBroadcast(const BroadcastRequest& request) override
    {
        std::lock_guard lock(state_->mutex);
        state_->broadcastRequests.push_back(request);
        return Ack{};
    }

    Ack submitPCVRequest(const Envelope<VehicleServiceRequest>& request) override
    {
        std::lock_guard lock(state_->mutex);
        state_->pcvRequests.push_back(request);
        auto response = make_response(request, VehicleServiceStatus::ACCEPTED, *state_);
        state_->responsesByVehicle[response.data.vehicleId].push_back(response);
        return Ack{};
    }

    Ack submitPCAVRequest(const Envelope<VehicleServiceRequest>& request) override
    {
        std::lock_guard lock(state_->mutex);
        state_->pcavRequests.push_back(request);
        auto response = make_response(request, VehicleServiceStatus::ACCEPTED, *state_);
        state_->responsesByVehicle[response.data.vehicleId].push_back(response);
        return Ack{};
    }

    Ack submitPedestrianAcknowledgement(const Envelope<PedestrianAcknowledgement>& acknowledgement) override
    {
        std::lock_guard lock(state_->mutex);
        state_->pedestrianAcknowledgements.push_back(acknowledgement);
        state_->warningsByDevice[acknowledgement.metadata.source.id].clear();
        return Ack{};
    }

    Ack forwardIntersectionMessage(const Envelope<IntersectionMessage>& message) override
    {
        std::lock_guard lock(state_->mutex);
        state_->intersectionMessages.push_back(message);
        return Ack{};
    }

    SessionDescriptor registerSession(const SessionRegistration& registration) override
    {
        std::lock_guard lock(state_->mutex);
        SessionDescriptor descriptor;
        descriptor.sessionId = registration.metadata.sessionId.value_or(make_identifier(*state_));
        if (descriptor.sessionId.empty() || registration.vehicleProfile.vehicleId.empty() ||
            registration.metadata.intersectionId.empty()) {
            throw std::invalid_argument(
                "session registration requires session, vehicle, and intersection identifiers");
        }
        if (state_->sessionDirectory.find(descriptor.sessionId) != state_->sessionDirectory.end()) {
            throw std::invalid_argument("duplicate session id: " + descriptor.sessionId);
        }
        descriptor.vehicleProfile = registration.vehicleProfile;
        descriptor.transport = registration.metadata.transport;
        descriptor.state = SessionState::REGISTERED;
        descriptor.leaseSeconds = 30;
        descriptor.heartbeatIntervalSeconds = 5;
        descriptor.preferredChannels = registration.requestedServices;
        descriptor.grantedServices = registration.requestedServices;
        descriptor.registeredAt = std::chrono::system_clock::now();
        descriptor.lastHeartbeatAt = descriptor.registeredAt;
        descriptor.expiresAt = descriptor.registeredAt + std::chrono::seconds(descriptor.leaseSeconds);
        descriptor.rsuFallback = registration.rsuFallback;
        descriptor.minSidelinkRssi = registration.minSidelinkRssi;
        descriptor.inlineSubscription = registration.inlineSubscription;

        state_->sessionDirectory[descriptor.sessionId] = descriptor;
        return descriptor;
    }

    Ack heartbeat(const HeartbeatUpdate& heartbeat) override
    {
        std::lock_guard lock(state_->mutex);
        if (auto it = state_->sessionDirectory.find(heartbeat.sessionId); it != state_->sessionDirectory.end()) {
            if (it->second.state == SessionState::TERMINATED) {
                return reject(FailureCode::SESSION_TERMINATED, "session is terminated");
            }
            if (it->second.state == SessionState::EXPIRED) {
                return reject(FailureCode::SESSION_EXPIRED, "session is expired");
            }
            it->second.state = SessionState::ACTIVE;
            it->second.lastHeartbeatAt = std::chrono::system_clock::now();
            it->second.expiresAt = it->second.lastHeartbeatAt +
                                   std::chrono::seconds(it->second.leaseSeconds);
            if (heartbeat.telemetry) {
                state_->telemetryBySession[heartbeat.sessionId].push_back(*heartbeat.telemetry);
            }
            return Ack{};
        }
        return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
    }

    Ack patchSession(const SessionPatch& patch) override
    {
        std::lock_guard lock(state_->mutex);
        auto it = state_->sessionDirectory.find(patch.sessionId);
        if (it == state_->sessionDirectory.end()) {
            return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        }
        if (it->second.state == SessionState::TERMINATED ||
            it->second.state == SessionState::EXPIRED) {
            return reject(it->second.state == SessionState::TERMINATED
                              ? FailureCode::SESSION_TERMINATED
                              : FailureCode::SESSION_EXPIRED,
                          "session is terminal");
        }
        if (patch.profile) {
            if (patch.profile->vehicleId != it->second.vehicleProfile.vehicleId) {
                return reject(FailureCode::ACCESS_DENIED,
                              "session patch cannot change vehicle ownership");
            }
            it->second.vehicleProfile = *patch.profile;
        }
        if (patch.preferredChannels) {
            it->second.preferredChannels = *patch.preferredChannels;
        }
        return Ack{};
    }

    Ack terminateSession(const SessionTermination& termination) override
    {
        std::lock_guard lock(state_->mutex);
        auto it = state_->sessionDirectory.find(termination.sessionId);
        if (it == state_->sessionDirectory.end()) {
            return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        }
        if (it->second.state == SessionState::EXPIRED) {
            return reject(FailureCode::SESSION_EXPIRED, "session is expired");
        }
        it->second.state = SessionState::TERMINATED;
        return Ack{true, termination.sessionId};
    }

    Ack invokeService(const ServiceInvocation& invocation) override
    {
        std::lock_guard lock(state_->mutex);
        auto session = state_->sessionDirectory.find(invocation.sessionId);
        if (session == state_->sessionDirectory.end()) {
            return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        }
        if (session->second.state != SessionState::ACTIVE) {
            const auto code = session->second.state == SessionState::TERMINATED
                                  ? FailureCode::SESSION_TERMINATED
                              : session->second.state == SessionState::EXPIRED
                                  ? FailureCode::SESSION_EXPIRED
                                  : FailureCode::SESSION_INACTIVE;
            return reject(code, "session is not active");
        }
        if (!invocation.request.metadata.sessionId ||
            *invocation.request.metadata.sessionId != invocation.sessionId ||
            invocation.request.data.vehicleId != session->second.vehicleProfile.vehicleId) {
            return reject(FailureCode::CORRELATION_MISMATCH,
                          "service request identity does not match session");
        }
        auto progress = make_response(
            invocation.request, VehicleServiceStatus::IN_PROGRESS, *state_);
        auto completed = make_response(
            invocation.request, VehicleServiceStatus::COMPLETED, *state_);
        auto& vehicleResponses =
            state_->responsesByVehicle[invocation.request.data.vehicleId];
        vehicleResponses.push_back(progress);
        vehicleResponses.push_back(completed);
        state_->responsesBySession[invocation.sessionId].push_back(std::move(progress));
        state_->responsesBySession[invocation.sessionId].push_back(std::move(completed));
        return Ack{};
    }

    Ack submitTelemetry(const TelemetrySubmission& submission) override
    {
        std::lock_guard lock(state_->mutex);
        const auto session = state_->sessionDirectory.find(submission.sessionId);
        if (session == state_->sessionDirectory.end()) {
            return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        }
        if (session->second.state != SessionState::ACTIVE) {
            return reject(FailureCode::SESSION_INACTIVE, "session is not active");
        }
        auto& frames = state_->telemetryBySession[submission.sessionId];
        frames.insert(frames.end(), submission.frames.begin(), submission.frames.end());
        return Ack{};
    }

    Ack submitCloudExportJob(const CloudExportJob& job, std::string& outJobId) override
    {
        (void)job;
        std::lock_guard lock(state_->mutex);
        std::string id = make_identifier(*state_);
        CloudExportStatus status;
        status.jobId = id;
        status.status = "PROCESSING";
        status.transfer.reset();
        state_->cloudJobs[id] = status;
        outJobId = id;
        return Ack{true, id};
    }

private:
    std::shared_ptr<detail::SharedState> state_;
};

} // namespace

std::shared_ptr<ReceiverApi> make_in_memory_receiver_api()
{
    return std::make_shared<InMemoryReceiverApi>(std::make_shared<detail::SharedState>());
}

namespace detail {

std::shared_ptr<ReceiverApi> make_receiver_for_state(std::shared_ptr<SharedState> state) {
    return std::make_shared<InMemoryReceiverApi>(std::move(state));
}

} // namespace detail

} // namespace ipi::api
