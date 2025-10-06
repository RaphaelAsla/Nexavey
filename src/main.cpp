#include <cstdlib>
#include <iostream>

#include "engine/core/nex_engine.hpp"

int main() {
    nex::NexEngine app{};

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "An ERROR occurred: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
