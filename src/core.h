#pragma once

#include <glm/ext.hpp>
#include <memory>
#include <string>

#include "event.h"

namespace Core {	
	extern std::unique_ptr<Event<int, int, int>> keyEvent;
	extern std::unique_ptr<Event<double, double>> mouseMoveEvent;
	extern std::unique_ptr<Event<int, int, int>> mouseButtonEvent;

	void init(unsigned int width, unsigned int height);
	void update(float dt);
	void render();
	void resize(unsigned int width, unsigned int height);

	glm::ivec2 getWindowSize();
	glm::vec2 getMousePosition();

	bool isMouse1Down();
	bool isMouse2Down();

	void destroy();
};

void checkGLErrors(const std::string &file, int line);

#define CHECK_GL_ERRORS checkGLErrors(std::string(__FILE__), __LINE__);
