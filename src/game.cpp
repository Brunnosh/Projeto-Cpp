#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game.h>
#include <iostream>
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera& Game::getCamera() {
    return camera;
}

Window& Game::getWindow() {
    return window;
}

Game::Game() {}
Game::~Game() {}

bool Game::init() {



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



void Game::run() {
    Game* gameInstance = static_cast<Game*>(glfwGetWindowUserPointer(window.getNativeWindow()));
    
    glfwSetWindowUserPointer(window.getNativeWindow(), this);
    //setup
    glfwMakeContextCurrent(window.getNativeWindow());
    glfwSetFramebufferSizeCallback(window.getNativeWindow(), framebuffer_size_callback);
    //glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);
    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(VSYNC);

    //Carregar atlas
    
    unsigned int atlas;
    loadAtlas(&atlas);

    //usar shader
    window.useShader();
    window.getShader().setInt("atlas", 0);




   

    //OpenGL Loop
    while (!window.shouldClose()) {

        //per-frame - iterative logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


  

        std::cout << "X: " << camera.position.x << "\n Z: " << camera.position.z << " ";
        processInput();
        
        //------------------------

        //Render
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.1f, 100.0f);
        window.getShader().setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        window.getShader().setMat4("view", view);





        //tick(); // Ticking of entities/ blocks
        render(); // actual world generation
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
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //world.render(); // 
}

void Game::shutdown() {
    window.terminate();
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    game->getWindow().WIDHT = width;
    game->getWindow().HEIGHT = height;
    glViewport(0, 0, width, height);
    
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game)
        game->getCamera().processMouseScroll(static_cast<float>(yoffset));
    
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{



    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

    if (game->menu) return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (game->firstMouse)
    {
        game->camLastX = xpos;
        game->camLastY = ypos;
        game->firstMouse = false;
    }

    float xoffset = xpos - game->camLastX;
    float yoffset = game->camLastY - ypos; // reversed since y-coordinates go from bottom to top

    game->camLastX = xpos;
    game->camLastY = ypos;

    game->getCamera().processMouseMovement(xoffset, yoffset);

}

void Game::loadAtlas(unsigned int* atlas) {

    glGenTextures(1, atlas);
    glBindTexture(GL_TEXTURE_2D, *atlas);
    // set the texture wrapping parameters

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("assets/atlas.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
}