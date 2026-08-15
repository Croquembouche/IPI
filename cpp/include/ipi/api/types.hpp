#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace ipi::api {

using Timestamp = std::chrono::system_clock::time_point;

enum class TransportType : std::uint8_t {
    V2X_RSU = 0,
    C_V2X = 1,
    CELLULAR_5G = 2,
    WIRED_BACKHAUL = 3,
    BLE = 4,
    HTTP_BACKHAUL = 5
};

enum class SourceType : std::uint8_t {
    PCV = 0,
    PCAV = 1,
    PEDESTRIAN_DEVICE = 2,
    CLOUD = 3,
    INTERSECTION = 4,
    INFRASTRUCTURE = 5
};

enum class J2735MessageType {
    BSM,
    SPAT,
    MAP,
    SRM,
    SSM,
    PSM,
    TIM,
    IPI_COOPERATIVE_SERVICE
};

enum class J2735Encoding {
    UPER,
    JSON,
    BYTES
};

enum class PCVServiceType : std::uint8_t {
    UNPROTECTED_LEFT_TURN_AVAILABILITY = 0,
    LANE_KEEPING_AID = 1
};

enum class PCAVServiceType : std::uint8_t {
    PERCEPTION_AID = 0,
    PLANNING_AID = 1,
    CONTROL_AID = 2,
    COMPUTATION_AID = 3,
    HDMAP_LANELET_UPDATE = 4,
    UNPROTECTED_LEFT_TURN_AVAILABILITY = 5,
    LANE_KEEPING_AID = 6
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

enum class SessionState : std::uint8_t {
    REGISTERED = 0,
    ACTIVE = 1,
    SUSPENDED = 2,
    TERMINATED = 3,
    EXPIRED = 4
};

/** Machine-readable outcome codes shared by API acknowledgements and services. */
enum class FailureCode : std::uint8_t {
    NONE = 0,
    INVALID_REQUEST = 1,
    UNKNOWN_SESSION = 2,
    SESSION_INACTIVE = 3,
    SESSION_TERMINATED = 4,
    SESSION_EXPIRED = 5,
    TIMEOUT = 6,
    MISSING_RESPONSE = 7,
    TRANSPORT_INTERRUPTION = 8,
    SERVICE_UNAVAILABLE = 9,
    STALE_REQUEST = 10,
    STALE_RESPONSE = 11,
    DUPLICATE_RESPONSE = 12,
    CORRELATION_MISMATCH = 13,
    ACCESS_DENIED = 14,
    PROTOCOL_ERROR = 15,
    FALLBACK_FAILED = 16
};

enum class TerminationReasonCode : std::uint8_t {
    CLIENT_REQUEST = 0,
    LEASE_EXPIRED = 1,
    HEARTBEAT_TIMEOUT = 2,
    TRANSPORT_INTERRUPTION = 3,
    SERVICE_UNAVAILABLE = 4,
    ADMINISTRATIVE = 5,
    PROTOCOL_ERROR = 6
};

enum class FallbackResult : std::uint8_t {
    NOT_ATTEMPTED = 0,
    ACTIVATED = 1,
    SUCCEEDED = 2,
    FAILED = 3
};

enum class VehicleRole : std::uint8_t {
    PCV = 0,
    PCAV = 1,
    EMERGENCY = 2,
    TRANSIT = 3,
    PEDESTRIAN_DEVICE = 4
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
    std::optional<std::uint64_t> sequence{};
    std::optional<Timestamp> expiresAt{};
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

enum class VehicleServiceStatus : std::uint8_t {
    ACCEPTED = 0,
    REJECTED = 1,
    IN_PROGRESS = 2,
    COMPLETED = 3,
    TIMED_OUT = 4,
    MISSING_RESPONSE = 5,
    TRANSPORT_INTERRUPTED = 6,
    SERVICE_UNAVAILABLE = 7,
    EXPIRED = 8,
    FALLBACK_COMPLETED = 9
};

struct VehicleServiceResponse {
    std::variant<PCVServiceType, PCAVServiceType> serviceType;
    std::string vehicleId{};
    VehicleServiceStatus status{VehicleServiceStatus::ACCEPTED};
    std::optional<Timestamp> expiresAt{};
    std::vector<std::uint8_t> guidance{};
    std::string resultId{};
    FailureCode failureCode{FailureCode::NONE};
    std::optional<FallbackResult> fallbackResult{};
    std::string detail{};
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
    Timestamp registeredAt{};
    Timestamp lastHeartbeatAt{};
    Timestamp expiresAt{};
    std::optional<std::string> rsuFallback{};
    std::optional<int> minSidelinkRssi{};
    std::optional<Subscription> inlineSubscription{};
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
    TerminationReasonCode reasonCode{TerminationReasonCode::CLIENT_REQUEST};
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
    FailureCode code{FailureCode::NONE};
    std::string detail{};
    std::optional<FallbackResult> fallbackResult{};
};

[[nodiscard]] std::string to_string(SessionState state);
[[nodiscard]] std::string to_string(FailureCode code);
[[nodiscard]] std::string to_string(TerminationReasonCode code);
[[nodiscard]] std::string to_string(FallbackResult result);
[[nodiscard]] std::string to_string(VehicleServiceStatus status);
[[nodiscard]] bool is_terminal(VehicleServiceStatus status) noexcept;
void validate_vehicle_service_response(const VehicleServiceResponse& response);

} // namespace ipi::api
