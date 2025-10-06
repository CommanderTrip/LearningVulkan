
#include "Engine.hpp"

namespace pve {

    /**
     * Main loop of the program. Application will perpetually loop here.
     */
    void Engine::mainLoop() const {
        while (!_window.shouldClose()) {
            glfwPollEvents();
        }
    }

}