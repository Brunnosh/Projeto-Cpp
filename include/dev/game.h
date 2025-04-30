#pragma once


#include <window.h>
#include <world.h>
#include <camera.h>
#include <renderer.h>
#include <threadPool.h>
#include <shader.h>

class Game {
private:
    Window window;
    Renderer worldRenderer;
    ThreadPool globalPool = ThreadPool(int(std::thread::hardware_concurrency() * 3 / 4));

public:
    std::unique_ptr<World> currentWorld;
    
    
    bool wireframe = false;
    bool VSYNC = 1;

public:
    Game::Game() {};
    Game::~Game() {};

    void Game::loop();

    bool Game::init();

    bool Game::setup();

    void Game::cleanup(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        window.terminate();
    }
    Window& Game::getWindow(){
        return window;

    }

    Renderer& Game::getRenderer() {
        return worldRenderer;
    }

    void Game::endFrame() {
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swapBuffers();
        window.pollEvents();
    }

private:

    void Game::loadShaders() {
        Shaders[shaderType::MAIN] = Shader("assets/shaders/texture/mainShaderVertex.glsl", "assets/shaders/texture/mainShaderFragment.glsl");
        Shaders[shaderType::OUTLINE] = Shader("assets/shaders/outline/outlineVertex.glsl", "assets/shaders/outline/outlineFragment.glsl");
        Shaders[shaderType::SKYBOX] = Shader("assets/shaders/skybox/skyboxVertex.glsl", "assets/shaders/skybox/skyboxFragment.glsl");

    }

    void Game::setupImgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window.getNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void Game::processInput(float deltaTime, Camera& camera);


    void Game::loadTexture(unsigned int* texture, const std::string& path);

};
