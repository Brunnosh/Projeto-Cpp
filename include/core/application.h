#pragma once
#include <window.h>


class Application {
private:
	Window window;

	bool initWindow() {
		return window.init("VoxelGame");
	}
public:
	bool initialize();
	void run();
	void shutdown() {
		window.terminate();
	}
};