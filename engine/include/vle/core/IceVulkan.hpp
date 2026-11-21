#pragma once
#include <vulkan/vulkan.h>

namespace vle {

class IceVulkan {
 public:
  IceVulkan() noexcept;
  virtual ~IceVulkan();

 private:
  bool InitVulkan();
  void DestroyVulkan();

 private:
  VkInstance _vk_instance;
};
}  // namespace vle
