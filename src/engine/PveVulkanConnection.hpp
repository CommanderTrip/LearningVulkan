#pragma once
#include <vulkan/vulkan_core.h>

namespace pve {
    class PveVulkanConnection {
        VkInstance _instance;

        void createInstance();

    public:
        PveVulkanConnection();
    };
}
