#version 410

out vec3 result;

in vec2 texCoords;
in vec2 pixelPos;

uniform sampler2D velTexture;
uniform sampler2D obstaclesTexture;
uniform sampler2D densityTexture;

uniform vec2 simulationSize;

float dt = .025;

vec2 getVel(ivec2 p) {
	return texelFetch(velTexture, p, 0).xy;
}

float getO(ivec2 p) {
	return texelFetch(obstaclesTexture, p, 0).x;
}

int centerY = int(simulationSize.y - 1) / 2;
vec3 WHITE = vec3(1, 1, 1);
vec3 BLUE = vec3(0, .92, 1);
vec3 getDensity(ivec2 p) {
	int dy = abs(centerY - p.y);
	if (p.x == 1 && dy / 4 % 4 == 0) {
		float alpha = 1 - (float(dy) / 3.0);
		return mix(BLUE, WHITE, alpha);
	} else {
		return texelFetch(densityTexture, p, 0).xyz;
	}
}

float bilinear(float A, float B, float C, float D, vec2 alpha) {
	float u = mix(A, B, alpha.x);
	float v = mix(C, D, alpha.x);
	return mix(v, u, alpha.y);
}

vec3 bilinearV3(vec3 A, vec3 B, vec3 C, vec3 D, vec2 alpha) {
	vec3 u = mix(A, B, alpha.x);
	vec3 v = mix(C, D, alpha.x);
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

vec3 sampleDensity(vec2 pos) {
	pos -= vec2(.5, .5);
	ivec2 bl = ivec2(pos);
	vec3 A = getDensity(bl + ivec2(0, 1));
	vec3 B = getDensity(bl + ivec2(1, 1));
	vec3 C = getDensity(bl);
	vec3 D = getDensity(bl + ivec2(1, 0));
	
	vec2 alpha = pos - vec2(bl);
	return bilinearV3(A, B, C, D, alpha);
}

void main() {
	vec2 fPos = texCoords * simulationSize;
	ivec2 pos = ivec2(fPos);

	result = getDensity(pos);

	vec2 vel = vec2(sampleU(fPos), sampleV(fPos));
	vec2 advectFrom = fPos - vel * dt;
	if (getO(ivec2(advectFrom)) > 0) {
		result = sampleDensity(advectFrom);
	}

	result = mix(result, vec3(0), 1 - pow(.9, dt));
}