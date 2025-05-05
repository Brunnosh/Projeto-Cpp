
#include <application.h>
#include <iostream>

bool Application::initialize() {
	return initWindow();
}

void Application::run() {
	while (!window.shouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClearColor(0.5, 0.5, 0.5, 0.5);

		window.pollEvents();

		window.swapBuffers();

	}

}

int main()
{
	
	Application voxelGame;

	if (!voxelGame.initialize()) {
		return -1;
	}
	voxelGame.run();
	voxelGame.shutdown();

	return 0;
}