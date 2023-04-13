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
		bool jetStreamsEnabled = false;

		void updatePressure(float dt);
		void addParticleTextures();
		void solve(float dt);
		void advect(float dt);
	public:
		//! the simulation size of the simulator
		const glm::ivec2 simulationSize;

		/**
		 * @brief Construct a new Fluid Simulator object
		 * 
		 * @param simulationSize 
		 */
		explicit FluidSimulator(glm::ivec2 simulationSize);

		/**
		 * @brief Run a simulation step
		 * 
		 * @param dt 
		 */
		void update(float dt);

		/**
		 * @brief Bind and expose obstacles texture to be drawn to so that the fluid simulator can respond to external obstacles
		 * 
		 */
		void bindObstaclesTexture();

		/**
		 * @brief Get the particle velocity texture so the fluid velocity may be manipulated externally
		 * 
		 * @return GLuint 
		 */
		GLuint getParticleVelocityTexture();

		/**
		 * @brief Get the particle density texture so the fluid density may be manipulated externally
		 * 
		 * @return GLuint 
		 */
		GLuint getParticleDensityTexture();

		/**
		 * @brief Get the current fluid velocity grid (meant for reading, writing to this does nothing)
		 * 
		 * @return float* 
		 */
		float* getVelocityBuffer();

		void render() override;
};