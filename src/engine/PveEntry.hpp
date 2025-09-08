#pragma once
#include "PveVulkanConnection.hpp"
#include "PveWindow.hpp"

namespace pve {

    class PveEntry {
        // Destruction happens in the reverse order of construction thankfully :)
        PveWindow _window{WIDTH, HEIGHT, WINDOW_NAME};
        PveVulkanConnection _vkConnection{WINDOW_NAME, ENGINE_NAME};

    public:
        static constexpr int WIDTH = 600;
        static constexpr int HEIGHT = 400;
        static constexpr const char *ENGINE_NAME = "Primus Vulkan Engine";
        static constexpr const char *WINDOW_NAME = "Romulus";

        PveEntry() = default;
        ~PveEntry() = default;

        void mainLoop();
    };

}