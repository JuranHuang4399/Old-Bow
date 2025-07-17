/**
 * @file main.cpp
 * @brief Entry point of the game
 * Initialize application and run the main loop
 */


#define SDL_MAIN_HANDLED ///< fixes the WinMain() problem in windows system
#include "Application.hpp"
#include <iostream>

int main(int argc, char* argv[]) {

    try {
        Application app;
        app.startUp();
        app.loop(60);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error:" << e.what() << std::endl;
        return 1;
    }
    return 0;
}
