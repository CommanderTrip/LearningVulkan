#pragma once

#include "engine/Engine.hpp"

namespace romulus {

    class RomulusApp {
        pve::Engine _engine{};

    public:
        RomulusApp() = default;
        ~RomulusApp() = default;

        void run() { _engine.mainLoop(); }
    };
}