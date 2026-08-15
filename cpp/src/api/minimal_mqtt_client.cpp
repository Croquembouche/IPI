#include "ipi/api/minimal_mqtt_client.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace ipi::api {

namespace {

std::uint16_t read_uint16(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 2 > buffer.size()) {
        throw std::runtime_error("mqtt buffer underrun");
    }
    const auto hi = static_cast<std::uint16_t>(buffer[offset++]);
    const auto lo = static_cast<std::uint16_t>(buffer[offset++]);
    return static_cast<std::uint16_t>((hi << 8) | lo);
}

void append_uint16(std::vector<std::uint8_t>& buffer, std::uint16_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void append_string(std::vector<std::uint8_t>& buffer, const std::string& value) {
    if (value.size() > 65535U) {
        throw std::invalid_argument("mqtt string field exceeds 65535 bytes");
    }
    append_uint16(buffer, static_cast<std::uint16_t>(value.size()));
    buffer.insert(buffer.end(), value.begin(), value.end());
}

std::string read_string(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    const auto length = read_uint16(buffer, offset);
    if (offset + length > buffer.size()) {
        throw std::runtime_error("mqtt string exceeds buffer");
    }
    const auto begin = buffer.begin() + static_cast<std::ptrdiff_t>(offset);
    const auto end = begin + static_cast<std::ptrdiff_t>(length);
    offset += length;
    return std::string(begin, end);
}

void append_remaining_length(std::vector<std::uint8_t>& buffer, std::size_t value) {
    do {
        std::uint8_t encoded = static_cast<std::uint8_t>(value % 128U);
        value /= 128U;
        if (value > 0U) {
            encoded |= 0x80U;
        }
        buffer.push_back(encoded);
    } while (value > 0U);
}

std::size_t read_remaining_length(int socketFd) {
    std::size_t multiplier = 1;
    std::size_t value = 0;
    std::uint8_t encoded = 0;
    for (int i = 0; i < 4; ++i) {
        const auto received = ::recv(socketFd, &encoded, sizeof(encoded), MSG_WAITALL);
        if (received <= 0) {
            throw std::runtime_error("mqtt recv() failed while reading remaining length");
        }
        value += static_cast<std::size_t>(encoded & 0x7FU) * multiplier;
        if ((encoded & 0x80U) == 0) {
            return value;
        }
        multiplier *= 128U;
    }
    throw std::runtime_error("mqtt remaining length exceeds 4 bytes");
}

void send_all(int socketFd, const std::vector<std::uint8_t>& packet) {
    std::size_t offset = 0;
    while (offset < packet.size()) {
        const auto sent = ::send(
            socketFd, packet.data() + offset, packet.size() - offset, MSG_NOSIGNAL);
        if (sent <= 0) {
            throw std::runtime_error("mqtt send() failed");
        }
        offset += static_cast<std::size_t>(sent);
    }
}

std::vector<std::uint8_t> recv_exact(int socketFd, std::size_t size) {
    std::vector<std::uint8_t> buffer(size);
    std::size_t offset = 0;
    while (offset < size) {
        const auto received = ::recv(socketFd, buffer.data() + offset, size - offset, 0);
        if (received == 0) {
            throw std::runtime_error("mqtt peer closed");
        }
        if (received < 0) {
            throw std::runtime_error("mqtt recv() failed");
        }
        offset += static_cast<std::size_t>(received);
    }
    return buffer;
}

std::vector<std::uint8_t> make_packet(std::uint8_t header, const std::vector<std::uint8_t>& variablePayload) {
    std::vector<std::uint8_t> packet;
    packet.reserve(1 + 4 + variablePayload.size());
    packet.push_back(header);
    append_remaining_length(packet, variablePayload.size());
    packet.insert(packet.end(), variablePayload.begin(), variablePayload.end());
    return packet;
}

std::vector<std::uint8_t> make_connect_packet(const std::string& clientId,
                                              const MqttConnectOptions& options) {
    if (options.password && !options.username) {
        throw std::invalid_argument("mqtt password requires a username");
    }

    std::vector<std::uint8_t> payload;
    append_string(payload, "MQTT");
    payload.push_back(0x04U);

    std::uint8_t connectFlags = options.cleanSession ? 0x02U : 0x00U;
    if (options.username) {
        connectFlags |= 0x80U;
    }
    if (options.password) {
        connectFlags |= 0x40U;
    }
    payload.push_back(connectFlags);
    append_uint16(payload, options.keepAliveSeconds);
    append_string(payload, clientId);
    if (options.username) {
        append_string(payload, *options.username);
    }
    if (options.password) {
        append_string(payload, *options.password);
    }
    return make_packet(0x10U, payload);
}

std::vector<std::uint8_t> make_subscribe_packet(std::uint16_t packetId, const std::string& topicFilter) {
    std::vector<std::uint8_t> payload;
    append_uint16(payload, packetId);
    append_string(payload, topicFilter);
    payload.push_back(0x00U);
    return make_packet(0x82U, payload);
}

std::vector<std::uint8_t> make_publish_packet(const std::string& topic, const std::vector<std::uint8_t>& payload) {
    std::vector<std::uint8_t> variablePayload;
    append_string(variablePayload, topic);
    variablePayload.insert(variablePayload.end(), payload.begin(), payload.end());
    return make_packet(0x30U, variablePayload);
}

std::vector<std::uint8_t> make_disconnect_packet() {
    return {0xE0U, 0x00U};
}

std::uint8_t packet_type(std::uint8_t header) {
    return static_cast<std::uint8_t>((header >> 4) & 0x0FU);
}

int poll_timeout(std::chrono::milliseconds timeout) {
    const auto maximum = static_cast<long long>(std::numeric_limits<int>::max());
    return static_cast<int>(std::min<long long>(timeout.count(), maximum));
}

void set_socket_timeouts(int socketFd, std::chrono::milliseconds timeout) {
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timeout);
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        timeout - seconds);
    timeval value{};
    value.tv_sec = static_cast<decltype(value.tv_sec)>(seconds.count());
    value.tv_usec = static_cast<decltype(value.tv_usec)>(microseconds.count());
    if (::setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &value, sizeof(value)) != 0 ||
        ::setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &value, sizeof(value)) != 0) {
        throw std::runtime_error("mqtt setsockopt() timeout configuration failed");
    }
}

int connect_socket(const std::string& host,
                   std::uint16_t port,
                   std::chrono::milliseconds timeout) {
    if (timeout <= std::chrono::milliseconds::zero()) {
        throw std::invalid_argument("mqtt I/O timeout must be positive");
    }
    const int socketFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        throw std::runtime_error("mqtt socket() failed");
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (::inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1) {
        ::close(socketFd);
        throw std::runtime_error("mqtt inet_pton() failed");
    }

    const auto originalFlags = ::fcntl(socketFd, F_GETFL, 0);
    if (originalFlags < 0 || ::fcntl(socketFd, F_SETFL, originalFlags | O_NONBLOCK) != 0) {
        ::close(socketFd);
        throw std::runtime_error("mqtt fcntl() failed");
    }

    const auto connected = ::connect(
        socketFd, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (connected != 0 && errno != EINPROGRESS) {
        ::close(socketFd);
        throw std::runtime_error("mqtt connect() failed");
    }
    if (connected != 0) {
        pollfd descriptor{socketFd, POLLOUT, 0};
        const auto ready = ::poll(&descriptor, 1, poll_timeout(timeout));
        if (ready <= 0) {
            ::close(socketFd);
            throw std::runtime_error(ready == 0 ? "mqtt connect() timed out"
                                                : "mqtt connect poll() failed");
        }
        int socketError = 0;
        socklen_t errorLength = sizeof(socketError);
        if (::getsockopt(socketFd, SOL_SOCKET, SO_ERROR,
                         &socketError, &errorLength) != 0 || socketError != 0) {
            ::close(socketFd);
            throw std::runtime_error("mqtt connect() failed after poll");
        }
    }
    if (::fcntl(socketFd, F_SETFL, originalFlags) != 0) {
        ::close(socketFd);
        throw std::runtime_error("mqtt fcntl() could not restore blocking mode");
    }
    try {
        set_socket_timeouts(socketFd, timeout);
    } catch (...) {
        ::close(socketFd);
        throw;
    }
    return socketFd;
}

std::pair<std::uint8_t, std::vector<std::uint8_t>> recv_packet(int socketFd) {
    std::uint8_t header = 0;
    const auto received = ::recv(socketFd, &header, sizeof(header), MSG_WAITALL);
    if (received == 0) {
        throw std::runtime_error("mqtt peer closed");
    }
    if (received < 0) {
        throw std::runtime_error("mqtt recv() failed");
    }
    const auto remainingLength = read_remaining_length(socketFd);
    return {header, recv_exact(socketFd, remainingLength)};
}

} // namespace

MinimalMqttClient::MinimalMqttClient(std::string clientId)
    : clientId_(std::move(clientId)) {
    if (clientId_.empty()) {
        clientId_ = "ipi-client";
    }
}

MinimalMqttClient::~MinimalMqttClient() {
    try {
        disconnect();
    } catch (...) {
    }
}

MinimalMqttClient::MinimalMqttClient(MinimalMqttClient&& other) noexcept
    : socketFd_(other.socketFd_),
      clientId_(std::move(other.clientId_)),
      nextPacketId_(other.nextPacketId_) {
    other.socketFd_ = -1;
}

MinimalMqttClient& MinimalMqttClient::operator=(MinimalMqttClient&& other) noexcept {
    if (this != &other) {
        disconnect();
        socketFd_ = other.socketFd_;
        clientId_ = std::move(other.clientId_);
        nextPacketId_ = other.nextPacketId_;
        other.socketFd_ = -1;
    }
    return *this;
}

void MinimalMqttClient::connect(const std::string& host,
                                std::uint16_t port,
                                std::uint16_t keepAliveSeconds) {
    MqttConnectOptions options;
    options.keepAliveSeconds = keepAliveSeconds;
    connect(host, port, options);
}

void MinimalMqttClient::connect(const std::string& host,
                                std::uint16_t port,
                                const MqttConnectOptions& options) {
    const auto connectPacket = make_connect_packet(clientId_, options);
    disconnect();
    socketFd_ = connect_socket(host, port, options.ioTimeout);
    try {
        send_all(socketFd_, connectPacket);
        const auto [header, packet] = recv_packet(socketFd_);
        if (packet_type(header) != 2U || packet.size() != 2U) {
            throw std::runtime_error("mqtt expected CONNACK");
        }
        if (packet[1] != 0U) {
            throw std::runtime_error("mqtt broker rejected connection");
        }
    } catch (...) {
        disconnect();
        throw;
    }
}

void MinimalMqttClient::subscribe(const std::string& topicFilter) {
    ensure_connected();
    const auto packetId = next_packet_id();
    send_all(socketFd_, make_subscribe_packet(packetId, topicFilter));

    const auto [header, packet] = recv_packet(socketFd_);
    if (packet_type(header) != 9U || packet.size() < 3U) {
        throw std::runtime_error("mqtt expected SUBACK");
    }

    std::size_t offset = 0;
    const auto ackPacketId = read_uint16(packet, offset);
    if (ackPacketId != packetId) {
        throw std::runtime_error("mqtt SUBACK packet id mismatch");
    }
    if (packet[offset] == 0x80U) {
        throw std::runtime_error("mqtt broker rejected subscription");
    }
}

void MinimalMqttClient::publish(const std::string& topic, const std::vector<std::uint8_t>& payload) {
    ensure_connected();
    send_all(socketFd_, make_publish_packet(topic, payload));
}

void MinimalMqttClient::ping() {
    ensure_connected();
    send_all(socketFd_, {0xC0U, 0x00U});
}

std::optional<MqttMessage> MinimalMqttClient::receive(std::chrono::milliseconds timeout) {
    ensure_connected();

    const auto deadline = std::chrono::steady_clock::now() + timeout;
    for (;;) {
        const auto now = std::chrono::steady_clock::now();
        if (now >= deadline) {
            return std::nullopt;
        }

        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now);
        pollfd descriptor{};
        descriptor.fd = socketFd_;
        descriptor.events = POLLIN;
        const int ready = ::poll(&descriptor, 1, poll_timeout(remaining));
        if (ready == 0) {
            return std::nullopt;
        }
        if (ready < 0) {
            throw std::runtime_error("mqtt poll() failed");
        }

        const auto [header, packet] = recv_packet(socketFd_);
        switch (packet_type(header)) {
            case 3U: {
                std::size_t offset = 0;
                MqttMessage message;
                message.topic = read_string(packet, offset);
                message.payload.assign(packet.begin() + static_cast<std::ptrdiff_t>(offset), packet.end());
                return message;
            }
            case 13U:
                continue;
            default:
                continue;
        }
    }
}

void MinimalMqttClient::disconnect() {
    if (socketFd_ >= 0) {
        try {
            send_all(socketFd_, make_disconnect_packet());
        } catch (...) {
        }
        ::close(socketFd_);
        socketFd_ = -1;
    }
}

const std::string& MinimalMqttClient::client_id() const noexcept {
    return clientId_;
}

bool MinimalMqttClient::connected() const noexcept {
    return socketFd_ >= 0;
}

void MinimalMqttClient::ensure_connected() const {
    if (socketFd_ < 0) {
        throw std::runtime_error("mqtt client is not connected");
    }
}

std::uint16_t MinimalMqttClient::next_packet_id() {
    const auto id = nextPacketId_;
    ++nextPacketId_;
    if (nextPacketId_ == 0U) {
        nextPacketId_ = 1U;
    }
    return id;
}

} // namespace ipi::api
