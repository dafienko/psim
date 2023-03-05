#include <iostream>

#include "window.h"
#include "shader.h"
#include "core.h"
#include "text.h"

#define W 800
#define H 600
#define WINDOW_TITLE "psim"

int main(int argc, char** argv) {
	Window::init(W, H, WINDOW_TITLE);
	Core::init(W, H);
	Text::init();

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
 
	Text::destroy();
	Core::destroy();
	Window::destroy();

	exit(EXIT_SUCCESS);
}