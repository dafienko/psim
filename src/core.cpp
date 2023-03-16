#include <memory>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "window.h"
#include "simulation.h"
#include "core.h"
#include "quad.h"
#include "text.h"
#include "rendertarget.h"
#include "fluidsimulator.h"
#include "ui.h"
#include "uiframe.h"
#include "uitext.h"

glm::ivec2 windowSize;

std::unique_ptr<Event<int, int, int>> Core::keyEvent;
std::unique_ptr<Event<double, double>> Core::mouseMoveEvent;
std::unique_ptr<Event<int, int, int>> Core::mouseButtonEvent;

std::unique_ptr<Simulation> simulation;
std::unique_ptr<UI> ui;

bool mouse1Down = false;
bool mouse2Down = false;

void Core::init(int width, int height) {
	windowSize = glm::ivec2(width, height);

	keyEvent = std::make_unique<Event<int, int, int>>();
	mouseMoveEvent = std::make_unique<Event<double, double>>();
	mouseButtonEvent = std::make_unique<Event<int, int, int>>();

	Quad::init();
	Text::init();
	UI::init();

	simulation = std::make_unique<Simulation>(glm::ivec2(width / 10, height / 10));

	ui = std::unique_ptr<UI>(dynamic_cast<UI*>((Instance::fromJSON("ui/main.json"))));
	ui->rendered = false;

	keyEvent->bind([&] (int key, int action, int mods) {
		if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			Core::update(1.0 / 120.0);
		} 
		
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_M) {
				ui->rendered = !ui->rendered;
			}	
		}
	});

	mouseButtonEvent->bind([&] (int key, int action, int mods) {
		if (action == GLFW_PRESS) {
			switch(key) {
			case GLFW_MOUSE_BUTTON_1:
				mouse1Down = true;
				break;
			case GLFW_MOUSE_BUTTON_2:
				mouse2Down = true;
				break;
			}
		} else if (action == GLFW_RELEASE) {
			switch(key) {
			case GLFW_MOUSE_BUTTON_1:
				mouse1Down = false;
				break;
			case GLFW_MOUSE_BUTTON_2:
				mouse2Down = false;
				break;
			}
		}
	});
}

glm::vec2 Core::getMousePosition() {
	return Window::getMousePosition();
}

glm::ivec2 Core::getWindowSize() {
	return windowSize;
}

bool Core::isMouse1Down() {
	return mouse1Down;
}

bool Core::isMouse2Down() {
	return mouse2Down;
}

void Core::resize(int width, int height) {
	windowSize = glm::ivec2(width, height);
}

static float elapsedTime, floatFPS, averageFrameTime;
static int elapsedFrames;

void Core::update(float dt) {
	simulation->update(dt);

	elapsedTime += dt;
	elapsedFrames += 1;
	
	if (elapsedTime > .25f) {
		floatFPS = elapsedFrames / elapsedTime;
		averageFrameTime = elapsedTime / elapsedFrames;
		elapsedTime = 0.0f;
		elapsedFrames = 0;
	}
}

void Core::render() {
	RenderTarget::bindDefault();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	simulation->render();

	UIText* fpsLabel = dynamic_cast<UIText*>(ui->getChild("main")->getChild("fps"));
	fpsLabel->text = std::to_string(averageFrameTime * 1000.0f) + " ms (" + std::to_string(floatFPS) + " fps)";

	ui->render();
}

void Core::destroy() {
	keyEvent.release();
	mouseMoveEvent.release();

	simulation.release();
	ui.release();

	UI::destroy();
	Text::destroy();
	Quad::destroy();
}

void checkGLErrors(const std::string &file, int line) {
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << file << " at line "  << line << " " << std::endl;
		std::cerr << "error: " << err << std::endl;
		exit(EXIT_FAILURE);
	}
}