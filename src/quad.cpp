#include "quad.h"

#include <GL/glew.h>

GLuint quadVAO, positionsVBO, relPositionsVBO;

void Quad::init() {
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

void Quad::destroy() {
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &positionsVBO);
}