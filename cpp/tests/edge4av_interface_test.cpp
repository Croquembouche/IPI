#include "test_support.hpp"

#include "ipi/api/edge4av_interface.hpp"

int main() {
    return ipi::tests::run_test("edge4av_interface_dual_plane", [] {
        using ipi::IpiServiceRequest;
        using ipi::ServiceType;
        using ipi::api::Edge4AvInterface;
        using ipi::api::EnvelopeMetadata;
        using ipi::api::ServiceRequestContext;
        using ipi::api::SourceType;
        using ipi::api::TransportType;
        using ipi::api::VehicleProfile;
        using ipi::api::VehicleRole;
        using ipi::api::VehicleTelemetryFrame;
        using ipi::j2735::BasicSafetyMessage;

        Edge4AvInterface edge4av;

        EnvelopeMetadata radioMetadata;
        radioMetadata.intersectionId = "int-1";
        radioMetadata.transport = TransportType::C_V2X;
        radioMetadata.source.type = SourceType::PCAV;
        radioMetadata.source.id = "veh-1";

        BasicSafetyMessage bsm{};
        bsm.vehicleId = 0xAABBCCDD;
        bsm.latitude = 42.0;
        bsm.longitude = -83.0;
        bsm.speedMps = 12.0F;
        bsm.headingDeg = 90.0F;

        ipi::tests::expect(edge4av.ingest_v2x_message(radioMetadata, bsm).accepted,
                           "radio ingest should succeed");
        const auto bsmMessages = edge4av.list_v2x_messages<BasicSafetyMessage>();
        ipi::tests::expect(!bsmMessages.empty(), "BSM should be retrievable after ingest");
        ipi::tests::expect(bsmMessages.back().vehicleId == bsm.vehicleId,
                           "retrieved BSM should match stored vehicle id");

        VehicleProfile profile;
        profile.vehicleId = "veh-1";
        profile.role = VehicleRole::PCAV;

        EnvelopeMetadata sessionMetadata;
        sessionMetadata.intersectionId = "int-1";
        sessionMetadata.transport = TransportType::CELLULAR_5G;

        auto session = edge4av.register_vehicle_session(sessionMetadata, profile, {"planningAid"});
        ipi::tests::expect(!session.sessionId.empty(), "session registration should produce a session id");

        auto heartbeatAck = edge4av.heartbeat(session.sessionId, VehicleTelemetryFrame{});
        ipi::tests::expect(heartbeatAck.accepted, "session heartbeat should succeed");

        IpiServiceRequest request;
        request.serviceType = ServiceType::PlanningAid;
        request.requestId = 17;
        request.desiredHorizonMs = 2500;
        request.additionalData = {'p', 'l', 'a', 'n'};

        sessionMetadata.sessionId = session.sessionId;
        ServiceRequestContext context;
        context.vehicleId = profile.vehicleId;
        context.location.latitude = 42.0;
        context.location.longitude = -83.0;
        context.speedMps = 12.0;
        context.headingDegrees = 90.0;

        auto serviceAck = edge4av.submit_service_request(sessionMetadata, profile, request, context);
        ipi::tests::expect(serviceAck.accepted, "session service request should succeed");

        VehicleTelemetryFrame telemetry{};
        telemetry.pose.latitude = 42.1;
        telemetry.pose.longitude = -83.1;
        ipi::tests::expect(edge4av.submit_telemetry(session.sessionId, {telemetry}).accepted,
                           "session telemetry should succeed");

        const auto sessionResponses = edge4av.list_session_responses(session.sessionId);
        ipi::tests::expect(sessionResponses.size() == 1, "session invocation should produce one session response");

        const auto publications = edge4av.private_session_transport()->list_publications({});
        ipi::tests::expect(publications.size() >= 4,
                           "register, heartbeat, service request, and telemetry publications should be recorded");
    });
}
