#version 410

out float result;

in vec2 texCoords;
in vec2 pixelPos;

uniform vec2 gridSize;

uniform usampler2D particlesTexture;

#include "shaders/particles/particleTypes.glsl"

void main() {
	vec2 p = ivec2(texCoords * gridSize);
	uint particleType = texture(particlesTexture, texCoords).r;

	if (p.x == 0 || p.y == 0 || p.y == gridSize.y - 1) {
		result = 0.0;
	} else {
		if (particleType == Wall) {
			result = 0.0;
		} else {
			result = 1.0;
		}
	}
}