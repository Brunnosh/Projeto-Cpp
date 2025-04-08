#include <game.h>
#include <iostream>
#include <chrono>

Game::Game() {}
Game::~Game() {}

bool Game::init() {



    if (!window.init("Voxel Game")) {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }

    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    return true;
}

void Game::processInput() {

}

void Game::run() {
    //OpenGL Loop
    while (!window.shouldClose()) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        this->processInput();

        tick(deltaTime);
        render();

        window.swapBuffers();
        window.pollEvents();
    }

    shutdown();
}

    //World Updates
void Game::tick(float deltaTime) {
    // Atualize lógica do mundo ou entrada

}

void Game::render() {
    renderer.clear();
    renderer.render(); // renderiza o mundo ou cena atual
}

void Game::shutdown() {
    renderer.shutdown();
    window.terminate();
}