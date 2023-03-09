#include <iostream>

#include "window.h"
#include "core.h"

#define W 800
#define H 600
#define WINDOW_TITLE "psim"

int main(int argc, char** argv) {
	Window::init(W, H, WINDOW_TITLE);
	Core::init(W, H);

	Window::setKeyCallback([&] (int key, int action, int mods) {
		Core::keyEvent->fire(key, action, mods);
	});

	Window::loop([&] (int width, int height, float dt) {
		Core::resize(width, height);
		Core::update(dt);
		Core::render();
	});
 
	Core::destroy();
	Window::destroy();

	exit(EXIT_SUCCESS);
}