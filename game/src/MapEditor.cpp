/**
 * @file MapEditor.cpp
 * @author Prof. Dr. David Buzatto
 * @brief MapEditor class implementation.
 *
 * @copyright Copyright (c) 2024
 */
#include "GameWorld.h"
#include "MapEditor.h"
#include "Tile.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
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
    maxLayers( 2 ),

    guiContainerRect( Rectangle( pos.x + tileComposerDim.x + 40, pos.y, 50, 50 ) ),

    colorPickerContainerRect( Rectangle( guiContainerRect.x + 10, guiContainerRect.y + 20, 245, 250 ) ),
    colorPickerRect( Rectangle( colorPickerContainerRect.x + 10, colorPickerContainerRect.y + 10, 200, 200 ) ),
    sliderAlphaRect( Rectangle( colorPickerRect.x, colorPickerRect.y + colorPickerRect.height + 10, colorPickerRect.width, 20 ) ),

    spinnerCurrentLayerRect( Rectangle( guiContainerRect.x + 90, colorPickerContainerRect.y + colorPickerContainerRect.height + 10, 100, 20 ) ),
    spinnerLinesRect( Rectangle( spinnerCurrentLayerRect.x, spinnerCurrentLayerRect.y + spinnerCurrentLayerRect.height + 10, 100, 20 ) ),
    spinnerColumnsRect( Rectangle( spinnerCurrentLayerRect.x, spinnerLinesRect.y + spinnerLinesRect.height + 10, 100, 20 ) ),

    previewTileWidth( 6 ),
    layersPreviewRect( Rectangle( colorPickerContainerRect.x + colorPickerContainerRect.width + 10, colorPickerContainerRect.y, previewTileWidth * minColumns + 20, maxLayers * ( previewTileWidth * minLines + 10 ) + 10 ) ),

    dummyTile( Vector2( 0, 0 ), BLACK, 1 ) {

    /*for ( int i = 0; i < lines; i++ ) {
        for ( int j = 0; j < columns; j++ ) {
            tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE ) );
        }
    }*/

    for ( int k = 0; k < maxLayers; k++ ) {
        layers.emplace_back();
        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                layers[k].push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE, 0 ) );
            }
        }
    }

}

MapEditor::~MapEditor() {

    /*for ( const auto *tile : tiles ) {
        delete tile;
    }*/

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
        //tiles[line * columns + column]->setSelected( true );
        layers[currentLayer - 1][line * columns + column]->setSelected( true );
        if ( firstSelectedTile == nullptr ) {
            //firstSelectedTile = tiles[line * columns + column];
            firstSelectedTile = layers[currentLayer - 1][line * columns + column];
        }
    }

}

void MapEditor::deselectTile( Vector2 &mousePos ) {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        //Tile *tile = tiles[line * columns + column];
        Tile *tile = layers[currentLayer - 1][line * columns + column];
        tile->setSelected( false );
        if ( firstSelectedTile == tile ) {
            firstSelectedTile = nullptr;
        }
    }

}

void MapEditor::deselectTiles() {
    //for ( auto *tile : tiles ) {
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
        //return tiles[line * columns + column]->isSelected();
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

void MapEditor::drawLayerPreview( int x, int y, int tileWidth, const std::vector<Tile*>& tiles ) const {

    for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            Tile *tile = tiles[i * columns + j];
            DrawRectangle( x + (j-startColumn) * tileWidth, y + (i-startLine) * tileWidth, tileWidth, tileWidth, Fade( *(tile->getColor()), *(tile->getAlpha())) );
        }
    }

    DrawRectangleLines( x, y, minColumns * tileWidth, minLines * tileWidth, BLACK );

}

void MapEditor::inputAndUpdate() {

    guiContainerRect.width = GetScreenWidth() - ( tileComposerDim.x + 60 );
    guiContainerRect.height = GetScreenHeight() - 20;

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {

        Vector2 mousePos = GetMousePosition();

        if ( isMouseInsideEditor( mousePos ) ) {

            computePressedLineAndColumn( mousePos, pressedLine, pressedColumn );

            if ( isTileSelected( mousePos ) ) {
                deselectTile( mousePos );
            } else {
                selectTile( mousePos );
            }
            
        } else if ( !CheckCollisionPointRec( mousePos, colorPickerContainerRect ) ) {
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

        } else if ( CheckCollisionPointRec( mousePos, colorPickerContainerRect ) ) {

            //for ( auto& tile : tiles ) {
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

}

void MapEditor::draw() {

    startLine = lines - minLines - viewOffsetLine;
    startColumn = viewOffsetColumn;

    /*for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            tiles[i*columns + j]->draw( Vector2( pos.x + ( j - startColumn ) * Tile::TILE_WIDTH, pos.y + ( i - startLine ) * Tile::TILE_WIDTH ) );
        }
    }*/

    for ( int k = 0; k < maxLayers; k++ ) {
        for ( int i = startLine; i < startLine + minLines; i++ ) {
            for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
                layers[k][i * columns + j]->draw( Vector2( pos.x + ( j - startColumn ) * Tile::TILE_WIDTH, pos.y + ( i - startLine ) * Tile::TILE_WIDTH ) );
            }
        }
    }

    DrawRectangle( pos.x + tileComposerDim.x, pos.y, Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH + 1, Fade( LIGHTGRAY, 0.5 ) );
    DrawRectangle( pos.x-1, pos.y + tileComposerDim.y, minColumns * Tile::TILE_WIDTH + 1, Tile::TILE_WIDTH, Fade( LIGHTGRAY, 0.5 ) );

    for ( int i = 0; i <= minLines + 1; i++ ) {
        DrawLine( pos.x-1, pos.y + i * Tile::TILE_WIDTH, pos.x + minColumns * Tile::TILE_WIDTH, pos.y + i * Tile::TILE_WIDTH, BLACK );
        if ( i < minLines ) {
            const char* t = TextFormat( "%d", startLine + i + 1 );
            DrawText( t, pos.x + tileComposerDim.x + Tile::TILE_WIDTH / 2 - MeasureText( t, 10 ) / 2, pos.y + i * Tile::TILE_WIDTH + 12, 10, BLACK );
        }
    }

    for ( int i = 0; i <= minColumns + 1; i++ ) {
        DrawLine( pos.x + i * Tile::TILE_WIDTH, pos.y, pos.x + i * Tile::TILE_WIDTH, pos.y + minLines * Tile::TILE_WIDTH + 1, BLACK );
        if ( i < minColumns ) {
            const char* t = TextFormat( "%d", startColumn + i + 1 );
            DrawText( t, pos.x + i * Tile::TILE_WIDTH + Tile::TILE_WIDTH / 2 - MeasureText( t, 10 ) / 2, pos.y + tileComposerDim.y + 12, 10, BLACK );
        }
    }

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
    GuiGroupBox( guiContainerRect, "Options" );

    GuiGroupBox( colorPickerContainerRect, "Tile Color" );
    if ( firstSelectedTile != nullptr ) {
        GuiColorPicker( colorPickerRect, nullptr, firstSelectedTile->getColor() );
        GuiColorBarAlpha( sliderAlphaRect, nullptr, firstSelectedTile->getAlpha() );
    } else {
        GuiColorPicker( colorPickerRect, nullptr, dummyTile.getColor() );
        GuiColorBarAlpha( sliderAlphaRect, nullptr, dummyTile.getAlpha() );
    }

    GuiSpinner( spinnerCurrentLayerRect, "Current Layer: ", &currentLayer, 1, maxLayers, false );
    GuiSpinner( spinnerLinesRect, "Lines: ", &lines, minLines, maxLines, false );
    GuiSpinner( spinnerColumnsRect, "Columns: ", &columns, minColumns, maxColumns, false );

    if ( lines != previousLines || columns != previousColumns ) {
        deselectTiles();
        //relocateTiles( tiles );
        for ( int i = 0; i < maxLayers; i++ ) {
            relocateTiles( layers[i] );
        }
    }

    previousLines = lines;
    previousColumns = columns;

    GuiGroupBox( layersPreviewRect, "Layers" );

    for ( int i = maxLayers - 1; i >= 0; i-- ) {
        drawLayerPreview( layersPreviewRect.x + 10, layersPreviewRect.y + 10 + (maxLayers - i - 1) * ( previewTileWidth * minLines + 10 ), previewTileWidth, layers[i] );
    }

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
