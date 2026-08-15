#include "ipi/api/private_session_transport.hpp"

#include "ipi/api/in_memory_api.hpp"
#include "ipi/api/minimal_mqtt_client.hpp"
#include "ipi/api/session_wire_codec.hpp"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>

namespace ipi::api {
namespace {

bool topic_filter_matches(const std::string& filter, const std::string& topic) {
    auto split = [](const std::string& value) {
        std::vector<std::string> parts;
        std::size_t start = 0;
        while (start <= value.size()) {
            const auto end = value.find('/', start);
            parts.push_back(value.substr(
                start, end == std::string::npos ? std::string::npos : end - start));
            if (end == std::string::npos) break;
            start = end + 1;
        }
        return parts;
    };

    const auto filterParts = split(filter);
    const auto topicParts = split(topic);
    std::size_t index = 0;
    for (; index < filterParts.size(); ++index) {
        if (filterParts[index] == "#") return index + 1 == filterParts.size();
        if (index >= topicParts.size()) return false;
        if (filterParts[index] != "+" && filterParts[index] != topicParts[index]) return false;
    }
    return index == topicParts.size();
}

bool timestamp_is_default(Timestamp value) {
    return value.time_since_epoch() == Timestamp::duration::zero();
}

Ack reject(FailureCode code, std::string detail) {
    Ack acknowledgement;
    acknowledgement.accepted = false;
    acknowledgement.code = code;
    acknowledgement.detail = std::move(detail);
    return acknowledgement;
}

SessionTopic service_response_topic(
    const SessionSnapshot& snapshot,
    const std::string& sessionId,
    const Envelope<VehicleServiceResponse>& response) {
    if (std::holds_alternative<PCVServiceType>(response.data.serviceType)) {
        return SessionTopic{SessionTopicKind::PcvResponse,
                            snapshot.registration.metadata.intersectionId,
                            std::nullopt, response.data.vehicleId};
    }
    return SessionTopic{SessionTopicKind::ServiceUpdate,
                        snapshot.registration.metadata.intersectionId,
                        sessionId, std::nullopt};
}

class MqttSessionMessageBroker final : public SessionMessageBroker {
public:
    explicit MqttSessionMessageBroker(MqttSessionBrokerConfig config)
        : config_(std::move(config)),
          publisher_(config_.clientId + "-publisher"),
          subscriber_(config_.clientId + "-subscriber") {
        if (config_.host.empty() || config_.port == 0 || config_.clientId.empty() ||
            config_.allowedTopicPrefix.empty() ||
            config_.allowedTopicPrefix.find_first_of("+#") != std::string::npos ||
            config_.reconnectBackoff <= std::chrono::milliseconds::zero() ||
            config_.ioTimeout <= std::chrono::milliseconds::zero() ||
            (config_.password && !config_.username)) {
            throw std::invalid_argument(
                "MQTT session broker configuration has an invalid host, port, clientId, credentials, topic prefix, or timeout");
        }
        worker_ = std::thread([this] { receive_loop(); });
    }

    ~MqttSessionMessageBroker() override {
        stopping_.store(true);
        if (worker_.joinable()) worker_.join();
        std::lock_guard<std::mutex> publisherLock(publisherMutex_);
        publisher_.disconnect();
        std::lock_guard<std::mutex> subscriberLock(subscriberMutex_);
        subscriber_.disconnect();
    }

    void publish(const std::string& topic,
                 const std::vector<std::uint8_t>& payload) override {
        validate_publish_topic(topic);
        std::lock_guard<std::mutex> lock(publisherMutex_);
        try {
            ensure_publisher_connected();
            publisher_.publish(topic, payload);
        } catch (...) {
            publisher_.disconnect();
            ensure_publisher_connected();
            publisher_.publish(topic, payload);
        }
    }

    std::uint64_t subscribe(const std::string& topicFilter,
                            Handler handler) override {
        if (topicFilter.rfind(config_.allowedTopicPrefix, 0) != 0 || !handler) {
            throw std::invalid_argument("MQTT subscription is outside the allowed topic prefix");
        }
        const auto id = ++nextSubscriptionId_;
        std::lock_guard<std::mutex> lock(handlerMutex_);
        handlers_[id] = {topicFilter, std::move(handler)};
        subscriptionsDirty_.store(true);
        return id;
    }

    void unsubscribe(std::uint64_t subscriptionId) override {
        std::lock_guard<std::mutex> lock(handlerMutex_);
        handlers_.erase(subscriptionId);
        subscriptionsDirty_.store(true);
    }

private:
    struct HandlerRecord {
        std::string filter;
        Handler handler;
    };

    MqttConnectOptions connect_options() const {
        MqttConnectOptions options;
        options.keepAliveSeconds = config_.keepAliveSeconds;
        options.cleanSession = config_.cleanSession;
        options.username = config_.username;
        options.password = config_.password;
        options.ioTimeout = config_.ioTimeout;
        return options;
    }

    void validate_publish_topic(const std::string& topic) const {
        if (topic.rfind(config_.allowedTopicPrefix, 0) != 0 ||
            topic.find_first_of("+#") != std::string::npos) {
            throw std::invalid_argument("MQTT publish topic is outside the allowed prefix or has wildcards");
        }
    }

    void ensure_publisher_connected() {
        if (!publisher_.connected()) {
            publisher_.connect(config_.host, config_.port, connect_options());
        }
    }

    std::vector<std::string> unique_filters() const {
        std::lock_guard<std::mutex> lock(handlerMutex_);
        std::vector<std::string> filters;
        for (const auto& [id, record] : handlers_) {
            (void)id;
            if (std::find(filters.begin(), filters.end(), record.filter) == filters.end()) {
                filters.push_back(record.filter);
            }
        }
        return filters;
    }

    void reconnect_subscriber() {
        subscriber_.disconnect();
        subscriber_.connect(config_.host, config_.port, connect_options());
        for (const auto& filter : unique_filters()) subscriber_.subscribe(filter);
        subscriptionsDirty_.store(false);
        lastPing_ = std::chrono::steady_clock::now();
    }

    void refresh_subscriptions() {
        if (!subscriber_.connected() || subscriptionsDirty_.load()) {
            reconnect_subscriber();
        }
    }

    void dispatch(const MqttMessage& message) {
        std::vector<Handler> callbacks;
        {
            std::lock_guard<std::mutex> lock(handlerMutex_);
            for (const auto& [id, record] : handlers_) {
                (void)id;
                if (topic_filter_matches(record.filter, message.topic)) {
                    callbacks.push_back(record.handler);
                }
            }
        }
        for (const auto& callback : callbacks) callback(message.topic, message.payload);
    }

    void receive_loop() {
        while (!stopping_.load()) {
            try {
                std::optional<MqttMessage> message;
                {
                    std::lock_guard<std::mutex> lock(subscriberMutex_);
                    refresh_subscriptions();
                    message = subscriber_.receive(std::chrono::milliseconds(100));
                    const auto now = std::chrono::steady_clock::now();
                    const auto pingInterval = std::chrono::seconds(
                        std::max<std::uint16_t>(1, config_.keepAliveSeconds / 2));
                    if (!message && now - lastPing_ >= pingInterval) {
                        subscriber_.ping();
                        lastPing_ = now;
                    }
                }
                if (message) dispatch(*message);
            } catch (...) {
                {
                    std::lock_guard<std::mutex> lock(subscriberMutex_);
                    subscriber_.disconnect();
                }
                if (!stopping_.load()) {
                    std::this_thread::sleep_for(config_.reconnectBackoff);
                }
            }
        }
    }

    MqttSessionBrokerConfig config_;
    MinimalMqttClient publisher_;
    MinimalMqttClient subscriber_;
    mutable std::mutex publisherMutex_;
    mutable std::mutex subscriberMutex_;
    mutable std::mutex handlerMutex_;
    std::map<std::uint64_t, HandlerRecord> handlers_;
    std::atomic<std::uint64_t> nextSubscriptionId_{0};
    std::atomic<bool> subscriptionsDirty_{true};
    std::atomic<bool> stopping_{false};
    std::chrono::steady_clock::time_point lastPing_{};
    std::thread worker_;
};

class BrokerPrivateSessionTransport final
    : public PrivateSessionTransport,
      public std::enable_shared_from_this<BrokerPrivateSessionTransport> {
public:
    BrokerPrivateSessionTransport(std::shared_ptr<SessionMessageBroker> broker,
                                  SessionTransportOptions options,
                                  std::chrono::milliseconds responseTimeout)
        : broker_(std::move(broker)),
          wallClock_(options.wallClock ? options.wallClock
                                       : [] { return std::chrono::system_clock::now(); }),
          lifecycle_(std::move(options)),
          responseTimeout_(responseTimeout) {
        if (!broker_) throw std::invalid_argument("broker transport requires a broker");
        if (responseTimeout_ <= std::chrono::milliseconds::zero()) {
            throw std::invalid_argument("broker response timeout must be positive");
        }
    }

    ~BrokerPrivateSessionTransport() override {
        for (const auto id : subscriptionIds_) broker_->unsubscribe(id);
    }

    void start() {
        const auto weak = weak_from_this();
        subscriptionIds_.push_back(broker_->subscribe(
            "ipi/+/session/+/events",
            [weak](const std::string& topic, const std::vector<std::uint8_t>& payload) {
                if (const auto self = weak.lock()) self->handle_incoming(topic, payload);
            }));
        subscriptionIds_.push_back(broker_->subscribe(
            "ipi/+/session/+/service/update",
            [weak](const std::string& topic, const std::vector<std::uint8_t>& payload) {
                if (const auto self = weak.lock()) self->handle_incoming(topic, payload);
            }));
        subscriptionIds_.push_back(broker_->subscribe(
            "ipi/+/pcv/+/response",
            [weak](const std::string& topic, const std::vector<std::uint8_t>& payload) {
                if (const auto self = weak.lock()) self->handle_incoming(topic, payload);
            }));
    }

    SessionDescriptor register_session(const SessionRegistration& input) override {
        SessionRegistration registration = input;
        normalize_registration(registration);
        if (!registration.metadata.sessionId) {
            registration.metadata.sessionId = next_session_id();
        }
        const auto requestId = registration.metadata.messageId;
        const auto pending = create_pending(requestId);
        const auto topic = SessionTopic{SessionTopicKind::Register,
                                        registration.metadata.intersectionId,
                                        std::nullopt, std::nullopt}.to_string();
        const auto wire = make_session_registration_wire_message(topic, registration);
        try {
            publish_wire(wire);
        } catch (const std::exception& error) {
            std::lock_guard<std::mutex> lock(mutex_);
            pending_.erase(requestId);
            throw SessionTransportException(FailureCode::TRANSPORT_INTERRUPTION,
                                            error.what());
        }
        const auto result = wait_pending(requestId, pending);
        if (!result) {
            throw SessionTransportException(FailureCode::TIMEOUT,
                                            "timed out waiting for session registration result");
        }
        if (const auto acknowledgement = std::get_if<Ack>(&*result)) {
            if (!acknowledgement->accepted) {
                throw SessionTransportException(acknowledgement->code,
                                                acknowledgement->detail);
            }
            throw SessionTransportException(FailureCode::PROTOCOL_ERROR,
                                            "registration returned acknowledgement without descriptor");
        }
        const auto descriptor = std::get<SessionDescriptor>(*result);
        if (descriptor.sessionId != *registration.metadata.sessionId) {
            throw SessionTransportException(FailureCode::CORRELATION_MISMATCH,
                                            "registration descriptor session mismatch");
        }
        const auto local = lifecycle_.register_session(registration);
        if (local.sessionId != descriptor.sessionId) {
            throw SessionTransportException(FailureCode::CORRELATION_MISMATCH,
                                            "local and remote session identifiers differ");
        }
        return descriptor;
    }

    Ack heartbeat(const HeartbeatUpdate& input) override {
        HeartbeatUpdate heartbeatUpdate = input;
        if (timestamp_is_default(heartbeatUpdate.timestamp)) heartbeatUpdate.timestamp = wallClock_();
        const auto snapshot = lifecycle_.get_snapshot(heartbeatUpdate.sessionId);
        if (!snapshot) return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        auto metadata = operation_metadata(*snapshot, heartbeatUpdate.sessionId);
        const auto wire = make_heartbeat_wire_message(
            SessionTopic{SessionTopicKind::Heartbeat,
                         snapshot->registration.metadata.intersectionId,
                         heartbeatUpdate.sessionId, std::nullopt}.to_string(),
            metadata, heartbeatUpdate);
        auto acknowledgement = transact_ack(wire);
        if (acknowledgement.accepted) {
            const auto local = lifecycle_.heartbeat(heartbeatUpdate);
            if (!local.accepted) return local;
        }
        return acknowledgement;
    }

    Ack patch_session(const SessionPatch& patch) override {
        const auto snapshot = lifecycle_.get_snapshot(patch.sessionId);
        if (!snapshot) return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        auto metadata = operation_metadata(*snapshot, patch.sessionId);
        const auto wire = make_session_patch_wire_message(
            SessionTopic{SessionTopicKind::Patch,
                         snapshot->registration.metadata.intersectionId,
                         patch.sessionId, std::nullopt}.to_string(), metadata, patch);
        auto acknowledgement = transact_ack(wire);
        if (acknowledgement.accepted) {
            const auto local = lifecycle_.patch_session(patch);
            if (!local.accepted) return local;
        }
        return acknowledgement;
    }

    Ack terminate_session(const SessionTermination& termination) override {
        const auto snapshot = lifecycle_.get_snapshot(termination.sessionId);
        if (!snapshot) return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        auto metadata = operation_metadata(*snapshot, termination.sessionId);
        const auto wire = make_session_termination_wire_message(
            SessionTopic{SessionTopicKind::Terminate,
                         snapshot->registration.metadata.intersectionId,
                         termination.sessionId, std::nullopt}.to_string(),
            metadata, termination);
        auto acknowledgement = transact_ack(wire);
        if (acknowledgement.accepted) {
            const auto local = lifecycle_.terminate_session(termination);
            if (!local.accepted) return local;
        }
        return acknowledgement;
    }

    Ack invoke_service(const ServiceInvocation& input) override {
        ServiceInvocation invocation = input;
        normalize_invocation(invocation);
        auto local = lifecycle_.authorize_service(invocation);
        if (!local.accepted) return local;
        const auto snapshot = lifecycle_.get_snapshot(invocation.sessionId);
        if (!snapshot) return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        const auto wire = make_service_invocation_wire_message(
            SessionTopic{SessionTopicKind::ServiceRequest,
                         snapshot->registration.metadata.intersectionId,
                         invocation.sessionId, std::nullopt}.to_string(), invocation);
        return transact_ack(wire);
    }

    Ack submit_telemetry(const TelemetrySubmission& submission) override {
        auto local = lifecycle_.authorize_telemetry(submission);
        if (!local.accepted) return local;
        const auto snapshot = lifecycle_.get_snapshot(submission.sessionId);
        if (!snapshot) return reject(FailureCode::UNKNOWN_SESSION, "unknown session");
        auto metadata = operation_metadata(*snapshot, submission.sessionId);
        const auto wire = make_telemetry_wire_message(
            SessionTopic{SessionTopicKind::Telemetry,
                         snapshot->registration.metadata.intersectionId,
                         submission.sessionId, std::nullopt}.to_string(),
            metadata, submission);
        return transact_ack(wire);
    }

    Ack deliver_service_update(
        const std::string& sessionId,
        const Envelope<VehicleServiceResponse>& response) override {
        auto acknowledgement = lifecycle_.accept_service_response(sessionId, response);
        if (!acknowledgement.accepted) return acknowledgement;
        std::lock_guard<std::mutex> lock(mutex_);
        responses_[sessionId].push_back(response);
        acknowledgement.id = response.data.resultId;
        return acknowledgement;
    }

    std::optional<SessionDescriptor> get_session(const std::string& sessionId) const override {
        return lifecycle_.get_session(sessionId);
    }

    std::vector<Envelope<VehicleServiceResponse>> list_session_responses(
        const SessionResponseQuery& query) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto found = responses_.find(query.sessionId);
        if (found == responses_.end()) return {};
        const auto now = wallClock_();
        std::vector<Envelope<VehicleServiceResponse>> result;
        for (const auto& response : found->second) {
            if (!response.data.expiresAt || *response.data.expiresAt > now) result.push_back(response);
        }
        return result;
    }

    std::vector<SessionPublication> list_publications(
        const SessionPublicationQuery& query = {}) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<SessionPublication> result;
        for (auto it = publications_.rbegin(); it != publications_.rend(); ++it) {
            if (query.kind && it->topic.kind != *query.kind) continue;
            if (query.intersectionId && it->topic.intersectionId != *query.intersectionId) continue;
            if (query.sessionId && it->topic.sessionId != query.sessionId) continue;
            result.push_back(*it);
            if (result.size() >= query.limit) break;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

private:
    using PendingValue = std::variant<Ack, SessionDescriptor>;
    struct Pending {
        std::mutex mutex;
        std::condition_variable condition;
        std::optional<PendingValue> value;
    };

    void normalize_registration(SessionRegistration& registration) {
        if (registration.metadata.messageId.empty()) registration.metadata.messageId = next_message_id();
        if (timestamp_is_default(registration.metadata.sentAt)) registration.metadata.sentAt = wallClock_();
        if (registration.metadata.source.id.empty()) {
            registration.metadata.source.id = registration.vehicleProfile.vehicleId;
        }
    }

    void normalize_invocation(ServiceInvocation& invocation) {
        auto& metadata = invocation.request.metadata;
        metadata.sessionId = invocation.sessionId;
        if (metadata.messageId.empty()) metadata.messageId = next_message_id();
        if (timestamp_is_default(metadata.sentAt)) metadata.sentAt = wallClock_();
        if (!metadata.correlationId) metadata.correlationId = metadata.messageId;
        if (metadata.source.id.empty()) metadata.source.id = invocation.request.data.vehicleId;
    }

    EnvelopeMetadata operation_metadata(const SessionSnapshot& snapshot,
                                        const std::string& sessionId) {
        EnvelopeMetadata metadata;
        metadata.messageId = next_message_id();
        metadata.sentAt = wallClock_();
        metadata.intersectionId = snapshot.registration.metadata.intersectionId;
        metadata.transport = snapshot.descriptor.transport;
        metadata.source = snapshot.registration.metadata.source;
        metadata.sessionId = sessionId;
        return metadata;
    }

    std::string next_message_id() {
        std::ostringstream out;
        out << "broker-message-" << ++messageCounter_;
        return out.str();
    }

    std::string next_session_id() {
        std::ostringstream out;
        out << "broker-session-" << ++sessionCounter_ << '-'
            << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()).count();
        return out.str();
    }

    std::shared_ptr<Pending> create_pending(const std::string& requestId) {
        auto pending = std::make_shared<Pending>();
        std::lock_guard<std::mutex> lock(mutex_);
        if (!pending_.emplace(requestId, pending).second) {
            throw SessionTransportException(FailureCode::STALE_REQUEST,
                                            "duplicate broker request messageId");
        }
        return pending;
    }

    std::optional<PendingValue> wait_pending(const std::string& requestId,
                                             const std::shared_ptr<Pending>& pending) {
        std::unique_lock<std::mutex> lock(pending->mutex);
        pending->condition.wait_for(lock, responseTimeout_, [&] { return pending->value.has_value(); });
        auto result = pending->value;
        lock.unlock();
        std::lock_guard<std::mutex> mapLock(mutex_);
        pending_.erase(requestId);
        return result;
    }

    Ack transact_ack(const SessionWireMessage& wire) {
        const auto pending = create_pending(wire.metadata.messageId);
        try {
            publish_wire(wire);
        } catch (const std::exception& error) {
            std::lock_guard<std::mutex> lock(mutex_);
            pending_.erase(wire.metadata.messageId);
            return reject(FailureCode::TRANSPORT_INTERRUPTION, error.what());
        }
        const auto result = wait_pending(wire.metadata.messageId, pending);
        if (!result) return reject(FailureCode::TIMEOUT, "broker operation timed out");
        if (const auto acknowledgement = std::get_if<Ack>(&*result)) return *acknowledgement;
        return reject(FailureCode::PROTOCOL_ERROR,
                      "broker operation returned an unexpected session descriptor");
    }

    void publish_wire(const SessionWireMessage& wire) {
        const auto encoded = encode_session_wire_message(wire);
        record(wire, encoded);
        broker_->publish(wire.topic, encoded);
    }

    void handle_incoming(const std::string& topic,
                         const std::vector<std::uint8_t>& payload) {
        try {
            auto wire = decode_session_wire_message(payload);
            if (wire.topic != topic) return;
            record(wire, payload);
            if (wire.kind == SessionWireKind::EVENT) {
                if (!wire.metadata.correlationId) return;
                PendingValue value = wire.payloadType == "ack-v1"
                                         ? PendingValue{decode_ack_payload(wire)}
                                         : PendingValue{decode_session_descriptor_payload(wire)};
                std::shared_ptr<Pending> pending;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    const auto found = pending_.find(*wire.metadata.correlationId);
                    if (found == pending_.end()) return;
                    pending = found->second;
                }
                {
                    std::lock_guard<std::mutex> lock(pending->mutex);
                    if (pending->value) return;
                    pending->value = std::move(value);
                }
                pending->condition.notify_all();
            } else if (wire.kind == SessionWireKind::SERVICE_UPDATE ||
                       wire.kind == SessionWireKind::PCV_RESPONSE) {
                const auto response = decode_service_response_payload(wire);
                if (wire.metadata.sessionId) {
                    (void)deliver_service_update(*wire.metadata.sessionId, response);
                }
            }
        } catch (...) {
            // Malformed/unmatched broker input is deliberately not delivered.
        }
    }

    void record(const SessionWireMessage& wire,
                const std::vector<std::uint8_t>& encoded) {
        SessionPublication publication;
        publication.topic = SessionTopic::parse(wire.topic);
        publication.metadata = wire.metadata;
        publication.payloadType = wire.payloadType;
        publication.payloadSize = wire.payload.size();
        publication.encodedPayload = encoded;
        std::lock_guard<std::mutex> lock(mutex_);
        publications_.push_back(std::move(publication));
    }

    std::shared_ptr<SessionMessageBroker> broker_;
    std::function<Timestamp()> wallClock_;
    mutable SessionLifecycle lifecycle_;
    std::chrono::milliseconds responseTimeout_;
    std::vector<std::uint64_t> subscriptionIds_;
    std::atomic<std::uint64_t> messageCounter_{0};
    std::atomic<std::uint64_t> sessionCounter_{0};
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<Pending>> pending_;
    std::unordered_map<std::string,
        std::vector<Envelope<VehicleServiceResponse>>> responses_;
    std::vector<SessionPublication> publications_;
};

class BrokerSessionEndpoint final
    : public SessionBrokerEndpoint,
      public std::enable_shared_from_this<BrokerSessionEndpoint> {
public:
    BrokerSessionEndpoint(std::shared_ptr<SessionMessageBroker> broker,
                          std::shared_ptr<ReceiverApi> receiver,
                          std::shared_ptr<SenderApi> sender,
                          SessionTransportOptions options)
        : broker_(std::move(broker)),
          receiver_(std::move(receiver)),
          sender_(std::move(sender)),
          wallClock_(options.wallClock ? options.wallClock
                                       : [] { return std::chrono::system_clock::now(); }),
          lifecycle_(std::move(options)) {
        if (!broker_ || !receiver_ || !sender_) {
            throw std::invalid_argument("broker endpoint requires broker, receiver, and sender");
        }
    }

    ~BrokerSessionEndpoint() override {
        if (subscriptionId_) broker_->unsubscribe(*subscriptionId_);
    }

    void start() {
        const auto weak = weak_from_this();
        subscriptionId_ = broker_->subscribe(
            "ipi/+/session/#",
            [weak](const std::string& topic, const std::vector<std::uint8_t>& payload) {
                if (const auto self = weak.lock()) self->handle(topic, payload);
            });
    }

private:
    void handle(const std::string& topic, const std::vector<std::uint8_t>& payload) {
        std::optional<SessionWireMessage> wire;
        try {
            wire = decode_session_wire_message(payload);
            if (wire->topic != topic) return;
            switch (wire->kind) {
                case SessionWireKind::REGISTER: handle_registration(*wire); break;
                case SessionWireKind::HEARTBEAT: handle_heartbeat(*wire); break;
                case SessionWireKind::PATCH: handle_patch(*wire); break;
                case SessionWireKind::TERMINATE: handle_termination(*wire); break;
                case SessionWireKind::SERVICE_REQUEST: handle_service(*wire); break;
                case SessionWireKind::TELEMETRY: handle_telemetry(*wire); break;
                default: break;
            }
        } catch (const std::exception& error) {
            if (wire) publish_error(*wire, FailureCode::PROTOCOL_ERROR, error.what());
        }
    }

    void handle_registration(const SessionWireMessage& wire) {
        const auto registration = decode_session_registration_payload(wire);
        try {
            const auto descriptor = lifecycle_.register_session(registration);
            auto receiverRegistration = registration;
            receiverRegistration.metadata.sessionId = descriptor.sessionId;
            (void)receiver_->registerSession(receiverRegistration);
            auto metadata = response_metadata(wire, descriptor.sessionId);
            const auto topic = SessionTopic{SessionTopicKind::Events,
                                            wire.metadata.intersectionId,
                                            std::nullopt,
                                            descriptor.vehicleProfile.vehicleId}.to_string();
            publish(make_session_descriptor_wire_message(
                topic, std::move(metadata), descriptor));
        } catch (const SessionTransportException& error) {
            publish_error(wire, error.code(), error.what(), registration.vehicleProfile.vehicleId);
        } catch (const std::exception& error) {
            publish_error(wire, FailureCode::INVALID_REQUEST, error.what(),
                          registration.vehicleProfile.vehicleId);
        }
    }

    void handle_heartbeat(const SessionWireMessage& wire) {
        const auto heartbeatUpdate = decode_heartbeat_payload(wire);
        auto acknowledgement = lifecycle_.heartbeat(heartbeatUpdate);
        if (acknowledgement.accepted) acknowledgement = receiver_->heartbeat(heartbeatUpdate);
        publish_ack(wire, acknowledgement, heartbeatUpdate.sessionId);
    }

    void handle_patch(const SessionWireMessage& wire) {
        const auto patch = decode_session_patch_payload(wire);
        auto acknowledgement = lifecycle_.patch_session(patch);
        if (acknowledgement.accepted) acknowledgement = receiver_->patchSession(patch);
        publish_ack(wire, acknowledgement, patch.sessionId);
    }

    void handle_termination(const SessionWireMessage& wire) {
        const auto termination = decode_session_termination_payload(wire);
        auto acknowledgement = lifecycle_.terminate_session(termination);
        if (acknowledgement.accepted) acknowledgement = receiver_->terminateSession(termination);
        publish_ack(wire, acknowledgement, termination.sessionId);
    }

    void handle_service(const SessionWireMessage& wire) {
        const auto invocation = decode_service_invocation_payload(wire);
        auto acknowledgement = lifecycle_.authorize_service(invocation);
        if (acknowledgement.accepted) acknowledgement = receiver_->invokeService(invocation);
        if (acknowledgement.accepted) {
            acknowledgement.id = invocation.request.metadata.correlationId;
        }
        publish_ack(wire, acknowledgement, invocation.sessionId);
        if (!acknowledgement.accepted) return;

        const auto snapshot = lifecycle_.get_snapshot(invocation.sessionId);
        if (!snapshot) return;
        const auto responses = sender_->listSessionResponses(
            SessionResponseQuery{invocation.sessionId});
        for (const auto& response : responses) {
            if (response.metadata.correlationId != invocation.request.metadata.correlationId) continue;
            auto accepted = lifecycle_.accept_service_response(invocation.sessionId, response);
            if (!accepted.accepted) continue;
            const auto topic = service_response_topic(
                *snapshot, invocation.sessionId, response).to_string();
            publish(make_service_response_wire_message(topic, response));
        }
    }

    void handle_telemetry(const SessionWireMessage& wire) {
        const auto telemetry = decode_telemetry_payload(wire);
        auto acknowledgement = lifecycle_.authorize_telemetry(telemetry);
        if (acknowledgement.accepted) acknowledgement = receiver_->submitTelemetry(telemetry);
        publish_ack(wire, acknowledgement, telemetry.sessionId);
    }

    EnvelopeMetadata response_metadata(const SessionWireMessage& request,
                                       const std::optional<std::string>& sessionId) {
        EnvelopeMetadata metadata;
        metadata.messageId = next_message_id();
        metadata.sentAt = wallClock_();
        metadata.intersectionId = request.metadata.intersectionId;
        metadata.transport = request.metadata.transport;
        metadata.source.type = SourceType::INFRASTRUCTURE;
        metadata.source.id = request.metadata.intersectionId;
        metadata.sessionId = sessionId;
        metadata.correlationId = request.metadata.messageId;
        return metadata;
    }

    void publish_ack(const SessionWireMessage& request,
                     Ack acknowledgement,
                     const std::string& sessionId) {
        const auto snapshot = lifecycle_.get_snapshot(sessionId);
        const auto vehicleId = snapshot ? snapshot->descriptor.vehicleProfile.vehicleId
                                        : request.metadata.source.id;
        if (acknowledgement.accepted && acknowledgement.code != FailureCode::NONE) {
            acknowledgement.code = FailureCode::NONE;
        }
        if (!acknowledgement.accepted && acknowledgement.code == FailureCode::NONE) {
            acknowledgement.code = FailureCode::SERVICE_UNAVAILABLE;
        }
        auto metadata = response_metadata(request, sessionId.empty()
                                                       ? std::optional<std::string>{}
                                                       : std::optional<std::string>{sessionId});
        const auto topic = SessionTopic{SessionTopicKind::Events,
                                        request.metadata.intersectionId,
                                        std::nullopt, vehicleId}.to_string();
        publish(make_ack_wire_message(topic, std::move(metadata), acknowledgement));
    }

    void publish_error(const SessionWireMessage& request,
                       FailureCode code,
                       const std::string& detail,
                       std::string vehicleId = {}) {
        if (vehicleId.empty()) vehicleId = request.metadata.source.id;
        if (vehicleId.empty()) return;
        const auto sessionId = request.metadata.sessionId.value_or("");
        publish_ack(request, reject(code, detail), sessionId);
    }

    void publish(const SessionWireMessage& wire) {
        broker_->publish(wire.topic, encode_session_wire_message(wire));
    }

    std::string next_message_id() {
        std::ostringstream out;
        out << "broker-endpoint-message-" << ++messageCounter_;
        return out.str();
    }

    std::shared_ptr<SessionMessageBroker> broker_;
    std::shared_ptr<ReceiverApi> receiver_;
    std::shared_ptr<SenderApi> sender_;
    std::function<Timestamp()> wallClock_;
    SessionLifecycle lifecycle_;
    std::optional<std::uint64_t> subscriptionId_;
    std::atomic<std::uint64_t> messageCounter_{0};
};

} // namespace

std::shared_ptr<SessionMessageBroker> make_mqtt_session_message_broker(
    MqttSessionBrokerConfig config) {
    return std::make_shared<MqttSessionMessageBroker>(std::move(config));
}

std::shared_ptr<PrivateSessionTransport> make_broker_private_session_transport(
    std::shared_ptr<SessionMessageBroker> broker,
    SessionTransportOptions options,
    std::chrono::milliseconds responseTimeout) {
    auto transport = std::make_shared<BrokerPrivateSessionTransport>(
        std::move(broker), std::move(options), responseTimeout);
    transport->start();
    return transport;
}

std::shared_ptr<SessionBrokerEndpoint> make_broker_session_endpoint(
    std::shared_ptr<SessionMessageBroker> broker,
    std::shared_ptr<ReceiverApi> receiver,
    std::shared_ptr<SenderApi> sender,
    SessionTransportOptions options) {
    if (!receiver && !sender) {
        auto pair = make_in_memory_api_pair();
        receiver = std::move(pair.receiver);
        sender = std::move(pair.sender);
    } else if (!receiver || !sender) {
        throw std::invalid_argument(
            "provide both endpoint receiver and sender, or neither");
    }
    auto endpoint = std::make_shared<BrokerSessionEndpoint>(
        std::move(broker), std::move(receiver), std::move(sender), std::move(options));
    endpoint->start();
    return endpoint;
}

} // namespace ipi::api
