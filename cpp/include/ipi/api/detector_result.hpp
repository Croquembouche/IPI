#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ipi::api {

enum class DetectorCoordinateFrame : std::uint8_t {
    SENSOR = 0,
    VEHICLE = 1,
    MAP = 2
};

struct DetectorObject3d {
    std::string objectId{};
    std::uint16_t classId{0};
    std::array<double, 3> centerMeters{};
    std::array<double, 3> dimensionsMeters{};
    double yawRadians{0.0};
    double confidence{0.0};
    std::vector<double> velocityMps{};       ///< Empty or exactly xyz.
    std::vector<double> covariance{};        ///< Empty or row-major covariance.
};

struct DetectorResultFrame {
    std::string frameId{};
    std::uint64_t captureTimeUnixNs{0};
    DetectorCoordinateFrame coordinateFrame{DetectorCoordinateFrame::SENSOR};
    std::string coordinateFrameId{};
    std::vector<DetectorObject3d> objects{};
};

struct DetectorResultCodecLimits {
    std::size_t maxObjects{256};
    std::size_t maxEncodedBytes{1024U * 1024U};
};

[[nodiscard]] std::vector<std::uint8_t> encode_detector_result(
    const DetectorResultFrame& result,
    DetectorResultCodecLimits limits = {});

[[nodiscard]] DetectorResultFrame decode_detector_result(
    const std::vector<std::uint8_t>& encoded,
    DetectorResultCodecLimits limits = {});

/** Deterministic confidence-descending, objectId-ascending top-K selection. */
[[nodiscard]] DetectorResultFrame select_top_k_detector_objects(
    DetectorResultFrame result,
    std::size_t maximumObjects);

} // namespace ipi::api
