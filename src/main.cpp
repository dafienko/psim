#include <iostream>

#include "window.h"
#include "core.h"

#define W 1000
#define H 800 
#define WINDOW_TITLE "psim" 

int main(int argc, char** argv) {
	Window::init(W, H, WINDOW_TITLE);
	Core::init(Window::getWindowFramebufferSize().x, Window::getWindowFramebufferSize().y);

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
		Core::resize(Window::getWindowFramebufferSize().x, Window::getWindowFramebufferSize().y);
		Core::render();
	});

	Window::loop([&] (float dt) {
		Core::update(dt);
		Core::render();
	});
 
	Core::destroy();
	Window::destroy();

	exit(EXIT_SUCCESS); 
}