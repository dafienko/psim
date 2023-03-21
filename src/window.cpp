#include <iostream>

#include "window.h"

GLFWwindow* window;
std::function<void(int, int, int)> keyCallback = [] (int a, int b, int c) {};
std::function<void(double, double)> mousePosCallback = [] (double a, double b) {};
std::function<void(int, int, int)> mouseButtonCallback = [] (int a, int b, int c) {};
std::function<void(int, int)> windowResizedCallback = [] (int a, int b) {};

glm::vec2 contentScale(1.0f);

void Window::init(int width, int height, const char* title) {
	glfwSetErrorCallback([] (int error, const char* description) {
		std::cerr << "Error: " << description << std::endl;
		exit(EXIT_FAILURE);
	});
 
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		keyCallback(key, action, mods);
	});
	
	glfwSetCursorPosCallback(window, [] (GLFWwindow* window, double xPos, double yPos) {
		glm::ivec2 windowSize = getWindowScreenCoordinateSize();
		glm::ivec2 frameBufferSize = getWindowFramebufferSize();
		xPos = (xPos / windowSize.x) * frameBufferSize.x;
		yPos = (yPos / windowSize.x) * frameBufferSize.x;
		mousePosCallback(xPos, yPos);
	});

	glfwSetMouseButtonCallback(window, [] (GLFWwindow* window, int button, int action, int mods) {
		mouseButtonCallback(button, action, mods);
	});

	glfwSetWindowSizeCallback(window, [] (GLFWwindow* window, int width, int height) {
		windowResizedCallback(width, height);
		glfwSwapBuffers(window);
	});
	
	glfwGetWindowContentScale(window, &contentScale.x, &contentScale.y);
	glfwSetWindowContentScaleCallback(window, [] (GLFWwindow* window, float xscale, float yscale) {
		contentScale = glm::vec2(xscale, yscale);
	});

	glfwMakeContextCurrent(window);
	glewInit();
	glfwSwapInterval(1);
}

void Window::setKeyCallback(std::function<void(int, int, int)> callback) {
	keyCallback = callback;
}

void Window::setMousePosCallback(std::function<void(double, double)> callback) {
	mousePosCallback = callback;
}

void Window::setMouseButtonCallback(std::function<void(int, int, int)> callback) {
	mouseButtonCallback = callback;
}

void Window::setWindowResizeCallback(std::function<void(int, int)> callback) {
	windowResizedCallback = callback;
}

glm::vec2 Window::getMousePosition() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	
	glm::vec2 pos((float)x, (float)y);
	pos /= Window::getWindowScreenCoordinateSize();
	pos *= Window::getWindowFramebufferSize();
	return pos;
}

glm::ivec2 Window::getWindowFramebufferSize() {
	glm::ivec2 size;
	glfwGetFramebufferSize(window, &size.x, &size.y);
	return size;
}

glm::ivec2 Window::getWindowScreenCoordinateSize() {
	glm::ivec2 size;
	glfwGetWindowSize(window, &size.x, &size.y);
	return size;
}

void Window::loop(std::function<void(float)> loopCallback) {
	float last = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float t = (float)glfwGetTime();
		float dt = t - last;
		last = t;

		loopCallback(dt);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

glm::vec2 Window::getContentScale() {
	return contentScale;
}

void Window::destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
