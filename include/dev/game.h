#pragma once

#include <window.h>
#include <glRenderer.h>
#include <world.h>

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void shutdown();
    void processInput();

private:
    void tick(float deltaTime);
    void render();

    Window window;
    GLRenderer renderer;
    World world;

    float lastFrameTime = 0.0f;
};
