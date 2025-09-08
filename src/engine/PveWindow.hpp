#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace pve {

    class PveWindow {
        GLFWwindow *_window = nullptr;

    public:
        PveWindow(int width, int height, const char *windowName);
        PveWindow(const PveWindow &) = delete;
        PveWindow &operator=(const PveWindow &) = delete;
        ~PveWindow();

        [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(_window); };
    };

}