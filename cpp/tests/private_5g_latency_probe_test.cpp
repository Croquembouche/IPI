#include "test_support.hpp"

#include "ipi/api/private_5g_latency_probe.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

template <typename Callable>
bool rejects(Callable&& callable) {
    try {
        callable();
    } catch (const std::exception&) {
        return true;
    }
    return false;
}

void append_uint16(std::vector<std::uint8_t>& buffer, std::uint16_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xffU));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

void append_uint32(std::vector<std::uint8_t>& buffer, std::uint32_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 24) & 0xffU));
    buffer.push_back(static_cast<std::uint8_t>((value >> 16) & 0xffU));
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xffU));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

std::vector<std::uint8_t> make_unversioned_operation(bool fourByteLengths) {
    std::vector<std::uint8_t> encoded(16U, 0U);
    const std::vector<std::uint8_t> vehicleId{'v', 'e', 'h'};
    const std::vector<std::uint8_t> object{0x10U, 0x11U, 0x12U, 0x13U};
    const std::string taskId{"legacy"};
    encoded.push_back(static_cast<std::uint8_t>(vehicleId.size()));
    encoded.insert(encoded.end(), vehicleId.begin(), vehicleId.end());
    encoded.push_back(static_cast<std::uint8_t>(ipi::ServiceClass::GuidedPlanning));
    encoded.push_back(static_cast<std::uint8_t>(ipi::GuidanceStatus::Request));
    encoded.push_back(0xc0U); // application object and task identifier
    if (fourByteLengths) {
        append_uint32(encoded, static_cast<std::uint32_t>(object.size()));
    } else {
        append_uint16(encoded, static_cast<std::uint16_t>(object.size()));
    }
    encoded.insert(encoded.end(), object.begin(), object.end());
    if (fourByteLengths) {
        append_uint32(encoded, static_cast<std::uint32_t>(taskId.size()));
    } else {
        append_uint16(encoded, static_cast<std::uint16_t>(taskId.size()));
    }
    encoded.insert(encoded.end(), taskId.begin(), taskId.end());
    return encoded;
}

} // namespace

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

        // The released Uu operation profile uses a five-byte, self-identifying
        // header and four-byte content lengths. Verify both the 2-MiB object
        // boundary and the O+83-byte profile cost for a seven-byte task id.
        constexpr std::size_t largeObjectBytes = 2U * 1024U * 1024U;
        ipi::CooperativeServiceMessage largeMessage;
        largeMessage.vehicleId = {'v', 'e', 'h', '-', '0', '1'};
        largeMessage.serviceClass = ipi::ServiceClass::GuidedPlanning;
        largeMessage.guidanceStatus = ipi::GuidanceStatus::Request;
        largeMessage.requestedHorizonMs = 2500;
        largeMessage.confidence = 100;

        ipi::GuidedPlanningPayload largePlanning;
        ipi::Waypoint largeWaypoint;
        largeWaypoint.position.latitude = 42.3314;
        largeWaypoint.position.longitude = -83.0458;
        largeWaypoint.targetSpeedMps = 8.0;
        largePlanning.waypoints.push_back(largeWaypoint);
        largeMessage.planning = largePlanning;
        largeMessage.offloadPayload = std::vector<std::uint8_t>(largeObjectBytes, 0x5aU);
        largeMessage.offloadTaskId = "probe-1";

        const auto largeEncoding = largeMessage.to_canonical_encoding();
        ipi::tests::expect(largeEncoding.size() == largeObjectBytes + 83U,
                           "2-MiB operation object should use the O+83-byte versioned profile");
        ipi::tests::expect(
            largeEncoding.size() >= 5U && largeEncoding[0] == 'I' &&
                largeEncoding[1] == 'P' && largeEncoding[2] == 'I' &&
                largeEncoding[3] == 'O' &&
                largeEncoding[4] == ipi::kIpiOperationEncodingVersion,
            "new operation records should carry the IPIO marker and current version");
        const auto decodedLarge =
            ipi::CooperativeServiceMessage::from_canonical_encoding(largeEncoding);
        ipi::tests::expect(decodedLarge.offloadPayload == largeMessage.offloadPayload,
                           "2-MiB operation object should round-trip without chunking");
        ipi::tests::expect(decodedLarge.offloadTaskId == largeMessage.offloadTaskId,
                           "large-object task identity should round-trip");

        auto planningOnly = largeMessage;
        planningOnly.offloadPayload.reset();
        planningOnly.offloadTaskId.reset();
        ipi::tests::expect(planningOnly.to_canonical_encoding().size() == 68U,
                           "versioned planning request should encode to 68 bytes");

        const auto legacy32 = ipi::CooperativeServiceMessage::from_canonical_encoding(
            make_unversioned_operation(true));
        const auto legacy16 = ipi::CooperativeServiceMessage::from_canonical_encoding(
            make_unversioned_operation(false));
        ipi::tests::expect(
            legacy32.offloadPayload && legacy32.offloadPayload->size() == 4U &&
                legacy32.offloadTaskId == std::optional<std::string>{"legacy"},
            "decoder should retain unversioned four-byte-section artifacts");
        ipi::tests::expect(
            legacy16.offloadPayload && legacy16.offloadPayload->size() == 4U &&
                legacy16.offloadTaskId == std::optional<std::string>{"legacy"},
            "decoder should retain unversioned two-byte-section artifacts");

        auto unsupportedVersion = largeEncoding;
        unsupportedVersion[4] = 0xffU;
        ipi::tests::expect(
            rejects([&] {
                (void)ipi::CooperativeServiceMessage::from_canonical_encoding(
                    unsupportedVersion);
            }),
            "self-identified operation records with unknown versions must be rejected");

        Private5gProbeRequest largeRequest;
        largeRequest.sequence = 8;
        largeRequest.messageId = "large-message-8";
        largeRequest.requestId = "large-request-8";
        largeRequest.frame = ipi::api::make_private_5g_probe_frame(largeMessage);
        const auto encodedLargeRequest =
            ipi::api::encode_private_5g_probe_request(largeRequest);
        ipi::tests::expect(
            encodedLargeRequest.size() < ipi::api::kDefaultPrivate5gProbePacketLimit,
            "default Uu probe packet limit should admit the evaluated 2-MiB object");
        const auto decodedLargeRequest =
            ipi::api::decode_private_5g_probe_request(encodedLargeRequest);
        ipi::tests::expect(decodedLargeRequest.frame.payload == largeRequest.frame.payload,
                           "2-MiB operation frame should round-trip through the Uu probe envelope");
        const ipi::v2x::UperCodec inspectionCodec;
        const auto largeDetail = ipi::api::inspect_private_5g_probe_frame(
            decodedLargeRequest.frame, inspectionCodec);
        ipi::tests::expect(
            largeDetail.find("offloadPayload=2097152 bytes") != std::string::npos,
            "Uu inspection must decode the canonical large-object profile, not the PC5 profile");

        // Corrupt the four-byte object length so the decoder must reject a
        // section that extends beyond the received operation record.
        auto truncatedLarge = largeEncoding;
        truncatedLarge.pop_back();
        ipi::tests::expect(
            rejects([&] {
                (void)ipi::CooperativeServiceMessage::from_canonical_encoding(truncatedLarge);
            }),
            "truncated large operation objects must be rejected");

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
