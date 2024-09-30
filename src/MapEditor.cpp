/**
 * @file MapEditor.cpp
 * @author Prof. Dr. David Buzatto
 * @brief MapEditor class implementation.
 *
 * @copyright Copyright (c) 2024
 */
#include "GameWorld.h"
#include "MapEditor.h"
#include "ResourceManager.h"
#include "Tile.h"
#include "raylib.h"
#include "ComponentInsertionType.h"
#include "TileCollisionType.h"
#include "TilePaintingType.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

MapEditor::MapEditor( Vector2 pos, GameWorld *gw )
    :
    pos( pos ),
    gw( gw ),

    minLines( 14 ),
    maxLines( 40 ),
    lines( minLines ),
    previousLines( minLines ),
    pressedLine( -1 ),

    minColumns( 18 ),
    maxColumns( 400 ),
    columns( minColumns ),
    previousColumns( minColumns ),
    pressedColumn( -1 ),

    tileComposerDim( Vector2( minColumns * Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH ) ),

    startLine( lines - minLines ),
    startColumn( columns - minColumns ),
    viewOffsetLine( 0 ),
    viewOffsetColumn( 0 ),
    
    firstSelectedTile( nullptr ),
    currentLayer( 1 ),
    maxLayers( 7 ),

    guiContainerRect( Rectangle( pos.x + tileComposerDim.x + 40, pos.y, 50, 50 ) ),

    previewTileWidth( 6 ),
    layersPreviewRect( Rectangle( guiContainerRect.x + 10, guiContainerRect.y + 20, previewTileWidth* minColumns + 20 + 30, maxLayers* ( previewTileWidth* minLines + 10 ) + 10 ) ),

    toogleGroupInsertRect( Rectangle( pos.x, pos.y + tileComposerDim.y + Tile::TILE_WIDTH + 10, 44, 44 ) ),
    checkShowGridRect( Rectangle( pos.x + minColumns * Tile::TILE_WIDTH - 80, pos.y + tileComposerDim.y + Tile::TILE_WIDTH + 10, 20, 20 ) ),
    checkPlayMusicRect( Rectangle( checkShowGridRect.x, checkShowGridRect.y + checkShowGridRect.height + 10, 20, 20 ) ),
    activeInsertOption( static_cast<int>(ComponentInsertionType::tiles ) ),

    mapPropertiesRect( Rectangle( layersPreviewRect.x + layersPreviewRect.width + 10, layersPreviewRect.y, 260, 270 )  ),
    spinnerLinesRect( Rectangle( mapPropertiesRect.x + 125, mapPropertiesRect.y + 10, 100, 20 ) ),
    spinnerColumnsRect( Rectangle( spinnerLinesRect.x, spinnerLinesRect.y + spinnerLinesRect.height + 10, 100, 20 ) ),
    labelBackgroundColorRect( Rectangle( spinnerColumnsRect.x - 100, spinnerColumnsRect.y + spinnerColumnsRect.height + 50, 100, 20 ) ),
    colorPickerBackgroundColorRect( Rectangle( spinnerColumnsRect.x, spinnerColumnsRect.y + spinnerColumnsRect.height + 10, 100, 100 )  ),
    spinnerBackgroundTextureIdRect( Rectangle( colorPickerBackgroundColorRect.x, colorPickerBackgroundColorRect.y + colorPickerBackgroundColorRect.height + 10, 100, 20 ) ),
    spinnerMusicIdRect( Rectangle( spinnerBackgroundTextureIdRect.x, spinnerBackgroundTextureIdRect.y + spinnerBackgroundTextureIdRect.height + 10, 100, 20 ) ),
    spinnerTimeToFinishRect( Rectangle( spinnerMusicIdRect.x, spinnerMusicIdRect.y + spinnerMusicIdRect.height + 10, 100, 20 ) ),

    componentPropertiesRect( Rectangle( mapPropertiesRect.x, mapPropertiesRect.y + mapPropertiesRect.height + 10, 245, 250 ) ),

    comboTileCollisionTypeRect( Rectangle( componentPropertiesRect.x + 10, componentPropertiesRect.y + 10, 145, 20 ) ),
    togglePaintingTypeRect( Rectangle( comboTileCollisionTypeRect.x, comboTileCollisionTypeRect.y + comboTileCollisionTypeRect.height + 10, 72, 20 ) ),
    colorPickerTileContainerRect( Rectangle( togglePaintingTypeRect.x, togglePaintingTypeRect.y + togglePaintingTypeRect.height + 10, 145, 150 ) ),
    colorPickerTileRect( Rectangle( colorPickerTileContainerRect.x + 10, colorPickerTileContainerRect.y + 10, 100, 100 ) ),
    sliderAlphaTileRect( Rectangle( colorPickerTileRect.x, colorPickerTileRect.y + colorPickerTileRect.height + 10, colorPickerTileRect.width, 20 ) ),
    checkVisibleRect( Rectangle( colorPickerTileContainerRect.x, colorPickerTileContainerRect.y + colorPickerTileContainerRect.height + 10, 20, 20 ) ),
    tileCollisionType( static_cast<int>(TileCollisionType::solid) ),
    tilePaintingType( static_cast<int>(TilePaintingType::textured) ), 
    tileVisible( true ),

    coloredModelTile( Vector2( 0, 0 ), BLACK, 1, true, Vector2( pos.x, pos.y ) ),

    backgroundColor( WHITE ),
    backgroundTextureId( 1 ),
    musicId( 1 ),
    timeToFinish( 200 ),
    showGrid( true ),
    playMusic( false ),
    previousSelectedMusicId( musicId ),

    terrainRect( Rectangle(
        comboTileCollisionTypeRect.x + comboTileCollisionTypeRect.width + 10,
        togglePaintingTypeRect.y,
        88,
        componentPropertiesRect.height - 50 ) ),
    pipesRect( Rectangle(
        terrainRect.x + terrainRect.width + 10,
        terrainRect.y,
        55,
        componentPropertiesRect.height - 50 ) ),
    selectedTile( nullptr ),

    staticRect( Rectangle(
        componentPropertiesRect.x + 10,
        componentPropertiesRect.y + 15,
        52,
        componentPropertiesRect.height - 25 ) ),
    interactiveRect( Rectangle(
        staticRect.x + staticRect.width + 10,
        staticRect.y,
        88,
        componentPropertiesRect.height - 25 ) ),
    selectedBlock( nullptr ),
    selectedItem( nullptr ),
    selectedBaddie( nullptr ),
    mario( Vector2( 0, 0 ), nullptr, 1, false, Vector2( 0, -8 ) ),
    lastMarioTile( nullptr )

{

    for ( int k = 0; k < maxLayers; k++ ) {
        layers.emplace_back();
        layersState.emplace_back( true );
        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                layers[k].push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE, 0, true, Vector2( pos.x, pos.y ) ) );
            }
        }
    }

    bool first = true;
    for ( auto const& c : pipeColors ) {
        pipeColorOptions += ( first ? "": ";" ) + c;
        first = false;
    }

}

MapEditor::~MapEditor() {

    for ( const auto &layer : layers ) {
        for ( const auto* tile : layer ) {
            delete tile;
        }
    }
    
}

void MapEditor::computePressedLineAndColumn( Vector2 &mousePos, int &line, int &column ) const {

    if ( isMouseInsideEditor( mousePos ) ) {

        line = ( static_cast<int>( mousePos.y ) - pos.y ) / Tile::TILE_WIDTH + startLine;
        column = ( static_cast<int>( mousePos.x ) - pos.x ) / Tile::TILE_WIDTH + startColumn;

        if ( !isTilePositionValid( line, column ) ) {
            line = - 1;
            column = -1;
        }

    } else {
        line = -1;
        column = -1;
    }

}

void MapEditor::selectTile( Vector2 &mousePos ) {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        layers[currentLayer - 1][line * columns + column]->setSelected( true );
        if ( firstSelectedTile == nullptr ) {
            firstSelectedTile = layers[currentLayer - 1][line * columns + column];
        }
    }

}

Tile* MapEditor::getTileFromPosition( Vector2& mousePos ) {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        return layers[currentLayer - 1][line * columns + column];
    }

    return nullptr;

}

void MapEditor::deselectTile( Vector2 &mousePos ) {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        Tile *tile = layers[currentLayer - 1][line * columns + column];
        tile->setSelected( false );
        if ( firstSelectedTile == tile ) {
            firstSelectedTile = nullptr;
        }
    }

}

void MapEditor::deselectTiles() {
    for ( auto *tile : layers[currentLayer - 1] ) {
        tile->setSelected( false );
    }
    firstSelectedTile = nullptr;
    selectedTiles.clear();
}

bool MapEditor::isTileSelected( Vector2 &mousePos ) const {

    int line;
    int column;

    computePressedLineAndColumn( mousePos, line, column );

    if ( isTilePositionValid( line, column ) ) {
        return layers[currentLayer - 1][line * columns + column]->isSelected();
    }

    return false;

}

bool MapEditor::isTilePositionValid( int line, int column ) const {
    return line >= 0 && line < lines && column >= 0 && column < columns;
}

bool MapEditor::isMouseInsideEditor( const Vector2 &mousePos ) const {
    return mousePos.x >= pos.x &&
           mousePos.x <= pos.x + minColumns * Tile::TILE_WIDTH &&
           mousePos.y >= pos.y &&
           mousePos.y <= pos.y + minLines * Tile::TILE_WIDTH;
}

void MapEditor::drawLayerPreview( int x, int y, int tileWidth, bool active, const std::vector<Tile*>& tiles ) const {

    for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            int p = i * columns + j;
            if ( p < tiles.size() ) {
                Tile *tile = tiles[p];
                if ( tile->isVisible() ) {
                    if ( tile->getTexture() != nullptr ) {
                        DrawTextureEx(
                            *( tile->getTexture() ),
                            Vector2( x + ( j - startColumn ) * tileWidth, y + ( i - startLine ) * tileWidth ),
                            0,
                            static_cast<float>( tileWidth ) / Tile::TILE_WIDTH,
                            WHITE );
                    } else {
                        DrawRectangle( x + ( j - startColumn ) * tileWidth, y + ( i - startLine ) * tileWidth, tileWidth, tileWidth, Fade( *( tile->getColor() ), *( tile->getAlpha() ) ) );
                    }
                }
            }
        }
    }

    DrawRectangleLines( x, y, minColumns * tileWidth, minLines * tileWidth, active ? BLACK : LIGHTGRAY );

}

void MapEditor::highlightSelectedTile( Tile& tile ) const {
    DrawRectangleRec( tile.getRectangle(), Fade( BLUE, 0.3 ) );
    DrawRectangleLinesEx( tile.getRectangle(), 3, BLUE );
}

void MapEditor::inputAndUpdate() {

    std::map<std::string, Texture2D>& textures = ResourceManager::getTextures();

    if ( !resourceDependantComponentsCreated && !textures.empty() ) {

        for ( int k = 1; k < 5; k++ ) {

            int p = 0;

            for ( int i = 0; i < 18; i += 2 ) {
                tilesToSelect.push_back(
                    Tile( 
                        Vector2( 
                            terrainRect.x + 10,
                            terrainRect.y + 10 + ( Tile::TILE_WIDTH + 4 ) * p
                        ),
                        &textures[TextFormat( "%c%d", 'A' + i, k )], 
                        1,
                        false,
                        Vector2( pos.x, pos.y )
                    )
                );
                tilesToSelect.push_back(
                    Tile(
                        Vector2(
                            terrainRect.x + 15 + Tile::TILE_WIDTH,
                            terrainRect.y + 10 + ( Tile::TILE_WIDTH + 4 ) * p
                        ),
                        &textures[TextFormat( "%c%d", 'A' + ( i + 1 ), k )],
                        1,
                        false,
                        Vector2( pos.x, pos.y )
                    )
                );
                p++;
            }

        }

        for ( const auto& color : pipeColors ) {

            int p = 0;

            for ( int i = 0; i < 4; i++ ) {
                pipesToSelect.push_back(
                    Tile(
                        Vector2(
                            pipesRect.x + 10,
                            pipesRect.y + 10 + ( Tile::TILE_WIDTH + 4 ) * p
                        ),
                        &textures[TextFormat( "pipe_%s%d", color.c_str(), i )],
                        1,
                        false,
                        Vector2( pos.x, pos.y )
                    )
                );
                p++;
            }
            for ( int i = 4; i < 6; i++ ) {
                pipesToSelect.push_back(
                    Tile(
                        Vector2(
                            pipesRect.x + 10,
                            pipesRect.y + 10 + ( Tile::TILE_WIDTH + 4 ) * p
                        ),
                        &textures[TextFormat( "sm_pipe_%s%d", color.c_str(), ( i - 4 ) )],
                        1,
                        false,
                        Vector2( pos.x, pos.y )
                    )
                );
                p++;
            }

        }

        selectedTile = tilesToSelect.data();
        selectedTile->setSelected( true );

        for ( int i = 0; i < 5; i++ ) {
            blocksToSelect.push_back(
                Tile(
                    Vector2(
                        staticRect.x + 10,
                        staticRect.y + 10 + ( Tile::TILE_WIDTH + 4 ) * i
                    ),
                    &textures[TextFormat( "block%d", i )],
                    1,
                    false,
                    Vector2( pos.x, pos.y )
                )
            );
        }

        for ( int i = 5; i < 9; i++ ) {
            blocksToSelect.push_back(
                Tile(
                    Vector2(
                        interactiveRect.x + 10,
                        interactiveRect.y + 10 + ( Tile::TILE_WIDTH + 4 ) * ( i - 5 )
                    ),
                    &textures[TextFormat( "block%d", i )],
                    1,
                    false,
                    Vector2( pos.x, pos.y )
                )
            );
        }

        for ( int i = 9; i < 15; i++ ) {
            blocksToSelect.push_back(
                Tile(
                    Vector2(
                        interactiveRect.x + 10 + Tile::TILE_WIDTH + 4,
                        interactiveRect.y + 10 + ( Tile::TILE_WIDTH + 4 ) * ( i - 9 )
                    ),
                    &textures[TextFormat( "block%d", i )],
                    1,
                    false,
                    Vector2( pos.x, pos.y )
                )
            );
        }

        selectedBlock = blocksToSelect.data();
        selectedBlock->setSelected( true );

        std::vector itemsTextures{ &textures["coin"], &textures["yoshiCoin"] };
        std::vector itemsOffsets{ Vector2( 0, 0 ), Vector2( 0, 0 ) };
        int offset = 0;

        for ( size_t i = 0; i < itemsTextures.size(); i++ ) {
            itemsToSelect.push_back(
                Tile(
                    Vector2(
                        componentPropertiesRect.x + 10,
                        componentPropertiesRect.y + 10 + offset
                    ),
                    itemsTextures[i],
                    1,
                    false,
                    Vector2( pos.y, pos.y ),
                    itemsOffsets[i]
                )
            );
            offset += itemsTextures[i]->height + 4;
        }

        selectedItem = itemsToSelect.data();
        selectedItem->setSelected( true );

        std::vector baddiesTextures{
            &textures["goombaL"],
            &textures["flyingGoombaL"],
            &textures["redKoopaTroopaL"],
            &textures["greenKoopaTroopaL"],
            &textures["blueKoopaTroopaL"],
            &textures["yellowKoopaTroopaL"],
            &textures["rexL"],
            &textures["montyMoleL"],
            &textures["bobOmbL"],
            &textures["bulletBillL"],
            &textures["buzzyBeetleL"],
            &textures["mummyBeetleL"],
            &textures["swooperL"],
            &textures["banzaiBillL"],
            &textures["muncher"],
            &textures["piranhaPlant"],
            &textures["jumpingPiranhaPlant"]
        };

        offset = 0;
        int max = baddiesTextures.size();
        int marginLeft = 0;
        int maxWidth = 0;

        std::vector intervals{
            Vector2( 0, 2 ),
            Vector2( 2, 6 ),
            Vector2( 6, 13 ),
            Vector2( 13, 14 ),
            Vector2( 14, max )
        };

        for ( const auto& interval : intervals ) {

            marginLeft += maxWidth + 10;
            maxWidth = 0;
            offset = 0;
            size_t ini = static_cast<size_t>( interval.x );
            size_t end = static_cast<size_t>( interval.y );

            for ( size_t i = ini; i < end; i++ ) {
                if ( maxWidth < baddiesTextures[i]->width ) {
                    maxWidth = baddiesTextures[i]->width;
                }
            }

            for ( size_t i = ini; i < end; i++ ) {
                baddiesToSelect.push_back(
                    Tile(
                        Vector2(
                            componentPropertiesRect.x + marginLeft + maxWidth / 2 - baddiesTextures[i]->width / 2,
                            componentPropertiesRect.y + 10 + offset
                        ),
                        baddiesTextures[i],
                        1,
                        false,
                        Vector2( pos.y, pos.y )
                    )
                );
                offset += baddiesTextures[i]->height + 10;
            }

        }

        selectedBaddie = baddiesToSelect.data();
        selectedBaddie->setSelected( true );

        mario.setTexture( &textures["marioR"] );

        resourceDependantComponentsCreated = true;

    }

    guiContainerRect.width = GetScreenWidth() - ( tileComposerDim.x + 60 );
    guiContainerRect.height = GetScreenHeight() - 20;

    mapPropertiesRect.width = guiContainerRect.width - layersPreviewRect.width - 30;

    componentPropertiesRect.width = guiContainerRect.width - layersPreviewRect.width - 30;
    componentPropertiesRect.height = guiContainerRect.height - mapPropertiesRect.height - 40;

    terrainRect.height = componentPropertiesRect.height - 50;
    pipesRect.height = terrainRect.height;

    staticRect.height = componentPropertiesRect.height - 25;
    interactiveRect.height = staticRect.height;

    Vector2 mousePos = GetMousePosition();

    if ( isMouseInsideEditor( mousePos ) ) {
        if ( activeInsertOption == static_cast<int>( ComponentInsertionType::select ) ) {
            SetMouseCursor( MOUSE_CURSOR_CROSSHAIR );
        } else {
            SetMouseCursor( MOUSE_CURSOR_ARROW );
        }
    } else {
        SetMouseCursor( MOUSE_CURSOR_ARROW );
    }

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {    

        for ( int i = maxLayers - 1; i >= 0; i-- ) {
            Rectangle previewRect( layersPreviewRect.x + 40,
                                   layersPreviewRect.y + 10 + ( maxLayers - i - 1 ) * ( previewTileWidth * minLines + 10 ),
                                   previewTileWidth * minColumns,
                                   previewTileWidth * minLines );
            if ( CheckCollisionPointRec( mousePos, previewRect ) ) {
                currentLayer = i + 1;
            }
        }

        if ( isMouseInsideEditor( mousePos ) ) {

            computePressedLineAndColumn( mousePos, pressedLine, pressedColumn );

            Tile* tile = getTileFromPosition( mousePos );

            if ( tile != nullptr ) {
                if ( activeInsertOption == static_cast<int>( ComponentInsertionType::tiles ) ) {
                    if ( tilePaintingType == static_cast<int>( TilePaintingType::textured ) ) {
                        if ( selectedTile != nullptr ) {
                            tile->copyData( *selectedTile, Tile::getCollisionTypeFromInt( tileCollisionType ), tileVisible );
                        }
                    } else { // colored
                        tile->copyData( coloredModelTile, Tile::getCollisionTypeFromInt( tileCollisionType ), tileVisible );
                    }
                } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::blocks ) ) {
                    if ( selectedBlock != nullptr ) {
                        tile->copyData( *selectedBlock, TileCollisionType::solid, true );
                    }
                } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::items ) ) {
                    if ( selectedItem != nullptr ) {
                        tile->copyData( *selectedItem, TileCollisionType::solid, true );
                    }
                } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::baddies ) ) {
                    if ( selectedBaddie != nullptr ) {
                        tile->copyData( *selectedBaddie, TileCollisionType::solid, true );
                    }
                } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::mario ) ) {
                    tile->copyData( mario, TileCollisionType::solid, true );
                    if ( lastMarioTile != nullptr ) {
                        Tile::resetTile( *lastMarioTile );
                    }
                    lastMarioTile = tile;
                } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::select ) ) {
                    if ( !tile->isSelected() ) {
                        tile->setSelected( true );
                        selectedTiles.push_back( tile );
                    }
                }
            }
            
        }

        if ( activeInsertOption == static_cast<int>( ComponentInsertionType::tiles) && !terrainPageEdit && !pipesPageEdit ) {

            int ini = currentTerrainTile * 18;
            int end = ini + 18;

            for ( int i = ini; i < end; i++ ) {
                Tile& tile = tilesToSelect[i];
                if ( CheckCollisionPointRec( mousePos, tile.getRectangle() ) ) {
                    if ( selectedTile != nullptr ) {
                        selectedTile->setSelected( false );
                    }
                    selectedTile = &tile;
                    selectedTile->setSelected( true );
                    break;
                }
            }

            ini = currentPipeColor * 6;
            end = ini + 6;

            for ( int i = ini; i < end; i++ ) {
                Tile& tile = pipesToSelect[i];
                if ( CheckCollisionPointRec( mousePos, tile.getRectangle() ) ) {
                    if ( selectedTile != nullptr ) {
                        selectedTile->setSelected( false );
                    }
                    selectedTile = &tile;
                    selectedTile->setSelected( true );
                    break;
                }
            }

        } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::blocks ) ) {

            for ( auto& tile : blocksToSelect ) {
                if ( CheckCollisionPointRec( mousePos, tile.getRectangle() ) ) {
                    if ( selectedBlock != nullptr ) {
                        selectedBlock->setSelected( false );
                    }
                    selectedBlock = &tile;
                    selectedBlock->setSelected( true );
                    break;
                }
            }

        } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::items ) ) {

            for ( auto& tile : itemsToSelect ) {
                if ( CheckCollisionPointRec( mousePos, tile.getRectangle() ) ) {
                    if ( selectedItem != nullptr ) {
                        selectedItem->setSelected( false );
                    }
                    selectedItem = &tile;
                    selectedItem->setSelected( true );
                    break;
                }
            }

        } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::baddies ) ) {

            for ( auto& tile : baddiesToSelect ) {
                if ( CheckCollisionPointRec( mousePos, tile.getRectangle() ) ) {
                    if ( selectedBaddie != nullptr ) {
                        selectedBaddie->setSelected( false );
                    }
                    selectedBaddie = &tile;
                    selectedBaddie->setSelected( true );
                    break;
                }
            }

        }

    } else if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {

        if ( isMouseInsideEditor( mousePos ) ) {

            int currentLine;
            int currentColumn;

            computePressedLineAndColumn( mousePos, currentLine, currentColumn );

            if ( pressedLine != currentLine || 
                 pressedColumn != currentColumn ) {

                Tile* tile = getTileFromPosition( mousePos );

                if ( tile != nullptr ) {
                    if ( activeInsertOption == static_cast<int>( ComponentInsertionType::tiles ) ) {
                        if ( tilePaintingType == static_cast<int>( TilePaintingType::textured ) ) {
                            if ( selectedTile != nullptr ) {
                                tile->copyData( *selectedTile, Tile::getCollisionTypeFromInt( tileCollisionType ), tileVisible );
                            }
                        } else { // colored
                            tile->copyData( coloredModelTile, Tile::getCollisionTypeFromInt( tileCollisionType ), tileVisible );
                        }
                    } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::blocks ) ) {
                        if ( selectedBlock != nullptr ) {
                            tile->copyData( *selectedBlock, TileCollisionType::solid, true );
                        }
                    } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::items ) ) {
                        if ( selectedItem != nullptr ) {
                            tile->copyData( *selectedItem, TileCollisionType::solid, true );
                        }
                    } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::baddies ) ) {
                        if ( selectedBaddie != nullptr ) {
                            tile->copyData( *selectedBaddie, TileCollisionType::solid, true );
                        }
                    } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::select ) ) {
                        if ( !tile->isSelected() ) {
                            tile->setSelected( true );
                            selectedTiles.push_back( tile );
                        }
                    }
                }

            }

        } else {
            if ( !selectedTiles.empty() ) {
                for ( Tile* t : selectedTiles ) {
                    Tile::resetTile( *t );
                    if ( activeInsertOption == static_cast<int>( ComponentInsertionType::tiles ) ) {
                        if ( tilePaintingType == static_cast<int>( TilePaintingType::textured ) ) {
                            if ( selectedTile != nullptr ) {
                                t->copyData( *selectedTile, Tile::getCollisionTypeFromInt( tileCollisionType ), tileVisible );
                            }
                        } else {
                            t->copyData( coloredModelTile, Tile::getCollisionTypeFromInt( tileCollisionType ), tileVisible );
                        }
                    } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::blocks ) ) {
                        if ( selectedBlock != nullptr ) {
                            t->copyData( *selectedBlock, TileCollisionType::solid, true );
                        }
                    } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::items ) ) {
                        if ( selectedItem != nullptr ) {
                            t->copyData( *selectedItem, TileCollisionType::solid, true );
                        }
                    } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::baddies ) ) {
                        if ( selectedBaddie != nullptr ) {
                            t->copyData( *selectedBaddie, TileCollisionType::solid, true );
                        }
                    }
                }
            }
        }


        /* else if ( CheckCollisionPointRec( mousePos, colorPickerTileContainerRect ) ) {

            for ( auto& tile : layers[currentLayer - 1] ) {
                if ( tile->isSelected() && firstSelectedTile != nullptr ) {
                    tile->setColor( *(firstSelectedTile->getColor()) );
                    tile->setAlpha( *(firstSelectedTile->getAlpha()) );
                }
            }

        }*/

    } else if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) ) {
        pressedLine = -1;
        pressedColumn = -1;
    }

    if ( IsKeyPressed( KEY_ESCAPE ) ) {
        deselectTiles();
    }

    if ( IsKeyPressed( KEY_DELETE ) ) {
        if ( !selectedTiles.empty() ) {
            for ( Tile* t : selectedTiles ) {
                t->setSelected( false );
                Tile::resetTile( *t );
            }
            selectedTiles.clear();
        }
    }

    if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
        if ( IsKeyDown( KEY_W ) || IsKeyDown( KEY_UP ) ) {
            viewOffsetLine++;
        } else if ( IsKeyDown( KEY_S ) || IsKeyDown( KEY_DOWN ) ) {
            viewOffsetLine--;
        } else if ( IsKeyDown( KEY_A ) || IsKeyDown( KEY_LEFT ) ) {
            viewOffsetColumn--;
        } else if ( IsKeyDown( KEY_D ) || IsKeyDown( KEY_RIGHT ) ) {
            viewOffsetColumn++;
        }
    }

    const int mouseWheelMove = GetMouseWheelMove();
    if ( mouseWheelMove > 0 ) {
        if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
            viewOffsetColumn++;
        } else {
            viewOffsetLine++;
        }
    } else if ( mouseWheelMove ) {
        if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
            viewOffsetColumn--;
        } else {
            viewOffsetLine--;
        }
    }

    if ( IsKeyPressed( KEY_W ) || IsKeyPressed( KEY_UP ) ) {
        viewOffsetLine++;
    } else if ( IsKeyPressed( KEY_S ) || IsKeyPressed( KEY_DOWN ) ) {
        viewOffsetLine--;
    } else if ( IsKeyPressed( KEY_A ) || IsKeyPressed( KEY_LEFT ) ) {
        viewOffsetColumn--;
    } else if ( IsKeyPressed( KEY_D ) || IsKeyPressed( KEY_RIGHT ) ) {
        viewOffsetColumn++;
    }

    if ( viewOffsetLine < 0 ) {
        viewOffsetLine = 0;
    }

    if ( viewOffsetLine > lines - minLines ) {
        viewOffsetLine = lines - minLines;
    }

    if ( viewOffsetColumn < 0 ) {
        viewOffsetColumn = 0;
    }

    if ( viewOffsetColumn > columns - minColumns ) {
        viewOffsetColumn = columns - minColumns;
    }


    std::map<std::string, Music>& musics = ResourceManager::getMusics();
    const Music currentSelectedMusic = musics[TextFormat( "music%d", musicId )];
    const Music previousSelectedMusic = musics[TextFormat( "music%d", previousSelectedMusicId )];

    if ( playMusic ) {
        if ( musicId != previousSelectedMusicId ) {
            StopMusicStream( previousSelectedMusic );
            PlayMusicStream( currentSelectedMusic );
        } else {
            if ( IsMusicStreamPlaying( currentSelectedMusic ) ) {
                UpdateMusicStream( currentSelectedMusic );
            } else {
                PlayMusicStream( currentSelectedMusic );
            }
        }
    } else {
        if ( IsMusicStreamPlaying( previousSelectedMusic ) ) {
            StopMusicStream( previousSelectedMusic );
        }
        if ( IsMusicStreamPlaying( currentSelectedMusic ) ) {
            StopMusicStream( currentSelectedMusic );
        }
    }

    previousSelectedMusicId = musicId;


}

void MapEditor::draw() {

    std::map<std::string, Texture2D> &textures = ResourceManager::getTextures();

    startLine = lines - minLines - viewOffsetLine;
    startColumn = viewOffsetColumn;

    // background
    DrawRectangle( pos.x, pos.y, minColumns * Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH, backgroundColor );

    if ( backgroundTextureId > 0 ) {
        const Texture2D backgroundTexture = textures[TextFormat( "background%d", backgroundTextureId )];
        const int repeats = ( columns * Tile::TILE_WIDTH ) / backgroundTexture.width + 1;

        for ( int i = 0; i < repeats; i++ ) {
            DrawTexture(
                backgroundTexture,
                pos.x + backgroundTexture.width * i - startColumn * Tile::TILE_WIDTH,
                pos.y - backgroundTexture.height + ( lines - startLine ) * Tile::TILE_WIDTH,
                WHITE
            );
        }
    }

    DrawRectangle( 0, 0, GetScreenWidth(), pos.y, WHITE );
    DrawRectangle( 0, pos.y + minLines * Tile::TILE_WIDTH, GetScreenWidth(), GetScreenHeight(), WHITE );
    DrawRectangle( 0, 0, pos.x, GetScreenHeight(), WHITE );
    DrawRectangle( pos.x + minColumns * Tile::TILE_WIDTH, 0, GetScreenWidth(), GetScreenHeight(), WHITE );

    // tiles
    for ( int k = 0; k < maxLayers; k++ ) {
        if ( layersState[k].visible ) {
            for ( int i = startLine; i < startLine + minLines; i++ ) {
                for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
                    layers[k][i * columns + j]->draw( Vector2( pos.x + ( j - startColumn ) * Tile::TILE_WIDTH, pos.y + ( i - startLine ) * Tile::TILE_WIDTH ) );
                }
            }
        }
    }

    // rulers background
    DrawRectangle( pos.x + tileComposerDim.x, pos.y, Tile::TILE_WIDTH, minLines * Tile::TILE_WIDTH + 1, Fade( LIGHTGRAY, 0.5 ) );
    DrawRectangle( pos.x-1, pos.y + tileComposerDim.y, minColumns * Tile::TILE_WIDTH + 1, Tile::TILE_WIDTH, Fade( LIGHTGRAY, 0.5 ) );

    // grid and rulers
    for ( int i = 0; i <= minLines + 1; i++ ) {
        if ( showGrid || i == 0 || ( i >= minLines ) ) {
            DrawLine( pos.x - 1, pos.y + i * Tile::TILE_WIDTH, pos.x + minColumns * Tile::TILE_WIDTH, pos.y + i * Tile::TILE_WIDTH, BLACK );
        }
        if ( i < minLines ) {
            const char* t = TextFormat( "%d", startLine + i + 1 );
            DrawText( t, pos.x + tileComposerDim.x + Tile::TILE_WIDTH / 2 - MeasureText( t, 10 ) / 2, pos.y + i * Tile::TILE_WIDTH + 12, 10, BLACK );
        }
    }

    for ( int i = 0; i <= minColumns + 1; i++ ) {
        if ( showGrid || i == 0 || ( i >= minColumns ) ) {
            DrawLine( pos.x + i * Tile::TILE_WIDTH, pos.y, pos.x + i * Tile::TILE_WIDTH, pos.y + minLines * Tile::TILE_WIDTH + 1, BLACK );
        }
        if ( i < minColumns ) {
            const char* t = TextFormat( "%d", startColumn + i + 1 );
            DrawText( t, pos.x + i * Tile::TILE_WIDTH + Tile::TILE_WIDTH / 2 - MeasureText( t, 10 ) / 2, pos.y + tileComposerDim.y + 12, 10, BLACK );
        }
    }

    // selected tiles
    /*for ( int i = startLine; i < startLine + minLines; i++ ) {
        for ( int j = startColumn; j < startColumn + minColumns; j++ ) {
            if ( layers[currentLayer - 1][i * columns + j]->isSelected() ) {
                Vector2 p( pos.x + ( j - startColumn ) * Tile::TILE_WIDTH, pos.y + ( i - startLine ) * Tile::TILE_WIDTH );
                Vector2 d( Tile::TILE_WIDTH, Tile::TILE_WIDTH );
                DrawRectangleLinesEx( Rectangle( p.x - 2, p.y - 1, d.x + 3, d.y + 3 ), 3, BLACK );
                DrawCircle( p.x + d.x - 6, p.y + d.y - 5, 2, Fade( BLACK, 0.5 ) );
            }
        }
    }*/

    // GUI
    GuiCheckBox( checkShowGridRect, "Show Grid", &showGrid );
    GuiCheckBox( checkPlayMusicRect, "Play Music", &playMusic );

    GuiToggleGroup( toogleGroupInsertRect, ";;;;;", &activeInsertOption );
    DrawTexture( textures["B1"], toogleGroupInsertRect.x + 6, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["block9"], toogleGroupInsertRect.x + toogleGroupInsertRect.width + 8, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["coin"], toogleGroupInsertRect.x + toogleGroupInsertRect.width * 2 + 10, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["goombaR"], toogleGroupInsertRect.x + toogleGroupInsertRect.width * 3 + 12, toogleGroupInsertRect.y + 6, WHITE );
    DrawTexture( textures["marioR"], toogleGroupInsertRect.x + toogleGroupInsertRect.width * 4 + 12, toogleGroupInsertRect.y + 2, WHITE );
    DrawTexture( textures["selectBlock"], toogleGroupInsertRect.x + toogleGroupInsertRect.width * 5 + 16, toogleGroupInsertRect.y + 6, WHITE );

    GuiGroupBox( guiContainerRect, "Options" );
    GuiGroupBox( layersPreviewRect, "Layers" );

    for ( int i = maxLayers - 1; i >= 0; i-- ) {
        Vector2 pos( layersPreviewRect.x + 40,
                     layersPreviewRect.y + 10 + ( maxLayers - i - 1 ) * ( previewTileWidth * minLines + 10 ) );
        drawLayerPreview( pos.x, pos.y,
                          previewTileWidth,
                          i + 1 == currentLayer, layers[i] );
        GuiCheckBox( Rectangle( pos.x - 30, pos.y + ( previewTileWidth * minLines ) / 2 - 10, 20, 20 ), nullptr, &( layersState[i].visible ) );
    }

    GuiGroupBox( mapPropertiesRect, "Map Properties" );
    GuiSpinner( spinnerLinesRect, "Lines: ", &lines, minLines, maxLines, linesEdit );
    GuiSpinner( spinnerColumnsRect, "Columns: ", &columns, minColumns, maxColumns, columnsEdit );

    GuiLabel( labelBackgroundColorRect, "Background Color: " );
    GuiColorPicker( colorPickerBackgroundColorRect, nullptr, &backgroundColor );
    GuiSpinner( spinnerBackgroundTextureIdRect, "Background Texture: ", &backgroundTextureId, 0, 10, false );
    GuiSpinner( spinnerMusicIdRect, "Music: ", &musicId, 1, 9, false );
    if ( GuiSpinner( spinnerTimeToFinishRect, "Time to Finish: ", &timeToFinish, 1, 2000, timeToFinishEdit ) ) timeToFinishEdit = !timeToFinishEdit;

    if ( activeInsertOption == static_cast<int>(ComponentInsertionType::tiles) ) {

        GuiGroupBox( componentPropertiesRect, "Tiles" );

        GuiToggleGroup( togglePaintingTypeRect, "textured;colored", &tilePaintingType );
        GuiCheckBox( checkVisibleRect, "Visible", &tileVisible );

        GuiGroupBox( colorPickerTileContainerRect, "Color" );
        if ( firstSelectedTile != nullptr ) {
            GuiColorPicker( colorPickerTileRect, nullptr, firstSelectedTile->getColor() );
            GuiColorBarAlpha( sliderAlphaTileRect, nullptr, firstSelectedTile->getAlpha() );
        } else {
            GuiColorPicker( colorPickerTileRect, nullptr, coloredModelTile.getColor() );
            GuiColorBarAlpha( sliderAlphaTileRect, nullptr, coloredModelTile.getAlpha() );
        }

        if ( GuiDropdownBox( comboTileCollisionTypeRect, "solid;solid from above;solid only for baddies;non-solid", &tileCollisionType, tileCollisionTypeEdit ) ) tileCollisionTypeEdit = !tileCollisionTypeEdit;

        GuiGroupBox( terrainRect, "Terrain" );
        GuiGroupBox( pipesRect, "Pipes" );

        int ini = currentTerrainTile * 18;
        int end = ini + 18;

        for ( int i = ini; i < end; i++ ) {
            tilesToSelect[i].draw();
            if ( tilesToSelect[i].isSelected() ) {
                highlightSelectedTile( tilesToSelect[i] );
            }
        }

        ini = currentPipeColor * 6;
        end = ini + 6;

        for ( int i = ini; i < end; i++ ) {
            pipesToSelect[i].draw();
            if ( pipesToSelect[i].isSelected() ) {
                highlightSelectedTile( pipesToSelect[i] );
            }
        }

        Rectangle comboTerrainRect(
            comboTileCollisionTypeRect.x + comboTileCollisionTypeRect.width + 10,
            comboTileCollisionTypeRect.y,
            terrainRect.width,
            comboTileCollisionTypeRect.height
        );

        Rectangle comboPipeColorsRect(
            comboTerrainRect.x + comboTerrainRect.width + 10,
            comboTerrainRect.y,
            pipesRect.width,
            comboTileCollisionTypeRect.height
        );

        if ( GuiDropdownBox(
            comboPipeColorsRect,
            pipeColorOptions.c_str(), &currentPipeColor, pipesPageEdit ) ) pipesPageEdit = !pipesPageEdit;

        if ( GuiDropdownBox(
            comboTerrainRect,
            "terrain 1;terrain 2;terrain 3;terrain 4", &currentTerrainTile, terrainPageEdit ) ) terrainPageEdit = !terrainPageEdit;

    } else if ( activeInsertOption == static_cast<int>(ComponentInsertionType::blocks) ) {

        GuiGroupBox( componentPropertiesRect, "Blocks" );

        GuiGroupBox( staticRect, "Static" );
        GuiGroupBox( interactiveRect, "Interactive" );

        for ( auto& block : blocksToSelect ) {
            block.draw();
            if ( block.isSelected() ) {
                highlightSelectedTile( block );
            }
        }

    } else if ( activeInsertOption == static_cast<int>(ComponentInsertionType::items) ) {

        GuiGroupBox( componentPropertiesRect, "Items" );

        for ( auto& item : itemsToSelect ) {
            item.draw();
            if ( item.isSelected() ) {
                highlightSelectedTile( item );
            }
        }
        
    } else if ( activeInsertOption == static_cast<int>(ComponentInsertionType::baddies) ) {

        GuiGroupBox( componentPropertiesRect, "Baddies" );

        for ( auto& baddie : baddiesToSelect ) {
            baddie.draw();
            if ( baddie.isSelected() ) {
                highlightSelectedTile( baddie );
            }
        }

    }

    if ( lines != previousLines || columns != previousColumns ) {
        deselectTiles();
        for ( int i = 0; i < maxLayers; i++ ) {
            relocateTiles( layers[i] );
        }
    }

    previousLines = lines;
    previousColumns = columns;


    Vector2 mousePos = GetMousePosition();
    if ( isMouseInsideEditor( mousePos ) ) {

        /*mousePos.x -= Tile::TILE_WIDTH / 2;
        mousePos.y -= Tile::TILE_WIDTH / 2;*/

        if ( activeInsertOption == static_cast<int>( ComponentInsertionType::tiles ) ) {
            if ( tilePaintingType == static_cast<int>( TilePaintingType::textured ) ) {
                if ( selectedTile != nullptr ) {
                    selectedTile->draw( mousePos, true );
                }
            } else { // colored
                coloredModelTile.draw( mousePos, true );
            }
        } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::blocks ) ) {
            if ( selectedBlock != nullptr ) {
                selectedBlock->draw( mousePos, true );
            }
        } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::items ) ) {
            if ( selectedItem != nullptr ) {
                selectedItem->draw( mousePos, true );
            }
        } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::baddies ) ) {
            if ( selectedBaddie != nullptr ) {
                selectedBaddie->draw( mousePos, true );
            }
        } else if ( activeInsertOption == static_cast<int>( ComponentInsertionType::mario ) ) {
            mario.draw( mousePos, true );
        }
        
    }

}

void MapEditor::relocateTiles( std::vector<Tile*>& tiles ) const {

    const std::vector<Tile*> prevTiles = tiles;
    tiles.clear();

    const int lineDiff = abs( lines - previousLines );
    const int columnDiff = abs( columns - previousColumns );

    if ( lines > previousLines ) {

        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                if ( i < lineDiff ) {
                    tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE, 0, true, Vector2( pos.x, pos.y ) ) );
                } else {
                    Tile *tile = prevTiles[( i - lineDiff ) * columns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                }
            }
        }

    } else if( lines < previousLines ) {

        for ( int i = 0; i < previousLines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                if ( i < lineDiff ) {
                    delete prevTiles[i * columns + j];
                } else {
                    Tile* tile = prevTiles[i * columns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, (i-lineDiff) * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                }
            }
        }

    }

    if ( columns > previousColumns ) {

        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < columns; j++ ) {
                if ( j < previousColumns ) {
                    Tile* tile = prevTiles[i * previousColumns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                } else {
                    tiles.push_back( new Tile( Vector2( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH ), WHITE, 0, true, Vector2( pos.x, pos.y ) ) );
                }
            }
        }

    } else if ( columns < previousColumns ) {

        for ( int i = 0; i < lines; i++ ) {
            for ( int j = 0; j < previousColumns; j++ ) {
                if ( j < columns ) {
                    Tile* tile = prevTiles[i * previousColumns + j];
                    tile->setPos( j * Tile::TILE_WIDTH, i * Tile::TILE_WIDTH );
                    tiles.push_back( tile );
                } else {
                    delete prevTiles[i * previousColumns + j];
                }
            }
        }

    }

}