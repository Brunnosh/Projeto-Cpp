#pragma once

#include <window.h>
#include <world.h>
#include <camera.h>
#include <chunk.h>

class Game {
private:
    Window window;

public:
    World* currentWorld = nullptr;

    // Keys


    // Toggles
    bool wireframe = false;
    bool VSYNC = 1;

public:
    Game() {};
    ~Game() {};

    void run();

    bool init() {


        if (!window.init("Voxel Game")) {
            std::cerr << "Failed to initialize window" << std::endl;
            return false;
        }

        if (!window.glInit()) {
            std::cerr << "Failed to initialize OpenGL" << std::endl;
            return false;
        }


        return true;
    }
    

    Window& getWindow(){ return window; }
    void loadTexture(unsigned int* texture, const std::string& path);
};
