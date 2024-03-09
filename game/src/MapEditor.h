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

    int lines;
    int columns;
    int pressedLine;
    int pressedColumn;

    std::vector<Tile*> tiles;
    Tile *firstSelectedTile;

    Rectangle colorPickerContainerRect;
    Rectangle colorPickerRect;

    void updateLinesAndColumns();
    void computePressedLineAndColumn( Vector2 mousePos, int *line, int *column ) const;
    void selectTile( Vector2 mousePos );
    void deselectTile( Vector2 mousePos );
    void deselectTiles();
    bool isTileSelected( Vector2 mousePos ) const;
    bool isTilePositionValid( int line, int column ) const;
    bool isMouseInsideEditor( const Vector2 *mousePos ) const;

public:

    MapEditor( Vector2 pos, Vector2 dim, GameWorld* gw );
    virtual ~MapEditor();

    void inputAndUpdate();
    void draw() override;

    Vector2& getPos();
    Vector2& getDim();
    Rectangle getRectangle() const;

};

