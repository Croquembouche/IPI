#pragma once

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
};

enum class OffloadStatus {
    Pending,
    Accepted,
    Completed,
    Rejected
};

struct OffloadProgress {
    std::string taskId;
    OffloadStatus status{OffloadStatus::Pending};
    CooperativeServiceMessage message{};
};

class TaskOffloader {
public:
    using SendCallback = std::function<void(const CooperativeServiceMessage&)>;
    using ProgressCallback = std::function<void(const OffloadProgress&)>;

    TaskOffloader(SessionId sessionId,
                  std::vector<std::uint8_t> vehicleId,
                  SendCallback sendCallback);

    void set_progress_callback(ProgressCallback callback);

    void request_offload(const OffloadTask& task);

    void handle_cooperative_message(const CooperativeServiceMessage& message);

    [[nodiscard]] std::vector<std::string> pending_tasks() const;

private:
    SessionId sessionId_{};
    std::vector<std::uint8_t> vehicleId_;
    SendCallback sendCallback_;
    ProgressCallback progressCallback_;

    struct TaskState {
        OffloadTask descriptor;
        OffloadStatus status{OffloadStatus::Pending};
    };

    std::unordered_map<std::string, TaskState> activeTasks_;

    static std::uint16_t clamp_horizon(std::chrono::milliseconds horizon);
    void emit_progress(const std::string& taskId, OffloadStatus status, const CooperativeServiceMessage& message);
};

} // namespace ipi::mesh

