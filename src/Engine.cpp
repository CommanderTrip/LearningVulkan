#include "Engine.hpp"

#include <iostream>
#include <stdexcept>

namespace pve {
    Engine::Engine() {
        // Start GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        _window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, nullptr, nullptr);
        if (nullptr == _window) throw std::runtime_error("Failed to Create GLFW Window.");
        glfwMakeContextCurrent(_window);

        std::cout << "OpenGL and Driver Version: " << glGetString(GL_VERSION) << std::endl;

        // Start GLEW
        if (glewInit() != GLEW_OK) throw std::runtime_error("GLEW failed to load.");

        // Start First render
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