#include "vle/core/IceVulkan.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <vector>

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
    uint32_t count = 0;
    VkResult layer_ret = vkEnumerateInstanceLayerProperties(&count, nullptr);
    if (layer_ret != VK_SUCCESS) {
        std::cerr << "Get vulkan layer count failed, VkResult = "
                  << static_cast<int>(layer_ret) << '\n';
        return false;
    }
    std::vector<VkLayerProperties> layers(count);
    layer_ret = vkEnumerateInstanceLayerProperties(&count, layers.data());
    if (layer_ret != VK_SUCCESS) {
        std::cerr << "Get Vulkan layer props failed, VkResult = "
                  << static_cast<int>(layer_ret) << '\n';
        return false;
    }

    for (auto propName : IceVulkanLayers) {
        const std::string_view required_layer{propName};
        const auto layer_it = std::find_if(
            layers.begin(), layers.end(),
            [required_layer](const VkLayerProperties& layer) {
                return std::string_view{layer.layerName} == required_layer;
            });
        if (layer_it == layers.end()) {
            std::cerr << "Missing required vulkan layer " << required_layer
                      << '\n';
            return false;
        }
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "IceShell";
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo vk_create_info = {};
    vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_create_info.pApplicationInfo = &app_info;
    vk_create_info.enabledExtensionCount = 0;
    vk_create_info.ppEnabledExtensionNames = nullptr;
    vk_create_info.enabledLayerCount =
        static_cast<uint32_t>(std::size(IceVulkanLayers));
    vk_create_info.ppEnabledLayerNames = IceVulkanLayers;

    VkInstance instance = VK_NULL_HANDLE;
    VkResult ret = vkCreateInstance(&vk_create_info, nullptr, &instance);
    if (ret != VK_SUCCESS) {
        std::cerr << "vkCreateInstance failed, VkResult="
                  << static_cast<int>(ret) << '\n';
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
