#include "quad.h"

#include <GL/glew.h>
#include <iostream>

#include "core.h"

GLuint quadVAO, positionsVBO, relPositionsVBO;

std::unique_ptr<ShaderProgram> Quad::quadShaderProgram;

void Quad::init() {
	quadShaderProgram = std::make_unique<ShaderProgram>("shaders/quad.vsh", "shaders/quad.fs");

	const float relPositions[4][2] = {
		{0, 0},
		{1, 0},
		{0, 1},
		{1, 1},
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	glGenBuffers(1, &relPositionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, relPositionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, relPositions, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Quad::render(glm::vec2 tl, glm::vec2 br) {
	float positions[4][2] = {
		{tl.x, tl.y},
		{br.x, tl.y},
		{tl.x, br.y},
		{br.x, br.y},
	};

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, positions, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Quad::render() {
	Quad::render(
		glm::vec2(-1, -1),
		glm::vec2(1, 1)
	);
}

glm::vec2 Quad::getAspectSize(float aspect) {
	glm::vec2 windowSize = Core::getWindowSize();
	float x = std::min(windowSize.x, windowSize.y * aspect);
	
	return glm::vec2(
		x,
		x / aspect
	);
}

glm::vec2 Quad::getAspectTL(float aspect) {
	glm::vec2 windowSize = Core::getWindowSize();
	glm::vec2 aspectSize = getAspectSize(aspect);

	return (windowSize - aspectSize) / 2.0f;
}

void Quad::renderAspect(float aspect) {
	glm::vec2 windowSize = Core::getWindowSize();
	glm::vec2 test = glm::vec2(1) / glm::vec2(2, 3);
	glm::vec2 tl = glm::vec2(-1, -1) + (getAspectTL(aspect) / windowSize) * 2.0f;
	glm::vec2 br = -tl;
	
	Quad::render(tl, br);
}

void Quad::destroy() {
	quadShaderProgram.release();
	
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &positionsVBO);
}