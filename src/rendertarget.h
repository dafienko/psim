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
		RenderTarget(int width, int height);
		RenderTarget(int width, int height, GLint internalFormat, GLint texFilterType);
		~RenderTarget();

		void setSize(int width, int height);
		GLuint getTexture() const;
		void bind();
		void bindAsTexture();
		void bindAsTexture(const char* textureName, GLuint program);
		void bindAsTexture(const char* textureName, GLuint program, GLint location);
		void clear(int buffersToClear);
		void renderToQuad();
};