#include "shared_state.hpp"

#include "ipi/api/in_memory_api.hpp"

#include <memory>

namespace ipi::api {

InMemoryApiPair make_in_memory_api_pair() {
    auto state = std::make_shared<detail::SharedState>();
    return {detail::make_receiver_for_state(state),
            detail::make_sender_for_state(std::move(state))};
}

} // namespace ipi::api
