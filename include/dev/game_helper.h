#pragma once

//Variable Declaration--------------------------------------------
Camera camera(glm::vec3(0.0f, 70.0f, 0.0f));
std::chrono::steady_clock::time_point fpsStartTime;
float  fps, fpsCount, avgFps, lowestFps, highestFps = 0;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
unsigned int modelLoc;
Shader* defaultShader;
//---------------------------------------------------------------


//Setups--------------------------------------------------------
void setupImgui(Game& game) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(game.getWindow().getNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}
void frameSetups() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

}
void updateCameraMatrices(Window & window,Shader & shader) {
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.01f, 5000.0f);
    shader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

};
void loadTexture(unsigned int* texture, const std::string& path) {

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    // set the texture wrapping parameters

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
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

//--------------------------------------------------------------------

//Input Processing--------------------------------------------------

void processInput(Game& game, float deltaTime) {
    GLFWwindow* window = game.getWindow().getNativeWindow();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!camera.escDown) {
            camera.menu = !camera.menu;
            camera.firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, camera.menu ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
        camera.escDown = true;
    }
    else {
        camera.escDown = false;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!camera.lDown) {
            game.wireframe = !game.wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, game.wireframe ? GL_FILL : GL_LINE);
        }
        camera.lDown = true;
    }
    else {
        camera.lDown = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::DOWN, deltaTime);
}

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
            world.removeBlock(camera.raycastInfo.value());
        }
    }
        
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        if (camera.raycastInfo.has_value() && camera.selectedBlock != nullptr) {
           
            world.placeBlock(camera, camera.raycastInfo.value(), *camera.selectedBlock);
        }
}
//-----------------------------------------------------------------------------------




