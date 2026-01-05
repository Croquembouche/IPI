#include "ipi/mesh/task_offloader.hpp"

#include <limits>
#include <stdexcept>

namespace ipi::mesh {

TaskOffloader::TaskOffloader(SessionId sessionId,
                             std::vector<std::uint8_t> vehicleId,
                             SendCallback sendCallback)
    : sessionId_(sessionId),
      vehicleId_(std::move(vehicleId)),
      sendCallback_(std::move(sendCallback)) {
    if (!sendCallback_) {
        throw std::invalid_argument("TaskOffloader requires a valid send callback");
    }
}

void TaskOffloader::set_progress_callback(ProgressCallback callback) {
    progressCallback_ = std::move(callback);
}

std::uint16_t TaskOffloader::clamp_horizon(std::chrono::milliseconds horizon) {
    const auto count = horizon.count();
    if (count <= 0) {
        return 0;
    }
    if (count > std::numeric_limits<std::uint16_t>::max()) {
        return std::numeric_limits<std::uint16_t>::max();
    }
    return static_cast<std::uint16_t>(count);
}

void TaskOffloader::emit_progress(const std::string& taskId,
                                  OffloadStatus status,
                                  const CooperativeServiceMessage& message) {
    if (progressCallback_) {
        progressCallback_({taskId, status, message});
    }
}

void TaskOffloader::request_offload(const OffloadTask& task) {
    if (task.taskId.empty()) {
        throw std::invalid_argument("Offload taskId must not be empty");
    }
    if (task.payload.size() > 65535) {
        throw std::invalid_argument("Offload payload exceeds 65535 bytes");
    }
    if (activeTasks_.count(task.taskId) != 0U) {
        throw std::invalid_argument("Offload taskId already pending");
    }

    TaskState state{task, OffloadStatus::Pending};
    activeTasks_.emplace(task.taskId, state);

    CooperativeServiceMessage message;
    message.sessionId = sessionId_;
    message.vehicleId = vehicleId_;
    message.serviceClass = task.serviceClass;
    message.guidanceStatus = GuidanceStatus::Request;
    if (task.desiredHorizon) {
        message.requestedHorizonMs = clamp_horizon(*task.desiredHorizon);
    }
    message.offloadTaskId = task.taskId;
    message.offloadPayload = task.payload;

    sendCallback_(message);
}

void TaskOffloader::handle_cooperative_message(const CooperativeServiceMessage& message) {
    if (!message.offloadTaskId) {
        return;
    }

    const auto& taskId = *message.offloadTaskId;
    auto it = activeTasks_.find(taskId);
    if (it == activeTasks_.end()) {
        return;
    }

    OffloadStatus newStatus = it->second.status;
    switch (message.guidanceStatus) {
        case GuidanceStatus::Update:
            newStatus = OffloadStatus::Accepted;
            break;
        case GuidanceStatus::Complete:
            newStatus = OffloadStatus::Completed;
            break;
        case GuidanceStatus::Reject:
            newStatus = OffloadStatus::Rejected;
            break;
        default:
            break;
    }

    it->second.status = newStatus;
    emit_progress(taskId, newStatus, message);

    if (newStatus == OffloadStatus::Completed || newStatus == OffloadStatus::Rejected) {
        activeTasks_.erase(it);
    }
}

std::vector<std::string> TaskOffloader::pending_tasks() const {
    std::vector<std::string> ids;
    ids.reserve(activeTasks_.size());
    for (const auto& [taskId, state] : activeTasks_) {
        if (state.status != OffloadStatus::Completed && state.status != OffloadStatus::Rejected) {
            ids.push_back(taskId);
        }
    }
    return ids;
}

} // namespace ipi::mesh

