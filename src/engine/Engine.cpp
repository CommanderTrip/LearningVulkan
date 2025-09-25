
#include "Engine.hpp"

namespace pve {

    void Engine::mainLoop() {
        while (!_window.shouldClose()) {
            glfwPollEvents();
        }
    }

}