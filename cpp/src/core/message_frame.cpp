#include "ipi/core/message_frame.hpp"

#include <iomanip>
#include <sstream>

namespace ipi {

std::string to_string(MessageType type) {
    switch (type) {
        case MessageType::BSM:
            return "BSM";
        case MessageType::SRM:
            return "SRM";
        case MessageType::TIM:
            return "TIM";
        case MessageType::MAP:
            return "MAP";
        case MessageType::SPAT:
            return "SPAT";
        case MessageType::SSM:
            return "SSM";
        case MessageType::IpiCooperativeService:
            return "IPI-CooperativeService";
        default:
            return "unknown";
    }
}

std::string MessageFrame::to_string() const {
    std::ostringstream oss;
    oss << "MessageFrame{type=" << ipi::to_string(type) << ", payload=" << payload.size() << " bytes";
    if (!annotations.empty()) {
        oss << ", annotations={";
        bool first = true;
        for (const auto& [key, value] : annotations) {
            if (!first) {
                oss << ", ";
            }
            first = false;
            oss << key << ':' << value;
        }
        oss << '}';
    }
    oss << '}';
    return oss.str();
}

} // namespace ipi
