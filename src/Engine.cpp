#include "Engine.hpp"

#include <stdexcept>

namespace pve {
    Engine::Engine() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

        _window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, nullptr, nullptr);
        if (_window == nullptr) throw std::runtime_error("Failed to Create GLFW Window");
        glfwMakeContextCurrent(_window);

        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        draw();
    }

    Engine::~Engine() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void Engine::run() const {
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
        }
    }

    void Engine::draw() const {
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        glfwSwapBuffers(_window);
    }
}