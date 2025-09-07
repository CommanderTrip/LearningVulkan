#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace ve {

class VeWindow {
    const int _width;
    const int _height;
    std::string _windowName;
    GLFWwindow *_window = nullptr;

    [[nodiscard]] GLFWwindow *initWindow() const;

public:
    VeWindow(int width, int height, std::string windowName);
    VeWindow(const VeWindow &) = delete;
    VeWindow &operator=(const VeWindow &) = delete;

    ~VeWindow();

    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(_window); }
};

}  // namespace ve