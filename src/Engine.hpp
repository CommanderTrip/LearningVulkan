#pragma once
#include <GLFW/glfw3.h>

namespace pve {
    class Engine {
        const int WIDTH = 600;
        const int HEIGHT = 400;
        const char *WINDOW_NAME = "Learning Graphics";

        GLFWwindow *_window = nullptr;

    public:
        Engine();
        ~Engine();

        void run() const;
        void draw() const;
    };
}
