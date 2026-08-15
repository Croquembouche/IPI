#pragma once

#include "ipi/core/ipi_cooperative_service.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ipi::v2x {

struct J2735IpiRegionalProfile {
    static constexpr std::uint16_t kTestMessageId = 240;
    static constexpr std::uint8_t kDefaultRegionId = 200;
    static constexpr std::uint8_t kSupportedProfileVersion = 1;
    static constexpr std::size_t kPortableFrameLimit = 2048;

    std::uint8_t regionId{kDefaultRegionId};
    std::uint8_t profileVersion{kSupportedProfileVersion};
    std::size_t maxFrameBytes{kPortableFrameLimit};

    void validate() const;
};

/**
 * SAE J2735 regional-extension codec for IPI cooperative service messages.
 *
 * The outer value is a J2735 MessageFrame containing TestMessage00
 * (DSRCmsgID 240). TestMessage00 carries the IPI value at its regional
 * extension point. The payload follows cpp/asn1/IPI.asn and uses unaligned
 * Packed Encoding Rules (UPER).
 *
 * The implementation deliberately supports only the declared IPI profile.
 * Unknown ASN.1 extensions, a non-local RegionId, a different test message,
 * malformed lengths, non-zero padding, and values outside the schema
 * constraints are rejected.
 */
class J2735IpiRegionalCodec {
public:
    explicit J2735IpiRegionalCodec(J2735IpiRegionalProfile profile = {});

    [[nodiscard]] const J2735IpiRegionalProfile& profile() const noexcept;

    [[nodiscard]] std::vector<std::uint8_t> encode_regional_value(
        const CooperativeServiceMessage& message) const;
    [[nodiscard]] CooperativeServiceMessage decode_regional_value(
        const std::vector<std::uint8_t>& encoded) const;

    [[nodiscard]] std::vector<std::uint8_t> encode_message_frame(
        const CooperativeServiceMessage& message) const;
    [[nodiscard]] CooperativeServiceMessage decode_message_frame(
        const std::vector<std::uint8_t>& encoded) const;

private:
    J2735IpiRegionalProfile profile_{};
};

} // namespace ipi::v2x
