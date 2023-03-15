#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>

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
			} else if (key == GLFW_KEY_3) {
				selectParticleType(Water);
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
	
	case Water:
		particleName = "Water";
		break;

	case Test:
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

glm::vec2 ParticleSimulator::sampleVelocityBuffer(float* fluidVelocityBuffer, glm::ivec2 pos) {
	float* thisVel = fluidVelocityBuffer + pos.y * (simulationSize.x + 1) * 3 + pos.x * 3;
	float* rightVel = fluidVelocityBuffer + pos.y * (simulationSize.x + 1) * 3 + (pos.x+1) * 3;
	float* upVel = fluidVelocityBuffer + (pos.y+1) * (simulationSize.x + 1) * 3 + pos.x * 3;

	return glm::vec2(
		(*(thisVel + 0) + *(rightVel + 0)) / 2.0f,
		(*(thisVel + 1) + *(upVel + 1)) / 2.0f
	);
}

void ParticleSimulator::updateParticles(float* fluidVelocityBuffer) {
	static const glm::ivec2 DOWN = glm::ivec2(0, -1);
	static const glm::ivec2 LEFT = glm::ivec2(-1, 0);
	static const glm::ivec2 RIGHT = glm::ivec2(1, 0);
	static const glm::ivec2 DOWNLEFT = glm::ivec2(-1, -1);
	static const glm::ivec2 DOWNRIGHT = glm::ivec2(1, -1);
	static bool reverse = false;
	reverse = !reverse;

	static auto checkDir = [&] (glm::ivec2 pos, glm::ivec2 dir) {
		glm::ivec2 searchAt = pos + dir;
		Particle particle = particleGet(pos);
		particle.updated = true;

		if (isEmpty(searchAt)) {
			particleSet(pos, {});
			particleSet(searchAt, particle);

			return true;
		}

		return false;
	};

	static auto checkSymmetricalDirs = [&] (glm::ivec2 pos, glm::ivec2 dirA, glm::ivec2 dirB) {
		if (rand() % 2 == 0) {
			if (checkDir(pos, dirA)) { return true; }
			if (checkDir(pos, dirB)) { return true; }
		} else {
			if (checkDir(pos, dirB)) { return true; }
			if (checkDir(pos, dirA)) { return true; }
		}

		return false;
	};

	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			glm::ivec2 pos(x, y);
			Particle particle = particleGet(pos);
			particle.updated = false;
			particleSet(pos, particle);
		}
	}

	for (int y = 0; y < simulationSize.y; y++) {
		for (int i = 0; i < simulationSize.x; i++) {
			int x = reverse ? (simulationSize.x - 1) - i : i;

			glm::ivec2 pos(x, y);
			glm::vec2 vel = sampleVelocityBuffer(fluidVelocityBuffer, pos);

			Particle particle = particleGet(pos);
			if (particle.updated) {
				continue;
			}

			switch (particle.type) {
			case Air:
			case Test:
			case Wall:
				break;

			case Sand:
				if (checkDir(pos, DOWN)) { break; }	
				if (checkSymmetricalDirs(pos, DOWNLEFT, DOWNRIGHT)) { break; }

				break;

			case Water:
				if (checkDir(pos, DOWN)) { break; }
				if (checkSymmetricalDirs(pos, DOWNLEFT, DOWNRIGHT)) { break; }
				if (checkSymmetricalDirs(pos, LEFT, RIGHT)) { break; }

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
	
	glClearColor(0, 0, 0, 0);
	particlesTargets.clear(GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glUniform1i(glGetUniformLocation(particlesShader.getProgram(), "particlesTexture"), 0);

	glUniform2f(
		glGetUniformLocation(particlesShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	Quad::render();
}

void ParticleSimulator::update(float dt, float* fluidVelocityBuffer) {
	updateParticles(fluidVelocityBuffer);
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