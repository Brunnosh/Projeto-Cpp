#include <glRenderer.h>
#include <glad/glad.h>
#include <iostream>

bool GLRenderer::init() {
    glEnable(GL_DEPTH_TEST);
    defaultShader = new Shader("assets/shaders/vertexshader.glsl", "assets/shaders/fragmentshader.glsl");
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    return true;
}

void GLRenderer::clear() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::render() {

}

void GLRenderer::useShader() {
    if (defaultShader) {
        defaultShader->use();
        // Aqui entraria o código de renderização do mundo
    }
}

Shader GLRenderer::getShader() {
    return *defaultShader;
}

void GLRenderer::shutdown() {
    delete defaultShader;
    defaultShader = nullptr;
}