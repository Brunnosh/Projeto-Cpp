#pragma once

#include <window.h>
#include <world.h>
#include <camera.h>


class Game {
private:
    Window window;

public:
    World* currentWorld = nullptr;
    
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
    void cleanup(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        window.terminate();
    }
    Window& getWindow(){
        return window;
    }
    void endFrame() {
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swapBuffers();
        window.pollEvents();
    }


};
