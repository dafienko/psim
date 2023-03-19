#pragma once 

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>

#include "rendertarget.h"
#include "rendertargetarray.h"
#include "shader.h"
#include "renderable.h"

class FluidSimulator: public Renderable {
	private: 
		ShaderProgram fluidShader;
		ShaderProgram gridSolveShader;
		ShaderProgram advectVelocityShader;
		ShaderProgram advectDensityShader;
		ShaderProgram visualShader;

		RenderTargetArray physicsTargets;
		RenderTargetArray obstaclesTargets;
		RenderTargetArray densityTargets;

		RenderTarget particleDensityTarget;
		RenderTarget particleVelocityTarget;

		RenderTarget pressureTarget;
		RenderTarget visualTarget;
		
		std::unique_ptr<float> velocityBuffer;

		int fluidRenderMode = 0;

		void updatePressure(float dt);
		void addParticleTextures();
		void solve(float dt);
		void advect(float dt);
	public:
		const glm::ivec2 simulationSize;

		explicit FluidSimulator(glm::ivec2 simulationSize);

		void update(float dt);
		void bindObstaclesTexture();
		GLuint getParticleVelocityTexture();
		GLuint getParticleDensityTexture();
		float* getVelocityBuffer();
		void render() override;
};