#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ipi::api {

struct MqttMessage {
    std::string topic{};
    std::vector<std::uint8_t> payload{};
};

struct MqttConnectOptions {
    std::uint16_t keepAliveSeconds{30};
    bool cleanSession{true};
    std::optional<std::string> username{};
    std::optional<std::string> password{};
    std::chrono::milliseconds ioTimeout{std::chrono::seconds(5)};
};

class MinimalMqttClient {
public:
    explicit MinimalMqttClient(std::string clientId = {});
    ~MinimalMqttClient();

    MinimalMqttClient(const MinimalMqttClient&) = delete;
    MinimalMqttClient& operator=(const MinimalMqttClient&) = delete;

    MinimalMqttClient(MinimalMqttClient&& other) noexcept;
    MinimalMqttClient& operator=(MinimalMqttClient&& other) noexcept;

    void connect(const std::string& host,
                 std::uint16_t port,
                 std::uint16_t keepAliveSeconds = 30);

    void connect(const std::string& host,
                 std::uint16_t port,
                 const MqttConnectOptions& options);

    void subscribe(const std::string& topicFilter);
    void publish(const std::string& topic, const std::vector<std::uint8_t>& payload);
    void ping();

    [[nodiscard]] std::optional<MqttMessage> receive(std::chrono::milliseconds timeout);

    void disconnect();

    [[nodiscard]] const std::string& client_id() const noexcept;
    [[nodiscard]] bool connected() const noexcept;

private:
    int socketFd_{-1};
    std::string clientId_{};
    std::uint16_t nextPacketId_{1};

    void ensure_connected() const;
    [[nodiscard]] std::uint16_t next_packet_id();
};

} // namespace ipi::api
