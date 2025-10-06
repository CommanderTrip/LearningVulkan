#pragma once
#include "VulkanConnection.hpp"
#include "Window.hpp"

namespace pve {

    class Engine {
        // Destruction happens in the reverse order of construction thankfully :)
        Window _window{WIDTH, HEIGHT, WINDOW_NAME};                // initWindow
        VulkanConnection _vkConnection{WINDOW_NAME, ENGINE_NAME};  // initVulkan

    public:
        static constexpr int WIDTH = 600;
        static constexpr int HEIGHT = 400;
        static constexpr const char *ENGINE_NAME = "Primus Vulkan Engine";
        static constexpr const char *WINDOW_NAME = "Romulus";

        Engine() = default;
        ~Engine() = default;

        void mainLoop() const;
    };

}