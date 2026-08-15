#pragma once

#include "ipi/activation/activation.hpp"
#include "ipi/api/types.hpp"
#include "ipi/core/ipi_service_request.hpp"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

namespace ipi::policy {

enum class ServiceTier : std::uint8_t {
    SAFETY_CRITICAL = 0,
    REAL_TIME = 1,
    INTERACTIVE = 2,
    BEST_EFFORT = 3
};

enum class PolicyAction : std::uint8_t {
    MAINTAIN = 0,
    START = 1,
    THROTTLE = 2,
    PAUSE = 3,
    USE_LOCAL_FALLBACK = 4,
    REJECT = 5
};

enum class PolicyReasonCode : std::uint8_t {
    NONE = 0,
    ZONE_INACTIVE = 1,
    SERVICE_UNAVAILABLE = 2,
    LINK_UNAVAILABLE = 3,
    LATENCY_BUDGET_EXCEEDED = 4,
    LOSS_BUDGET_EXCEEDED = 5,
    CONGESTED = 6,
    LOCAL_FALLBACK_REQUIRED = 7,
    INVALID_INTENT = 8
};

struct ServiceIntent {
    std::string requestId{};
    std::string sessionId{};
    ipi::ServiceType serviceType{ipi::ServiceType::PlanningAid};
    ServiceTier tier{ServiceTier::BEST_EFFORT};
    std::chrono::milliseconds deadline{1000};
    std::chrono::milliseconds maximumRtt{500};
    double maximumLossFraction{0.05};
    double minimumRateHz{1.0};
    bool localFallbackAvailable{false};
    bool currentlyActive{false};
};

struct LinkSnapshot {
    api::TransportType transport{api::TransportType::CELLULAR_5G};
    bool available{true};
    bool serviceAvailable{true};
    std::chrono::milliseconds rtt{0};
    double packetLossFraction{0.0};
    double congestionFraction{0.0};
    double estimatedUplinkMbps{0.0};
    double estimatedDownlinkMbps{0.0};
};

struct PolicyDecision {
    std::string requestId{};
    PolicyAction action{PolicyAction::REJECT};
    std::optional<api::TransportType> selectedTransport{};
    double maximumRateHz{0.0};
    std::chrono::milliseconds validity{0};
    PolicyReasonCode reason{PolicyReasonCode::NONE};
};

class TieredServicePolicy {
public:
    [[nodiscard]] PolicyDecision evaluate(
        const ServiceIntent& intent,
        const LinkSnapshot& link,
        activation::ActivationState activationState) const;
};

[[nodiscard]] std::string to_string(ServiceTier tier);
[[nodiscard]] std::string to_string(PolicyAction action);
[[nodiscard]] std::string to_string(PolicyReasonCode reason);

} // namespace ipi::policy
