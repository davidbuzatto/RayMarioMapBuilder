/**
 * @file main.cpp
 * @author Prof. Dr. David Buzatto
 * @brief Main function. Base template for game development in C++ using
 * Raylib (https://www.raylib.com/).
 * 
 * @copyright Copyright (c) 2024
 */
#include "GameWindow.h"

int main( void ) {

    GameWindow gameWindow(
        1280, 720,                 // dimensions
        "RayMario Map Builder",   // title
        60,                       // target FPS
        false,                    // antialiasing
        true,                     // resizable
        false,                    // fullscreen
        false,                    // undecorated
        false,                    // always on top
        false,                    // always run
        true );                   // init audio

    gameWindow.init();

    return 0;

}