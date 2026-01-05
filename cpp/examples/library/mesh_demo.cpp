#include "ipi/mesh/mesh_manager.hpp"
#include "ipi/mesh/task_offloader.hpp"

#include "ipi/api/types.hpp"
#include "ipi/v2x/j2735_messages.hpp"

#include <chrono>
#include <iostream>

using ipi::SessionId;
using ipi::api::GeoPoint;
using ipi::api::Timestamp;
using ipi::api::VehicleTelemetryFrame;
using ipi::j2735::BasicSafetyMessage;
using ipi::mesh::MeshConfig;
using ipi::mesh::MeshLink;
using ipi::mesh::MeshManager;
using ipi::mesh::OffloadProgress;
using ipi::mesh::OffloadStatus;
using ipi::mesh::OffloadTask;
using ipi::mesh::TaskOffloader;

namespace {

class LoggingMeshLink : public MeshLink {
public:
    void broadcast(const ipi::CooperativeServiceMessage& message) override {
        std::cout << "[mesh] broadcast cooperative frame: " << message.to_string() << '\n';
    }
};

Timestamp now() {
    return std::chrono::system_clock::now();
}

std::string to_string(OffloadStatus status) {
    switch (status) {
        case OffloadStatus::Pending:
            return "pending";
        case OffloadStatus::Accepted:
            return "accepted";
        case OffloadStatus::Completed:
            return "completed";
        case OffloadStatus::Rejected:
            return "rejected";
        default:
            return "unknown";
    }
}

} // namespace

int main() {
    LoggingMeshLink link;

    MeshConfig config;
    config.infrastructureTimeout = std::chrono::seconds(3);
    config.neighborTimeout = std::chrono::seconds(30);
    config.broadcastInterval = std::chrono::seconds(1);
    config.maxNeighborDistanceMeters = 250.0;

    SessionId sessionId{};
    sessionId[0] = 0x12;
    sessionId[1] = 0x34;

    std::vector<std::uint8_t> vehicleId = {0x01, 0x02, 0x03, 0x04};

    MeshManager manager(config, &link, sessionId, vehicleId);
    TaskOffloader offloader(sessionId, vehicleId, [&](const ipi::CooperativeServiceMessage& message) {
        link.broadcast(message);
    });
    offloader.set_progress_callback([](const OffloadProgress& progress) {
        std::cout << "[offload] task=" << progress.taskId
                  << " status=" << to_string(progress.status) << '\n';
    });

    auto t0 = now();

    VehicleTelemetryFrame self{};
    self.timestamp = t0;
    self.pose = GeoPoint{37.3349, -122.0090, std::nullopt};
    self.speedMps = 5.0;
    manager.update_self_telemetry(self);

    manager.update_infrastructure_heartbeat(t0);
    manager.tick(t0);
    std::cout << "mesh mode active (expected false): "
              << std::boolalpha << manager.mesh_mode_active() << '\n';

    BasicSafetyMessage neighbor{};
    neighbor.vehicleId = 0xDEADBEEF;
    neighbor.latitude = 37.3350;
    neighbor.longitude = -122.0095;
    neighbor.speedMps = 4.0F;
    neighbor.headingDeg = 90.0F;

    manager.on_bsm(neighbor, t0, true, 200.0);

    auto tMesh = t0 + std::chrono::seconds(5);
    manager.tick(tMesh);

    std::cout << "mesh mode active (expected true): "
              << std::boolalpha << manager.mesh_mode_active() << '\n';

    auto neighbors = manager.neighbors();
    std::cout << "neighbor count: " << neighbors.size() << '\n';

    auto tBroadcast = tMesh + std::chrono::seconds(1);
    manager.tick(tBroadcast);

    OffloadTask task;
    task.taskId = "reprocess-perception";
    task.serviceClass = ipi::ServiceClass::GuidedPerception;
    task.payload = {0xAA, 0xBB, 0xCC};
    task.desiredHorizon = std::chrono::milliseconds(500);
    offloader.request_offload(task);

    ipi::CooperativeServiceMessage accepted{};
    accepted.sessionId = sessionId;
    accepted.vehicleId = vehicleId;
    accepted.serviceClass = ipi::ServiceClass::GuidedPerception;
    accepted.guidanceStatus = ipi::GuidanceStatus::Update;
    accepted.offloadTaskId = task.taskId;
    offloader.handle_cooperative_message(accepted);

    ipi::CooperativeServiceMessage completed = accepted;
    completed.guidanceStatus = ipi::GuidanceStatus::Complete;
    completed.offloadPayload = std::vector<std::uint8_t>{0x01, 0x02};
    offloader.handle_cooperative_message(completed);

    return 0;
}
