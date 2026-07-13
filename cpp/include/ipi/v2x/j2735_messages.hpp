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

enum class PersonalDeviceUserType : std::uint8_t {
    Unavailable = 0,
    Pedestrian = 1,
    PedalCyclist = 2,
    PublicSafetyWorker = 3,
    Animal = 4
};

enum class PersonalPropulsionKind : std::uint8_t {
    Unavailable = 0,
    Human = 1,
    Animal = 2,
    Motor = 3
};

struct PersonalAccelerationSet {
    float longitudinalMps2{0.0F};
    float lateralMps2{0.0F};
    float verticalMps2{0.0F};
    float yawRateDegPerSec{0.0F};
};

struct PersonalPathHistoryPoint {
    std::int16_t latitudeOffset{0};
    std::int16_t longitudeOffset{0};
    std::int16_t elevationOffset{0};
    std::uint16_t timeOffsetMs{0};
};

struct PersonalPathPrediction {
    std::int16_t radiusOfCurveM{0};
    std::uint8_t confidence{0};
};

struct PersonalPropelledInformation {
    PersonalPropulsionKind kind{PersonalPropulsionKind::Unavailable};
    std::uint8_t subtype{0};
};

/**
 * Lightweight SAE J2735 Personal Safety Message profile.
 *
 * The required fields cover phone-originated vulnerable-road-user telemetry.
 * Optional accuracy, acceleration, path, prediction, and propulsion fields
 * preserve the information needed for trajectory and collision analysis.
 */
struct PersonalSafetyMessage {
    PersonalDeviceUserType basicType{PersonalDeviceUserType::Unavailable};
    std::uint16_t secondMarkMs{0};
    std::uint8_t messageCount{0};
    std::uint32_t temporaryId{0};
    double latitude{0.0};
    double longitude{0.0};
    std::optional<float> elevationM{};
    std::optional<float> horizontalAccuracyM{};
    float speedMps{0.0F};
    float headingDeg{0.0F};
    std::optional<PersonalAccelerationSet> acceleration{};
    std::vector<PersonalPathHistoryPoint> pathHistory{};
    std::optional<PersonalPathPrediction> pathPrediction{};
    std::optional<PersonalPropelledInformation> propulsion{};

    void validate() const;
    [[nodiscard]] std::vector<std::uint8_t> to_bytes() const;
    static PersonalSafetyMessage from_bytes(const std::vector<std::uint8_t>& buffer);
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
[[nodiscard]] std::string to_string(PersonalDeviceUserType type);
[[nodiscard]] std::string to_string(PersonalPropulsionKind kind);

} // namespace ipi::j2735
