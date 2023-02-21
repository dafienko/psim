#version 410

out float result;

in vec2 texCoords;
in vec2 pixelPos;

uniform sampler2D oldObstacles;

uniform vec2 simulationSize;
uniform bool init;

void main() {
	vec2 fp = texCoords * simulationSize;
	ivec2 p = ivec2(fp);

	if (init) {
		if (p.x == 0 || p.y == 0 || p.x == simulationSize.x - 1 || p.y == simulationSize.y - 1) {
			result = 0.0;
		} else if (length(fp - vec2(simulationSize / 2)) < 5) {
			result = 0.0;
		} else {
			result = 1.0;
		}
	} else {
		float old = texelFetch(oldObstacles, p, 0).x;
		result = old;
	}
}