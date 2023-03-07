#version 410

layout (location = 0) in vec2 pos; 
layout (location = 1) in vec2 relPos; 

out vec2 TexCoords;

uniform vec2 glyphTL;
uniform vec2 glyphBR;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
	
	TexCoords = vec2(
		mix(glyphTL.x, glyphBR.x, relPos.x),
		mix(glyphTL.y, glyphBR.y, relPos.y)
	);
}