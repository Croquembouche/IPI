#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

extern "C" {
#define class class_field
#include <v2x_api.h>
#undef class
}

namespace {

constexpr std::uint16_t kDefaultPort = 35555;
std::uint32_t g_revision = 0;

std::uint16_t get_port() {
    const char* env = std::getenv("MOCAR_TCP_PORT");
    if (env && *env) {
        return static_cast<std::uint16_t>(std::atoi(env));
    }
    return kDefaultPort;
}

int map_event_state(ipi::j2735::MovementPhaseState state) {
    switch (state) {
        case ipi::j2735::MovementPhaseState::Proceed:
            return 4; // protected-Movement-Allowed
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
    auto spat = codec.decode_spat(payload);
    std::cout << "[device] recv SPaT intersection=" << spat.intersectionId
              << " phases=" << spat.phases.size() << std::endl;
    for (const auto& p : spat.phases) {
        std::cout << "         sg=" << static_cast<int>(p.signalGroup)
                  << " state=" << static_cast<int>(p.state)
                  << " ttcMs=" << (p.timeToChangeMs ? std::to_string(*p.timeToChangeMs) : "n/a")
                  << std::endl;
    }
    return spat;
}

std::vector<std::uint8_t> recv_frame(int client_fd) {
    std::uint32_t len_be = 0;
    ssize_t n = ::recv(client_fd, &len_be, sizeof(len_be), MSG_WAITALL);
    if (n == 0) {
        throw std::runtime_error("client closed");
    }
    if (n != sizeof(len_be)) {
        throw std::runtime_error("failed to read length");
    }
    std::uint32_t len = ntohl(len_be);
    if (len == 0 || len > (10 * 1024)) {
        throw std::runtime_error("invalid length");
    }
    std::vector<std::uint8_t> buf(len);
    n = ::recv(client_fd, buf.data(), len, MSG_WAITALL);
    if (n != static_cast<ssize_t>(len)) {
        throw std::runtime_error("failed to read payload");
    }
    return buf;
}

void populate_mocar(const ipi::j2735::SpatMessage& msg, v2x_msg_spat_t* out) {
    std::memset(out, 0, sizeof(*out));

    // Top-level timestamp (Minute of the Year).
    auto now = std::chrono::system_clock::now();
    auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
    std::time_t tt = std::chrono::system_clock::to_time_t(now_s);
    std::tm utc{};
    gmtime_r(&tt, &utc);
    int moy = (utc.tm_yday * 24 * 60) + (utc.tm_hour * 60) + utc.tm_min;
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
        auto ttc_ds = static_cast<uint16_t>(std::max<long>(1, static_cast<long>(phase.timeToChangeMs.value_or(500)) / 100));
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

void dump_mocar(const v2x_msg_spat_t& m) {
    std::cout << "[device] mocar SPaT timeStamp(MOY)=" << m.timeStamp
              << " intersections=" << static_cast<int>(m.intersections_count) << std::endl;
    for (uint8_t i = 0; i < m.intersections_count; ++i) {
        const auto& inter = m.intersections[i];
        std::cout << "   inter id=" << inter.id << " region=" << inter.region
                  << " revision=" << static_cast<int>(inter.revision)
                  << " states=" << static_cast<int>(inter.state_count) << std::endl;
        for (uint8_t j = 0; j < inter.state_count; ++j) {
            const auto& st = inter.state[j];
            std::cout << "      sg=" << static_cast<int>(st.signalGroup)
                      << " events=" << static_cast<int>(st.state_time_speed_count) << std::endl;
            for (uint8_t k = 0; k < st.state_time_speed_count; ++k) {
                const auto& evt = st.state_time_speed[k];
                std::cout << "         eventState=" << static_cast<int>(evt.eventState)
                          << " min=" << evt.timing.minEndTime
                          << " likely=" << evt.timing.likelyTime
                          << " max=" << evt.timing.maxEndTime << std::endl;
            }
        }
    }
}

void handle_client(int client_fd, ipi::v2x::UperCodec& codec) {
    while (true) {
        auto payload = recv_frame(client_fd);
        std::cout << "[device] received raw bytes=" << payload.size() << " hex=";
        for (auto b : payload) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
        }
        std::cout << std::dec << std::setfill(' ') << std::endl;

        auto spat = decode_spat(payload, codec);

        auto* mocar = static_cast<v2x_msg_spat_t*>(std::calloc(1, sizeof(v2x_msg_spat_t)));
        if (!mocar) {
            std::cerr << "[device] alloc failed" << std::endl;
            continue;
        }
        try {
            populate_mocar(spat, mocar);
            dump_mocar(*mocar);
            if (mde_v2x_spat_send(mocar, nullptr, 32) != 0) {
                std::cerr << "[device] mde_v2x_spat_send failed" << std::endl;
            } else {
                std::cout << "[device] forwarded SPaT" << std::endl;
            }
        } catch (const std::exception& ex) {
            std::cerr << "[device] build/send failed: " << ex.what() << std::endl;
        }
        std::free(mocar);
    }
}

} // namespace

int main() {
    if (mde_v2x_init(0) != 0) {
        std::cerr << "mde_v2x_init failed" << std::endl;
        return 1;
    }

    std::uint16_t port = get_port();
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket() failed" << std::endl;
        return 1;
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
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
    std::cout << "[device] listening on port " << port << std::endl;

    ipi::v2x::UperCodec codec;
    while (true) {
        int client_fd = ::accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "accept() failed" << std::endl;
            continue;
        }
        std::cout << "[device] client connected" << std::endl;
        try {
            handle_client(client_fd, codec);
        } catch (const std::exception& ex) {
            std::cerr << "[device] client handler error: " << ex.what() << std::endl;
        }
        ::close(client_fd);
    }

    ::close(server_fd);
    return 0;
}
