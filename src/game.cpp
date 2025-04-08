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
    {
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (escDown) return;

            escDown = true;
            menu = !menu;
            firstMouse = true;
            glfwSetInputMode(this->window.getNativeWindow(), GLFW_CURSOR, menu ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);


        }
        else {
            escDown = false;
        }
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_L) == GLFW_PRESS) {
            if (lDown) return;

            lDown = true;
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_FILL : GL_LINE);

        }
        else {
            lDown = false;
        }


        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_P) == GLFW_PRESS)
            glfwSetWindowShouldClose(this->window.getNativeWindow(), true);

        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_W) == GLFW_PRESS)
            this->camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_S) == GLFW_PRESS)
            this->camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_A) == GLFW_PRESS)
            this->camera.processKeyboard(CameraMovement::LEFT, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_D) == GLFW_PRESS)
            this->camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
            this->camera.processKeyboard(CameraMovement::UP, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            this->camera.processKeyboard(CameraMovement::DOWN, deltaTime);
    }
}

void Game::run() {
    //setup


    //Carregar atlas

    renderer.useShader();





    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //OpenGL Loop
    while (!window.shouldClose()) {

        //per-frame - iterative logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        std::cout << "X: " << camera.position.x << "\n Z: " << camera.position.z << " ";
        this->processInput();
        
        //------------------------

        //Render
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.1f, 100.0f);
        renderer.getShader().setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        renderer.getShader().setMat4("view", view);





        //tick();
        render();
        //-------------
        



        window.swapBuffers();
        window.pollEvents();
    }

    shutdown();
}

    //World Updates
void Game::tick() {
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