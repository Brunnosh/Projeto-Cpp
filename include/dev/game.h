#pragma once

#include <window.h>
#include <glRenderer.h>
#include <world.h>
#include <camera.h>

class Game {
private:
    Window window;
    GLRenderer renderer;
    World world;
    Camera camera = (glm::vec3(0.0f, 17.0f, 0.0f)); //CErto seria inicializar a camera quando entrar no mundo, mas agora vou inicializar pora qui mesmo

    float lastFrameTime = 0.0f;

private:
    void tick();
    void render();

public:
    //Control/Timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    //Keys
    bool escDown = false;
    bool lDown = false;

    //Toggles
    bool menu = false;
    bool firstMouse = true;
    bool wireframe = false;

public:
    Game();
    ~Game();

    bool init();
    void run();
    void shutdown();
    void processInput();



};
