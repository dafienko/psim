#include "shader.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

void getFileSource(const std::string &filename, std::string &source_text) {
	std::ifstream file(filename);
	
	if (file) {
		std::string line;
		int lno = 0;
		while (std::getline(file, line)) {
			lno++;

			if (line.find("#include") != std::string::npos) {
				std::size_t firstQuote = line.find('"');
				if (firstQuote == std::string::npos) {
					std::cerr << filename << ": error at line " << lno << std::endl;
					exit(EXIT_FAILURE);
				}

				std::size_t secondQuote = line.find('"', firstQuote + 1);
				if (secondQuote == std::string::npos) {
					std::cerr << filename << ": error at line " << lno << std::endl;
					exit(EXIT_FAILURE);
				}

				std::string includeFileName = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
				std::string includeSource;
				getFileSource(includeFileName, includeSource);
				source_text += includeSource + "\n";
			} else {
				source_text += line + "\n";
			}
		}
	} else {
		throw std::invalid_argument("could not find file " + filename);
	}
}

void checkShaderErrors(const std::string &filename, GLuint shader, std::string source) {
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		std::istringstream iss(source);
		int lno = 1;
		for (std::string line; std::getline(iss, line); lno++)
		{
			std::cerr << lno << "   " << line << std::endl;
		}

		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		std::cerr << "error compiling shader " << filename << std::endl;
		std::cerr << &errorLog[0] << std::endl;
		exit(EXIT_FAILURE);
	}
}

ShaderProgram::ShaderProgram(const char* vs_filename, const char* fs_filename) : 
	ShaderProgram(std::string(vs_filename), std::string(fs_filename)) 
{}

ShaderProgram::ShaderProgram(const std::string &vs_filename, const std::string &fs_filename) {
	std::string vertex_source, fragment_source;
	getFileSource(vs_filename, vertex_source);
	getFileSource(fs_filename, fragment_source);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* p = vertex_source.c_str();
	glShaderSource(vertex_shader, 1, &p, NULL);
	glCompileShader(vertex_shader);
	checkShaderErrors(vs_filename, vertex_shader, vertex_source);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	p = fragment_source.c_str();
	glShaderSource(fragment_shader, 1, &p, NULL);
	glCompileShader(fragment_shader);
	checkShaderErrors(fs_filename, fragment_shader, fragment_source);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

GLuint ShaderProgram::getProgram() {
	return program;
}

void ShaderProgram::bind() {
	glUseProgram(program);
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program);
}