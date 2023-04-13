#pragma once

#include <GL/glew.h>
#include <string>

class ShaderProgram {
	private:
		GLuint program;
		GLuint vertex_shader, fragment_shader;
	public:
		/**
		 * @brief Construct a new Shader Program object
		 * 
		 * @param vs_filename the filename of the vertex shader
		 * @param fs_filename the filename of the fragment shader
		 */
		ShaderProgram(const char* vs_filename, const char* fs_filename);

		/**
		 * @brief Construct a new Shader Program object
		 * 
		 * @param vs_filename the filename of the vertex shader
		 * @param fs_filename the filename of the fragment shader
		 */
		ShaderProgram(const std::string &vs_filename, const std::string &fs_filename);
		~ShaderProgram();

		/**
		 * @brief Get the OpenGL shader program object
		 * 
		 * @return GLuint 
		 */
		GLuint getProgram();

		/**
		 * @brief bind the shader program to the OpenGL context
		 * 
		 */
		void bind();
};