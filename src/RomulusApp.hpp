#include "engine/PveWindow.hpp"

namespace pve {

    class RomulusApp {
        PveWindow _window{WIDTH, HEIGHT, WINDOW_NAME};

    public:
        static constexpr int WIDTH = 600;
        static constexpr int HEIGHT = 400;
        static constexpr std::string WINDOW_NAME = "Romulus";

        void run() {}
    };

}