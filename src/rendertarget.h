#pragma once 

#include <GL/glew.h>
#include <memory>

#include "shader.h"

class RenderTarget {
	public: 
		static void bindDefault();

	private:
		int width, height;
		GLuint fbo, textureColorbuffer, rbo;

	public:
		/**
		 * @brief Construct a new Render Target object
		 * 
		 * @param width the width of the render target
		 * @param height the height of the render target
		 */
		RenderTarget(int width, int height);

		/**
		 * @brief Construct a new Render Target object
		 * 
		 * @param width the width of the render target
		 * @param height the height of the render target
		 * @param internalFormat the openGL internal format to use
		 * @param texFilterType the openGL texture filter type to use
		 */
		RenderTarget(int width, int height, GLint internalFormat, GLint texFilterType);
		~RenderTarget();

		/**
		 * @brief resize the render target
		 * 
		 * @param width the new width in pixels
		 * @param height the new height in pixels
		 */
		void setSize(int width, int height);

		/**
		 * @brief Get the OpenGL Texture object of this rendertarget
		 * 
		 * @return GLuint 
		 */
		GLuint getTexture() const;

		/**
		 * @brief bind the render target so it can be drawn to
		 * 
		 */
		void bind();

		/**
		 * @brief bind the render target to the GL_TEXTURE0
		 * 
		 */
		void bindAsTexture();

		/**
		 * @brief bind the render target to GL_TEXTURE0 for the specified uniform
		 * 
		 * @param textureName the name of the sampler uniform
		 * @param program the program that contains the uniform
		 */
		void bindAsTexture(const char* textureName, GLuint program);

		/**
		 * @brief bind the render target to the specified location for the specified uniform
		 * 
		 * @param textureName the name of the sampler uniform
		 * @param program the program that contains the uniform
		 * @param location the GL_TEXTURE location to set the uniform to
		 */
		void bindAsTexture(const char* textureName, GLuint program, GLint location);

		/**
		 * @brief clear the render target
		 * 
		 * @param buffersToClear the OpenGL flags to clear
		 */
		void clear(int buffersToClear);

		/**
		 * @brief Render the render target stretched over the current viewport size
		 * 
		 */
		void renderToQuad();

		/**
		 * @brief Render the render target as large as possible while maintaining the specified aspect ratio
		 * 
		 * @param aspect the aspect ratio to maintain
		 */
		void renderToQuadWithAspect(float aspect);
};