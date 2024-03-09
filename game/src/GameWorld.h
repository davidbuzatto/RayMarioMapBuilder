/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld class declaration. This class should contain all
 * game components and its state.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "Drawable.h"
#include "MapEditor.h"
#include "Tile.h"

class GameWorld : public virtual Drawable {

    MapEditor mapEditor;
    Tile *selectedTile;
    
public:

    GameWorld();
    virtual ~GameWorld();

    void inputAndUpdate();
    void draw() override;

    static void loadResources();
    static void unloadResources();
    
};
