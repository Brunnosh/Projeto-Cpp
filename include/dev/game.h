#pragma once

#include <window.h>
#include <world.h>
#include <player.h>
#include <chunk.h>


class Game {
private:
    Window window;
    

public:
    World* currentWorld;
    Player player;


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
    Game() {};
    ~Game() {};

    bool init();
    void run();
  
   
   
    Window &getWindow();

    void loadTexture(unsigned int* texture, const std::string& path);

};
