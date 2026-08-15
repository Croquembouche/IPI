#pragma once

#include "ipi/api/types.hpp"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ipi::activation {

enum class ActivationAction : std::uint8_t {
    MODEL_PULL = 0,
    TELEMETRY_PUSH = 1,
    ENABLE_EDGE_PERCEPTION = 2,
    ENABLE_EDGE_PLANNING = 3,
    SERVICE_MODE_HINT = 4
};

enum class ActivationState : std::uint8_t {
    OUTSIDE = 0,
    ENTERING = 1,
    ACTIVE = 2,
    EXITING = 3,
    FALLBACK = 4
};

enum class ActivationReasonCode : std::uint8_t {
    NONE = 0,
    ZONE_INSTALLED = 1,
    ENTRY_CANDIDATE = 2,
    ENTRY_CONFIRMED = 3,
    ENTRY_CANCELLED = 4,
    EXIT_CANDIDATE = 5,
    EXIT_CONFIRMED = 6,
    EXIT_CANCELLED = 7,
    INFRASTRUCTURE_UNAVAILABLE = 8,
    ZONE_NOT_VALID = 9,
    RECOVERED = 10
};

struct ActivationZone {
    std::string zoneId{};
    std::uint64_t revision{0};
    api::Geofence geofence{};
    api::Timestamp validFrom{};
    api::Timestamp expiresAt{};
    double enterHysteresisMeters{0.0};
    double exitHysteresisMeters{0.0};
    std::chrono::milliseconds minimumDwell{0};
    std::vector<ActivationAction> actions{};
};

struct ActivationUpdate {
    std::string zoneId{};
    std::uint64_t revision{0};
    ActivationState previousState{ActivationState::OUTSIDE};
    ActivationState state{ActivationState::OUTSIDE};
    std::uint64_t sequence{0};
    bool stateChanged{false};
    ActivationReasonCode reason{ActivationReasonCode::NONE};
    std::vector<ActivationAction> enabledActions{};
};

class ActivationZoneTracker {
public:
    /** Installs a newer zone revision. Same revision is idempotent. */
    bool set_zone(ActivationZone zone, api::Timestamp observedAt);
    void clear_zone();

    [[nodiscard]] ActivationUpdate update(
        const api::GeoPoint& position,
        std::chrono::steady_clock::time_point observedAt,
        api::Timestamp wallTime,
        bool infrastructureAvailable = true);

    [[nodiscard]] ActivationState state() const noexcept { return state_; }
    [[nodiscard]] const std::optional<ActivationZone>& zone() const noexcept { return zone_; }

private:
    std::optional<ActivationZone> zone_{};
    ActivationState state_{ActivationState::OUTSIDE};
    std::chrono::steady_clock::time_point transitionStarted_{};
    std::uint64_t sequence_{0};

    [[nodiscard]] ActivationUpdate transition(
        ActivationState next,
        ActivationReasonCode reason,
        std::chrono::steady_clock::time_point observedAt);
};

[[nodiscard]] std::string to_string(ActivationState state);
[[nodiscard]] std::string to_string(ActivationReasonCode reason);

} // namespace ipi::activation
