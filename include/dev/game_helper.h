#pragma once



void setupImgui(Game &game) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(game.getWindow().getNativeWindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init("#version 330");
}

void processInput(Game& game, float deltaTime) {
    {
        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (game.escDown) return;

            game.escDown = true;
            game.menu = !game.menu;
            game.firstMouse = true;
            glfwSetInputMode(game.getWindow().getNativeWindow(), GLFW_CURSOR, game.menu ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);


        }
        else {
            game.escDown = false;
        }
        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_L) == GLFW_PRESS) {
            if (game.lDown) return;

            game.lDown = true;
            game.wireframe = !game.wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, game.wireframe ? GL_FILL : GL_LINE);

        }
        else {
            game.lDown = false;
        }


        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_P) == GLFW_PRESS)
            glfwSetWindowShouldClose(game.getWindow().getNativeWindow(), true);

        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_W) == GLFW_PRESS)
            game.player.camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_S) == GLFW_PRESS)
            game.player.camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_A) == GLFW_PRESS)
            game.player.camera.processKeyboard(CameraMovement::LEFT, deltaTime);
        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_D) == GLFW_PRESS)
            game.player.camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
            game.player.camera.processKeyboard(CameraMovement::UP, deltaTime);
        if (glfwGetKey(game.getWindow().getNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            game.player.camera.processKeyboard(CameraMovement::DOWN, deltaTime);
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


void Game::loadTexture(unsigned int* texture, const std::string& path) {

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


