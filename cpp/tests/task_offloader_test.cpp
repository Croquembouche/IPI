#include "test_support.hpp"

#include "ipi/mesh/task_offloader.hpp"

#include <chrono>

int main() {
    return ipi::tests::run_test("task_offloader_correlation_and_fallback", [] {
        using namespace std::chrono_literals;

        ipi::SessionId session{};
        session[0] = 0x42;
        std::vector<std::uint8_t> vehicle{1, 2, 3, 4};
        auto now = std::chrono::steady_clock::time_point{10s};
        std::vector<ipi::CooperativeServiceMessage> sent;
        std::vector<ipi::mesh::OffloadProgress> progress;
        ipi::mesh::TaskOffloader offloader(
            session, vehicle,
            [&](const ipi::CooperativeServiceMessage& message) { sent.push_back(message); },
            [&] { return now; });
        offloader.set_progress_callback(
            [&](const ipi::mesh::OffloadProgress& update) { progress.push_back(update); });

        ipi::mesh::OffloadTask task;
        task.taskId = "task-1";
        task.serviceClass = ipi::ServiceClass::GuidedPerception;
        task.payload = {1, 2, 3};
        task.timeout = 5s;
        offloader.request_offload(task);
        ipi::tests::expect(sent.size() == 1 && sent.front().sessionId == session,
                           "offload request must bind expected session");

        ipi::CooperativeServiceMessage update = sent.front();
        update.guidanceStatus = ipi::GuidanceStatus::Update;
        ipi::tests::expect(offloader.handle_cooperative_message(update).accepted,
                           "matching update should be accepted");
        ipi::tests::expect(
            offloader.handle_cooperative_message(update).code ==
                ipi::api::FailureCode::DUPLICATE_RESPONSE,
            "duplicate update should be rejected");

        auto mismatch = update;
        mismatch.sessionId[1] = 0x99;
        mismatch.guidanceStatus = ipi::GuidanceStatus::Complete;
        ipi::tests::expect(
            offloader.handle_cooperative_message(mismatch).code ==
                ipi::api::FailureCode::CORRELATION_MISMATCH,
            "session mismatch should be rejected");

        auto complete = update;
        complete.guidanceStatus = ipi::GuidanceStatus::Complete;
        ipi::tests::expect(offloader.handle_cooperative_message(complete).accepted,
                           "matching completion should be accepted");
        ipi::tests::expect(
            offloader.handle_cooperative_message(complete).code ==
                ipi::api::FailureCode::DUPLICATE_RESPONSE,
            "second terminal response should be rejected");

        ipi::mesh::OffloadTask fallbackTask;
        fallbackTask.taskId = "task-2";
        fallbackTask.serviceClass = ipi::ServiceClass::GuidedPlanning;
        fallbackTask.payload = {4, 5};
        fallbackTask.timeout = 2s;
        fallbackTask.localFallbackPayload = std::vector<std::uint8_t>{8, 9};
        offloader.request_offload(fallbackTask);
        now += 2s;
        ipi::tests::expect(offloader.expire_due_tasks() == 1,
                           "deadline should expire exactly at monotonic boundary");
        ipi::tests::expect(progress.back().status ==
                               ipi::mesh::OffloadStatus::FallbackCompleted,
                           "expired task with retained input should complete local fallback");
        ipi::tests::expect(progress.back().fallbackResult ==
                               ipi::api::FallbackResult::SUCCEEDED,
                           "fallback terminal result should be machine-readable");
        ipi::tests::expect(offloader.pending_tasks().empty(),
                           "terminal and expired tasks must leave active set");
    });
}
