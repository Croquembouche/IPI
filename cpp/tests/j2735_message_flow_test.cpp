#include "test_support.hpp"

#include "ipi/api/edge4av_interface.hpp"
#include "ipi/api/private_5g_latency_probe.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace {

bool near(double lhs, double rhs, double tolerance) {
    return std::fabs(lhs - rhs) <= tolerance;
}

ipi::j2735::PersonalSafetyMessage make_psm() {
    ipi::j2735::PersonalSafetyMessage psm;
    psm.basicType = ipi::j2735::PersonalDeviceUserType::Pedestrian;
    psm.secondMarkMs = 42123;
    psm.messageCount = 63;
    psm.temporaryId = 0xA1B2C3D4;
    psm.latitude = 42.3314271;
    psm.longitude = -83.0457538;
    psm.elevationM = 182.4F;
    psm.horizontalAccuracyM = 1.25F;
    psm.speedMps = 1.42F;
    psm.headingDeg = 271.125F;
    psm.acceleration = ipi::j2735::PersonalAccelerationSet{-0.25F, 0.13F, -0.04F, -2.5F};
    psm.pathHistory = {
        {-12, 7, -1, 100},
        {-24, 15, -1, 200},
    };
    psm.pathPrediction = ipi::j2735::PersonalPathPrediction{-35, 92};
    psm.propulsion = ipi::j2735::PersonalPropelledInformation{
        ipi::j2735::PersonalPropulsionKind::Human, 2};
    return psm;
}

ipi::CooperativeServiceMessage make_cooperative_message() {
    ipi::CooperativeServiceMessage message;
    message.sessionId[0] = 0x42;
    message.vehicleId = {'c', 'a', 'v', '-', '1'};
    message.serviceClass = ipi::ServiceClass::GuidedPerception;
    message.guidanceStatus = ipi::GuidanceStatus::Update;
    message.confidence = 96;

    ipi::DetectedObject detected;
    detected.objectId = {0x10, 0x20, 0x30, 0x40};
    detected.classification = ipi::DetectedObject::Classification::Pedestrian;
    detected.position.latitude = 42.3314271;
    detected.position.longitude = -83.0457538;
    detected.velocityMps = 1.42;

    ipi::GuidedPerceptionPayload perception;
    perception.detectedObjects.push_back(detected);
    message.perception = perception;
    return message;
}

} // namespace

int main() {
    return ipi::tests::run_test("j2735_psm_and_cooperative_message_flow", [] {
        ipi::v2x::UperCodec codec;
        const auto psm = make_psm();

        ipi::j2735::BasicSafetyMessage signedBsm;
        signedBsm.vehicleId = 7;
        signedBsm.latitude = 42.0;
        signedBsm.longitude = -83.0;
        signedBsm.speedMps = 3.0F;
        signedBsm.headingDeg = 45.0F;
        signedBsm.accelerationMps2 = -0.25F;
        const auto decodedSignedBsm = codec.decode_bsm(codec.encode(signedBsm));
        ipi::tests::expect(decodedSignedBsm.accelerationMps2 &&
                               near(*decodedSignedBsm.accelerationMps2, -0.25, 0.01),
                           "BSM profile codec should sign-extend acceleration");

        const auto byteEncoded = psm.to_bytes();
        const auto byteDecoded = ipi::j2735::PersonalSafetyMessage::from_bytes(byteEncoded);
        ipi::tests::expect(byteDecoded.temporaryId == psm.temporaryId,
                           "PSM byte codec should preserve temporary id");
        ipi::tests::expect(byteDecoded.pathHistory.size() == psm.pathHistory.size(),
                           "PSM byte codec should preserve path history");
        ipi::tests::expect(byteDecoded.acceleration &&
                               byteDecoded.acceleration->longitudinalMps2 ==
                                   psm.acceleration->longitudinalMps2,
                           "PSM byte codec should preserve signed acceleration");

        const auto uperEncoded = codec.encode(psm);
        const auto uperDecoded = codec.decode_psm(uperEncoded);
        ipi::tests::expect(uperDecoded.basicType == psm.basicType,
                           "PSM profile codec should preserve basic type");
        ipi::tests::expect(near(uperDecoded.latitude, psm.latitude, 0.0000001),
                           "PSM profile codec should preserve latitude resolution");
        ipi::tests::expect(near(uperDecoded.longitude, psm.longitude, 0.0000001),
                           "PSM profile codec should preserve longitude resolution");
        ipi::tests::expect(near(uperDecoded.speedMps, psm.speedMps, 0.02),
                           "PSM profile codec should preserve speed resolution");
        ipi::tests::expect(uperDecoded.acceleration &&
                               near(uperDecoded.acceleration->longitudinalMps2, -0.25, 0.01),
                           "PSM profile codec should sign-extend acceleration");
        ipi::tests::expect(uperDecoded.pathPrediction &&
                               uperDecoded.pathPrediction->radiusOfCurveM == -35,
                           "PSM profile codec should preserve signed path curvature");
        ipi::tests::expect(uperDecoded.pathHistory.size() == 2 &&
                               uperDecoded.pathHistory.front().latitudeOffset == -12,
                           "PSM profile codec should preserve path offsets");

        const auto packedPsm = ipi::api::pack_j2735_payload(psm, codec);
        ipi::tests::expect(packedPsm.type == ipi::api::J2735MessageType::PSM,
                           "typed PSM payload should use the PSM API type");
        const auto unpackedPsm =
            ipi::api::unpack_j2735_payload<ipi::j2735::PersonalSafetyMessage>(packedPsm, codec);
        ipi::tests::expect(unpackedPsm.temporaryId == psm.temporaryId,
                           "typed PSM payload should round-trip");

        const auto psmFrame = ipi::api::make_message_frame(psm, codec);
        ipi::tests::expect(psmFrame.type == ipi::MessageType::PSM,
                           "PSM should be represented by a PSM MessageFrame");
        ipi::tests::expect(ipi::to_string(psmFrame.type) == "PSM",
                           "PSM MessageFrame should have a stable name");
        const auto probeFrame = ipi::api::make_private_5g_probe_frame(psm, codec);
        ipi::tests::expect(ipi::api::inspect_private_5g_probe_frame(probeFrame, codec).find("PSM{") == 0,
                           "private 5G receiver should decode a PSM frame");

        const auto cooperative = make_cooperative_message();
        const auto cooperativePayload = ipi::api::pack_j2735_payload(cooperative, codec);
        ipi::tests::expect(
            cooperativePayload.type == ipi::api::J2735MessageType::IPI_COOPERATIVE_SERVICE,
            "cooperative payload should use the regional cooperative API type");
        const auto decodedCooperative =
            ipi::api::unpack_j2735_payload<ipi::CooperativeServiceMessage>(cooperativePayload, codec);
        ipi::tests::expect(decodedCooperative.perception &&
                               decodedCooperative.perception->detectedObjects.size() == 1,
                           "cooperative perception data should round-trip");
        ipi::tests::expect(decodedCooperative.perception->detectedObjects.front().classification ==
                               ipi::DetectedObject::Classification::Pedestrian,
                           "cooperative object classification should round-trip");

        ipi::api::Edge4AvInterface edge4av;
        ipi::api::EnvelopeMetadata metadata;
        metadata.intersectionId = "int-psm";
        metadata.transport = ipi::api::TransportType::CELLULAR_5G;
        metadata.source.type = ipi::api::SourceType::PEDESTRIAN_DEVICE;
        metadata.source.id = "phone-ped-1";

        ipi::tests::expect(edge4av.ingest_v2x_message(metadata, psm).accepted,
                           "high-level receiver should ingest PSM");
        const auto receivedPsms =
            edge4av.list_v2x_messages<ipi::j2735::PersonalSafetyMessage>();
        ipi::tests::expect(!receivedPsms.empty() &&
                               receivedPsms.back().temporaryId == psm.temporaryId,
                           "high-level sender should return decoded PSM");

        metadata.source.type = ipi::api::SourceType::PCAV;
        metadata.source.id = "cav-1";
        ipi::tests::expect(edge4av.ingest_v2x_message(metadata, cooperative).accepted,
                           "high-level receiver should ingest cooperative CAV messages");
        const auto receivedCooperative =
            edge4av.list_v2x_messages<ipi::CooperativeServiceMessage>();
        ipi::tests::expect(!receivedCooperative.empty() &&
                               receivedCooperative.back().confidence == cooperative.confidence,
                           "high-level sender should return decoded cooperative CAV messages");

        ipi::api::BroadcastTarget target;
        target.channel = "c-v2x";
        ipi::tests::expect(edge4av.request_broadcast(metadata, target, psm).accepted,
                           "typed PSM should use the broadcast path");
        ipi::tests::expect(edge4av.request_broadcast(metadata, target, cooperative).accepted,
                           "cooperative CAV messages should use the broadcast path");

        ipi::api::J2735Payload rawTim;
        rawTim.type = ipi::api::J2735MessageType::TIM;
        rawTim.encoding = ipi::api::J2735Encoding::UPER;
        rawTim.payload = {0x1f, 0x01, 0x02, 0x03};
        ipi::tests::expect(edge4av.ingest_v2x_payload(metadata, rawTim).accepted,
                           "opaque standard J2735 payloads should pass through");
        const auto receivedTim =
            edge4av.list_v2x_payloads(ipi::api::J2735MessageType::TIM);
        ipi::tests::expect(!receivedTim.empty() && receivedTim.back().payload == rawTim.payload,
                           "opaque J2735 payload bytes should be preserved");
        ipi::tests::expect(edge4av.request_broadcast_payload(metadata, target, rawTim).accepted,
                           "opaque standard J2735 payloads should use the broadcast path");

        bool truncatedRejected = false;
        try {
            (void)codec.decode_psm(std::vector<std::uint8_t>{0x00, 0x01});
        } catch (const std::runtime_error&) {
            truncatedRejected = true;
        }
        ipi::tests::expect(truncatedRejected, "truncated PSM should be rejected");
    });
}
