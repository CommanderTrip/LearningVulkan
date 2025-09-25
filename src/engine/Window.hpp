#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace pve {

    class Window {
        GLFWwindow *_window = nullptr;

    public:
        Window(int width, int height, const char *windowName);
        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        ~Window();

        [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(_window); };
    };

}