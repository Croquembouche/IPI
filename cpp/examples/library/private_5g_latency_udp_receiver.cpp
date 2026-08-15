#include "ipi/api/experiment_logging.hpp"
#include "ipi/api/private_5g_latency_probe.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

constexpr std::uint16_t kDefaultPort = 36667;
constexpr std::size_t kMaxUdpPayload = 65507;
constexpr std::size_t kMaxReassembledPayload = 1024U * 1024U;
constexpr std::size_t kFragmentHeaderSize = 28U;
constexpr std::uint8_t kFragmentVersion = 1U;
constexpr std::uint64_t kFragmentTtlNs = 10ULL * 1000ULL * 1000ULL * 1000ULL;
constexpr std::array<std::uint8_t, 4> kFragmentMagic{'I', '5', 'G', 'F'};

struct FragmentKey {
    std::uint32_t peerAddress{};
    std::uint16_t peerPort{};
    std::uint64_t messageId{};

    bool operator==(const FragmentKey& other) const {
        return peerAddress == other.peerAddress && peerPort == other.peerPort && messageId == other.messageId;
    }
};

struct FragmentKeyHash {
    std::size_t operator()(const FragmentKey& key) const {
        const auto h1 = std::hash<std::uint32_t>{}(key.peerAddress);
        const auto h2 = std::hash<std::uint16_t>{}(key.peerPort);
        const auto h3 = std::hash<std::uint64_t>{}(key.messageId);
        return h1 ^ (h2 << 1U) ^ (h3 << 2U);
    }
};

struct FragmentAssembly {
    std::uint32_t totalSize{};
    std::uint16_t fragmentCount{};
    std::vector<std::uint8_t> bytes{};
    std::vector<bool> received{};
    std::size_t receivedCount{};
    std::uint64_t lastUpdateNs{};
};

using FragmentAssemblies = std::unordered_map<FragmentKey, FragmentAssembly, FragmentKeyHash>;

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

bool is_fragment_datagram(const std::uint8_t* data, std::size_t size) {
    return size >= kFragmentHeaderSize && std::equal(kFragmentMagic.begin(), kFragmentMagic.end(), data);
}

std::uint16_t read_u16_be(const std::uint8_t* data) {
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(data[0]) << 8U) |
                                      static_cast<std::uint16_t>(data[1]));
}

std::uint32_t read_u32_be(const std::uint8_t* data) {
    return (static_cast<std::uint32_t>(data[0]) << 24U) |
           (static_cast<std::uint32_t>(data[1]) << 16U) |
           (static_cast<std::uint32_t>(data[2]) << 8U) |
           static_cast<std::uint32_t>(data[3]);
}

std::uint64_t read_u64_be(const std::uint8_t* data) {
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < 8U; ++i) {
        value = (value << 8U) | static_cast<std::uint64_t>(data[i]);
    }
    return value;
}

void cleanup_stale_fragments(FragmentAssemblies& assemblies, std::uint64_t nowNs) {
    for (auto it = assemblies.begin(); it != assemblies.end();) {
        if (nowNs > it->second.lastUpdateNs && nowNs - it->second.lastUpdateNs > kFragmentTtlNs) {
            it = assemblies.erase(it);
        } else {
            ++it;
        }
    }
}

std::optional<std::vector<std::uint8_t>> reassemble_fragment(const std::uint8_t* data,
                                                             std::size_t size,
                                                             const sockaddr_in& peer,
                                                             FragmentAssemblies& assemblies) {
    if (!is_fragment_datagram(data, size)) {
        return std::vector<std::uint8_t>(data, data + size);
    }
    if (data[4] != kFragmentVersion) {
        throw std::runtime_error("unsupported UDP fragment version");
    }
    const auto headerSize = read_u16_be(data + 6U);
    if (headerSize != kFragmentHeaderSize || size < headerSize) {
        throw std::runtime_error("invalid UDP fragment header");
    }

    const auto messageId = read_u64_be(data + 8U);
    const auto totalSize = read_u32_be(data + 16U);
    const auto offset = read_u32_be(data + 20U);
    const auto fragmentIndex = read_u16_be(data + 24U);
    const auto fragmentCount = read_u16_be(data + 26U);
    const auto chunkSize = size - headerSize;

    if (totalSize == 0U || totalSize > kMaxReassembledPayload || fragmentCount == 0U ||
        fragmentIndex >= fragmentCount || chunkSize == 0U ||
        static_cast<std::uint64_t>(offset) + static_cast<std::uint64_t>(chunkSize) > totalSize) {
        throw std::runtime_error("invalid UDP fragment");
    }

    const auto nowNs = ipi::api::current_unix_time_ns();
    cleanup_stale_fragments(assemblies, nowNs);

    const FragmentKey key{peer.sin_addr.s_addr, peer.sin_port, messageId};
    auto& assembly = assemblies[key];
    if (assembly.bytes.empty() || assembly.totalSize != totalSize || assembly.fragmentCount != fragmentCount) {
        assembly.totalSize = totalSize;
        assembly.fragmentCount = fragmentCount;
        assembly.bytes.assign(totalSize, 0U);
        assembly.received.assign(fragmentCount, false);
        assembly.receivedCount = 0U;
    }
    assembly.lastUpdateNs = nowNs;

    if (!assembly.received[fragmentIndex]) {
        std::copy(data + headerSize, data + size, assembly.bytes.begin() + offset);
        assembly.received[fragmentIndex] = true;
        ++assembly.receivedCount;
    }

    if (assembly.receivedCount != assembly.fragmentCount) {
        return std::nullopt;
    }

    auto complete = std::move(assembly.bytes);
    assemblies.erase(key);
    return complete;
}

ipi::api::Private5gProbeAck handle_request(const ipi::api::Private5gProbeRequest& request,
                                           ipi::v2x::UperCodec& codec) {
    const auto steadyStart = std::chrono::steady_clock::now();
    const auto serverReceiveTimeNs = ipi::api::current_unix_time_ns();
    bool accepted = true;
    std::string detail;

    try {
        if (request.messageId.empty() || request.requestId.empty()) {
            throw std::runtime_error("probe request is missing message or request identity");
        }
        if (request.expiresAtUnixNs != 0 &&
            serverReceiveTimeNs >= request.expiresAtUnixNs) {
            throw std::runtime_error("probe request is expired");
        }
        detail = ipi::api::inspect_private_5g_probe_frame(request.frame, codec);
    } catch (const std::exception& ex) {
        detail = ex.what();
        accepted = false;
    }

    const auto serverSendTimeNs = ipi::api::current_unix_time_ns();
    const auto processingNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - steadyStart).count();
    return ipi::api::make_private_5g_probe_ack(
        request, serverReceiveTimeNs, serverSendTimeNs,
        static_cast<std::uint64_t>(processingNs), accepted, std::move(detail),
        "udp-probe-response-" + request.requestId + "-" + std::to_string(request.sequence));
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
    record.rttClock = "sender-steady";
    record.correlationStatus = "echoed";
    record.responseId = ack.responseId;
    record.correlationId = ack.correlationId;
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
        FragmentAssemblies fragmentAssemblies;

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
                auto encodedRequest = reassemble_fragment(buffer.data(),
                                                          static_cast<std::size_t>(received),
                                                          peer,
                                                          fragmentAssemblies);
                if (!encodedRequest) {
                    continue;
                }
                const auto request = ipi::api::decode_private_5g_probe_request(*encodedRequest);
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
