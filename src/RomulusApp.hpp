#pragma once

#include "engine/PveEntry.hpp"

namespace pve {

    class RomulusApp {
        PveEntry _engine{};

    public:
        RomulusApp() = default;
        ~RomulusApp() = default;

        void run() { _engine.mainLoop(); }
    };
}