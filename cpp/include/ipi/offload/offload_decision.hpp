#pragma once

#include "ipi/policy/service_policy.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

namespace ipi::offload {

enum class ExecutionSite : std::uint8_t {
    LOCAL = 0,
    EDGE = 1,
    HYBRID = 2
};

enum class OffloadReasonCode : std::uint8_t {
    EDGE_FASTER = 0,
    LOCAL_FASTER = 1,
    EDGE_UNAVAILABLE = 2,
    LINK_UNAVAILABLE = 3,
    INSUFFICIENT_CONFIDENCE = 4,
    DEADLINE_MISS_PREDICTED = 5,
    INVALID_INPUT = 6
};

struct OffloadInputs {
    std::string requestId{};
    std::chrono::milliseconds deadline{1000};
    std::chrono::milliseconds localQueueTime{0};
    std::chrono::milliseconds localComputeTime{0};
    std::size_t uploadBytes{0};
    std::size_t downloadBytes{0};
    policy::LinkSnapshot link{};
    std::chrono::milliseconds edgeQueueTime{0};
    std::chrono::milliseconds edgeComputeTime{0};
    std::chrono::milliseconds uncertaintyMargin{0};
    std::chrono::milliseconds minimumEdgeBenefit{5};
    double minimumResultConfidence{0.0};
    double expectedResultConfidence{1.0};
    bool edgeServiceAvailable{true};
    bool localFallbackAvailable{true};
};

struct OffloadDecision {
    std::string requestId{};
    ExecutionSite executionSite{ExecutionSite::LOCAL};
    std::chrono::milliseconds predictedLocalCompletion{0};
    std::chrono::milliseconds predictedEdgeCompletion{0};
    bool deadlineExpectedToBeMet{false};
    bool fallbackRequired{false};
    OffloadReasonCode reason{OffloadReasonCode::INVALID_INPUT};
};

class OffloadDecisionEngine {
public:
    [[nodiscard]] OffloadDecision evaluate(const OffloadInputs& inputs) const;
};

[[nodiscard]] std::string to_string(ExecutionSite site);
[[nodiscard]] std::string to_string(OffloadReasonCode reason);

} // namespace ipi::offload
