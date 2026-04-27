#include "ipi/api/experiment_logging.hpp"
#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

extern "C" {
#define class class_field
#include <v2x_api.h>
#undef class
}

namespace {

constexpr std::uint16_t kDefaultPort = 35555;
std::uint32_t g_revision = 0;
std::uint64_t g_forwardedCount = 0;

struct Args {
    std::uint16_t port{kDefaultPort};
    ipi::api::ExperimentContext context{};
    bool csv{false};
    bool quiet{false};
};

std::uint16_t default_port() {
    const char* env = std::getenv("MOCAR_TCP_PORT");
    if (env && *env) {
        return static_cast<std::uint16_t>(std::atoi(env));
    }
    return kDefaultPort;
}

std::uint64_t current_unix_time_ns() {
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count());
}

void apply_context_defaults(Args& args) {
    if (args.context.conditionLabel.empty()) {
        args.context.conditionLabel = ipi::api::default_condition_label("radio", args.context.networkLoadLevel);
    }
}

Args parse_args(int argc, char** argv) {
    Args args;
    args.port = default_port();

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout
                << "Usage: " << argv[0]
                << " [--port <p>] [--run-id <id>] [--condition-id <id>] [--condition-label <label>]\n"
                << "       [--request-id <id>] [--av-id <id>] [--obu-id <id>] [--rsu-id <id>]\n"
                << "       [--network-load-level <id>] [--qos-profile <id>] [--mobility-state <id>]\n"
                << "       [--clock-sync-state <id>] [--service-success <bool>] [--vehicle-outcome-name <s>]\n"
                << "       [--vehicle-outcome-value <v>] [--vehicle-outcome-unit <u>] [--csv] [--quiet]\n";
            std::exit(0);
        }
        if ((arg == "--port" || arg == "-P") && i + 1 < argc) {
            args.port = static_cast<std::uint16_t>(std::stoul(argv[++i]));
        } else if (arg == "--csv") {
            args.csv = true;
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (ipi::api::consume_experiment_context_arg(args.context, arg, i, argc, argv)) {
            continue;
        } else {
            throw std::invalid_argument("unknown argument: " + std::string(arg));
        }
    }

    apply_context_defaults(args);
    return args;
}

int map_event_state(ipi::j2735::MovementPhaseState state) {
    switch (state) {
        case ipi::j2735::MovementPhaseState::Proceed:
            return 4;
        case ipi::j2735::MovementPhaseState::StopThenProceed:
            return 2;
        case ipi::j2735::MovementPhaseState::Flashing:
            return 8;
        case ipi::j2735::MovementPhaseState::StopAndRemain:
        default:
            return 3;
    }
}

ipi::j2735::SpatMessage decode_spat(const std::vector<std::uint8_t>& payload, ipi::v2x::UperCodec& codec) {
    return codec.decode_spat(payload);
}

std::vector<std::uint8_t> recv_frame(int client_fd) {
    std::uint32_t len_be = 0;
    const ssize_t n = ::recv(client_fd, &len_be, sizeof(len_be), MSG_WAITALL);
    if (n == 0) {
        throw std::runtime_error("client closed");
    }
    if (n != sizeof(len_be)) {
        throw std::runtime_error("failed to read length");
    }
    const std::uint32_t len = ntohl(len_be);
    if (len == 0 || len > (10 * 1024)) {
        throw std::runtime_error("invalid length");
    }
    std::vector<std::uint8_t> buf(len);
    if (::recv(client_fd, buf.data(), len, MSG_WAITALL) != static_cast<ssize_t>(len)) {
        throw std::runtime_error("failed to read payload");
    }
    return buf;
}

void populate_mocar(const ipi::j2735::SpatMessage& msg, v2x_msg_spat_t* out) {
    std::memset(out, 0, sizeof(*out));

    const auto now = std::chrono::system_clock::now();
    const auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
    const std::time_t tt = std::chrono::system_clock::to_time_t(now_s);
    std::tm utc{};
    gmtime_r(&tt, &utc);
    const int moy = (utc.tm_yday * 24 * 60) + (utc.tm_hour * 60) + utc.tm_min;
    out->timeStamp_is_exist = SDK_OPTIONAL_EXSIT;
    out->timeStamp = moy;

    out->name_is_exist = SDK_OPTIONAL_EXSIT;
    std::snprintf(out->name, sizeof(out->name), "SPAT");

    out->intersections_count = 1;
    auto& inter = out->intersections[0];

    inter.name_is_exist = SDK_OPTIONAL_EXSIT;
    std::snprintf(inter.name, sizeof(inter.name), "inter0");

    inter.region_is_exist = SDK_OPTIONAL_EXSIT;
    inter.region = 1;
    inter.id = msg.intersectionId == 0 ? 1 : msg.intersectionId;
    inter.revision = static_cast<uint8_t>(g_revision++ % 128);

    std::memset(inter.status, 0, sizeof(inter.status));
    inter.status[1] = 1;
    inter.status[5] = 1;

    inter.moy_is_exist = SDK_OPTIONAL_EXSIT;
    inter.moy = moy;
    inter.timeStamp_is_exist = SDK_OPTIONAL_EXSIT;
    inter.timeStamp = static_cast<uint16_t>((utc.tm_sec * 1000) % 60000);

    inter.enabledLane_count = 1;
    inter.enabledLane[0] = 1;

    inter.state_count = static_cast<uint8_t>(std::max<std::size_t>(1, std::min<std::size_t>(msg.phases.size(), 16)));
    for (uint8_t i = 0; i < inter.state_count; ++i) {
        const ipi::j2735::SpatPhaseState* phase_in_ptr = nullptr;
        if (i < msg.phases.size()) {
            phase_in_ptr = &msg.phases[i];
        }
        ipi::j2735::SpatPhaseState phase{};
        if (phase_in_ptr) {
            phase = *phase_in_ptr;
        } else {
            phase.signalGroup = i + 1;
            phase.state = ipi::j2735::MovementPhaseState::Proceed;
            phase.timeToChangeMs = 5000;
        }

        auto& mv = inter.state[i];
        mv.movementName_is_exist = SDK_OPTIONAL_EXSIT;
        std::snprintf(mv.movementName, sizeof(mv.movementName), "move%u", i);
        mv.signalGroup = phase.signalGroup == 0 ? static_cast<uint8_t>(i + 1) : phase.signalGroup;

        mv.state_time_speed_count = 1;
        auto& evt = mv.state_time_speed[0];
        evt.eventState = map_event_state(phase.state);
        evt.timing_is_exsit = SDK_OPTIONAL_EXSIT;
        evt.timing.startTime_is_exsit = SDK_OPTIONAL_EXSIT;
        evt.timing.startTime = 1;
        const auto ttc_ds =
            static_cast<uint16_t>(std::max<long>(1, static_cast<long>(phase.timeToChangeMs.value_or(500)) / 100));
        evt.timing.minEndTime = ttc_ds;
        evt.timing.maxEndTime_is_exsit = SDK_OPTIONAL_EXSIT;
        evt.timing.maxEndTime = static_cast<uint16_t>(ttc_ds + 1);
        evt.timing.likelyTime_is_exsit = SDK_OPTIONAL_EXSIT;
        evt.timing.likelyTime = ttc_ds;
        evt.timing.confidence_is_exsit = SDK_OPTIONAL_EXSIT;
        evt.timing.confidence = 1;
        evt.timing.nextTime_is_exsit = SDK_OPTIONAL_EXSIT;
        evt.timing.nextTime = static_cast<uint16_t>(ttc_ds + 2);
        evt.speed_count = 0;

        mv.maneuverAssist_count = 0;
    }

    inter.maneuverAssist_count = 0;
}

void log_forward_result(const Args& args,
                        const ipi::j2735::SpatMessage& spat,
                        std::size_t payloadBytes,
                        bool accepted,
                        std::uint64_t serverReceiveTimeNs,
                        std::uint64_t serverSendTimeNs,
                        std::string detail) {
    if (args.quiet) {
        return;
    }

    ++g_forwardedCount;
    ipi::api::ExperimentLogRecord record;
    record.emitterRole = "radio-bridge";
    record.emitTimeNs = serverSendTimeNs;
    record.runId = args.context.runId;
    record.conditionId = args.context.conditionId;
    record.conditionLabel = args.context.conditionLabel;
    record.serviceType = "intersection-state";
    record.transport = "radio";
    record.avId = args.context.avId;
    record.obuId = args.context.obuId;
    record.rsuId = args.context.rsuId;
    record.requestId = args.context.requestIdBase;
    if (g_forwardedCount > 1) {
        record.requestId += "-" + std::to_string(g_forwardedCount);
    }
    record.intersectionId = std::to_string(spat.intersectionId);
    record.sequence = g_forwardedCount;
    record.networkLoadLevel = args.context.networkLoadLevel;
    record.qosProfile = args.context.qosProfile;
    record.mobilityState = args.context.mobilityState;
    record.clockSyncState = args.context.clockSyncState;
    record.accepted = accepted;
    record.serviceSuccess = accepted && args.context.serviceSuccess;
    record.vehicleOutcomeName = args.context.vehicleOutcomeName;
    record.vehicleOutcomeValue = args.context.vehicleOutcomeValue;
    record.vehicleOutcomeUnit = args.context.vehicleOutcomeUnit;
    record.frameType = "SPAT";
    record.payloadBytes = static_cast<std::uint32_t>(payloadBytes);
    record.serverReceiveTimeNs = serverReceiveTimeNs;
    record.serverSendTimeNs = serverSendTimeNs;
    record.detail = std::move(detail);

    if (args.csv) {
        std::cout << ipi::api::experiment_log_to_csv(record) << '\n';
    } else {
        std::cout << ipi::api::experiment_log_to_text(record) << '\n';
    }
}

void handle_client(int client_fd, const Args& args, ipi::v2x::UperCodec& codec) {
    while (true) {
        const auto payload = recv_frame(client_fd);
        const auto serverReceiveTimeNs = current_unix_time_ns();
        const auto spat = decode_spat(payload, codec);

        auto* mocar = static_cast<v2x_msg_spat_t*>(std::calloc(1, sizeof(v2x_msg_spat_t)));
        if (!mocar) {
            log_forward_result(args,
                               spat,
                               payload.size(),
                               false,
                               serverReceiveTimeNs,
                               current_unix_time_ns(),
                               "alloc failed");
            continue;
        }

        bool accepted = false;
        std::string detail = "forwarded SPAT to Mocar";
        try {
            populate_mocar(spat, mocar);
            if (mde_v2x_spat_send(mocar, nullptr, 32) != 0) {
                detail = "mde_v2x_spat_send failed";
            } else {
                accepted = true;
            }
        } catch (const std::exception& ex) {
            detail = ex.what();
        }

        log_forward_result(args,
                           spat,
                           payload.size(),
                           accepted,
                           serverReceiveTimeNs,
                           current_unix_time_ns(),
                           detail);
        std::free(mocar);
    }
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Args args = parse_args(argc, argv);
        if (args.csv) {
            std::cout << ipi::api::experiment_log_csv_header() << '\n';
        }

        if (mde_v2x_init(0) != 0) {
            std::cerr << "mde_v2x_init failed" << std::endl;
            return 1;
        }

        const int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cerr << "socket() failed" << std::endl;
            return 1;
        }
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(args.port);
        if (::bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            std::cerr << "bind() failed" << std::endl;
            ::close(server_fd);
            return 1;
        }
        if (::listen(server_fd, 1) != 0) {
            std::cerr << "listen() failed" << std::endl;
            ::close(server_fd);
            return 1;
        }
        std::cerr << "[device] listening on port " << args.port << std::endl;

        ipi::v2x::UperCodec codec;
        while (true) {
            const int client_fd = ::accept(server_fd, nullptr, nullptr);
            if (client_fd < 0) {
                std::cerr << "accept() failed" << std::endl;
                continue;
            }
            try {
                handle_client(client_fd, args, codec);
            } catch (const std::exception& ex) {
                std::cerr << "[device] client handler error: " << ex.what() << std::endl;
            }
            ::close(client_fd);
        }

        ::close(server_fd);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
