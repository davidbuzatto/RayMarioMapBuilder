/**
 * @file Tile.h
 * @author Prof. Dr. David Buzatto
 * @brief Tile class declaration.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "Drawable.h"
#include "raylib.h"
#include "TileCollisionType.h"

class Tile : public virtual Drawable {

    Vector2 pos;
    Vector2 dim;
    Color color;
    float alpha;
    Texture2D* texture;
    TileCollisionType collisionType;
    bool visible;
    bool selected;
    
public:

    static constexpr int TILE_WIDTH = 32;

    Tile( Vector2 pos, Color color, float alpha );
    Tile( Vector2 pos, Texture2D* texture, float alpha );
    virtual ~Tile();

    void inputAndUpdate();
    void draw() override;
    void draw( float customFade );
    void draw( Vector2 drawPos );
    void draw( Vector2 drawPos, float customFade );

    Vector2& getPos();
    Vector2& getDim();
    Rectangle getRectangle() const;

    void setPos( Vector2 pos );
    void setPos( int x, int y );
    void setX( int x );
    void setY( int y );

    Color* getColor();
    void setColor( Color color );

    float* getAlpha();
    void setAlpha( float alpha );

    Texture2D* getTexture();
    void setTexture( Texture2D* texture );

    bool isSelected() const;
    void setSelected( bool selected );

    bool isVisible() const;
    void setVisible( bool visible );

    TileCollisionType getCollisionType();
    void setCollisionType( TileCollisionType collisionType );

    void copyData( Tile& tile, TileCollisionType collisionType, bool visible );

    static TileCollisionType getCollisionTypeFromInt( int collisionTypeInt );
    
};
