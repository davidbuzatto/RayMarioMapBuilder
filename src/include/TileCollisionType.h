/**
 * @file TileCollisionType.h
 * @author Prof. Dr. David Buzatto
 * @brief TileCollisionType enumeration.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

enum class TileCollisionType {

    solid = 0,
    solid_from_above = 1,
    solid_only_for_baddies = 2,
    non_solid = 3

};