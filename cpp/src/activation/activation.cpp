#include "ipi/activation/activation.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace ipi::activation {
namespace {

constexpr double kEarthRadiusMeters = 6371008.8;
constexpr double kPi = 3.14159265358979323846;

double radians(double degrees) { return degrees * kPi / 180.0; }

double distance_meters(const api::GeoPoint& first, const api::GeoPoint& second) {
    const auto lat1 = radians(first.latitude);
    const auto lat2 = radians(second.latitude);
    const auto deltaLat = lat2 - lat1;
    const auto deltaLon = radians(second.longitude - first.longitude);
    const auto a = std::sin(deltaLat / 2.0) * std::sin(deltaLat / 2.0) +
                   std::cos(lat1) * std::cos(lat2) *
                       std::sin(deltaLon / 2.0) * std::sin(deltaLon / 2.0);
    return 2.0 * kEarthRadiusMeters * std::asin(std::min(1.0, std::sqrt(a)));
}

std::pair<double, double> local_xy(const api::GeoPoint& origin,
                                   const api::GeoPoint& point) {
    const auto meanLat = radians((origin.latitude + point.latitude) / 2.0);
    const auto x = radians(point.longitude - origin.longitude) *
                   kEarthRadiusMeters * std::cos(meanLat);
    const auto y = radians(point.latitude - origin.latitude) * kEarthRadiusMeters;
    return {x, y};
}

double distance_to_segment(double px, double py,
                           double ax, double ay,
                           double bx, double by) {
    const auto dx = bx - ax;
    const auto dy = by - ay;
    const auto lengthSquared = dx * dx + dy * dy;
    if (lengthSquared == 0.0) return std::hypot(px - ax, py - ay);
    const auto projection = std::clamp(
        ((px - ax) * dx + (py - ay) * dy) / lengthSquared, 0.0, 1.0);
    return std::hypot(px - (ax + projection * dx),
                      py - (ay + projection * dy));
}

struct FencePosition {
    bool inside{false};
    double boundaryDistanceMeters{0.0};
};

FencePosition locate(const api::Geofence& fence, const api::GeoPoint& position) {
    if (fence.type == api::GeometryType::CIRCLE) {
        if (fence.points.size() != 1 || !fence.radiusMeters || *fence.radiusMeters <= 0.0) {
            throw std::invalid_argument("circle activation zone requires one center and positive radius");
        }
        const auto distance = distance_meters(fence.points.front(), position);
        return {distance <= *fence.radiusMeters,
                std::abs(*fence.radiusMeters - distance)};
    }
    if (fence.points.size() < 3) {
        throw std::invalid_argument("polygon activation zone requires at least three points");
    }

    const auto origin = fence.points.front();
    const auto [px, py] = local_xy(origin, position);
    bool inside = false;
    double minimumDistance = std::numeric_limits<double>::infinity();
    for (std::size_t current = 0, previous = fence.points.size() - 1;
         current < fence.points.size(); previous = current++) {
        const auto [cx, cy] = local_xy(origin, fence.points[current]);
        const auto [qx, qy] = local_xy(origin, fence.points[previous]);
        if (((cy > py) != (qy > py)) &&
            (px < (qx - cx) * (py - cy) / (qy - cy) + cx)) {
            inside = !inside;
        }
        minimumDistance = std::min(minimumDistance,
            distance_to_segment(px, py, cx, cy, qx, qy));
    }
    return {inside, minimumDistance};
}

void validate_zone(const ActivationZone& zone) {
    if (zone.zoneId.empty()) throw std::invalid_argument("activation zone requires zoneId");
    if (zone.expiresAt <= zone.validFrom) {
        throw std::invalid_argument("activation zone expiration must follow validFrom");
    }
    if (zone.enterHysteresisMeters < 0.0 || zone.exitHysteresisMeters < 0.0 ||
        zone.minimumDwell < std::chrono::milliseconds::zero()) {
        throw std::invalid_argument("activation hysteresis and dwell must be non-negative");
    }
    (void)locate(zone.geofence, zone.geofence.points.front());
}

} // namespace

bool ActivationZoneTracker::set_zone(ActivationZone zone, api::Timestamp observedAt) {
    validate_zone(zone);
    if (observedAt >= zone.expiresAt) {
        throw std::invalid_argument("cannot install an expired activation zone");
    }
    if (zone_) {
        if (zone.zoneId != zone_->zoneId) {
            throw std::invalid_argument("clear the current zone before changing zoneId");
        }
        if (zone.revision < zone_->revision) {
            throw std::invalid_argument("activation zone revision is stale");
        }
        if (zone.revision == zone_->revision) return false;
    }
    zone_ = std::move(zone);
    state_ = ActivationState::OUTSIDE;
    transitionStarted_ = {};
    ++sequence_;
    return true;
}

void ActivationZoneTracker::clear_zone() {
    zone_.reset();
    state_ = ActivationState::OUTSIDE;
    transitionStarted_ = {};
    ++sequence_;
}

ActivationUpdate ActivationZoneTracker::transition(
    ActivationState next,
    ActivationReasonCode reason,
    std::chrono::steady_clock::time_point observedAt) {
    ActivationUpdate update;
    update.zoneId = zone_ ? zone_->zoneId : std::string{};
    update.revision = zone_ ? zone_->revision : 0;
    update.previousState = state_;
    update.state = next;
    update.stateChanged = next != state_;
    update.reason = reason;
    if (update.stateChanged) {
        state_ = next;
        transitionStarted_ = observedAt;
        ++sequence_;
    }
    update.sequence = sequence_;
    if (zone_ && state_ == ActivationState::ACTIVE) update.enabledActions = zone_->actions;
    return update;
}

ActivationUpdate ActivationZoneTracker::update(
    const api::GeoPoint& position,
    std::chrono::steady_clock::time_point observedAt,
    api::Timestamp wallTime,
    bool infrastructureAvailable) {
    if (!zone_) return transition(ActivationState::OUTSIDE,
                                  ActivationReasonCode::ZONE_NOT_VALID, observedAt);
    if (wallTime < zone_->validFrom || wallTime >= zone_->expiresAt) {
        const auto next = state_ == ActivationState::ACTIVE ||
                                  state_ == ActivationState::ENTERING
                              ? ActivationState::FALLBACK
                              : ActivationState::OUTSIDE;
        return transition(next, ActivationReasonCode::ZONE_NOT_VALID, observedAt);
    }
    if (!infrastructureAvailable) {
        const auto next = state_ == ActivationState::OUTSIDE
                              ? ActivationState::OUTSIDE
                              : ActivationState::FALLBACK;
        return transition(next, ActivationReasonCode::INFRASTRUCTURE_UNAVAILABLE,
                          observedAt);
    }

    const auto fence = locate(zone_->geofence, position);
    const bool entryQualified = fence.inside &&
                                fence.boundaryDistanceMeters >= zone_->enterHysteresisMeters;
    const bool exitQualified = !fence.inside &&
                               fence.boundaryDistanceMeters >= zone_->exitHysteresisMeters;
    const auto dwellElapsed = transitionStarted_ == std::chrono::steady_clock::time_point{} ||
                              observedAt - transitionStarted_ >= zone_->minimumDwell;

    switch (state_) {
        case ActivationState::OUTSIDE:
            if (entryQualified) {
                return transition(ActivationState::ENTERING,
                                  ActivationReasonCode::ENTRY_CANDIDATE, observedAt);
            }
            break;
        case ActivationState::ENTERING:
            if (!entryQualified) {
                return transition(ActivationState::OUTSIDE,
                                  ActivationReasonCode::ENTRY_CANCELLED, observedAt);
            }
            if (dwellElapsed) {
                return transition(ActivationState::ACTIVE,
                                  ActivationReasonCode::ENTRY_CONFIRMED, observedAt);
            }
            break;
        case ActivationState::ACTIVE:
            if (exitQualified) {
                return transition(ActivationState::EXITING,
                                  ActivationReasonCode::EXIT_CANDIDATE, observedAt);
            }
            break;
        case ActivationState::EXITING:
            if (!exitQualified) {
                return transition(ActivationState::ACTIVE,
                                  ActivationReasonCode::EXIT_CANCELLED, observedAt);
            }
            if (dwellElapsed) {
                return transition(ActivationState::OUTSIDE,
                                  ActivationReasonCode::EXIT_CONFIRMED, observedAt);
            }
            break;
        case ActivationState::FALLBACK:
            if (entryQualified) {
                return transition(ActivationState::ENTERING,
                                  ActivationReasonCode::RECOVERED, observedAt);
            }
            return transition(ActivationState::OUTSIDE,
                              ActivationReasonCode::RECOVERED, observedAt);
        default:
            throw std::logic_error("unknown activation state");
    }
    return transition(state_, ActivationReasonCode::NONE, observedAt);
}

std::string to_string(ActivationState state) {
    switch (state) {
        case ActivationState::OUTSIDE: return "outside";
        case ActivationState::ENTERING: return "entering";
        case ActivationState::ACTIVE: return "active";
        case ActivationState::EXITING: return "exiting";
        case ActivationState::FALLBACK: return "fallback";
        default: return "unknown";
    }
}

std::string to_string(ActivationReasonCode reason) {
    switch (reason) {
        case ActivationReasonCode::NONE: return "none";
        case ActivationReasonCode::ZONE_INSTALLED: return "zone-installed";
        case ActivationReasonCode::ENTRY_CANDIDATE: return "entry-candidate";
        case ActivationReasonCode::ENTRY_CONFIRMED: return "entry-confirmed";
        case ActivationReasonCode::ENTRY_CANCELLED: return "entry-cancelled";
        case ActivationReasonCode::EXIT_CANDIDATE: return "exit-candidate";
        case ActivationReasonCode::EXIT_CONFIRMED: return "exit-confirmed";
        case ActivationReasonCode::EXIT_CANCELLED: return "exit-cancelled";
        case ActivationReasonCode::INFRASTRUCTURE_UNAVAILABLE: return "infrastructure-unavailable";
        case ActivationReasonCode::ZONE_NOT_VALID: return "zone-not-valid";
        case ActivationReasonCode::RECOVERED: return "recovered";
        default: return "unknown";
    }
}

} // namespace ipi::activation
