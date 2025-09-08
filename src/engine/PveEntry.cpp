
#include "PveEntry.hpp"

namespace pve {

    void PveEntry::mainLoop() {
        while (!_window.shouldClose()) {
            glfwPollEvents();
        }
    }

}