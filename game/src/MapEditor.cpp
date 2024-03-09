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
    rows( 0 ),
    cols( 0 ),
    colorPickerContainerRect( Rectangle( pos.x + dim.x + 10, pos.y, 245, 245 ) ),
    colorPickerRect( Rectangle( colorPickerContainerRect.x + 10, colorPickerContainerRect.y + 35, 200, 200 ) ) {

    updateRowsAndCols();
    for ( int i = 0; i < rows; i++ ) {
        for ( int j = 0; j < cols; j++ ) {
            tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE ) );
        }
    }
}

MapEditor::~MapEditor() {
    for ( const auto *tile : tiles ) {
        delete tile;
    }
}

void MapEditor::updateRowsAndCols() {
    rows = static_cast<int>( dim.y ) / Tile::TILE_WIDTH;
    cols = static_cast<int>( dim.x ) / Tile::TILE_WIDTH;
}

void MapEditor::inputAndUpdate() {

    /**/

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {

        const Vector2 mousePos = GetMousePosition();

        if ( mousePos.x >= pos.x &&
             mousePos.x <= pos.x + dim.x &&
             mousePos.y >= pos.y &&
             mousePos.y <= pos.y + dim.y ) {
            selectedTiles.clear();
        } else if ( !CheckCollisionPointRec( mousePos, colorPickerContainerRect ) ) {
            selectedTiles.clear();
        }

    }

    if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {

        const Vector2 mousePos = GetMousePosition();

        if ( mousePos.x >= pos.x &&
             mousePos.x <= pos.x + dim.x &&
             mousePos.y >= pos.y &&
             mousePos.y <= pos.y + dim.y ) {

            const int line = static_cast<int>( mousePos.y ) / Tile::TILE_WIDTH;
            const int column = static_cast<int>( mousePos.x ) / Tile::TILE_WIDTH;
            selectedTiles.push_back( tiles[line * cols + column] );

        } else if ( CheckCollisionPointRec( mousePos, colorPickerContainerRect ) ) {

            if ( !selectedTiles.empty() ) {
                Tile* first = selectedTiles[0];
                for ( const auto& selectedTile : selectedTiles ) {
                    selectedTile->setColor( *( first->getColor() ) );
                }
            }

        } else {
            selectedTiles.clear();
        }

    } else if ( IsKeyPressed( KEY_ESCAPE ) ) {
        selectedTiles.clear();
    }

}

void MapEditor::draw() {

    for ( const auto &tile : tiles ) {
        tile->draw();
    }

    DrawRectangleLines( pos.x, pos.y, dim.x, dim.y, BLACK );

    for ( int i = 1; i < rows; i++ ) {
        DrawLine( 0, pos.y + i * Tile::TILE_WIDTH, pos.x + dim.x, pos.y + i * Tile::TILE_WIDTH, BLACK );
    }

    for ( int i = 1; i < cols; i++ ) {
        DrawLine( pos.x + i * Tile::TILE_WIDTH, pos.y, pos.x + i * Tile::TILE_WIDTH, pos.y + dim.y, BLACK );
    }

    if ( !selectedTiles.empty() ) {
        for ( const auto& selectedTile : selectedTiles ) {
            Vector2& pos = selectedTile->getPos();
            Vector2& dim = selectedTile->getDim();
            DrawRectangleLinesEx( Rectangle( pos.x - 2, pos.y - 1, dim.x + 3, dim.y + 3 ), 3, BLACK );
        }
    }

    GuiWindowBox( colorPickerContainerRect, "Tile Color" );
    if ( !selectedTiles.empty() ) {
        GuiColorPicker( colorPickerRect, nullptr, selectedTiles[0]->getColor());
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
