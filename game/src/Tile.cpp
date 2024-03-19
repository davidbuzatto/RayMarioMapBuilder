/**
 * @file Tile.cpp
 * @author Prof. Dr. David Buzatto
 * @brief Tile class implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <iostream>
#include "Tile.h"
#include "raylib.h"

Tile::Tile( Vector2 pos, Color color, float alpha, Vector2 drawOffset )
    :
    pos( pos ),
    dim( Vector2( TILE_WIDTH, TILE_WIDTH ) ),
    color( color ),
    alpha( alpha ),
    texture( nullptr ),
    collisionType( TileCollisionType::non_solid ),
    visible( true ),
    selected( false ),
    drawOffset( drawOffset ) {
}

Tile::Tile( Vector2 pos, Texture2D* texture, float alpha, Vector2 drawOffset )
    :
    pos( pos ),
    dim( Vector2( TILE_WIDTH, TILE_WIDTH ) ),
    color( BLACK ),
    alpha( alpha ),
    texture( texture ),
    collisionType( TileCollisionType::non_solid ),
    visible( true ),
    selected( false ),
    drawOffset( drawOffset ) {
}

Tile::~Tile() = default;

void Tile::inputAndUpdate() {
    
}

void Tile::draw() {
    if ( visible ) {
        if ( texture != nullptr ) {
            DrawTexture( *texture, pos.x + drawOffset.x, pos.y + drawOffset.y, WHITE );
        } else {
            DrawRectangle( pos.x + drawOffset.x, pos.y + drawOffset.y, dim.x, dim.y, Fade( color, alpha ) );
        }
    }
}

void Tile::draw( float customFade ) {
    if ( visible ) {
        if ( texture != nullptr ) {
            DrawTexture( *texture, pos.x + drawOffset.x, pos.y + drawOffset.y, WHITE );
        } else {
            DrawRectangle( pos.x + drawOffset.x, pos.y + drawOffset.y, dim.x, dim.y, Fade( color, customFade ) );
        }
    }
}

void Tile::draw( Vector2 drawPos, bool alignCenter ) {
    if ( visible ) {
        if ( texture != nullptr ) {
            if ( alignCenter ) {
                DrawTexture( *texture, drawPos.x - texture->width / 2 + drawOffset.x, drawPos.y - texture->height / 2 + drawOffset.y, WHITE );
            } else {
                DrawTexture( *texture, drawPos.x + drawOffset.x, drawPos.y + drawOffset.y, WHITE );
            }
        } else {
            if ( alignCenter ) {
                DrawRectangle( drawPos.x - dim.x/2 + drawOffset.x, drawPos.y - dim.y/2 + drawOffset.y, dim.x, dim.y, Fade( color, alpha ) );
            } else {
                DrawRectangle( drawPos.x + drawOffset.x, drawPos.y + drawOffset.y, dim.x, dim.y, Fade( color, alpha ) );
            }
        }
    }
}

void Tile::draw( Vector2 drawPos, float customFade ) {
    if ( visible ) {
        if ( texture != nullptr ) {
            DrawTexture( *texture, drawPos.x, drawPos.y, WHITE );
        } else {
            DrawRectangle( drawPos.x, drawPos.y, dim.x, dim.y, Fade( color, customFade ) );
        }
    }
}

Vector2& Tile::getPos() {
    return pos;
}
Vector2& Tile::getDim() {
    return dim;
}

Rectangle Tile::getRectangle() const {
    if ( texture != nullptr ) {
        return Rectangle( pos.x, pos.y, texture->width, texture->height );
    }
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

Texture2D* Tile::getTexture() {
    return texture;
}

void Tile::setTexture( Texture2D* texture ) {
    this->texture = texture;
}

bool Tile::isSelected() const {
    return selected;
}

void Tile::setSelected( bool selected ) {
    this->selected = selected;
}

bool Tile::isVisible() const {
    return visible;
}

void Tile::setVisible( bool visible ) {
    this->visible = visible;
}

TileCollisionType Tile::getCollisionType() {
    return collisionType;
}

void Tile::setCollisionType( TileCollisionType collisionType ) {
    this->collisionType = collisionType;
}

void Tile::copyData( Tile& tile, TileCollisionType collisionType, bool visible ) {

    color = tile.color;
    alpha = tile.alpha;
    texture = tile.texture;
    this->collisionType = collisionType;
    this->visible = visible;
    drawOffset = tile.drawOffset;

}

TileCollisionType Tile::getCollisionTypeFromInt( int collisionTypeInt ) {
    switch ( collisionTypeInt ) {
        case 0:
            return TileCollisionType::solid;
        case 1:
            return TileCollisionType::solid_from_above;
        case 2:
            return TileCollisionType::solid_only_for_baddies;
        case 3:
            return TileCollisionType::non_solid;
        default:
            return TileCollisionType::solid;
    }
}

void Tile::resetTile( Tile& tile ) {

    tile.color = WHITE;
    tile.alpha = 0;
    tile.texture = nullptr;
    tile.collisionType = TileCollisionType::non_solid;
    tile.visible = true;
    tile.selected = false;
    tile.drawOffset = Vector2( 0, 0 );

}
