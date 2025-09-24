#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ipi {

/**
 * \brief Enumerates supported service request types carried in the IPI-ServiceRequest
 * regional extension within a J2735 Basic Safety Message (BSM).
 */
enum class ServiceType : std::uint8_t {
    LaneKeepingAid = 0,
    UnprotectedLeftAvailability = 1,
    PerceptionAid = 2,
    PlanningAid = 3,
    ControlAid = 4,
    ComputationAid = 5,
    HdMapUpdate = 6
};

/**
 * \brief Represents the payload of the IPI-ServiceRequest regional extension.
 *
 * This structure is transport-agnostic. Real deployments should marshal the
 * values into the J2735 ASN.1 representation. The helper functions provided
 * here implement a simple byte-oriented canonical encoding so integrations can
 * be validated before wiring in a full ASN.1 toolchain.
 */
struct IpiServiceRequest {
    ServiceType serviceType{ServiceType::LaneKeepingAid};
    std::uint16_t requestId{0};
    std::optional<std::uint16_t> desiredHorizonMs{}; ///< Optional planning horizon in milliseconds.
    std::vector<std::uint8_t> additionalData{};      ///< Service-specific opaque payload.

    /**
     * Validates field ranges according to the specification.
     * @throws std::invalid_argument when constraints are violated.
     */
    void validate() const;

    /**
     * Serialises this structure into a canonical byte buffer. The format is:
     * [serviceType:1][requestId:2][flags:1][desiredHorizonMs?:2][dataLen:2][data:N]
     * where flags bit0 indicates presence of desiredHorizonMs.
     */
    [[nodiscard]] std::vector<std::uint8_t> to_canonical_encoding() const;

    /**
     * Deserialises a canonical byte buffer produced by to_canonical_encoding().
     */
    static IpiServiceRequest from_canonical_encoding(const std::vector<std::uint8_t>& buffer);

    /**
     * Returns a human-readable summary (e.g. for logging).
     */
    [[nodiscard]] std::string to_string() const;
};

/**
 * Helper converting a ServiceType to a descriptive string.
 */
[[nodiscard]] std::string to_string(ServiceType type);

} // namespace ipi
