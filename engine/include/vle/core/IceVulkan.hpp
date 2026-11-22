#pragma once
#include <vulkan/vulkan.h>

namespace vle {

class IceVulkan {
 public:
  IceVulkan() noexcept;
  virtual ~IceVulkan();

  IceVulkan(IceVulkan&) = delete;
  IceVulkan& operator=(const IceVulkan&) = delete;

  bool IsValid() const noexcept;

 private:
  bool InitVulkan();
  void DestroyVulkan();

 private:
  VkInstance _vk_instance = VK_NULL_HANDLE;
};
}  // namespace vle
