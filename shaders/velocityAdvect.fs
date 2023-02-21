#version 410

out vec4 result;

in vec2 texCoords;
in vec2 pixelPos;

uniform sampler2D velTexture;
uniform sampler2D obstaclesTexture;

uniform vec2 simulationSize;

float dt = .025;

vec2 getVel(ivec2 p) {
	return texelFetch(velTexture, p, 0).xy;
}

float getO(ivec2 p) {
	return texelFetch(obstaclesTexture, p, 0).x;
}

float bilinear(float A, float B, float C, float D, vec2 alpha) {
	float u = mix(A, B, alpha.x);
	float v = mix(C, D, alpha.x);
	return mix(v, u, alpha.y);
}

float sampleU(vec2 pos) {
	pos -= vec2(0, .5);
	ivec2 bl = ivec2(pos);
	float A = getVel(bl + ivec2(0, 1)).x;
	float B = getVel(bl + ivec2(1, 1)).x;
	float C = getVel(bl).x;
	float D = getVel(bl + ivec2(1, 0)).x;
	
	vec2 alpha = pos - vec2(bl);
	return bilinear(A, B, C, D, alpha);
}

float sampleV(vec2 pos) {
	pos -= vec2(.5, 0);
	ivec2 bl = ivec2(pos);
	float A = getVel(bl + ivec2(0, 1)).y;
	float B = getVel(bl + ivec2(1, 1)).y;
	float C = getVel(bl).y;
	float D = getVel(bl + ivec2(1, 0)).y;

	vec2 alpha = pos - vec2(bl);
	return bilinear(A, B, C, D, alpha);
}

float sampleDensity(vec2 pos) {
	ivec2 bl = ivec2(pos);
	float A = getVel(bl + ivec2(0, 1)).y;
	float B = getVel(bl + ivec2(1, 1)).y;
	float C = getVel(bl).y;
	float D = getVel(bl + ivec2(1, 0)).y;
	
	vec2 alpha = pos - vec2(bl);
	return bilinear(A, B, C, D, alpha);
}

void main() {
	vec2 fPos = texCoords * simulationSize;
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