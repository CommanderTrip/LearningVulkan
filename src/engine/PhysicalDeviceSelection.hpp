#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>

#include "Debugger.hpp"

namespace pve {
    class PhysicalDeviceSelection {
        Debugger &_debugger;
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;  // Implicitly destroyed with the VkInstance
        VkDevice _device = VK_NULL_HANDLE;                  // Logical device that interfaces with the physical device

        // We want to optionally determine if the physical device has this queue family
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;

            bool isComplete() const { return graphicsFamily.has_value(); }
        };

        static bool _isDeviceSuitable(VkPhysicalDevice device);
        static QueueFamilyIndices _findQueueFamilies(VkPhysicalDevice device);

    public:
        VkQueue graphicsQueue;  // All queues are implicitly destroyed with the VkInstance

        explicit PhysicalDeviceSelection(Debugger &debugger) : _debugger(debugger) {};
        ~PhysicalDeviceSelection() { vkDestroyDevice(_device, nullptr); };
        void pickPhysicalDevice(VkInstance &instance);
        void pickLogicalDevice();
    };
}
