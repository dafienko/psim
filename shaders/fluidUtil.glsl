in vec2 texCoords;
in vec2 pixelPos;

uniform vec2 simulationSize;
uniform vec2 gridSize;

uniform sampler2D velTexture;
uniform sampler2D pressureTexture;
uniform sampler2D obstaclesTexture;
uniform sampler2D densityTexture;

vec2 getDS(ivec2 cell) {
	vec2 ds = texelFetch(pressureTexture, cell, 0).xy;
	return ds;
}

float getO(ivec2 p) {
	if (p.x < 0 || p.y < 0 || p.x >= simulationSize.x || p.y >= simulationSize.y) {
		return 1;
	}
	return texelFetch(obstaclesTexture, p, 0).x;
}

int centerY = int(simulationSize.y - 1) / 2;
vec2 getVel(ivec2 p) {
	vec2 vel = texelFetch(velTexture, p, 0).xy;

	int dy = abs(centerY - p.y);
	if (p.x == 1 && dy / 4 % 4 == 0) {
		vel.x = 40;
	} 

	return vel;
}

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