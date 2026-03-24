#include "test_support.hpp"

#include "ipi/api/minimal_mqtt_client.hpp"
#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <exception>
#include <thread>

namespace {

std::uint16_t read_uint16(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    if (offset + 2 > buffer.size()) {
        throw std::runtime_error("mqtt test buffer underrun");
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
    append_uint16(buffer, static_cast<std::uint16_t>(value.size()));
    buffer.insert(buffer.end(), value.begin(), value.end());
}

std::string read_string(const std::vector<std::uint8_t>& buffer, std::size_t& offset) {
    const auto length = read_uint16(buffer, offset);
    if (offset + length > buffer.size()) {
        throw std::runtime_error("mqtt test string overflow");
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
            throw std::runtime_error("mqtt test recv remaining length failed");
        }
        value += static_cast<std::size_t>(encoded & 0x7FU) * multiplier;
        if ((encoded & 0x80U) == 0) {
            return value;
        }
        multiplier *= 128U;
    }
    throw std::runtime_error("mqtt test remaining length too large");
}

std::vector<std::uint8_t> recv_exact(int socketFd, std::size_t size) {
    std::vector<std::uint8_t> buffer(size);
    std::size_t offset = 0;
    while (offset < size) {
        const auto received = ::recv(socketFd, buffer.data() + offset, size - offset, 0);
        if (received <= 0) {
            throw std::runtime_error("mqtt test recv failed");
        }
        offset += static_cast<std::size_t>(received);
    }
    return buffer;
}

void send_packet(int socketFd, std::uint8_t header, const std::vector<std::uint8_t>& body) {
    std::vector<std::uint8_t> packet;
    packet.push_back(header);
    append_remaining_length(packet, body.size());
    packet.insert(packet.end(), body.begin(), body.end());

    std::size_t offset = 0;
    while (offset < packet.size()) {
        const auto sent = ::send(socketFd, packet.data() + offset, packet.size() - offset, 0);
        if (sent <= 0) {
            throw std::runtime_error("mqtt test send failed");
        }
        offset += static_cast<std::size_t>(sent);
    }
}

std::pair<std::uint8_t, std::vector<std::uint8_t>> recv_packet(int socketFd) {
    std::uint8_t header = 0;
    const auto received = ::recv(socketFd, &header, sizeof(header), MSG_WAITALL);
    if (received <= 0) {
        throw std::runtime_error("mqtt test recv header failed");
    }
    const auto remainingLength = read_remaining_length(socketFd);
    return {header, recv_exact(socketFd, remainingLength)};
}

std::vector<std::uint8_t> make_publish_body(const std::string& topic, const std::vector<std::uint8_t>& payload) {
    std::vector<std::uint8_t> body;
    append_string(body, topic);
    body.insert(body.end(), payload.begin(), payload.end());
    return body;
}

int create_server_socket(std::uint16_t& outPort) {
    const int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        throw std::runtime_error("socket() failed");
    }

    int reuse = 1;
    if (::setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
        ::close(serverFd);
        throw std::runtime_error("setsockopt() failed");
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;

    if (::bind(serverFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        ::close(serverFd);
        throw std::runtime_error("bind() failed");
    }
    if (::listen(serverFd, 2) != 0) {
        ::close(serverFd);
        throw std::runtime_error("listen() failed");
    }

    sockaddr_in bound{};
    socklen_t boundLen = sizeof(bound);
    if (::getsockname(serverFd, reinterpret_cast<sockaddr*>(&bound), &boundLen) != 0) {
        ::close(serverFd);
        throw std::runtime_error("getsockname() failed");
    }

    outPort = ntohs(bound.sin_port);
    return serverFd;
}

void expect_subscribe(int clientFd, const std::string& topic) {
    const auto [header, packet] = recv_packet(clientFd);
    ipi::tests::expect((header >> 4) == 8U, "expected SUBSCRIBE packet");

    std::size_t offset = 0;
    const auto packetId = read_uint16(packet, offset);
    const auto subscribedTopic = read_string(packet, offset);
    ipi::tests::expect(subscribedTopic == topic, "subscription topic should match expected topic");

    std::vector<std::uint8_t> subackBody;
    append_uint16(subackBody, packetId);
    subackBody.push_back(0x00U);
    send_packet(clientFd, 0x90U, subackBody);
}

std::vector<std::uint8_t> expect_publish(int clientFd, const std::string& topic) {
    const auto [header, packet] = recv_packet(clientFd);
    ipi::tests::expect((header >> 4) == 3U, "expected PUBLISH packet");

    std::size_t offset = 0;
    const auto publishedTopic = read_string(packet, offset);
    ipi::tests::expect(publishedTopic == topic, "published topic should match expected topic");
    return std::vector<std::uint8_t>(packet.begin() + static_cast<std::ptrdiff_t>(offset), packet.end());
}

} // namespace

int main() {
    return ipi::tests::run_test("private_5g_latency_mqtt_loopback", [] {
        const std::string requestTopic = "ipi/int-1/latency/veh-1/request";
        const std::string ackTopic = "ipi/int-1/latency/veh-1/ack";

        std::uint16_t port = 0;
        const int serverFd = create_server_socket(port);

        std::exception_ptr brokerFailure;
        std::exception_ptr receiverFailure;

        std::thread broker([&] {
            int receiverFd = -1;
            int senderFd = -1;
            try {
                receiverFd = ::accept(serverFd, nullptr, nullptr);
                if (receiverFd < 0) {
                    throw std::runtime_error("accept(receiver) failed");
                }
                auto [header, _] = recv_packet(receiverFd);
                ipi::tests::expect((header >> 4) == 1U, "receiver should CONNECT first");
                send_packet(receiverFd, 0x20U, std::vector<std::uint8_t>{0x00U, 0x00U});
                expect_subscribe(receiverFd, requestTopic);

                senderFd = ::accept(serverFd, nullptr, nullptr);
                if (senderFd < 0) {
                    throw std::runtime_error("accept(sender) failed");
                }
                auto [senderHeader, __] = recv_packet(senderFd);
                ipi::tests::expect((senderHeader >> 4) == 1U, "sender should CONNECT second");
                send_packet(senderFd, 0x20U, std::vector<std::uint8_t>{0x00U, 0x00U});
                expect_subscribe(senderFd, ackTopic);

                const auto requestPayload = expect_publish(senderFd, requestTopic);
                send_packet(receiverFd, 0x30U, make_publish_body(requestTopic, requestPayload));

                const auto ackPayload = expect_publish(receiverFd, ackTopic);
                send_packet(senderFd, 0x30U, make_publish_body(ackTopic, ackPayload));

                ::close(receiverFd);
                ::close(senderFd);
            } catch (...) {
                if (receiverFd >= 0) {
                    ::close(receiverFd);
                }
                if (senderFd >= 0) {
                    ::close(senderFd);
                }
                brokerFailure = std::current_exception();
            }
        });

        std::thread receiver([&] {
            try {
                ipi::api::MinimalMqttClient client("receiver");
                client.connect("127.0.0.1", port);
                client.subscribe(requestTopic);

                const auto message = client.receive(std::chrono::seconds(2));
                ipi::tests::expect(message.has_value(), "receiver should get published request");
                ipi::tests::expect(message->topic == requestTopic, "receiver topic should match request topic");

                ipi::v2x::UperCodec codec;
                const auto request = ipi::api::decode_private_5g_probe_request(message->payload);

                ipi::api::Private5gProbeAck ack;
                ack.sequence = request.sequence;
                ack.clientSendTimeNs = request.clientSendTimeNs;
                ack.serverReceiveTimeNs = ipi::api::current_unix_time_ns();
                ack.serverSendTimeNs = ipi::api::current_unix_time_ns();
                ack.frameType = request.frame.type;
                ack.payloadSize = static_cast<std::uint32_t>(request.frame.payload.size());
                ack.accepted = true;
                ack.detail = ipi::api::inspect_private_5g_probe_frame(request.frame, codec);

                client.publish(ackTopic, ipi::api::encode_private_5g_probe_ack(ack));
                client.disconnect();
            } catch (...) {
                receiverFailure = std::current_exception();
            }
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        ipi::api::MinimalMqttClient sender("sender");
        sender.connect("127.0.0.1", port);
        sender.subscribe(ackTopic);

        ipi::v2x::UperCodec codec;
        ipi::j2735::SpatMessage spat;
        spat.intersectionId = 101;
        spat.timestampMs = 123456;
        spat.phases = {{1, ipi::j2735::MovementPhaseState::Proceed, static_cast<std::uint16_t>(5000)}};

        ipi::api::Private5gProbeRequest request;
        request.sequence = 1;
        request.clientSendTimeNs = ipi::api::current_unix_time_ns();
        request.intersectionId = "int-1";
        request.sourceId = "veh-1";
        request.frame = ipi::api::make_private_5g_probe_frame(spat, codec);

        sender.publish(requestTopic, ipi::api::encode_private_5g_probe_request(request));
        const auto message = sender.receive(std::chrono::seconds(2));
        ipi::tests::expect(message.has_value(), "sender should get published ack");
        ipi::tests::expect(message->topic == ackTopic, "sender topic should match ack topic");

        const auto clientReceiveTimeNs = ipi::api::current_unix_time_ns();
        const auto ack = ipi::api::decode_private_5g_probe_ack(message->payload);
        const auto metrics = ipi::api::compute_private_5g_latency_metrics(ack, clientReceiveTimeNs);

        sender.disconnect();
        receiver.join();
        broker.join();
        ::close(serverFd);

        if (receiverFailure) {
            std::rethrow_exception(receiverFailure);
        }
        if (brokerFailure) {
            std::rethrow_exception(brokerFailure);
        }

        ipi::tests::expect(ack.accepted, "mqtt loopback ack should be accepted");
        ipi::tests::expect(ack.frameType == ipi::MessageType::SPAT, "mqtt loopback should preserve frame type");
        ipi::tests::expect(metrics.roundTripNs >= 0, "mqtt loopback RTT should be non-negative");
    });
}
