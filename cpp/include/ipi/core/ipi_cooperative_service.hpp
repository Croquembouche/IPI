#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ipi {

using SessionId = std::array<std::uint8_t, 16>;
using ObjectId = std::array<std::uint8_t, 4>;

struct Position3D {
    double latitude{};   ///< Degrees
    double longitude{};  ///< Degrees
    std::optional<double> elevation{}; ///< Meters (optional)
};

struct Waypoint {
    Position3D position{};
    std::optional<double> targetSpeedMps{};
    std::optional<std::uint16_t> dwellTimeMs{};
};

struct DetectedObject {
    enum class Classification : std::uint8_t {
        Vehicle = 0,
        Pedestrian = 1,
        Cyclist = 2,
        Obstacle = 3
    };

    ObjectId objectId{};
    Classification classification{Classification::Obstacle};
    Position3D position{};
    std::optional<double> velocityMps{};
    std::vector<std::uint8_t> covariance{}; ///< Optional covariance matrix bytes.
};

struct ControlCommand {
    enum class Axis : std::uint8_t {
        Steering = 0,
        Throttle = 1,
        Brake = 2
    };

    Axis axis{Axis::Steering};
    std::int16_t valuePermille{}; ///< -1000..1000 representing -100%..100%.
};

struct GuidedPlanningPayload {
    std::vector<Waypoint> waypoints{};
    bool fallbackRoute{false};
};

struct GuidedPerceptionPayload {
    std::vector<DetectedObject> detectedObjects{};
};

struct GuidedControlPayload {
    std::vector<ControlCommand> commands{};
};

enum class ServiceClass : std::uint8_t {
    GuidedPlanning = 0,
    GuidedPerception = 1,
    GuidedControl = 2
};

enum class GuidanceStatus : std::uint8_t {
    Request = 0,
    Update = 1,
    Complete = 2,
    Reject = 3
};

struct CooperativeServiceMessage {
    SessionId sessionId{};
    std::vector<std::uint8_t> vehicleId{}; ///< Raw VehicleReferenceID bytes (4 or 8 bytes per J2735).
    ServiceClass serviceClass{ServiceClass::GuidedPlanning};
    GuidanceStatus guidanceStatus{GuidanceStatus::Request};
    std::optional<std::uint16_t> requestedHorizonMs{};
    std::optional<GuidedPlanningPayload> planning{};
    std::optional<GuidedPerceptionPayload> perception{};
    std::optional<GuidedControlPayload> control{};
    std::optional<std::vector<std::uint8_t>> offloadPayload{};
    std::optional<std::string> offloadTaskId{};
    std::optional<std::uint8_t> confidence{}; ///< 0..100 percent.
    std::optional<std::uint32_t> expirationTimeDs{}; ///< Tenths of a second since epoch (J2735 DTime).

    void validate() const;

    /**
     * Serialises the cooperative message to a canonical byte encoding.
     * Format (big-endian multi-byte fields):
     * [sessionId:16][vehicleIdLen:1][vehicleId:N][serviceClass:1][status:1]
     * [flags:1][requestedHorizon?:2][confidence?:1][expirationTime?:4]
     * [planning?][perception?][control?]
     *
     * Optional payload sections are prefixed with their own length (uint16).
     */
    [[nodiscard]] std::vector<std::uint8_t> to_canonical_encoding() const;

    /**
     * Deserialises a canonical buffer produced by to_canonical_encoding().
     */
    static CooperativeServiceMessage from_canonical_encoding(const std::vector<std::uint8_t>& buffer);

    [[nodiscard]] std::string to_string() const;
};

[[nodiscard]] std::string to_string(ServiceClass serviceClass);
[[nodiscard]] std::string to_string(GuidanceStatus status);
[[nodiscard]] std::string to_string(DetectedObject::Classification classification);
[[nodiscard]] std::string to_string(ControlCommand::Axis axis);

} // namespace ipi
