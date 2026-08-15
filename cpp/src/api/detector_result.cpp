#include "ipi/api/detector_result.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace ipi::api {
namespace {

constexpr std::array<std::uint8_t, 4> kMagic{{'I', 'D', 'E', 'T'}};
constexpr std::uint8_t kVersion = 1;

class Writer {
public:
    void u8(std::uint8_t value) { bytes.push_back(value); }
    void u16(std::uint16_t value) {
        bytes.push_back(static_cast<std::uint8_t>(value >> 8));
        bytes.push_back(static_cast<std::uint8_t>(value));
    }
    void u32(std::uint32_t value) {
        for (int shift = 24; shift >= 0; shift -= 8) bytes.push_back(static_cast<std::uint8_t>(value >> shift));
    }
    void u64(std::uint64_t value) {
        for (int shift = 56; shift >= 0; shift -= 8) bytes.push_back(static_cast<std::uint8_t>(value >> shift));
    }
    void f64(double value) {
        std::uint64_t bits = 0;
        std::memcpy(&bits, &value, sizeof(bits));
        u64(bits);
    }
    void string(const std::string& value) {
        if (value.size() > std::numeric_limits<std::uint16_t>::max()) {
            throw std::invalid_argument("detector string exceeds uint16");
        }
        u16(static_cast<std::uint16_t>(value.size()));
        bytes.insert(bytes.end(), value.begin(), value.end());
    }
    std::vector<std::uint8_t> bytes{};
};

class Reader {
public:
    explicit Reader(const std::vector<std::uint8_t>& input) : bytes(input) {}
    std::uint8_t u8() { require(1); return bytes[offset++]; }
    std::uint16_t u16() {
        require(2);
        const auto value = static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(bytes[offset]) << 8) | bytes[offset + 1]);
        offset += 2;
        return value;
    }
    std::uint32_t u32() {
        require(4);
        std::uint32_t value = 0;
        for (int index = 0; index < 4; ++index) value = (value << 8) | bytes[offset++];
        return value;
    }
    std::uint64_t u64() {
        require(8);
        std::uint64_t value = 0;
        for (int index = 0; index < 8; ++index) value = (value << 8) | bytes[offset++];
        return value;
    }
    double f64() {
        const auto bits = u64();
        double value = 0;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }
    std::string string() {
        const auto size = u16();
        require(size);
        const auto first = bytes.begin() + static_cast<std::ptrdiff_t>(offset);
        const auto last = first + static_cast<std::ptrdiff_t>(size);
        offset += size;
        return {first, last};
    }
    void finish() const {
        if (offset != bytes.size()) throw std::runtime_error("detector result has trailing bytes");
    }
private:
    void require(std::size_t count) const {
        if (count > bytes.size() - offset) throw std::runtime_error("detector result is truncated");
    }
    const std::vector<std::uint8_t>& bytes;
    std::size_t offset{0};
};

void validate_object(const DetectorObject3d& object) {
    if (object.objectId.empty()) throw std::invalid_argument("detector object requires objectId");
    for (const auto value : object.centerMeters) {
        if (!std::isfinite(value)) throw std::invalid_argument("detector center must be finite");
    }
    for (const auto value : object.dimensionsMeters) {
        if (!std::isfinite(value) || value <= 0.0) {
            throw std::invalid_argument("detector dimensions must be finite and positive");
        }
    }
    if (!std::isfinite(object.yawRadians) || !std::isfinite(object.confidence) ||
        object.confidence < 0.0 || object.confidence > 1.0) {
        throw std::invalid_argument("detector yaw/confidence is invalid");
    }
    if (!object.velocityMps.empty() && object.velocityMps.size() != 3) {
        throw std::invalid_argument("detector velocity must be empty or xyz");
    }
    if (object.covariance.size() > std::numeric_limits<std::uint16_t>::max()) {
        throw std::invalid_argument("detector covariance exceeds uint16 element count");
    }
    for (const auto value : object.velocityMps) {
        if (!std::isfinite(value)) throw std::invalid_argument("detector velocity must be finite");
    }
    for (const auto value : object.covariance) {
        if (!std::isfinite(value)) throw std::invalid_argument("detector covariance must be finite");
    }
}

void validate(const DetectorResultFrame& result, DetectorResultCodecLimits limits) {
    if (limits.maxObjects == 0 || limits.maxObjects > std::numeric_limits<std::uint32_t>::max() ||
        limits.maxEncodedBytes < 16) {
        throw std::invalid_argument("invalid detector codec limits");
    }
    if (result.frameId.empty() || result.captureTimeUnixNs == 0 ||
        result.coordinateFrameId.empty()) {
        throw std::invalid_argument("detector result requires frame and capture identities");
    }
    if (static_cast<std::uint8_t>(result.coordinateFrame) >
        static_cast<std::uint8_t>(DetectorCoordinateFrame::MAP)) {
        throw std::invalid_argument("unknown detector coordinate frame");
    }
    if (result.objects.size() > limits.maxObjects) {
        throw std::invalid_argument("detector object count exceeds configured maximum");
    }
    for (const auto& object : result.objects) validate_object(object);
}

} // namespace

std::vector<std::uint8_t> encode_detector_result(
    const DetectorResultFrame& result, DetectorResultCodecLimits limits) {
    validate(result, limits);
    Writer writer;
    writer.bytes.insert(writer.bytes.end(), kMagic.begin(), kMagic.end());
    writer.u8(kVersion);
    writer.string(result.frameId);
    writer.u64(result.captureTimeUnixNs);
    writer.u8(static_cast<std::uint8_t>(result.coordinateFrame));
    writer.string(result.coordinateFrameId);
    writer.u32(static_cast<std::uint32_t>(result.objects.size()));
    for (const auto& object : result.objects) {
        writer.string(object.objectId);
        writer.u16(object.classId);
        for (const auto value : object.centerMeters) writer.f64(value);
        for (const auto value : object.dimensionsMeters) writer.f64(value);
        writer.f64(object.yawRadians);
        writer.f64(object.confidence);
        writer.u8(object.velocityMps.empty() ? 0U : 1U);
        for (const auto value : object.velocityMps) writer.f64(value);
        writer.u16(static_cast<std::uint16_t>(object.covariance.size()));
        for (const auto value : object.covariance) writer.f64(value);
    }
    if (writer.bytes.size() > limits.maxEncodedBytes) {
        throw std::length_error("detector result exceeds configured encoded-size maximum");
    }
    return writer.bytes;
}

DetectorResultFrame decode_detector_result(
    const std::vector<std::uint8_t>& encoded, DetectorResultCodecLimits limits) {
    if (encoded.size() > limits.maxEncodedBytes || encoded.size() < 5 ||
        !std::equal(kMagic.begin(), kMagic.end(), encoded.begin())) {
        throw std::runtime_error("invalid detector result header or size");
    }
    Reader reader(encoded);
    for (const auto expected : kMagic) {
        if (reader.u8() != expected) throw std::runtime_error("invalid detector result magic");
    }
    if (reader.u8() != kVersion) throw std::runtime_error("unsupported detector result version");
    DetectorResultFrame result;
    result.frameId = reader.string();
    result.captureTimeUnixNs = reader.u64();
    const auto coordinateFrame = reader.u8();
    if (coordinateFrame > static_cast<std::uint8_t>(DetectorCoordinateFrame::MAP)) {
        throw std::runtime_error("unknown detector coordinate frame");
    }
    result.coordinateFrame = static_cast<DetectorCoordinateFrame>(coordinateFrame);
    result.coordinateFrameId = reader.string();
    const auto count = reader.u32();
    if (count > limits.maxObjects) throw std::runtime_error("detector object count exceeds limit");
    result.objects.reserve(count);
    for (std::uint32_t index = 0; index < count; ++index) {
        DetectorObject3d object;
        object.objectId = reader.string();
        object.classId = reader.u16();
        for (auto& value : object.centerMeters) value = reader.f64();
        for (auto& value : object.dimensionsMeters) value = reader.f64();
        object.yawRadians = reader.f64();
        object.confidence = reader.f64();
        const auto velocity = reader.u8();
        if (velocity > 1U) throw std::runtime_error("invalid detector velocity flag");
        if (velocity) {
            object.velocityMps.resize(3);
            for (auto& value : object.velocityMps) value = reader.f64();
        }
        const auto covarianceCount = reader.u16();
        object.covariance.resize(covarianceCount);
        for (auto& value : object.covariance) value = reader.f64();
        result.objects.push_back(std::move(object));
    }
    reader.finish();
    validate(result, limits);
    return result;
}

DetectorResultFrame select_top_k_detector_objects(
    DetectorResultFrame result, std::size_t maximumObjects) {
    std::stable_sort(result.objects.begin(), result.objects.end(),
        [](const DetectorObject3d& first, const DetectorObject3d& second) {
            if (first.confidence != second.confidence) return first.confidence > second.confidence;
            return first.objectId < second.objectId;
        });
    if (result.objects.size() > maximumObjects) result.objects.resize(maximumObjects);
    return result;
}

} // namespace ipi::api
