#pragma once

#include "ipi/api/types.hpp"
#include "ipi/core/ipi_cooperative_service.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ipi::mesh {

struct OffloadTask {
    std::string taskId;
    ServiceClass serviceClass{ServiceClass::GuidedPerception};
    std::vector<std::uint8_t> payload;
    std::optional<std::chrono::milliseconds> desiredHorizon{};
    std::optional<std::chrono::milliseconds> timeout{};
    std::optional<std::vector<std::uint8_t>> localFallbackPayload{};
};

enum class OffloadStatus {
    Pending,
    Accepted,
    Completed,
    Rejected,
    TimedOut,
    FallbackCompleted
};

struct OffloadProgress {
    std::string taskId;
    OffloadStatus status{OffloadStatus::Pending};
    CooperativeServiceMessage message{};
    api::FailureCode failureCode{api::FailureCode::NONE};
    std::optional<api::FallbackResult> fallbackResult{};
    std::string detail{};
};

class TaskOffloader {
public:
    using SendCallback = std::function<void(const CooperativeServiceMessage&)>;
    using ProgressCallback = std::function<void(const OffloadProgress&)>;

    TaskOffloader(SessionId sessionId,
                  std::vector<std::uint8_t> vehicleId,
                  SendCallback sendCallback,
                  std::function<std::chrono::steady_clock::time_point()> steadyClock = {});

    void set_progress_callback(ProgressCallback callback);

    void request_offload(const OffloadTask& task);

    [[nodiscard]] api::Ack handle_cooperative_message(
        const CooperativeServiceMessage& message);

    /** Expire due tasks and activate a retained local fallback when available. */
    std::size_t expire_due_tasks();

    [[nodiscard]] std::vector<std::string> pending_tasks() const;

private:
    SessionId sessionId_{};
    std::vector<std::uint8_t> vehicleId_;
    SendCallback sendCallback_;
    ProgressCallback progressCallback_;

    struct TaskState {
        OffloadTask descriptor;
        OffloadStatus status{OffloadStatus::Pending};
        std::optional<std::chrono::steady_clock::time_point> deadline{};
    };

    std::unordered_map<std::string, TaskState> activeTasks_;
    std::unordered_map<std::string, OffloadStatus> terminalTasks_;
    std::function<std::chrono::steady_clock::time_point()> steadyClock_;

    static std::uint16_t clamp_horizon(std::chrono::milliseconds horizon);
    void emit_progress(const std::string& taskId,
                       OffloadStatus status,
                       const CooperativeServiceMessage& message,
                       api::FailureCode failureCode = api::FailureCode::NONE,
                       std::optional<api::FallbackResult> fallbackResult = {},
                       std::string detail = {});
};

} // namespace ipi::mesh
