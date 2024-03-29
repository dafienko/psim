in vec2 texCoords;
in vec2 pixelPos;

uniform vec2 simulationSize;
uniform vec2 gridSize;

uniform sampler2D velTexture;
uniform sampler2D pressureTexture;
uniform sampler2D obstaclesTexture;
uniform sampler2D densityTexture;

const bool EDGES_ARE_BORDERS = false;

uniform bool JET_STREAMS_ENABLED;
const int spacing = 30;
const int thickness = 3;

vec2 getDS(ivec2 cell) {
	vec2 ds = texelFetch(pressureTexture, cell, 0).xy;
	return ds;
}

float getO(ivec2 p) {
	if (EDGES_ARE_BORDERS && (p.x <= 0 || p.y <= 0 || p.x >= simulationSize.x - 1 || p.y >= simulationSize.y - 1)) {
		return 0.0;
	}
	return texelFetch(obstaclesTexture, p, 0).x;
}

bool onBorder(ivec2 p) {
	return p.x <= 0 || p.y <= 0 || p.x >= simulationSize.x - 1 || p.y >= simulationSize.y - 1;
}

float inBounds(ivec2 p) {
	if (p.x <= 0 || p.y <= 0 || p.x >= simulationSize.x - 1 || p.y >= simulationSize.y - 1) {
		return 0.0;
	}
	return 1.0;
}

int centerY = int(simulationSize.y - 1) / 2;
vec2 getVel(ivec2 p) {
	vec2 vel = texelFetch(velTexture, p, 0).xy;

	if (JET_STREAMS_ENABLED) {
		int dy = abs(centerY - p.y);
		if (p.x == 1 && dy % spacing < thickness) {
			vel.x = 40;
		} 
	}

	return vel;
}

vec3 getDensity(ivec2 p) {
	if (JET_STREAMS_ENABLED) {
		int dy = centerY - p.y;
		int n = dy / spacing;

		if (p.x == 1 && abs(dy - (n * spacing)) < thickness) {
			if (n == -2) {
				return vec3(1, .2, .2);
			} else if (n == -1) {
				return vec3(1, .9, .2);
			} else if (n == 0) {
				return vec3(.2, 1, .1);
			} else if (n == 1) {
				return vec3(0, .95, 1);
			} else if (n == 2) {
				return vec3(.8, .1, 1);
			} else {
				return vec3(1, 1, 1);
			}
		} else {
			return texelFetch(densityTexture, p, 0).xyz;
		}
	} else {
		return texelFetch(densityTexture, p, 0).xyz;
	}
}