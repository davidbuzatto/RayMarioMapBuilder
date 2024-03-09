/**
 * @file Tile.cpp
 * @author Prof. Dr. David Buzatto
 * @brief Tile class implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <iostream>
#include <string>

#include "Tile.h"
#include "raylib.h"

Tile::Tile( Vector2 pos, Color color )
    :
    pos( pos ),
    dim( Vector2( TILE_WIDTH, TILE_WIDTH ) ),
    color( color ),
    selected( false ) {
}

Tile::~Tile() = default;

void Tile::inputAndUpdate() {
    
}

void Tile::draw() {
    DrawRectangle( pos.x, pos.y, dim.x, dim.y, color );
}

Vector2& Tile::getPos() {
    return pos;
}
Vector2& Tile::getDim() {
    return dim;
}

Rectangle Tile::getRectangle() const {
    return Rectangle( pos.x, pos.y, dim.x, dim.y );
}

Color* Tile::getColor() {
    return &color;
}

void Tile::setColor( Color color ) {
    this->color = color;
}

bool Tile::isSelected() const {
    return selected;
}

void Tile::setSelected( bool selected ) {
    this->selected = selected;
}
