#include "ipi/api/session_lifecycle.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <iomanip>
#include <mutex>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace ipi::api {
namespace {

Ack reject(FailureCode code, std::string detail) {
    Ack result;
    result.accepted = false;
    result.code = code;
    result.detail = std::move(detail);
    return result;
}

bool is_terminal(SessionState state) {
    return state == SessionState::TERMINATED || state == SessionState::EXPIRED;
}

bool contains_service(const std::vector<std::string>& services, const std::string& service) {
    return std::find(services.begin(), services.end(), service) != services.end();
}

std::string default_session_id() {
    static std::atomic<std::uint64_t> counter{0};
    static std::random_device random;
    std::array<std::uint32_t, 4> words{{random(), random(), random(), random()}};
    words[3] ^= static_cast<std::uint32_t>(++counter);

    std::ostringstream out;
    out << "ipi-session-" << std::hex << std::setfill('0');
    for (const auto word : words) {
        out << std::setw(8) << word;
    }
    return out.str();
}

bool timestamp_is_default(Timestamp value) {
    return value.time_since_epoch() == Timestamp::duration::zero();
}

} // namespace

struct SessionLifecycle::Impl {
    struct Record {
        struct OutstandingRequest {
            std::string messageId{};
            std::string vehicleId{};
            bool terminal{false};
        };

        SessionRegistration registration{};
        SessionDescriptor descriptor{};
        SteadyTimestamp leaseDeadline{};
        bool receivedHeartbeat{false};
        Timestamp lastClientHeartbeat{};
        std::optional<TerminationReasonCode> terminationReason{};
        std::string terminationDetail{};
        std::unordered_set<std::string> messageIds{};
        std::unordered_set<std::string> correlationIds{};
        std::optional<std::uint64_t> lastSequence{};
        std::unordered_map<std::string, OutstandingRequest> outstandingRequests{};
        std::unordered_set<std::string> responseIds{};
    };

    explicit Impl(SessionLifecycleOptions input)
        : options(std::move(input)) {
        if (options.leaseSeconds == 0) {
            throw std::invalid_argument("session lease must be greater than zero");
        }
        if (options.heartbeatIntervalSeconds == 0 ||
            options.heartbeatIntervalSeconds > options.leaseSeconds) {
            throw std::invalid_argument(
                "heartbeat interval must be greater than zero and no longer than the lease");
        }
        if (!options.wallClock) {
            options.wallClock = [] { return std::chrono::system_clock::now(); };
        }
        if (!options.steadyClock) {
            options.steadyClock = [] { return std::chrono::steady_clock::now(); };
        }
        if (!options.sessionIdGenerator) {
            options.sessionIdGenerator = default_session_id;
        }
    }

    SessionLifecycleOptions options;
    mutable std::mutex mutex;
    mutable std::unordered_map<std::string, Record> records;

    void expire_if_due(Record& record, SteadyTimestamp steadyNow, Timestamp wallNow) const {
        if (!is_terminal(record.descriptor.state) && steadyNow >= record.leaseDeadline) {
            record.descriptor.state = SessionState::EXPIRED;
            record.descriptor.expiresAt = wallNow;
            record.terminationReason = TerminationReasonCode::LEASE_EXPIRED;
            record.terminationDetail = "session lease expired";
        }
    }

    Ack require_operable(Record& record,
                         const std::string& sessionId,
                         bool requireActive) const {
        const auto steadyNow = options.steadyClock();
        const auto wallNow = options.wallClock();
        expire_if_due(record, steadyNow, wallNow);

        if (record.descriptor.state == SessionState::EXPIRED) {
            return reject(FailureCode::SESSION_EXPIRED,
                          "session " + sessionId + " has expired");
        }
        if (record.descriptor.state == SessionState::TERMINATED) {
            return reject(FailureCode::SESSION_TERMINATED,
                          "session " + sessionId + " is terminated");
        }
        if (requireActive && record.descriptor.state != SessionState::ACTIVE) {
            return reject(FailureCode::SESSION_INACTIVE,
                          "session " + sessionId + " is not active");
        }
        return Ack{};
    }
};

SessionLifecycle::SessionLifecycle(SessionLifecycleOptions options)
    : impl_(std::make_unique<Impl>(std::move(options))) {}

SessionLifecycle::~SessionLifecycle() = default;
SessionLifecycle::SessionLifecycle(SessionLifecycle&&) noexcept = default;
SessionLifecycle& SessionLifecycle::operator=(SessionLifecycle&&) noexcept = default;

SessionDescriptor SessionLifecycle::register_session(const SessionRegistration& registration) {
    if (registration.metadata.intersectionId.empty()) {
        throw std::invalid_argument("session registration requires an intersectionId");
    }
    if (registration.vehicleProfile.vehicleId.empty()) {
        throw std::invalid_argument("session registration requires a vehicleId");
    }
    if (!registration.metadata.source.id.empty() &&
        registration.metadata.source.id != registration.vehicleProfile.vehicleId) {
        throw std::invalid_argument("session registration source does not own vehicle profile");
    }
    for (const auto& service : registration.requestedServices) {
        if (service.empty()) {
            throw std::invalid_argument("requested service names must not be empty");
        }
    }

    const auto wallNow = impl_->options.wallClock();
    const auto steadyNow = impl_->options.steadyClock();
    const auto lease = std::chrono::seconds(impl_->options.leaseSeconds);

    Impl::Record record;
    record.registration = registration;
    record.descriptor.sessionId = registration.metadata.sessionId.value_or(
        impl_->options.sessionIdGenerator());
    if (record.descriptor.sessionId.empty()) {
        throw std::runtime_error("session id generator returned an empty identifier");
    }
    record.descriptor.vehicleProfile = registration.vehicleProfile;
    record.descriptor.transport = registration.metadata.transport;
    record.descriptor.state = SessionState::REGISTERED;
    record.descriptor.leaseSeconds = impl_->options.leaseSeconds;
    record.descriptor.heartbeatIntervalSeconds = impl_->options.heartbeatIntervalSeconds;
    record.descriptor.preferredChannels = registration.requestedServices;
    record.descriptor.grantedServices = registration.requestedServices;
    record.descriptor.registeredAt = wallNow;
    record.descriptor.lastHeartbeatAt = wallNow;
    record.descriptor.expiresAt = wallNow + lease;
    record.descriptor.rsuFallback = registration.rsuFallback;
    record.descriptor.minSidelinkRssi = registration.minSidelinkRssi;
    record.descriptor.inlineSubscription = registration.inlineSubscription;
    record.leaseDeadline = steadyNow + lease;

    std::lock_guard<std::mutex> lock(impl_->mutex);
    if (impl_->records.find(record.descriptor.sessionId) != impl_->records.end()) {
        throw std::invalid_argument("duplicate session id: " + record.descriptor.sessionId);
    }
    const auto sessionId = record.descriptor.sessionId;
    auto [it, inserted] = impl_->records.emplace(sessionId, std::move(record));
    if (!inserted) {
        throw std::runtime_error("failed to store session record");
    }
    return it->second.descriptor;
}

Ack SessionLifecycle::heartbeat(const HeartbeatUpdate& heartbeat) {
    if (heartbeat.sessionId.empty()) {
        return reject(FailureCode::INVALID_REQUEST, "heartbeat requires a sessionId");
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto found = impl_->records.find(heartbeat.sessionId);
    if (found == impl_->records.end()) {
        return reject(FailureCode::UNKNOWN_SESSION,
                      "unknown session " + heartbeat.sessionId);
    }
    auto& record = found->second;
    if (auto state = impl_->require_operable(record, heartbeat.sessionId, false);
        !state.accepted) {
        return state;
    }

    const auto wallNow = impl_->options.wallClock();
    const auto supplied = timestamp_is_default(heartbeat.timestamp) ? wallNow : heartbeat.timestamp;
    if (record.receivedHeartbeat && supplied <= record.lastClientHeartbeat) {
        return reject(FailureCode::STALE_REQUEST,
                      "heartbeat timestamp is not newer than the prior heartbeat");
    }

    record.receivedHeartbeat = true;
    record.lastClientHeartbeat = supplied;
    record.descriptor.lastHeartbeatAt = wallNow;
    record.descriptor.expiresAt = wallNow + std::chrono::seconds(record.descriptor.leaseSeconds);
    record.leaseDeadline = impl_->options.steadyClock() +
                           std::chrono::seconds(record.descriptor.leaseSeconds);
    record.descriptor.state = SessionState::ACTIVE;
    return Ack{};
}

Ack SessionLifecycle::patch_session(const SessionPatch& patch) {
    if (patch.sessionId.empty() || (!patch.profile && !patch.preferredChannels)) {
        return reject(FailureCode::INVALID_REQUEST,
                      "session patch requires a sessionId and at least one change");
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto found = impl_->records.find(patch.sessionId);
    if (found == impl_->records.end()) {
        return reject(FailureCode::UNKNOWN_SESSION, "unknown session " + patch.sessionId);
    }
    auto& record = found->second;
    if (auto state = impl_->require_operable(record, patch.sessionId, false);
        !state.accepted) {
        return state;
    }
    if (patch.profile) {
        if (patch.profile->vehicleId.empty() ||
            patch.profile->vehicleId != record.descriptor.vehicleProfile.vehicleId) {
            return reject(FailureCode::ACCESS_DENIED,
                          "a session patch cannot change vehicle ownership");
        }
        record.registration.vehicleProfile = *patch.profile;
        record.descriptor.vehicleProfile = *patch.profile;
    }
    if (patch.preferredChannels) {
        if (std::any_of(patch.preferredChannels->begin(), patch.preferredChannels->end(),
                        [](const std::string& value) { return value.empty(); })) {
            return reject(FailureCode::INVALID_REQUEST,
                          "preferred channel names must not be empty");
        }
        record.descriptor.preferredChannels = *patch.preferredChannels;
    }
    return Ack{};
}

Ack SessionLifecycle::terminate_session(const SessionTermination& termination) {
    if (termination.sessionId.empty()) {
        return reject(FailureCode::INVALID_REQUEST, "termination requires a sessionId");
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto found = impl_->records.find(termination.sessionId);
    if (found == impl_->records.end()) {
        return reject(FailureCode::UNKNOWN_SESSION,
                      "unknown session " + termination.sessionId);
    }
    auto& record = found->second;
    impl_->expire_if_due(record, impl_->options.steadyClock(), impl_->options.wallClock());
    if (record.descriptor.state == SessionState::EXPIRED) {
        return reject(FailureCode::SESSION_EXPIRED,
                      "session " + termination.sessionId + " has expired");
    }
    if (record.descriptor.state == SessionState::TERMINATED) {
        return Ack{true, termination.sessionId};
    }
    record.descriptor.state = SessionState::TERMINATED;
    record.terminationReason = termination.reasonCode;
    record.terminationDetail = termination.reason;
    return Ack{true, termination.sessionId};
}

Ack SessionLifecycle::authorize_service(const ServiceInvocation& invocation) {
    if (invocation.sessionId.empty()) {
        return reject(FailureCode::INVALID_REQUEST, "service invocation requires a sessionId");
    }
    if (invocation.request.metadata.messageId.empty() ||
        !invocation.request.metadata.correlationId) {
        return reject(FailureCode::INVALID_REQUEST,
                      "service invocation requires messageId and correlationId");
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto found = impl_->records.find(invocation.sessionId);
    if (found == impl_->records.end()) {
        return reject(FailureCode::UNKNOWN_SESSION,
                      "unknown session " + invocation.sessionId);
    }
    auto& record = found->second;
    if (auto state = impl_->require_operable(record, invocation.sessionId, true);
        !state.accepted) {
        return state;
    }

    const auto& metadata = invocation.request.metadata;
    if (!metadata.sessionId || *metadata.sessionId != invocation.sessionId) {
        return reject(FailureCode::CORRELATION_MISMATCH,
                      "request metadata session does not match invocation session");
    }
    if (metadata.intersectionId != record.registration.metadata.intersectionId) {
        return reject(FailureCode::ACCESS_DENIED,
                      "request intersection does not match registered session");
    }
    if (invocation.request.data.vehicleId != record.descriptor.vehicleProfile.vehicleId ||
        (!metadata.source.id.empty() &&
         metadata.source.id != record.descriptor.vehicleProfile.vehicleId)) {
        return reject(FailureCode::ACCESS_DENIED,
                      "request vehicle does not own the session");
    }
    if (metadata.expiresAt && *metadata.expiresAt <= impl_->options.wallClock()) {
        return reject(FailureCode::STALE_REQUEST, "service request is expired");
    }

    const auto service = service_contract_name(invocation.request.data.serviceType);
    if (!contains_service(record.descriptor.grantedServices, service)) {
        return reject(FailureCode::SERVICE_UNAVAILABLE,
                      "service is not granted for this session: " + service);
    }
    const bool insertedMessage = record.messageIds.insert(metadata.messageId).second;
    const bool insertedCorrelation = record.correlationIds.insert(*metadata.correlationId).second;
    if (!insertedMessage || !insertedCorrelation) {
        if (insertedMessage) record.messageIds.erase(metadata.messageId);
        if (insertedCorrelation) record.correlationIds.erase(*metadata.correlationId);
        return reject(FailureCode::STALE_REQUEST,
                      "duplicate service request identity");
    }
    if (metadata.sequence) {
        if (record.lastSequence && *metadata.sequence <= *record.lastSequence) {
            record.messageIds.erase(metadata.messageId);
            record.correlationIds.erase(*metadata.correlationId);
            return reject(FailureCode::STALE_REQUEST,
                          "service sequence is not newer than the prior sequence");
        }
        record.lastSequence = *metadata.sequence;
    }
    record.outstandingRequests.emplace(
        *metadata.correlationId,
        Impl::Record::OutstandingRequest{metadata.messageId,
                                         invocation.request.data.vehicleId, false});
    return Ack{};
}

Ack SessionLifecycle::authorize_telemetry(const TelemetrySubmission& submission) {
    if (submission.sessionId.empty() || submission.frames.empty()) {
        return reject(FailureCode::INVALID_REQUEST,
                      "telemetry requires a sessionId and at least one frame");
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto found = impl_->records.find(submission.sessionId);
    if (found == impl_->records.end()) {
        return reject(FailureCode::UNKNOWN_SESSION,
                      "unknown session " + submission.sessionId);
    }
    return impl_->require_operable(found->second, submission.sessionId, true);
}

Ack SessionLifecycle::accept_service_response(
    const std::string& sessionId,
    const Envelope<VehicleServiceResponse>& response) {
    if (sessionId.empty() || !response.metadata.sessionId ||
        *response.metadata.sessionId != sessionId ||
        !response.metadata.correlationId) {
        return reject(FailureCode::CORRELATION_MISMATCH,
                      "service response lacks matching session and correlation identity");
    }
    try {
        validate_vehicle_service_response(response.data);
    } catch (const std::exception& error) {
        return reject(FailureCode::PROTOCOL_ERROR, error.what());
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto found = impl_->records.find(sessionId);
    if (found == impl_->records.end()) {
        return reject(FailureCode::UNKNOWN_SESSION, "unknown session " + sessionId);
    }
    auto& record = found->second;
    if (auto state = impl_->require_operable(record, sessionId, false); !state.accepted) {
        return state;
    }
    const auto request = record.outstandingRequests.find(*response.metadata.correlationId);
    if (request == record.outstandingRequests.end()) {
        return reject(FailureCode::CORRELATION_MISMATCH,
                      "response does not match an outstanding request");
    }
    if (response.data.vehicleId != request->second.vehicleId ||
        response.data.vehicleId != record.descriptor.vehicleProfile.vehicleId) {
        return reject(FailureCode::CORRELATION_MISMATCH,
                      "response vehicle does not match the outstanding request");
    }
    if (response.data.expiresAt && *response.data.expiresAt <= impl_->options.wallClock()) {
        return reject(FailureCode::STALE_RESPONSE, "service response has expired");
    }
    if (!record.responseIds.insert(response.data.resultId).second) {
        return reject(FailureCode::DUPLICATE_RESPONSE, "duplicate service result identity");
    }
    if (request->second.terminal) {
        record.responseIds.erase(response.data.resultId);
        return reject(FailureCode::STALE_RESPONSE,
                      "service request already has a terminal result");
    }
    if (is_terminal(response.data.status)) {
        request->second.terminal = true;
    }
    return Ack{};
}

std::optional<SessionDescriptor> SessionLifecycle::get_session(
    const std::string& sessionId) const {
    const auto snapshot = get_snapshot(sessionId);
    if (!snapshot) {
        return std::nullopt;
    }
    return snapshot->descriptor;
}

std::optional<SessionSnapshot> SessionLifecycle::get_snapshot(
    const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto found = impl_->records.find(sessionId);
    if (found == impl_->records.end()) {
        return std::nullopt;
    }
    auto& record = found->second;
    impl_->expire_if_due(record, impl_->options.steadyClock(), impl_->options.wallClock());
    return SessionSnapshot{record.descriptor, record.registration,
                           record.terminationReason, record.terminationDetail};
}

std::size_t SessionLifecycle::expire_due_sessions() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    const auto steadyNow = impl_->options.steadyClock();
    const auto wallNow = impl_->options.wallClock();
    std::size_t expired = 0;
    for (auto& [sessionId, record] : impl_->records) {
        (void)sessionId;
        const auto prior = record.descriptor.state;
        impl_->expire_if_due(record, steadyNow, wallNow);
        if (prior != SessionState::EXPIRED && record.descriptor.state == SessionState::EXPIRED) {
            ++expired;
        }
    }
    return expired;
}

std::string service_contract_name(
    const std::variant<PCVServiceType, PCAVServiceType>& serviceType) {
    if (std::holds_alternative<PCVServiceType>(serviceType)) {
        switch (std::get<PCVServiceType>(serviceType)) {
            case PCVServiceType::UNPROTECTED_LEFT_TURN_AVAILABILITY:
                return "unprotectedLeftTurnAvailability";
            case PCVServiceType::LANE_KEEPING_AID:
                return "laneKeepingAid";
            default:
                throw std::invalid_argument("unknown PCV service type");
        }
    }
    switch (std::get<PCAVServiceType>(serviceType)) {
        case PCAVServiceType::PERCEPTION_AID: return "perceptionAid";
        case PCAVServiceType::PLANNING_AID: return "planningAid";
        case PCAVServiceType::CONTROL_AID: return "controlAid";
        case PCAVServiceType::COMPUTATION_AID: return "computationAid";
        case PCAVServiceType::HDMAP_LANELET_UPDATE: return "hdMapUpdate";
        case PCAVServiceType::UNPROTECTED_LEFT_TURN_AVAILABILITY:
            return "unprotectedLeftTurnAvailability";
        case PCAVServiceType::LANE_KEEPING_AID: return "laneKeepingAid";
        default: throw std::invalid_argument("unknown PCAV service type");
    }
}

} // namespace ipi::api
