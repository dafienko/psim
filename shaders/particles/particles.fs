#version 410

in vec2 texCoords;
in vec2 pixelPos;

uniform vec2 gridSize;

uniform usampler2D particlesTexture;

out vec4 FragColor;


const uint Particle1 = 0x01u;
const uint Particle2 = 0x02u;
const uint Particle3 = 0x03u;

void main() {
	uint v = texture(particlesTexture, texCoords).r;

	switch (v) {
	case Particle1:
		FragColor = vec4(1, 0, 0, 1);
		break;
	case Particle2:
		FragColor = vec4(0, 0, 1, 1);
		break;
	case Particle3:
		FragColor = vec4(0, 1, 0, 1);
		break;
	default:
		FragColor = vec4(0, 0, 0, 0);	
	}
}