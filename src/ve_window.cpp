#include "ve_window.hpp"

#include <stdexcept>

namespace ve {

VeWindow::VeWindow(int width, int height, std::string windowName)
    : _width(width), _height(height), _windowName(std::move(windowName)) {
    _window = initWindow();
}

GLFWwindow* VeWindow::initWindow() const {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Not an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);    // Disable resizing until we can handle it properly
    return glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
}

VeWindow::~VeWindow() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}
void VeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface");
}

}  // namespace ve