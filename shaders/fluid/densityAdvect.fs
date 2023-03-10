#version 410

out vec3 result;

#include "shaders/fluid/advectUtil.glsl"

void main() {
	vec2 fPos = texCoords * gridSize;
	ivec2 pos = ivec2(fPos);

	result = getDensity(pos);

	vec2 vel = vec2(sampleU(fPos), sampleV(fPos));
	vec2 advectFrom = fPos - vel * dt;
	if (getO(ivec2(advectFrom)) > 0) {
		result = sampleDensity(advectFrom);
	} else {
		result = vec3(0, 0, 0);
	}

	result = mix(result, vec3(0), 1 - pow(.9, dt));
}