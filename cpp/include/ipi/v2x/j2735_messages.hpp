#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ipi::j2735 {

struct BasicSafetyMessage {
    std::uint32_t vehicleId{0};
    double latitude{0.0};
    double longitude{0.0};
    float speedMps{0.0F};
    float headingDeg{0.0F};
    std::optional<float> accelerationMps2{};
    std::optional<std::uint16_t> laneId{};

    void validate() const;
    [[nodiscard]] std::vector<std::uint8_t> to_bytes() const;
    static BasicSafetyMessage from_bytes(const std::vector<std::uint8_t>& buffer);
    [[nodiscard]] std::string to_string() const;
};

struct MapLane {
    std::uint16_t laneId{0};
    bool ingress{true};
};

struct MapMessage {
    std::uint16_t intersectionId{0};
    std::uint8_t revision{0};
    std::optional<std::string> name{};
    std::vector<MapLane> lanes{};

    void validate() const;
    [[nodiscard]] std::vector<std::uint8_t> to_bytes() const;
    static MapMessage from_bytes(const std::vector<std::uint8_t>& buffer);
    [[nodiscard]] std::string to_string() const;
};

enum class MovementPhaseState : std::uint8_t {
    Dark = 0,
    StopAndRemain = 1,
    StopThenProceed = 2,
    Proceed = 3,
    Flashing = 4
};

struct SpatPhaseState {
    std::uint8_t signalGroup{0};
    MovementPhaseState state{MovementPhaseState::Dark};
    std::optional<std::uint16_t> timeToChangeMs{};
};

struct SpatMessage {
    std::uint16_t intersectionId{0};
    std::uint32_t timestampMs{0};
    std::vector<SpatPhaseState> phases{};

    void validate() const;
    [[nodiscard]] std::vector<std::uint8_t> to_bytes() const;
    static SpatMessage from_bytes(const std::vector<std::uint8_t>& buffer);
    [[nodiscard]] std::string to_string() const;
};

struct SignalRequestMessage {
    std::uint16_t requestId{0};
    std::uint32_t vehicleId{0};
    std::uint16_t intersectionId{0};
    std::uint8_t requestedSignalGroup{0};
    std::optional<std::uint32_t> estimatedArrivalMs{};
    std::optional<std::uint8_t> priorityLevel{}; ///< 0 normal, 1 high, etc.

    void validate() const;
    [[nodiscard]] std::vector<std::uint8_t> to_bytes() const;
    static SignalRequestMessage from_bytes(const std::vector<std::uint8_t>& buffer);
    [[nodiscard]] std::string to_string() const;
};

struct SignalStatusMessage {
    std::uint16_t requestId{0};
    std::uint16_t intersectionId{0};
    std::uint8_t grantedSignalGroup{0};
    bool granted{false};
    std::optional<std::uint16_t> estimatedServedTimeMs{};

    void validate() const;
    [[nodiscard]] std::vector<std::uint8_t> to_bytes() const;
    static SignalStatusMessage from_bytes(const std::vector<std::uint8_t>& buffer);
    [[nodiscard]] std::string to_string() const;
};

[[nodiscard]] std::string to_string(MovementPhaseState state);

} // namespace ipi::j2735
