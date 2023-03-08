#version 410

layout (location = 0) in vec2 pos; 
layout (location = 1) in vec2 relPos; 

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
}