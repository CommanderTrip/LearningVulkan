#include "Window.hpp"

namespace pve {

    /**
     * The `initWindow` function from the Vulkan guide
     *
     * @param width The width of the program window in pixels.
     * @param height The height of the program window in pixels.
     * @param windowName The text in the title bar of the program window.
     */
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