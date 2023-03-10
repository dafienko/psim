#include <iostream>

#include "particlesimulator.h"
#include "quad.h"
#include "core.h"

ParticleSimulator::ParticleSimulator(glm::ivec2 simulationSize) : 
	grid(new Particle[simulationSize.x * simulationSize.y]),
	byteBuffer(new unsigned int[simulationSize.x * simulationSize.y]),
	particlesShader("shaders/grid.vsh", "shaders/particles/particles.fs"),
	particlesTargets(2, simulationSize.x, simulationSize.y, GL_RGBA, GL_NEAREST),
	simulationSize(simulationSize)
{
	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			Particle p = {};
			p.type = 0;
			grid.get()[y * simulationSize.x + x] = p;
		}
	}

	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, simulationSize.x, simulationSize.y, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool ParticleSimulator::inBounds(glm::ivec2 p) {
	return p.x >= 0 && p.y >= 0 && p.x < simulationSize.x && p.y < simulationSize.y;
}

ParticleSimulator::Particle ParticleSimulator::particleGet(glm::ivec2 p) {
	return grid.get()[p.y * simulationSize.x + p.x];
}

void ParticleSimulator::particleSet(glm::ivec2 p, Particle particle) {
	grid.get()[p.y * simulationSize.x + p.x] = particle;
}

bool ParticleSimulator::isEmpty(glm::ivec2 p) {
	return inBounds(p) && particleGet(p).type == 0;
}



void ParticleSimulator::updateParticles() {
	static const glm::ivec2 DIRECTIONS[3] = {
		glm::ivec2(0, -1),
		glm::ivec2(1, -1),
		glm::ivec2(-1, -1)
	};

	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			glm::ivec2 pos(x, y);
			Particle p = particleGet(pos);
			
			switch (p.type) {
			case (1):
				for (glm::ivec2 dir : DIRECTIONS) {
					glm::ivec2 searchAt = pos + dir;
					if (isEmpty(searchAt)) {
						particleSet(pos, {});
						particleSet(searchAt, p);
						break;
					}
				}
				break;
			}
		}
	}

	Particle p = {};
	p.type = 1;
	particleSet(glm::ivec2(simulationSize.x / 2, simulationSize.y - 1), p);
}

void ParticleSimulator::updateParticlesTexture() {
	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			Particle particle = particleGet(glm::ivec2(x, y));

			byteBuffer.get()[y * simulationSize.x + x] = particle.type;
		}
	}

	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, simulationSize.x, simulationSize.y, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, byteBuffer.get());
	CHECK_GL_ERRORS;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	particlesShader.bind();
	particlesTargets.bind();
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glUniform1i(glGetUniformLocation(particlesShader.getProgram(), "particlesTexture"), 0);

	glUniform2f(
		glGetUniformLocation(particlesShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	Quad::render();
}

void ParticleSimulator::update(float dt) {
	updateParticles();
	updateParticlesTexture();
}

void ParticleSimulator::render() {
	particlesTargets.getCurrent().renderToQuad();
}

ParticleSimulator::~ParticleSimulator() {
	glDeleteTextures(1, &textureColorBuffer);
}