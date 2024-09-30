/**
 * @file GameWindow.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWindow class declaration. This class contains one instance of
 * the Game World and it is responsible to create a new Raylib window,
 * start the game loop and finish the Raylib window.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <string>

#include "GameWorld.h"

class GameWindow {
    
    int width;
    int height;
    std::string title;

    int targetFPS;
    bool antialiasing;
    bool resizable;
    bool fullScreen;
    bool undecorated;
    bool alwaysOnTop;
    bool alwaysRun;
    bool initAudio;

    GameWorld gw;

    bool initialized;

public:

    GameWindow();
    GameWindow( bool initAudio );
    GameWindow( int width, int height, std::string title, bool initAudio );
    GameWindow( int width, int height, std::string title, int targetFPS, bool initAudio );
    GameWindow( int width, int height, std::string title, bool antialiasing, bool initAudio );
    GameWindow( int width, int height, std::string title, int targetFPS, bool antialiasing, bool initAudio );
    GameWindow( int width, int height, std::string title, int targetFPS,
                bool antialiasing, bool resizable, bool fullScreen,
                bool undecorated, bool alwaysOnTop, bool alwaysRun, bool initAudio );

    ~GameWindow();

    void init();

    int getWidth() const;
    int getHeight() const;
    std::string getTitle() const;
    int getTargetFPS() const;
    bool isAntialiasing() const;
    bool isResizable() const;
    bool isFullScreen() const;
    bool isUndecorated() const;
    bool isAlwaysOnTop() const;
    bool isAlwaysRun() const;
    bool isInitAudio() const;
    bool isInitialized() const;

    void setWidth( int width );
    void setHeight( int height );
    void setTitle( std::string title );
    void setTargetFPS( int targetFPS );
    void setAntialiasing( bool antialiasing );
    void setResizable( bool resizable );
    void setFullScreen( bool fullScreen );
    void setUndecorated( bool undecorated );
    void setAlwaysOnTop( bool alwaysOnTop );
    void setAlwaysRun( bool alwaysRun );
    void setInitAudio( bool initAudio );
    
};