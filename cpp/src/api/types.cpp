#include "ipi/api/types.hpp"

#include <stdexcept>

namespace ipi::api {

std::string to_string(SessionState state) {
    switch (state) {
        case SessionState::REGISTERED: return "registered";
        case SessionState::ACTIVE: return "active";
        case SessionState::SUSPENDED: return "suspended";
        case SessionState::TERMINATED: return "terminated";
        case SessionState::EXPIRED: return "expired";
        default: return "unknown";
    }
}

std::string to_string(FailureCode code) {
    switch (code) {
        case FailureCode::NONE: return "none";
        case FailureCode::INVALID_REQUEST: return "invalid-request";
        case FailureCode::UNKNOWN_SESSION: return "unknown-session";
        case FailureCode::SESSION_INACTIVE: return "session-inactive";
        case FailureCode::SESSION_TERMINATED: return "session-terminated";
        case FailureCode::SESSION_EXPIRED: return "session-expired";
        case FailureCode::TIMEOUT: return "timeout";
        case FailureCode::MISSING_RESPONSE: return "missing-response";
        case FailureCode::TRANSPORT_INTERRUPTION: return "transport-interruption";
        case FailureCode::SERVICE_UNAVAILABLE: return "service-unavailable";
        case FailureCode::STALE_REQUEST: return "stale-request";
        case FailureCode::STALE_RESPONSE: return "stale-response";
        case FailureCode::DUPLICATE_RESPONSE: return "duplicate-response";
        case FailureCode::CORRELATION_MISMATCH: return "correlation-mismatch";
        case FailureCode::ACCESS_DENIED: return "access-denied";
        case FailureCode::PROTOCOL_ERROR: return "protocol-error";
        case FailureCode::FALLBACK_FAILED: return "fallback-failed";
        default: return "unknown";
    }
}

std::string to_string(TerminationReasonCode code) {
    switch (code) {
        case TerminationReasonCode::CLIENT_REQUEST: return "client-request";
        case TerminationReasonCode::LEASE_EXPIRED: return "lease-expired";
        case TerminationReasonCode::HEARTBEAT_TIMEOUT: return "heartbeat-timeout";
        case TerminationReasonCode::TRANSPORT_INTERRUPTION: return "transport-interruption";
        case TerminationReasonCode::SERVICE_UNAVAILABLE: return "service-unavailable";
        case TerminationReasonCode::ADMINISTRATIVE: return "administrative";
        case TerminationReasonCode::PROTOCOL_ERROR: return "protocol-error";
        default: return "unknown";
    }
}

std::string to_string(FallbackResult result) {
    switch (result) {
        case FallbackResult::NOT_ATTEMPTED: return "not-attempted";
        case FallbackResult::ACTIVATED: return "activated";
        case FallbackResult::SUCCEEDED: return "succeeded";
        case FallbackResult::FAILED: return "failed";
        default: return "unknown";
    }
}

std::string to_string(VehicleServiceStatus status) {
    switch (status) {
        case VehicleServiceStatus::ACCEPTED: return "accepted";
        case VehicleServiceStatus::REJECTED: return "rejected";
        case VehicleServiceStatus::IN_PROGRESS: return "in-progress";
        case VehicleServiceStatus::COMPLETED: return "completed";
        case VehicleServiceStatus::TIMED_OUT: return "timed-out";
        case VehicleServiceStatus::MISSING_RESPONSE: return "missing-response";
        case VehicleServiceStatus::TRANSPORT_INTERRUPTED: return "transport-interrupted";
        case VehicleServiceStatus::SERVICE_UNAVAILABLE: return "service-unavailable";
        case VehicleServiceStatus::EXPIRED: return "expired";
        case VehicleServiceStatus::FALLBACK_COMPLETED: return "fallback-completed";
        default: return "unknown";
    }
}

bool is_terminal(VehicleServiceStatus status) noexcept {
    return status != VehicleServiceStatus::ACCEPTED &&
           status != VehicleServiceStatus::IN_PROGRESS;
}

void validate_vehicle_service_response(const VehicleServiceResponse& response) {
    if (response.vehicleId.empty()) {
        throw std::invalid_argument("service response requires a vehicleId");
    }
    if (response.resultId.empty()) {
        throw std::invalid_argument("service response requires a resultId");
    }

    auto require_code = [&](FailureCode expected) {
        if (response.failureCode != expected) {
            throw std::invalid_argument(
                "service status " + to_string(response.status) +
                " requires failure code " + to_string(expected));
        }
    };

    switch (response.status) {
        case VehicleServiceStatus::ACCEPTED:
        case VehicleServiceStatus::IN_PROGRESS:
        case VehicleServiceStatus::COMPLETED:
            require_code(FailureCode::NONE);
            if (response.fallbackResult) {
                throw std::invalid_argument(
                    "ordinary service progress cannot carry a fallback result");
            }
            break;
        case VehicleServiceStatus::REJECTED:
            if (response.failureCode == FailureCode::NONE) {
                throw std::invalid_argument("rejected service response requires a failure code");
            }
            break;
        case VehicleServiceStatus::TIMED_OUT:
            require_code(FailureCode::TIMEOUT);
            break;
        case VehicleServiceStatus::MISSING_RESPONSE:
            require_code(FailureCode::MISSING_RESPONSE);
            break;
        case VehicleServiceStatus::TRANSPORT_INTERRUPTED:
            require_code(FailureCode::TRANSPORT_INTERRUPTION);
            break;
        case VehicleServiceStatus::SERVICE_UNAVAILABLE:
            require_code(FailureCode::SERVICE_UNAVAILABLE);
            break;
        case VehicleServiceStatus::EXPIRED:
            if (response.failureCode != FailureCode::STALE_RESPONSE &&
                response.failureCode != FailureCode::SESSION_EXPIRED) {
                throw std::invalid_argument(
                    "expired response requires stale-response or session-expired code");
            }
            break;
        case VehicleServiceStatus::FALLBACK_COMPLETED:
            if (!response.fallbackResult ||
                (*response.fallbackResult != FallbackResult::SUCCEEDED &&
                 *response.fallbackResult != FallbackResult::FAILED)) {
                throw std::invalid_argument(
                    "fallback completion requires succeeded or failed fallback result");
            }
            if (*response.fallbackResult == FallbackResult::SUCCEEDED) {
                require_code(FailureCode::NONE);
            } else {
                require_code(FailureCode::FALLBACK_FAILED);
            }
            break;
        default:
            throw std::invalid_argument("unknown vehicle service status");
    }
}

} // namespace ipi::api
