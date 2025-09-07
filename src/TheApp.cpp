#include "TheApp.hpp"

namespace ve {

void TheApp::run() const {
    while (!_window.shouldClose()) {
        glfwPollEvents();
    }
}

}  // namespace ve