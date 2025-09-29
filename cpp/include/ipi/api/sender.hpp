#pragma once

#include "ipi/api/types.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace ipi::api {

struct V2xQuery {
    J2735MessageType type{J2735MessageType::BSM};
    std::optional<Timestamp> since{};
    std::size_t limit{50};
};

struct ResponseQuery {
    std::string vehicleId{};
    std::optional<std::variant<PCVServiceType, PCAVServiceType>> serviceType{};
    std::optional<Timestamp> since{};
};

struct SessionResponseQuery {
    std::string sessionId{};
};

struct WarningQuery {
    std::string deviceId{};
};

struct InterchangeQuery {
    std::string destinationIntersectionId{};
    std::optional<Timestamp> since{};
};

class SenderApi {
public:
    virtual ~SenderApi() = default;

    virtual SubscriptionHandle registerSubscription(const Subscription& subscription) = 0;
    virtual Ack unregisterSubscription(const SubscriptionHandle& handle) = 0;

    virtual std::vector<J2735Payload> listV2xMessages(const V2xQuery& query) const = 0;

    virtual std::vector<Envelope<VehicleServiceResponse>> listPcvResponses(const ResponseQuery& query) const = 0;

    virtual std::vector<Envelope<VehicleServiceResponse>> listSessionResponses(const SessionResponseQuery& query) const = 0;

    virtual std::vector<Envelope<WarningMessage>> listPedestrianWarnings(const WarningQuery& query) const = 0;

    virtual Ack requestCloudExport(const CloudExportJob& job, std::string& outJobId) = 0;

    virtual std::optional<CloudExportStatus> getCloudExportStatus(const std::string& jobId) const = 0;

    virtual std::vector<Envelope<IntersectionMessage>> listOutboundIntersectionMessages(const InterchangeQuery& query) const = 0;
};

std::shared_ptr<SenderApi> make_in_memory_sender_api();

} // namespace ipi::api
