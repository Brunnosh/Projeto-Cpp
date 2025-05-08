#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    int WIDHT = 1280;
    int HEIGHT = 720;
public:
    bool init(const std::string& title);
    void pollEvents();
    void swapBuffers();
    bool shouldClose() const;
    void terminate();
    GLFWwindow* getNativeWindow();

    bool VSYNC = true;
private:
    GLFWwindow* window = nullptr;
    
};
