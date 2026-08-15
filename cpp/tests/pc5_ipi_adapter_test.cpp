#include "test_support.hpp"

#include "ipi/api/pc5_ipi_adapter.hpp"
#include "ipi/v2x/j2735_ipi_regional_codec.hpp"

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
    return ipi::tests::run_test("pc5_ipi_adapter_contract", [] {
        using namespace std::chrono_literals;
        using namespace ipi::api;

        Pc5IpiEnvelope request;
        request.message.metadata.messageId = "message-1";
        request.message.metadata.sentAt = Timestamp{100s};
        request.message.metadata.intersectionId = "intersection-1";
        request.message.metadata.transport = TransportType::C_V2X;
        request.message.metadata.source.type = SourceType::PCAV;
        request.message.metadata.source.id = "vehicle-1";
        request.message.metadata.sessionId = "session-1";
        request.message.metadata.correlationId = "request-1";
        request.message.metadata.sequence = 7;
        request.message.metadata.expiresAt = Timestamp{105s};
        request.message.data.type = J2735MessageType::IPI_COOPERATIVE_SERVICE;
        request.message.data.encoding = J2735Encoding::BYTES;
        request.message.data.frameCounter = 4;
        request.message.data.payload = {1, 2, 3, 4};

        const auto encoded = encode_pc5_ipi_envelope(request);
        const auto decoded = decode_pc5_ipi_envelope(encoded);
        ipi::tests::expect(decoded.message.metadata.messageId == "message-1",
                           "PC5 request message identity must round-trip");
        ipi::tests::expect(decoded.message.metadata.sessionId ==
                               request.message.metadata.sessionId,
                           "PC5 request session must round-trip");
        ipi::tests::expect(decoded.message.data.payload == request.message.data.payload,
                           "PC5 request payload must round-trip");
        ipi::tests::expect(pc5_ipi_encoded_size(request) == encoded.size(),
                           "PC5 encoded-size accounting must be exact");

        ipi::CooperativeServiceMessage regionalRequest;
        regionalRequest.sessionId[0] = 0x07;
        regionalRequest.vehicleId = {'v', 'e', 'h', 'i', 'c', 'l', 'e', '-', '1'};
        regionalRequest.serviceClass = ipi::ServiceClass::GuidedPlanning;
        regionalRequest.guidanceStatus = ipi::GuidanceStatus::Request;
        regionalRequest.offloadPayload = std::vector<std::uint8_t>{1, 2, 3, 4};
        regionalRequest.offloadTaskId = "request-1";
        const ipi::v2x::J2735IpiRegionalCodec regionalCodec;

        auto formalRequest = request;
        formalRequest.message.data.encoding = J2735Encoding::UPER;
        formalRequest.message.data.payload =
            regionalCodec.encode_message_frame(regionalRequest);
        const auto decodedFormalRequest = decode_pc5_ipi_envelope(
            encode_pc5_ipi_envelope(formalRequest));
        const auto decodedRegionalRequest = regionalCodec.decode_message_frame(
            decodedFormalRequest.message.data.payload);
        ipi::tests::expect(
            decodedRegionalRequest.guidanceStatus == ipi::GuidanceStatus::Request &&
                decodedRegionalRequest.sessionId == regionalRequest.sessionId &&
                decodedRegionalRequest.offloadTaskId == regionalRequest.offloadTaskId,
            "IP5X request must preserve the formal IPI TestMessage00 frame");

        Pc5IpiResponse response;
        response.responseId = "result-1";
        response.requestMessageId = "message-1";
        response.correlationId = "request-1";
        response.sessionId = "session-1";
        response.sequence = 7;
        response.expiresAt = Timestamp{104s};
        response.accepted = true;
        response.result = request.message.data;
        response.result.payload = {9, 8, 7};
        const auto encodedResponse = encode_pc5_ipi_response(response);
        const auto decodedResponse = decode_pc5_ipi_response(encodedResponse);
        ipi::tests::expect(decodedResponse.responseId == response.responseId,
                           "PC5 response identity must round-trip");
        ipi::tests::expect(validate_pc5_ipi_response(
                               request, decodedResponse, {}, false, Timestamp{102s}).matched(),
                           "matching PC5 response must validate");

        auto regionalResult = regionalRequest;
        regionalResult.guidanceStatus = ipi::GuidanceStatus::Complete;
        auto formalResponse = response;
        formalResponse.result = formalRequest.message.data;
        formalResponse.result.payload = regionalCodec.encode_message_frame(regionalResult);
        const auto decodedFormalResponse = decode_pc5_ipi_response(
            encode_pc5_ipi_response(formalResponse));
        ipi::tests::expect(
            validate_pc5_ipi_response(formalRequest, decodedFormalResponse, {}, false,
                                      Timestamp{102s}).matched(),
            "IP5X metadata must accept the correlated formal terminal response");
        const auto decodedRegionalResult = regionalCodec.decode_message_frame(
            decodedFormalResponse.result.payload);
        ipi::tests::expect(
            decodedRegionalResult.guidanceStatus == ipi::GuidanceStatus::Complete &&
                decodedRegionalResult.sessionId == regionalRequest.sessionId &&
                decodedRegionalResult.offloadTaskId == regionalRequest.offloadTaskId,
            "IP5X response must preserve the formal terminal IPI frame");

        ipi::tests::expect(
            validate_pc5_ipi_response(request, decodedResponse, {"result-1"},
                                      false, Timestamp{102s}).disposition ==
                Pc5IpiResponseDisposition::DUPLICATE,
            "duplicate PC5 response must be rejected");
        auto mismatch = decodedResponse;
        mismatch.sessionId = "session-2";
        ipi::tests::expect(
            validate_pc5_ipi_response(request, mismatch, {}, false,
                                      Timestamp{102s}).disposition ==
                Pc5IpiResponseDisposition::MISMATCHED_SESSION,
            "PC5 session mismatch must be classified");
        mismatch = decodedResponse;
        mismatch.result.encoding = J2735Encoding::JSON;
        ipi::tests::expect(
            validate_pc5_ipi_response(request, mismatch, {}, false,
                                      Timestamp{102s}).disposition ==
                Pc5IpiResponseDisposition::MISMATCHED_RESULT,
            "PC5 result encoding mismatch must be classified");
        mismatch = decodedResponse;
        mismatch.result.frameCounter = 5;
        ipi::tests::expect(
            validate_pc5_ipi_response(request, mismatch, {}, false,
                                      Timestamp{102s}).disposition ==
                Pc5IpiResponseDisposition::MISMATCHED_RESULT,
            "PC5 result frame counter mismatch must be classified");
        ipi::tests::expect(
            validate_pc5_ipi_response(request, decodedResponse, {}, false,
                                      Timestamp{105s}).disposition ==
                Pc5IpiResponseDisposition::STALE,
            "expired PC5 exchange must be classified stale");
        ipi::tests::expect(
            validate_pc5_ipi_response(request, decodedResponse, {}, true,
                                      Timestamp{102s}).disposition ==
                Pc5IpiResponseDisposition::LATE,
            "local monotonic deadline violation must be classified late");

        auto corrupted = encoded;
        corrupted[corrupted.size() / 2] ^= 0x01U;
        expect_throw([&] { (void)decode_pc5_ipi_envelope(corrupted); },
                     "PC5 decoder must reject CRC corruption");

        const auto exactLimit = Pc5IpiCodecLimits{encoded.size()};
        ipi::tests::expect(encode_pc5_ipi_envelope(request, exactLimit).size() == encoded.size(),
                           "packet exactly at configured cap must be accepted");
        expect_throw([&] {
            (void)encode_pc5_ipi_envelope(
                request, Pc5IpiCodecLimits{encoded.size() - 1});
        }, "packet one byte over configured cap must be rejected");

        auto nearLimit = request;
        nearLimit.message.data.payload = {0};
        const auto overhead = encode_pc5_ipi_envelope(
            nearLimit, Pc5IpiCodecLimits{kPc5IpiOperationalMaxBytes}).size() - 1;
        nearLimit.message.data.payload.assign(
            kPc5IpiOperationalMaxBytes - overhead, 0xA5U);
        ipi::tests::expect(encode_pc5_ipi_envelope(nearLimit).size() ==
                               kPc5IpiOperationalMaxBytes,
                           "operational-limit packet must encode to exactly 2048 bytes");
        nearLimit.message.data.payload.push_back(0xA5U);
        expect_throw([&] { (void)encode_pc5_ipi_envelope(nearLimit); },
                     "packet above conservative operational limit must be rejected");

        expect_throw([&] {
            (void)encode_pc5_ipi_envelope(
                request, Pc5IpiCodecLimits{kMocarDeployedApplicationHardMaxBytes + 1});
        }, "adapter must reject caps above the deployment-specific hard maximum");
        expect_throw([&] {
            auto invalid = request;
            invalid.message.metadata.transport = static_cast<TransportType>(255);
            (void)encode_pc5_ipi_envelope(invalid);
        }, "PC5 encoder must reject unknown metadata enums");
        expect_throw([&] {
            auto invalid = response;
            invalid.result.type = static_cast<J2735MessageType>(256);
            (void)encode_pc5_ipi_response(invalid);
        }, "PC5 encoder must reject unknown result enums");
    });
}
