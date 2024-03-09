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
    
public:

    static constexpr int TILE_WIDTH = 32;

    Tile( Vector2 pos, Color color );
    virtual ~Tile();

    void inputAndUpdate();
    void draw() override;

    Vector2& getPos();
    Vector2& getDim();
    Rectangle getRectangle() const;

    Color* getColor();
    void setColor( Color color );
    
};
