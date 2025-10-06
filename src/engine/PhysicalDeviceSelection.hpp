#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>

namespace pve {
    class PhysicalDeviceSelection {
        const char *name = nullptr;
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;  // Implicitly destroyed with the VkInstance

        // We want to optionally determine if the physical device has this queue family
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;

            bool isComplete() const { return graphicsFamily.has_value(); }
        };

        static bool _isDeviceSuitable(VkPhysicalDevice device);
        static QueueFamilyIndices _findQueueFamilies(VkPhysicalDevice device);

    public:
        PhysicalDeviceSelection() = default;
        ~PhysicalDeviceSelection() = default;
        void pickPhysicalDevice(VkInstance &instance);
    };
}
