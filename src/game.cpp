#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game.h>
#include <iostream>
#include <chrono>
#include <world.h>
#include <chunk.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

std::chrono::steady_clock::time_point fpsStartTime;
float fpsCount, avgFps, lowestFps, highestFps = 0;

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
    //setup
    
    
    glfwSetWindowUserPointer(window.getNativeWindow(), this);
    glfwMakeContextCurrent(window.getNativeWindow());
    glfwSetFramebufferSizeCallback(window.getNativeWindow(), framebuffer_size_callback);
    glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
    glfwSetMouseButtonCallback(window.getNativeWindow(), mouse_button_callback);
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);
    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(VSYNC);




    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.getNativeWindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init("#version 330");





    //Carregar atlas
    
    unsigned int atlas;
    loadAtlas(&atlas);

    //usar shader
    window.useShader();
    window.getShader().setInt("atlas", 0);
    initBlockUVs();

                    
    World mundoTeste(player.camera);
    this->currentWorld = &mundoTeste; //carregar mundo do arquivo e carregas as infos na classe.


    
    fpsStartTime = std::chrono::steady_clock::now();

    //OpenGL Loop
    while (!window.shouldClose()) {

        //per-frame - iterative logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        

        // FPS Calculations
        float fps = 1.0f / deltaTime;
        if (lowestFps == -1 || fps < lowestFps)
            lowestFps = fps;
        if (highestFps == -1 || fps > highestFps)
            highestFps = fps;
        fpsCount++;
        std::chrono::steady_clock::time_point currentTimePoint = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - fpsStartTime).count() > 1000)
        {
            avgFps = fpsCount;
            lowestFps = -1;
            highestFps = -1;
            fpsCount = 0;
            fpsStartTime = currentTimePoint;
        }





        //std::cout << "X: " << camera.position.x <<  " Y: " << camera.position.y <<  " Z: " << camera.position.z << "\n ";
        processInput();
        
        //------------------------

        //Render
        
        glm::mat4 projection = glm::perspective(glm::radians(player.camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.1f, 5000.0f);
        window.getShader().setMat4("projection", projection);
        // camera/view transformation
        glm::mat4 view = player.camera.GetViewMatrix();
        window.getShader().setMat4("view", view);


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        


        ImGui::Begin("Test");
        ImGui::Text("FPS: %f (Avg: %f, Min: %f, Max: %f)", fps, avgFps, lowestFps, highestFps);
        ImGui::Text("MS: %f", deltaTime * 100.0f);
        ImGui::Text("X %f", player.camera.position.x);
        ImGui::Text("Y %f", player.camera.position.y);
        ImGui::Text("Z %f", player.camera.position.z);
        ImGui::Text("Chunks rendered %d", mundoTeste.getNumberChunks());
        //ImGui::Text("Chunks: %d (%d rendered)", Planet::planet->numChunks, Planet::planet->numChunksRendered);
        ImGui::End();


        unsigned int modelLoc = glGetUniformLocation(window.getShader().ID, "model");     
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        mundoTeste.update(player.camera,deltaTime, modelLoc);// actual world generation & rendering
        //world.tick(); // Ticking of entities/ blocks
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        std::cout << "Planet Update: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
            << "[ms]\n";
         
        //-------------
        


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swapBuffers();
        window.pollEvents();
    }

    shutdown();
}



void Game::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
            this->player.camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_S) == GLFW_PRESS)
            this->player.camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_A) == GLFW_PRESS)
            this->player.camera.processKeyboard(CameraMovement::LEFT, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_D) == GLFW_PRESS)
            this->player.camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
            this->player.camera.processKeyboard(CameraMovement::UP, deltaTime);
        if (glfwGetKey(this->window.getNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            this->player.camera.processKeyboard(CameraMovement::DOWN, deltaTime);
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
        game->player.camera.processMouseScroll(static_cast<float>(yoffset));
    
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

    game->player.camera.processMouseMovement(xoffset, yoffset);

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "viado" << "";
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        std::cout << "viado" << "";
    }
        
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