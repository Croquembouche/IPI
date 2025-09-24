#pragma once

#include <cstdint>
#include <string>

namespace ipi {

struct ValidationResult {
    bool ok{true};
    std::string message{};

    static ValidationResult success();
    static ValidationResult failure(std::string msg);
};

} // namespace ipi
