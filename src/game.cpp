#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game.h>
#include <shader.h>
#include <iostream>
#include <chrono>
#include <game_helper.h>


unsigned int atlas, crosshair;

void loadShaders() {
    Shaders[shaderType::MAIN] = Shader("assets/shaders/texture/mainShaderVertex.glsl", "assets/shaders/texture/mainShaderFragment.glsl");
    Shaders[shaderType::OUTLINE] = Shader("assets/shaders/outline/outlineVertex.glsl", "assets/shaders/outline/outlineFragment.glsl");
    Shaders[shaderType::SKYBOX] = Shader("assets/shaders/skybox/skyboxVertex.glsl", "assets/shaders/skybox/skyboxFragment.glsl");
    
}

void calcDrawCalls() {
    
    // Verifica o tempo desde o último cálculo
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = currentTime - startTime;
    drawcallsec = drawcallsec + drawCallCount;

    // Se passou o intervalo de tempo definido (1 segundo, por exemplo)
    if (elapsed.count() >= interval) {
        // Exibe o número de draw calls por segundo
        

        // Reseta o contador e o tempo
        drawcallsec = 0;
        startTime = currentTime;
    }
}

void drawGui(World & mundoTeste, Window & window, unsigned int crosshair, std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end, float sunAngle) {
    glm::ivec3 playerChunkPos = glm::ivec3(glm::floor(camera.position / float(CHUNKSIZE)));
    //Debug panel
    ImGui::Begin("WorldDebug");
    ImGui::Text("FPS: %f (Avg: %f, Min: %f, Max: %f)", fps, avgFps, lowestFps, highestFps);
    ImGui::Text("FrameTime (ms): %f", deltaTime * 100.0f);
    ImGui::Text("X %f", camera.position.x);
    ImGui::Text("Y %f", camera.position.y);
    ImGui::Text("Z %f", camera.position.z);
    ImGui::Text("Chunks loaded: %d", mundoTeste.getNumberChunks());
    ImGui::Text("DrawCalls (sec): %d", drawcallsec);
    ImGui::Text("DrawCalls (frame): %d", drawCallCount);
    ImGui::Text("World update time (ms): %f", (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    ImGui::Text("Sun Angle: %f", sunAngle);
    ImGui::Text("Highest Y chunk for (X:%d ,Z:%d) -> %d", playerChunkPos.x, playerChunkPos.z, mundoTeste.getMaxChunkY(playerChunkPos.x, playerChunkPos.z));
    if (camera.selectedBlock != nullptr) ImGui::Text("Selected Block: %s", (*camera.selectedBlock).getTypeToString());
    ImGui::End();


    ImGui::Begin("LookinAt");
    if (camera.raycastInfo.has_value()) {
        
        auto& hit = camera.raycastInfo.value();
        ImGui::Text("BlockSkyLight %d", hit.copiedBlock->getSkyLight());
        ImGui::Text("------------------");
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
    glClearColor(0.5, 0.5, 0.5, 0.5);
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

bool Game::setup() {
    loadShaders();

    
    modelLoc = glGetUniformLocation(Shaders[shaderType::MAIN].ID, "model");
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

    Shaders[shaderType::MAIN].setInt("atlas", 0);

    glActiveTexture(GL_TEXTURE1);
    loadTexture(&crosshair, "assets/crosshair.png");

    World mundoTeste;
    currentWorld = std::make_unique<World>();

    fpsStartTime = std::chrono::steady_clock::now();

    worldRenderer.worldReference = currentWorld.get();

    

    return true;
}

//Check game_helper.h (function de-clutter,( callbacks, setups))
void Game::loop() {
    while (!window.shouldClose()) {
        
        drawCallCount = 0;

        frameSetups();

        perFrameLogic();
        
        processInput(*this, deltaTime);

        camera.update(*currentWorld, window, drawCallCount);
        
        updateCameraMatrices(window, Shaders[shaderType::MAIN]);




        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        currentWorld->queueChunks(camera);

        currentWorld->genChunks(worldRenderer);

        currentWorld->tick();
        
        currentWorld->update(camera, deltaTime);

        
        
        currentWorld->sunAngle += currentWorld->sunSpeed * deltaTime;
        if (currentWorld->sunAngle >= 360.0f)
            currentWorld->sunAngle -= 360.0f;

        float sunRadians = glm::radians(currentWorld->sunAngle + 180.0f);
        glm::vec3 sunDirection = glm::normalize(glm::vec3(cos(sunRadians), sin(sunRadians), 0.0f));
        float sunHeight = sunDirection.y;  // Pega a componente Y da direção do sol

        

        glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "sunHeight"), sunHeight);
        glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "ambientStrength"), 15);
        glUniform3fv(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "lightDir"), 1, &sunDirection[0]);
        glUniform3fv(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "viewPos"), 1, &camera.position[0]);

        glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "specularStrength"), 0.05f);
        glUniform1f(glGetUniformLocation(Shaders[shaderType::MAIN].ID, "shininess"), 8.0f);

        worldRenderer.processPendingChunks();

        worldRenderer.rebuildDirtyChunks(currentWorld->getWorldDataRef());

        Shaders[shaderType::MAIN].use();
        worldRenderer.renderChunks(modelLoc, drawCallCount);
        



        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        drawGui(*currentWorld,window,crosshair, begin, end, currentWorld->sunAngle);
        calcDrawCalls();
        endFrame();
    }
    worldRenderer.cleanup();
    cleanup();
    

}

