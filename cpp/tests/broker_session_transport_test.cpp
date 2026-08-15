#include "test_support.hpp"

#include "ipi/api/private_session_transport.hpp"

#include <algorithm>
#include <map>
#include <mutex>
#include <stdexcept>

namespace {

std::vector<std::string> split(const std::string& value) {
    std::vector<std::string> parts;
    std::size_t start = 0;
    while (start <= value.size()) {
        const auto end = value.find('/', start);
        parts.push_back(value.substr(start,
            end == std::string::npos ? std::string::npos : end - start));
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return parts;
}

bool matches(const std::string& filter, const std::string& topic) {
    const auto expected = split(filter);
    const auto actual = split(topic);
    std::size_t index = 0;
    for (; index < expected.size(); ++index) {
        if (expected[index] == "#") return index + 1 == expected.size();
        if (index >= actual.size()) return false;
        if (expected[index] != "+" && expected[index] != actual[index]) return false;
    }
    return index == actual.size();
}

class InMemoryBroker final : public ipi::api::SessionMessageBroker {
public:
    void publish(const std::string& topic,
                 const std::vector<std::uint8_t>& payload) override {
        std::vector<Handler> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (failNext_) {
                failNext_ = false;
                throw std::runtime_error("injected broker interruption");
            }
            for (const auto& [id, entry] : subscriptions_) {
                (void)id;
                if (matches(entry.first, topic)) callbacks.push_back(entry.second);
            }
        }
        for (const auto& callback : callbacks) callback(topic, payload);
    }

    std::uint64_t subscribe(const std::string& filter, Handler handler) override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto id = ++nextId_;
        subscriptions_[id] = {filter, std::move(handler)};
        return id;
    }

    void unsubscribe(std::uint64_t id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        subscriptions_.erase(id);
    }

    void fail_next_publish() {
        std::lock_guard<std::mutex> lock(mutex_);
        failNext_ = true;
    }

private:
    std::mutex mutex_;
    std::uint64_t nextId_{0};
    bool failNext_{false};
    std::map<std::uint64_t, std::pair<std::string, Handler>> subscriptions_;
};

} // namespace

int main() {
    return ipi::tests::run_test("broker_session_transport_roundtrip", [] {
        using namespace ipi::api;

        bool rejectedInvalidMqttConfig = false;
        try {
            MqttSessionBrokerConfig invalid;
            invalid.ioTimeout = std::chrono::milliseconds::zero();
            (void)make_mqtt_session_message_broker(invalid);
        } catch (const std::invalid_argument&) {
            rejectedInvalidMqttConfig = true;
        }
        ipi::tests::expect(rejectedInvalidMqttConfig,
                           "MQTT broker adapter must reject non-positive I/O timeouts");

        auto broker = std::make_shared<InMemoryBroker>();
        auto endpoint = make_broker_session_endpoint(broker);
        auto client = make_broker_private_session_transport(
            broker, {}, std::chrono::milliseconds(100));

        SessionRegistration registration;
        registration.metadata.messageId = "registration-1";
        registration.metadata.intersectionId = "intersection-1";
        registration.metadata.source.id = "vehicle-1";
        registration.vehicleProfile.vehicleId = "vehicle-1";
        registration.vehicleProfile.role = VehicleRole::PCAV;
        registration.requestedServices = {"planningAid"};
        registration.rsuFallback = "rsu-fallback";
        registration.minSidelinkRssi = -90;
        const auto descriptor = client->register_session(registration);
        ipi::tests::expect(descriptor.state == SessionState::REGISTERED,
                           "broker registration should return registered descriptor");
        ipi::tests::expect(descriptor.rsuFallback == registration.rsuFallback,
                           "broker registration must retain fallback settings");

        HeartbeatUpdate heartbeat;
        heartbeat.sessionId = descriptor.sessionId;
        ipi::tests::expect(client->heartbeat(heartbeat).accepted,
                           "broker heartbeat should round-trip");
        ipi::tests::expect(client->get_session(descriptor.sessionId)->state == SessionState::ACTIVE,
                           "broker client mirror should become active");

        ServiceInvocation invocation;
        invocation.sessionId = descriptor.sessionId;
        invocation.request.metadata.messageId = "service-message-1";
        invocation.request.metadata.intersectionId = "intersection-1";
        invocation.request.metadata.source.id = "vehicle-1";
        invocation.request.metadata.sessionId = descriptor.sessionId;
        invocation.request.metadata.correlationId = "request-1";
        invocation.request.metadata.sequence = 1;
        invocation.request.data.serviceType = PCAVServiceType::PLANNING_AID;
        invocation.request.data.vehicleId = "vehicle-1";
        invocation.request.data.context = {1, 2, 3};
        ipi::tests::expect(client->invoke_service(invocation).accepted,
                           "broker service invocation should be acknowledged");
        const auto responses = client->list_session_responses(
            SessionResponseQuery{descriptor.sessionId});
        ipi::tests::expect(responses.size() == 2,
                           "broker path should deliver progress and terminal responses");
        ipi::tests::expect(responses.back().data.status == VehicleServiceStatus::COMPLETED,
                           "broker path should deliver a completed terminal response");

        SessionRegistration pcvRegistration;
        pcvRegistration.metadata.messageId = "pcv-registration-1";
        pcvRegistration.metadata.intersectionId = "intersection-1";
        pcvRegistration.metadata.source.id = "vehicle-2";
        pcvRegistration.vehicleProfile.vehicleId = "vehicle-2";
        pcvRegistration.vehicleProfile.role = VehicleRole::PCV;
        pcvRegistration.requestedServices = {"laneKeepingAid"};
        const auto pcvSession = client->register_session(pcvRegistration);
        ipi::tests::expect(
            client->heartbeat(HeartbeatUpdate{pcvSession.sessionId}).accepted,
            "PCV broker heartbeat should round-trip");

        ServiceInvocation pcvInvocation;
        pcvInvocation.sessionId = pcvSession.sessionId;
        pcvInvocation.request.metadata.messageId = "pcv-service-message-1";
        pcvInvocation.request.metadata.intersectionId = "intersection-1";
        pcvInvocation.request.metadata.source.id = "vehicle-2";
        pcvInvocation.request.metadata.sessionId = pcvSession.sessionId;
        pcvInvocation.request.metadata.correlationId = "pcv-request-1";
        pcvInvocation.request.metadata.sequence = 1;
        pcvInvocation.request.data.serviceType = PCVServiceType::LANE_KEEPING_AID;
        pcvInvocation.request.data.vehicleId = "vehicle-2";
        pcvInvocation.request.data.context = {4, 5, 6};
        ipi::tests::expect(client->invoke_service(pcvInvocation).accepted,
                           "PCV service invocation should be acknowledged");
        ipi::tests::expect(
            client->list_session_responses(SessionResponseQuery{pcvSession.sessionId}).size() == 2,
            "PCV progress and terminal responses should reach the client");
        SessionPublicationQuery pcvPublications;
        pcvPublications.kind = SessionTopicKind::PcvResponse;
        ipi::tests::expect(client->list_publications(pcvPublications).size() == 2,
                           "PCV responses must use the dedicated response topic family");

        SessionPatch patch;
        patch.sessionId = descriptor.sessionId;
        patch.preferredChannels = std::vector<std::string>{"planningAid", "local"};
        ipi::tests::expect(client->patch_session(patch).accepted,
                           "broker session patch should round-trip");

        broker->fail_next_publish();
        const auto interrupted = client->submit_telemetry(
            TelemetrySubmission{descriptor.sessionId, {VehicleTelemetryFrame{}}});
        ipi::tests::expect(!interrupted.accepted &&
                               interrupted.code == FailureCode::TRANSPORT_INTERRUPTION,
                           "broker publication failure must be typed as transport interruption");

        SessionTermination termination{descriptor.sessionId,
                                       TerminationReasonCode::CLIENT_REQUEST,
                                       "test complete"};
        ipi::tests::expect(client->terminate_session(termination).accepted,
                           "broker termination should round-trip");
        auto afterTermination = invocation;
        afterTermination.request.metadata.messageId = "service-message-2";
        afterTermination.request.metadata.correlationId = "request-2";
        afterTermination.request.metadata.sequence = 2;
        ipi::tests::expect(
            client->invoke_service(afterTermination).code == FailureCode::SESSION_TERMINATED,
            "broker client must reject requests after termination");

        const auto publications = client->list_publications({});
        ipi::tests::expect(!publications.empty(),
                           "broker transport should retain encoded request/response evidence");
        ipi::tests::expect(std::all_of(publications.begin(), publications.end(),
            [](const SessionPublication& publication) {
                return !publication.encodedPayload.empty();
            }), "every broker publication must retain encoded wire bytes");

        (void)endpoint;
    });
}
