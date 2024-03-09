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
    lines( 0 ),
    columns( 0 ),
    pressedLine( -1 ),
    pressedColumn( -1 ),
    firstSelectedTile( nullptr ),
    colorPickerContainerRect( Rectangle( pos.x + dim.x + 10, pos.y + 10, 245, 220 ) ),
    colorPickerRect( Rectangle( colorPickerContainerRect.x + 10, colorPickerContainerRect.y + 10, 200, 200 ) ) {

    updateLinesAndColumns();
    for ( int i = 0; i < lines; i++ ) {
        for ( int j = 0; j < columns; j++ ) {
            tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE ) );
        }
    }
}

MapEditor::~MapEditor() {
    for ( const auto *tile : tiles ) {
        delete tile;
    }
}

void MapEditor::updateLinesAndColumns() {
    lines = static_cast<int>( dim.y ) / Tile::TILE_WIDTH;
    columns = static_cast<int>( dim.x ) / Tile::TILE_WIDTH;
}

void MapEditor::computePressedLineAndColumn( Vector2 mousePos, int* line, int* column ) const {

    if ( mousePos.x >= pos.x &&
         mousePos.x <= pos.x + dim.x &&
         mousePos.y >= pos.y &&
         mousePos.y <= pos.y + dim.y ) {

        *line = static_cast<int>( mousePos.y ) / Tile::TILE_WIDTH;
        *column = static_cast<int>( mousePos.x ) / Tile::TILE_WIDTH;

        if ( !isTilePositionValid( *line, *column ) ) {
            *line = - 1;
            *column = -1;
        }

    } else {
        *line = -1;
        *column = -1;
    }

}

void MapEditor::selectTile( Vector2 mousePos ) {

    const int line = static_cast<int>( mousePos.y ) / Tile::TILE_WIDTH;
    const int column = static_cast<int>( mousePos.x ) / Tile::TILE_WIDTH;

    if ( isTilePositionValid( line, column ) ) {
        tiles[line * columns + column]->setSelected( true );
        if ( firstSelectedTile == nullptr ) {
            firstSelectedTile = tiles[line * columns + column];
        }
    }

}

void MapEditor::deselectTile( Vector2 mousePos ) {

    const int line = static_cast<int>( mousePos.y ) / Tile::TILE_WIDTH;
    const int column = static_cast<int>( mousePos.x ) / Tile::TILE_WIDTH;

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

bool MapEditor::isTileSelected( Vector2 mousePos ) const {

    const int line = static_cast<int>( mousePos.y ) / Tile::TILE_WIDTH;
    const int column = static_cast<int>( mousePos.x ) / Tile::TILE_WIDTH;

    if ( isTilePositionValid( line, column ) ) {
        return tiles[line * columns + column]->isSelected();
    }

    return false;

}

bool MapEditor::isTilePositionValid( int line, int column ) const {
    return line >= 0 && line < lines && column >= 0 && column < columns;
}

bool MapEditor::isMouseInsideEditor( const Vector2* mousePos ) const {
    return mousePos->x >= pos.x &&
           mousePos->x <= pos.x + dim.x &&
           mousePos->y >= pos.y &&
           mousePos->y <= pos.y + dim.y;
}

void MapEditor::inputAndUpdate() {

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {

        Vector2 mousePos = GetMousePosition();

        if ( isMouseInsideEditor( &mousePos ) ) {

            computePressedLineAndColumn( mousePos, &pressedLine, &pressedColumn );

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

        if ( isMouseInsideEditor( &mousePos ) ) {

            int currentLine;
            int currentColumn;

            computePressedLineAndColumn( mousePos, &currentLine, &currentColumn );

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

}

void MapEditor::draw() {

    for ( const auto &tile : tiles ) {
        tile->draw();
    }

    DrawRectangleLines( pos.x, pos.y, dim.x, dim.y, BLACK );

    for ( int i = 1; i < lines; i++ ) {
        DrawLine( 0, pos.y + i * Tile::TILE_WIDTH, pos.x + dim.x, pos.y + i * Tile::TILE_WIDTH, BLACK );
    }

    for ( int i = 1; i < columns; i++ ) {
        DrawLine( pos.x + i * Tile::TILE_WIDTH, pos.y, pos.x + i * Tile::TILE_WIDTH, pos.y + dim.y, BLACK );
    }

    for ( const auto& tile : tiles ) {
        if ( tile->isSelected() ) {
            Vector2& pos = tile->getPos();
            Vector2& dim = tile->getDim();
            DrawRectangleLinesEx( Rectangle( pos.x - 2, pos.y - 1, dim.x + 3, dim.y + 3 ), 3, BLACK );
            DrawCircle( pos.x + dim.x - 6, pos.y + dim.y - 5, 2, Fade( BLACK, 0.5 ) );
        }
    }

    GuiGroupBox( colorPickerContainerRect, "Tile Color" );
    if ( firstSelectedTile != nullptr ) {
        GuiColorPicker( colorPickerRect, nullptr, firstSelectedTile->getColor());
    } else {
        GuiColorPicker( colorPickerRect, nullptr, nullptr );
    }

}

Vector2& MapEditor::getPos() {
    return pos;
}
Vector2& MapEditor::getDim() {
    return dim;
}

Rectangle MapEditor::getRectangle() const {
    return Rectangle( pos.x, pos.y, dim.x, dim.y );
}
