#pragma once

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace ipi::tests {

inline void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

inline int run_test(const char* name, const std::function<void()>& testBody) {
    try {
        testBody();
        std::cout << "[PASS] " << name << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[FAIL] " << name << ": " << ex.what() << '\n';
        return 1;
    }
}

} // namespace ipi::tests
