#include "ipi/api/experiment_logging.hpp"
#include "ipi/common/debug.hpp"
#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::uint16_t kDefaultPort = 35555;

struct Args {
    std::string host{"127.0.0.1"};
    std::uint16_t port{kDefaultPort};
    std::uint16_t intersectionId{101};
    ipi::api::ExperimentContext context{};
    bool csv{false};
    std::vector<ipi::j2735::SpatPhaseState> phases{
        {1, ipi::j2735::MovementPhaseState::Proceed, static_cast<std::uint16_t>(5000)},
        {2, ipi::j2735::MovementPhaseState::StopAndRemain, static_cast<std::uint16_t>(5000)}};
};

ipi::j2735::MovementPhaseState parse_state(int v) {
    switch (v) {
        case 0: return ipi::j2735::MovementPhaseState::Dark;
        case 1: return ipi::j2735::MovementPhaseState::StopAndRemain;
        case 2: return ipi::j2735::MovementPhaseState::StopThenProceed;
        case 3: return ipi::j2735::MovementPhaseState::Proceed;
        case 4: return ipi::j2735::MovementPhaseState::Flashing;
        default: return ipi::j2735::MovementPhaseState::StopAndRemain;
    }
}

void apply_context_defaults(Args& args) {
    if (args.context.conditionLabel.empty()) {
        args.context.conditionLabel = ipi::api::default_condition_label("radio", args.context.networkLoadLevel);
    }
}

Args parse_args(int argc, char** argv) {
    Args args;
    bool hostProvided = false;
    bool portProvided = false;
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        const std::string_view v(argv[i]);
        if (v == "--help" || v == "-h") {
            std::cout << "Usage: " << argv[0]
                      << " [--host <ip>] [--port <p>] [--intersection-id <id>] "
                         "[--run-id <id>] [--condition-id <id>] [--condition-label <label>] "
                         "[--request-id <id>] [--av-id <id>] [--obu-id <id>] [--rsu-id <id>] "
                         "[--network-load-level <id>] [--qos-profile <id>] [--mobility-state <id>] "
                         "[--clock-sync-state <id>] [--service-success <bool>] "
                         "[--vehicle-outcome-name <s>] [--vehicle-outcome-value <v>] [--vehicle-outcome-unit <u>] "
                         "[--csv] [host] [port] [sg1 state1 time1 [sg2 state2 time2]]\n"
                      << "State: 0=dark,1=stopAndRemain,2=stopThenProceed,3=proceed,4=flashing\n";
            std::exit(0);
        } else if ((v == "--host" || v == "-H") && i + 1 < argc) {
            args.host = argv[++i];
            hostProvided = true;
        } else if ((v == "--port" || v == "-P") && i + 1 < argc) {
            args.port = static_cast<std::uint16_t>(std::stoul(argv[++i]));
            portProvided = true;
        } else if (v == "--intersection-id" && i + 1 < argc) {
            args.intersectionId = static_cast<std::uint16_t>(std::stoul(argv[++i]));
        } else if (v == "--csv") {
            args.csv = true;
        } else if (ipi::api::consume_experiment_context_arg(args.context, v, i, argc, argv)) {
            continue;
        } else if (!v.empty() && v.front() != '-') {
            positional.emplace_back(argv[i]);
        } else {
            throw std::invalid_argument("unknown argument: " + std::string(v));
        }
    }

    std::size_t offset = 0;
    if (!hostProvided && positional.size() > offset) {
        args.host = positional[offset++];
    }
    if (!portProvided && positional.size() > offset) {
        args.port = static_cast<std::uint16_t>(std::stoul(positional[offset++]));
    }

    const auto remaining = positional.size() - offset;
    if (remaining == 3 || remaining == 6) {
        args.phases.clear();
        if (remaining == 6) {
            ipi::j2735::SpatPhaseState p0{};
            p0.signalGroup = static_cast<std::uint8_t>(std::stoi(positional[offset + 0]));
            p0.state = parse_state(std::stoi(positional[offset + 1]));
            p0.timeToChangeMs = static_cast<std::uint16_t>(std::stoi(positional[offset + 2]));
            args.phases.push_back(p0);
            offset += 3;
        }
        ipi::j2735::SpatPhaseState p1{};
        p1.signalGroup = static_cast<std::uint8_t>(std::stoi(positional[offset + 0]));
        p1.state = parse_state(std::stoi(positional[offset + 1]));
        p1.timeToChangeMs = static_cast<std::uint16_t>(std::stoi(positional[offset + 2]));
        args.phases.push_back(p1);
    } else if (remaining != 0) {
        throw std::invalid_argument("phase override expects 3 or 6 positional values");
    }

    apply_context_defaults(args);
    return args;
}

std::vector<std::uint8_t> build_spat_frame(const Args& args) {
    ipi::j2735::SpatMessage spat;
    spat.intersectionId = args.intersectionId;
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
    spat.timestampMs = static_cast<std::uint32_t>(now_ms & 0xFFFFFFFF);
    spat.phases = args.phases;

    ipi::v2x::UperCodec codec;
    return codec.encode(spat);
}

void log_send_result(const Args& args,
                     std::uint64_t requestTimeNs,
                     std::size_t payloadBytes) {
    ipi::api::ExperimentLogRecord record;
    record.emitterRole = "radio-sender";
    record.emitTimeNs = requestTimeNs;
    record.runId = args.context.runId;
    record.conditionId = args.context.conditionId;
    record.conditionLabel = args.context.conditionLabel;
    record.serviceType = "intersection-state";
    record.transport = "radio";
    record.avId = args.context.avId;
    record.obuId = args.context.obuId;
    record.rsuId = args.context.rsuId;
    record.requestId = args.context.requestIdBase;
    record.intersectionId = std::to_string(args.intersectionId);
    record.networkLoadLevel = args.context.networkLoadLevel;
    record.qosProfile = args.context.qosProfile;
    record.mobilityState = args.context.mobilityState;
    record.clockSyncState = args.context.clockSyncState;
    record.accepted = true;
    record.serviceSuccess = args.context.serviceSuccess;
    record.vehicleOutcomeName = args.context.vehicleOutcomeName;
    record.vehicleOutcomeValue = args.context.vehicleOutcomeValue;
    record.vehicleOutcomeUnit = args.context.vehicleOutcomeUnit;
    record.frameType = "SPAT";
    record.payloadBytes = static_cast<std::uint32_t>(payloadBytes);
    record.clientSendTimeNs = requestTimeNs;
    record.detail = "spat payload transmitted to bridge";

    if (args.csv) {
        std::cout << ipi::api::experiment_log_csv_header() << '\n'
                  << ipi::api::experiment_log_to_csv(record) << '\n';
    } else {
        std::cout << ipi::api::experiment_log_to_text(record) << '\n';
    }
}

void send_payload(const Args& args, const std::vector<std::uint8_t>& payload) {
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("socket() failed");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(args.port);
    if (::inet_pton(AF_INET, args.host.c_str(), &addr.sin_addr) != 1) {
        ::close(sock);
        throw std::runtime_error("inet_pton failed");
    }

    if (::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        ::close(sock);
        throw std::runtime_error("connect() failed");
    }

    std::uint32_t len = htonl(static_cast<std::uint32_t>(payload.size()));
    std::vector<std::uint8_t> frame(sizeof(len) + payload.size());
    std::memcpy(frame.data(), &len, sizeof(len));
    std::memcpy(frame.data() + sizeof(len), payload.data(), payload.size());

    if (ipi::debug::enabled()) {
        std::cerr << "[edge] payload bytes=" << payload.size()
                  << " hex=" << ipi::debug::hex(payload) << "\n";
    }

    const auto requestTimeNs = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count());

    const ssize_t sent = ::send(sock, frame.data(), frame.size(), 0);
    if (sent < 0 || static_cast<std::size_t>(sent) != frame.size()) {
        ::close(sock);
        throw std::runtime_error("send() failed");
    }
    ::close(sock);

    log_send_result(args, requestTimeNs, payload.size());
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Args args = parse_args(argc, argv);
        const auto payload = build_spat_frame(args);
        send_payload(args, payload);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
