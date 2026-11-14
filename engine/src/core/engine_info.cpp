#include <vle/core/engine_info.hpp>

namespace vle {

std::string_view engine_name() noexcept
{
    // The literal outlives every caller; this view does not own its characters.
    return "Vulkan Learning Engine";
}

} // namespace vle
