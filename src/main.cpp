#include "first_app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {

    ember::FirstApp app{};

    try{
        app.Run();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}