#pragma once

#include "ipi/api/types.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace ipi::api {

class ReceiverApi {
public:
    virtual ~ReceiverApi() = default;

    virtual Ack ingestV2xMessage(const Envelope<J2735Payload>& message,
                                 std::optional<int> rssi = {},
                                 std::optional<int> channel = {}) = 0;

    virtual Ack requestBroadcast(const BroadcastRequest& request) = 0;

    virtual Ack submitPCVRequest(const Envelope<VehicleServiceRequest>& request) = 0;

    virtual Ack submitPCAVRequest(const Envelope<VehicleServiceRequest>& request) = 0;

    virtual Ack submitPedestrianAcknowledgement(const Envelope<PedestrianAcknowledgement>& acknowledgement) = 0;

    virtual Ack forwardIntersectionMessage(const Envelope<IntersectionMessage>& message) = 0;

    virtual SessionDescriptor registerSession(const SessionRegistration& registration) = 0;

    virtual Ack heartbeat(const HeartbeatUpdate& heartbeat) = 0;

    virtual Ack patchSession(const SessionPatch& patch) = 0;

    virtual Ack terminateSession(const SessionTermination& termination) = 0;

    virtual Ack invokeService(const ServiceInvocation& invocation) = 0;

    virtual Ack submitTelemetry(const TelemetrySubmission& submission) = 0;

    virtual Ack submitCloudExportJob(const CloudExportJob& job, std::string& outJobId) = 0;
};

std::shared_ptr<ReceiverApi> make_in_memory_receiver_api();

} // namespace ipi::api
