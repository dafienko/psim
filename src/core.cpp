#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <memory>
#include <string>
#include <iostream>

#include "simulation.h"
#include "core.h"
#include "quad.h"
#include "text.h"
#include "rendertarget.h"
#include "fluidsimulator.h"
#include "ui.h"
#include "uiframe.h"
#include "uitext.h"

unsigned int Core::screenWidth, Core::screenHeight;

std::unique_ptr<Event<int, int, int>> Core::keyEvent;

std::unique_ptr<Simulation> simulation;
std::unique_ptr<UI> ui;

void Core::init(unsigned int width, unsigned int height) {
	screenWidth = width;
	screenHeight = height;

	keyEvent = std::make_unique<Event<int, int, int>>();

	RenderTarget::init();
	Quad::init();
	Text::init();
	UI::init();

	simulation = std::make_unique<Simulation>(glm::ivec2(width / 5, height / 5));

	ui = std::unique_ptr<UI>(dynamic_cast<UI*>((Instance::fromJSON("ui/main.json"))));
	ui->rendered = false;

	keyEvent->bind([&] (int key, int action, int mods) {
		if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS or action == GLFW_REPEAT)) {
			Core::update(1.0 / 120.0);
		} 
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_M) {
				ui->rendered = !ui->rendered;
			}	
		}
	});
}

void Core::resize(unsigned int width, unsigned int height) {
	if (width == screenWidth && height == screenHeight) {
		return;
	}

	screenWidth = width;
	screenHeight = height;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	simulation->render();

	UIText* fpsLabel = dynamic_cast<UIText*>(ui->findChild("main")->findChild("fps"));
	fpsLabel->text = std::to_string(averageFrameTime * 1000.0f) + " ms (" + std::to_string(floatFPS) + " fps)";

	ui->render();
}

void Core::destroy() {
	simulation.release();
	ui.release();

	UI::destroy();
	Text::destroy();
	Quad::destroy();
	RenderTarget::destroy();
}

void checkGLErrors(const std::string &file, int line) {
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << file << " at line "  << line << " ";
		std::cerr << err << std::endl;
		exit(EXIT_FAILURE);
	}
}