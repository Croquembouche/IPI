#include "ipi/api/edge4av_interface.hpp"

#include "ipi/api/in_memory_api.hpp"

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

Ack invalid_request(std::string detail) {
    Ack acknowledgement;
    acknowledgement.accepted = false;
    acknowledgement.code = FailureCode::INVALID_REQUEST;
    acknowledgement.detail = std::move(detail);
    return acknowledgement;
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

PCAVServiceType translate_service_class(const ipi::ServiceClass serviceClass) {
    switch (serviceClass) {
        case ipi::ServiceClass::GuidedPlanning:
            return PCAVServiceType::PLANNING_AID;
        case ipi::ServiceClass::GuidedPerception:
            return PCAVServiceType::PERCEPTION_AID;
        case ipi::ServiceClass::GuidedControl:
            return PCAVServiceType::CONTROL_AID;
        default:
            throw std::invalid_argument("unknown IPI cooperative service class");
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
    if (!receiver_ && !sender_) {
        auto pair = make_in_memory_api_pair();
        receiver_ = std::move(pair.receiver);
        sender_ = std::move(pair.sender);
    } else if (!receiver_ || !sender_) {
        throw std::invalid_argument(
            "IpiInterface requires both ReceiverApi and SenderApi, or neither");
    }
    if (!privateSessionTransport_) {
        privateSessionTransport_ = make_in_memory_private_session_transport(receiver_, sender_);
    }
}

Ack Edge4AvInterface::ingest_v2x_payload(EnvelopeMetadata metadata,
                                        J2735Payload payload,
                                        std::optional<int> rssi,
                                        std::optional<int> channel) const {
    if (payload.payload.empty()) {
        return invalid_request("J2735 payload must not be empty");
    }
    Envelope<J2735Payload> envelope;
    envelope.metadata = normalize_metadata(std::move(metadata));
    envelope.data = std::move(payload);
    return receiver_->ingestV2xMessage(envelope, rssi, channel);
}

Ack Edge4AvInterface::request_broadcast_payload(EnvelopeMetadata metadata,
                                                BroadcastTarget target,
                                                J2735Payload payload) const {
    if (payload.payload.empty()) {
        return invalid_request("J2735 payload must not be empty");
    }
    BroadcastRequest request;
    request.metadata = normalize_metadata(std::move(metadata));
    request.target = std::move(target);
    request.message = std::move(payload);
    return receiver_->requestBroadcast(request);
}

std::vector<J2735Payload> Edge4AvInterface::list_v2x_payloads(
    J2735MessageType type,
    std::optional<Timestamp> since,
    std::size_t limit) const {
    V2xQuery query;
    query.type = type;
    query.since = since;
    query.limit = limit;
    return sender_->listV2xMessages(query);
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

Ack Edge4AvInterface::patch_session(SessionPatch patch) const {
    return privateSessionTransport_->patch_session(patch);
}

Ack Edge4AvInterface::terminate_session(SessionTermination termination) const {
    return privateSessionTransport_->terminate_session(termination);
}

std::optional<SessionDescriptor> Edge4AvInterface::get_session(
    const std::string& sessionId) const {
    return privateSessionTransport_->get_session(sessionId);
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
            return invalid_request("vehicleId is required for service submission");
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
        return invalid_request(ex.what());
    }
}

Ack Edge4AvInterface::submit_cooperative_service(
    EnvelopeMetadata metadata,
    const VehicleProfile& vehicleProfile,
    const ipi::CooperativeServiceMessage& operation,
    ServiceRequestContext context) const {
    try {
        if (vehicleProfile.role != VehicleRole::PCAV) {
            return invalid_request("cooperative operations require a CAV vehicle profile");
        }
        operation.validate();

        Envelope<VehicleServiceRequest> envelope;
        envelope.metadata = normalize_metadata(std::move(metadata));

        if (context.vehicleId.empty()) {
            context.vehicleId = vehicleProfile.vehicleId;
        }
        if (context.vehicleId.empty()) {
            return invalid_request("vehicleId is required for cooperative service submission");
        }

        if (envelope.metadata.source.id.empty()) {
            envelope.metadata.source.id = context.vehicleId;
        }
        envelope.metadata.source.type = SourceType::PCAV;
        if (!envelope.metadata.correlationId) {
            envelope.metadata.correlationId = operation.offloadTaskId
                ? *operation.offloadTaskId
                : make_identifier("ipi-operation");
        }

        envelope.data.serviceType = translate_service_class(operation.serviceClass);
        envelope.data.vehicleId = std::move(context.vehicleId);
        envelope.data.vin = context.vin ? std::move(context.vin) : vehicleProfile.vin;
        envelope.data.location = std::move(context.location);
        envelope.data.speedMps = context.speedMps;
        envelope.data.headingDegrees = context.headingDegrees;
        envelope.data.context = operation.to_canonical_encoding();

        if (envelope.metadata.sessionId) {
            return privateSessionTransport_->invoke_service(
                ServiceInvocation{*envelope.metadata.sessionId, std::move(envelope)});
        }
        return receiver_->submitPCAVRequest(envelope);
    } catch (const std::exception& ex) {
        return invalid_request(ex.what());
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

Ack Edge4AvInterface::deliver_service_update(
    const std::string& sessionId,
    Envelope<VehicleServiceResponse> response) const {
    response.metadata.sessionId = sessionId;
    response.metadata = normalize_metadata(std::move(response.metadata));
    return privateSessionTransport_->deliver_service_update(sessionId, response);
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
        metadata.messageId = make_identifier("ipi-msg");
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
