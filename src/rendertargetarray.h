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
		/**
		 * @brief Construct a new Render Target Array object
		 * 
		 * @param nTargets the number of sub RenderTarget objects to generate
		 * @param width the width in pixels of each render target
		 * @param height the height in pixels of each render target
		 */
		RenderTargetArray(unsigned int nTargets, int width, int height);

		/**
		 * @brief Construct a new Render Target Array object
		 * 
		 * @param nTargets the number of sub RenderTarget objects to generate
		 * @param width the width in pixels of each render target
		 * @param height the height in pixels of each render target
		 * @param internalFormat the OpenGL internal format to use
		 * @param texFilterType the OpenGL texture filter type to use
		 */
		RenderTargetArray(unsigned int nTargets, int width, int height, GLint internalFormat, GLint texFilterType);

		/**
		 * @brief bind the top render target to draw to 
		 * 
		 */
		void bind();

		/**
		 * @brief bind the last render target that was drawn to
		 * 
		 */
		void bindOld();

		/**
		 * @brief bind the top render target as a texture
		 * 
		 */
		void bindAsTexture();

		/**
		 * @brief bind the top render target as a texture
		 * 
		 * @param textureName the name of the sampler uniform
		 * @param program the program the texture uniform is in
		 */
		void bindAsTexture(const char* textureName, GLuint program);

		/**
		 * @brief bind the top render target as a texture
		 * 
		 * @param textureName the name of the sampler uniform
		 * @param program the program the texture uniform is in
		 * @param location the GL_TEXTURE location to set the sampler uniform to
		 */
		void bindAsTexture(const char* textureName, GLuint program, GLint location);

		/**
		 * @brief clear the top render buffer
		 * 
		 * @param buffersToClear the OpenGL buffer flags to clear
		 */
		void clear(int buffersToClear);
		
		/**
		 * @brief Get the last Render Target
		 * 
		 * @return RenderTarget& 
		 */
		RenderTarget& getLast();

		/**
		 * @brief Get the top Render Target object
		 * 
		 * @return RenderTarget& 
		 */
		RenderTarget& getCurrent();
};