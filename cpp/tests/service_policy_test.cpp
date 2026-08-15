#include "test_support.hpp"

#include "ipi/policy/service_policy.hpp"

int main() {
    return ipi::tests::run_test("tiered_service_policy", [] {
        using namespace std::chrono_literals;
        using namespace ipi::policy;

        ServiceIntent intent;
        intent.requestId = "request-1";
        intent.sessionId = "session-1";
        intent.serviceType = ipi::ServiceType::PlanningAid;
        intent.tier = ServiceTier::BEST_EFFORT;
        intent.deadline = 500ms;
        intent.maximumRtt = 100ms;
        intent.maximumLossFraction = 0.05;
        intent.minimumRateHz = 10.0;

        LinkSnapshot link;
        link.available = true;
        link.serviceAvailable = true;
        link.rtt = 20ms;
        link.packetLossFraction = 0.01;
        link.congestionFraction = 0.1;
        link.estimatedUplinkMbps = 20.0;
        link.estimatedDownlinkMbps = 50.0;

        TieredServicePolicy policy;
        auto decision = policy.evaluate(
            intent, link, ipi::activation::ActivationState::ACTIVE);
        ipi::tests::expect(decision.action == PolicyAction::START,
                           "healthy active-zone service should start");

        link.congestionFraction = 0.9;
        decision = policy.evaluate(intent, link, ipi::activation::ActivationState::ACTIVE);
        ipi::tests::expect(decision.action == PolicyAction::PAUSE,
                           "congested best-effort work should pause");

        intent.tier = ServiceTier::SAFETY_CRITICAL;
        intent.localFallbackAvailable = true;
        link.available = false;
        decision = policy.evaluate(intent, link, ipi::activation::ActivationState::ACTIVE);
        ipi::tests::expect(decision.action == PolicyAction::USE_LOCAL_FALLBACK,
                           "critical link loss must immediately select local fallback");
        ipi::tests::expect(decision.action != PolicyAction::PAUSE,
                           "critical service must never be intentionally paused");

        link.available = true;
        link.congestionFraction = 0.95;
        intent.currentlyActive = true;
        decision = policy.evaluate(intent, link, ipi::activation::ActivationState::ACTIVE);
        ipi::tests::expect(decision.action == PolicyAction::MAINTAIN,
                           "congestion alone must not pause an active critical service");

        decision = policy.evaluate(intent, link, ipi::activation::ActivationState::OUTSIDE);
        ipi::tests::expect(decision.action == PolicyAction::USE_LOCAL_FALLBACK,
                           "critical service outside infrastructure zone remains local");

        intent.tier = ServiceTier::REAL_TIME;
        link.congestionFraction = 0.1;
        decision = policy.evaluate(intent, link, ipi::activation::ActivationState::OUTSIDE);
        ipi::tests::expect(decision.action == PolicyAction::USE_LOCAL_FALLBACK,
                           "inactive zones should preserve a real-time local fallback");

        link.rtt = -1ms;
        decision = policy.evaluate(intent, link, ipi::activation::ActivationState::ACTIVE);
        ipi::tests::expect(decision.action == PolicyAction::REJECT &&
                               decision.reason == PolicyReasonCode::INVALID_INTENT,
                           "negative link timing should be rejected as invalid input");
    });
}
