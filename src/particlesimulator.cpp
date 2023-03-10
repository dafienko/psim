#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "particlesimulator.h"
#include "quad.h"
#include "core.h"
#include "uitext.h"

ParticleSimulator::ParticleSimulator(glm::ivec2 simulationSize) : 
	grid(new Particle[simulationSize.x * simulationSize.y]),
	byteBuffer(new ParticleType[simulationSize.x * simulationSize.y]),
	ui(dynamic_cast<UI*>((Instance::fromJSON("ui/particlesUI.json")))),
	particlesShader("shaders/grid.vsh", "shaders/particles/particles.fs"),
	obstaclesShader("shaders/grid.vsh", "shaders/particles/obstacles.fs"),
	particlesTargets(2, simulationSize.x, simulationSize.y, GL_RGBA, GL_NEAREST),
	simulationSize(simulationSize)
{
	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			Particle p = {};
			p.type = Air;
			grid.get()[y * simulationSize.x + x] = p;
		}
	}

	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, simulationSize.x, simulationSize.y, 0, GL_RED_INTEGER, GL_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	selectParticleType(Sand);

	Core::keyEvent->bind([&] (int key, int action, int mods) {
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_0) {
				selectParticleType(Air);
			} else if (key == GLFW_KEY_1) {
				selectParticleType(Sand);
			} else if (key == GLFW_KEY_2) {
				selectParticleType(Wall);
			}
		}
	});
}

void ParticleSimulator::selectParticleType(ParticleType type) {
	selectedParticleType = type;
	std::string particleName;
	switch (type) {
	case Air:
		particleName = "Air";
		break;

	case Wall:
		particleName = "Wall";
		break;

	case Sand:
		particleName = "Sand";
		break;
	}

	UIText* typeLabel = dynamic_cast<UIText*>(ui->getChild("particleType"));
	typeLabel->text = particleName;
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

glm::ivec2 ParticleSimulator::screenPosToGridPos(glm::vec2 screenPos) {
	screenPos /= Core::getWindowSize();
	return glm::ivec2(
		(int)(screenPos.x * simulationSize.x),
		(int)((1.0f - screenPos.y) * simulationSize.y)
	);
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
			case Air:
				break;

			case Wall:
				break;

			case Sand:
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

	if (Core::isMouse1Down()) {
		glm::ivec2 mouseGridPos = screenPosToGridPos(Core::getMousePosition());
		for (int x = mouseGridPos.x - brushRadius; x <= mouseGridPos.x + brushRadius; x++) {
			for (int y = mouseGridPos.y - brushRadius; y <= mouseGridPos.y + brushRadius; y++) {
				glm::ivec2 pos(x, y);
				if (inBounds(pos)) {
					Particle p = {};
					p.type = selectedParticleType;
					particleSet(pos, p);
				}
			}
		}
	}
}

void ParticleSimulator::updateParticlesTexture() {
	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			Particle particle = particleGet(glm::ivec2(x, y));

			byteBuffer.get()[y * simulationSize.x + x] = particle.type;
		}
	}

	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, simulationSize.x, simulationSize.y, 0, GL_RED_INTEGER, GL_INT, byteBuffer.get());

	particlesShader.bind();
	particlesTargets.bind();
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClearColor(0, 0, 0, 0);
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

void ParticleSimulator::renderObstacles() {
	obstaclesShader.bind();
	
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glUniform1i(glGetUniformLocation(obstaclesShader.getProgram(), "particlesTexture"), 0);

	glUniform2f(
		glGetUniformLocation(obstaclesShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	Quad::render();
}

void ParticleSimulator::render() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	particlesTargets.getCurrent().renderToQuad();
	ui->render();
}

ParticleSimulator::~ParticleSimulator() {
	glDeleteTextures(1, &textureColorBuffer);
}