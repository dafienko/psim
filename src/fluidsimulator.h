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

		RenderTarget pressureTarget;
		RenderTarget visualTarget;
		
		std::unique_ptr<float> velocityBuffer;

		int fluidRenderMode = 0;

		void updatePressure(float dt);
		void solve(float dt);
		void advect(float dt);
	public:
		const glm::ivec2 simulationSize;

		FluidSimulator(glm::ivec2 simulationSize);

		void update(float dt);
		void bindObstaclesTexture();
		float* getVelocityBuffer();
		void render() override;
};