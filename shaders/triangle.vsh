#version 410

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec3 vCol;

out vec3 color;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
}