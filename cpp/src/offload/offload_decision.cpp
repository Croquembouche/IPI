#include "ipi/offload/offload_decision.hpp"

#include <cmath>
#include <limits>

namespace ipi::offload {
namespace {

std::chrono::milliseconds transfer_time(std::size_t bytes, double megabitsPerSecond) {
    if (bytes == 0) return std::chrono::milliseconds::zero();
    if (megabitsPerSecond <= 0.0) return std::chrono::milliseconds::max();
    const auto milliseconds = std::ceil(
        static_cast<double>(bytes) * 8.0 / (megabitsPerSecond * 1000.0));
    if (milliseconds >= static_cast<double>(std::numeric_limits<std::int64_t>::max())) {
        return std::chrono::milliseconds::max();
    }
    return std::chrono::milliseconds(static_cast<std::int64_t>(milliseconds));
}

std::chrono::milliseconds add_saturated(std::chrono::milliseconds first,
                                        std::chrono::milliseconds second) {
    if (first == std::chrono::milliseconds::max() ||
        second == std::chrono::milliseconds::max() ||
        first.count() > std::chrono::milliseconds::max().count() - second.count()) {
        return std::chrono::milliseconds::max();
    }
    return first + second;
}

} // namespace

OffloadDecision OffloadDecisionEngine::evaluate(const OffloadInputs& inputs) const {
    OffloadDecision decision;
    decision.requestId = inputs.requestId;
    decision.predictedLocalCompletion = add_saturated(
        inputs.localQueueTime, inputs.localComputeTime);

    const bool invalid = inputs.requestId.empty() ||
                         inputs.deadline <= std::chrono::milliseconds::zero() ||
                         inputs.localQueueTime < std::chrono::milliseconds::zero() ||
                         inputs.localComputeTime < std::chrono::milliseconds::zero() ||
                         inputs.edgeQueueTime < std::chrono::milliseconds::zero() ||
                         inputs.edgeComputeTime < std::chrono::milliseconds::zero() ||
                         inputs.link.rtt < std::chrono::milliseconds::zero() ||
                         inputs.link.estimatedUplinkMbps < 0.0 ||
                         inputs.link.estimatedDownlinkMbps < 0.0 ||
                         inputs.uncertaintyMargin < std::chrono::milliseconds::zero() ||
                         inputs.minimumEdgeBenefit < std::chrono::milliseconds::zero() ||
                         inputs.minimumResultConfidence < 0.0 ||
                         inputs.minimumResultConfidence > 1.0 ||
                         inputs.expectedResultConfidence < 0.0 ||
                         inputs.expectedResultConfidence > 1.0;
    if (invalid) {
        decision.reason = OffloadReasonCode::INVALID_INPUT;
        decision.fallbackRequired = inputs.localFallbackAvailable;
        return decision;
    }

    const auto upload = transfer_time(inputs.uploadBytes, inputs.link.estimatedUplinkMbps);
    const auto download = transfer_time(inputs.downloadBytes, inputs.link.estimatedDownlinkMbps);
    auto edge = add_saturated(upload, inputs.link.rtt);
    edge = add_saturated(edge, inputs.edgeQueueTime);
    edge = add_saturated(edge, inputs.edgeComputeTime);
    edge = add_saturated(edge, download);
    edge = add_saturated(edge, inputs.uncertaintyMargin);
    decision.predictedEdgeCompletion = edge;

    auto choose_local = [&](OffloadReasonCode reason) {
        decision.executionSite = ExecutionSite::LOCAL;
        decision.reason = reason;
        decision.deadlineExpectedToBeMet =
            decision.predictedLocalCompletion <= inputs.deadline;
        decision.fallbackRequired = reason != OffloadReasonCode::LOCAL_FASTER;
    };

    if (!inputs.edgeServiceAvailable) {
        choose_local(OffloadReasonCode::EDGE_UNAVAILABLE);
        return decision;
    }
    if (!inputs.link.available || upload == std::chrono::milliseconds::max() ||
        download == std::chrono::milliseconds::max()) {
        choose_local(OffloadReasonCode::LINK_UNAVAILABLE);
        return decision;
    }
    if (inputs.expectedResultConfidence < inputs.minimumResultConfidence) {
        choose_local(OffloadReasonCode::INSUFFICIENT_CONFIDENCE);
        return decision;
    }

    const bool edgeMeetsDeadline = edge <= inputs.deadline;
    const bool localMeetsDeadline = decision.predictedLocalCompletion <= inputs.deadline;
    const bool edgeMateriallyFaster =
        edge != std::chrono::milliseconds::max() &&
        add_saturated(edge, inputs.minimumEdgeBenefit) < decision.predictedLocalCompletion;
    if (edgeMeetsDeadline && edgeMateriallyFaster) {
        decision.executionSite = ExecutionSite::EDGE;
        decision.reason = OffloadReasonCode::EDGE_FASTER;
        decision.deadlineExpectedToBeMet = true;
        return decision;
    }
    if (localMeetsDeadline) {
        choose_local(OffloadReasonCode::LOCAL_FASTER);
        return decision;
    }
    if (edgeMeetsDeadline) {
        decision.executionSite = ExecutionSite::EDGE;
        decision.reason = OffloadReasonCode::EDGE_FASTER;
        decision.deadlineExpectedToBeMet = true;
        return decision;
    }
    choose_local(OffloadReasonCode::DEADLINE_MISS_PREDICTED);
    decision.deadlineExpectedToBeMet = false;
    return decision;
}

std::string to_string(ExecutionSite site) {
    switch (site) {
        case ExecutionSite::LOCAL: return "local";
        case ExecutionSite::EDGE: return "edge";
        case ExecutionSite::HYBRID: return "hybrid";
        default: return "unknown";
    }
}

std::string to_string(OffloadReasonCode reason) {
    switch (reason) {
        case OffloadReasonCode::EDGE_FASTER: return "edge-faster";
        case OffloadReasonCode::LOCAL_FASTER: return "local-faster";
        case OffloadReasonCode::EDGE_UNAVAILABLE: return "edge-unavailable";
        case OffloadReasonCode::LINK_UNAVAILABLE: return "link-unavailable";
        case OffloadReasonCode::INSUFFICIENT_CONFIDENCE: return "insufficient-confidence";
        case OffloadReasonCode::DEADLINE_MISS_PREDICTED: return "deadline-miss-predicted";
        case OffloadReasonCode::INVALID_INPUT: return "invalid-input";
        default: return "unknown";
    }
}

} // namespace ipi::offload
