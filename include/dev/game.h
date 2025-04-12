#pragma once

#include <window.h>
#include <world.h>
#include <player.h>
#include <chunk.h>

class Game {
private:
    Window window;
    World *currentWorld;
    
    
    float lastFrameTime = 0.0f;

private:
    //void tick();
    

public:
    Player player;
    //Storage
    


    //Control/Timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    float camLastX = window.WIDHT / 2.0f;
    float camLastY = window.HEIGHT / 2.0f;

    //Keys
    bool escDown = false;
    bool lDown = false;

    //Toggles
    bool menu = false;
    bool firstMouse = true;
    bool wireframe = false;
    bool VSYNC = 1;

public:
    Game();
    ~Game();

    bool init();
    void run();
    void shutdown();
    void processInput();
   
    Window &getWindow();
    void loadAtlas(unsigned int* atlas);



};
