/**
 * @file MapEditor.cpp
 * @author Prof. Dr. David Buzatto
 * @brief MapEditor class implementation.
 *
 * @copyright Copyright (c) 2024
 */
#include "GameWorld.h"
#include "MapEditor.h"
#include "ResourceManager.h"
#include "Tile.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "ComponentInsertionType.h"
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

MapEditor::MapEditor( Vector2 pos, GameWorld *gw )
    :
    pos( pos ),
    gw( gw ),

    minLines( 14 ),
    maxLines( 40 ),
    lines( minLines ),
    previousLines( minLines ),
    pressedLine( -1 ),

    minColumns( 18 ),
    maxColumns( 400 ),
    columns( minColumns ),
    previousColumns( minColumns ),
    pressedColumn( -1 ),

    tileComposerDim( Vector2( minColumns * Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH ) ),

    startLine( lines - minLines ),
    startColumn( columns - minColumns ),
    viewOffsetLine( 0 ),
    viewOffsetColumn( 0 ),
    
    firstSelectedTile( nullptr ),
    currentLayer( 1 ),
    maxLayers( 7 ),

    guiContainerRect( Rectangle( pos.x + tileComposerDim.x + 40, pos.y, 50, 50 ) ),

    previewTileWidth( 6 ),
    layersPreviewRect( Rectangle( guiContainerRect.x + 10, guiContainerRect.y + 20, previewTileWidth* minColumns + 20 + 30, maxLayers* ( previewTileWidth* minLines + 10 ) + 10 ) ),

    toogleGroupInsertRect( Rectangle( pos.x, pos.y + tileComposerDim.y + Tile::TILE_WIDTH + 10, 44, 44 ) ),
    checkShowGridRect( Rectangle( pos.x + minColumns * Tile::TILE_WIDTH - 80, pos.y + tileComposerDim.y + Tile::TILE_WIDTH + 10, 20, 20 ) ),
    checkPlayMusicRect( Rectangle( checkShowGridRect.x, checkShowGridRect.y + checkShowGridRect.height + 10, 20, 20 ) ),
    activeInsertOption( 0 ),

    mapPropertiesRect( Rectangle( layersPreviewRect.x + layersPreviewRect.width + 10, layersPreviewRect.y, 260, 270 )  ),
    spinnerLinesRect( Rectangle( mapPropertiesRect.x + 125, mapPropertiesRect.y + 10, 100, 20 ) ),
    spinnerColumnsRect( Rectangle( spinnerLinesRect.x, spinnerLinesRect.y + spinnerLinesRect.height + 10, 100, 20 ) ),
    labelBackgroundColorRect( Rectangle( spinnerColumnsRect.x - 100, spinnerColumnsRect.y + spinnerColumnsRect.height + 50, 100, 20 ) ),
    colorPickerBackgroundColorRect( Rectangle( spinnerColumnsRect.x, spinnerColumnsRect.y + spinnerColumnsRect.height + 10, 100, 100 )  ),
    spinnerBackgroundTextureIdRect( Rectangle( colorPickerBackgroundColorRect.x, colorPickerBackgroundColorRect.y + colorPickerBackgroundColorRect.height + 10, 100, 20 ) ),
    spinnerMusicIdRect( Rectangle( spinnerBackgroundTextureIdRect.x, spinnerBackgroundTextureIdRect.y + spinnerBackgroundTextureIdRect.height + 10, 100, 20 ) ),
    spinnerTimeToFinishRect( Rectangle( spinnerMusicIdRect.x, spinnerMusicIdRect.y + spinnerMusicIdRect.height + 10, 100, 20 ) ),

    componentPropertiesRect( Rectangle( mapPropertiesRect.x, mapPropertiesRect.y + mapPropertiesRect.height + 10, 245, 250 ) ),

    colorPickerTileContainerRect( Rectangle( componentPropertiesRect.x + 10, componentPropertiesRect.y + 20, 145, 150 ) ),
    colorPickerTileRect( Rectangle( colorPickerTileContainerRect.x + 10, colorPickerTileContainerRect.y + 10, 100, 100 ) ),
    sliderAlphaTileRect( Rectangle( colorPickerTileRect.x, colorPickerTileRect.y + colorPickerTileRect.height + 10, colorPickerTileRect.width, 20 ) ),

    dummyTile( Vector2( 0, 0 ), BLACK, 1 ),

    backgroundColor( WHITE ),
    backgroundTextureId( 1 ),
    musicId( 1 ),
    timeToFinish( 200 ),
    showGrid( true ),
    playMusic( false ),
    previousSelectedMusicId( musicId ) {

    for ( int k = 0; k < maxLayers; k++ ) {
        layers.emplace_back();
        layersState.emplace_back( true );
        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                layers[k].push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE, 0 ) );
            }
        }
    }

}

MapEditor::~MapEditor() {

    for ( const auto &layer : layers ) {
        for ( const auto* tile : layer ) {
            delete tile;
        }
    }
    
}

void MapEditor::computePressedLineAndColumn( Vector2 &mousePos, int &line, int &column ) const {

    if ( isMouseInsideEditor( mousePos ) ) {

        line = ( static_cast<int>( mousePos.y ) - pos.y ) / Tile::TILE_WIDTH + startLine;
        column = ( static_cast<int>( mousePos.x ) - pos.x ) / Tile::TILE_WIDTH + startColumn;

        if ( !isTilePositionValid( line, column ) ) {
            line = - 1;
            column = -1;
        }

    } else {
        line = -1;
        column = -1;
    }

}

void MapEditor::selectTile( Vector2 &mousePos ) {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        layers[currentLayer - 1][line * columns + column]->setSelected( true );
        if ( firstSelectedTile == nullptr ) {
            firstSelectedTile = layers[currentLayer - 1][line * columns + column];
        }
    }

}

void MapEditor::deselectTile( Vector2 &mousePos ) {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        Tile *tile = layers[currentLayer - 1][line * columns + column];
        tile->setSelected( false );
        if ( firstSelectedTile == tile ) {
            firstSelectedTile = nullptr;
        }
    }

}

void MapEditor::deselectTiles() {
    for ( auto *tile : layers[currentLayer - 1] ) {
        tile->setSelected( false );
    }
    firstSelectedTile = nullptr;
}

bool MapEditor::isTileSelected( Vector2 &mousePos ) const {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        return layers[currentLayer - 1][line * columns + column]->isSelected();
    }

    return false;

}

bool MapEditor::isTilePositionValid( int line, int column ) const {
    return line >= 0 && line < lines && column >= 0 && column < columns;
}

bool MapEditor::isMouseInsideEditor( const Vector2 &mousePos ) const {
    return mousePos.x >= pos.x &&
           mousePos.x <= pos.x + minColumns * Tile::TILE_WIDTH &&
           mousePos.y >= pos.y &&
           mousePos.y <= pos.y + minLines * Tile::TILE_WIDTH;
}

void MapEditor::drawLayerPreview( int x, int y, int tileWidth, bool active, const std::vector<Tile*>& tiles ) const {

    for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            int p = i * columns + j;
            if ( p < tiles.size() ) {
                Tile *tile = tiles[p];
                DrawRectangle( x + (j-startColumn) * tileWidth, y + (i-startLine) * tileWidth, tileWidth, tileWidth, Fade( *(tile->getColor()), *(tile->getAlpha())) );
            }
        }
    }

    DrawRectangleLines( x, y, minColumns * tileWidth, minLines * tileWidth, active ? BLACK : LIGHTGRAY );

}

void MapEditor::inputAndUpdate() {

    guiContainerRect.width = GetScreenWidth() - ( tileComposerDim.x + 60 );
    guiContainerRect.height = GetScreenHeight() - 20;

    mapPropertiesRect.width = guiContainerRect.width - layersPreviewRect.width - 30;

    componentPropertiesRect.width = guiContainerRect.width - layersPreviewRect.width - 30;
    componentPropertiesRect.height = guiContainerRect.height - mapPropertiesRect.height - 40;

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {    

        Vector2 mousePos = GetMousePosition();

        for ( int i = maxLayers - 1; i >= 0; i-- ) {
            Rectangle previewRect( layersPreviewRect.x + 40,
                                   layersPreviewRect.y + 10 + ( maxLayers - i - 1 ) * ( previewTileWidth * minLines + 10 ),
                                   previewTileWidth * minColumns,
                                   previewTileWidth * minLines );
            if ( CheckCollisionPointRec( mousePos, previewRect ) ) {
                currentLayer = i + 1;
            }
        }

        if ( isMouseInsideEditor( mousePos ) ) {

            computePressedLineAndColumn( mousePos, pressedLine, pressedColumn );

            if ( isTileSelected( mousePos ) ) {
                deselectTile( mousePos );
            } else {
                selectTile( mousePos );
            }
            
        } else if ( !CheckCollisionPointRec( mousePos, colorPickerTileContainerRect ) ) {
            deselectTiles();
        }

    } else if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {

        Vector2 mousePos = GetMousePosition();

        if ( isMouseInsideEditor( mousePos ) ) {

            int currentLine;
            int currentColumn;

            computePressedLineAndColumn( mousePos, currentLine, currentColumn );

            if ( pressedLine != currentLine || 
                 pressedColumn != currentColumn ) {
                selectTile( mousePos );
            }

        } else if ( CheckCollisionPointRec( mousePos, colorPickerTileContainerRect ) ) {

            for ( auto& tile : layers[currentLayer - 1] ) {
                if ( tile->isSelected() && firstSelectedTile != nullptr ) {
                    tile->setColor( *(firstSelectedTile->getColor()) );
                    tile->setAlpha( *(firstSelectedTile->getAlpha()) );
                }
            }

        }

    } else if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) ) {
        pressedLine = -1;
        pressedColumn = -1;
    }

    if ( IsKeyPressed( KEY_ESCAPE ) ) {
        deselectTiles();
    }

    if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
        if ( IsKeyDown( KEY_W ) || IsKeyDown( KEY_UP ) ) {
            viewOffsetLine++;
        } else if ( IsKeyDown( KEY_S ) || IsKeyDown( KEY_DOWN ) ) {
            viewOffsetLine--;
        } else if ( IsKeyDown( KEY_A ) || IsKeyDown( KEY_LEFT ) ) {
            viewOffsetColumn--;
        } else if ( IsKeyDown( KEY_D ) || IsKeyDown( KEY_RIGHT ) ) {
            viewOffsetColumn++;
        }
    }

    const int mouseWheelMove = GetMouseWheelMove();
    if ( mouseWheelMove > 0 ) {
        if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
            viewOffsetColumn++;
        } else {
            viewOffsetLine++;
        }
    } else if ( mouseWheelMove ) {
        if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
            viewOffsetColumn--;
        } else {
            viewOffsetLine--;
        }
    }

    if ( IsKeyPressed( KEY_W ) || IsKeyPressed( KEY_UP ) ) {
        viewOffsetLine++;
    } else if ( IsKeyPressed( KEY_S ) || IsKeyPressed( KEY_DOWN ) ) {
        viewOffsetLine--;
    } else if ( IsKeyPressed( KEY_A ) || IsKeyPressed( KEY_LEFT ) ) {
        viewOffsetColumn--;
    } else if ( IsKeyPressed( KEY_D ) || IsKeyPressed( KEY_RIGHT ) ) {
        viewOffsetColumn++;
    }

    if ( viewOffsetLine < 0 ) {
        viewOffsetLine = 0;
    }

    if ( viewOffsetLine > lines - minLines ) {
        viewOffsetLine = lines - minLines;
    }

    if ( viewOffsetColumn < 0 ) {
        viewOffsetColumn = 0;
    }

    if ( viewOffsetColumn > columns - minColumns ) {
        viewOffsetColumn = columns - minColumns;
    }

    std::map<std::string, Music>& musics = ResourceManager::getMusics();
    Music currentSelectedMusic = musics[TextFormat( "music%d", musicId )];
    Music previousSelectedMusic = musics[TextFormat( "music%d", previousSelectedMusicId )];

    if ( playMusic ) {
        if ( musicId != previousSelectedMusicId ) {
            StopMusicStream( previousSelectedMusic );
            PlayMusicStream( currentSelectedMusic );
        } else {
            if ( IsMusicStreamPlaying( currentSelectedMusic ) ) {
                UpdateMusicStream( currentSelectedMusic );
            } else {
                PlayMusicStream( currentSelectedMusic );
            }
        }
    } else {
        if ( IsMusicStreamPlaying( previousSelectedMusic ) ) {
            StopMusicStream( previousSelectedMusic );
        }
        if ( IsMusicStreamPlaying( currentSelectedMusic ) ) {
            StopMusicStream( currentSelectedMusic );
        }
    }

    previousSelectedMusicId = musicId;


}

void MapEditor::draw() {

    std::map<std::string, Texture2D> &textures = ResourceManager::getTextures();

    startLine = lines - minLines - viewOffsetLine;
    startColumn = viewOffsetColumn;

    // background
    DrawRectangle( pos.x, pos.y, minColumns * Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH, backgroundColor );
    const Texture2D backgroundTexture = textures[TextFormat( "background%d", backgroundTextureId )];
    const int repeats = ( columns * Tile::TILE_WIDTH ) / backgroundTexture.width + 1;

    for ( int i = 0; i < repeats; i++ ) {
        DrawTexture(
            backgroundTexture,
            pos.x + backgroundTexture.width * i - startColumn * Tile::TILE_WIDTH,
            pos.y - backgroundTexture.height + ( lines - startLine ) * Tile::TILE_WIDTH,
            WHITE
        );
    }

    DrawRectangle( 0, 0, GetScreenWidth(), pos.y, WHITE );
    DrawRectangle( 0, pos.y + minLines * Tile::TILE_WIDTH, GetScreenWidth(), GetScreenHeight(), WHITE );
    DrawRectangle( 0, 0, pos.x, GetScreenHeight(), WHITE );
    DrawRectangle( pos.x + minColumns * Tile::TILE_WIDTH, 0, GetScreenWidth(), GetScreenHeight(), WHITE );

    // tiles
    for ( int k = 0; k < maxLayers; k++ ) {
        if ( layersState[k].visible ) {
            for ( int i = startLine; i < startLine + minLines; i++ ) {
                for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
                    layers[k][i * columns + j]->draw( Vector2( pos.x + ( j - startColumn ) * Tile::TILE_WIDTH, pos.y + ( i - startLine ) * Tile::TILE_WIDTH ) );
                }
            }
        }
    }

    // rulers background
    DrawRectangle( pos.x + tileComposerDim.x, pos.y, Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH + 1, Fade( LIGHTGRAY, 0.5 ) );
    DrawRectangle( pos.x-1, pos.y + tileComposerDim.y, minColumns * Tile::TILE_WIDTH + 1, Tile::TILE_WIDTH, Fade( LIGHTGRAY, 0.5 ) );

    // grid and rulers
    for ( int i = 0; i <= minLines + 1; i++ ) {
        if ( showGrid || i == 0 || ( i >= minLines ) ) {
            DrawLine( pos.x - 1, pos.y + i * Tile::TILE_WIDTH, pos.x + minColumns * Tile::TILE_WIDTH, pos.y + i * Tile::TILE_WIDTH, BLACK );
        }
        if ( i < minLines ) {
            const char* t = TextFormat( "%d", startLine + i + 1 );
            DrawText( t, pos.x + tileComposerDim.x + Tile::TILE_WIDTH / 2 - MeasureText( t, 10 ) / 2, pos.y + i * Tile::TILE_WIDTH + 12, 10, BLACK );
        }
    }

    for ( int i = 0; i <= minColumns + 1; i++ ) {
        if ( showGrid || i == 0 || ( i >= minColumns ) ) {
            DrawLine( pos.x + i * Tile::TILE_WIDTH, pos.y, pos.x + i * Tile::TILE_WIDTH, pos.y + minLines * Tile::TILE_WIDTH + 1, BLACK );
        }
        if ( i < minColumns ) {
            const char* t = TextFormat( "%d", startColumn + i + 1 );
            DrawText( t, pos.x + i * Tile::TILE_WIDTH + Tile::TILE_WIDTH / 2 - MeasureText( t, 10 ) / 2, pos.y + tileComposerDim.y + 12, 10, BLACK );
        }
    }

    // selected tiles
    for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            if ( layers[currentLayer - 1][i * columns + j]->isSelected() ) {
                Vector2 p( pos.x + ( j - startColumn ) * Tile::TILE_WIDTH, pos.y + ( i - startLine ) * Tile::TILE_WIDTH );
                Vector2 d( Tile::TILE_WIDTH, Tile::TILE_WIDTH );
                DrawRectangleLinesEx( Rectangle( p.x - 2, p.y - 1, d.x + 3, d.y + 3 ), 3, BLACK );
                DrawCircle( p.x + d.x - 6, p.y + d.y - 5, 2, Fade( BLACK, 0.5 ) );
            }
        }
    }

    // GUI
    GuiCheckBox( checkShowGridRect, "Show Grid", &showGrid );
    GuiCheckBox( checkPlayMusicRect, "Play Music", &playMusic );

    GuiToggleGroup( toogleGroupInsertRect, ";;;;", &activeInsertOption );
    DrawTexture( textures["B1"], toogleGroupInsertRect.x + 6, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["blockQuestion0"], toogleGroupInsertRect.x + toogleGroupInsertRect.width + 8, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["coin2"], toogleGroupInsertRect.x + toogleGroupInsertRect.width * 2 + 14, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["goomba0R"], toogleGroupInsertRect.x + toogleGroupInsertRect.width * 3 + 12, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["smallMario0R"], toogleGroupInsertRect.x + toogleGroupInsertRect.width * 4 + 12, toogleGroupInsertRect.y + 2, WHITE );

    GuiGroupBox( guiContainerRect, "Options" );
    GuiGroupBox( layersPreviewRect, "Layers" );

    for ( int i = maxLayers - 1; i >= 0; i-- ) {
        Vector2 pos( layersPreviewRect.x + 40,
                     layersPreviewRect.y + 10 + ( maxLayers - i - 1 ) * ( previewTileWidth * minLines + 10 ) );
        drawLayerPreview( pos.x, pos.y,
                          previewTileWidth,
                          i + 1 == currentLayer, layers[i] );
        GuiCheckBox( Rectangle( pos.x - 30, pos.y + ( previewTileWidth * minLines ) / 2 - 10, 20, 20 ), nullptr, &( layersState[i].visible ) );
    }

    GuiGroupBox( mapPropertiesRect, "Map Properties" );
    GuiSpinner( spinnerLinesRect, "Lines: ", &lines, minLines, maxLines, false );
    GuiSpinner( spinnerColumnsRect, "Columns: ", &columns, minColumns, maxColumns, false );

    GuiLabel( labelBackgroundColorRect, "Background Color: " );
    GuiColorPicker( colorPickerBackgroundColorRect, nullptr, &backgroundColor );
    GuiSpinner( spinnerBackgroundTextureIdRect, "Background Texture: ", &backgroundTextureId, 1, 10, false );
    GuiSpinner( spinnerMusicIdRect, "Music: ", &musicId, 1, 9, false );
    GuiSpinner( spinnerTimeToFinishRect, "Time to Finish: ", &timeToFinish, 1, 2000, true );

    if ( activeInsertOption == COMPONENT_INSERTION_TYPE_TILES ) {
        GuiGroupBox( componentPropertiesRect, "Tiles" );
        GuiGroupBox( colorPickerTileContainerRect, "Color" );
        if ( firstSelectedTile != nullptr ) {
            GuiColorPicker( colorPickerTileRect, nullptr, firstSelectedTile->getColor() );
            GuiColorBarAlpha( sliderAlphaTileRect, nullptr, firstSelectedTile->getAlpha() );
        } else {
            GuiColorPicker( colorPickerTileRect, nullptr, dummyTile.getColor() );
            GuiColorBarAlpha( sliderAlphaTileRect, nullptr, dummyTile.getAlpha() );
        }
    } else if ( activeInsertOption == COMPONENT_INSERTION_TYPE_BLOCKS ) {
        GuiGroupBox( componentPropertiesRect, "Blocks" );
    } else if ( activeInsertOption == COMPONENT_INSERTION_TYPE_ITEMS ) {
        GuiGroupBox( componentPropertiesRect, "Items" );
    } else if ( activeInsertOption == COMPONENT_INSERTION_TYPE_BADDIES ) {
        GuiGroupBox( componentPropertiesRect, "Baddies" );
    }

    if ( lines != previousLines || columns != previousColumns ) {
        deselectTiles();
        for ( int i = 0; i < maxLayers; i++ ) {
            relocateTiles( layers[i] );
        }
    }

    previousLines = lines;
    previousColumns = columns;

}

void MapEditor::relocateTiles( std::vector<Tile*>& tiles ) const {

    const std::vector<Tile*> prevTiles = tiles;
    tiles.clear();

    const int lineDiff = abs( lines - previousLines );
    const int columnDiff = abs( columns - previousColumns );

    if ( lines > previousLines ) {

        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                if ( i < lineDiff ) {
                    tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE, 0 ) );
                } else {
                    Tile *tile = prevTiles[( i - lineDiff ) * columns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                }
            }
        }

    } else if( lines < previousLines ) {

        for ( int i = 0; i < previousLines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                if ( i < lineDiff ) {
                    delete prevTiles[i * columns + j];
                } else {
                    Tile* tile = prevTiles[i * columns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, (i-lineDiff) * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                }
            }
        }

    }

    if ( columns > previousColumns ) {

        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                if ( j < previousColumns ) {
                    Tile* tile = prevTiles[i * previousColumns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                } else {
                    tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE, 0 ) );
                }
            }
        }

    } else if ( columns < previousColumns ) {

        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < previousColumns; j++ ) {
                if ( j < columns ) {
                    Tile* tile = prevTiles[i * previousColumns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                } else {
                    delete prevTiles[i * previousColumns + j];
                }
            }
        }

    }

}
