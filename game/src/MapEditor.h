/**
 * @file MapEditor.h
 * @author Prof. Dr. David Buzatto
 * @brief MapEditor class declaration.
 *
 * @copyright Copyright (c) 2024
 */

#pragma once
#include <vector>
#include <string>

#include "Drawable.h"
#include "raylib.h"
#include "Tile.h"

class GameWorld;

class MapEditor : public virtual Drawable {

    struct LayerState {
        bool visible;
    };

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

    std::vector<std::vector<Tile*>> layers;
    Tile *firstSelectedTile;
    int currentLayer;
    int maxLayers;

    Rectangle guiContainerRect;

    int previewTileWidth;
    Rectangle layersPreviewRect;
    std::vector<LayerState> layersState;

    Rectangle toogleGroupInsertRect;
    Rectangle checkShowGridRect;
    Rectangle checkPlayMusicRect;
    int activeInsertOption;

    Rectangle mapPropertiesRect;
    Rectangle spinnerLinesRect;
    Rectangle spinnerColumnsRect;
    Rectangle labelBackgroundColorRect;
    Rectangle colorPickerBackgroundColorRect;
    Rectangle spinnerBackgroundTextureIdRect;
    Rectangle spinnerMusicIdRect;
    Rectangle spinnerTimeToFinishRect;
    bool linesEdit{ false };
    bool columnsEdit{ false };
    bool terrainPageEdit{ false };
    bool pipesPageEdit{ false };
    bool timeToFinishEdit{ false };
    int currentTerrainTile{ 0 };
    int currentPipeColor{ 0 };
    

    Rectangle componentPropertiesRect;

    Rectangle comboTileCollisionTypeRect;
    Rectangle togglePaintingTypeRect;
    Rectangle colorPickerTileContainerRect;
    Rectangle colorPickerTileRect;
    Rectangle sliderAlphaTileRect;
    Rectangle checkVisibleRect;
    int tileCollisionType;
    int tilePaintingType;
    bool tileVisible;
    bool tileCollisionTypeEdit{ false };

    std::vector<std::string> pipeColors{ "blue", "darkgray", "gray", "green", "orange", "pink", "purple", "red", "yellow" };
    std::string pipeColorOptions;

    Tile dummyTile;

    // map properties
    Color backgroundColor;
    int backgroundTextureId;
    int musicId;
    int timeToFinish;
    bool showGrid;
    bool playMusic;
    int previousSelectedMusicId;

    // component rectangles and helper attributes for GUI construction and interaction
    Rectangle terrainRect;
    Rectangle pipesRect;
    Rectangle* selectedTileRect;
    std::vector<Rectangle> tilesRects;
    std::vector<Rectangle> pipesRects;


    void computePressedLineAndColumn( Vector2 &mousePos, int &line, int &column ) const;
    void selectTile( Vector2 &mousePos );
    void deselectTile( Vector2 &mousePos );
    void deselectTiles();
    bool isTileSelected( Vector2 &mousePos ) const;
    bool isTilePositionValid( int line, int column ) const;
    bool isMouseInsideEditor( const Vector2 &mousePos ) const;

    void drawLayerPreview( int x, int y, int tileWidth, bool active, const std::vector<Tile*>& tiles ) const;

public:

    MapEditor( Vector2 pos, GameWorld* gw );
    virtual ~MapEditor();

    void inputAndUpdate();
    void draw() override;

    void relocateTiles( std::vector<Tile*> &tiles ) const;

};

