#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <string>

class Window {
public:
    bool init(const std::string& title);
    bool glInit();
    void useShader();
    Shader getShader();
  
    void pollEvents();
    void swapBuffers();
    bool shouldClose() const;
    void terminate();
    int WIDHT = 1280;
    int HEIGHT = 720;

    GLFWwindow* getNativeWindow();

private:
    GLFWwindow* window = nullptr;
    Shader* defaultShader = nullptr;
};
