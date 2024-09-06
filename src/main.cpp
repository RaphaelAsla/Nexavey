#include <iostream>

#include "core/Engine.h"

int main() {
    nex::Engine app{};

    try {
        app.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
