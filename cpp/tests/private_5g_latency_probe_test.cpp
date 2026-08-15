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
        request.expiresAtUnixNs = 5000;
        request.messageId = "message-7";
        request.runId = "run-42";
        request.conditionId = "cond-baseline";
        request.conditionLabel = "private-5g-baseline";
        request.requestId = "req-7";
        request.serviceType = "guided-planning";
        request.intersectionId = "int-1";
        request.sourceId = "veh-1";
        request.sessionId = "sess-7";
        request.networkLoadLevel = "idle";
        request.qosProfile = "default";
        request.mobilityState = "stationary";
        request.clockSyncState = "ptp-synced";
        request.frame = ipi::api::make_private_5g_probe_frame(message);

        const auto encodedRequest = ipi::api::encode_private_5g_probe_request(request);
        const auto decodedRequest = ipi::api::decode_private_5g_probe_request(encodedRequest);
        ipi::tests::expect(decodedRequest.sequence == request.sequence, "sequence should round-trip");
        ipi::tests::expect(decodedRequest.runId == request.runId, "runId should round-trip");
        ipi::tests::expect(decodedRequest.conditionId == request.conditionId, "conditionId should round-trip");
        ipi::tests::expect(decodedRequest.conditionLabel == request.conditionLabel,
                           "conditionLabel should round-trip");
        ipi::tests::expect(decodedRequest.requestId == request.requestId, "requestId should round-trip");
        ipi::tests::expect(decodedRequest.serviceType == request.serviceType, "serviceType should round-trip");
        ipi::tests::expect(decodedRequest.intersectionId == request.intersectionId,
                           "intersectionId should round-trip");
        ipi::tests::expect(decodedRequest.sessionId == request.sessionId, "sessionId should round-trip");
        ipi::tests::expect(decodedRequest.networkLoadLevel == request.networkLoadLevel,
                           "networkLoadLevel should round-trip");
        ipi::tests::expect(decodedRequest.qosProfile == request.qosProfile, "qosProfile should round-trip");
        ipi::tests::expect(decodedRequest.mobilityState == request.mobilityState,
                           "mobilityState should round-trip");
        ipi::tests::expect(decodedRequest.clockSyncState == request.clockSyncState,
                           "clockSyncState should round-trip");
        ipi::tests::expect(decodedRequest.frame.type == request.frame.type, "frame type should round-trip");
        ipi::tests::expect(decodedRequest.frame.payload == request.frame.payload,
                           "frame payload should round-trip");

        auto ack = ipi::api::make_private_5g_probe_ack(
            request, 1200, 1300, 100, true, "accepted", "response-7");

        const auto encodedAck = ipi::api::encode_private_5g_probe_ack(ack);
        const auto decodedAck = ipi::api::decode_private_5g_probe_ack(encodedAck);
        ipi::tests::expect(decodedAck.sequence == ack.sequence, "ack sequence should round-trip");
        ipi::tests::expect(decodedAck.accepted == ack.accepted, "ack accepted should round-trip");
        ipi::tests::expect(decodedAck.payloadSize == ack.payloadSize, "ack payload size should round-trip");
        ipi::tests::expect(decodedAck.detail == ack.detail, "ack detail should round-trip");
        ipi::tests::expect(decodedAck.requestMessageId == request.messageId,
                           "ack request message id should round-trip");
        ipi::tests::expect(decodedAck.requestId == request.requestId,
                           "ack request id should round-trip");
        ipi::tests::expect(decodedAck.sessionId == request.sessionId,
                           "ack session id should round-trip");
        ipi::tests::expect(decodedAck.serverProcessingElapsedNs == 100,
                           "server steady duration should round-trip");

        const auto validation = ipi::api::validate_private_5g_probe_ack(
            request, decodedAck, {}, false, 1500);
        ipi::tests::expect(validation.matched(), "matching ack should validate");

        const auto metrics = ipi::api::compute_private_5g_latency_metrics(
            decodedAck, 500, true, 1500);
        ipi::tests::expect(metrics.roundTripNs == 500, "round-trip latency should be computed");
        ipi::tests::expect(metrics.serverProcessingNs == 100, "server processing latency should be computed");
        ipi::tests::expect(metrics.uplinkNs && *metrics.uplinkNs == 200, "uplink latency should be computed");
        ipi::tests::expect(metrics.downlinkNs && *metrics.downlinkNs == 200,
                           "downlink latency should be computed");

        const auto unsynchronized = ipi::api::compute_private_5g_latency_metrics(
            decodedAck, 500, false, 1500);
        ipi::tests::expect(!unsynchronized.uplinkNs && !unsynchronized.downlinkNs,
                           "unsynchronized clocks must not produce one-way latency");
        const auto wallRollback = ipi::api::compute_private_5g_latency_metrics(
            decodedAck, 500, true, 1100);
        ipi::tests::expect(!wallRollback.downlinkNs,
                           "wall-clock rollback must not underflow downlink latency");

        ipi::api::Private5gProbeResponseTracker tracker;
        ipi::tests::expect(tracker.validate(request, decodedAck, false, 1500).matched(),
                           "tracker should accept first matching response");
        ipi::tests::expect(
            tracker.validate(request, decodedAck, false, 1500).disposition ==
                ipi::api::Private5gProbeAckDisposition::DUPLICATE,
            "tracker should reject duplicate response id");

        auto wrongSession = decodedAck;
        wrongSession.responseId = "response-8";
        wrongSession.sessionId = "other-session";
        ipi::tests::expect(
            ipi::api::validate_private_5g_probe_ack(
                request, wrongSession, {}, false, 1500).disposition ==
                ipi::api::Private5gProbeAckDisposition::MISMATCHED_SESSION,
            "session mismatch should be classified");
        ipi::tests::expect(
            ipi::api::validate_private_5g_probe_ack(
                request, decodedAck, {}, false, 5000).disposition ==
                ipi::api::Private5gProbeAckDisposition::STALE,
            "wall expiration should be classified stale");
        ipi::tests::expect(
            ipi::api::validate_private_5g_probe_ack(
                request, decodedAck, {}, true, 1500).disposition ==
                ipi::api::Private5gProbeAckDisposition::LATE,
            "local monotonic deadline should be classified late");

        auto trailing = encodedAck;
        trailing.push_back(0);
        bool rejectedTrailing = false;
        try {
            (void)ipi::api::decode_private_5g_probe_ack(trailing);
        } catch (const std::exception&) {
            rejectedTrailing = true;
        }
        ipi::tests::expect(rejectedTrailing, "probe decoder must reject trailing bytes");
    });
}
