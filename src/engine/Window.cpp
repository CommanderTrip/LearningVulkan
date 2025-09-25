#include "Window.hpp"

namespace pve {

    /// The `initWindow` function from the Vulkan guide
    Window::Window(const int width, const int height, const char *windowName) {
        glfwInit();                                    // Initialize the GLFW Library
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Do not use an OpenGL Context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);    // Resizing takes special care. Disable for now
        _window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
    }

    Window::~Window() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}