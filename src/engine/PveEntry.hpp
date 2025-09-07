#pragma once
#include "PveVulkanConnection.hpp"
#include "PveWindow.hpp"

namespace pve {

    class PveEntry {
        PveWindow _window{WIDTH, HEIGHT, WINDOW_NAME};
        PveVulkanConnection _vkConnection{};

    public:
        static constexpr int WIDTH = 600;
        static constexpr int HEIGHT = 400;
        static constexpr const char *ENGINE_NAME = "Primus Vulkan Engine";

        PveEntry();
        ~PveEntry();
    };

}