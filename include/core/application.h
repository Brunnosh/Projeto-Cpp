#pragma once
#include <window.h>


class Application {
private:
	Window window;
	//struct settings


public:
	bool wireframe;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	bool Application::initialize();
	void Application::run();
	void Application::shutdown();

	void Application::processInput(float deltaTime);
};