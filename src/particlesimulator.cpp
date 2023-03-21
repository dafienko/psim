#include "particlesimulator.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <optional>

#include "quad.h"
#include "core.h"
#include "uitext.h"

#define FLUID_AFFECTS_PARTICLE_TYPE(particleType) particleType == Sand || particleType == Water
#define GRAVITY_AFFECTS_PARTICLE_TYPE(particleType) particleType == Sand || particleType == Water

float randf() {
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

ParticleSimulator::ParticleSimulator(glm::ivec2 simulationSize) : 
	grid(new Particle[simulationSize.x * simulationSize.y]),
	
	byteBuffer(new ParticleType[simulationSize.x * simulationSize.y]),
	addedFluidVelocityBuffer(new float[(simulationSize.x + 1) * (simulationSize.y + 1) * 4]),
	addedFluidDensityBuffer(new float[simulationSize.x * simulationSize.y * 4]),

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
			p.simulateAsParticle = false;
			p.cellularVelocity = glm::vec2(0.0f);
			p.particleVelocity = glm::vec2(0.0f);
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
			} else if (key == GLFW_KEY_4) {
				selectParticleType(Wood);
			} else if (key == GLFW_KEY_5) {
				selectParticleType(Fire);
			}
		}
	});

	Core::mouseMoveEvent->bind([&] (double x, double y) {
		if (Core::isMouse1Down()) {
			paintAtCursor();
		}
	});
}

void ParticleSimulator::paintAtCursor() {
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

	case Wood:
		particleName = "Wood";
		break;
	
	case Fire:
		particleName = "Fire";
		break;
	
	default:
		break;
	}

	UIText* typeLabel = dynamic_cast<UIText*>(ui->getChild("particleType"));
	typeLabel->text = particleName;
}

bool ParticleSimulator::inBounds(glm::ivec2 p) {
	return p.x >= 0 && p.y >= 0 && p.x < simulationSize.x && p.y < simulationSize.y;
}

ParticleSimulator::Particle& ParticleSimulator::particleGet(glm::ivec2 p) const {
	return grid.get()[p.y * simulationSize.x + p.x];
}

void ParticleSimulator::particleSet(glm::ivec2 p, Particle particle) {
	grid.get()[p.y * simulationSize.x + p.x] = particle;
}

void ParticleSimulator::particleSwap(glm::ivec2 src, glm::ivec2 dest) {
	if (src == dest) { return; }

	Particle tmp = Particle(particleGet(dest));
	tmp.updated = true;
	particleSet(dest, particleGet(src));
	particleSet(src, tmp);
}

bool ParticleSimulator::isEmpty(glm::ivec2 p) {
	return inBounds(p) && particleGet(p).type == 0;
}

std::optional<ParticleSimulator::ParticleType> ParticleSimulator::isSwappable(glm::ivec2 p, ParticleType replacementType) {
	if (inBounds(p)) {
		Particle& dest = particleGet(p);
		ParticleType type = dest.type;
		if (type == Air || type == Fire || (type == Water && replacementType == Sand)) {
			if (type == Fire) {
				dest.type = Air;
			}
			return type;
		}
	}

	return {};
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

const float GRAVITY_ACCEL = 0.1f;

void ParticleSimulator::updateParticle(glm::ivec2 pos, glm::vec2 fluidVel) {
	static const glm::ivec2 DOWN = glm::ivec2(0, -1);
	static const glm::ivec2 UP = glm::ivec2(0, 1);
	static const glm::ivec2 LEFT = glm::ivec2(-1, 0);
	static const glm::ivec2 RIGHT = glm::ivec2(1, 0);
	static const glm::ivec2 DOWNLEFT = glm::ivec2(-1, -1);
	static const glm::ivec2 DOWNRIGHT = glm::ivec2(1, -1);
	static const glm::ivec2 UPLEFT = glm::ivec2(-1, 1);
	static const glm::ivec2 UPRIGHT = glm::ivec2(1, 1);
	static const glm::ivec2 DIRS[] = {DOWN, UP, LEFT, RIGHT, DOWNLEFT, UPLEFT, DOWNRIGHT, UPRIGHT};

	static auto checkDir = [&] (glm::ivec2 pos, glm::ivec2 dir) {
		glm::ivec2 searchAt = pos + dir;

		return isSwappable(searchAt, particleGet(pos).type);
	};

	static auto checkSymmetricalDirs = [&] (glm::ivec2 pos, glm::ivec2 dirA, glm::ivec2 dirB) {
		if (rand() % 2 == 0) {
			if (checkDir(pos, dirA)) { return dirA; }
			if (checkDir(pos, dirB)) { return dirB; }
		} else {
			if (checkDir(pos, dirB)) { return dirB; }
			if (checkDir(pos, dirA)) { return dirA; }
		}

		return glm::ivec2(0);
	};
	
 	Particle& particle = particleGet(pos);

	if (FLUID_AFFECTS_PARTICLE_TYPE(particle.type)) {
		if (glm::length(fluidVel) > 10) {
			if (!particle.simulateAsParticle) {
				// particle.simulateAsParticle = true;
				particle.particleVelocity = glm::vec2(particle.cellularVelocity.x, particle.cellularVelocity.y);
			}

			particle.particleVelocity = glm::mix(particle.particleVelocity, fluidVel, .5f);
		}
	}

	if (GRAVITY_AFFECTS_PARTICLE_TYPE(particle.type)) {
		particle.particleVelocity += glm::vec2(0, -GRAVITY_ACCEL);
		particle.cellularVelocity += glm::vec2(0, -GRAVITY_ACCEL);
	}

	if (particle.simulateAsParticle) {
		
	} else {
		switch (particle.type) {
		case Air:
		case Wood:
		case Wall: {
			break;
		}

		case Sand: {
			glm::ivec2 newPos = pos;
			for (int y = 0; y > (int)particle.cellularVelocity.y - 1; y--) {
				auto downMaterial = checkDir(newPos, DOWN);
				if (downMaterial == Air) {
					newPos += DOWN;
				} else {
					if (downMaterial == Water) {
						newPos += DOWN;
					} else {
						newPos += checkSymmetricalDirs(newPos, DOWNLEFT, DOWNRIGHT);
					}
					
					particle.cellularVelocity = glm::vec2(0, 0);
					
					break;
				}
			}

			particleSwap(pos, newPos);

			break;
		}

		case Water: {
			glm::ivec2 newPos = pos;
			for (int y = 0; y > std::min((int)particle.cellularVelocity.y, -1); y--) {
				if (checkDir(newPos, DOWN)) {
					newPos += DOWN;
				} else {
					glm::ivec2 fallDir = checkSymmetricalDirs(newPos, DOWNLEFT, DOWNRIGHT);
					if (fallDir == glm::ivec2(0)) {
						int spreadDir = rand() % 2 * 2 - 1;
						for (int i = 0; i < 2 + rand() % 8; i++) {
							glm::ivec2 dir(spreadDir, 0);
							if (checkDir(newPos, dir)) {
								newPos += dir;
							} else {
								break;
							}
						}
					} else {
						newPos += fallDir;
					}

					particle.cellularVelocity = glm::vec2(0, 0);
					
					break;
				}
			}

			particleSwap(pos, newPos);

			break;
		}
		
		case Fire: {
			if (rand() % 1 == 0) { // smoke chance
				addFluidVelocity(pos + glm::ivec2(0, 2), 1, 15, 0, 5, -5);
				setFluidDensity(pos + glm::ivec2(0, 2), 0, glm::vec4(.24, .24, .24, 1.0));
			}

			if (rand() % 5 == 0) { // spread chance
				for (glm::ivec2 dir : DIRS) {
					glm::ivec2 searchAt = pos + dir;
					if (inBounds(searchAt) && particleGet(searchAt).type == Wood) {
						Particle& near = particleGet(searchAt);
						near.type = BurningWood;
						near.updated = true;
					}
				}
			}

			if (rand() % 3 == 0) { // rise chance
				if (rand() % 4 < 3) { // straight up
					glm::ivec2 searchAt = pos + UP;
					if (inBounds(searchAt) && particleGet(searchAt).type == Air) {
						particleSwap(pos, searchAt);
					}
				} else { // up diag
					glm::ivec2 dir = rand() % 2 == 0 ? UPRIGHT : UPLEFT;
					glm::ivec2 searchAt = pos + dir;
					if (inBounds(searchAt) && particleGet(searchAt).type == Air) {
						particleSwap(pos, searchAt);
					}
				}
			}

			if (rand() % 20 == 0) { // die chance
				particle.type = Air;
			}

			break;
		}

		case BurningWood: {
			for (glm::ivec2 dir : {RIGHT, LEFT, UP, UPRIGHT, UPLEFT}) {
				glm::ivec2 searchAt = pos + dir;
				if (inBounds(searchAt) && particleGet(searchAt).type == Water) {
					particle.type = Wood;
					break;
				}
			}

			if (rand() % 20 == 0) { // spread chance
				for (glm::ivec2 dir : DIRS) {
					glm::ivec2 searchAt = pos + dir;
					if (inBounds(searchAt) && particleGet(searchAt).type == Wood) {
						Particle& near = particleGet(searchAt);
						near.type = BurningWood;
						near.updated = true;
					}
				}
			}

			if (rand() % 3 == 0) { // flame chance
				if (rand() % 4 < 3) { // straight up
					glm::ivec2 searchAt = pos + UP;
					if (inBounds(searchAt) && particleGet(searchAt).type == Air) {
						particleGet(searchAt).type = Fire;
					}
				} else { // up diag
					glm::ivec2 dir = rand() % 2 == 0 ? UPRIGHT : UPLEFT;
					glm::ivec2 searchAt = pos + dir;
					if (inBounds(searchAt) && particleGet(searchAt).type == Air) {
						particleGet(searchAt).type = Fire;
					}
				}
			}

			if (rand() % 80 == 0) { // die chance
				particle.type = Air;
			}

			break;
		}
		}
	}
}

void ParticleSimulator::updateParticles(float* fluidVelocityBuffer) {
	static bool reverse = false;
	reverse = !reverse;

	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			particleGet(glm::ivec2(x, y)).updated = false;
		}
	}

	for (int y = 0; y < simulationSize.y; y++) {
		for (int i = 0; i < simulationSize.x; i++) {
			int x = reverse ? (simulationSize.x - 1) - i : i;

			glm::ivec2 pos(x, y);
			Particle particle = particleGet(pos);
			if (particle.updated) {
				continue;
			}

			updateParticle(
				pos, 
				sampleVelocityBuffer(fluidVelocityBuffer, pos)
			);
		}
	}

	if (Core::isMouse1Down()) {
		glm::ivec2 mouseGridPos = screenPosToGridPos(Core::getMousePosition());
		paintAtCursor();
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

void addPixel(float* pixel, float r, float g, float b, float a) {
	*(pixel + 0) += r;
	*(pixel + 1) += g;
	*(pixel + 2) += b;
	*(pixel + 3) = a;
}

void addPixel(float* pixel, glm::vec4 c) {
	addPixel(pixel, c.x, c.y, c.z, c.w);
}

void setPixel(float* pixel, float r, float g, float b, float a) {
	*(pixel + 0) = r;
	*(pixel + 1) = g;
	*(pixel + 2) = b;
	*(pixel + 3) = a;
}

void setPixel(float* pixel, glm::vec4 c) {
	setPixel(pixel, c.x, c.y, c.z, c.w);
}

void ParticleSimulator::addFluidVelocity(glm::ivec2 pos, int r, float top, float bottom, float right, float left) {
	int leftBound = pos.x - r;
	int rightBound = pos.x + r + 1;
	int topBound = pos.y + r + 1;
	int bottomBound = pos.y + r;
	for (int y = bottomBound; y <= topBound; y++) {
		for (int x = leftBound; x <= (y == topBound ? rightBound - 1 : rightBound); x++) {
			if (!(x > 0 && y > 0 && x <= simulationSize.x && y <= simulationSize.y)) {
				continue;
			}

			float u = 0;
			if (y < topBound) {
				u = x <= pos.x ? left : right;
			}

			float v = 0;
			if (x < rightBound) {
				v = y <= pos.y ? bottom : top;
			}

			float* pixel = addedFluidVelocityBuffer.get() + 4 * (y * (simulationSize.x + 1) + x);
			addPixel(pixel, glm::vec4(
				u,
				v,
				0,
				1
			));
		}
	}
}

void ParticleSimulator::setFluidDensity(glm::ivec2 pos, int r, glm::vec4 col) {
	for (int y = pos.y - r; y <= pos.y + r; y++) {
		for (int x = pos.x - r; x <= pos.x + r; x++) {
			if (!inBounds(glm::ivec2(x, y))) {
				continue;
			}

			float* pixel = addedFluidDensityBuffer.get() + 4 * (y * simulationSize.x + x);
			setPixel(pixel, col);
		}
	}
}

void ParticleSimulator::updateFluidTextures(GLuint fluidVelocityTexture, GLuint fluidDensityTexture) {
	glBindTexture(GL_TEXTURE_2D, fluidVelocityTexture);
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA32F, 
		simulationSize.x + 1, simulationSize.y + 1, 
		0, GL_RGBA, GL_FLOAT, 
		addedFluidVelocityBuffer.get()
	);

	glBindTexture(GL_TEXTURE_2D, fluidDensityTexture);
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA32F, 
		simulationSize.x, simulationSize.y, 
		0, GL_RGBA, GL_FLOAT, 
		addedFluidDensityBuffer.get()
	);
}

void ParticleSimulator::clearFluidBuffers() {
	for (int y = 0; y < simulationSize.y; y++) {
		for (int x = 0; x < simulationSize.x; x++) {
			float* pixel = addedFluidDensityBuffer.get() + 4 * (y * simulationSize.x + x);
			setPixel(pixel, glm::vec4(0, 0, 0, 0));
		}
	}

	for (int y = 0; y <= simulationSize.y; y++) {
		for (int x = 0; x <= simulationSize.x; x++) {
			float* pixel = addedFluidVelocityBuffer.get() + 4 * (y * (simulationSize.x + 1) + x);
			setPixel(pixel, glm::vec4(0, 0, 0, 0));
		}
	}
}

void ParticleSimulator::update(float dt, float* fluidVelocityBuffer, GLuint fluidVelocityTexture, GLuint fluidDensityTexture) {
	clearFluidBuffers();
	updateParticles(fluidVelocityBuffer);
	updateFluidTextures(fluidVelocityTexture, fluidDensityTexture); 
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
	updateParticlesTexture();
	RenderTarget::bindDefault();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	particlesTargets.getCurrent().renderToQuadWithAspect(static_cast<float>(simulationSize.x) / static_cast<float>(simulationSize.y));
	ui->render();
}

ParticleSimulator::~ParticleSimulator() {
	glDeleteTextures(1, &textureColorBuffer);
}