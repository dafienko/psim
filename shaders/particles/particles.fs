#version 410

in vec2 texCoords;
in vec2 pixelPos;

uniform vec2 gridSize;

uniform usampler2D particlesTexture;

out vec4 FragColor;

#include "shaders/particles/particleTypes.glsl"

void main() {
	uint particleType = texture(particlesTexture, texCoords).r;

	switch (particleType) {
	case Wall:
		FragColor = vec4(.6, .6, .6, 1);
		break;
	case Sand:
		FragColor = vec4(1, 206.0/255.0, 107.0/255.0, 1);
		break;
	case Water:
		FragColor = vec4(0, 0, 1, 1);
		break;
	case Particle4:
		FragColor = vec4(1, 1, 1, .5);
		break;
	case Particle5:
		FragColor = vec4(.1, .5, 1, 1);
		break;
	default:
		FragColor = vec4(0, 0, 0, 0);	
	}
}