#version 410

out vec4 result;

in vec2 texCoords;
in vec2 pixelPos;

uniform sampler2D velTexture;
uniform sampler2D pressureTexture;
uniform sampler2D obstaclesTexture;

uniform vec2 simulationSize;

vec2 getDS(ivec2 cell) {
	return texelFetch(pressureTexture, cell, 0).xy;
}

vec2 getVel(ivec2 p) {
	vec2 vel = texelFetch(velTexture, p, 0).xy;
	if (p == ivec2(0, int(simulationSize.y) / 2)) {
		vel.x = 1;
	} 

	return vel;
}

float getO(ivec2 p) {
	return texelFetch(obstaclesTexture, p, 0).x;
}

void main() {
	ivec2 pos = ivec2(texCoords * simulationSize);
	vec2 ds = getDS(pos);
	vec2 thisVel = getVel(pos);
	vec2 rightVel = getVel(pos + ivec2(1, 0));
	vec2 upVel = getVel(pos + ivec2(0, 1));
	thisVel = vec2(thisVel.x + rightVel.x, thisVel.y + upVel.y) / 2;

	float o = getO(pos);
	result = vec4(abs(thisVel.x) / 1, abs(thisVel.y) / 1, 0.0, 1.0);
	// result = vec4(ds.x / 1.0, 0, -ds.x / 1.0, 1);

	if (o == 0.0) {
		result = vec4(1.0, 1.0, 1.0, 1.0);
	}
}