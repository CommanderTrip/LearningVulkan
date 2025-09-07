#include <iostream>

#include "TheApp.hpp"

int main() {
    ve::TheApp app;

    try {
        app.run();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}