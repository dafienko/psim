#version 410

out vec4 result;

#include "shaders/fluid/advectUtil.glsl"

void main() {
	vec2 fPos = texCoords * gridSize;
	ivec2 pos = ivec2(fPos.xy);
	vec2 vel = getVel(pos);
	vec2 newVel = vel;

	vec2 leftMarker = fPos - vec2(.5, 0);
	vec2 u = vec2(vel.x, sampleV(leftMarker)) * dt;
	vec2 leftAdvectFrom = leftMarker - u;
	if (getO(ivec2(round(leftAdvectFrom.x), round(leftAdvectFrom.y))) > 0) { 
		newVel.x = sampleU(leftAdvectFrom);
	}

	vec2 bottomMarker = fPos - vec2(0, .5);
	vec2 v = vec2(sampleU(bottomMarker), vel.y) * dt;
	vec2 bottomAdvectFrom = bottomMarker - v;
	if (getO(ivec2(round(bottomAdvectFrom.x), round(bottomAdvectFrom.y))) > 0) { 
		newVel.y = sampleV(bottomAdvectFrom);
	}

	result = vec4(newVel.xy, 0.0, 1.0);
}