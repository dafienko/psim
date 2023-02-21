#pragma once 

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <memory>

#include "rendertarget.h"
#include "shader.h"

class FluidTexture {
	private: 
		std::unique_ptr<ShaderProgram> fluidShader;
		std::unique_ptr<ShaderProgram> gridSolveShader;
		std::unique_ptr<ShaderProgram> advectShader;
		std::unique_ptr<ShaderProgram> obstaclesShader;
		std::unique_ptr<ShaderProgram> visualShader;

		std::unique_ptr<RenderTarget> physicsTargets[2];
		unsigned int currentPhysicalTarget;
		
		std::unique_ptr<RenderTarget> obstaclesTargets[2];
		unsigned int currentObstacleTarget;

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
		void render();
};