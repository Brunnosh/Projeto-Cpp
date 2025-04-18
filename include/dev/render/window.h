#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    bool init(const std::string& title);
    bool glInit();

    
  
    void pollEvents();
    void swapBuffers();
    bool shouldClose() const;
    void terminate();
    int WIDHT = 1280;
    int HEIGHT = 720;

    GLFWwindow* getNativeWindow();

private:
    GLFWwindow* window = nullptr;
    
};
