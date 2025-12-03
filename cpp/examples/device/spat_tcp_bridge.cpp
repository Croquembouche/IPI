#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

extern "C" {
#include <v2x_api.h>
}

namespace {

constexpr std::uint16_t kDefaultPort = 35555;
std::uint32_t g_msg_counter = 0;

std::uint16_t get_port() {
    const char* env = std::getenv("MOCAR_TCP_PORT");
    if (env && *env) {
        return static_cast<std::uint16_t>(std::atoi(env));
    }
    return kDefaultPort;
}

long map_light_state(ipi::j2735::MovementPhaseState state) {
    switch (state) {
        case ipi::j2735::MovementPhaseState::Proceed:
            return Mde_LightState_protected_Movement_Allowed;
        case ipi::j2735::MovementPhaseState::StopThenProceed:
            return Mde_LightState_stop_Then_Proceed;
        case ipi::j2735::MovementPhaseState::Flashing:
            return Mde_LightState_caution_Conflicting_Traffic;
        case ipi::j2735::MovementPhaseState::StopAndRemain:
        default:
            return Mde_LightState_stop_And_Remain;
    }
}

void free_spat(v2x_msg_spat_t* spat) {
    if (!spat || !spat->intersections) return;
    for (int i = 0; i < spat->n_intersection; ++i) {
        mde_intersection_t& inter = spat->intersections[i];
        if (inter.phases) {
            for (int j = 0; j < inter.n_phase; ++j) {
                free(inter.phases[j].states);
            }
            free(inter.phases);
        }
    }
    free(spat->intersections);
    spat->intersections = nullptr;
    spat->n_intersection = 0;
}

void to_mocar(const ipi::j2735::SpatMessage& msg, v2x_msg_spat_t* out) {
    std::memset(out, 0, sizeof(*out));
    out->msg_count = g_msg_counter++ & 0xFF;
    out->moy = 0;
    out->time_stamp = static_cast<unsigned short>(msg.timestampMs & 0xFFFF);
    out->n_intersection = 1;
    out->intersections = static_cast<mde_intersection_t*>(
        std::calloc(out->n_intersection, sizeof(mde_intersection_t)));
    if (!out->intersections) {
        throw std::runtime_error("alloc intersections failed");
    }

    mde_intersection_t& inter = out->intersections[0];
    inter.intersection_id.region = 0;
    inter.intersection_id.id = msg.intersectionId;
    inter.intersection_status = Mde_IntersectionStatusObject_fixedTimeOperation;
    inter.moy = 0;
    inter.time_stamp = msg.timestampMs;
    inter.timeConfidence = 0;
    inter.n_phase = static_cast<int>(msg.phases.size());
    inter.phases = static_cast<mde_phase_t*>(std::calloc(inter.n_phase, sizeof(mde_phase_t)));
    if (!inter.phases) {
        throw std::runtime_error("alloc phases failed");
    }

    for (int i = 0; i < inter.n_phase; ++i) {
        const auto& phase_in = msg.phases[static_cast<std::size_t>(i)];
        mde_phase_t& phase_out = inter.phases[i];
        phase_out.id = phase_in.signalGroup;
        phase_out.n_state = 1;
        phase_out.states = static_cast<mde_phase_state_t*>(
            std::calloc(phase_out.n_state, sizeof(mde_phase_state_t)));
        if (!phase_out.states) {
            throw std::runtime_error("alloc states failed");
        }
        phase_out.states[0].light_state = map_light_state(phase_in.state);
        phase_out.states[0].timing.likelyEndTime = phase_in.timeToChangeMs.value_or(0);
        phase_out.states[0].timing.startTime = 0;
        phase_out.states[0].timing.minEndTime = 0;
        phase_out.states[0].timing.maxEndTime = 0;
        phase_out.states[0].timing.timeConfidence = 0;
        phase_out.states[0].timing.nextStartTime = 0;
        phase_out.states[0].timing.nextDuration = 0;
    }
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

void handle_client(int client_fd, ipi::v2x::UperCodec& codec) {
    while (true) {
        auto payload = recv_frame(client_fd);
        auto spat = codec.decode_spat(payload);
        std::cout << "[device] recv SPaT phases=" << spat.phases.size() << "\n";

        v2x_msg_spat_t mocar{};
        try {
            to_mocar(spat, &mocar);
            if (mde_cv2x_spat_send(&mocar) != 0) {
                std::cerr << "[device] mde_cv2x_spat_send failed\n";
            } else {
                std::cout << "[device] forwarded SPaT sg0=" << (int)spat.phases[0].signalGroup << "\n";
            }
        } catch (const std::exception& ex) {
            std::cerr << "[device] build/send failed: " << ex.what() << "\n";
        }
        free_spat(&mocar);
    }
}

} // namespace

int main() {
    if (mde_cv2x_init(0) != 0) {
        std::cerr << "mde_cv2x_init failed\n";
        return 1;
    }

    std::uint16_t port = get_port();
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket() failed\n";
        return 1;
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (::bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        std::cerr << "bind() failed\n";
        ::close(server_fd);
        return 1;
    }
    if (::listen(server_fd, 1) != 0) {
        std::cerr << "listen() failed\n";
        ::close(server_fd);
        return 1;
    }
    std::cout << "[device] listening on port " << port << "\n";

    ipi::v2x::UperCodec codec;
    while (true) {
        int client_fd = ::accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "accept() failed\n";
            continue;
        }
        std::cout << "[device] client connected\n";
        try {
            handle_client(client_fd, codec);
        } catch (const std::exception& ex) {
            std::cerr << "[device] client handler error: " << ex.what() << "\n";
        }
        ::close(client_fd);
    }

    ::close(server_fd);
    return 0;
}
