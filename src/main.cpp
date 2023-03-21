#include <iostream>

#include "window.h"
#include "core.h"

#define W 1000
#define H 800 
#define SIM_WIDTH W / 5
#define SIM_HEIGHT H / 5
#define WINDOW_TITLE "psim" 

int main(int argc, char** argv) {
	Window::init(W, H, WINDOW_TITLE);
	Core::init(glm::ivec2(SIM_WIDTH, SIM_HEIGHT));

	Window::setKeyCallback([&] (int key, int action, int mods) {
		Core::keyEvent->fire(key, action, mods);
	});

	Window::setMousePosCallback([&] (double x, double y) {
		Core::mouseMoveEvent->fire(x, y);
	});

	Window::setMouseButtonCallback([&] (int key, int action, int mods) {
		Core::mouseButtonEvent->fire(key, action, mods);
	});

	Window::setWindowResizeCallback([&] (int width, int height) {
		Core::resize(Window::getWindowFramebufferSize());
		Core::render();
	});

	Window::loop([] (float dt) {
		if (!Core::paused) {
			Core::update(dt);
		}
		Core::render();
	});
 
	Core::destroy();
	Window::destroy();

	exit(EXIT_SUCCESS); 
}