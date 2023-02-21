#version 410

out vec4 result;

in vec2 texCoords;
in vec2 pixelPos;

uniform sampler2D velTexture;
uniform sampler2D pressureTexture;
uniform sampler2D obstaclesTexture;

uniform vec2 simulationSize;

float dt = .49;

vec2 getDS(ivec2 cell) {
	vec2 ds = texelFetch(pressureTexture, cell, 0).xy;
	ds.y = max(ds.y, 1.0);
	return ds;
}

float getO(ivec2 p) {
	return texelFetch(obstaclesTexture, p, 0).x;
}

vec2 getVel(ivec2 p) {
	vec2 vel = texelFetch(velTexture, p, 0).xy;
	if (p == ivec2(1, int(simulationSize.y - 1) / 2)) {
		vel.x = 10;
	} 

	return vel;
}

float getForceFraction(float cellO, float neighborS) {
	return (cellO / max(neighborS, 1)) * min(1, neighborS);
}

void main() {
	ivec2 pos = ivec2(texCoords * simulationSize);
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