#include <GL/glew.h>
#include <glm/ext.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <string>

#include "core.h"
#include "shader.h"

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

GLuint vertex_buffer;
GLint mvp_location, vpos_location, vcol_location;
std::unique_ptr<ShaderProgram> shaderProgram;

glm::mat4x4 p, mvp;
glm::mat4x4 m = glm::identity<glm::mat4x4>();

void Core::init() {
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	shaderProgram = std::make_unique<ShaderProgram>("shaders/vertex.glsl", "shaders/fragment.glsl");

	GLuint program = shaderProgram->getProgram();
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
 
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*) 0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*) (sizeof(float) * 2));

	m = glm::identity<glm::mat4x4>();
}

void Core::update(int width, int height, float dt) {
	float ratio = width / (float) height;
	p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

	m = glm::rotate(m, dt, glm::vec3(0, 0, 1));
	mvp = p * m;
}

void Core::render(int width, int height) {
	GLuint program = shaderProgram->getProgram();
	glUseProgram(program);
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Core::destroy() {
	shaderProgram.release();
}