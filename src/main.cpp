#include <iostream>

#include "window.h"
#include "shader.h"
#include "core.h"
#include "text.h"
#include "quad.h"
#include "rendertarget.h"

#define W 800
#define H 600
#define WINDOW_TITLE "psim"

int main(int argc, char** argv) {
	Window::init(W, H, WINDOW_TITLE);
	RenderTarget::init();
	Quad::init();
	Text::init();
	Core::init(W, H);

	Window::setKeyCallback([&] (int key, int action, int mods) {
		if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS or action == GLFW_REPEAT)) {
			Core::update(1.0 / 120.0);
		}
	});

	Window::loop([&] (int width, int height, float dt) {
		Core::resize(width, height);
		Core::update(dt);
		Core::render();
	});
 
	Core::destroy();
	Text::destroy();
	Quad::destroy();
	RenderTarget::destroy();
	Window::destroy();

	exit(EXIT_SUCCESS);
}