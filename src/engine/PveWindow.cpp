#include "PveWindow.hpp"

namespace pve {

    /// The `initWindow` function from the Vulkan guide
    PveWindow::PveWindow(const int width, const int height, const char *windowName) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Do not use an OpenGL Context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);    // Resizing takes special care. Disable for now
        _window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
    }

    PveWindow::~PveWindow() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}