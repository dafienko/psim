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
		//! The size in pixels of the simulator
		const glm::ivec2 simulationSize;

		/**
		 * @brief Construct a new Simulation object
		 * 
		 * @param simulationSize the simulation size in pixels
		 */
		explicit Simulation(glm::ivec2 simulationSize);

		/**
		 * @brief step the simulation once
		 * 
		 * @param dt the real-time delta since the last update
		 */
		void update(float dt);

		void render() override;
};