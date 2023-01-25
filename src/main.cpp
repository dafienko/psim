#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shader.h"
#include "core.h"
 
static void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
	exit(EXIT_FAILURE);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

GLFWwindow* createWindow(const char* windowTitle) {
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
 
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, windowTitle, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
 
	glfwSetKeyCallback(window, key_callback);
 
	glfwMakeContextCurrent(window);
	glewInit();
	glfwSwapInterval(1);

	return window;
}

void destroyWindow(GLFWwindow* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main(int argc, char** argv)
{
	GLFWwindow* window = createWindow(*argv);

	Core::init(640, 480);
 
	float last = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float t = (float)glfwGetTime();
		float dt = t - last;
		last = t;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
 
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		Core::resize(width, height);
		Core::update(dt);
		Core::render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
 
	Core::destroy();
	destroyWindow(window);

	exit(EXIT_SUCCESS);
}