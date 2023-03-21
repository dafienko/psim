#version 410

#include "shaders/fluid/fluidUtil.glsl"

out vec4 result;

void main() {
	ivec2 pos = ivec2(texCoords * gridSize);
	if (min(inBounds(pos), getO(pos)) == 0.0) {
		result = vec4(0, 10.0, 0, 1);
		return;
	}

	ivec2 up = pos + ivec2(0, 1);
	ivec2 dp = pos + ivec2(0, -1);
	ivec2 rp = pos + ivec2(1, 0);
	ivec2 lp = pos + ivec2(-1, 0);

	vec2 bl = getVel(pos);
	vec2 tl = getVel(up);
	vec2 br = getVel(rp);

	float b = bl.y;
	float l = bl.x;
	float t = -tl.y;
	float r = -br.x;
	
	float ts = getO(up);
	float bs = getO(dp);
	float rs = getO(rp);
	float ls = getO(lp);
	
	float d = (b + l + t + r); 
	float s = ts + bs + rs + ls;

	result = vec4(d, s, 0.0, 1.0);
}