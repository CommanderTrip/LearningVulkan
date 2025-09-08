#pragma once
#include <vulkan/vulkan_core.h>

#include <vector>

#include "PveDebugger.hpp"

namespace pve {
    class PveVulkanConnection {
        VkInstance _instance;
        PveDebugger _debugger;

        std::vector<const char *> _getRequiredExtensions();
        bool _areRequiredExtSupported(const char **requiredExtensions, uint32_t requiredExtensionCount);

    public:
        PveVulkanConnection(const char *appName, const char *engineName);
        ~PveVulkanConnection() { vkDestroyInstance(_instance, nullptr); }
    };
}
