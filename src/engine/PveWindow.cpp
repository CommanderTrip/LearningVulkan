#include "PveWindow.hpp"

namespace pve {
    PveWindow::PveWindow(int width, int height, const char *windowName)
        : _width(width), _height(height), _windowName(windowName) {
        _window = initWindow();
    }

    PveWindow::~PveWindow() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    GLFWwindow *PveWindow::initWindow() const {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Do not use an OpenGL Context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);    // Resizing takes special care. Disable for now
        return glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
    }

    bool PveWindow::shouldClose() const { return glfwWindowShouldClose(_window); }

}