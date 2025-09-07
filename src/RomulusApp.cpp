#include "RomulusApp.hpp"

namespace pve {
    RomulusApp::RomulusApp() {}
    RomulusApp::~RomulusApp() {}

    void RomulusApp::mainLoop() {
        while (!_window.shouldClose()) {
            glfwPollEvents();
        }
    }

    void RomulusApp::run() {
        // initWindow already done
        // TODO: initVulkan
        mainLoop();
        // cleanup will be handled as each object get destroyed
    }

}