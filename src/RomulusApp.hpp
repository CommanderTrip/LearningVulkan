#pragma once

#include "engine/PveEntry.hpp"

namespace pve {

    class RomulusApp {
        static constexpr const char *WINDOW_NAME = "Romulus";
        PveEntry _engine{};

        void mainLoop() const;

    public:
        RomulusApp();
        ~RomulusApp();

        void run() const;
    };
}