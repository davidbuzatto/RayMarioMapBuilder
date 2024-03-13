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
    GameWorld *gw;

    int minLines;
    int maxLines;
    int lines;
    int previousLines;
    int pressedLine;

    int minColumns;
    int maxColumns;
    int columns;
    int previousColumns;
    int pressedColumn;

    Vector2 tileComposerDim;

    int startLine;
    int startColumn;
    int viewOffsetLine;
    int viewOffsetColumn;

    //std::vector<Tile*> tiles;
    std::vector<std::vector<Tile*>> layers;
    Tile *firstSelectedTile;
    int currentLayer;
    int maxLayers;

    Rectangle guiContainerRect;

    Rectangle colorPickerContainerRect;
    Rectangle colorPickerRect;
    Rectangle sliderAlphaRect;

    Rectangle spinnerCurrentLayerRect;
    Rectangle spinnerLinesRect;
    Rectangle spinnerColumnsRect;

    int previewTileWidth;
    Rectangle layersPreviewRect;

    Tile dummyTile;

    void computePressedLineAndColumn( Vector2 &mousePos, int &line, int &column ) const;
    void selectTile( Vector2 &mousePos );
    void deselectTile( Vector2 &mousePos );
    void deselectTiles();
    bool isTileSelected( Vector2 &mousePos ) const;
    bool isTilePositionValid( int line, int column ) const;
    bool isMouseInsideEditor( const Vector2 &mousePos ) const;

    void drawLayerPreview( int x, int y, int tileWidth, const std::vector<Tile*>& tiles ) const;

public:

    MapEditor( Vector2 pos, GameWorld* gw );
    virtual ~MapEditor();

    void inputAndUpdate();
    void draw() override;

    void relocateTiles( std::vector<Tile*> &tiles ) const;

};

