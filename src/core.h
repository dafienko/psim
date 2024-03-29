#pragma once

#include <memory>
#include <string>
#include <glm/ext.hpp>

#include "event.h"
#include "ui.h"

namespace Core {	
	extern std::unique_ptr<Event<int, int, int>> keyEvent;
	extern std::unique_ptr<Event<double, double>> mouseMoveEvent;
	extern std::unique_ptr<Event<int, int, int>> mouseButtonEvent;
	extern std::unique_ptr<UI> mainUI;
	extern bool paused;

	void init(glm::ivec2 simSize);
	void update(float dt);
	void render();
	void resize(glm::ivec2 windowSize);

	glm::ivec2 getWindowSize();
	glm::vec2 getMousePosition();

	bool isMouse1Down();
	bool isMouse2Down();

	void destroy();
};

void checkGLErrors(const std::string &file, int line);

#define CHECK_GL_ERRORS checkGLErrors(std::string(__FILE__), __LINE__);
