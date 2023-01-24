#pragma once

#include <GL/glew.h>

class ShaderProgram {
	private:
		GLuint program;
		GLuint vertex_shader, fragment_shader;
	public:
		ShaderProgram(const std::string &vs_filename, const std::string &fs_filename);
		~ShaderProgram();

		GLuint getProgram();		
};