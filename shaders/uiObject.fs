#version 410

out vec4 color;

uniform vec3 backgroundColor;

void main()
{
	color = vec4(backgroundColor.xyz, 1);
}