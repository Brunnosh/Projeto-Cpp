#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game.h>
#include <shader.h>
#include <threadPool.h>

#include <iostream>
#include <chrono>

Camera camera(glm::vec3(0.0f, 130.0f, 0.0f));

std::chrono::steady_clock::time_point fpsStartTime;
static int drawCallCount = 0;  // Contador de draw calls
static int drawcallsec = 0;  // Contador de draw calls
std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
const double interval = 1.0; // Intervalo de tempo (1 segundo)

float  fps, fpsCount, avgFps, lowestFps, highestFps = 0;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
unsigned int modelLoc;

unsigned int atlas, crosshair;

#include <game_helper.h>


bool Game::init() {
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

bool Game::setup() {

    loadShaders();
    initBlockUVs();

    glfwSetWindowUserPointer(window.getNativeWindow(), this);
    glfwMakeContextCurrent(window.getNativeWindow());
    glfwSetFramebufferSizeCallback(window.getNativeWindow(), framebuffer_size_callback);
    glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
    glfwSetMouseButtonCallback(window.getNativeWindow(), mouse_button_callback);
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);
    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(VSYNC);
    setupImgui();

    modelLoc = glGetUniformLocation(Shaders[shaderType::MAIN].ID, "model");


    glActiveTexture(GL_TEXTURE0);
    loadTexture(&atlas, "assets/atlas.png");

    Shaders[shaderType::MAIN].setInt("atlas", 0);

    glActiveTexture(GL_TEXTURE1);
    loadTexture(&crosshair, "assets/crosshair.png");


    currentWorld = std::make_unique<World>();

    fpsStartTime = std::chrono::steady_clock::now();

    worldRenderer.worldReference = currentWorld.get();

    

    return true;
}


//Check game_helper.h (function de-clutter,( callbacks, setups))
void Game::loop() {
    while (!window.shouldClose()) {

        drawCallCount = 0;

        frameSetups();

        perFrameLogic();

        processInput(deltaTime, camera);

        camera.update(*currentWorld, window, drawCallCount);

        updateCameraMatrices(window, Shaders[shaderType::MAIN]);



        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        currentWorld->queueChunks(camera);
        currentWorld->genChunks(worldRenderer,globalPool);


        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        drawGui(*currentWorld, window, crosshair, begin, end, currentWorld->sunAngle);

        calcDrawCalls();
        endFrame();
    }
    worldRenderer.cleanup();
    cleanup();


}

void Game::processInput(float deltaTime, Camera& camera) {


    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!camera.escDown) {
            camera.menu = !camera.menu;
            camera.firstMouse = true;
            glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, camera.menu ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
        camera.escDown = true;
    }
    else {
        camera.escDown = false;
    }

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_L) == GLFW_PRESS) {
        if (!camera.lDown) {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_FILL : GL_LINE);
        }
        camera.lDown = true;
    }
    else {
        camera.lDown = false;
    }

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_P) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.getNativeWindow(), true);

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::DOWN, deltaTime);
}

void Game::loadTexture(unsigned int* texture, const std::string& path) {

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    // set the texture wrapping parameters

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}






