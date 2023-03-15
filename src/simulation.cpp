#include "simulation.h"
#include "core.h"

Simulation::Simulation(glm::ivec2 simulationSize) : 
	fluidSimulator(simulationSize),
	particleSimulator(simulationSize),
	simulationSize(simulationSize)
{}

void Simulation::update(float dt) {
	float* velBuffer = fluidSimulator.getVelocityBuffer();
	particleSimulator.update(dt, velBuffer);

	fluidSimulator.bindObstaclesTexture();
	particleSimulator.renderObstacles();

	fluidSimulator.update(dt);
}

void Simulation::render() {
	fluidSimulator.render();
	particleSimulator.render();
	CHECK_GL_ERRORS;
}