#pragma once 

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>
#include <vector>
#include <optional>

#include "shader.h"
#include "rendertargetarray.h"
#include "renderable.h"
#include "ui.h"

class ParticleSimulator : public Renderable {
	private: 
		enum ParticleType {
			Air,
			Wall,
			Sand,
			Water,
			Wood,
			Fire
		};

		struct Particle {
			ParticleType type;
			bool updated;

			glm::vec2 cellularVelocity;

			bool simulateAsParticle;
			glm::vec2 particlePosition;
			glm::vec2 particleVelocity;
		};

	private:
		std::unique_ptr<Particle> grid;
		std::unique_ptr<ParticleType> byteBuffer;
		std::unique_ptr<float> addedFluidVelocityBuffer;
		std::unique_ptr<float> addedFluidDensityBuffer;
		std::unique_ptr<UI> ui;

		ShaderProgram particlesShader;
		ShaderProgram obstaclesShader;
		RenderTargetArray particlesTargets;

		GLuint textureColorBuffer;

		int brushRadius = 2;
		ParticleType selectedParticleType = Sand;

		void updateParticles(float* fluidVelocityBuffer);
		void updateParticlesTexture();

		bool inBounds(glm::ivec2 pos);
		bool isEmpty(glm::ivec2 pos);
		std::optional<ParticleType> isSwappable(glm::ivec2 pos, ParticleType replacementType);

		Particle& particleGet(glm::ivec2 pos) const;
		void particleSet(glm::ivec2 pos, Particle particle);
		void particleSwap(glm::ivec2 src, glm::ivec2 dest);

		glm::ivec2 screenPosToGridPos(glm::vec2 screenPos);
		glm::vec2 sampleVelocityBuffer(float* fluidVelocityBuffer, glm::ivec2 pos);

		void selectParticleType(ParticleType type);

		void clearFluidBuffers();
		void addFluidVelocity(glm::ivec2 pos, int r, float top, float bottom, float right, float left);
		void setFluidDensity(glm::ivec2 pos, int r, glm::ivec4 color);
		void updateFluidTextures(GLuint fluidVelocityTexture, GLuint fluidDensityTexture);
		void updateParticle(glm::ivec2, glm::vec2 fluidVel);
	public: 
		const glm::ivec2 simulationSize;

		explicit ParticleSimulator(glm::ivec2 simulationSize);

		void update(float dt, float* fluidVelocityBuffer, GLuint fluidVelocityTexture, GLuint fluidDensityTexture);
		void renderObstacles();
		void render() override;

		~ParticleSimulator();
};