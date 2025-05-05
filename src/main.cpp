#include <window.h>
#include <iostream>




int main()
{
	Window window;

	window.init("teste");

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	while (!window.shouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClearColor(0.5, 0.5, 0.5, 0.5);

		window.pollEvents();

		window.swapBuffers();

	}

	window.terminate();

	return 0;
}