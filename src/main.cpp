

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <dev/shader.h>
#include <camera.h>
#include <block.h>

#include <iostream>

#define VSYNC 1


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void initGLAD();
GLFWwindow *initGLFW();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    //GLFW initialization and window creation
    GLFWwindow *window;
    window = initGLFW();

    //load all OpenGL function pointers
    initGLAD();

    //global opengl state
    glEnable(GL_DEPTH_TEST);

    // build shader 
    Shader ourShader("shaders/vertexshader.glsl", "shaders/fragmentshader.glsl");

    //UV 0 (bottom-left) -> 1 (Top-right)

    //dirt bottom -> (0,1) até (0.0625, 0.9375)

    //dir side -> (0.0625, 1) até (0.125, 0.9375)

    //vertex test (all this verices are just one box)

    const Block& dirtBlock = blockList[static_cast<int>(BlockType::DIRT)];
    float uMin, uMax, vMin, vMax;
    uMin = dirtBlock.getUVXMin();
    uMax = dirtBlock.getUVXMax();
    vMin = dirtBlock.getUVYMin();
    vMax = dirtBlock.getUVYMax();


    float block[] = {
        // face trás
        -0.5f, -0.5f, -0.5f,  uMin, vMin,
         0.5f, -0.5f, -0.5f,  uMax, vMin,
         0.5f,  0.5f, -0.5f,  uMax, vMax,
         0.5f,  0.5f, -0.5f,  uMax, vMax,
        -0.5f,  0.5f, -0.5f,  uMin, vMax,
        -0.5f, -0.5f, -0.5f,  uMin, vMin,

        // face frente
        -0.5f, -0.5f,  0.5f,  uMin, vMin,
         0.5f, -0.5f,  0.5f,  uMax, vMin,
         0.5f,  0.5f,  0.5f,  uMax, vMax,
         0.5f,  0.5f,  0.5f,  uMax, vMax,
        -0.5f,  0.5f,  0.5f,  uMin, vMax,
        -0.5f, -0.5f,  0.5f,  uMin, vMin,

        // face esquerda
        -0.5f,  0.5f,  0.5f,  uMax, vMin,
        -0.5f,  0.5f, -0.5f,  uMax, vMax,
        -0.5f, -0.5f, -0.5f,  uMin, vMax,
        -0.5f, -0.5f, -0.5f,  uMin, vMax,
        -0.5f, -0.5f,  0.5f,  uMin, vMin,
        -0.5f,  0.5f,  0.5f,  uMax, vMin,

        // face direita
         0.5f,  0.5f,  0.5f,  uMax, vMin,
         0.5f,  0.5f, -0.5f,  uMax, vMax,
         0.5f, -0.5f, -0.5f,  uMin, vMax,
         0.5f, -0.5f, -0.5f,  uMin, vMax,
         0.5f, -0.5f,  0.5f,  uMin, vMin,
         0.5f,  0.5f,  0.5f,  uMax, vMin,

         // face baixo
         -0.5f, -0.5f, -0.5f,  uMin, vMax,
          0.5f, -0.5f, -0.5f,  uMax, vMax,
          0.5f, -0.5f,  0.5f,  uMax, vMin,
          0.5f, -0.5f,  0.5f,  uMax, vMin,
         -0.5f, -0.5f,  0.5f,  uMin, vMin,
         -0.5f, -0.5f, -0.5f,  uMin, vMax,

         // face cima
         -0.5f,  0.5f, -0.5f,  uMin, vMax,
          0.5f,  0.5f, -0.5f,  uMax, vMax,
          0.5f,  0.5f,  0.5f,  uMax, vMin,
          0.5f,  0.5f,  0.5f,  uMax, vMin,
         -0.5f,  0.5f,  0.5f,  uMin, vMin,
         -0.5f,  0.5f, -0.5f,  uMin, vMax
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(block), block, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // load and create a texture 
    unsigned int atlas;
    glGenTextures(1, &atlas);
    glBindTexture(GL_TEXTURE_2D, atlas);
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


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("atlas", 0);
    



    


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        // -----
       

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas);


        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);

            // calculate the model matrix for each object and pass it to shader before drawing
           glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
           model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
           ourShader.setMat4("model", model);

           glDrawArrays(GL_TRIANGLES, 0, 36);
        


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(0);
    }
}

GLFWwindow *initGLFW() {
    // glfw: initialize and configure
// ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(0);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(VSYNC);

    return window;
}