#pragma once
#include <window.h>
#include <settings.h>
#include <inputHandler.h>




class Application {
private:
	Window window;
	Settings appSettings;
	InputHandler inputHandler;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

public:
	Application() : inputHandler(&window,&deltaTime) {};


	bool Application::initialize();
	void Application::run();
	void Application::shutdown();

	//remover
	void Application::processInput(float deltaTime);
};