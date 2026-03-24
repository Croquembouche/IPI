#include "test_support.hpp"

#include "ipi/api/private_session_transport.hpp"

int main() {
    return ipi::tests::run_test("private_session_transport_topics", [] {
        using ipi::api::Envelope;
        using ipi::api::EnvelopeMetadata;
        using ipi::api::HeartbeatUpdate;
        using ipi::api::PCAVServiceType;
        using ipi::api::PrivateSessionTransport;
        using ipi::api::ResponseQuery;
        using ipi::api::ServiceInvocation;
        using ipi::api::SessionPublicationQuery;
        using ipi::api::SessionRegistration;
        using ipi::api::SessionResponseQuery;
        using ipi::api::SessionTopic;
        using ipi::api::SessionTopicKind;
        using ipi::api::TelemetrySubmission;
        using ipi::api::TransportType;
        using ipi::api::VehicleProfile;
        using ipi::api::VehicleRole;
        using ipi::api::VehicleServiceRequest;

        auto transport = ipi::api::make_in_memory_private_session_transport();

        SessionTopic requestTopic;
        requestTopic.kind = SessionTopicKind::ServiceRequest;
        requestTopic.intersectionId = "int-2";
        requestTopic.sessionId = "sess-9";
        const auto topicText = requestTopic.to_string();
        const auto parsed = SessionTopic::parse(topicText);
        ipi::tests::expect(parsed.kind == SessionTopicKind::ServiceRequest,
                           "topic parser should preserve kind");
        ipi::tests::expect(parsed.sessionId == requestTopic.sessionId,
                           "topic parser should preserve session id");

        SessionRegistration registration;
        registration.metadata.intersectionId = "int-2";
        registration.metadata.transport = TransportType::CELLULAR_5G;
        registration.vehicleProfile.vehicleId = "veh-9";
        registration.vehicleProfile.role = VehicleRole::PCAV;
        auto session = transport->register_session(registration);

        HeartbeatUpdate heartbeat;
        heartbeat.sessionId = session.sessionId;
        ipi::tests::expect(transport->heartbeat(heartbeat).accepted, "heartbeat should succeed");

        Envelope<VehicleServiceRequest> requestEnvelope;
        requestEnvelope.metadata.intersectionId = "int-2";
        requestEnvelope.metadata.transport = TransportType::CELLULAR_5G;
        requestEnvelope.metadata.sessionId = session.sessionId;
        requestEnvelope.data.serviceType = PCAVServiceType::PLANNING_AID;
        requestEnvelope.data.vehicleId = "veh-9";
        requestEnvelope.data.context = {0x01, 0x02, 0x03};
        ipi::tests::expect(
            transport->invoke_service(ServiceInvocation{session.sessionId, requestEnvelope}).accepted,
            "service invocation should succeed");

        TelemetrySubmission telemetry;
        telemetry.sessionId = session.sessionId;
        telemetry.frames.resize(2);
        ipi::tests::expect(transport->submit_telemetry(telemetry).accepted,
                           "telemetry submission should succeed");

        const auto publications = transport->list_publications(
            SessionPublicationQuery{std::string{"int-2"}, session.sessionId, std::nullopt, 10});
        ipi::tests::expect(publications.size() == 3,
                           "heartbeat, service request, and telemetry should be queryable by session");

        const auto sessionResponses = transport->list_session_responses(SessionResponseQuery{session.sessionId});
        ipi::tests::expect(sessionResponses.size() == 1, "service invocation should produce a session response");
    });
}
