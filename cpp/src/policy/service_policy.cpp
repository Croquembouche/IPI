#include "ipi/policy/service_policy.hpp"

#include <algorithm>

namespace ipi::policy {
namespace {

bool valid_service_type(ipi::ServiceType serviceType) {
    return static_cast<std::uint8_t>(serviceType) <=
           static_cast<std::uint8_t>(ipi::ServiceType::HdMapUpdate);
}

bool valid_tier(ServiceTier tier) {
    return static_cast<std::uint8_t>(tier) <=
           static_cast<std::uint8_t>(ServiceTier::BEST_EFFORT);
}

bool valid_transport(api::TransportType transport) {
    return static_cast<std::uint8_t>(transport) <=
           static_cast<std::uint8_t>(api::TransportType::HTTP_BACKHAUL);
}

PolicyDecision fallback_or_reject(const ServiceIntent& intent,
                                  PolicyReasonCode reason) {
    PolicyDecision decision;
    decision.requestId = intent.requestId;
    decision.maximumRateHz = intent.minimumRateHz;
    decision.validity = std::chrono::milliseconds(100);
    decision.reason = reason;
    if (intent.localFallbackAvailable) {
        decision.action = PolicyAction::USE_LOCAL_FALLBACK;
        decision.reason = PolicyReasonCode::LOCAL_FALLBACK_REQUIRED;
    } else {
        decision.action = PolicyAction::REJECT;
    }
    return decision;
}

PolicyDecision degrade(const ServiceIntent& intent,
                       const LinkSnapshot& link,
                       PolicyReasonCode reason) {
    if (intent.tier == ServiceTier::SAFETY_CRITICAL ||
        intent.tier == ServiceTier::REAL_TIME) {
        return fallback_or_reject(intent, reason);
    }
    PolicyDecision decision;
    decision.requestId = intent.requestId;
    decision.selectedTransport = link.transport;
    decision.maximumRateHz = intent.tier == ServiceTier::INTERACTIVE
                                 ? std::max(0.1, intent.minimumRateHz * 0.5)
                                 : 0.0;
    decision.validity = std::chrono::milliseconds(250);
    decision.reason = reason;
    decision.action = intent.tier == ServiceTier::INTERACTIVE
                          ? PolicyAction::THROTTLE
                          : PolicyAction::PAUSE;
    return decision;
}

} // namespace

PolicyDecision TieredServicePolicy::evaluate(
    const ServiceIntent& intent,
    const LinkSnapshot& link,
    activation::ActivationState activationState) const {
    if (intent.requestId.empty() || intent.sessionId.empty() ||
        !valid_service_type(intent.serviceType) || !valid_tier(intent.tier) ||
        !valid_transport(link.transport) ||
        intent.deadline <= std::chrono::milliseconds::zero() ||
        intent.maximumRtt < std::chrono::milliseconds::zero() ||
        intent.maximumLossFraction < 0.0 || intent.maximumLossFraction > 1.0 ||
        intent.minimumRateHz < 0.0 || link.packetLossFraction < 0.0 ||
        link.packetLossFraction > 1.0 || link.congestionFraction < 0.0 ||
        link.congestionFraction > 1.0 ||
        link.rtt < std::chrono::milliseconds::zero() ||
        link.estimatedUplinkMbps < 0.0 || link.estimatedDownlinkMbps < 0.0) {
        auto decision = fallback_or_reject(intent, PolicyReasonCode::INVALID_INTENT);
        decision.action = PolicyAction::REJECT;
        decision.reason = PolicyReasonCode::INVALID_INTENT;
        return decision;
    }

    if (activationState != activation::ActivationState::ACTIVE) {
        if (intent.tier == ServiceTier::SAFETY_CRITICAL ||
            intent.tier == ServiceTier::REAL_TIME) {
            return fallback_or_reject(intent, PolicyReasonCode::ZONE_INACTIVE);
        }
        return degrade(intent, link, PolicyReasonCode::ZONE_INACTIVE);
    }
    if (!link.serviceAvailable) {
        return fallback_or_reject(intent, PolicyReasonCode::SERVICE_UNAVAILABLE);
    }
    if (!link.available) {
        return fallback_or_reject(intent, PolicyReasonCode::LINK_UNAVAILABLE);
    }
    if (link.rtt > intent.maximumRtt || link.rtt > intent.deadline) {
        return degrade(intent, link, PolicyReasonCode::LATENCY_BUDGET_EXCEEDED);
    }
    if (link.packetLossFraction > intent.maximumLossFraction) {
        return degrade(intent, link, PolicyReasonCode::LOSS_BUDGET_EXCEEDED);
    }
    if (link.congestionFraction >= 0.85) {
        if (intent.tier == ServiceTier::SAFETY_CRITICAL) {
            PolicyDecision decision;
            decision.requestId = intent.requestId;
            decision.action = intent.currentlyActive ? PolicyAction::MAINTAIN : PolicyAction::START;
            decision.selectedTransport = link.transport;
            decision.maximumRateHz = intent.minimumRateHz;
            decision.validity = std::chrono::milliseconds(100);
            decision.reason = PolicyReasonCode::CONGESTED;
            return decision;
        }
        return degrade(intent, link, PolicyReasonCode::CONGESTED);
    }

    PolicyDecision decision;
    decision.requestId = intent.requestId;
    decision.action = intent.currentlyActive ? PolicyAction::MAINTAIN : PolicyAction::START;
    decision.selectedTransport = link.transport;
    decision.maximumRateHz = intent.minimumRateHz;
    decision.validity = std::chrono::milliseconds(500);
    decision.reason = PolicyReasonCode::NONE;
    return decision;
}

std::string to_string(ServiceTier tier) {
    switch (tier) {
        case ServiceTier::SAFETY_CRITICAL: return "safety-critical";
        case ServiceTier::REAL_TIME: return "real-time";
        case ServiceTier::INTERACTIVE: return "interactive";
        case ServiceTier::BEST_EFFORT: return "best-effort";
        default: return "unknown";
    }
}

std::string to_string(PolicyAction action) {
    switch (action) {
        case PolicyAction::MAINTAIN: return "maintain";
        case PolicyAction::START: return "start";
        case PolicyAction::THROTTLE: return "throttle";
        case PolicyAction::PAUSE: return "pause";
        case PolicyAction::USE_LOCAL_FALLBACK: return "use-local-fallback";
        case PolicyAction::REJECT: return "reject";
        default: return "unknown";
    }
}

std::string to_string(PolicyReasonCode reason) {
    switch (reason) {
        case PolicyReasonCode::NONE: return "none";
        case PolicyReasonCode::ZONE_INACTIVE: return "zone-inactive";
        case PolicyReasonCode::SERVICE_UNAVAILABLE: return "service-unavailable";
        case PolicyReasonCode::LINK_UNAVAILABLE: return "link-unavailable";
        case PolicyReasonCode::LATENCY_BUDGET_EXCEEDED: return "latency-budget-exceeded";
        case PolicyReasonCode::LOSS_BUDGET_EXCEEDED: return "loss-budget-exceeded";
        case PolicyReasonCode::CONGESTED: return "congested";
        case PolicyReasonCode::LOCAL_FALLBACK_REQUIRED: return "local-fallback-required";
        case PolicyReasonCode::INVALID_INTENT: return "invalid-intent";
        default: return "unknown";
    }
}

} // namespace ipi::policy
