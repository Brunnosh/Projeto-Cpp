
#include <application.h>
#include <inputHandler.h>

#include <iostream>

bool Application::initialize() {
	if (!window.init("VoxelGame")) { return false; }

	/*
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetWindowFocusCallback(window, windowFocusCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetCharCallback(window, characterCallback);
	glfwSetScrollCallback(window, scrollCallback);
	*/
}

void Application::run() {
	while (!window.shouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glClearColor(0.5, 0.5, 0.5, 0.5);


		window.pollEvents();
		window.swapBuffers();

	}

}
void Application::shutdown() {
	window.terminate();
}

