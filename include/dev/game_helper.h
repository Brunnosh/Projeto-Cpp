#pragma once

//Callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    game->getWindow().WIDHT = width;
    game->getWindow().HEIGHT = height;
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (camera.menu) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (camera.firstMouse) {
        camera.camLastX = xpos;
        camera.camLastY = ypos;
        camera.firstMouse = false;
    }

    float xoffset = xpos - camera.camLastX;
    float yoffset = camera.camLastY - ypos;

    camera.camLastX = xpos;
    camera.camLastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    World &world = *game->currentWorld;

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action);

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.selectNextBlock();

        if (camera.raycastInfo.has_value()) {
            camera.selectBlock();
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (camera.raycastInfo.has_value()) {
            
            world.removeBlock(camera.raycastInfo.value(), game->getRenderer());
        }
    }
        
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        if (camera.raycastInfo.has_value() && camera.selectedBlock != nullptr) {
           
            world.placeBlock(camera, camera.raycastInfo.value(), *camera.selectedBlock, game->getRenderer());
        }
}
//-----------------------------------------------------------------------------------
//Misc


void frameSetups() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

}
void updateCameraMatrices(Window& window, Shader& shader) {
    //skip projection matrix if fov/window size is same from last frame
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.01f, 5000.0f);
    shader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

};



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


void drawGui(World& mundoTeste, Window& window, unsigned int crosshair, std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end, float sunAngle) {
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

