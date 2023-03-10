#include <iostream>

#include "window.h"

GLFWwindow* window;
std::function<void(int, int, int)> keyCallback = [] (int a, int b, int c) {};
std::function<void(double, double)> mousePosCallback = [] (double a, double b) {};
std::function<void(int, int, int)> mouseButtonCallback = [] (int a, int b, int c) {};

static void error_callback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
	exit(EXIT_FAILURE);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	keyCallback(key, action, mods);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mousePosCallback(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	mouseButtonCallback(button, action, mods);
}

void Window::init(int width, int height, const char* title) {
	glfwSetErrorCallback(error_callback);
 
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
 
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
 
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

glm::vec2 Window::getMousePosition() {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return glm::vec2((float)x, (float)y);
}

void Window::loop(std::function<void(int, int, float)> loopCallback) {
	float last = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float t = (float)glfwGetTime();
		float dt = t - last;
		last = t;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		loopCallback(width, height, dt);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Window::destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
