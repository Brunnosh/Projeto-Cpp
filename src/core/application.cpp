
#include <application.h>
#include <inputHandler.h>
#include <camera.h>
#include <shader.h>

#include <iostream>



Camera camera(glm::vec3(0));

bool Application::initialize() {


	if (!window.init("VoxelGame")) { return false; }

	
	
    //glfwSetKeyCallback(window.getNativeWindow(), inputHandler.keyCallback);
    /*
    glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetWindowFocusCallback(window, windowFocusCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetCharCallback(window, characterCallback);
	glfwSetScrollCallback(window, scrollCallback);
	*/

	Shaders[shaderType::MAIN] = Shader("resources/shaders/mainVertexShader.glsl", "resources/shaders/mainFragmentShader.glsl");

	GLuint modelLoc = glGetUniformLocation(Shaders[shaderType::MAIN].ID, "model");

	

}

void Application::run() {
	while (!window.shouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5, 0.5, 0.5, 0.5);
	    
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		//--------------------update main shader matrices----------------------//
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window.WIDHT / (float)window.HEIGHT, 0.01f, 5000.0f);
		Shaders[shaderType::MAIN].setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		Shaders[shaderType::MAIN].setMat4("view", view);
		//--------------------update main shader matrices----------------------//



        processInput(deltaTime);


		window.pollEvents();
		window.swapBuffers();

	}

}
void Application::shutdown() {
	window.terminate();
}

void Application::processInput( float deltaTime) {
    

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!camera.escDown) {
            camera.menu = !camera.menu;
            camera.firstMouse = true;
            glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, camera.menu ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
        camera.escDown = true;
    }
    else {
        camera.escDown = false;
    }

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_L) == GLFW_PRESS) {
        if (!camera.lDown) {
            appSettings.wireframe = appSettings.wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_FILL : GL_LINE);
        }
        camera.lDown = true;
    }
    else {
        camera.lDown = false;
    }

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_P) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.getNativeWindow(), true);

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::DOWN, deltaTime);
}