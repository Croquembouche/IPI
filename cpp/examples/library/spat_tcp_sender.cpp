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
#include <vector>

namespace {

constexpr std::uint16_t kDefaultPort = 35555;

struct Args {
    std::string host{"127.0.0.1"};
    std::uint16_t port{kDefaultPort};
    // Two-phase default: SG1 proceed, SG2 stop.
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

Args parse_args(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        std::string_view v(argv[i]);
        if (v == "--help" || v == "-h") {
            std::cout << "Usage: " << argv[0]
                      << " [--host <ip>] [--port <p>] [sg1 state1 time1 [sg2 state2 time2]]\n"
                      << "State: 0=dark,1=stopAndRemain,2=stopThenProceed,3=proceed,4=flashing\n";
            std::exit(0);
        } else if ((v == "--host" || v == "-H") && i + 1 < argc) {
            args.host = argv[++i];
        } else if ((v == "--port" || v == "-P") && i + 1 < argc) {
            args.port = static_cast<std::uint16_t>(std::stoi(argv[++i]));
        }
    }
    // Also support bare positional host/port for simplicity.
    if (argc >= 2 && argv[1][0] != '-') {
        args.host = argv[1];
    }
    if (argc >= 3 && argv[2][0] != '-') {
        args.port = static_cast<std::uint16_t>(std::stoi(argv[2]));
    }
    if (argc >= 6) {
        // Allow overriding the two default phases via CLI:
        // host port sg1 state1 time1 [sg2 state2 time2]
        args.phases.clear();
        ipi::j2735::SpatPhaseState p1{};
        p1.signalGroup = static_cast<std::uint8_t>(std::stoi(argv[argc - 3]));
        p1.state = parse_state(std::stoi(argv[argc - 2]));
        p1.timeToChangeMs = static_cast<std::uint16_t>(std::stoi(argv[argc - 1]));
        args.phases.push_back(p1);
        if (argc >= 9) {
            ipi::j2735::SpatPhaseState p0{};
            p0.signalGroup = static_cast<std::uint8_t>(std::stoi(argv[argc - 6]));
            p0.state = parse_state(std::stoi(argv[argc - 5]));
            p0.timeToChangeMs = static_cast<std::uint16_t>(std::stoi(argv[argc - 4]));
            args.phases.insert(args.phases.begin(), p0);
        }
    }
    return args;
}

std::vector<std::uint8_t> build_spat_frame(const Args& args) {
    ipi::j2735::SpatMessage spat;
    spat.intersectionId = 101; // demo id; adjust as needed
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
    spat.timestampMs = static_cast<std::uint32_t>(now_ms & 0xFFFFFFFF);
    spat.phases = args.phases;

    std::cout << "[edge] building SPaT intersection=" << spat.intersectionId
              << " phases=" << spat.phases.size() << "\n";
    for (const auto& p : spat.phases) {
        std::cout << "        sg=" << static_cast<int>(p.signalGroup)
                  << " state=" << static_cast<int>(p.state)
                  << " ttcMs=" << (p.timeToChangeMs ? std::to_string(*p.timeToChangeMs) : "n/a") << "\n";
    }

    ipi::v2x::UperCodec codec;
    return codec.encode(spat);
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

    std::cout << "[edge] payload bytes=" << payload.size();
    if (ipi::debug::enabled()) {
        std::cout << " hex=" << ipi::debug::hex(payload);
    }
    std::cout << "\n";

    ssize_t sent = ::send(sock, frame.data(), frame.size(), 0);
    if (sent < 0 || static_cast<std::size_t>(sent) != frame.size()) {
        ::close(sock);
        throw std::runtime_error("send() failed");
    }
    std::cout << "[edge] sent SPaT payload bytes=" << payload.size() << "\n";
    ::close(sock);
}

} // namespace

int main(int argc, char** argv) {
    try {
        Args args = parse_args(argc, argv);
        auto payload = build_spat_frame(args);
        send_payload(args, payload);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
