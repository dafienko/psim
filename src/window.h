#pragma once

#include <functional>
#include <GL/glew.h> // glew MUST be included before glfw
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

namespace Window {
	/**
	 * @brief Create the main process window
	 * 
	 * @param width the width of the main window
	 * @param height the height of the main window
	 * @param title the title of the main window
	 */
	void init(int width, int height, const char* title);

	/**
	 * @brief set the function to be run in the main window loop
	 * 
	 * @param loopCallback the callback to be run
	 */
	void loop(std::function<void(float)> loopCallback);

	/**
	 * @brief Set the callback that runs for keyboard events
	 * 
	 * @param keyCallback the callback to be run
	 */
	void setKeyCallback(std::function<void(int, int, int)> keyCallback);

	/**
	 * @brief Set the callback that runs for mouse movement events
	 * 
	 * @param mousePosCallback the callback to be run
	 */
	void setMousePosCallback(std::function<void(double, double)> mousePosCallback);

	/**
	 * @brief Set the callback that runs for mouse button events
	 * 
	 * @param mouseButtonCallback the callback to be run
	 */
	void setMouseButtonCallback(std::function<void(int, int, int)> mouseButtonCallback);

	/**
	 * @brief Set the callback for window resize events
	 * 
	 * @param windowResizeCallback the callback to be run
	 */
	void setWindowResizeCallback(std::function<void(int, int)> windowResizeCallback);

	/**
	 * @brief Get the window framebuffer size in pixels
	 * 
	 * @return glm::ivec2 
	 */
	glm::ivec2 getWindowFramebufferSize();

	/**
	 * @brief Get the window framebuffer size in screen coordinates
	 * 
	 * @return glm::ivec2 
	 */
	glm::ivec2 getWindowScreenCoordinateSize();

	/**
	 * @brief Get the Mouse Position object
	 * 
	 * @return glm::vec2 
	 */
	glm::vec2 getMousePosition();

	/**
	 * @brief Get the screen content scale factor
	 * 
	 * @return glm::vec2 
	 */
	glm::vec2 getContentScale();

	void destroy();
}