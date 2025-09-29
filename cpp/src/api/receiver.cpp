#include "ipi/api/receiver.hpp"

#include "shared_state.hpp"

#include <chrono>
#include <random>
#include <sstream>

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
    response.data.guidance = request.data.context;
    return response;
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
        SessionDescriptor descriptor;
        descriptor.sessionId = registration.metadata.sessionId.value_or(make_identifier(*state_));
        descriptor.vehicleProfile = registration.vehicleProfile;
        descriptor.transport = registration.metadata.transport;
        descriptor.state = SessionState::ACTIVE;
        descriptor.leaseSeconds = 30;
        descriptor.heartbeatIntervalSeconds = 5;
        descriptor.preferredChannels = registration.requestedServices;
        descriptor.grantedServices = registration.requestedServices;

        std::lock_guard lock(state_->mutex);
        state_->sessionDirectory[descriptor.sessionId] = descriptor;
        return descriptor;
    }

    Ack heartbeat(const HeartbeatUpdate& heartbeat) override
    {
        std::lock_guard lock(state_->mutex);
        if (auto it = state_->sessionDirectory.find(heartbeat.sessionId); it != state_->sessionDirectory.end()) {
            it->second.state = SessionState::ACTIVE;
            if (heartbeat.telemetry) {
                state_->telemetryBySession[heartbeat.sessionId].push_back(*heartbeat.telemetry);
            }
            return Ack{};
        }
        return Ack{false, {"unknown-session"}};
    }

    Ack patchSession(const SessionPatch& patch) override
    {
        std::lock_guard lock(state_->mutex);
        auto it = state_->sessionDirectory.find(patch.sessionId);
        if (it == state_->sessionDirectory.end()) {
            return Ack{false, {"unknown-session"}};
        }
        if (patch.profile) {
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
            return Ack{false, {"unknown-session"}};
        }
        it->second.state = SessionState::TERMINATED;
        return Ack{};
    }

    Ack invokeService(const ServiceInvocation& invocation) override
    {
        std::lock_guard lock(state_->mutex);
        state_->responsesBySession[invocation.sessionId].push_back(
            make_response(invocation.request, VehicleServiceStatus::IN_PROGRESS, *state_));
        return Ack{};
    }

    Ack submitTelemetry(const TelemetrySubmission& submission) override
    {
        std::lock_guard lock(state_->mutex);
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
    return std::make_shared<InMemoryReceiverApi>(detail::obtain_shared_state());
}

} // namespace ipi::api
