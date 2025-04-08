#include <glRenderer.h>
#include <glad/glad.h>
#include <iostream>

bool GLRenderer::init() {
    glEnable(GL_DEPTH_TEST);
    defaultShader = new Shader("assets/shaders/vertexshader.glsl", "assets/shaders/fragmentshader.glsl");
    return true;
}

void GLRenderer::clear() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::render() {
    if (defaultShader) {
        defaultShader->use();
        // Aqui entraria o código de renderização do mundo
    }
}

void GLRenderer::shutdown() {
    delete defaultShader;
    defaultShader = nullptr;
}