
#include <iostream>
#include <stdexcept>

#include "Engine.hpp"

int main() {
    const pve::Engine engine{};

    try {
        engine.run();
    } catch (std::exception &ex) {
        std::cerr << ex.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
