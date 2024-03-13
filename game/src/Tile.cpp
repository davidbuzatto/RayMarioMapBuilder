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

Tile::Tile( Vector2 pos, Color color, float alpha )
    :
    pos( pos ),
    dim( Vector2( TILE_WIDTH, TILE_WIDTH ) ),
    color( color ),
    alpha( alpha ),
    selected( false ) {
}

Tile::~Tile() = default;

void Tile::inputAndUpdate() {
    
}

void Tile::draw() {
    DrawRectangle( pos.x, pos.y, dim.x, dim.y, color );
}

void Tile::draw( Vector2 drawPos ) {
    DrawRectangle( drawPos.x, drawPos.y, dim.x, dim.y, Fade( color, alpha ) );
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

void Tile::setPos( Vector2 pos ) {
    this->pos = pos;
}

void Tile::setPos( int x, int y ) {
    pos.x = x;
    pos.y = y;
}

void Tile::setX( int x ) {
    pos.x = x;
}

void Tile::setY( int y ) {
    pos.y = y;
}

Color* Tile::getColor() {
    return &color;
}

void Tile::setColor( Color color ) {
    this->color = color;
}

float* Tile::getAlpha() {
    return &alpha;
}

void Tile::setAlpha( float alpha ) {
    this->alpha = alpha;
}

bool Tile::isSelected() const {
    return selected;
}

void Tile::setSelected( bool selected ) {
    this->selected = selected;
}
