#include "ipi/api/experiment_logging.hpp"
#include "ipi/api/minimal_mqtt_client.hpp"
#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

constexpr std::uint16_t kDefaultPort = 36666;

enum class ProbeMessageKind {
    CooperativeService,
    Spat
};

enum class TransportKind {
    Tcp,
    Mqtt
};

struct Args {
    std::string host{"127.0.0.1"};
    std::uint16_t port{kDefaultPort};
    std::size_t count{10};
    std::uint32_t intervalMs{1000};
    std::uint32_t timeoutMs{5000};
    ProbeMessageKind messageKind{ProbeMessageKind::CooperativeService};
    TransportKind transport{TransportKind::Tcp};
    std::string intersectionId{"intersection-101"};
    std::string sourceId{"veh-01"};
    std::string sessionId{"ipi-session-1"};
    std::uint16_t spatIntersectionId{101};
    std::uint16_t horizonMs{2500};
    std::size_t payloadBytes{0};
    ipi::api::ExperimentContext context{};
    bool csv{false};
};

struct ProbeStats {
    std::size_t attempted{0};
    std::size_t accepted{0};
    std::size_t rejected{0};
    std::size_t failed{0};
    std::vector<std::int64_t> rtts{};
    std::vector<std::int64_t> uplinks{};
    std::vector<std::int64_t> downlinks{};
};

std::string_view service_type_name(ProbeMessageKind kind) {
    switch (kind) {
        case ProbeMessageKind::Spat:
            return "intersection-state";
        case ProbeMessageKind::CooperativeService:
        default:
            return "guided-planning";
    }
}

std::string_view transport_name(TransportKind kind) {
    switch (kind) {
        case TransportKind::Mqtt:
            return "mqtt";
        case TransportKind::Tcp:
        default:
            return "tcp";
    }
}

double ns_to_ms(std::int64_t ns) {
    return static_cast<double>(ns) / 1000000.0;
}

ipi::SessionId session_id_from_text(const std::string& text) {
    ipi::SessionId sessionId{};
    const auto limit = std::min(sessionId.size(), text.size());
    std::copy_n(text.begin(), limit, sessionId.begin());
    return sessionId;
}

std::string make_request_topic(const Args& args) {
    return "ipi/" + args.intersectionId + "/latency/" + args.sourceId + "/request";
}

std::string make_ack_topic(const Args& args) {
    return "ipi/" + args.intersectionId + "/latency/" + args.sourceId + "/ack";
}

std::string make_mqtt_client_id(const Args& args) {
    std::ostringstream oss;
    oss << "ipi-latency-sender-" << args.sourceId << '-' << ipi::api::current_unix_time_ns();
    return oss.str();
}

std::string make_request_id(const Args& args, std::uint64_t sequence) {
    if (args.count <= 1U) {
        return args.context.requestIdBase;
    }
    return args.context.requestIdBase + "-" + std::to_string(sequence);
}

void apply_context_defaults(Args& args) {
    if (args.context.conditionLabel.empty()) {
        args.context.conditionLabel =
            ipi::api::default_condition_label(transport_name(args.transport), args.context.networkLoadLevel);
    }
    if (args.context.avId.empty()) {
        args.context.avId = args.sourceId;
    }
    if (args.context.obuId.empty()) {
        args.context.obuId = args.sourceId;
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
                << "  --host <ip>                  Receiver host or MQTT broker host (default 127.0.0.1)\n"
                << "  --port <port>                Receiver port or MQTT broker port (default 36666)\n"
                << "  --count <n>                  Number of probes to send (default 10)\n"
                << "  --interval-ms <ms>           Delay between probes (default 1000)\n"
                << "  --timeout-ms <ms>            Ack wait timeout for MQTT mode (default 5000)\n"
                << "  --message <service|spat>     Payload kind (default service)\n"
                << "  --intersection-id <id>       Logical intersection id string\n"
                << "  --session-id <id>            Session id text for service probes\n"
                << "  --source-id <id>             Vehicle/source id text\n"
                << "  --payload-bytes <n>          Extra service payload bytes for sizing tests\n"
                << "  --spat-intersection <id>     Numeric SPaT intersection id (default 101)\n"
                << "  --horizon-ms <ms>            Requested service horizon (default 2500)\n"
                << "  --run-id <id>                Shared experiment run identifier\n"
                << "  --condition-id <id>          Shared condition identifier\n"
                << "  --condition-label <label>    Run class such as private-5g-baseline\n"
                << "  --request-id <id>            Request id base used in logs and probe metadata\n"
                << "  --av-id <id>                Local AV identifier for logging\n"
                << "  --obu-id <id>               Local OBU identifier for logging\n"
                << "  --rsu-id <id>               Target RSU identifier for logging\n"
                << "  --network-load-level <id>   idle|moderate|heavy|near-saturation\n"
                << "  --qos-profile <id>          FIFO, default, 5qi-mapped, etc.\n"
                << "  --mobility-state <id>       stationary, moving, handover, etc.\n"
                << "  --clock-sync-state <id>     ptp-synced, ntp-synced, unsynced\n"
                << "  --service-success <bool>    AV/service success annotation for this run\n"
                << "  --vehicle-outcome-name <s>  Driving metric name\n"
                << "  --vehicle-outcome-value <v> Driving metric value\n"
                << "  --vehicle-outcome-unit <u>  Driving metric unit\n"
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
        } else if (arg == "--count" && i + 1 < argc) {
            args.count = static_cast<std::size_t>(std::stoull(argv[++i]));
        } else if (arg == "--interval-ms" && i + 1 < argc) {
            args.intervalMs = static_cast<std::uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--timeout-ms" && i + 1 < argc) {
            args.timeoutMs = static_cast<std::uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--message" && i + 1 < argc) {
            const std::string_view value(argv[++i]);
            if (value == "service") {
                args.messageKind = ProbeMessageKind::CooperativeService;
            } else if (value == "spat") {
                args.messageKind = ProbeMessageKind::Spat;
            } else {
                throw std::invalid_argument("unsupported --message value");
            }
        } else if (arg == "--intersection-id" && i + 1 < argc) {
            args.intersectionId = argv[++i];
        } else if (arg == "--session-id" && i + 1 < argc) {
            args.sessionId = argv[++i];
        } else if (arg == "--source-id" && i + 1 < argc) {
            args.sourceId = argv[++i];
        } else if (arg == "--payload-bytes" && i + 1 < argc) {
            args.payloadBytes = static_cast<std::size_t>(std::stoull(argv[++i]));
        } else if (arg == "--spat-intersection" && i + 1 < argc) {
            args.spatIntersectionId = static_cast<std::uint16_t>(std::stoul(argv[++i]));
        } else if (arg == "--horizon-ms" && i + 1 < argc) {
            args.horizonMs = static_cast<std::uint16_t>(std::stoul(argv[++i]));
        } else if (arg == "--csv") {
            args.csv = true;
        } else if (ipi::api::consume_experiment_context_arg(args.context, arg, i, argc, argv)) {
            continue;
        } else {
            throw std::invalid_argument("unknown argument: " + std::string(arg));
        }
    }
    if (args.count == 0) {
        throw std::invalid_argument("--count must be greater than zero");
    }
    apply_context_defaults(args);
    return args;
}

int connect_socket(const Args& args) {
    const int socketFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        throw std::runtime_error("socket() failed");
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(args.port);
    if (::inet_pton(AF_INET, args.host.c_str(), &address.sin_addr) != 1) {
        ::close(socketFd);
        throw std::runtime_error("inet_pton() failed");
    }

    if (::connect(socketFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        ::close(socketFd);
        throw std::runtime_error("connect() failed");
    }
    return socketFd;
}

ipi::MessageFrame build_service_frame(const Args& args, std::uint64_t sequence) {
    ipi::CooperativeServiceMessage message;
    message.sessionId = session_id_from_text(args.sessionId);
    message.vehicleId.assign(args.sourceId.begin(), args.sourceId.end());
    message.serviceClass = ipi::ServiceClass::GuidedPlanning;
    message.guidanceStatus = ipi::GuidanceStatus::Request;
    message.requestedHorizonMs = args.horizonMs;
    message.confidence = 100;

    ipi::GuidedPlanningPayload planning;
    ipi::Waypoint waypoint;
    waypoint.position.latitude = 42.3314 + (static_cast<double>(sequence) * 0.00001);
    waypoint.position.longitude = -83.0458;
    waypoint.targetSpeedMps = 8.0;
    planning.waypoints.push_back(waypoint);
    message.planning = planning;

    if (args.payloadBytes > 0) {
        message.offloadPayload = std::vector<std::uint8_t>(args.payloadBytes,
                                                           static_cast<std::uint8_t>(sequence & 0xFF));
        message.offloadTaskId = "probe-" + std::to_string(sequence);
    }

    return ipi::api::make_private_5g_probe_frame(message);
}

ipi::MessageFrame build_spat_frame(const Args& args, std::uint64_t sequence, const ipi::v2x::UperCodec& codec) {
    ipi::j2735::SpatMessage spat;
    spat.intersectionId = args.spatIntersectionId;
    spat.timestampMs = static_cast<std::uint32_t>((ipi::api::current_unix_time_ns() / 1000000ULL) & 0xFFFFFFFFULL);
    spat.phases = {
        {1, ipi::j2735::MovementPhaseState::Proceed, static_cast<std::uint16_t>(5000)},
        {2, sequence % 2 == 0 ? ipi::j2735::MovementPhaseState::StopAndRemain
                              : ipi::j2735::MovementPhaseState::StopThenProceed,
         static_cast<std::uint16_t>(5000)}};
    return ipi::api::make_private_5g_probe_frame(spat, codec);
}

ipi::MessageFrame build_frame(const Args& args, std::uint64_t sequence, const ipi::v2x::UperCodec& codec) {
    switch (args.messageKind) {
        case ProbeMessageKind::Spat:
            return build_spat_frame(args, sequence, codec);
        case ProbeMessageKind::CooperativeService:
        default:
            return build_service_frame(args, sequence);
    }
}

ipi::api::Private5gProbeRequest build_request(const Args& args,
                                              std::uint64_t sequence,
                                              const ipi::v2x::UperCodec& codec) {
    ipi::api::Private5gProbeRequest request;
    request.sequence = sequence;
    request.clientSendTimeNs = ipi::api::current_unix_time_ns();
    request.runId = args.context.runId;
    request.conditionId = args.context.conditionId;
    request.conditionLabel = args.context.conditionLabel;
    request.requestId = make_request_id(args, sequence);
    request.serviceType = std::string(service_type_name(args.messageKind));
    request.intersectionId = args.intersectionId;
    request.sourceId = args.sourceId;
    if (args.messageKind == ProbeMessageKind::CooperativeService) {
        request.sessionId = args.sessionId;
    }
    request.networkLoadLevel = args.context.networkLoadLevel;
    request.qosProfile = args.context.qosProfile;
    request.mobilityState = args.context.mobilityState;
    request.clockSyncState = args.context.clockSyncState;
    request.frame = build_frame(args, request.sequence, codec);
    return request;
}

std::int64_t percentile_ns(std::vector<std::int64_t> values, double percentile) {
    if (values.empty()) {
        return 0;
    }
    std::sort(values.begin(), values.end());
    const auto rank = static_cast<std::size_t>(std::ceil(percentile * static_cast<double>(values.size()))) - 1U;
    return values[std::min(rank, values.size() - 1U)];
}

void print_csv_header() {
    std::cout << ipi::api::experiment_log_csv_header() << '\n';
}

ipi::api::ExperimentLogRecord make_base_record(const Args& args,
                                               const ipi::api::Private5gProbeRequest& request,
                                               std::uint64_t emitTimeNs) {
    ipi::api::ExperimentLogRecord record;
    record.emitterRole = "5g-sender";
    record.emitTimeNs = emitTimeNs;
    record.runId = request.runId;
    record.conditionId = request.conditionId;
    record.conditionLabel = request.conditionLabel;
    record.serviceType = request.serviceType;
    record.transport = std::string(transport_name(args.transport));
    record.avId = args.context.avId;
    record.obuId = args.context.obuId;
    record.rsuId = args.context.rsuId;
    record.requestId = request.requestId;
    record.sessionId = request.sessionId.value_or(std::string{});
    record.intersectionId = request.intersectionId;
    record.sourceId = request.sourceId;
    record.sequence = request.sequence;
    record.networkLoadLevel = request.networkLoadLevel;
    record.qosProfile = request.qosProfile;
    record.mobilityState = request.mobilityState;
    record.clockSyncState = request.clockSyncState;
    record.serviceSuccess = args.context.serviceSuccess;
    record.vehicleOutcomeName = args.context.vehicleOutcomeName;
    record.vehicleOutcomeValue = args.context.vehicleOutcomeValue;
    record.vehicleOutcomeUnit = args.context.vehicleOutcomeUnit;
    record.clientSendTimeNs = request.clientSendTimeNs;
    record.frameType = ipi::to_string(request.frame.type);
    record.payloadBytes = static_cast<std::uint32_t>(request.frame.payload.size());
    return record;
}

void emit_record(const Args& args, const ipi::api::ExperimentLogRecord& record) {
    if (args.csv) {
        std::cout << ipi::api::experiment_log_to_csv(record) << '\n';
    } else {
        std::cout << ipi::api::experiment_log_to_text(record) << '\n';
    }
}

void record_probe_result(const Args& args,
                         const ipi::api::Private5gProbeRequest& request,
                         const ipi::api::Private5gProbeAck& ack,
                         std::uint64_t clientReceiveTimeNs,
                         ProbeStats& stats) {
    ++stats.attempted;
    if (ack.accepted) {
        ++stats.accepted;
    } else {
        ++stats.rejected;
    }

    const auto metrics = ipi::api::compute_private_5g_latency_metrics(ack, clientReceiveTimeNs);
    auto record = make_base_record(args, request, clientReceiveTimeNs);
    record.accepted = ack.accepted;
    record.serviceSuccess = ack.accepted && args.context.serviceSuccess;
    record.frameType = ipi::to_string(ack.frameType);
    record.payloadBytes = ack.payloadSize;
    record.serverReceiveTimeNs = ack.serverReceiveTimeNs;
    record.serverSendTimeNs = ack.serverSendTimeNs;
    record.clientReceiveTimeNs = clientReceiveTimeNs;
    record.roundTripMs = ns_to_ms(metrics.roundTripNs);
    if (metrics.uplinkNs) {
        record.uplinkMs = ns_to_ms(*metrics.uplinkNs);
    }
    record.serverMs = ns_to_ms(metrics.serverProcessingNs);
    if (metrics.downlinkNs) {
        record.downlinkMs = ns_to_ms(*metrics.downlinkNs);
    }
    record.detail = ack.detail;
    emit_record(args, record);

    if (ack.accepted) {
        stats.rtts.push_back(metrics.roundTripNs);
        if (metrics.uplinkNs) {
            stats.uplinks.push_back(*metrics.uplinkNs);
        }
        if (metrics.downlinkNs) {
            stats.downlinks.push_back(*metrics.downlinkNs);
        }
    }
}

void record_probe_failure(const Args& args,
                          const ipi::api::Private5gProbeRequest& request,
                          std::string detail,
                          std::uint64_t emitTimeNs,
                          ProbeStats& stats) {
    ++stats.attempted;
    ++stats.failed;

    auto record = make_base_record(args, request, emitTimeNs);
    record.accepted = false;
    record.serviceSuccess = false;
    record.clientReceiveTimeNs = emitTimeNs;
    record.detail = std::move(detail);
    emit_record(args, record);
}

void print_summary(const ProbeStats& stats, bool csv) {
    std::ostream& out = csv ? std::cerr : std::cout;
    const auto total = stats.attempted;
    const auto successRate = total == 0 ? 0.0 : (100.0 * static_cast<double>(stats.accepted) / static_cast<double>(total));

    out << "summary attempts=" << total
        << " accepted=" << stats.accepted
        << " rejected=" << stats.rejected
        << " failed=" << stats.failed
        << " success_rate_pct=" << std::fixed << std::setprecision(2) << successRate << '\n';

    if (stats.rtts.empty()) {
        out << "summary rtt_ms unavailable (no successful probes)\n";
        out << "summary uplink_ms unavailable (no successful probes)\n";
        out << "summary downlink_ms unavailable (no successful probes)\n";
        return;
    }

    out << "summary rtt_ms"
        << " p50=" << std::fixed << std::setprecision(3) << ns_to_ms(percentile_ns(stats.rtts, 0.50))
        << " p95=" << ns_to_ms(percentile_ns(stats.rtts, 0.95))
        << " p99=" << ns_to_ms(percentile_ns(stats.rtts, 0.99))
        << " count=" << stats.rtts.size() << '\n';

    if (!stats.uplinks.empty()) {
        out << "summary uplink_ms"
            << " p50=" << ns_to_ms(percentile_ns(stats.uplinks, 0.50))
            << " p95=" << ns_to_ms(percentile_ns(stats.uplinks, 0.95))
            << " p99=" << ns_to_ms(percentile_ns(stats.uplinks, 0.99))
            << " count=" << stats.uplinks.size() << '\n';
    } else {
        out << "summary uplink_ms unavailable (clocks not synchronized or negative one-way samples)\n";
    }

    if (!stats.downlinks.empty()) {
        out << "summary downlink_ms"
            << " p50=" << ns_to_ms(percentile_ns(stats.downlinks, 0.50))
            << " p95=" << ns_to_ms(percentile_ns(stats.downlinks, 0.95))
            << " p99=" << ns_to_ms(percentile_ns(stats.downlinks, 0.99))
            << " count=" << stats.downlinks.size() << '\n';
    } else {
        out << "summary downlink_ms unavailable (clocks not synchronized or negative one-way samples)\n";
    }
}

ProbeStats run_tcp_sender(const Args& args) {
    ProbeStats stats;
    stats.rtts.reserve(args.count);

    const int socketFd = connect_socket(args);
    ipi::v2x::UperCodec codec;

    for (std::size_t i = 0; i < args.count; ++i) {
        const auto request = build_request(args, i + 1U, codec);
        try {
            const auto encodedRequest = ipi::api::encode_private_5g_probe_request(request);
            ipi::api::send_private_5g_probe_packet(socketFd, encodedRequest);

            const auto encodedAck = ipi::api::recv_private_5g_probe_packet(socketFd);
            const auto clientReceiveTimeNs = ipi::api::current_unix_time_ns();
            const auto ack = ipi::api::decode_private_5g_probe_ack(encodedAck);
            record_probe_result(args, request, ack, clientReceiveTimeNs, stats);
        } catch (const std::exception& ex) {
            record_probe_failure(args, request, ex.what(), ipi::api::current_unix_time_ns(), stats);
            break;
        }

        if (i + 1U < args.count) {
            std::this_thread::sleep_for(std::chrono::milliseconds(args.intervalMs));
        }
    }

    ::close(socketFd);
    return stats;
}

ProbeStats run_mqtt_sender(const Args& args) {
    ProbeStats stats;
    stats.rtts.reserve(args.count);

    ipi::v2x::UperCodec codec;
    ipi::api::MinimalMqttClient client(make_mqtt_client_id(args));
    client.connect(args.host, args.port);
    client.subscribe(make_ack_topic(args));

    for (std::size_t i = 0; i < args.count; ++i) {
        const auto request = build_request(args, i + 1U, codec);
        try {
            client.publish(make_request_topic(args), ipi::api::encode_private_5g_probe_request(request));
            const auto message = client.receive(std::chrono::milliseconds(args.timeoutMs));
            if (!message) {
                record_probe_failure(args, request, "mqtt ack timeout", ipi::api::current_unix_time_ns(), stats);
            } else {
                const auto clientReceiveTimeNs = ipi::api::current_unix_time_ns();
                const auto ack = ipi::api::decode_private_5g_probe_ack(message->payload);
                record_probe_result(args, request, ack, clientReceiveTimeNs, stats);
            }
        } catch (const std::exception& ex) {
            record_probe_failure(args, request, ex.what(), ipi::api::current_unix_time_ns(), stats);
            break;
        }

        if (i + 1U < args.count) {
            std::this_thread::sleep_for(std::chrono::milliseconds(args.intervalMs));
        }
    }

    client.disconnect();
    return stats;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Args args = parse_args(argc, argv);
        if (args.csv) {
            print_csv_header();
        }

        const auto stats = args.transport == TransportKind::Mqtt
                               ? run_mqtt_sender(args)
                               : run_tcp_sender(args);
        print_summary(stats, args.csv);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
