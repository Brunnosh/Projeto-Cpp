#pragma once

#include <window.h>
#include <world.h>
#include <chunk.h>

class Game {
private:
    Window window;

public:
    World* currentWorld = nullptr;

    // Keys
    bool escDown = false;
    bool lDown = false;

    // Toggles
    bool wireframe = false;
    bool VSYNC = 1;

public:
    Game() {};
    ~Game() {};

    bool init();
    void run();

    Window& getWindow();
    void loadTexture(unsigned int* texture, const std::string& path);
};
