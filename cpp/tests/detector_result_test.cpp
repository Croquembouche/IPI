#include "test_support.hpp"

#include "ipi/api/detector_result.hpp"

#include <stdexcept>

namespace {

template <typename Callable>
void expect_throw(Callable callable, const std::string& message) {
    bool threw = false;
    try {
        callable();
    } catch (const std::exception&) {
        threw = true;
    }
    ipi::tests::expect(threw, message);
}

} // namespace

int main() {
    return ipi::tests::run_test("native_detector_result_schema", [] {
        using namespace ipi::api;

        DetectorResultFrame frame;
        frame.frameId = "lidar-frame-17";
        frame.captureTimeUnixNs = 1000000000ULL;
        frame.coordinateFrame = DetectorCoordinateFrame::SENSOR;
        frame.coordinateFrameId = "lidar_front";

        DetectorObject3d vehicle;
        vehicle.objectId = "track-2";
        vehicle.classId = 1;
        vehicle.centerMeters = {10.0, 1.0, 0.5};
        vehicle.dimensionsMeters = {4.5, 1.8, 1.6};
        vehicle.yawRadians = 0.2;
        vehicle.confidence = 0.91;
        vehicle.velocityMps = {5.0, 0.0, 0.0};
        vehicle.covariance = {0.1, 0.0, 0.0, 0.1};
        frame.objects.push_back(vehicle);

        auto pedestrian = vehicle;
        pedestrian.objectId = "track-1";
        pedestrian.classId = 2;
        pedestrian.confidence = 0.97;
        pedestrian.dimensionsMeters = {0.5, 0.5, 1.7};
        frame.objects.push_back(pedestrian);

        const auto encoded = encode_detector_result(frame);
        const auto decoded = decode_detector_result(encoded);
        ipi::tests::expect(decoded.frameId == frame.frameId,
                           "detector frame identity must round-trip");
        ipi::tests::expect(decoded.objects.size() == 2,
                           "native detector objects must round-trip");
        ipi::tests::expect(decoded.objects.front().dimensionsMeters ==
                               vehicle.dimensionsMeters,
                           "3D box dimensions must be retained");
        ipi::tests::expect(decoded.objects.front().velocityMps.size() == 3,
                           "native xyz velocity must be retained");

        const auto top = select_top_k_detector_objects(frame, 1);
        ipi::tests::expect(top.objects.size() == 1 &&
                               top.objects.front().objectId == "track-1",
                           "top-K selection must be deterministic by confidence");

        auto trailing = encoded;
        trailing.push_back(0);
        expect_throw([&] { (void)decode_detector_result(trailing); },
                     "detector decoder must reject trailing bytes");
        auto invalid = frame;
        invalid.objects.front().confidence = 1.5;
        expect_throw([&] { (void)encode_detector_result(invalid); },
                     "detector encoder must reject invalid confidence");
        expect_throw([&] {
            (void)encode_detector_result(frame, DetectorResultCodecLimits{1, 1024U * 1024U});
        }, "detector codec must enforce object-count policy");
    });
}
