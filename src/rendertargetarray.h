#pragma once

#include <vector>
#include <memory>
#include "rendertarget.h"

class RenderTargetArray {
	private:
		std::vector<std::unique_ptr<RenderTarget>> targets;

		int currentTargetIndex;
		int lastTargetIndex;
		const unsigned int N_TARGETS;

	public: 
		RenderTargetArray(unsigned int nTargets, int width, int height);
		RenderTargetArray(unsigned int nTargets, int width, int height, GLint internalFormat, GLint texFilterType);

		void bind();
		void bindAsTexture();
		void bindAsTexture(const char* textureName, GLuint program);
		void bindAsTexture(const char* textureName, GLuint program, GLint location);
		
		RenderTarget& getLast();
		RenderTarget& getCurrent();
};