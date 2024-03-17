/**
 * @file ResourceManager.cpp
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager class implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include "raylib.h"
#include "ResourceManager.h"
#include <map>
#include <sstream>
#include <string>
#include <utils.h>
#include <vector>

#define RRES_IMPLEMENTATION
#include "rres.h"
#define RRES_RAYLIB_IMPLEMENTATION
#include "rres-raylib.h"

std::map<std::string, Texture2D> ResourceManager::textures;
std::map<std::string, Sound> ResourceManager::sounds;
std::map<std::string, Music> ResourceManager::musics;
std::vector<void*> ResourceManager::musicDataStreamDataPointers;

std::string ResourceManager::centralDirLocation = "resources/resources.rres";
rresCentralDir ResourceManager::centralDir = rresLoadCentralDirectory( centralDirLocation.c_str() );

void ResourceManager::loadTextureFromResource(
    const std::string& fileName,
    const std::string& textureKey ) {

    const unsigned int id = rresGetResourceId( centralDir, fileName.c_str() );
    rresResourceChunk chunk = rresLoadResourceChunk( centralDirLocation.c_str(), id );
    const int result = UnpackResourceChunk( &chunk );

    if ( result == 0 ) {
        const Image image = LoadImageFromResource( chunk );
        textures[textureKey] = LoadTextureFromImage( image );
        UnloadImage( image );
    }

    rresUnloadResourceChunk( chunk );

}

void ResourceManager::loadSoundFromResource(
    const std::string& fileName,
    const std::string& soundKey ) {

    const unsigned int id = rresGetResourceId( centralDir, fileName.c_str() );
    rresResourceChunk chunk = rresLoadResourceChunk( centralDirLocation.c_str(), id );
    const int result = UnpackResourceChunk( &chunk );

    if ( result == 0 ) {
        const Wave wave = LoadWaveFromResource( chunk );
        sounds[soundKey] = LoadSoundFromWave( wave );
        UnloadWave( wave );
    }

    rresUnloadResourceChunk( chunk );

}

void ResourceManager::loadMusicFromResource(
    const std::string& fileName,
    const std::string& musicKey ) {

    const unsigned int id = rresGetResourceId( centralDir, fileName.c_str() );
    rresResourceChunk chunk = rresLoadResourceChunk( centralDirLocation.c_str(), id );
    const int result = UnpackResourceChunk( &chunk );

    if ( result == 0 ) {
        unsigned int dataSize = 0;
        void* data = LoadDataFromResource( chunk, &dataSize );
        musics[musicKey] = LoadMusicStreamFromMemory( ".mp3", static_cast<unsigned char*>( data ), static_cast<int>( dataSize ) );
        musicDataStreamDataPointers.push_back( data );
    }

    rresUnloadResourceChunk( chunk );

}

void ResourceManager::loadTextures() {

    if ( textures.empty() ) {

        // load textures...

        // mario
        if ( loadFromRRES ) {
            loadTextureFromResource( "resources/images/sprites/mario/SmallMario_0.png", "marioR" );
        } else {
            textures["marioR"] = LoadTexture( "resources/images/sprites/mario/SmallMario_0.png" );
        }
        textures["marioL"] = texture2DFlipHorizontal( textures["marioR"] );


        // tiles
        for ( int i = 1; i <= 4; i++ ) {
            for ( char c = 'A'; c <= 'R'; c++ ) {
                std::stringstream ss;
                ss << c << i;
                if ( loadFromRRES ) {
                    loadTextureFromResource( TextFormat( "resources/images/tiles/%d/tile_%c.png", i, c ), ss.str() );
                } else {
                    textures[ss.str()] = LoadTexture( TextFormat( "resources/images/tiles/%d/tile_%c.png", i, c ) );
                }
            }
        }

        // pipes
        std::vector<std::string> pipeColors = { "blue", "darkgray", "gray", "green", "orange", "pink", "purple", "red", "yellow" };
        for ( auto const &c : pipeColors ) {
            for ( int i = 0; i < 4; i++ ) {
                std::stringstream ss;
                ss << "pipe_" << c << i;
                if ( loadFromRRES ) { // will fail! needs to update resource file
                    loadTextureFromResource( TextFormat( "resources/images/tiles/pipes/%s/tile_%d.png", c.c_str(), i ), ss.str() );
                } else {
                    textures[ss.str()] = LoadTexture( TextFormat( "resources/images/tiles/pipes/%s/tile_%d.png", c.c_str(), i ) );
                }
            }
            for ( int i = 0; i < 2; i++ ) {
                std::stringstream ss;
                ss << "sm_pipe_" << c << i;
                if ( loadFromRRES ) { // will fail! needs to update resource file
                    loadTextureFromResource( TextFormat( "resources/images/tiles/smallPipes/%s/tile_%d.png", c.c_str(), i ), ss.str() );
                } else {
                    textures[ss.str()] = LoadTexture( TextFormat( "resources/images/tiles/smallPipes/%s/tile_%d.png", c.c_str(), i ) );
                }
            }
        }

        // generic
        if ( loadFromRRES ) {
            loadTextureFromResource( "resources/images/tiles/scenario/tile_CourseClearPoleBackTop.png", "tileCourseClearPoleBackTop" );
            loadTextureFromResource( "resources/images/tiles/scenario/tile_CourseClearPoleBackBody.png", "tileCourseClearPoleBackBody" );
            loadTextureFromResource( "resources/images/tiles/scenario/tile_CourseClearPoleFrontTop.png", "tileCourseClearPoleFrontTop" );
            loadTextureFromResource( "resources/images/tiles/scenario/tile_CourseClearPoleFrontBody.png", "tileCourseClearPoleFrontBody" );
        } else {
            textures["tileCourseClearPoleBackTop"] = LoadTexture( "resources/images/tiles/scenario/tile_CourseClearPoleBackTop.png" );
            textures["tileCourseClearPoleBackBody"] = LoadTexture( "resources/images/tiles/scenario/tile_CourseClearPoleBackBody.png" );
            textures["tileCourseClearPoleFrontTop"] = LoadTexture( "resources/images/tiles/scenario/tile_CourseClearPoleFrontTop.png" );
            textures["tileCourseClearPoleFrontBody"] = LoadTexture( "resources/images/tiles/scenario/tile_CourseClearPoleFrontBody.png" );
        }

        // blocks (editor)
        for ( int i = 0; i < 14; i++ ) {
            std::stringstream ss;
            ss << "block" << i;
            if ( loadFromRRES ) {
                loadTextureFromResource( TextFormat( "resources/images/sprites/blocks/block%d.png", i ), ss.str() );
            } else {
                textures[ss.str()] = LoadTexture( TextFormat( "resources/images/sprites/blocks/block%d.png", i ) );
            }
        }

        // backgrounds
        if ( loadFromRRES ) {
            loadTextureFromResource( "resources/images/backgrounds/background1.png", "background1" );
            loadTextureFromResource( "resources/images/backgrounds/background2.png", "background2" );
            loadTextureFromResource( "resources/images/backgrounds/background3.png", "background3" );
            loadTextureFromResource( "resources/images/backgrounds/background4.png", "background4" );
            loadTextureFromResource( "resources/images/backgrounds/background5.png", "background5" );
            loadTextureFromResource( "resources/images/backgrounds/background6.png", "background6" );
            loadTextureFromResource( "resources/images/backgrounds/background7.png", "background7" );
            loadTextureFromResource( "resources/images/backgrounds/background8.png", "background8" );
            loadTextureFromResource( "resources/images/backgrounds/background9.png", "background9" );
            loadTextureFromResource( "resources/images/backgrounds/background10.png", "background10" );
        } else {
            textures["background1"] = LoadTexture( "resources/images/backgrounds/background1.png" );
            textures["background2"] = LoadTexture( "resources/images/backgrounds/background2.png" );
            textures["background3"] = LoadTexture( "resources/images/backgrounds/background3.png" );
            textures["background4"] = LoadTexture( "resources/images/backgrounds/background4.png" );
            textures["background5"] = LoadTexture( "resources/images/backgrounds/background5.png" );
            textures["background6"] = LoadTexture( "resources/images/backgrounds/background6.png" );
            textures["background7"] = LoadTexture( "resources/images/backgrounds/background7.png" );
            textures["background8"] = LoadTexture( "resources/images/backgrounds/background8.png" );
            textures["background9"] = LoadTexture( "resources/images/backgrounds/background9.png" );
            textures["background10"] = LoadTexture( "resources/images/backgrounds/background10.png" );
        }

        // items
        if ( loadFromRRES ) {
            loadTextureFromResource( "resources/images/sprites/items/coin.png", "coin" );
            loadTextureFromResource( "resources/images/sprites/items/yoshiCoin.png", "yoshiCoin" );
        } else {
            textures["coin"] = LoadTexture( "resources/images/sprites/items/coin.png" );
            textures["yoshiCoin"] = LoadTexture( "resources/images/sprites/items/yoshiCoin.png" );
        }

        if ( loadFromRRES ) {
            loadTextureFromResource( "resources/images/sprites/baddies/BlueKoopaTroopa_0.png", "blueKoopaTroopaR" );
            loadTextureFromResource( "resources/images/sprites/baddies/BobOmb_0.png", "bobOmbR" );
            loadTextureFromResource( "resources/images/sprites/baddies/BulletBill_0.png", "bulletBillR" );
            loadTextureFromResource( "resources/images/sprites/baddies/BuzzyBeetle_0.png", "buzzyBeetleR" );
            loadTextureFromResource( "resources/images/sprites/baddies/FlyingGoomba_0.png", "flyingGoombaR" );
            loadTextureFromResource( "resources/images/sprites/baddies/Goomba_0.png", "goombaR" );
            loadTextureFromResource( "resources/images/sprites/baddies/GreenKoopaTroopa_0.png", "greenKoopaTroopaR" );
            loadTextureFromResource( "resources/images/sprites/baddies/MummyBeetle_0.png", "mummyBeetleR" );
            loadTextureFromResource( "resources/images/sprites/baddies/Muncher_0.png", "muncher" );
            loadTextureFromResource( "resources/images/sprites/baddies/PiranhaPlant_0.png", "piranhaPlant" );
            loadTextureFromResource( "resources/images/sprites/baddies/RedKoopaTroopa_0.png", "redKoopaTroopaR" );
            loadTextureFromResource( "resources/images/sprites/baddies/Rex_2_0.png", "rexR" );
            loadTextureFromResource( "resources/images/sprites/baddies/Swooper_1.png", "swooperR" );
            loadTextureFromResource( "resources/images/sprites/baddies/YellowKoopaTroopa_0.png", "yellowKoopaTroopaR" );
            loadTextureFromResource( "resources/images/sprites/baddies/MontyMole_0.png", "montyMoleR" );
            loadTextureFromResource( "resources/images/sprites/baddies/BanzaiBill_0.png", "banzaiBillR" );
            loadTextureFromResource( "resources/images/sprites/baddies/JumpingPiranhaPlant_0.png", "jumpingPiranhaPlant" );
        } else {
            textures["blueKoopaTroopaR"] = LoadTexture( "resources/images/sprites/baddies/BlueKoopaTroopa_0.png" );
            textures["bobOmbR"] = LoadTexture( "resources/images/sprites/baddies/BobOmb_0.png" );
            textures["bulletBillR"] = LoadTexture( "resources/images/sprites/baddies/BulletBill_0.png" );
            textures["buzzyBeetleR"] = LoadTexture( "resources/images/sprites/baddies/BuzzyBeetle_0.png" );
            textures["flyingGoombaR"] = LoadTexture( "resources/images/sprites/baddies/FlyingGoomba_0.png" );
            textures["goombaR"] = LoadTexture( "resources/images/sprites/baddies/Goomba_0.png" );
            textures["greenKoopaTroopaR"] = LoadTexture( "resources/images/sprites/baddies/GreenKoopaTroopa_0.png" );
            textures["mummyBeetleR"] = LoadTexture( "resources/images/sprites/baddies/MummyBeetle_0.png" );
            textures["muncher"] = LoadTexture( "resources/images/sprites/baddies/Muncher_0.png" );
            textures["piranhaPlant"] = LoadTexture( "resources/images/sprites/baddies/PiranhaPlant_0.png" );
            textures["redKoopaTroopaR"] = LoadTexture( "resources/images/sprites/baddies/RedKoopaTroopa_0.png" );
            textures["rexR"] = LoadTexture( "resources/images/sprites/baddies/Rex_2_0.png" );
            textures["swooperR"] = LoadTexture( "resources/images/sprites/baddies/Swooper_1.png" );
            textures["yellowKoopaTroopaR"] = LoadTexture( "resources/images/sprites/baddies/YellowKoopaTroopa_0.png" );
            textures["montyMoleR"] = LoadTexture( "resources/images/sprites/baddies/MontyMole_0.png" );
            textures["banzaiBillR"] = LoadTexture( "resources/images/sprites/baddies/BanzaiBill_0.png" );
            textures["jumpingPiranhaPlant"] = LoadTexture( "resources/images/sprites/baddies/JumpingPiranhaPlant_0.png" );
        }
        textures["blueKoopaTroopaL"] = texture2DFlipHorizontal( textures["blueKoopaTroopaR"] );
        textures["bobOmbL"] = texture2DFlipHorizontal( textures["bobOmbR"] );
        textures["bulletBillL"] = texture2DFlipHorizontal( textures["bulletBillR"] );
        textures["buzzyBeetleL"] = texture2DFlipHorizontal( textures["buzzyBeetleR"] );
        textures["flyingGoombaL"] = texture2DFlipHorizontal( textures["flyingGoombaR"] );
        textures["goombaL"] = texture2DFlipHorizontal( textures["goombaR"] );
        textures["greenKoopaTroopaL"] = texture2DFlipHorizontal( textures["greenKoopaTroopaR"] );
        textures["mummyBeetleL"] = texture2DFlipHorizontal( textures["mummyBeetleR"] );
        textures["redKoopaTroopaL"] = texture2DFlipHorizontal( textures["redKoopaTroopaR"] );
        textures["rexL"] = texture2DFlipHorizontal( textures["rexR"] );
        textures["swooperL"] = texture2DFlipHorizontal( textures["swooperR"] );
        textures["yellowKoopaTroopaL"] = texture2DFlipHorizontal( textures["yellowKoopaTroopaR"] );
        textures["montyMoleL"] = texture2DFlipHorizontal( textures["montyMoleR"] );
        textures["banzaiBillL"] = texture2DFlipHorizontal( textures["banzaiBillR"] );

    }

}

void ResourceManager::loadSounds() {

    if ( sounds.empty() ) {

        if ( loadFromRRES ) {
            
        } else {
            
        }

    }

}

void ResourceManager::loadMusics() {

    if ( musics.empty() ) {

        if ( loadFromRRES ) {
            loadMusicFromResource( "resources/musics/music1.mp3", "music1" );
            loadMusicFromResource( "resources/musics/music2.mp3", "music2" );
            loadMusicFromResource( "resources/musics/music3.mp3", "music3" );
            loadMusicFromResource( "resources/musics/music4.mp3", "music4" );
            loadMusicFromResource( "resources/musics/music5.mp3", "music5" );
            loadMusicFromResource( "resources/musics/music6.mp3", "music6" );
            loadMusicFromResource( "resources/musics/music7.mp3", "music7" );
            loadMusicFromResource( "resources/musics/music8.mp3", "music8" );
            loadMusicFromResource( "resources/musics/music9.mp3", "music9" );
        } else {
            musics["music1"] = LoadMusicStream( "resources/musics/music1.mp3" );
            musics["music2"] = LoadMusicStream( "resources/musics/music2.mp3" );
            musics["music3"] = LoadMusicStream( "resources/musics/music3.mp3" );
            musics["music4"] = LoadMusicStream( "resources/musics/music4.mp3" );
            musics["music5"] = LoadMusicStream( "resources/musics/music5.mp3" );
            musics["music6"] = LoadMusicStream( "resources/musics/music6.mp3" );
            musics["music7"] = LoadMusicStream( "resources/musics/music7.mp3" );
            musics["music8"] = LoadMusicStream( "resources/musics/music8.mp3" );
            musics["music9"] = LoadMusicStream( "resources/musics/music9.mp3" );
        }

    }

}

void ResourceManager::loadTexture( const std::string& key, const std::string& path ) {
    unloadTexture( key );
    textures[key] = LoadTexture( path.c_str() );
}

void ResourceManager::loadSound( const std::string& key, const std::string& path ) {
    unloadSound( key );
    sounds[key] = LoadSound( path.c_str() );
}

void ResourceManager::loadMusic( const std::string& key, const std::string& path ) {
    unloadMusic( key );
    musics[key] = LoadMusicStream( path.c_str() );
}

void ResourceManager::unloadTextures() {
    for ( auto const& [key, val] : textures ) {
        UnloadTexture( val );
    }
    textures.clear();
}

void ResourceManager::unloadSounds() {
    for ( auto const& [key, val] : sounds ) {
        UnloadSound( val );
    }
    sounds.clear();
}

void ResourceManager::unloadMusics() {
    for ( auto const& [key, val] : musics ) {
        UnloadMusicStream( val );
    }
    musics.clear();
}

void ResourceManager::unloadTexture( const std::string& key ) {
    if ( textures.contains( key ) ) {
        UnloadTexture( textures[key] );
        textures.erase( key );
    }
}

void ResourceManager::unloadSound( const std::string& key ) { 
    if ( sounds.contains( key ) ) {
        UnloadSound( sounds[key] );
        sounds.erase( key );
    }
}

void ResourceManager::unloadMusic( const std::string& key ) {
    if ( musics.contains( key ) ) {
        UnloadMusicStream( musics[key] );
        musics.erase( key );
    }
}

void ResourceManager::loadResources() {
    loadTextures();
    loadSounds();
    loadMusics();
    rresUnloadCentralDirectory( centralDir );
}

void ResourceManager::unloadResources() {
    unloadTextures();
    unloadSounds();
    unloadMusics();
    for ( const auto& data : musicDataStreamDataPointers ) {
        MemFree( data );
    }
}

std::map<std::string, Texture2D> &ResourceManager::getTextures() {
    return textures;
}

std::map<std::string, Sound> &ResourceManager::getSounds() {
    return sounds;
}

std::map<std::string, Music> &ResourceManager::getMusics() {
    return musics;
}