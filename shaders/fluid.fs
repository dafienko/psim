#version 410

out vec4 result;

in vec2 texCoords;
in vec2 pixelPos;

uniform sampler2D velTexture;
uniform sampler2D obstaclesTexture;

uniform vec2 simulationSize;

float getS(ivec2 p) {
	if (p.x == 0 || p.y == 0 || p.x == simulationSize.x - 1 || p.y == simulationSize.y - 1) {
		return 0.0;
	}

	return texelFetch(obstaclesTexture, p, 0).x;
}

vec2 getVel(ivec2 p) {
	vec2 vel = texelFetch(velTexture, p, 0).xy;

	if (p == ivec2(1, int(simulationSize.y - 1) / 2)) {
		vel.x = 10;
	} 

	return vel;
}

void main() {
	ivec2 pos = ivec2(texCoords * simulationSize);
	if (getS(pos) == 0.0) {
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

	float s = 0;
	float d = 0;
	
	float ts = getS(pos + ivec2(0, 1));
	float bs = getS(pos + ivec2(0, -1));
	float rs = getS(pos + ivec2(1, 0));
	float ls = getS(pos + ivec2(-1, 0));
	
	d = (b + l + t + r); 
	s = ts + bs + rs + ls;

	result = vec4(d, s, 0.0, 1.0);
}