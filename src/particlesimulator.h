#pragma once 

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>

#include "shader.h"
#include "rendertargetarray.h"

class ParticleSimulator {
	private: 
		struct Particle {
			unsigned int type;
		};

	private:
		std::unique_ptr<Particle> grid;
		std::unique_ptr<unsigned int> byteBuffer;

		ShaderProgram particlesShader;
		RenderTargetArray particlesTargets;

		GLuint textureColorBuffer;

		void updateParticles();
		void updateParticlesTexture();
		bool inBounds(glm::ivec2 pos);
		bool isEmpty(glm::ivec2 pos);
		Particle particleGet(glm::ivec2 pos);
		void particleSet(glm::ivec2 pos, Particle particle);
	public: 
		const glm::ivec2 simulationSize;

		ParticleSimulator(glm::ivec2 simulationSize);

		void update(float dt);
		void render();

		~ParticleSimulator();
};