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

class Tile : public virtual Drawable {

    Vector2 pos;
    Vector2 dim;
    Color color;
    bool selected;
    
public:

    static constexpr int TILE_WIDTH = 32;

    Tile( Vector2 pos, Color color );
    virtual ~Tile();

    void inputAndUpdate();
    void draw() override;
    void draw( Vector2 drawPos );

    Vector2& getPos();
    Vector2& getDim();
    Rectangle getRectangle() const;

    void setPos( Vector2 pos );
    void setPos( int x, int y );
    void setX( int x );
    void setY( int y );

    Color* getColor();
    void setColor( Color color );

    bool isSelected() const;
    void setSelected( bool selected );
    
};
