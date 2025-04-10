#pragma once

#include <window.h>
#include <world.h>
#include <camera.h>
#include <chunk.h>

class Game {
private:
    Window window;
    World *currentWorld;
    Camera camera = (glm::vec3(0.0f, 17.0f, 0.0f)); //CErto seria inicializar a camera quando entrar no mundo, mas agora vou inicializar pora qui mesmo
    
    float lastFrameTime = 0.0f;

private:
    //void tick();
    

public:
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
    Camera &getCamera();
    Window &getWindow();
    void loadAtlas(unsigned int* atlas);



};
