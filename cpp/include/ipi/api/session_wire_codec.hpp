#pragma once

#include "ipi/api/types.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ipi::api {

enum class SessionWireKind : std::uint8_t {
    REGISTER = 1,
    HEARTBEAT = 2,
    PATCH = 3,
    TERMINATE = 4,
    SERVICE_REQUEST = 5,
    SERVICE_UPDATE = 6,
    EVENT = 7,
    TELEMETRY = 8,
    PCV_RESPONSE = 9
};

/**
 * Broker record shared by all IPI session topics. The payload is an operation-
 * specific binary body; the common envelope remains stable and independently
 * decodable so brokers, recorders, and access-control gateways can route it.
 */
struct SessionWireMessage {
    SessionWireKind kind{SessionWireKind::REGISTER};
    std::string topic{};
    EnvelopeMetadata metadata{};
    std::string payloadType{};
    std::vector<std::uint8_t> payload{};
};

[[nodiscard]] std::vector<std::uint8_t> encode_session_wire_message(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage decode_session_wire_message(
    const std::vector<std::uint8_t>& buffer);

/** Verify kind, topic, metadata, and payload type agree before dispatch. */
void validate_session_wire_message(const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_session_registration_wire_message(
    const std::string& topic,
    const SessionRegistration& registration);
[[nodiscard]] SessionRegistration decode_session_registration_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_session_descriptor_wire_message(
    const std::string& topic,
    EnvelopeMetadata metadata,
    const SessionDescriptor& descriptor);
[[nodiscard]] SessionDescriptor decode_session_descriptor_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_heartbeat_wire_message(
    const std::string& topic,
    EnvelopeMetadata metadata,
    const HeartbeatUpdate& heartbeat);
[[nodiscard]] HeartbeatUpdate decode_heartbeat_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_session_patch_wire_message(
    const std::string& topic,
    EnvelopeMetadata metadata,
    const SessionPatch& patch);
[[nodiscard]] SessionPatch decode_session_patch_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_session_termination_wire_message(
    const std::string& topic,
    EnvelopeMetadata metadata,
    const SessionTermination& termination);
[[nodiscard]] SessionTermination decode_session_termination_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_service_invocation_wire_message(
    const std::string& topic,
    const ServiceInvocation& invocation);
[[nodiscard]] ServiceInvocation decode_service_invocation_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_service_response_wire_message(
    const std::string& topic,
    const Envelope<VehicleServiceResponse>& response);
[[nodiscard]] Envelope<VehicleServiceResponse> decode_service_response_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_telemetry_wire_message(
    const std::string& topic,
    EnvelopeMetadata metadata,
    const TelemetrySubmission& submission);
[[nodiscard]] TelemetrySubmission decode_telemetry_payload(
    const SessionWireMessage& message);

[[nodiscard]] SessionWireMessage make_ack_wire_message(
    const std::string& topic,
    EnvelopeMetadata metadata,
    const Ack& acknowledgement);
[[nodiscard]] Ack decode_ack_payload(const SessionWireMessage& message);

} // namespace ipi::api
