#pragma once
#include <vulkan/vulkan_core.h>

#include <vector>

#include "Debugger.hpp"

namespace pve {
    class VulkanConnection {
        VkInstance _instance;
        Debugger _debugger;

        void _createInstance(const char *appName, const char *engineName);
        void _setupDebugMessenger();

        std::vector<const char *> _getRequiredExtensions();
        bool _areRequiredExtSupported(const char **requiredExtensions, uint32_t requiredExtensionCount);
        void _populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    public:
        VulkanConnection(const char *appName, const char *engineName);
        ~VulkanConnection();
    };
}
