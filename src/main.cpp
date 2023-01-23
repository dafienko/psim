#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/ext.hpp"
#include <iostream>

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
 
static void error_callback(int error, const char* description)
{
    std::cout << "Error: " << description << std::endl;
	exit(1);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
 
int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer;
    GLint mvp_location, vpos_location, vcol_location;
 
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);
 
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    ShaderProgram shaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
	GLuint program = shaderProgram.GetProgram();
	
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
 
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*) 0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*) (sizeof(float) * 2));
 
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glm::mat4x4 p, mvp;
		glm::mat4x4 m = glm::identity<glm::mat4x4>();
 
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
 
		m = glm::rotate(m, (float)glfwGetTime(), glm::vec3(0, 0, 1));
		p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		mvp = p * m;
 
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}