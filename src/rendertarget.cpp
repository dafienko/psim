#include <iostream>
#include <glm/ext.hpp>

#include "core.h"
#include "rendertarget.h"

static constexpr glm::vec2 QUAD_VERTICES[4] = {
	glm::vec2(-1.0f, 1.0f), // top left
	glm::vec2(-1.0f, -1.0f), // bottom left
	glm::vec2(1.0f, -1.0f), // bottom right
	glm::vec2(1.0f, 1.0f), // top right
};

static constexpr glm::vec2 TEX_COORDS[4] = {
	glm::vec2(0.0f, 1.0f), 
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
};

static constexpr unsigned int QUAD_INDICES[] = {
	0, 1, 2,
	2, 3, 0
};

GLuint RenderTarget::quadVAO, RenderTarget::posVBO, RenderTarget::tposVBO, RenderTarget::iVBO;
std::unique_ptr<ShaderProgram> RenderTarget::quadShaderProgram;

void RenderTarget::init() {
	quadShaderProgram = std::make_unique<ShaderProgram>("shaders/quad.vsh", "shaders/quad.fs");
	quadShaderProgram->bind();

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &posVBO); 
	glBindBuffer(GL_ARRAY_BUFFER, posVBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), &QUAD_VERTICES[0].x, GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD_VERTICES[0]), (void*) 0); 

	glGenBuffers(1, &tposVBO); 
	glBindBuffer(GL_ARRAY_BUFFER, tposVBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(TEX_COORDS), &TEX_COORDS[0].x, GL_STATIC_DRAW); 
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TEX_COORDS[0]), (void*) 0); 

	glGenBuffers(1, &iVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderTarget::clear(int buffersToClear) {
	glClear(buffersToClear); 
}

void RenderTarget::bindDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Core::screenWidth, Core::screenHeight);
}

void RenderTarget::destroy() {
	quadShaderProgram.release();
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &posVBO);
}

RenderTarget::RenderTarget(int width, int height) : width(width), height(height) {
	glGenFramebuffers(1, &fbo);

	bind();

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0); 
	
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "framebuffer is incomplete" << std::endl;
		exit(EXIT_FAILURE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

void RenderTarget::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
}

void RenderTarget::renderToQuad() {
	quadShaderProgram->bind(); 
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glUniform1i(glGetUniformLocation(quadShaderProgram->getProgram(), "screenTexture"), 0);
	
	glBindVertexArray(quadVAO); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iVBO);
	glDrawElements(GL_TRIANGLES, sizeof(QUAD_INDICES) / sizeof(QUAD_INDICES[0]), GL_UNSIGNED_INT, (void*)0); 
}

RenderTarget::~RenderTarget() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &textureColorbuffer);
	glDeleteRenderbuffers(1, &rbo);
}