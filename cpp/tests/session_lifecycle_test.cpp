#include "test_support.hpp"

#include "ipi/api/session_lifecycle.hpp"

#include <chrono>

int main() {
    return ipi::tests::run_test("session_lifecycle_enforcement", [] {
        using namespace std::chrono_literals;
        using namespace ipi::api;

        Timestamp wall = Timestamp{1000s};
        SteadyTimestamp steady = SteadyTimestamp{100s};
        std::uint64_t ids = 0;
        SessionLifecycleOptions options;
        options.leaseSeconds = 10;
        options.heartbeatIntervalSeconds = 2;
        options.wallClock = [&] { return wall; };
        options.steadyClock = [&] { return steady; };
        options.sessionIdGenerator = [&] { return "session-" + std::to_string(++ids); };
        SessionLifecycle lifecycle(options);

        SessionRegistration registration;
        registration.metadata.messageId = "registration-1";
        registration.metadata.sentAt = wall;
        registration.metadata.intersectionId = "intersection-1";
        registration.metadata.source.id = "vehicle-1";
        registration.vehicleProfile.vehicleId = "vehicle-1";
        registration.vehicleProfile.role = VehicleRole::PCAV;
        registration.requestedServices = {"planningAid"};
        registration.rsuFallback = "rsu-2";
        registration.minSidelinkRssi = -91;
        registration.inlineSubscription = Subscription{"https://callback", {"service"}, {1, 2}};

        const auto descriptor = lifecycle.register_session(registration);
        ipi::tests::expect(descriptor.state == SessionState::REGISTERED,
                           "new session must start registered");
        ipi::tests::expect(descriptor.rsuFallback == registration.rsuFallback,
                           "fallback RSU must be retained");
        ipi::tests::expect(descriptor.minSidelinkRssi == registration.minSidelinkRssi,
                           "minimum RSSI must be retained");
        ipi::tests::expect(descriptor.inlineSubscription.has_value(),
                           "inline subscription must be retained");

        ServiceInvocation invocation;
        invocation.sessionId = descriptor.sessionId;
        invocation.request.metadata.messageId = "service-message-1";
        invocation.request.metadata.sentAt = wall;
        invocation.request.metadata.intersectionId = "intersection-1";
        invocation.request.metadata.source.id = "vehicle-1";
        invocation.request.metadata.sessionId = descriptor.sessionId;
        invocation.request.metadata.correlationId = "request-1";
        invocation.request.metadata.sequence = 1;
        invocation.request.metadata.expiresAt = wall + 5s;
        invocation.request.data.serviceType = PCAVServiceType::PLANNING_AID;
        invocation.request.data.vehicleId = "vehicle-1";
        ipi::tests::expect(
            lifecycle.authorize_service(invocation).code == FailureCode::SESSION_INACTIVE,
            "service must be rejected before first heartbeat");

        HeartbeatUpdate heartbeat{descriptor.sessionId, wall, std::nullopt};
        ipi::tests::expect(lifecycle.heartbeat(heartbeat).accepted,
                           "first heartbeat should activate the session");
        ipi::tests::expect(lifecycle.get_session(descriptor.sessionId)->state == SessionState::ACTIVE,
                           "heartbeat must transition to active");
        ipi::tests::expect(lifecycle.authorize_service(invocation).accepted,
                           "active granted service should be authorized");
        ipi::tests::expect(
            lifecycle.authorize_service(invocation).code == FailureCode::STALE_REQUEST,
            "duplicate request identity must be rejected");

        Envelope<VehicleServiceResponse> progress;
        progress.metadata = invocation.request.metadata;
        progress.data.serviceType = invocation.request.data.serviceType;
        progress.data.vehicleId = "vehicle-1";
        progress.data.status = VehicleServiceStatus::IN_PROGRESS;
        progress.data.resultId = "result-1";
        ipi::tests::expect(lifecycle.accept_service_response(descriptor.sessionId, progress).accepted,
                           "matching progress response should be accepted");
        ipi::tests::expect(
            lifecycle.accept_service_response(descriptor.sessionId, progress).code ==
                FailureCode::DUPLICATE_RESPONSE,
            "duplicate response identity must be rejected");

        auto completed = progress;
        completed.data.status = VehicleServiceStatus::COMPLETED;
        completed.data.resultId = "result-2";
        ipi::tests::expect(lifecycle.accept_service_response(descriptor.sessionId, completed).accepted,
                           "matching terminal response should be accepted");
        auto lateTerminal = completed;
        lateTerminal.data.resultId = "result-3";
        ipi::tests::expect(
            lifecycle.accept_service_response(descriptor.sessionId, lateTerminal).code ==
                FailureCode::STALE_RESPONSE,
            "second terminal response must be rejected as stale");

        SessionPatch ownershipChange;
        ownershipChange.sessionId = descriptor.sessionId;
        ownershipChange.profile = descriptor.vehicleProfile;
        ownershipChange.profile->vehicleId = "vehicle-2";
        ipi::tests::expect(
            lifecycle.patch_session(ownershipChange).code == FailureCode::ACCESS_DENIED,
            "patch must not change session ownership");

        steady += 10s;
        wall += 10s;
        ipi::tests::expect(lifecycle.get_session(descriptor.sessionId)->state == SessionState::EXPIRED,
                           "lease must expire exactly at the monotonic deadline");
        TelemetrySubmission telemetry{descriptor.sessionId, {VehicleTelemetryFrame{}}};
        ipi::tests::expect(
            lifecycle.authorize_telemetry(telemetry).code == FailureCode::SESSION_EXPIRED,
            "expired session telemetry must be rejected");

        wall += 1s;
        steady += 1s;
        registration.metadata.messageId = "registration-2";
        const auto second = lifecycle.register_session(registration);
        ipi::tests::expect(lifecycle.heartbeat({second.sessionId, wall, std::nullopt}).accepted,
                           "second session should activate");
        SessionTermination termination{second.sessionId,
                                       TerminationReasonCode::CLIENT_REQUEST,
                                       "driver exit"};
        ipi::tests::expect(lifecycle.terminate_session(termination).accepted,
                           "active session should terminate");
        ipi::tests::expect(
            lifecycle.heartbeat({second.sessionId, wall + 1s, std::nullopt}).code ==
                FailureCode::SESSION_TERMINATED,
            "terminated session cannot be revived by heartbeat");
        const auto snapshot = lifecycle.get_snapshot(second.sessionId);
        ipi::tests::expect(snapshot->terminationReason == TerminationReasonCode::CLIENT_REQUEST,
                           "structured termination reason must be retained");
    });
}
