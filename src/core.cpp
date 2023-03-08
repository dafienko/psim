#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>
#include <string>
#include <iostream>

#include "core.h"
#include "rendertarget.h"
#include "shader.h"
#include "text.h"
#include "quad.h"
#include "fluidTexture.h"
#include "ui.h"
#include "uiframe.h"

unsigned int Core::screenWidth, Core::screenHeight;

std::unique_ptr<FluidTexture> fluid;
std::unique_ptr<UI> ui;

void Core::init(unsigned int width, unsigned int height) {
	screenWidth = width;
	screenHeight = height;

	RenderTarget::init();
	Quad::init();
	Text::init();
	UI::init();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	fluid = std::make_unique<FluidTexture>(glm::ivec2(width / 5, height / 5));

	ui = std::unique_ptr<UI>(dynamic_cast<UI*>((Instance::fromJSON("ui/main.json"))));

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
	fluid->update(dt);

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

	fluid->render();

	std::string frameTimeStr =  std::to_string(averageFrameTime * 1000.0f) + " ms (" + std::to_string(floatFPS) + " fps)";
	Text::renderText(
		frameTimeStr, 
		glm::ivec2(20, Core::screenHeight - 15), 
		FontFace::Consola, 28, 
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	ui->render();
}

void Core::destroy() {
	fluid.release();
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