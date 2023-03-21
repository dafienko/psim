#include <iostream>
#include <glm/ext.hpp>

#include "core.h"
#include "rendertarget.h"
#include "quad.h"

void RenderTarget::clear(int buffersToClear) {
	bind();
	glClear(buffersToClear); 
}

void RenderTarget::bindDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Core::getWindowSize().x, Core::getWindowSize().y);
}

RenderTarget::RenderTarget(int width, int height, GLint internalFormat, GLint texFilterType) : width(width), height(height) {
	glGenFramebuffers(1, &fbo);

	bind();

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilterType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilterType);
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

RenderTarget::RenderTarget(int width, int height) : RenderTarget(width, height, GL_RGBA, GL_LINEAR) {}

void RenderTarget::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
}

GLuint RenderTarget::getTexture() const {
	return textureColorbuffer;
}

void RenderTarget::bindAsTexture(const char* textureName, GLuint shaderProgram, GLint location) {
	glActiveTexture(GL_TEXTURE0 + location); 
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glUniform1i(glGetUniformLocation(shaderProgram, textureName), location);
}

void RenderTarget::bindAsTexture(const char* textureName, GLuint shaderProgram) {
	bindAsTexture(textureName, shaderProgram, 0);
}

void RenderTarget::bindAsTexture() {
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
}

void RenderTarget::renderToQuad() {
	Quad::quadShaderProgram->bind(); 
	glDisable(GL_DEPTH_TEST);

	bindAsTexture("screenTexture", Quad::quadShaderProgram->getProgram());
	
	Quad::render();
}

void RenderTarget::renderToQuadWithAspect(float aspect) {
	Quad::quadShaderProgram->bind(); 
	glDisable(GL_DEPTH_TEST);

	bindAsTexture("screenTexture", Quad::quadShaderProgram->getProgram());
	
	Quad::renderAspect(aspect);
}

RenderTarget::~RenderTarget() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &textureColorbuffer);
	glDeleteRenderbuffers(1, &rbo);
}