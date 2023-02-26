#version 410

#include "shaders/fluidUtil.glsl"

out vec4 result;

void main() {
	ivec2 pos = ivec2(texCoords * gridSize);
	if (getO(pos) == 0.0) {
		result = vec4(0, 0, 0, 1);
		return;
	}

	vec2 bl = getVel(pos);
	vec2 tl = getVel(pos + ivec2(0, 1));
	vec2 br = getVel(pos + ivec2(1, 0));

	float b = bl.y;
	float l = bl.x;
	float t = -tl.y;
	float r = -br.x;
	
	float ts = getO(pos + ivec2(0, 1));
	float bs = getO(pos + ivec2(0, -1));
	float rs = getO(pos + ivec2(1, 0));
	float ls = getO(pos + ivec2(-1, 0));
	
	float d = (b + l + t + r); 
	float s = ts + bs + rs + ls;

	result = vec4(d, s, 0.0, 1.0);
}