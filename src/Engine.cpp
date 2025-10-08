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
    }

    Engine::~Engine() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void Engine::run() const {
        // Create some Vertex Buffer
        uint32_t bufferId;
        glGenBuffers(1, &bufferId);

        // Select the buffer
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);

        // Put data in the buffer
        const float positions[6] = {-0.5f, -0.5, 0.0f, 0.5f, 0.5f, -0.5f};
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, positions, GL_STATIC_DRAW);

        // Set Attributes of the buffer
        /*
         * We only have 1 attribute in our vertices (position) so we only need to specify this once.
         * Index: Only 1 attribute so specify it as the first - 0
         * Size: How many components of this attribute (2 for x and y)
         * Type: Data type we used
         * Normalized: Does the data need to be normalized?
         * Stride: Amount of bytes for each vertex
         * Pointer: Offset from the start of the vertex to this attribute
         */
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
        glEnableVertexAttribArray(bufferId);

        while (!glfwWindowShouldClose(_window)) {
            draw();
            glfwPollEvents();
        }
    }

    void Engine::draw() const {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(_window);
    }
}