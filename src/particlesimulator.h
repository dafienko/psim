#pragma once 

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>

#include "shader.h"
#include "rendertargetarray.h"
#include "ui.h"

class ParticleSimulator {
	private: 
		enum ParticleType {
			Air,
			Wall,
			Sand,
			Water,
			Test
		};

		struct Particle {
			ParticleType type;
			bool updated;
		};

	private:
		std::unique_ptr<Particle> grid;
		std::unique_ptr<ParticleType> byteBuffer;
		std::unique_ptr<UI> ui;

		ShaderProgram particlesShader;
		ShaderProgram obstaclesShader;
		RenderTargetArray particlesTargets;

		GLuint textureColorBuffer;

		int brushRadius = 5;
		ParticleType selectedParticleType = Sand;

		void updateParticles(float* fluidVelocityBuffer);
		void updateParticlesTexture();

		bool inBounds(glm::ivec2 pos);
		bool isEmpty(glm::ivec2 pos);

		Particle particleGet(glm::ivec2 pos);
		void particleSet(glm::ivec2 pos, Particle particle);

		glm::ivec2 screenPosToGridPos(glm::vec2 screenPos);
		glm::vec2 sampleVelocityBuffer(float* fluidVelocityBuffer, glm::ivec2 pos);

		void selectParticleType(ParticleType type);
	public: 
		const glm::ivec2 simulationSize;

		ParticleSimulator(glm::ivec2 simulationSize);

		void update(float dt, float* fluidVelocityBuffer);
		void renderObstacles();
		void render();

		~ParticleSimulator();
};