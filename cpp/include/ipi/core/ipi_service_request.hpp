#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ipi {

/**
 * \brief Enumerates the historical compact IPI-ServiceRequest types.
 *
 * New correlated operations use ServiceClass in CooperativeServiceMessage.
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
 * \brief Legacy compact IPI-ServiceRequest retained for source compatibility.
 *
 * Its historical byte encoding uses a 16-bit additional-data length. New CAV
 * operations, especially Uu objects above 65,535 bytes, should use
 * CooperativeServiceMessage and IpiInterface::submit_cooperative_service().
 * PC5 deployments should use the formal J2735 regional profile and obey the
 * selected PC5 binding's smaller operational limit.
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
