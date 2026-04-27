#include "ipi/api/experiment_logging.hpp"
#include "ipi/api/minimal_mqtt_client.hpp"
#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

constexpr std::uint16_t kDefaultPort = 36666;

enum class TransportKind {
    Tcp,
    Mqtt
};

struct Args {
    std::string host{"127.0.0.1"};
    std::uint16_t port{kDefaultPort};
    TransportKind transport{TransportKind::Tcp};
    std::string intersectionId{"intersection-101"};
    std::string sourceId{"veh-01"};
    ipi::api::ExperimentContext context{};
    bool once{false};
    bool quiet{false};
    bool csv{false};
};

std::string_view transport_name(TransportKind kind) {
    switch (kind) {
        case TransportKind::Mqtt:
            return "mqtt";
        case TransportKind::Tcp:
        default:
            return "tcp";
    }
}

std::string make_request_topic(const Args& args) {
    return "ipi/" + args.intersectionId + "/latency/" + args.sourceId + "/request";
}

std::string make_ack_topic(const Args& args) {
    return "ipi/" + args.intersectionId + "/latency/" + args.sourceId + "/ack";
}

std::string make_mqtt_client_id(const Args& args) {
    return "ipi-latency-receiver-" + args.sourceId + '-' + std::to_string(ipi::api::current_unix_time_ns());
}

void apply_context_defaults(Args& args) {
    if (args.context.conditionLabel.empty()) {
        args.context.conditionLabel =
            ipi::api::default_condition_label(transport_name(args.transport), args.context.networkLoadLevel);
    }
    if (args.context.rsuId.empty()) {
        args.context.rsuId = args.intersectionId;
    }
}

Args parse_args(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout
                << "Usage: " << argv[0] << " [options]\n"
                << "  --transport <tcp|mqtt>       Network path to use (default tcp)\n"
                << "  --host <ip>                  MQTT broker host in mqtt mode (default 127.0.0.1)\n"
                << "  --port <port>                Listen port or MQTT broker port (default 36666)\n"
                << "  --intersection-id <id>       Logical intersection id string\n"
                << "  --source-id <id>             Vehicle/source id to bind the probe topic\n"
                << "  --run-id <id>                Receiver-side default run id if request metadata is absent\n"
                << "  --condition-id <id>          Receiver-side default condition id if request metadata is absent\n"
                << "  --condition-label <label>    Receiver-side default condition label\n"
                << "  --av-id <id>                AV identifier override for logging\n"
                << "  --obu-id <id>               OBU identifier override for logging\n"
                << "  --rsu-id <id>               RSU identifier for logging\n"
                << "  --network-load-level <id>   idle|moderate|heavy|near-saturation\n"
                << "  --qos-profile <id>          FIFO, default, 5qi-mapped, etc.\n"
                << "  --mobility-state <id>       stationary, moving, handover, etc.\n"
                << "  --clock-sync-state <id>     ptp-synced, ntp-synced, unsynced\n"
                << "  --service-success <bool>    Infrastructure-side service success annotation\n"
                << "  --vehicle-outcome-name <s>  Driving metric name\n"
                << "  --vehicle-outcome-value <v> Driving metric value\n"
                << "  --vehicle-outcome-unit <u>  Driving metric unit\n"
                << "  --once                      Exit after one successful probe\n"
                << "  --quiet                     Suppress per-probe logging\n"
                << "  --csv                       Emit structured CSV rows\n";
            std::exit(0);
        }
        if (arg == "--transport" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            if (value == "tcp") {
                args.transport = TransportKind::Tcp;
            } else if (value == "mqtt") {
                args.transport = TransportKind::Mqtt;
            } else {
                throw std::invalid_argument("unsupported --transport value");
            }
        } else if ((arg == "--host" || arg == "-H") && i + 1 < argc) {
            args.host = argv[++i];
        } else if ((arg == "--port" || arg == "-P") && i + 1 < argc) {
            args.port = static_cast<std::uint16_t>(std::stoul(argv[++i]));
        } else if (arg == "--intersection-id" && i + 1 < argc) {
            args.intersectionId = argv[++i];
        } else if (arg == "--source-id" && i + 1 < argc) {
            args.sourceId = argv[++i];
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
    apply_context_defaults(args);
    return args;
}

int create_server_socket(std::uint16_t port) {
    const int socketFd = ::socket(AF_INET, SOCK_STREAM, 0);
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
    if (::listen(socketFd, 4) != 0) {
        ::close(socketFd);
        throw std::runtime_error("listen() failed");
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

void print_csv_header() {
    std::cout << ipi::api::experiment_log_csv_header() << '\n';
}

void log_ack(const Args& args,
             const ipi::api::Private5gProbeRequest& request,
             const ipi::api::Private5gProbeAck& ack) {
    if (args.quiet) {
        return;
    }

    ipi::api::ExperimentLogRecord record;
    record.emitterRole = "5g-receiver";
    record.emitTimeNs = ack.serverSendTimeNs;
    record.runId = request.runId.empty() ? args.context.runId : request.runId;
    record.conditionId = request.conditionId.empty() ? args.context.conditionId : request.conditionId;
    record.conditionLabel = request.conditionLabel.empty() ? args.context.conditionLabel : request.conditionLabel;
    record.serviceType = request.serviceType.empty() ? "unknown" : request.serviceType;
    record.transport = std::string(transport_name(args.transport));
    record.avId = args.context.avId;
    record.obuId = args.context.obuId.empty() ? request.sourceId : args.context.obuId;
    record.rsuId = args.context.rsuId;
    record.requestId = request.requestId;
    record.sessionId = request.sessionId.value_or(std::string{});
    record.intersectionId = request.intersectionId;
    record.sourceId = request.sourceId;
    record.sequence = request.sequence;
    record.networkLoadLevel = request.networkLoadLevel.empty() ? args.context.networkLoadLevel : request.networkLoadLevel;
    record.qosProfile = request.qosProfile.empty() ? args.context.qosProfile : request.qosProfile;
    record.mobilityState = request.mobilityState.empty() ? args.context.mobilityState : request.mobilityState;
    record.clockSyncState = request.clockSyncState.empty() ? args.context.clockSyncState : request.clockSyncState;
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

bool handle_tcp_client(int clientFd, const Args& args, ipi::v2x::UperCodec& codec) {
    for (;;) {
        try {
            const auto encodedRequest = ipi::api::recv_private_5g_probe_packet(clientFd);
            const auto request = ipi::api::decode_private_5g_probe_request(encodedRequest);
            const auto ack = handle_request(request, codec);
            ipi::api::send_private_5g_probe_packet(clientFd, ipi::api::encode_private_5g_probe_ack(ack));
            log_ack(args, request, ack);
            if (args.once) {
                return true;
            }
        } catch (const std::exception& ex) {
            if (std::string_view(ex.what()) == "peer closed") {
                return false;
            }
            std::cerr << "client error: " << ex.what() << '\n';
            return false;
        }
    }
}

void run_tcp_receiver(const Args& args) {
    const int serverFd = create_server_socket(args.port);
    std::cerr << "private 5G latency receiver listening on port " << args.port << " over tcp\n";

    ipi::v2x::UperCodec codec;
    for (;;) {
        const int clientFd = ::accept(serverFd, nullptr, nullptr);
        if (clientFd < 0) {
            std::cerr << "accept() failed\n";
            continue;
        }
        const bool exitAfterClient = handle_tcp_client(clientFd, args, codec);
        ::close(clientFd);
        if (exitAfterClient) {
            break;
        }
    }

    ::close(serverFd);
}

void run_mqtt_receiver(const Args& args) {
    std::cerr << "private 5G latency receiver connected to broker " << args.host << ':' << args.port << " over mqtt\n";
    ipi::api::MinimalMqttClient client(make_mqtt_client_id(args));
    client.connect(args.host, args.port);
    client.subscribe(make_request_topic(args));

    ipi::v2x::UperCodec codec;
    for (;;) {
        const auto message = client.receive(std::chrono::hours(24));
        if (!message) {
            continue;
        }
        const auto request = ipi::api::decode_private_5g_probe_request(message->payload);
        const auto ack = handle_request(request, codec);
        client.publish(make_ack_topic(args), ipi::api::encode_private_5g_probe_ack(ack));
        log_ack(args, request, ack);
        if (args.once) {
            break;
        }
    }

    client.disconnect();
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Args args = parse_args(argc, argv);
        if (args.csv) {
            print_csv_header();
        }
        if (args.transport == TransportKind::Mqtt) {
            run_mqtt_receiver(args);
        } else {
            run_tcp_receiver(args);
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
