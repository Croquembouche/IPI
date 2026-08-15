#include "ipi/mesh/task_offloader.hpp"

#include <limits>
#include <stdexcept>

namespace ipi::mesh {

TaskOffloader::TaskOffloader(SessionId sessionId,
                             std::vector<std::uint8_t> vehicleId,
                             SendCallback sendCallback,
                             std::function<std::chrono::steady_clock::time_point()> steadyClock)
    : sessionId_(sessionId),
      vehicleId_(std::move(vehicleId)),
      sendCallback_(std::move(sendCallback)),
      steadyClock_(std::move(steadyClock)) {
    if (!sendCallback_) {
        throw std::invalid_argument("TaskOffloader requires a valid send callback");
    }
    if (!steadyClock_) {
        steadyClock_ = [] { return std::chrono::steady_clock::now(); };
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
                                  const CooperativeServiceMessage& message,
                                  api::FailureCode failureCode,
                                  std::optional<api::FallbackResult> fallbackResult,
                                  std::string detail) {
    if (progressCallback_) {
        progressCallback_({taskId, status, message, failureCode,
                           fallbackResult, std::move(detail)});
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

    TaskState state{task, OffloadStatus::Pending, std::nullopt};
    if (task.timeout) {
        if (*task.timeout <= std::chrono::milliseconds::zero()) {
            throw std::invalid_argument("Offload timeout must be positive");
        }
        state.deadline = steadyClock_() + *task.timeout;
    } else if (task.desiredHorizon && *task.desiredHorizon > std::chrono::milliseconds::zero()) {
        state.deadline = steadyClock_() + *task.desiredHorizon;
    }
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

    try {
        sendCallback_(message);
    } catch (...) {
        activeTasks_.erase(task.taskId);
        throw;
    }
}

api::Ack TaskOffloader::handle_cooperative_message(const CooperativeServiceMessage& message) {
    if (!message.offloadTaskId) {
        return api::Ack{false, std::nullopt, api::FailureCode::INVALID_REQUEST,
                        "offload response has no taskId"};
    }

    const auto& taskId = *message.offloadTaskId;
    if (terminalTasks_.find(taskId) != terminalTasks_.end()) {
        return api::Ack{false, taskId, api::FailureCode::DUPLICATE_RESPONSE,
                        "offload task already has a terminal response"};
    }
    auto it = activeTasks_.find(taskId);
    if (it == activeTasks_.end()) {
        return api::Ack{false, taskId, api::FailureCode::CORRELATION_MISMATCH,
                        "offload response does not match an active task"};
    }
    if (message.sessionId != sessionId_ || message.vehicleId != vehicleId_ ||
        message.serviceClass != it->second.descriptor.serviceClass) {
        return api::Ack{false, taskId, api::FailureCode::CORRELATION_MISMATCH,
                        "offload response session, vehicle, or service class mismatch"};
    }
    if (it->second.deadline && steadyClock_() >= *it->second.deadline) {
        (void)expire_due_tasks();
        return api::Ack{false, taskId, api::FailureCode::STALE_RESPONSE,
                        "offload response arrived after the task deadline"};
    }

    OffloadStatus newStatus = it->second.status;
    switch (message.guidanceStatus) {
        case GuidanceStatus::Update:
            if (it->second.status == OffloadStatus::Accepted) {
                return api::Ack{false, taskId, api::FailureCode::DUPLICATE_RESPONSE,
                                "duplicate offload acceptance update"};
            }
            newStatus = OffloadStatus::Accepted;
            break;
        case GuidanceStatus::Complete:
            newStatus = OffloadStatus::Completed;
            break;
        case GuidanceStatus::Reject:
            newStatus = OffloadStatus::Rejected;
            break;
        default:
            return api::Ack{false, taskId, api::FailureCode::PROTOCOL_ERROR,
                            "offload response has request status"};
    }

    it->second.status = newStatus;
    if (newStatus == OffloadStatus::Rejected) {
        emit_progress(taskId, newStatus, message, api::FailureCode::SERVICE_UNAVAILABLE,
                      api::FallbackResult::NOT_ATTEMPTED,
                      "edge offload request was rejected");
    } else {
        emit_progress(taskId, newStatus, message);
    }

    if (newStatus == OffloadStatus::Completed || newStatus == OffloadStatus::Rejected) {
        terminalTasks_[taskId] = newStatus;
        activeTasks_.erase(it);
    }
    return api::Ack{true, taskId};
}

std::size_t TaskOffloader::expire_due_tasks() {
    const auto now = steadyClock_();
    std::vector<std::string> expired;
    for (const auto& [taskId, state] : activeTasks_) {
        if (state.deadline && now >= *state.deadline) expired.push_back(taskId);
    }
    for (const auto& taskId : expired) {
        auto found = activeTasks_.find(taskId);
        if (found == activeTasks_.end()) continue;
        CooperativeServiceMessage timeoutMessage;
        timeoutMessage.sessionId = sessionId_;
        timeoutMessage.vehicleId = vehicleId_;
        timeoutMessage.serviceClass = found->second.descriptor.serviceClass;
        timeoutMessage.guidanceStatus = GuidanceStatus::Reject;
        timeoutMessage.offloadTaskId = taskId;
        if (found->second.descriptor.localFallbackPayload) {
            timeoutMessage.offloadPayload = *found->second.descriptor.localFallbackPayload;
            emit_progress(taskId, OffloadStatus::FallbackCompleted, timeoutMessage,
                          api::FailureCode::NONE, api::FallbackResult::SUCCEEDED,
                          "offload deadline expired; retained local fallback activated");
            terminalTasks_[taskId] = OffloadStatus::FallbackCompleted;
        } else {
            emit_progress(taskId, OffloadStatus::TimedOut, timeoutMessage,
                          api::FailureCode::TIMEOUT, api::FallbackResult::NOT_ATTEMPTED,
                          "offload task deadline expired");
            terminalTasks_[taskId] = OffloadStatus::TimedOut;
        }
        activeTasks_.erase(found);
    }
    return expired.size();
}

std::vector<std::string> TaskOffloader::pending_tasks() const {
    std::vector<std::string> ids;
    ids.reserve(activeTasks_.size());
    for (const auto& [taskId, state] : activeTasks_) {
        if (state.status != OffloadStatus::Completed && state.status != OffloadStatus::Rejected &&
            state.status != OffloadStatus::TimedOut &&
            state.status != OffloadStatus::FallbackCompleted) {
            ids.push_back(taskId);
        }
    }
    return ids;
}

} // namespace ipi::mesh
