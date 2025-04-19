#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game.h>
#include <shader.h>
#include <game_helper.h>
#include <iostream>
#include <chrono>



void loadShaders() {
    Shaders[shaderType::TEXTURE] = Shader("assets/shaders/vertexshader.glsl", "assets/shaders/fragmentshader.glsl");
    Shaders[shaderType::OUTLINE] = Shader("assets/shaders/outline/outlineVertex.glsl", "assets/shaders/outline/outlineFragment.glsl");
}

void drawGui(World & mundoTeste, Window & window, unsigned int crosshair, std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end) {

    //Debug panel
    ImGui::Begin("WorldDebug");
    ImGui::Text("FPS: %f (Avg: %f, Min: %f, Max: %f)", fps, avgFps, lowestFps, highestFps);
    ImGui::Text("FrameTime (ms): %f", deltaTime * 100.0f);
    ImGui::Text("X %f", camera.position.x);
    ImGui::Text("Y %f", camera.position.y);
    ImGui::Text("Z %f", camera.position.z);
    ImGui::Text("Chunks rendered %d", mundoTeste.getNumberChunks());
    ImGui::Text("World update time (ms): %f", (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    if (camera.selectedBlock != nullptr) ImGui::Text("Selected Block: %s", (*camera.selectedBlock).getTypeToString());
    ImGui::End();


    ImGui::Begin("LookinAt");
    if (camera.raycastInfo.has_value()) {
        
        auto& hit = camera.raycastInfo.value();
        ImGui::Text("BlockRelativeX %d", hit.blockRelativePos.x);
        ImGui::Text("BlockRelativeY %d", hit.blockRelativePos.y);
        ImGui::Text("BlockRelativeZ %d", hit.blockRelativePos.z);
        ImGui::Text("------------------");
        ImGui::Text("BlockWorldX %d", hit.blockWorldPos.x);
        ImGui::Text("BlockWorldY %d", hit.blockWorldPos.y);
        ImGui::Text("BlockWorldZ %d", hit.blockWorldPos.z);
        ImGui::Text("------------------");
        ImGui::Text("ChunkCoordX %d", hit.chunk->worldPos.x);
        ImGui::Text("ChunkCoordY %d", hit.chunk->worldPos.y);
        ImGui::Text("ChunkCoordZ %d", hit.chunk->worldPos.z);
        


  
    }
    ImGui::End();

    ImGui::Begin("CameraDebug");
    ImGui::Text("Yaw %f", camera.yaw);
    ImGui::Text("Pitch %f", camera.pitch);
    ImGui::Text("Fov %f", camera.fov);
    ImGui::Text("FrontX %f", camera.front.x);
    ImGui::Text("FrontY %f", camera.front.y);
    ImGui::Text("FrontZ %f", camera.front.z);

    ImGui::Text("RightX %f", camera.right.x);
    ImGui::Text("RightY %f", camera.right.y);
    ImGui::Text("RightZ %f", camera.right.z);

    ImGui::Text("UpX %f", camera.up.x);
    ImGui::Text("UpY %f", camera.up.y);
    ImGui::Text("UpZ %f", camera.up.z);

    ImGui::End();
    //Draw crosshair
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 center(window.WIDHT * 0.5f, window.HEIGHT * 0.5f);
    float size = 8.0f;

    drawList->AddImage((ImTextureID)(intptr_t)crosshair,
        ImVec2(center.x - size, center.y - size + 8),
        ImVec2(center.x + size, center.y + size + 8));


}



void perFrameLogic() {

    //per-frame - iterative logic
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;



    // FPS Calculations
     fps = 1.0f / deltaTime;
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

}



//Check game_helper.h (function de-clutter,( callbacks, setups))
void Game::run() {
    loadShaders();
    
    unsigned int atlas, crosshair;
    modelLoc = glGetUniformLocation(Shaders[shaderType::TEXTURE].ID, "model");
    initBlockUVs();

    glfwSetWindowUserPointer(window.getNativeWindow(), this);
    glfwMakeContextCurrent(window.getNativeWindow());
    glfwSetFramebufferSizeCallback(window.getNativeWindow(), framebuffer_size_callback);
    glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
    glfwSetMouseButtonCallback(window.getNativeWindow(), mouse_button_callback);
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);
    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(VSYNC);
    setupImgui(*this);

    

    glActiveTexture(GL_TEXTURE0);
    loadTexture(&atlas, "assets/atlas.png");
 
    Shaders[shaderType::TEXTURE].setInt("atlas", 0);

    glActiveTexture(GL_TEXTURE1);
    loadTexture(&crosshair, "assets/crosshair.png");

    World mundoTeste;
    this->currentWorld = &mundoTeste; 

    fpsStartTime = std::chrono::steady_clock::now();
    

    //Game Loop
    while (!window.shouldClose()) {
        frameSetups();
        perFrameLogic();
        processInput(*this, deltaTime);

        Shaders[shaderType::TEXTURE].use();
        updateCameraMatrices(window, Shaders[shaderType::TEXTURE]);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        mundoTeste.update(camera, deltaTime, modelLoc);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


        
        camera.update(mundoTeste, window);



   

        drawGui(mundoTeste,window,crosshair, begin, end);
        
        
        endFrame();
    }

    cleanup();


}

