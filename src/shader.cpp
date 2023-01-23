#include <fstream>
#include <vector>
#include <iostream>

#include "shader.h"

void getFileSource(const std::string &filename, std::string &source_text) {
	std::ifstream file(filename);
		
	std::string line;
	while (std::getline(file, line)) {
		source_text += line + "\n";
	}
}

void checkShaderErrors(const std::string &filename, GLuint shader) {
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		std::cerr << "error compiling shader " << filename << std::endl;
		std::cerr << &errorLog[0] << std::endl;
		exit(1);
	}
}

ShaderProgram::ShaderProgram(const std::string &vs_filename, const std::string &fs_filename) {
	std::string vertex_source, fragment_source;
	getFileSource(vs_filename, vertex_source);
	getFileSource(fs_filename, fragment_source);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* p = vertex_source.c_str();
    glShaderSource(vertex_shader, 1, &p, NULL);
    glCompileShader(vertex_shader);
	checkShaderErrors(vs_filename, vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	p = fragment_source.c_str();
    glShaderSource(fragment_shader, 1, &p, NULL);
    glCompileShader(fragment_shader);
	checkShaderErrors(fs_filename, fragment_shader);

	program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

GLuint ShaderProgram::GetProgram() {
	return program;
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program);
}