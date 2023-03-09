#include "simulation.h"

Simulation::Simulation(glm::ivec2 simulationSize) : 
	fluidSimulator(simulationSize),
	particleSimulator(simulationSize),
	simulationSize(simulationSize)
{}

void Simulation::update(float dt) {
	//fluidSimulator.update(dt);
	particleSimulator.update(dt);
}

void Simulation::render() {
	//fluidSimulator.render();
	particleSimulator.render();
}