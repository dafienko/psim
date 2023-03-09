#pragma once 

#include <glm/ext.hpp>

class ParticleSimulator {
	private: 
		


	public: 
		const glm::ivec2 simulationSize;

		ParticleSimulator(glm::ivec2 simulationSize);

		void update(float dt);
		void render();
};