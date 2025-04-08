#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    bool init(const std::string& title);
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
