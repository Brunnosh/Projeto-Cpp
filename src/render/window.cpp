#include <window.h>

#include <iostream>

bool Window::init(const std::string& title) {
    
        
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

     //width = mode->width;
     //height = mode->height;

    window = glfwCreateWindow(WIDHT, HEIGHT, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}

bool Window::glInit() {
    glEnable(GL_DEPTH_TEST);
    defaultShader = new Shader("assets/shaders/vertexshader.glsl", "assets/shaders/fragmentshader.glsl");
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);




    return true;
}

void Window::useShader() {
    if (defaultShader) {
        defaultShader->use();
        
    }
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::terminate() {

    delete defaultShader;
    defaultShader = nullptr;

    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Window::getNativeWindow() {
    return window;
}

Shader Window::getShader() {
    return *defaultShader;
}


/*
* CODIGO PARA INICIAR JANELA FULLSCREEN BORDERLESS
* 
bool Window::init(const std::string& title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Pega o monitor e o modo de vídeo (resolução nativa)
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        std::cerr << "Failed to get primary monitor" << std::endl;
        glfwTerminate();
        return false;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        std::cerr << "Failed to get video mode" << std::endl;
        glfwTerminate();
        return false;
    }

    int screenWidth = mode->width;
    int screenHeight = mode->height;

    // Define a janela como "sem decoração" (sem bordas, sem título)
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    // Cria a janela com o tamanho da tela, mas sem ser "exclusive fullscreen"
    window = glfwCreateWindow(screenWidth, screenHeight, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Move a janela pro canto superior esquerdo da tela
    glfwSetWindowPos(window, 0, 0);

    // Continua com o restante da inicialização
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}
*/