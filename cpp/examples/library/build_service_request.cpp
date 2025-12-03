#include "ipi/core/ipi_cooperative_service.hpp"
#include "ipi/core/ipi_service_request.hpp"
#include "ipi/core/message_frame.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std::chrono_literals;

int main() {
    try {
        ipi::IpiServiceRequest request{};
        request.serviceType = ipi::ServiceType::UnprotectedLeftAvailability;
        request.requestId = 42;
        request.desiredHorizonMs = 5000;
        request.additionalData = {'L', 'T'}; // placeholder lane tag

        auto encoded = request.to_canonical_encoding();
        auto decoded = ipi::IpiServiceRequest::from_canonical_encoding(encoded);
        std::cout << "Encoded IPI-ServiceRequest: " << encoded.size() << " bytes\n";
        std::cout << "Decoded : " << decoded.to_string() << "\n";

        ipi::CooperativeServiceMessage cavMsg{};
        cavMsg.vehicleId = {0x12, 0x34, 0x56, 0x78};
        cavMsg.serviceClass = ipi::ServiceClass::GuidedPlanning;
        cavMsg.guidanceStatus = ipi::GuidanceStatus::Request;
        cavMsg.requestedHorizonMs = 15000;
        cavMsg.confidence = 90;

        // Fill sessionId with pseudo-random bytes.
        std::mt19937 rng{12345};
        std::uniform_int_distribution<int> dist(0, 255);
        for (auto& byte : cavMsg.sessionId) {
            byte = static_cast<std::uint8_t>(dist(rng));
        }

        ipi::GuidedPlanningPayload planning{};
        planning.fallbackRoute = false;
        planning.waypoints.push_back({{37.336, -121.889, 15.0}, 8.0, static_cast<std::uint16_t>(1000)});
        planning.waypoints.push_back({{37.337, -121.888, {}}, 6.0, std::nullopt});
        cavMsg.planning = planning;

        auto cavEncoded = cavMsg.to_canonical_encoding();
        auto cavDecoded = ipi::CooperativeServiceMessage::from_canonical_encoding(cavEncoded);
        std::cout << "Encoded CooperativeService: " << cavEncoded.size() << " bytes\n";
        std::cout << "Decoded : " << cavDecoded.to_string() << "\n";

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
