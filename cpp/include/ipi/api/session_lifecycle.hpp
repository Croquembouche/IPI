#pragma once

#include "ipi/api/types.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace ipi::api {

using SteadyTimestamp = std::chrono::steady_clock::time_point;

struct SessionLifecycleOptions {
    std::uint32_t leaseSeconds{30};
    std::uint32_t heartbeatIntervalSeconds{5};
    std::function<Timestamp()> wallClock{};
    std::function<SteadyTimestamp()> steadyClock{};
    std::function<std::string()> sessionIdGenerator{};
};

struct SessionSnapshot {
    SessionDescriptor descriptor{};
    SessionRegistration registration{};
    std::optional<TerminationReasonCode> terminationReason{};
    std::string terminationDetail{};
};

/**
 * Transport-independent session authority.
 *
 * Lease deadlines use a monotonic clock. Absolute timestamps remain wall-clock
 * values for protocol exchange and log alignment. All session transports must
 * call this object before delegating an operation to a ReceiverApi.
 */
class SessionLifecycle {
public:
    explicit SessionLifecycle(SessionLifecycleOptions options = {});
    ~SessionLifecycle();

    SessionLifecycle(const SessionLifecycle&) = delete;
    SessionLifecycle& operator=(const SessionLifecycle&) = delete;
    SessionLifecycle(SessionLifecycle&&) noexcept;
    SessionLifecycle& operator=(SessionLifecycle&&) noexcept;

    SessionDescriptor register_session(const SessionRegistration& registration);
    Ack heartbeat(const HeartbeatUpdate& heartbeat);
    Ack patch_session(const SessionPatch& patch);
    Ack terminate_session(const SessionTermination& termination);

    Ack authorize_service(const ServiceInvocation& invocation);
    Ack authorize_telemetry(const TelemetrySubmission& submission);
    Ack accept_service_response(
        const std::string& sessionId,
        const Envelope<VehicleServiceResponse>& response);

    [[nodiscard]] std::optional<SessionDescriptor> get_session(
        const std::string& sessionId) const;
    [[nodiscard]] std::optional<SessionSnapshot> get_snapshot(
        const std::string& sessionId) const;

    /** Lazily marks every due lease expired and returns the number changed. */
    std::size_t expire_due_sessions();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

[[nodiscard]] std::string service_contract_name(
    const std::variant<PCVServiceType, PCAVServiceType>& serviceType);

} // namespace ipi::api
