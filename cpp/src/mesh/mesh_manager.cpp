#include "ipi/mesh/mesh_manager.hpp"

#include <algorithm>
#include <cmath>

// Anonymous namespace for internal helper functions that are not part of the
// public ipi::mesh API.
namespace {

// Approximate radius of the Earth, used by the haversine distance function.
constexpr double kEarthRadiusMeters = 6371000.0;
constexpr double kPi = 3.14159265358979323846;

// Convert degrees to radians.
double to_radians(double degrees) {
    return degrees * kPi / 180.0;
}

// Compute great‑circle distance between two latitude/longitude points using
// the haversine formula. This is used to filter neighbors that are outside
// the configured mesh radius.
double haversine_distance_meters(const ipi::api::GeoPoint& a, const ipi::api::GeoPoint& b) {
    const double lat1 = to_radians(a.latitude);
    const double lat2 = to_radians(b.latitude);
    const double dLat = lat2 - lat1;
    const double dLon = to_radians(b.longitude - a.longitude);

    const double sinLat = std::sin(dLat / 2.0);
    const double sinLon = std::sin(dLon / 2.0);
    const double h = sinLat * sinLat + std::cos(lat1) * std::cos(lat2) * sinLon * sinLon;
    const double c = 2.0 * std::atan2(std::sqrt(h), std::sqrt(1.0 - h));
    return kEarthRadiusMeters * c;
}

// Map the lightweight J2735 BSM representation to the API‑level GeoPoint
// type used throughout the mesh module.
ipi::api::GeoPoint to_geo_point(const ipi::j2735::BasicSafetyMessage& bsm) {
    ipi::api::GeoPoint point{};
    point.latitude = bsm.latitude;
    point.longitude = bsm.longitude;
    // Elevation is left unset unless it is explicitly available.
    point.elevation.reset();
    return point;
}

} // namespace

namespace ipi::mesh {

// Construct a MeshManager with the given configuration, underlying mesh link,
// and identifiers for this vehicle/session. The link pointer is not owned.
MeshManager::MeshManager(MeshConfig config,
                         MeshLink* link,
                         SessionId sessionId,
                         std::vector<std::uint8_t> vehicleId)
    : config_(config),
      link_(link),
      sessionId_(sessionId),
      vehicleId_(std::move(vehicleId)) {}

// Optional callback that lets callers customise the CooperativeServiceMessage
// payload built for each broadcast tick.
void MeshManager::set_cooperative_builder(CooperativeMessageBuilder builder) {
    builder_ = std::move(builder);
}

// Record that infrastructure (SPaT/MAP/5G, etc.) was observed as healthy at
// the given time. Loss of this heartbeat causes the manager to enter mesh mode.
void MeshManager::update_infrastructure_heartbeat(Timestamp now) {
    lastInfrastructureHeartbeat_ = now;
}

// Update the local vehicle telemetry. This is used for:
//   - distance calculations to neighbors, and
//   - seeding the default cooperative perception payload.
void MeshManager::update_self_telemetry(const api::VehicleTelemetryFrame& telemetry) {
    selfTelemetry_ = telemetry;
}

// Ingest a decoded BSM from a neighbor and update (or create) its entry in the
// neighbor table. The caller decides whether the sender is mesh‑capable and
// what mesh radius it advertises.
void MeshManager::on_bsm(const j2735::BasicSafetyMessage& bsm,
                         Timestamp receptionTime,
                         bool meshCapable,
                         std::optional<double> meshRadiusMeters) {
    // Insert or find the neighbor entry keyed by vehicleId.
    NeighborState& neighbor = neighbors_[bsm.vehicleId];
    neighbor.vehicleId = bsm.vehicleId;
    neighbor.position = to_geo_point(bsm);
    neighbor.speedMps = bsm.speedMps;
    neighbor.headingDeg = bsm.headingDeg;
    neighbor.lastSeen = receptionTime;
    neighbor.meshCapable = meshCapable;
    neighbor.meshRadiusMeters = meshRadiusMeters;
}

// Determine whether infrastructure is still considered healthy based on the
// time since the last heartbeat.
bool MeshManager::infrastructure_healthy(Timestamp now) const {
    if (!lastInfrastructureHeartbeat_) {
        return false;
    }
    const auto delta = std::chrono::duration_cast<std::chrono::seconds>(now - *lastInfrastructureHeartbeat_);
    return delta <= config_.infrastructureTimeout;
}

// Remove neighbors that have not been heard from within the configured timeout.
void MeshManager::prune_neighbors(Timestamp now) {
    for (auto it = neighbors_.begin(); it != neighbors_.end();) {
        const auto delta = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastSeen);
        if (delta > config_.neighborTimeout) {
            it = neighbors_.erase(it);
        } else {
            ++it;
        }
    }
}

// Take a snapshot of the current neighbors and, if self telemetry and a
// maximum distance are available, filter out neighbors that are too far away.
std::vector<NeighborState> MeshManager::snapshot_neighbors() const {
    std::vector<NeighborState> snapshot;
    snapshot.reserve(neighbors_.size());
    for (const auto& [id, state] : neighbors_) {
        snapshot.push_back(state);
    }

    if (selfTelemetry_ && config_.maxNeighborDistanceMeters > 0.0) {
        const auto selfPos = selfTelemetry_->pose;
        snapshot.erase(
            std::remove_if(snapshot.begin(), snapshot.end(),
                           [&](const NeighborState& n) {
                               const double distance = haversine_distance_meters(selfPos, n.position);
                               return distance > config_.maxNeighborDistanceMeters;
                           }),
            snapshot.end());
    }

    return snapshot;
}

// Build a minimal CooperativeServiceMessage when the caller has not installed
// a custom builder. This encodes the ego vehicle as a single detected object
// in a GuidedPerception payload so peers can localise it.
CooperativeServiceMessage MeshManager::build_default_message(
    const api::VehicleTelemetryFrame& self,
    const std::vector<NeighborState>& /*neighbors*/) const {
    CooperativeServiceMessage msg;
    msg.sessionId = sessionId_;
    msg.vehicleId = vehicleId_;
    msg.serviceClass = ServiceClass::GuidedPerception;
    msg.guidanceStatus = GuidanceStatus::Update;

    GuidedPerceptionPayload perception{};
    perception.detectedObjects.reserve(1);

    DetectedObject selfObject{};
    selfObject.classification = DetectedObject::Classification::Vehicle;
    selfObject.position.latitude = self.pose.latitude;
    selfObject.position.longitude = self.pose.longitude;
    selfObject.position.elevation = self.pose.elevation;
    if (self.speedMps) {
        selfObject.velocityMps = self.speedMps;
    }

    perception.detectedObjects.push_back(std::move(selfObject));
    msg.perception = std::move(perception);
    return msg;
}

// Main periodic entry point. Callers should invoke tick() on a timer. It:
//   1. prunes stale neighbors,
//   2. updates mesh mode based on infrastructure health,
//   3. and, when in mesh mode, broadcasts cooperative frames at the configured
//      interval over the MeshLink.
void MeshManager::tick(Timestamp now) {
    prune_neighbors(now);

    const bool infraOk = infrastructure_healthy(now);
    meshModeActive_ = !infraOk;

    // If infrastructure is still healthy, there is no mesh link, or we do not
    // know our own pose, skip sending any cooperative frames.
    if (!meshModeActive_ || !link_ || !selfTelemetry_) {
        return;
    }

    // Enforce a minimum interval between broadcasts.
    if (lastBroadcastTime_) {
        const auto delta = std::chrono::duration_cast<std::chrono::seconds>(now - *lastBroadcastTime_);
        if (delta < config_.broadcastInterval) {
            return;
        }
    }

    const auto neighborsSnapshot = snapshot_neighbors();
    const auto& self = *selfTelemetry_;

    CooperativeServiceMessage message =
        builder_ ? builder_(self, neighborsSnapshot) : build_default_message(self, neighborsSnapshot);

    // Hand the message to the underlying transport implementation.
    link_->broadcast(message);
    lastBroadcastTime_ = now;
}

// Expose whether the manager currently considers itself to be in mesh mode.
bool MeshManager::mesh_mode_active() const {
    return meshModeActive_;
}

// Return the current filtered view of mesh neighbors.
std::vector<NeighborState> MeshManager::neighbors() const {
    return snapshot_neighbors();
}

} // namespace ipi::mesh
