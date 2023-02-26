#version 410

out vec4 result;

#include "shaders/fluidUtil.glsl"

float dt = .49;

float getForceFraction(float cellO, float neighborS) {
	return (cellO / max(neighborS, 1)) * min(1, neighborS);
}

void main() {
	ivec2 pos = ivec2(texCoords * gridSize);
	vec2 thisVel = getVel(pos);
	vec2 DS = getDS(pos);
	vec2 lDS = getDS(pos + ivec2(-1, 0));
	vec2 bDS = getDS(pos + ivec2(0, -1));

	float o = getO(pos);
	float lo = getO(pos + ivec2(-1, 0));
	float bo = getO(pos + ivec2(0, -1));
	
	float u = thisVel.x;
	float v = thisVel.y;

	u += -DS.x * getForceFraction(lo, DS.y) * dt;
	v += -DS.x * getForceFraction(bo, DS.y) * dt;
	
	u += lDS.x * getForceFraction(o, lDS.y) * dt;
	v += bDS.x * getForceFraction(o, bDS.y) * dt;

	result = vec4(u, v, 0.0, 1.0);
}