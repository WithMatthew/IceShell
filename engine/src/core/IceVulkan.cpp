#include "vle/core/IceVulkan.hpp"

#include <cstddef>
#include <iostream>

namespace vle {

static const char* IceVulkanLayers[] = {"VK_LAYER_KHRONOS_validation"};

IceVulkan::IceVulkan() noexcept { InitVulkan(); }

IceVulkan::~IceVulkan() {
  if (IsValid()) DestroyVulkan();
  _vk_instance = VK_NULL_HANDLE;
}

bool IceVulkan::IsValid() const noexcept {
  return _vk_instance != VK_NULL_HANDLE;
}

bool IceVulkan::InitVulkan() {
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "IceShell";
  app_info.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo vk_create_info = {};
  vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vk_create_info.pApplicationInfo = &app_info;
  vk_create_info.enabledExtensionCount = 0;
  vk_create_info.ppEnabledExtensionNames = nullptr;
  vk_create_info.enabledLayerCount = 1;
  vk_create_info.ppEnabledLayerNames = IceVulkanLayers;

  VkInstance instance = VK_NULL_HANDLE;
  VkResult ret = vkCreateInstance(&vk_create_info, nullptr, &instance);
  if (ret != VK_SUCCESS) {
    std::cerr << "Failed to create vk instance \n";
    return false;
  }
  _vk_instance = instance;

  std::cout << "Create VK Instance Success" << std::endl;
  return true;
}

void IceVulkan::DestroyVulkan() {
  vkDestroyInstance(_vk_instance, nullptr);
  std::cout << "Destroy VK Instance Sucess" << std::endl;
}

}  // namespace vle
