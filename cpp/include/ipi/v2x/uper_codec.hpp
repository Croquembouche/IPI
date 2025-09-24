#pragma once

#include "ipi/v2x/j2735_messages.hpp"

#include <vector>

namespace ipi::v2x {

/**
 * Encoder/decoder for the simplified J2735 message helpers using ASN.1 UPER
 * bit packing. The implementation focuses on the fields surfaced in the helper
 * structs; additional fields can be added incrementally while preserving
 * backward compatibility.
 */
class UperCodec {
public:
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::BasicSafetyMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::MapMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::SpatMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::SignalRequestMessage& msg) const;
    [[nodiscard]] std::vector<std::uint8_t> encode(const j2735::SignalStatusMessage& msg) const;

    [[nodiscard]] j2735::BasicSafetyMessage decode_bsm(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::MapMessage decode_map(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::SpatMessage decode_spat(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::SignalRequestMessage decode_srm(const std::vector<std::uint8_t>& buffer) const;
    [[nodiscard]] j2735::SignalStatusMessage decode_ssm(const std::vector<std::uint8_t>& buffer) const;
};

} // namespace ipi::v2x
