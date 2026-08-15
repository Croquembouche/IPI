#pragma once

#include "ipi/core/ipi_cooperative_service.hpp"
#include "ipi/v2x/j2735_messages.hpp"

#include <vector>

namespace ipi::v2x {

/**
 * Encoder/decoder facade used by the J2735 message helpers.
 *
 * IPI cooperative-service values use the formal TestMessage00 regional profile
 * in cpp/asn1/IPI.asn. The legacy BSM/PSM/MAP/SPAT/SRM/SSM helper overloads are
 * project field profiles and must not be presented as complete generated SAE
 * J2735 codecs.
 */
class UperCodec {
public:
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::BasicSafetyMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::PersonalSafetyMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::MapMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::SpatMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::SignalRequestMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::SignalStatusMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const CooperativeServiceMessage& msg) const;

    [[nodiscard]] j2735::BasicSafetyMessage decode_bsm(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::PersonalSafetyMessage decode_psm(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::MapMessage decode_map(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::SpatMessage decode_spat(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::SignalRequestMessage decode_srm(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::SignalStatusMessage decode_ssm(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] CooperativeServiceMessage decode_ipi_cooperative_service(
        const std::vector<std::uint8_t>& buffer) const;
};

} // namespace ipi::v2x
