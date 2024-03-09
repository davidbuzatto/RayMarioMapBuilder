/**
 * @file MapEditor.h
 * @author Prof. Dr. David Buzatto
 * @brief MapEditor class declaration.
 *
 * @copyright Copyright (c) 2024
 */

#pragma once
#include <vector>

#include "Drawable.h"
#include "raylib.h"
#include "Tile.h"

class GameWorld;

class MapEditor : public virtual Drawable {

    Vector2 pos;
    Vector2 dim;
    GameWorld *gw;

    int rows;
    int cols;

    std::vector<Tile*> tiles;
    std::vector<Tile*> selectedTiles;

    Rectangle colorPickerContainerRect;
    Rectangle colorPickerRect;
    Color *lastColor;

    void updateRowsAndCols();

public:

    MapEditor( Vector2 pos, Vector2 dim, GameWorld* gw );
    ~MapEditor();

    void inputAndUpdate();
    void draw() override;

    Vector2& getPos();
    Vector2& getDim();
    Rectangle getRectangle() const;

};

