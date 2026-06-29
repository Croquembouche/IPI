#include "ipi/api/experiment_logging.hpp"
#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::uint16_t kDefaultPort = 36667;
constexpr std::size_t kMaxUdpPayload = 65507;

struct Args {
    std::uint16_t port{kDefaultPort};
    ipi::api::ExperimentContext context{};
    bool once{false};
    bool quiet{false};
    bool csv{false};
};

Args parse_args(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout
                << "Usage: " << argv[0] << " [options]\n"
                << "  --port <port>                UDP listen port (default 36667)\n"
                << "  --run-id <id>                Receiver-side default run id\n"
                << "  --condition-id <id>          Receiver-side default condition id\n"
                << "  --condition-label <label>    Receiver-side default condition label\n"
                << "  --av-id <id>                 AV identifier override for logging\n"
                << "  --obu-id <id>                OBU identifier override for logging\n"
                << "  --rsu-id <id>                RSU identifier for logging\n"
                << "  --network-load-level <id>    idle|moderate|heavy|near-saturation\n"
                << "  --qos-profile <id>           FIFO, default, 5qi-mapped, etc.\n"
                << "  --mobility-state <id>        stationary, moving, handover, etc.\n"
                << "  --clock-sync-state <id>      ptp-synced, ntp-synced, unsynced\n"
                << "  --service-success <bool>     Infrastructure-side service success annotation\n"
                << "  --vehicle-outcome-name <s>   Driving metric name\n"
                << "  --vehicle-outcome-value <v>  Driving metric value\n"
                << "  --vehicle-outcome-unit <u>   Driving metric unit\n"
                << "  --once                       Exit after one successful probe\n"
                << "  --quiet                      Suppress per-probe logging\n"
                << "  --csv                        Emit structured CSV rows\n";
            std::exit(0);
        }
        if ((arg == "--port" || arg == "-P") && i + 1 < argc) {
            args.port = static_cast<std::uint16_t>(std::stoul(argv[++i]));
        } else if (arg == "--once") {
            args.once = true;
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (arg == "--csv") {
            args.csv = true;
        } else if (ipi::api::consume_experiment_context_arg(args.context, arg, i, argc, argv)) {
            continue;
        } else {
            throw std::invalid_argument("unknown argument: " + std::string(arg));
        }
    }
    if (args.context.conditionLabel.empty()) {
        args.context.conditionLabel =
            ipi::api::default_condition_label("udp", args.context.networkLoadLevel);
    }
    return args;
}

int create_socket(std::uint16_t port) {
    const int socketFd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0) {
        throw std::runtime_error("socket() failed");
    }

    int reuse = 1;
    if (::setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
        ::close(socketFd);
        throw std::runtime_error("setsockopt() failed");
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(socketFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        ::close(socketFd);
        throw std::runtime_error("bind() failed");
    }
    return socketFd;
}

ipi::api::Private5gProbeAck handle_request(const ipi::api::Private5gProbeRequest& request,
                                           ipi::v2x::UperCodec& codec) {
    ipi::api::Private5gProbeAck ack;
    ack.sequence = request.sequence;
    ack.clientSendTimeNs = request.clientSendTimeNs;
    ack.serverReceiveTimeNs = ipi::api::current_unix_time_ns();
    ack.frameType = request.frame.type;
    ack.payloadSize = static_cast<std::uint32_t>(request.frame.payload.size());

    try {
        ack.detail = ipi::api::inspect_private_5g_probe_frame(request.frame, codec);
        ack.accepted = true;
    } catch (const std::exception& ex) {
        ack.detail = ex.what();
        ack.accepted = false;
    }

    ack.serverSendTimeNs = ipi::api::current_unix_time_ns();
    return ack;
}

void log_ack(const Args& args,
             const ipi::api::Private5gProbeRequest& request,
             const ipi::api::Private5gProbeAck& ack) {
    if (args.quiet) {
        return;
    }

    ipi::api::ExperimentLogRecord record;
    record.emitterRole = "5g-udp-receiver";
    record.emitTimeNs = ack.serverSendTimeNs;
    record.runId = request.runId.empty() ? args.context.runId : request.runId;
    record.conditionId = request.conditionId.empty() ? args.context.conditionId : request.conditionId;
    record.conditionLabel = request.conditionLabel.empty() ? args.context.conditionLabel : request.conditionLabel;
    record.serviceType = request.serviceType.empty() ? "unknown" : request.serviceType;
    record.transport = "udp";
    record.avId = args.context.avId;
    record.obuId = args.context.obuId.empty() ? request.sourceId : args.context.obuId;
    record.rsuId = args.context.rsuId;
    record.requestId = request.requestId;
    record.sessionId = request.sessionId.value_or(std::string{});
    record.intersectionId = request.intersectionId;
    record.sourceId = request.sourceId;
    record.sequence = request.sequence;
    record.networkLoadLevel =
        request.networkLoadLevel.empty() ? args.context.networkLoadLevel : request.networkLoadLevel;
    record.qosProfile = request.qosProfile.empty() ? args.context.qosProfile : request.qosProfile;
    record.mobilityState =
        request.mobilityState.empty() ? args.context.mobilityState : request.mobilityState;
    record.clockSyncState =
        request.clockSyncState.empty() ? args.context.clockSyncState : request.clockSyncState;
    record.accepted = ack.accepted;
    record.serviceSuccess = ack.accepted && args.context.serviceSuccess;
    record.vehicleOutcomeName = args.context.vehicleOutcomeName;
    record.vehicleOutcomeValue = args.context.vehicleOutcomeValue;
    record.vehicleOutcomeUnit = args.context.vehicleOutcomeUnit;
    record.frameType = ipi::to_string(ack.frameType);
    record.payloadBytes = ack.payloadSize;
    record.clientSendTimeNs = request.clientSendTimeNs;
    record.serverReceiveTimeNs = ack.serverReceiveTimeNs;
    record.serverSendTimeNs = ack.serverSendTimeNs;
    record.detail = ack.detail;

    if (args.csv) {
        std::cout << ipi::api::experiment_log_to_csv(record) << '\n';
    } else {
        std::cout << ipi::api::experiment_log_to_text(record) << '\n';
    }
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Args args = parse_args(argc, argv);
        if (args.csv) {
            std::cout << ipi::api::experiment_log_csv_header() << '\n';
        }

        const int socketFd = create_socket(args.port);
        std::cerr << "private 5G latency receiver listening on port " << args.port << " over udp\n";

        ipi::v2x::UperCodec codec;
        std::array<std::uint8_t, kMaxUdpPayload> buffer{};

        for (;;) {
            sockaddr_in peer{};
            socklen_t peerLen = sizeof(peer);
            const ssize_t received = ::recvfrom(socketFd,
                                                buffer.data(),
                                                buffer.size(),
                                                0,
                                                reinterpret_cast<sockaddr*>(&peer),
                                                &peerLen);
            if (received < 0) {
                continue;
            }
            try {
                const std::vector<std::uint8_t> encodedRequest(buffer.begin(), buffer.begin() + received);
                const auto request = ipi::api::decode_private_5g_probe_request(encodedRequest);
                const auto ack = handle_request(request, codec);
                const auto encodedAck = ipi::api::encode_private_5g_probe_ack(ack);
                static_cast<void>(::sendto(socketFd,
                                           encodedAck.data(),
                                           encodedAck.size(),
                                           0,
                                           reinterpret_cast<sockaddr*>(&peer),
                                           peerLen));
                log_ack(args, request, ack);
                if (args.once) {
                    break;
                }
            } catch (const std::exception& ex) {
                std::cerr << "udp request error: " << ex.what() << '\n';
            }
        }
        ::close(socketFd);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
