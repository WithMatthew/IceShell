#include <vle/core/IceVulkan.hpp>
#include <vle/core/engine_info.hpp>

using namespace vle;

int main() {
  auto vk_instance = IceVulkan();
  if (!vk_instance.IsValid()) return 1;
  return 0;
}
