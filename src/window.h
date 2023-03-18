#pragma once

#include <functional>
#include <GL/glew.h> // glew MUST be included before glfw
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

namespace Window {
	void init(int width, int height, const char* title);
	void loop(std::function<void(float)> loopCallback);

	void setKeyCallback(std::function<void(int, int, int)> keyCallback);
	void setMousePosCallback(std::function<void(double, double)> mousePosCallback);
	void setMouseButtonCallback(std::function<void(int, int, int)> mouseButtonCallback);
	void setWindowResizeCallback(std::function<void(int, int)> windowResizeCallback);

	glm::ivec2 getWindowFramebufferSize();
	glm::ivec2 getWindowScreenCoordinateSize();
	glm::vec2 getMousePosition();
	glm::vec2 getContentScale();

	void destroy();
}