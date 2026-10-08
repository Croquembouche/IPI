#include "ipi/api/edge4av_interface.hpp"

#include <chrono>
#include <iostream>

int main() {
    using ipi::api::IpiInterface;
    using ipi::api::EnvelopeMetadata;
    using ipi::api::ServiceRequestContext;
    using ipi::api::SourceType;
    using ipi::api::TransportType;
    using ipi::api::VehicleProfile;
    using ipi::api::VehicleRole;
    using ipi::api::VehicleTelemetryFrame;
    using ipi::j2735::BasicSafetyMessage;

    try {
        IpiInterface ipiInterface;

        EnvelopeMetadata radioMetadata;
        radioMetadata.intersectionId = "intersection-101";
        radioMetadata.transport = TransportType::C_V2X;
        radioMetadata.source.type = SourceType::PCAV;
        radioMetadata.source.id = "veh-01";

        BasicSafetyMessage bsm{};
        bsm.vehicleId = 0x01020304;
        bsm.latitude = 42.3314;
        bsm.longitude = -83.0458;
        bsm.speedMps = 8.5F;
        bsm.headingDeg = 90.0F;

        auto ingestAck = ipiInterface.ingest_v2x_message(radioMetadata, bsm, -58, 172);
        std::cout << "radio ingest accepted: " << std::boolalpha << ingestAck.accepted << '\n';

        auto receivedBsms = ipiInterface.list_v2x_messages<BasicSafetyMessage>();
        std::cout << "stored BSM count: " << receivedBsms.size() << '\n';

        VehicleProfile profile;
        profile.vehicleId = "veh-01";
        profile.role = VehicleRole::PCAV;
        profile.supportsStreaming = true;

        EnvelopeMetadata sessionMetadata;
        sessionMetadata.intersectionId = "intersection-101";
        sessionMetadata.transport = TransportType::CELLULAR_5G;

        auto session = ipiInterface.register_vehicle_session(sessionMetadata,
                                                        profile,
                                                        {"planningAid", "perceptionAid"});
        std::cout << "registered session: " << session.sessionId << '\n';
        sessionMetadata.sessionId = session.sessionId;

        auto heartbeatAck = ipiInterface.heartbeat(session.sessionId, VehicleTelemetryFrame{});
        std::cout << "session heartbeat accepted: " << std::boolalpha << heartbeatAck.accepted << '\n';

        ipi::CooperativeServiceMessage operation;
        operation.vehicleId = {'v', 'e', 'h', '-', '0', '1'};
        operation.serviceClass = ipi::ServiceClass::GuidedPlanning;
        operation.guidanceStatus = ipi::GuidanceStatus::Request;
        operation.requestedHorizonMs = 3000;
        operation.offloadPayload =
            std::vector<std::uint8_t>{'f', 'a', 'l', 'l', 'b', 'a', 'c', 'k'};
        operation.offloadTaskId = "planning-7";

        ServiceRequestContext context;
        context.vehicleId = profile.vehicleId;
        context.location.latitude = 42.3314;
        context.location.longitude = -83.0458;
        context.speedMps = 8.5;
        context.headingDegrees = 90.0;

        auto requestAck = ipiInterface.submit_cooperative_service(
            sessionMetadata, profile, operation, context);
        std::cout << "private service request accepted: " << std::boolalpha << requestAck.accepted << '\n';

        VehicleTelemetryFrame telemetry{};
        telemetry.pose.latitude = context.location.latitude;
        telemetry.pose.longitude = context.location.longitude;
        auto telemetryAck = ipiInterface.submit_telemetry(session.sessionId, {telemetry});
        std::cout << "session telemetry accepted: " << std::boolalpha << telemetryAck.accepted << '\n';

        auto sessionResponses = ipiInterface.list_session_responses(session.sessionId);
        std::cout << "session response count: " << sessionResponses.size() << '\n';
        auto publications = ipiInterface.private_session_transport()->list_publications({});
        std::cout << "session publication count: " << publications.size() << '\n';

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
