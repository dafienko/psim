#include <cassert>

#include "rendertargetarray.h"

RenderTargetArray::RenderTargetArray(unsigned int nTargets, int width, int height, GLint internalFormat, GLint texFilterType) : 
N_TARGETS(nTargets)
{
	assert(nTargets > 1);

	lastTargetIndex = 0;
	currentTargetIndex = 1;

	for (unsigned int i = 0; i < N_TARGETS; i++) {
		targets.push_back(std::make_unique<RenderTarget>(width, height, internalFormat, texFilterType));
	}
}

RenderTargetArray::RenderTargetArray(unsigned int nTargets, int width, int height) : 
RenderTargetArray(nTargets, width, height, GL_RGBA, GL_LINEAR) {}

void RenderTargetArray::bindAsTexture(const char* textureName, GLuint program, GLint location) {
	targets.at(currentTargetIndex)->bindAsTexture(textureName, program, location);
}

void RenderTargetArray::bindAsTexture(const char* textureName, GLuint program) {
	targets.at(currentTargetIndex)->bindAsTexture(textureName, program);
}

void RenderTargetArray::bind() {
	int newIndex = currentTargetIndex + 1;
	newIndex %= N_TARGETS;
	lastTargetIndex = currentTargetIndex;
	currentTargetIndex = newIndex;

	targets.at(currentTargetIndex)->bind();
}

RenderTarget& RenderTargetArray::getCurrent() {
	return *targets.at(currentTargetIndex).get();
}

RenderTarget& RenderTargetArray::getLast() {
	return *targets.at(lastTargetIndex).get();
}