#include "ipi/api/edge4av_interface.hpp"

#include <chrono>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace ipi::api {

namespace {

bool is_default_timestamp(const Timestamp& timestamp) {
    return timestamp.time_since_epoch() == Timestamp::duration::zero();
}

SourceType source_type_for_role(VehicleRole role) {
    switch (role) {
        case VehicleRole::PCAV:
            return SourceType::PCAV;
        case VehicleRole::PEDESTRIAN_DEVICE:
            return SourceType::PEDESTRIAN_DEVICE;
        case VehicleRole::EMERGENCY:
        case VehicleRole::TRANSIT:
        case VehicleRole::PCV:
        default:
            return SourceType::PCV;
    }
}

bool uses_pcav_contract(VehicleRole role) {
    return role == VehicleRole::PCAV;
}

std::variant<PCVServiceType, PCAVServiceType> translate_service_type(
    const ipi::ServiceType serviceType,
    const VehicleRole role) {
    switch (serviceType) {
        case ipi::ServiceType::LaneKeepingAid:
            if (uses_pcav_contract(role)) {
                return PCAVServiceType::LANE_KEEPING_AID;
            }
            return PCVServiceType::LANE_KEEPING_AID;
        case ipi::ServiceType::UnprotectedLeftAvailability:
            if (uses_pcav_contract(role)) {
                return PCAVServiceType::UNPROTECTED_LEFT_TURN_AVAILABILITY;
            }
            return PCVServiceType::UNPROTECTED_LEFT_TURN_AVAILABILITY;
        case ipi::ServiceType::PerceptionAid:
            if (!uses_pcav_contract(role)) {
                throw std::invalid_argument("perceptionAid requires a PCAV vehicle profile");
            }
            return PCAVServiceType::PERCEPTION_AID;
        case ipi::ServiceType::PlanningAid:
            if (!uses_pcav_contract(role)) {
                throw std::invalid_argument("planningAid requires a PCAV vehicle profile");
            }
            return PCAVServiceType::PLANNING_AID;
        case ipi::ServiceType::ControlAid:
            if (!uses_pcav_contract(role)) {
                throw std::invalid_argument("controlAid requires a PCAV vehicle profile");
            }
            return PCAVServiceType::CONTROL_AID;
        case ipi::ServiceType::ComputationAid:
            if (!uses_pcav_contract(role)) {
                throw std::invalid_argument("computationAid requires a PCAV vehicle profile");
            }
            return PCAVServiceType::COMPUTATION_AID;
        case ipi::ServiceType::HdMapUpdate:
            if (!uses_pcav_contract(role)) {
                throw std::invalid_argument("hdMapUpdate requires a PCAV vehicle profile");
            }
            return PCAVServiceType::HDMAP_LANELET_UPDATE;
        default:
            throw std::invalid_argument("unknown IPI service type");
    }
}

} // namespace

Edge4AvInterface::Edge4AvInterface(std::shared_ptr<ReceiverApi> receiver,
                                   std::shared_ptr<SenderApi> sender,
                                   std::shared_ptr<PrivateSessionTransport> privateSessionTransport,
                                   v2x::UperCodec codec)
    : receiver_(std::move(receiver)),
      sender_(std::move(sender)),
      privateSessionTransport_(std::move(privateSessionTransport)),
      codec_(std::move(codec)) {
    if (!receiver_) {
        throw std::invalid_argument("Edge4AvInterface requires a ReceiverApi");
    }
    if (!sender_) {
        throw std::invalid_argument("Edge4AvInterface requires a SenderApi");
    }
    if (!privateSessionTransport_) {
        privateSessionTransport_ = make_in_memory_private_session_transport(receiver_, sender_);
    }
}

SessionDescriptor Edge4AvInterface::register_session(SessionRegistration registration) const {
    registration.metadata = normalize_metadata(std::move(registration.metadata));
    if (registration.metadata.source.id.empty()) {
        registration.metadata.source.id = registration.vehicleProfile.vehicleId;
    }
    registration.metadata.source.type = source_type_for_role(registration.vehicleProfile.role);
    return privateSessionTransport_->register_session(registration);
}

SessionDescriptor Edge4AvInterface::register_vehicle_session(
    EnvelopeMetadata metadata,
    VehicleProfile vehicleProfile,
    std::vector<std::string> requestedServices,
    std::optional<std::string> rsuFallback,
    std::optional<int> minSidelinkRssi,
    std::optional<Subscription> inlineSubscription) const {
    SessionRegistration registration;
    registration.metadata = std::move(metadata);
    registration.vehicleProfile = std::move(vehicleProfile);
    registration.requestedServices = std::move(requestedServices);
    registration.rsuFallback = std::move(rsuFallback);
    registration.minSidelinkRssi = minSidelinkRssi;
    registration.inlineSubscription = std::move(inlineSubscription);
    return register_session(std::move(registration));
}

Ack Edge4AvInterface::heartbeat(HeartbeatUpdate heartbeat) const {
    if (is_default_timestamp(heartbeat.timestamp)) {
        heartbeat.timestamp = std::chrono::system_clock::now();
    }
    return privateSessionTransport_->heartbeat(heartbeat);
}

Ack Edge4AvInterface::heartbeat(const std::string& sessionId,
                                std::optional<VehicleTelemetryFrame> telemetry) const {
    HeartbeatUpdate heartbeatUpdate;
    heartbeatUpdate.sessionId = sessionId;
    heartbeatUpdate.timestamp = std::chrono::system_clock::now();
    heartbeatUpdate.telemetry = std::move(telemetry);
    return heartbeat(std::move(heartbeatUpdate));
}

Ack Edge4AvInterface::submit_service_request(EnvelopeMetadata metadata,
                                             const VehicleProfile& vehicleProfile,
                                             const ipi::IpiServiceRequest& request,
                                             ServiceRequestContext context) const {
    try {
        auto encodedRequest = request.to_canonical_encoding();
        auto translatedService = translate_service_type(request.serviceType, vehicleProfile.role);

        Envelope<VehicleServiceRequest> envelope;
        envelope.metadata = normalize_metadata(std::move(metadata));

        if (context.vehicleId.empty()) {
            context.vehicleId = vehicleProfile.vehicleId;
        }
        if (context.vehicleId.empty()) {
            return Ack{false, std::string{"vehicleId is required for service submission"}};
        }

        if (envelope.metadata.source.id.empty()) {
            envelope.metadata.source.id = context.vehicleId;
        }
        envelope.metadata.source.type = source_type_for_role(vehicleProfile.role);
        if (!envelope.metadata.correlationId) {
            envelope.metadata.correlationId = std::to_string(request.requestId);
        }

        envelope.data.serviceType = std::move(translatedService);
        envelope.data.vehicleId = std::move(context.vehicleId);
        envelope.data.vin = context.vin ? std::move(context.vin) : vehicleProfile.vin;
        envelope.data.location = std::move(context.location);
        envelope.data.speedMps = context.speedMps;
        envelope.data.headingDegrees = context.headingDegrees;
        envelope.data.context = std::move(encodedRequest);

        if (envelope.metadata.sessionId) {
            return privateSessionTransport_->invoke_service(
                ServiceInvocation{*envelope.metadata.sessionId, std::move(envelope)});
        }

        if (std::holds_alternative<PCVServiceType>(envelope.data.serviceType)) {
            return receiver_->submitPCVRequest(envelope);
        }
        return receiver_->submitPCAVRequest(envelope);
    } catch (const std::exception& ex) {
        return Ack{false, std::string{ex.what()}};
    }
}

Ack Edge4AvInterface::submit_telemetry(TelemetrySubmission submission) const {
    return privateSessionTransport_->submit_telemetry(submission);
}

Ack Edge4AvInterface::submit_telemetry(const std::string& sessionId,
                                       std::vector<VehicleTelemetryFrame> frames) const {
    TelemetrySubmission submission;
    submission.sessionId = sessionId;
    submission.frames = std::move(frames);
    return submit_telemetry(std::move(submission));
}

std::vector<Envelope<VehicleServiceResponse>> Edge4AvInterface::list_vehicle_responses(
    const std::string& vehicleId,
    std::optional<std::variant<PCVServiceType, PCAVServiceType>> serviceType,
    std::optional<Timestamp> since) const {
    ResponseQuery query;
    query.vehicleId = vehicleId;
    query.serviceType = std::move(serviceType);
    query.since = since;
    return sender_->listPcvResponses(query);
}

std::vector<Envelope<VehicleServiceResponse>> Edge4AvInterface::list_session_responses(
    const std::string& sessionId) const {
    return privateSessionTransport_->list_session_responses(SessionResponseQuery{sessionId});
}

const v2x::UperCodec& Edge4AvInterface::codec() const noexcept {
    return codec_;
}

const std::shared_ptr<PrivateSessionTransport>& Edge4AvInterface::private_session_transport() const noexcept {
    return privateSessionTransport_;
}

EnvelopeMetadata Edge4AvInterface::normalize_metadata(EnvelopeMetadata metadata) const {
    if (metadata.messageId.empty()) {
        metadata.messageId = make_identifier("edge4av-msg");
    }
    if (is_default_timestamp(metadata.sentAt)) {
        metadata.sentAt = std::chrono::system_clock::now();
    }
    return metadata;
}

std::string Edge4AvInterface::make_identifier(const char* prefix) const {
    std::ostringstream oss;
    oss << prefix << '-' << ++messageCounter_;
    return oss.str();
}

} // namespace ipi::api
