#pragma once

#include "Window.h"
#include "shader.h"
#include <glm/glm.hpp>

class GLRenderer {
public:
    bool init();
    void clear();
    void render();
    void shutdown();

private:
    Shader* defaultShader = nullptr;
};
