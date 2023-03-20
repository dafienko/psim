#version 410

out vec4 result;

#include "shaders/fluid/fluidUtil.glsl"

uniform int renderMode;

const int DensityRenderMode = 0x00;
const int PressureRenderMode = 0x01;
const int VelocityRenderMode = 0x02;

void main() {
	ivec2 pos = ivec2(texCoords * gridSize);

	vec2 ds = getDS(pos);

	vec2 thisVel = getVel(pos);
	vec2 rightVel = getVel(pos + ivec2(1, 0));
	vec2 upVel = getVel(pos + ivec2(0, 1));
	thisVel = vec2(thisVel.x + rightVel.x, thisVel.y + upVel.y) / 2;

	switch(renderMode) {
	case DensityRenderMode:
		result = vec4(getDensity(pos).xyz, 1);
		break;
	case PressureRenderMode:
		result = vec4(ds.x / 1.0, 0, -ds.x / 1.0, 1);
		break;
	case VelocityRenderMode:
		result = vec4(abs(thisVel.x) / 60, thisVel.y / 20, thisVel.y / -20, 1.0);
		break;
	}

	// draw fluid obstacles
	// float o = getO(pos);
	// if (o == 0.0) {
	// 	result = vec4(1.0, 1.0, 1.0, 1.0);
	// }
}