#version 410

out float result;

in vec2 texCoords;
in vec2 pixelPos;

uniform vec2 gridSize;

uniform sampler2D oldObstacles;

uniform bool init;

void main() {
	vec2 fp = texCoords * gridSize;
	ivec2 p = ivec2(fp);

	if (init) {
		if (p.x == 0 || p.y == 0 || p.y == gridSize.y - 1) {
			result = 0.0;
		} else if (length(fp - vec2(gridSize / 2)) < 4) {
			result = 0.0;
		} else {
			result = 1.0;
		}
	} else {
		float old = texelFetch(oldObstacles, p, 0).x;
		result = old;
	}
}