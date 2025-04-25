#pragma once

#include <window.h>
#include <world.h>
#include <camera.h>
#include <renderer.h>

class Game {
private:
    Window window;
    Renderer worldRenderer;

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


};
