#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace ipi {

enum class MessageType : std::uint8_t {
    BSM,
    SRM,
    TIM,
    MAP,
    SPAT,
    SSM,
    IpiCooperativeService
};

/**
 * Lightweight wrapper around a raw J2735 MessageFrame.
 *
 * The `payload` member contains the ASN.1 UPER-encoded byte stream. Helper
 * methods exist to annotate additional metadata (e.g., transport origin or
 * decoding hints).
 */
struct MessageFrame {
    MessageType type{MessageType::BSM};
    std::vector<std::uint8_t> payload{};
    std::map<std::string, std::string> annotations{};

    [[nodiscard]] std::string to_string() const;
};

[[nodiscard]] std::string to_string(MessageType type);

} // namespace ipi
