#pragma once

#include "ipi/api/types.hpp"
#include "ipi/core/ipi_cooperative_service.hpp"
#include "ipi/v2x/j2735_messages.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <vector>

namespace ipi::mesh {

using Timestamp = api::Timestamp;

struct MeshConfig {
    std::chrono::seconds infrastructureTimeout{std::chrono::seconds(5)};
    std::chrono::seconds neighborTimeout{std::chrono::seconds(5)};
    std::chrono::seconds broadcastInterval{std::chrono::seconds(1)};
    double maxNeighborDistanceMeters{250.0};
};

struct NeighborState {
    std::uint32_t vehicleId{0};
    api::GeoPoint position{};
    std::optional<double> speedMps{};
    std::optional<double> headingDeg{};
    Timestamp lastSeen{};
    bool meshCapable{false};
    std::optional<double> meshRadiusMeters{};
};

class MeshLink {
public:
    virtual ~MeshLink() = default;

    /**
     * Broadcast a cooperative service message over the underlying mesh transport
     * (C‑V2X sidelink, DSRC, Wi‑Fi Direct, etc.).
     */
    virtual void broadcast(const CooperativeServiceMessage& message) = 0;
};

using CooperativeMessageBuilder = std::function<CooperativeServiceMessage(
    const api::VehicleTelemetryFrame& self,
    const std::vector<NeighborState>& neighbors)>;

/**
 * MeshManager implements the Vehicle–Vehicle Local Mesh Mode described in
 * cpp/LOGIC.md. It tracks nearby vehicles from incoming BSMs, monitors
 * infrastructure health, and decides when to emit cooperative guidance frames
 * over a MeshLink during power‑outage or infrastructure‑loss scenarios.
 */
class MeshManager {
public:
    MeshManager(MeshConfig config,
                MeshLink* link,
                SessionId sessionId,
                std::vector<std::uint8_t> vehicleId);

    void set_cooperative_builder(CooperativeMessageBuilder builder);

    /**
     * Mark that upstream infrastructure (SPaT/MAP/5G backhaul) was observed as
     * healthy at the given time. When this heartbeat stops for longer than
     * infrastructureTimeout, the manager will enter mesh mode.
     */
    void update_infrastructure_heartbeat(Timestamp now);

    /**
     * Update the local vehicle telemetry used for range calculations and to
     * seed outgoing cooperative guidance frames.
     */
    void update_self_telemetry(const api::VehicleTelemetryFrame& telemetry);

    /**
     * Ingest a decoded BSM from a neighboring vehicle. The caller is
     * responsible for interpreting any J2735 regional extensions that encode
     * mesh capability and radius.
     *
     * @param bsm           Decoded BasicSafetyMessage from a neighbor.
     * @param receptionTime Local reception timestamp.
     * @param meshCapable   Whether the sender advertises mesh capability.
     * @param meshRadiusMeters Optional mesh radius advertised by the sender.
     */
    void on_bsm(const j2735::BasicSafetyMessage& bsm,
                Timestamp receptionTime,
                bool meshCapable,
                std::optional<double> meshRadiusMeters = {});

    /**
     * Periodic tick that drives neighbor pruning, mesh‑mode transitions, and
     * emission of cooperative service frames.
     */
    void tick(Timestamp now);

    [[nodiscard]] bool mesh_mode_active() const;

    [[nodiscard]] std::vector<NeighborState> neighbors() const;

private:
    MeshConfig config_;
    MeshLink* link_{nullptr};
    SessionId sessionId_{};
    std::vector<std::uint8_t> vehicleId_;
    CooperativeMessageBuilder builder_{};
    std::optional<api::VehicleTelemetryFrame> selfTelemetry_{};
    std::map<std::uint32_t, NeighborState> neighbors_{};
    std::optional<Timestamp> lastInfrastructureHeartbeat_{};
    std::optional<Timestamp> lastBroadcastTime_{};
    bool meshModeActive_{false};

    [[nodiscard]] bool infrastructure_healthy(Timestamp now) const;
    void prune_neighbors(Timestamp now);
    [[nodiscard]] std::vector<NeighborState> snapshot_neighbors() const;
    CooperativeServiceMessage build_default_message(const api::VehicleTelemetryFrame& self,
                                                    const std::vector<NeighborState>& neighbors) const;
};

} // namespace ipi::mesh

