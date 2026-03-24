#include "test_support.hpp"

#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <exception>
#include <thread>

namespace {

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
    if (::listen(serverFd, 1) != 0) {
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

int connect_loopback(std::uint16_t port) {
    const int clientFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd < 0) {
        throw std::runtime_error("socket() failed");
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (::connect(clientFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        ::close(clientFd);
        throw std::runtime_error("connect() failed");
    }
    return clientFd;
}

} // namespace

int main() {
    return ipi::tests::run_test("private_5g_latency_loopback", [] {
        std::uint16_t port = 0;
        const int serverFd = create_server_socket(port);
        std::exception_ptr serverFailure;

        std::thread server([&] {
            try {
                ipi::v2x::UperCodec codec;
                const int clientFd = ::accept(serverFd, nullptr, nullptr);
                if (clientFd < 0) {
                    throw std::runtime_error("accept() failed");
                }

                const auto encodedRequest = ipi::api::recv_private_5g_probe_packet(clientFd);
                const auto serverReceiveTimeNs = ipi::api::current_unix_time_ns();
                const auto request = ipi::api::decode_private_5g_probe_request(encodedRequest);
                const auto detail = ipi::api::inspect_private_5g_probe_frame(request.frame, codec);

                ipi::api::Private5gProbeAck ack;
                ack.sequence = request.sequence;
                ack.clientSendTimeNs = request.clientSendTimeNs;
                ack.serverReceiveTimeNs = serverReceiveTimeNs;
                ack.serverSendTimeNs = ipi::api::current_unix_time_ns();
                ack.frameType = request.frame.type;
                ack.payloadSize = static_cast<std::uint32_t>(request.frame.payload.size());
                ack.accepted = true;
                ack.detail = detail;

                ipi::api::send_private_5g_probe_packet(clientFd, ipi::api::encode_private_5g_probe_ack(ack));
                ::close(clientFd);
            } catch (...) {
                serverFailure = std::current_exception();
            }
        });

        const int clientFd = connect_loopback(port);
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

        ipi::api::send_private_5g_probe_packet(clientFd, ipi::api::encode_private_5g_probe_request(request));
        const auto encodedAck = ipi::api::recv_private_5g_probe_packet(clientFd);
        const auto clientReceiveTimeNs = ipi::api::current_unix_time_ns();
        const auto ack = ipi::api::decode_private_5g_probe_ack(encodedAck);
        const auto metrics = ipi::api::compute_private_5g_latency_metrics(ack, clientReceiveTimeNs);

        ::close(clientFd);
        server.join();
        ::close(serverFd);

        if (serverFailure) {
            std::rethrow_exception(serverFailure);
        }

        ipi::tests::expect(ack.accepted, "loopback ack should be accepted");
        ipi::tests::expect(ack.sequence == request.sequence, "loopback ack should preserve sequence");
        ipi::tests::expect(ack.frameType == ipi::MessageType::SPAT, "loopback ack should preserve frame type");
        ipi::tests::expect(metrics.roundTripNs >= 0, "loopback RTT should be non-negative");
        ipi::tests::expect(metrics.serverProcessingNs >= 0, "server processing time should be non-negative");
        ipi::tests::expect(metrics.uplinkNs.has_value(), "loopback should provide uplink timing");
        ipi::tests::expect(metrics.downlinkNs.has_value(), "loopback should provide downlink timing");
    });
}
