#pragma once

#include <glm/ext.hpp>

#include "renderable.h"
#include "fluidsimulator.h"
#include "particlesimulator.h"

class Simulation: public Renderable {
	private:
		FluidSimulator fluidSimulator;
		ParticleSimulator particleSimulator;

	public: 
		const glm::ivec2 simulationSize;

		Simulation(glm::ivec2 simulationSize);

		void update(float dt);
		void render() override;
};