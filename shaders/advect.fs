#version 410

out vec4 result;

in vec2 texCoords;
in vec2 pixelPos;

uniform sampler2D velTexture;
uniform sampler2D obstaclesTexture;

uniform vec2 simulationSize;

float dt = .02;

float getO(ivec2 p) {
	if (p.x == 0 || p.y == 0 || p.x == simulationSize.x - 1 || p.y == simulationSize.y - 1) {
		return 0.0;
	}
	
	return texelFetch(obstaclesTexture, p, 0).x;
}

vec2 getVel(ivec2 p) {
	return texelFetch(velTexture, p, 0).xy;
}

float linear(float A, float B, float alpha) {
	return A * (1 - alpha) + B * alpha;
}

float bilinear(float A, float B, float C, float D, vec2 alpha) {
	float u = linear(A, B, alpha.x);
	float v = linear(C, D, alpha.x);
	return linear(v, u, alpha.y);
}

float sampleU(vec2 pos) {
	ivec2 p = ivec2(pos - vec2(0, .5));
	float A = getVel(p).x;
	float B = getVel(p + ivec2(1, 0)).x;
	float C = getVel(p + ivec2(0, 1)).x;
	float D = getVel(p + ivec2(1, 1)).x;
	
	vec2 alpha = pos - vec2(p);
	
	return bilinear(A, B, C, D, alpha);
}

float sampleV(vec2 pos) {
	ivec2 p = ivec2(pos - vec2(.5, 0));
	float A = getVel(p).y;
	float B = getVel(p + ivec2(1, 0)).y;
	float C = getVel(p + ivec2(0, 1)).y;
	float D = getVel(p + ivec2(1, 1)).y;

	vec2 alpha = pos - vec2(p);
	
	return bilinear(A, B, C, D, alpha);
}

void main() {
	vec2 fPos = texCoords * simulationSize;
	ivec2 pos = ivec2(fPos.xy);
	vec2 vel = getVel(pos);
	vec2 newVel = vel;

	vec2 u = vec2(vel.x, sampleV(fPos + vec2(0, .5)));
	vec2 v = vec2(sampleU(fPos + vec2(.5, 0)), vel.y);

	if (getO(pos - ivec2(round(fPos.x - u.x), round(fPos.y + .5 - u.y))) > 0) { 
		newVel.x = sampleU(vec2(fPos.x - u.x, fPos.y + .5 - u.y));
	}

	if (getO(pos - ivec2(round(fPos.x + .5 - v.x), round(fPos.y - v.y))) > 0) { 
		newVel.y = sampleV(vec2(fPos.x + .5 - v.x, fPos.y - v.y));
	}

	result = vec4(newVel.xy, 0.0, 1.0);
}