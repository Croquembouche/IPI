#include "test_support.hpp"

#include "ipi/offload/offload_decision.hpp"

int main() {
    return ipi::tests::run_test("offload_decision_model", [] {
        using namespace std::chrono_literals;
        using namespace ipi::offload;

        OffloadInputs inputs;
        inputs.requestId = "request-1";
        inputs.deadline = 300ms;
        inputs.localQueueTime = 50ms;
        inputs.localComputeTime = 250ms;
        inputs.uploadBytes = 100000;
        inputs.downloadBytes = 10000;
        inputs.link.available = true;
        inputs.link.rtt = 20ms;
        inputs.link.estimatedUplinkMbps = 100.0;
        inputs.link.estimatedDownlinkMbps = 100.0;
        inputs.edgeQueueTime = 10ms;
        inputs.edgeComputeTime = 30ms;
        inputs.uncertaintyMargin = 10ms;
        inputs.minimumEdgeBenefit = 5ms;
        inputs.minimumResultConfidence = 0.8;
        inputs.expectedResultConfidence = 0.95;

        OffloadDecisionEngine engine;
        auto decision = engine.evaluate(inputs);
        ipi::tests::expect(decision.executionSite == ExecutionSite::EDGE,
                           "faster deadline-safe edge path should be selected");
        ipi::tests::expect(decision.deadlineExpectedToBeMet,
                           "selected edge path should meet deadline");

        inputs.link.available = false;
        decision = engine.evaluate(inputs);
        ipi::tests::expect(decision.executionSite == ExecutionSite::LOCAL &&
                               decision.fallbackRequired,
                           "link loss should select retained local fallback");

        inputs.link.available = true;
        inputs.expectedResultConfidence = 0.5;
        decision = engine.evaluate(inputs);
        ipi::tests::expect(decision.reason == OffloadReasonCode::INSUFFICIENT_CONFIDENCE,
                           "low expected confidence should prevent edge selection");

        inputs.expectedResultConfidence = 0.95;
        inputs.deadline = 20ms;
        decision = engine.evaluate(inputs);
        ipi::tests::expect(!decision.deadlineExpectedToBeMet &&
                               decision.reason == OffloadReasonCode::DEADLINE_MISS_PREDICTED,
                           "model should expose when neither execution path meets the deadline");

        inputs.deadline = 300ms;
        inputs.link.rtt = -1ms;
        decision = engine.evaluate(inputs);
        ipi::tests::expect(decision.reason == OffloadReasonCode::INVALID_INPUT,
                           "negative link timing must be rejected as invalid input");
    });
}
