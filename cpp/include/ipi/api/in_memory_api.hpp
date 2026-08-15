#pragma once

#include "ipi/api/receiver.hpp"
#include "ipi/api/sender.hpp"

#include <memory>

namespace ipi::api {

struct InMemoryApiPair {
    std::shared_ptr<ReceiverApi> receiver{};
    std::shared_ptr<SenderApi> sender{};
};

/** Create one isolated receiver/sender store for a logical intersection. */
[[nodiscard]] InMemoryApiPair make_in_memory_api_pair();

} // namespace ipi::api
