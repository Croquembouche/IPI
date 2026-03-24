#include "test_support.hpp"

#include "ipi/api/private_5g_latency_probe.hpp"

int main() {
    return ipi::tests::run_test("private_5g_latency_probe_codec", [] {
        using ipi::api::Private5gProbeAck;
        using ipi::api::Private5gProbeRequest;

        ipi::CooperativeServiceMessage message;
        message.sessionId[0] = 0x42;
        message.vehicleId = {'v', 'e', 'h', '-', '1'};
        message.serviceClass = ipi::ServiceClass::GuidedPlanning;
        message.guidanceStatus = ipi::GuidanceStatus::Request;
        message.requestedHorizonMs = 1500;

        ipi::GuidedPlanningPayload planning;
        ipi::Waypoint waypoint;
        waypoint.position.latitude = 42.0;
        waypoint.position.longitude = -83.0;
        waypoint.targetSpeedMps = 10.0;
        planning.waypoints.push_back(waypoint);
        message.planning = planning;
        message.offloadPayload = std::vector<std::uint8_t>{0x10, 0x11, 0x12};

        Private5gProbeRequest request;
        request.sequence = 7;
        request.clientSendTimeNs = 1000;
        request.intersectionId = "int-1";
        request.sourceId = "veh-1";
        request.sessionId = "sess-7";
        request.frame = ipi::api::make_private_5g_probe_frame(message);

        const auto encodedRequest = ipi::api::encode_private_5g_probe_request(request);
        const auto decodedRequest = ipi::api::decode_private_5g_probe_request(encodedRequest);
        ipi::tests::expect(decodedRequest.sequence == request.sequence, "sequence should round-trip");
        ipi::tests::expect(decodedRequest.intersectionId == request.intersectionId,
                           "intersectionId should round-trip");
        ipi::tests::expect(decodedRequest.sessionId == request.sessionId, "sessionId should round-trip");
        ipi::tests::expect(decodedRequest.frame.type == request.frame.type, "frame type should round-trip");
        ipi::tests::expect(decodedRequest.frame.payload == request.frame.payload,
                           "frame payload should round-trip");

        Private5gProbeAck ack;
        ack.sequence = request.sequence;
        ack.clientSendTimeNs = request.clientSendTimeNs;
        ack.serverReceiveTimeNs = 1200;
        ack.serverSendTimeNs = 1300;
        ack.frameType = request.frame.type;
        ack.payloadSize = static_cast<std::uint32_t>(request.frame.payload.size());
        ack.accepted = true;
        ack.detail = "accepted";

        const auto encodedAck = ipi::api::encode_private_5g_probe_ack(ack);
        const auto decodedAck = ipi::api::decode_private_5g_probe_ack(encodedAck);
        ipi::tests::expect(decodedAck.sequence == ack.sequence, "ack sequence should round-trip");
        ipi::tests::expect(decodedAck.accepted == ack.accepted, "ack accepted should round-trip");
        ipi::tests::expect(decodedAck.payloadSize == ack.payloadSize, "ack payload size should round-trip");
        ipi::tests::expect(decodedAck.detail == ack.detail, "ack detail should round-trip");

        const auto metrics = ipi::api::compute_private_5g_latency_metrics(decodedAck, 1500);
        ipi::tests::expect(metrics.roundTripNs == 500, "round-trip latency should be computed");
        ipi::tests::expect(metrics.serverProcessingNs == 100, "server processing latency should be computed");
        ipi::tests::expect(metrics.uplinkNs && *metrics.uplinkNs == 200, "uplink latency should be computed");
        ipi::tests::expect(metrics.downlinkNs && *metrics.downlinkNs == 200,
                           "downlink latency should be computed");
    });
}
