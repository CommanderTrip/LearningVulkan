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

    void Engine::run() {
        _createVertexBuffer();

        // The 0 is the index of the attribute
        // Because we've attributed the vertex buffer, OpenGL knows the vec4 passed in is actually a vec2 but it'll need
        // a vec4 for gl_Position, so it will handle that conversion for us.
        const std::string vertexShader =
            "#version 330 core\n"
            "\n"
            "layout(location = 0) in vec4 position;\n"
            "\n"
            "void main() {\n"
            "   gl_Position = position;\n"
            "}\n";

        const std::string fragmentShader =
            "#version 330 core\n"
            "\n"
            "layout(location = 0) out vec4 color;\n"
            "\n"
            "void main() {\n"
            "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
            "}\n";
        uint32_t shaderProgramId = _createShader(vertexShader, fragmentShader);
        glUseProgram(shaderProgramId);

        while (!glfwWindowShouldClose(_window)) {
            draw();
            glfwPollEvents();
        }
    }

    void Engine::draw() {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(_window);
    }

    void Engine::_createVertexBuffer() {
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
        glEnableVertexAttribArray(0);
    }

    /**
     * Gives OpenGL the source code to the shaders, and makes it compile and link them into a single shader program.
     *
     * @param vertexShader Vertex shader source code.
     * @param fragmentShader Fragment shader source code.
     * @return An ID for the shader program.
     */
    uint32_t Engine::_createShader(const std::string &vertexShader, const std::string &fragmentShader) {
        uint32_t programId = glCreateProgram();

        const uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        const char *vertexSource = vertexShader.c_str();
        glShaderSource(vertexShaderId, 1, &vertexSource, nullptr);
        glCompileShader(vertexShaderId);

        const uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        const char *fragmentSource = fragmentShader.c_str();
        glShaderSource(fragmentShaderId, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShaderId);

        // Do some error handling
        int result;
        glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
        if (GL_FALSE == result) {
            int messageLength;
            glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &messageLength);
            char message[messageLength];
            glGetShaderInfoLog(vertexShaderId, messageLength, &messageLength, message);
            std::cerr << "Failed to compile vertex shader:\n" << message << std::endl;
            glDeleteShader(vertexShaderId);
            return 0;
        }

        glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
        if (GL_FALSE == result) {
            int messageLength;
            glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &messageLength);
            char message[messageLength];
            glGetShaderInfoLog(fragmentShaderId, messageLength, &messageLength, message);
            std::cerr << "Failed to compile fragment shader:\n" << message << std::endl;
            glDeleteShader(fragmentShaderId);
            return 0;
        }

        // Begin linking
        glAttachShader(programId, vertexShaderId);
        glAttachShader(programId, fragmentShaderId);
        glLinkProgram(programId);
        glValidateProgram(programId);

        // We've compiled and linked so we can get rid of the intermediate objects
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        return programId;
    }
}