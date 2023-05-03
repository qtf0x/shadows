/**
 * @file main.cpp
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#include "Engine.hpp"

int main() {
    auto engine{new Engine()};

    engine->initialize();
    engine->run();
    engine->shutdown();

    delete engine;
    engine = nullptr;
}
