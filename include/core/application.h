#pragma once
#include <window.h>


class Application {
private:
	Window window;
	//struct settings


public:
	bool Application::initialize();
	void Application::run();
	void Application::shutdown();
};