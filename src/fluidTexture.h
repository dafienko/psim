#pragma once 

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>

#include "rendertarget.h"
#include "rendertargetarray.h"
#include "shader.h"
#include "renderable.h"

class FluidTexture: public Renderable {
	private: 
		std::unique_ptr<ShaderProgram> fluidShader;
		std::unique_ptr<ShaderProgram> gridSolveShader;
		std::unique_ptr<ShaderProgram> advectVelocityShader;
		std::unique_ptr<ShaderProgram> advectDensityShader;
		std::unique_ptr<ShaderProgram> obstaclesShader;
		std::unique_ptr<ShaderProgram> visualShader;

		std::unique_ptr<RenderTargetArray> physicsTargets;
		std::unique_ptr<RenderTargetArray> obstaclesTargets;
		std::unique_ptr<RenderTargetArray> densityTargets;

		std::unique_ptr<RenderTarget> pressureTarget;
		std::unique_ptr<RenderTarget> visualTarget;
		
		void updateObstacles(float dt);
		void updatePressure(float dt);
		void solve(float dt);
		void advect(float dt);
	public:
		const glm::ivec2 simulationSize;

		FluidTexture(glm::ivec2 simulationSize);
		~FluidTexture();

		void update(float dt);
		void render() override;
};