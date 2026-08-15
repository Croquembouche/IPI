#pragma once

#include "ipi/api/types.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace ipi::api {

inline constexpr std::size_t kPc5IpiOperationalMaxBytes = 2048;
inline constexpr std::size_t kMocarDeployedApplicationHardMaxBytes = 4080;

struct Pc5IpiCodecLimits {
    /** Conservative limit validated by the existing Mocar field samples. */
    std::size_t maxApplicationBytes{kPc5IpiOperationalMaxBytes};
};

struct Pc5IpiEnvelope {
    Envelope<J2735Payload> message{};
};

struct Pc5IpiResponse {
    std::string responseId{};
    std::string requestMessageId{};
    std::string correlationId{};
    std::optional<std::string> sessionId{};
    std::uint64_t sequence{0};
    Timestamp expiresAt{};
    bool accepted{true};
    FailureCode failureCode{FailureCode::NONE};
    std::string detail{};
    J2735Payload result{};
};

enum class Pc5IpiResponseDisposition {
    MATCHED,
    MISMATCHED_MESSAGE,
    MISMATCHED_REQUEST,
    MISMATCHED_SESSION,
    MISMATCHED_SEQUENCE,
    MISMATCHED_RESULT,
    DUPLICATE,
    STALE,
    LATE
};

struct Pc5IpiResponseValidation {
    Pc5IpiResponseDisposition disposition{Pc5IpiResponseDisposition::MATCHED};
    std::string detail{};

    [[nodiscard]] bool matched() const noexcept {
        return disposition == Pc5IpiResponseDisposition::MATCHED;
    }
};

[[nodiscard]] std::vector<std::uint8_t> encode_pc5_ipi_envelope(
    const Pc5IpiEnvelope& envelope,
    Pc5IpiCodecLimits limits = {});

[[nodiscard]] Pc5IpiEnvelope decode_pc5_ipi_envelope(
    const std::vector<std::uint8_t>& packet,
    Pc5IpiCodecLimits limits = {});

[[nodiscard]] std::vector<std::uint8_t> encode_pc5_ipi_response(
    const Pc5IpiResponse& response,
    Pc5IpiCodecLimits limits = {});

[[nodiscard]] Pc5IpiResponse decode_pc5_ipi_response(
    const std::vector<std::uint8_t>& packet,
    Pc5IpiCodecLimits limits = {});

[[nodiscard]] std::size_t pc5_ipi_encoded_size(
    const Pc5IpiEnvelope& envelope,
    Pc5IpiCodecLimits limits = {});

[[nodiscard]] std::size_t pc5_ipi_encoded_size(
    const Pc5IpiResponse& response,
    Pc5IpiCodecLimits limits = {});

[[nodiscard]] Pc5IpiResponseValidation validate_pc5_ipi_response(
    const Pc5IpiEnvelope& request,
    const Pc5IpiResponse& response,
    const std::unordered_set<std::string>& completedResponseIds = {},
    bool arrivedAfterDeadline = false,
    std::optional<Timestamp> observedAt = {});

[[nodiscard]] std::string to_string(Pc5IpiResponseDisposition disposition);

} // namespace ipi::api
