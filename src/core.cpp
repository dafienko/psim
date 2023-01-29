#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <format>

#include "core.h"
#include "rendertarget.h"
#include "shader.h"

unsigned int Core::screenWidth, Core::screenHeight;

GLuint position_buffer, color_buffer;
GLuint vao;
GLint mvp_location, vpos_location, vcol_location;
std::unique_ptr<ShaderProgram> shaderProgram;
std::unique_ptr<RenderTarget> offScreenTarget;

glm::mat4x4 p, mvp;
glm::mat4x4 m = glm::identity<glm::mat4x4>();

static glm::vec2 positions[3] = {
	glm::vec2(-0.6f, -0.4f),
	glm::vec2(0.6f, -0.4f),
	glm::vec2(0.0f, 0.6f)
};

static glm::vec3 colors[3] = {
	glm::vec3(1.f, 0.f, 0.f),
	glm::vec3(0.f, 1.f, 0.f),
	glm::vec3(0.f, 0.f, 1.f)
};

void Core::init(unsigned int width, unsigned int height) {
	screenWidth = width;
	screenHeight = height;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	RenderTarget::init();
	shaderProgram = std::make_unique<ShaderProgram>("shaders/triangle.vsh", "shaders/triangle.fs");
	offScreenTarget = std::make_unique<RenderTarget>(width, height);

	GLuint program = shaderProgram->getProgram();
	mvp_location = glGetUniformLocation(program, "MVP");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &position_buffer); 
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions[0].x, GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(positions[0]), (void*) 0); 

	glGenBuffers(1, &color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), &colors[0].r, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(colors[0]), (void*) 0);

	m = glm::identity<glm::mat4x4>();
}

void Core::resize(unsigned int width, unsigned int height) {
	if (width == screenWidth && height == screenHeight) {
		return;
	}

	screenWidth = width;
	screenHeight = height;

	offScreenTarget = std::make_unique<RenderTarget>(width, height);
}

void Core::update(float dt) {
	float ratio = screenWidth / (float) screenHeight;
	p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

	m = glm::rotate(m, dt, glm::vec3(0, 0, 1));
	mvp = p * m;
}

void Core::render() {
	offScreenTarget->bind();
	offScreenTarget->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	shaderProgram->bind();
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3); 
	
	RenderTarget::bindDefault();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	offScreenTarget->renderToQuad();
}

void Core::destroy() {
	shaderProgram.release();
	RenderTarget::destroy();
}

void checkGLErrors(const std::string &file, int line) {
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << file << " at line "  << line << " ";
		std::cerr << err << std::endl;
		exit(EXIT_FAILURE);
	}
}