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


MapEditor::MapEditor( Vector2 pos, Vector2 dim, GameWorld *gw )
    :
    pos( pos ),
    dim( dim ),
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

    startLine( lines - minLines ),
    startColumn( columns - minColumns ),
    viewOffsetLine( 0 ),
    viewOffsetColumn( 0 ),
    
    firstSelectedTile( nullptr ),
    spinnerLinesRect( Rectangle( pos.x + dim.x + 50, pos.y + 10, 200, 20 ) ),
    spinnerColumnsRect( Rectangle( spinnerLinesRect.x, spinnerLinesRect.y + spinnerLinesRect.height + 10, 200, 20 ) ),
    colorPickerContainerRect( Rectangle( pos.x + dim.x + 300, pos.y + 10, 245, 220 ) ),
    colorPickerRect( Rectangle( colorPickerContainerRect.x + 10, colorPickerContainerRect.y + 10, 200, 200 ) ) {

    for ( int i = 0; i < lines; i++ ) {
        for ( int j = 0; j < columns; j++ ) {
            tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), BLUE ) );
        }
    }

}

MapEditor::~MapEditor() {
    for ( const auto *tile : tiles ) {
        delete tile;
    }
}

void MapEditor::computePressedLineAndColumn( Vector2 &mousePos, int &line, int &column ) const {

    if ( isMouseInsideEditor( mousePos ) ) {

        line = static_cast<int>( mousePos.y ) / Tile::TILE_WIDTH + startLine;
        column = static_cast<int>( mousePos.x ) / Tile::TILE_WIDTH + startColumn;

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
        tiles[line * columns + column]->setSelected( true );
        if ( firstSelectedTile == nullptr ) {
            firstSelectedTile = tiles[line * columns + column];
        }
    }

}

void MapEditor::deselectTile( Vector2 &mousePos ) {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        Tile *tile = tiles[line * columns + column];
        tile->setSelected( false );
        if ( firstSelectedTile == tile ) {
            firstSelectedTile = nullptr;
        }
    }

}

void MapEditor::deselectTiles() {
    for ( auto *tile : tiles ) {
        tile->setSelected( false );
    }
    firstSelectedTile = nullptr;
}

bool MapEditor::isTileSelected( Vector2 &mousePos ) const {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        return tiles[line * columns + column]->isSelected();
    }

    return false;

}

bool MapEditor::
isTilePositionValid( int line, int column ) const {
    return line >= 0 && line < lines && column >= 0 && column < columns;
}

bool MapEditor::isMouseInsideEditor( const Vector2 &mousePos ) const {
    return mousePos.x >= pos.x &&
           mousePos.x <= pos.x + minColumns * Tile::TILE_WIDTH &&
           mousePos.y >= pos.y &&
           mousePos.y <= pos.y + minLines * Tile::TILE_WIDTH;
}

void MapEditor::inputAndUpdate() {

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

            for ( auto& tile : tiles ) {
                if ( tile->isSelected() && firstSelectedTile != nullptr ) {
                    tile->setColor( *(firstSelectedTile->getColor()) );
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
        viewOffsetLine++;
    } else if ( mouseWheelMove ) {
        viewOffsetLine--;
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

    for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            tiles[i*columns + j]->draw( Vector2( ( j - startColumn ) * Tile::TILE_WIDTH, ( i - startLine ) * Tile::TILE_WIDTH ) );
        }
    }

    DrawRectangleLines( pos.x, pos.y, minColumns * Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH, BLACK );

    for ( int i = 1; i < minLines; i++ ) {
        DrawLine( pos.x, pos.y + i * Tile::TILE_WIDTH, pos.x + minColumns * Tile::TILE_WIDTH, pos.y + i * Tile::TILE_WIDTH, BLACK );
    }

    for ( int i = 1; i < minColumns; i++ ) {
        DrawLine( pos.x + i * Tile::TILE_WIDTH, pos.y, pos.x + i * Tile::TILE_WIDTH, pos.y + minLines * Tile::TILE_WIDTH, BLACK );
    }

    for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            if ( tiles[i * columns + j]->isSelected() ) {
                Vector2 pos( ( j - startColumn ) * Tile::TILE_WIDTH, ( i - startLine ) * Tile::TILE_WIDTH );
                Vector2 dim( Tile::TILE_WIDTH, Tile::TILE_WIDTH );
                DrawRectangleLinesEx( Rectangle( pos.x - 2, pos.y - 1, dim.x + 3, dim.y + 3 ), 3, BLACK );
                DrawCircle( pos.x + dim.x - 6, pos.y + dim.y - 5, 2, Fade( BLACK, 0.5 ) );
            }
        }
    }

    // GUI
    GuiSpinner( spinnerLinesRect, "Lines: ", &lines, minLines, maxLines, false );
    GuiSpinner( spinnerColumnsRect, "Columns: ", &columns, minColumns, maxColumns, false );

    GuiGroupBox( colorPickerContainerRect, "Tile Color" );
    if ( firstSelectedTile != nullptr ) {
        GuiColorPicker( colorPickerRect, nullptr, firstSelectedTile->getColor());
    } else {
        GuiColorPicker( colorPickerRect, nullptr, nullptr );
    }

    if ( lines != previousLines || columns != previousColumns ) {
        deselectTiles();
        relocateTiles( tiles );
    }

    previousLines = lines;
    previousColumns = columns;

}

void MapEditor::relocateTiles( std::vector<Tile*>& tiles ) {

    const std::vector<Tile*> prevTiles = tiles;
    tiles.clear();

    const int lineDiff = abs( lines - previousLines );
    const int columnDiff = abs( columns - previousColumns );

    if ( lines > previousLines ) {

        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                if ( i < lineDiff ) {
                    tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE ) );
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
                    tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE ) );
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

Vector2& MapEditor::getPos() {
    return pos;
}
Vector2& MapEditor::getDim() {
    return dim;
}
