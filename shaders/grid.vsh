#version 410

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tPos;

uniform vec2 gridSize;

out vec2 texCoords;
out vec2 pixelPos;

void main() {
	gl_Position = vec4(pos, 0.0, 1.0);
	texCoords = tPos;
	pixelPos = (gridSize - vec2(1.0, 1.0)) * texCoords;
}