#include "ipi/v2x/j2735_messages.hpp"
#include "ipi/v2x/uper_codec.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

namespace {

void print_bytes(const std::vector<std::uint8_t>& bytes) {
    std::cout << "[";
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (i) std::cout << ' ';
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(bytes[i]);
    }
    std::cout << std::dec << "]\n";
}

} // namespace

int main() {
    using namespace ipi::j2735;

    try {
        ipi::v2x::UperCodec codec;
        BasicSafetyMessage bsm{};
        bsm.vehicleId = 0xABCDEF01;
        bsm.latitude = 37.3365;
        bsm.longitude = -121.8899;
        bsm.speedMps = 12.5F;
        bsm.headingDeg = 92.0F;
        bsm.accelerationMps2 = 0.5F;
        bsm.laneId = 4;

        auto bsmBytes = codec.encode(bsm);
        std::cout << "BSM send: " << bsm.to_string() << '\n';
        print_bytes(bsmBytes);
        auto bsmRecv = codec.decode_bsm(bsmBytes);
        std::cout << "BSM receive: " << bsmRecv.to_string() << "\n\n";

        MapMessage map{};
        map.intersectionId = 101;
        map.revision = 3;
        map.name = "Main & 1st";
        map.lanes = {{1, true}, {2, false}, {3, true}};
        auto mapBytes = codec.encode(map);
        std::cout << "MAP send: " << map.to_string() << '\n';
        print_bytes(mapBytes);
        auto mapRecv = codec.decode_map(mapBytes);
        std::cout << "MAP receive: " << mapRecv.to_string() << "\n\n";

        SpatMessage spat{};
        spat.intersectionId = 101;
        spat.timestampMs = 12345678;
        spat.phases = {{8, MovementPhaseState::Proceed, 5000},
                       {9, MovementPhaseState::StopAndRemain, std::nullopt}};
        auto spatBytes = codec.encode(spat);
        std::cout << "SPaT send: " << spat.to_string() << '\n';
        print_bytes(spatBytes);
        auto spatRecv = codec.decode_spat(spatBytes);
        std::cout << "SPaT receive: " << spatRecv.to_string() << "\n\n";

        SignalRequestMessage srm{};
        srm.requestId = 77;
        srm.vehicleId = 0xABCDEF01;
        srm.intersectionId = 101;
        srm.requestedSignalGroup = 8;
        srm.estimatedArrivalMs = 3200;
        srm.priorityLevel = 1;
        auto srmBytes = codec.encode(srm);
        std::cout << "SRM send: " << srm.to_string() << '\n';
        print_bytes(srmBytes);
        auto srmRecv = codec.decode_srm(srmBytes);
        std::cout << "SRM receive: " << srmRecv.to_string() << "\n\n";

        SignalStatusMessage ssm{};
        ssm.requestId = 77;
        ssm.intersectionId = 101;
        ssm.grantedSignalGroup = 8;
        ssm.granted = true;
        ssm.estimatedServedTimeMs = 4000;
        auto ssmBytes = codec.encode(ssm);
        std::cout << "SSM send: " << ssm.to_string() << '\n';
        print_bytes(ssmBytes);
        auto ssmRecv = codec.decode_ssm(ssmBytes);
        std::cout << "SSM receive: " << ssmRecv.to_string() << "\n\n";

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
