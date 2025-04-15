#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game.h>
Camera camera(glm::vec3(0.0f, 62.0f, 0.0f));
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

    initBlockUVs();


    
    World mundoTeste;
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

        processInput(*this, deltaTime);

        //------------------------

        //Render

        //get and set camera projection/view matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.1f, 5000.0f);
        window.getShader().setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        window.getShader().setMat4("view", view);

        ImGui::Begin("Debug Info");
        ImGui::Text("FPS: %f (Avg: %f, Min: %f, Max: %f)", fps, avgFps, lowestFps, highestFps);
        ImGui::Text("FrameTime (ms): %f", deltaTime * 100.0f);
        ImGui::Text("X %f", camera.position.x);
        ImGui::Text("Y %f", camera.position.y);
        ImGui::Text("Z %f", camera.position.z);
        ImGui::Text("Chunks rendered %d", mundoTeste.getNumberChunks());

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImVec2 center(window.WIDHT * 0.5f, window.HEIGHT * 0.5f);
        float size = 8.0f;

        drawList->AddImage((ImTextureID)(intptr_t)crosshair,
            ImVec2(center.x - size, center.y - size + 8),
            ImVec2(center.x + size, center.y + size + 8));

        unsigned int modelLoc = glGetUniformLocation(window.getShader().ID, "model");

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        mundoTeste.update(camera, deltaTime, modelLoc);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        ImGui::Text("World update time (ms): %f", (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

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
