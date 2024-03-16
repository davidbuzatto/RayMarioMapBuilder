/**
 * @file TileCollisionType.h
 * @author Prof. Dr. David Buzatto
 * @brief TileCollisionType enumeration.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

enum TileCollisionType {

    TILE_COLLISION_TYPE_SOLID = 0,
    TILE_COLLISION_TYPE_SOLID_FROM_ABOVE = 1,
    TILE_COLLISION_TYPE_SOLID_ONLY_FOR_BADDIES = 2,
    TILE_COLLISION_TYPE_NON_SOLID = 3

};