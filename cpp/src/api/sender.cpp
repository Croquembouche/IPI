#include "ipi/api/sender.hpp"

#include "shared_state.hpp"

#include <algorithm>
#include <optional>
#include <sstream>

namespace ipi::api {

namespace {

std::string make_subscription_id(detail::SharedState& state)
{
    std::ostringstream oss;
    oss << "sub-" << ++state.subscriptionCounter;
    return oss.str();
}

class InMemorySenderApi final : public SenderApi {
public:
    explicit InMemorySenderApi(std::shared_ptr<detail::SharedState> state)
        : state_(std::move(state)) {}

    SubscriptionHandle registerSubscription(const Subscription& subscription) override
    {
        std::lock_guard lock(state_->mutex);
        auto id = make_subscription_id(*state_);
        state_->subscriptions[id] = subscription;
        return SubscriptionHandle{id};
    }

    Ack unregisterSubscription(const SubscriptionHandle& handle) override
    {
        std::lock_guard lock(state_->mutex);
        auto erased = state_->subscriptions.erase(handle.subscriptionId);
        return Ack{erased > 0, handle.subscriptionId};
    }

    std::vector<J2735Payload> listV2xMessages(const V2xQuery& query) const override
    {
        std::lock_guard lock(state_->mutex);
        std::vector<J2735Payload> result;
        result.reserve(query.limit);
        for (auto it = state_->v2xMessages.rbegin(); it != state_->v2xMessages.rend(); ++it) {
            if (it->envelope.data.type != query.type) {
                continue;
            }
            if (query.since && it->envelope.metadata.sentAt < *query.since) {
                continue;
            }
            result.push_back(it->envelope.data);
            if (result.size() >= query.limit) break;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    std::vector<Envelope<VehicleServiceResponse>> listPcvResponses(const ResponseQuery& query) const override
    {
        std::lock_guard lock(state_->mutex);
        auto it = state_->responsesByVehicle.find(query.vehicleId);
        if (it == state_->responsesByVehicle.end()) {
            return {};
        }
        return filter_responses(it->second, query.serviceType, query.since);
    }

    std::vector<Envelope<VehicleServiceResponse>> listSessionResponses(const SessionResponseQuery& query) const override
    {
        std::lock_guard lock(state_->mutex);
        auto it = state_->responsesBySession.find(query.sessionId);
        if (it == state_->responsesBySession.end()) {
            return {};
        }
        return it->second;
    }

    std::vector<Envelope<WarningMessage>> listPedestrianWarnings(const WarningQuery& query) const override
    {
        std::lock_guard lock(state_->mutex);
        auto it = state_->warningsByDevice.find(query.deviceId);
        if (it == state_->warningsByDevice.end()) {
            return {};
        }
        return it->second;
    }

    Ack requestCloudExport(const CloudExportJob& job, std::string& outJobId) override
    {
        (void)job;
        std::lock_guard lock(state_->mutex);
        std::ostringstream oss;
        oss << "export-" << ++state_->cloudJobCounter;
        outJobId = oss.str();
        CloudExportStatus status;
        status.jobId = outJobId;
        status.status = "PROCESSING";
        status.transfer.reset();
        state_->cloudJobs[outJobId] = status;
        return Ack{true, outJobId};
    }

    std::optional<CloudExportStatus> getCloudExportStatus(const std::string& jobId) const override
    {
        std::lock_guard lock(state_->mutex);
        auto it = state_->cloudJobs.find(jobId);
        if (it == state_->cloudJobs.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<Envelope<IntersectionMessage>> listOutboundIntersectionMessages(const InterchangeQuery& query) const override
    {
        std::lock_guard lock(state_->mutex);
        std::vector<Envelope<IntersectionMessage>> result;
        for (const auto& entry : state_->intersectionMessages) {
            if (entry.data.destinationIntersectionId != query.destinationIntersectionId) {
                continue;
            }
            if (query.since && entry.metadata.sentAt < *query.since) {
                continue;
            }
            result.push_back(entry);
        }
        return result;
    }

private:
    static std::vector<Envelope<VehicleServiceResponse>> filter_responses(
        const std::vector<Envelope<VehicleServiceResponse>>& source,
        const std::optional<std::variant<PCVServiceType, PCAVServiceType>>& serviceType,
        const std::optional<Timestamp>& since)
    {
        if (!serviceType && !since) {
            return source;
        }
        std::vector<Envelope<VehicleServiceResponse>> filtered;
        for (const auto& response : source) {
            if (serviceType) {
                if (response.data.serviceType.index() != serviceType->index() || response.data.serviceType != *serviceType) {
                    continue;
                }
            }
            if (since && response.metadata.sentAt < *since) {
                continue;
            }
            filtered.push_back(response);
        }
        return filtered;
    }

    std::shared_ptr<detail::SharedState> state_;
};

} // namespace

std::shared_ptr<SenderApi> make_in_memory_sender_api()
{
    return std::make_shared<InMemorySenderApi>(detail::obtain_shared_state());
}

} // namespace ipi::api
