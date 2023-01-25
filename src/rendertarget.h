#pragma once 

#include <GL/glew.h>
#include <memory>
#include "shader.h"

class RenderTarget {
	private:
		static GLuint quadVAO, posVBO, tposVBO, iVBO;
		static std::unique_ptr<ShaderProgram> quadShaderProgram;
	public: 
		static void init();
		static void destroy();
		static void bindDefault();

	private:
		int width, height;
		GLuint fbo, textureColorbuffer, rbo;

	public:
		RenderTarget(int width, int height);
		~RenderTarget();

		void setSize(int width, int height);
		void bind();
		void clear();
		void renderToQuad();
};