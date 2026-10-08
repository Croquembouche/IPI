#include "test_support.hpp"

#include "ipi/api/private_session_transport.hpp"
#include "ipi/api/session_wire_codec.hpp"

#include <chrono>
#include <stdexcept>

namespace {

template <typename Callable>
void expect_throw(Callable callable, const std::string& message) {
    bool threw = false;
    try {
        callable();
    } catch (const std::exception&) {
        threw = true;
    }
    ipi::tests::expect(threw, message);
}

} // namespace

int main() {
    return ipi::tests::run_test("session_wire_codec_strictness", [] {
        using namespace std::chrono_literals;
        using namespace ipi::api;

        EnvelopeMetadata metadata;
        metadata.messageId = "message-1";
        metadata.sentAt = Timestamp{100s};
        metadata.intersectionId = "intersection-1";
        metadata.transport = TransportType::CELLULAR_5G;
        metadata.source.type = SourceType::PCAV;
        metadata.source.id = "vehicle-1";

        SessionRegistration registration;
        registration.metadata = metadata;
        registration.vehicleProfile.vehicleId = "vehicle-1";
        registration.vehicleProfile.role = VehicleRole::PCAV;
        registration.requestedServices = {"planningAid"};
        registration.rsuFallback = "rsu-2";
        registration.minSidelinkRssi = -92;
        registration.inlineSubscription = Subscription{"callback", {"update"}, {7}};

        const auto registerTopic = SessionTopic{SessionTopicKind::Register,
                                                "intersection-1", std::nullopt,
                                                std::nullopt}.to_string();
        const auto registrationWire = make_session_registration_wire_message(
            registerTopic, registration);
        const auto registrationRoundTrip = decode_session_wire_message(
            encode_session_wire_message(registrationWire));
        const auto decodedRegistration = decode_session_registration_payload(
            registrationRoundTrip);
        ipi::tests::expect(decodedRegistration.vehicleProfile.vehicleId == "vehicle-1",
                           "registration vehicle must round-trip");
        ipi::tests::expect(decodedRegistration.rsuFallback == registration.rsuFallback,
                           "registration fallback must round-trip");

        SessionDescriptor descriptor;
        descriptor.sessionId = "session-1";
        descriptor.vehicleProfile = registration.vehicleProfile;
        descriptor.transport = TransportType::CELLULAR_5G;
        descriptor.state = SessionState::ACTIVE;
        descriptor.leaseSeconds = 30;
        descriptor.heartbeatIntervalSeconds = 5;
        descriptor.grantedServices = {"planningAid"};
        descriptor.registeredAt = Timestamp{100s};
        descriptor.lastHeartbeatAt = Timestamp{101s};
        descriptor.expiresAt = Timestamp{131s};
        descriptor.rsuFallback = registration.rsuFallback;
        descriptor.minSidelinkRssi = registration.minSidelinkRssi;
        auto eventMetadata = metadata;
        eventMetadata.messageId = "event-1";
        eventMetadata.sessionId = descriptor.sessionId;
        eventMetadata.correlationId = metadata.messageId;
        const auto eventTopic = SessionTopic{SessionTopicKind::Events,
                                             "intersection-1", std::nullopt,
                                             "vehicle-1"}.to_string();
        const auto descriptorWire = make_session_descriptor_wire_message(
            eventTopic, eventMetadata, descriptor);
        ipi::tests::expect(
            decode_session_descriptor_payload(decode_session_wire_message(
                encode_session_wire_message(descriptorWire))).sessionId == descriptor.sessionId,
            "descriptor must round-trip");

        HeartbeatUpdate heartbeat{descriptor.sessionId, Timestamp{102s}, VehicleTelemetryFrame{}};
        auto operationMetadata = metadata;
        operationMetadata.messageId = "heartbeat-1";
        operationMetadata.sessionId = descriptor.sessionId;
        const auto heartbeatTopic = SessionTopic{SessionTopicKind::Heartbeat,
                                                 "intersection-1", descriptor.sessionId,
                                                 std::nullopt}.to_string();
        const auto heartbeatWire = make_heartbeat_wire_message(
            heartbeatTopic, operationMetadata, heartbeat);
        ipi::tests::expect(decode_heartbeat_payload(decode_session_wire_message(
                               encode_session_wire_message(heartbeatWire))).telemetry.has_value(),
                           "heartbeat telemetry must round-trip");

        SessionPatch patch;
        patch.sessionId = descriptor.sessionId;
        patch.preferredChannels = std::vector<std::string>{"planningAid", "perceptionAid"};
        const auto patchWire = make_session_patch_wire_message(
            SessionTopic{SessionTopicKind::Patch, "intersection-1",
                         descriptor.sessionId, std::nullopt}.to_string(),
            operationMetadata, patch);
        ipi::tests::expect(decode_session_patch_payload(decode_session_wire_message(
                               encode_session_wire_message(patchWire))).preferredChannels->size() == 2,
                           "patch must round-trip");

        SessionTermination termination{descriptor.sessionId,
                                       TerminationReasonCode::ADMINISTRATIVE,
                                       "maintenance"};
        const auto terminationWire = make_session_termination_wire_message(
            SessionTopic{SessionTopicKind::Terminate, "intersection-1",
                         descriptor.sessionId, std::nullopt}.to_string(),
            operationMetadata, termination);
        ipi::tests::expect(decode_session_termination_payload(decode_session_wire_message(
                               encode_session_wire_message(terminationWire))).reasonCode ==
                               TerminationReasonCode::ADMINISTRATIVE,
                           "termination reason must round-trip");

        ServiceInvocation invocation;
        invocation.sessionId = descriptor.sessionId;
        invocation.request.metadata = operationMetadata;
        invocation.request.metadata.messageId = "service-1";
        invocation.request.metadata.correlationId = "request-1";
        invocation.request.data.serviceType = PCAVServiceType::PLANNING_AID;
        invocation.request.data.vehicleId = "vehicle-1";
        invocation.request.data.context = {1, 2, 3};
        const auto requestWire = make_service_invocation_wire_message(
            SessionTopic{SessionTopicKind::ServiceRequest, "intersection-1",
                         descriptor.sessionId, std::nullopt}.to_string(), invocation);
        ipi::tests::expect(decode_service_invocation_payload(decode_session_wire_message(
                               encode_session_wire_message(requestWire))).request.data.context.size() == 3,
                           "service invocation must round-trip");

        Envelope<VehicleServiceResponse> response;
        response.metadata = invocation.request.metadata;
        response.data.serviceType = invocation.request.data.serviceType;
        response.data.vehicleId = "vehicle-1";
        response.data.status = VehicleServiceStatus::COMPLETED;
        response.data.resultId = "result-1";
        response.data.guidance = {9, 8};
        const auto responseWire = make_service_response_wire_message(
            SessionTopic{SessionTopicKind::ServiceUpdate, "intersection-1",
                         descriptor.sessionId, std::nullopt}.to_string(), response);
        ipi::tests::expect(decode_service_response_payload(decode_session_wire_message(
                               encode_session_wire_message(responseWire))).data.resultId == "result-1",
                           "service response must round-trip");

        auto fallbackResponse = response;
        fallbackResponse.metadata.messageId = "fallback-1";
        fallbackResponse.data.status = VehicleServiceStatus::FALLBACK_COMPLETED;
        fallbackResponse.data.resultId = "fallback-result-1";
        fallbackResponse.data.fallbackResult = FallbackResult::SUCCEEDED;
        const auto fallbackWire = make_service_response_wire_message(
            SessionTopic{SessionTopicKind::ServiceUpdate, "intersection-1",
                         descriptor.sessionId, std::nullopt}.to_string(), fallbackResponse);
        const auto decodedFallback = decode_service_response_payload(
            decode_session_wire_message(encode_session_wire_message(fallbackWire)));
        ipi::tests::expect(
            decodedFallback.data.status == VehicleServiceStatus::FALLBACK_COMPLETED &&
                decodedFallback.data.fallbackResult == FallbackResult::SUCCEEDED,
            "terminal fallback result must round-trip in the service-response body");

        TelemetrySubmission telemetry{descriptor.sessionId, {VehicleTelemetryFrame{}, VehicleTelemetryFrame{}}};
        const auto telemetryWire = make_telemetry_wire_message(
            SessionTopic{SessionTopicKind::Telemetry, "intersection-1",
                         descriptor.sessionId, std::nullopt}.to_string(),
            operationMetadata, telemetry);
        ipi::tests::expect(decode_telemetry_payload(decode_session_wire_message(
                               encode_session_wire_message(telemetryWire))).frames.size() == 2,
                           "telemetry must round-trip");

        const Ack rejection{false, std::nullopt, FailureCode::ACCESS_DENIED, "denied"};
        const auto ackWire = make_ack_wire_message(eventTopic, eventMetadata, rejection);
        ipi::tests::expect(decode_ack_payload(decode_session_wire_message(
                               encode_session_wire_message(ackWire))).code == FailureCode::ACCESS_DENIED,
                           "typed rejection must round-trip");

        auto trailing = encode_session_wire_message(registrationWire);
        trailing.push_back(0);
        expect_throw([&] { (void)decode_session_wire_message(trailing); },
                     "wire decoder must reject trailing bytes");
        expect_throw([&] { (void)SessionTopic::parse("ipi/int/session/+/heartbeat"); },
                     "topic parser must reject wildcard identifiers");
        expect_throw([&] {
            auto mismatch = registrationWire;
            mismatch.kind = SessionWireKind::HEARTBEAT;
            (void)encode_session_wire_message(mismatch);
        }, "wire encoder must reject kind/topic mismatch");
        expect_throw([&] {
            auto invalid = registrationWire;
            invalid.metadata.transport = static_cast<TransportType>(255);
            (void)encode_session_wire_message(invalid);
        }, "wire encoder must reject unknown metadata enums");
        expect_throw([&] {
            auto invalid = registration;
            invalid.vehicleProfile.role = static_cast<VehicleRole>(255);
            (void)make_session_registration_wire_message(registerTopic, invalid);
        }, "wire encoder must reject unknown payload enums");
        expect_throw([&] {
            auto invalid = registration;
            invalid.metadata.source.id = "vehicle-2";
            (void)make_session_registration_wire_message(registerTopic, invalid);
        }, "registration source and vehicle identity must agree");
        expect_throw([&] {
            (void)make_session_descriptor_wire_message(
                SessionTopic{SessionTopicKind::Events, "intersection-1", std::nullopt,
                             "vehicle-2"}.to_string(),
                eventMetadata, descriptor);
        }, "descriptor event topic and vehicle identity must agree");

        auto impossible = response.data;
        impossible.status = VehicleServiceStatus::TIMED_OUT;
        impossible.failureCode = FailureCode::NONE;
        expect_throw([&] { validate_vehicle_service_response(impossible); },
                     "impossible service status/failure combinations must be rejected");
    });
}
