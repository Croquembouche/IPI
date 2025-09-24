#include "ipi/core/validation.hpp"

namespace ipi {

ValidationResult ValidationResult::success() {
    return ValidationResult{true, {}};
}

ValidationResult ValidationResult::failure(std::string msg) {
    return ValidationResult{false, std::move(msg)};
}

} // namespace ipi
