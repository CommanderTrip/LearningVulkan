#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace pve {

    class PveWindow {
        GLFWwindow *_window;
        const int _width;
        const int _height;
        const std::string _windowName;

        GLFWwindow *initWindow() const;

    public:
        PveWindow(int width, int height, std::string windowName);
        PveWindow(const PveWindow &) = delete;
        PveWindow &operator=(const PveWindow &) = delete;
        ~PveWindow();

        [[nodiscard]] bool shouldClose() const;
    };

}