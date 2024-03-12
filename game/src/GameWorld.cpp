/**
 * @file GameWorld.cpp
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld class implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <iostream>
#include <string>

#include "GameWorld.h"
#include "ResourceManager.h"
#include "raylib.h"

GameWorld::GameWorld()
    :
    mapEditor( Vector2( 10, 10 ), this ),
    selectedTile( nullptr ) {
}

GameWorld::~GameWorld() = default;

void GameWorld::inputAndUpdate() {
    mapEditor.inputAndUpdate();
}

void GameWorld::draw() {

    BeginDrawing();
    ClearBackground( WHITE );

    mapEditor.draw();

    EndDrawing();

}

void GameWorld::loadResources() {
    ResourceManager::loadTextures();
    ResourceManager::loadSounds();
    ResourceManager::loadMusics();
}

void GameWorld::unloadResources() {
    ResourceManager::unloadTextures();
    ResourceManager::unloadSounds();
    ResourceManager::unloadMusics();
}