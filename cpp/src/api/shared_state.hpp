#pragma once

#include "ipi/api/types.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace ipi::api::detail {

struct SharedState {
    std::mutex mutex;

    struct StoredV2xMessage {
        Envelope<J2735Payload> envelope;
        std::optional<int> rssi;
        std::optional<int> channel;
    };

    std::vector<StoredV2xMessage> v2xMessages;
    std::vector<BroadcastRequest> broadcastRequests;
    std::vector<Envelope<VehicleServiceRequest>> pcvRequests;
    std::vector<Envelope<VehicleServiceRequest>> pcavRequests;
    std::vector<Envelope<PedestrianAcknowledgement>> pedestrianAcknowledgements;
    std::vector<Envelope<IntersectionMessage>> intersectionMessages;

    std::unordered_map<std::string, SessionDescriptor> sessionDirectory;
    std::unordered_map<std::string, std::vector<VehicleTelemetryFrame>> telemetryBySession;

    std::unordered_map<std::string, std::vector<Envelope<VehicleServiceResponse>>> responsesByVehicle;
    std::unordered_map<std::string, std::vector<Envelope<VehicleServiceResponse>>> responsesBySession;

    std::unordered_map<std::string, std::vector<Envelope<WarningMessage>>> warningsByDevice;

    std::unordered_map<std::string, CloudExportStatus> cloudJobs;
    std::uint64_t cloudJobCounter{0};

    std::unordered_map<std::string, Subscription> subscriptions;
    std::uint64_t subscriptionCounter{0};

    std::uint64_t messageCounter{0};
};

std::shared_ptr<SharedState> obtain_shared_state();

} // namespace ipi::api::detail
