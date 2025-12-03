#pragma once

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>

namespace ipi::debug {

inline bool enabled() {
    static bool cached = [] {
        const char* env = std::getenv("IPI_DEBUG");
        if (!env) {
            return false;
        }
        std::string_view v(env);
        return !v.empty() && v != "0" && v != "false" && v != "off";
    }();
    return cached;
}

template <typename... Args>
inline void log(Args&&... args) {
    if (!enabled()) {
        return;
    }
    (std::clog << ... << std::forward<Args>(args)) << '\n';
}

inline std::string hex(const std::vector<std::uint8_t>& data) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            oss << ' ';
        }
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    return oss.str();
}

} // namespace ipi::debug
