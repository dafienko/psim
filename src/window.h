#pragma once

#include <functional>

#include <GL/glew.h> // glew MUST be included before glfw
#include <GLFW/glfw3.h>

namespace Window {
	void init(int width, int height, const char* title);
	void loop(std::function<void(int, int, float)> loopCallback);
	void setKeyCallback(std::function<void(int, int, int)> keyCallback);
	void destroy();
}