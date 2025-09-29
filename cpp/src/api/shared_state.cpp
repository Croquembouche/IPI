#include "shared_state.hpp"

#include <memory>

namespace ipi::api::detail {

std::shared_ptr<SharedState> obtain_shared_state()
{
    static auto state = std::make_shared<SharedState>();
    return state;
}

} // namespace ipi::api::detail
