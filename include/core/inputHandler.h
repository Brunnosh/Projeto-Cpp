#pragma once


class Window;


struct InputHandler{
	Window* window;
	float* deltaTime;

	InputHandler(Window* _window, float* _deltaTime) : window(_window), deltaTime(_deltaTime) {};

	void pollButtons(float deltaTime) {

	}

};