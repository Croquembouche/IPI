#include "test_support.hpp"

#include "ipi/api/edge4av_interface.hpp"

#include <algorithm>

int main() {
    return ipi::tests::run_test("edge4av_interface_dual_plane", [] {
        using ipi::IpiServiceRequest;
        using ipi::ServiceType;
        using ipi::api::IpiInterface;
        using ipi::api::EnvelopeMetadata;
        using ipi::api::ServiceRequestContext;
        using ipi::api::SourceType;
        using ipi::api::TransportType;
        using ipi::api::VehicleProfile;
        using ipi::api::VehicleRole;
        using ipi::api::VehicleTelemetryFrame;
        using ipi::j2735::BasicSafetyMessage;

        IpiInterface ipiInterface;

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

        ipi::tests::expect(ipiInterface.ingest_v2x_message(radioMetadata, bsm).accepted,
                           "radio ingest should succeed");
        const auto bsmMessages = ipiInterface.list_v2x_messages<BasicSafetyMessage>();
        ipi::tests::expect(!bsmMessages.empty(), "BSM should be retrievable after ingest");
        ipi::tests::expect(bsmMessages.back().vehicleId == bsm.vehicleId,
                           "retrieved BSM should match stored vehicle id");

        VehicleProfile profile;
        profile.vehicleId = "veh-1";
        profile.role = VehicleRole::PCAV;

        EnvelopeMetadata sessionMetadata;
        sessionMetadata.intersectionId = "int-1";
        sessionMetadata.transport = TransportType::CELLULAR_5G;

        auto session = ipiInterface.register_vehicle_session(sessionMetadata, profile, {"planningAid"});
        ipi::tests::expect(!session.sessionId.empty(), "session registration should produce a session id");

        auto heartbeatAck = ipiInterface.heartbeat(session.sessionId, VehicleTelemetryFrame{});
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

        auto serviceAck = ipiInterface.submit_service_request(sessionMetadata, profile, request, context);
        ipi::tests::expect(serviceAck.accepted, "session service request should succeed");

        ipi::CooperativeServiceMessage operation;
        operation.sessionId.fill(0x7aU);
        operation.vehicleId = {'v', 'e', 'h', '-', '1'};
        operation.serviceClass = ipi::ServiceClass::GuidedPlanning;
        operation.guidanceStatus = ipi::GuidanceStatus::Request;
        operation.requestedHorizonMs = 2500;
        operation.offloadPayload = std::vector<std::uint8_t>(128U * 1024U, 0x5aU);
        operation.offloadTaskId = "operation-18";
        auto operationAck = ipiInterface.submit_cooperative_service(
            sessionMetadata, profile, operation, context);
        ipi::tests::expect(operationAck.accepted,
                           "distinct IPIS and operation-session identifiers should succeed");

        VehicleTelemetryFrame telemetry{};
        telemetry.pose.latitude = 42.1;
        telemetry.pose.longitude = -83.1;
        ipi::tests::expect(ipiInterface.submit_telemetry(session.sessionId, {telemetry}).accepted,
                           "session telemetry should succeed");

        const auto sessionResponses = ipiInterface.list_session_responses(session.sessionId);
        ipi::tests::expect(sessionResponses.size() == 4,
                           "two session invocations should each produce progress and terminal responses");
        ipi::tests::expect(
            sessionResponses.front().data.status == ipi::api::VehicleServiceStatus::IN_PROGRESS,
            "first session response should report progress");
        ipi::tests::expect(
            sessionResponses.back().data.status == ipi::api::VehicleServiceStatus::COMPLETED,
            "last session response should be terminal");
        const auto vehicleResponses = ipiInterface.list_vehicle_responses(profile.vehicleId);
        ipi::tests::expect(vehicleResponses.size() == 4,
                           "vehicle response lookup should retain both operations' results");

        ipi::api::SessionPatch patch;
        patch.sessionId = session.sessionId;
        auto patchedProfile = profile;
        patchedProfile.softwareVersion = "year2-demo";
        patch.profile = patchedProfile;
        ipi::tests::expect(ipiInterface.patch_session(patch).accepted,
                           "session profile patch should succeed");
        ipi::tests::expect(
            ipiInterface.get_session(session.sessionId)->vehicleProfile.softwareVersion ==
                patchedProfile.softwareVersion,
            "session patch should be visible through the facade");

        ipi::api::SessionTermination termination;
        termination.sessionId = session.sessionId;
        termination.reasonCode = ipi::api::TerminationReasonCode::CLIENT_REQUEST;
        termination.reason = "test complete";
        ipi::tests::expect(ipiInterface.terminate_session(termination).accepted,
                           "session termination should succeed");
        const auto rejectedTelemetry = ipiInterface.submit_telemetry(
            session.sessionId, {VehicleTelemetryFrame{}});
        ipi::tests::expect(
            !rejectedTelemetry.accepted &&
                rejectedTelemetry.code == ipi::api::FailureCode::SESSION_TERMINATED,
            "terminated session must reject later telemetry");

        const auto publications = ipiInterface.private_session_transport()->list_publications({});
        ipi::tests::expect(publications.size() >= 12,
                           "session operations and service updates should retain wire publications");
        ipi::tests::expect(
            std::all_of(publications.begin(), publications.end(),
                        [](const ipi::api::SessionPublication& publication) {
                            return !publication.encodedPayload.empty();
                        }),
            "every session publication should retain encoded wire bytes");
    });
}
