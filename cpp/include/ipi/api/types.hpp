#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace ipi::api {

using Timestamp = std::chrono::system_clock::time_point;

enum class TransportType {
    V2X_RSU,
    C_V2X,
    CELLULAR_5G,
    WIRED_BACKHAUL,
    BLE,
    HTTP_BACKHAUL
};

enum class SourceType {
    PCV,
    PCAV,
    PEDESTRIAN_DEVICE,
    CLOUD,
    INTERSECTION,
    INFRASTRUCTURE
};

enum class J2735MessageType {
    BSM,
    SPAT,
    MAP,
    SRM,
    SSM
};

enum class J2735Encoding {
    UPER,
    JSON,
    BYTES
};

enum class PCVServiceType {
    UNPROTECTED_LEFT_TURN_AVAILABILITY,
    LANE_KEEPING_AID
};

enum class PCAVServiceType {
    PERCEPTION_AID,
    PLANNING_AID,
    CONTROL_AID,
    COMPUTATION_AID,
    HDMAP_LANELET_UPDATE,
    UNPROTECTED_LEFT_TURN_AVAILABILITY,
    LANE_KEEPING_AID
};

enum class WarningType {
    PEDESTRIAN_WARNING,
    INCOMING_VEHICLE_WARNING
};

enum class WarningSeverity {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum class InterchangeType {
    DATA_PASSTHROUGH,
    ALERT_RELAY,
    CONTROL_HANDOFF
};

enum class SessionState {
    REGISTERED,
    ACTIVE,
    SUSPENDED,
    TERMINATED
};

enum class VehicleRole {
    PCV,
    PCAV,
    EMERGENCY,
    TRANSIT,
    PEDESTRIAN_DEVICE
};

enum class GuidanceStatus {
    REQUEST,
    UPDATE,
    COMPLETE,
    REJECT
};

enum class ServiceClass {
    GUIDED_PLANNING,
    GUIDED_PERCEPTION,
    GUIDED_CONTROL
};

struct EnvelopeSource {
    SourceType type{SourceType::PCV};
    std::string id{};
};

struct Subscription {
    std::string callbackUrl{};
    std::vector<std::string> eventTypes{};
    std::vector<std::uint8_t> secret{};
};

struct EnvelopeMetadata {
    std::string messageId{};
    Timestamp sentAt{};
    std::string intersectionId{};
    TransportType transport{TransportType::CELLULAR_5G};
    EnvelopeSource source{};
    std::optional<std::string> sessionId{};
    std::optional<std::string> correlationId{};
    std::optional<std::string> priority{};
};

template <typename T>
struct Envelope {
    EnvelopeMetadata metadata{};
    T data{};
};

struct J2735Payload {
    J2735MessageType type{J2735MessageType::BSM};
    J2735Encoding encoding{J2735Encoding::UPER};
    std::vector<std::uint8_t> payload{};
    std::optional<std::uint32_t> frameCounter{};
};

struct GeoPoint {
    double latitude{};
    double longitude{};
    std::optional<double> elevation{};
};

enum class GeometryType {
    POLYGON,
    CIRCLE
};

struct Geofence {
    GeometryType type{GeometryType::POLYGON};
    std::vector<GeoPoint> points{};
    std::optional<double> radiusMeters{};
};

struct Location {
    double latitude{};
    double longitude{};
    std::optional<double> elevation{};
};

struct VehicleServiceRequest {
    std::variant<PCVServiceType, PCAVServiceType> serviceType;
    std::string vehicleId{};
    std::optional<std::string> vin{};
    Location location{};
    std::optional<double> speedMps{};
    std::optional<double> headingDegrees{};
    std::vector<std::uint8_t> context{};
};

enum class VehicleServiceStatus {
    ACCEPTED,
    REJECTED,
    IN_PROGRESS,
    COMPLETED
};

struct VehicleServiceResponse {
    std::variant<PCVServiceType, PCAVServiceType> serviceType;
    std::string vehicleId{};
    VehicleServiceStatus status{VehicleServiceStatus::ACCEPTED};
    std::optional<Timestamp> expiresAt{};
    std::vector<std::uint8_t> guidance{};
};

struct WarningMessage {
    WarningType type{WarningType::PEDESTRIAN_WARNING};
    WarningSeverity severity{WarningSeverity::MEDIUM};
    Geofence geofence{};
    std::string summary{};
    std::vector<std::uint8_t> details{};
};

enum class PedestrianAckStatus {
    ACKED,
    DISMISSED,
    IGNORED
};

struct PedestrianAcknowledgement {
    std::string warningId{};
    PedestrianAckStatus status{PedestrianAckStatus::ACKED};
    Timestamp handledAt{};
    std::string response{};
};

struct CloudTransfer {
    std::string datasetType{};
    Timestamp from{};
    Timestamp to{};
    std::string uri{};
    std::uint64_t sizeBytes{0};
};

struct IntersectionMessage {
    InterchangeType type{InterchangeType::DATA_PASSTHROUGH};
    std::string destinationIntersectionId{};
    std::vector<std::uint8_t> payload{};
};

struct VehicleTelemetryFrame {
    Timestamp timestamp{};
    GeoPoint pose{};
    std::optional<double> speedMps{};
    std::optional<double> accelerationMps2{};
    std::vector<std::uint8_t> context{};
};

struct VehicleProfile {
    std::string vehicleId{};
    std::optional<std::string> vin{};
    VehicleRole role{VehicleRole::PCV};
    std::optional<std::string> oem{};
    std::optional<std::string> softwareVersion{};
    bool supportsStreaming{false};
};

struct SessionDescriptor {
    std::string sessionId{};
    VehicleProfile vehicleProfile{};
    TransportType transport{TransportType::CELLULAR_5G};
    SessionState state{SessionState::REGISTERED};
    std::uint32_t leaseSeconds{0};
    std::uint32_t heartbeatIntervalSeconds{0};
    std::vector<std::string> preferredChannels{};
    std::vector<std::string> grantedServices{};
};

struct CooperativeGuidance {
    GuidanceStatus status{GuidanceStatus::REQUEST};
    ServiceClass serviceClass{ServiceClass::GUIDED_PLANNING};
    std::vector<std::uint8_t> payload{};
};

struct SessionRegistration {
    EnvelopeMetadata metadata{};
    VehicleProfile vehicleProfile{};
    std::vector<std::string> requestedServices{};
    std::optional<std::string> rsuFallback{};
    std::optional<int> minSidelinkRssi{};
    std::optional<Subscription> inlineSubscription{};
};

struct HeartbeatUpdate {
    std::string sessionId{};
    Timestamp timestamp{};
    std::optional<VehicleTelemetryFrame> telemetry{};
};

struct SessionPatch {
    std::string sessionId{};
    std::optional<VehicleProfile> profile{};
    std::optional<std::vector<std::string>> preferredChannels{};
};

struct SessionTermination {
    std::string sessionId{};
    std::string reason{};
};

struct ServiceInvocation {
    std::string sessionId{};
    Envelope<VehicleServiceRequest> request;
};

struct TelemetrySubmission {
    std::string sessionId{};
    std::vector<VehicleTelemetryFrame> frames{};
};

struct CloudExportJob {
    EnvelopeMetadata metadata{};
    std::string datasetType{};
    Timestamp from{};
    Timestamp to{};
};

struct CloudExportStatus {
    std::string jobId{};
    std::string status{};
    std::optional<CloudTransfer> transfer{};
};

struct SubscriptionHandle {
    std::string subscriptionId{};
};

struct BroadcastTarget {
    std::string channel{};
    std::optional<std::string> rsuId{};
};

struct BroadcastRequest {
    EnvelopeMetadata metadata{};
    BroadcastTarget target{};
    J2735Payload message{};
};

struct Ack {
    bool accepted{true};
    std::optional<std::string> id{};
};

} // namespace ipi::api
