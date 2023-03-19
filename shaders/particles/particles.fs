#version 410

in vec2 texCoords;
in vec2 pixelPos;

uniform vec2 gridSize;

uniform usampler2D particlesTexture;

out vec4 FragColor;

#include "shaders/particles/particleTypes.glsl"

#define COL(v) v / 255.0
#define RGB(r, g, b) vec4(COL(r), COL(g), COL(b), 1.0)

void main() {
	uint particleType = texture(particlesTexture, texCoords).r;

	switch (particleType) {
	case Wall:
		FragColor = RGB(160, 160, 160);
		break;
	case Sand:
		FragColor = RGB(250, 202, 107);
		break;
	case Water:
		FragColor = RGB(50, 50, 255);
		break;
	case Wood:
		FragColor = RGB(117, 69, 26);
		break;
	case Fire:
		FragColor = RGB(255, 115, 0);
		break;
	default:
		FragColor = vec4(0, 0, 0, 0);	
	}
}