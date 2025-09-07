#include "RomulusApp.hpp"

namespace pve {
    RomulusApp::RomulusApp() {}
    RomulusApp::~RomulusApp() {}

    void RomulusApp::mainLoop() const {
        while (!_window.shouldClose()) {
            glfwPollEvents();
        }
    }

    void RomulusApp::run() const {
        // initWindow - done with `_window` initialization
        // initVulkan - done with `_vkConnection` initialization
        mainLoop();
        // cleanup will be handled as each object get destroyed
    }

}