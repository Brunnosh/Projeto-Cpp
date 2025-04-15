#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game.h>
#include <game_helper.h>
#include <iostream>
#include <chrono>

void frameSetups() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void perFrameLogic() {

}

std::chrono::steady_clock::time_point fpsStartTime;
float  fpsCount, avgFps, lowestFps, highestFps = 0;
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

Window& Game::getWindow() {
    return window;
}


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
    
    //Check game_helper.h (function de-clutter,( callbacks, setups))
    

    glfwSetWindowUserPointer(window.getNativeWindow(), this);
    glfwMakeContextCurrent(window.getNativeWindow());
    glfwSetFramebufferSizeCallback(window.getNativeWindow(), framebuffer_size_callback);
    glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
    glfwSetMouseButtonCallback(window.getNativeWindow(), mouse_button_callback);
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);
    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(VSYNC);


    setupImgui(*this);

    
    
    
    unsigned int atlas;
    glActiveTexture(GL_TEXTURE0);
    loadTexture(&atlas, "assets/atlas.png");
    window.useShader();
    window.getShader().setInt("atlas", 0);

   
    unsigned int crosshair;
    glActiveTexture(GL_TEXTURE1); 
    loadTexture(&crosshair, "assets/crosshair.png");


    
    
    
    Player playerteste;
    World mundoTeste(player.camera);
    this->currentWorld = &mundoTeste; //carregar mundo do arquivo e carregas as infos na classe.


    
    fpsStartTime = std::chrono::steady_clock::now();

    //Game Loop
    while (!window.shouldClose()) {
        frameSetups();


        perFrameLogic();
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

        processInput(*this,deltaTime);
        
        //------------------------

        //Render
        
        //get and set camera projection/view matrix
        glm::mat4 projection = glm::perspective(glm::radians(player.camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.1f, 5000.0f);
        window.getShader().setMat4("projection", projection);
        
        glm::mat4 view = player.camera.GetViewMatrix();
        window.getShader().setMat4("view", view);



        


        ImGui::Begin("Test");
        ImGui::Text("FPS: %f (Avg: %f, Min: %f, Max: %f)", fps, avgFps, lowestFps, highestFps);
        ImGui::Text("FrameTime (ms): %f", deltaTime * 100.0f);
        
        ImGui::Text("X %f", player.camera.position.x);
        ImGui::Text("Y %f", player.camera.position.y);
        ImGui::Text("Z %f", player.camera.position.z);
        ImGui::Text("Chunks rendered %d", mundoTeste.getNumberChunks());
        
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImVec2 center(window.WIDHT * 0.5f, window.HEIGHT * 0.5f);
        float halfSize = 16.0f;

        // Converta o identificador OpenGL para um ponteiro válido
        drawList->AddImage((intptr_t)crosshair,            // Identificador da textura OpenGL convertido para void*
            ImVec2(center.x - halfSize, center.y - halfSize),
            ImVec2(center.x + halfSize, center.y + halfSize));


        unsigned int modelLoc = glGetUniformLocation(window.getShader().ID, "model");     


        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        mundoTeste.update(player.camera,deltaTime, modelLoc);// actual world generation & rendering
        //world.tick(); // Ticking of entities/ blocks
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


        ImGui::Text("World update time (ms): %f", (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());




        


        //-------------
        

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swapBuffers();
        window.pollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    window.terminate();
}





