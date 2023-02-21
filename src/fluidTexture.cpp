#include "fluidTexture.h"
#include <iostream>

FluidTexture::FluidTexture(glm::ivec2 simulationSize) : 
	simulationSize(simulationSize)
{
	fluidShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/fluid.fs");
	gridSolveShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/solve.fs");
	advectShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/advect.fs");
	obstaclesShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/obstacles.fs");
	visualShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/fluidVisual.fs");

	physicsTargets[0] = std::make_unique<RenderTarget>(simulationSize.x + 1, simulationSize.y + 1, GL_RGBA32F, GL_NEAREST);
	physicsTargets[1] = std::make_unique<RenderTarget>(simulationSize.x + 1, simulationSize.y + 1, GL_RGBA32F, GL_NEAREST);

	pressureTarget = std::make_unique<RenderTarget>(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST);
	visualTarget = std::make_unique<RenderTarget>(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST);
	
	obstaclesTargets[0] = std::make_unique<RenderTarget>(simulationSize.x, simulationSize.y, GL_RED, GL_NEAREST);
	obstaclesTargets[1] = std::make_unique<RenderTarget>(simulationSize.x, simulationSize.y, GL_RED, GL_NEAREST);

	currentPhysicalTarget = 0;

	// initialize obstacles
	currentObstacleTarget = 0;
	obstaclesShader->bind();
	obstaclesTargets[currentObstacleTarget]->bind();
	
	glUniform2f(
		glGetUniformLocation(obstaclesShader->getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform1i(
		glGetUniformLocation(obstaclesShader->getProgram(), "init"),
		1
	);

	RenderTarget::renderQuad();
}

void FluidTexture::updateObstacles(float dt) {
	obstaclesShader->bind();
	
	obstaclesTargets[currentObstacleTarget]->bindAsTexture("oldObstacles", obstaclesShader->getProgram());
	
	unsigned int targetIndex = (currentObstacleTarget + 1) % 2;
	obstaclesTargets[targetIndex]->bind();
	currentObstacleTarget = targetIndex;
	
	glUniform2f(
		glGetUniformLocation(obstaclesShader->getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform1i(
		glGetUniformLocation(obstaclesShader->getProgram(), "init"),
		0
	);

	RenderTarget::renderQuad();
}

void FluidTexture::updatePressure(float dt) {
	fluidShader->bind();
	
	physicsTargets[currentPhysicalTarget]->bindAsTexture("velTexture", fluidShader->getProgram());
	obstaclesTargets[currentObstacleTarget]->bindAsTexture("obstaclesTexture", fluidShader->getProgram(), 1);

	pressureTarget->bind();

	glUniform2f(
		glGetUniformLocation(fluidShader->getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	RenderTarget::renderQuad();
}

void FluidTexture::solve(float dt) {
	gridSolveShader->bind();

	physicsTargets[currentPhysicalTarget]->bindAsTexture("velTexture", gridSolveShader->getProgram());
	obstaclesTargets[currentObstacleTarget]->bindAsTexture("obstaclesTexture", gridSolveShader->getProgram(), 1);
	pressureTarget->bindAsTexture("pressureTexture", gridSolveShader->getProgram(), 2);

	unsigned int targetIndex = (currentPhysicalTarget + 1) % 2;
	physicsTargets[targetIndex]->bind();
	currentPhysicalTarget = targetIndex;

	glUniform2f(
		glGetUniformLocation(gridSolveShader->getProgram(), "simulationSize"), 
		simulationSize.x + 1, simulationSize.y + 1
	);
	
	RenderTarget::renderQuad();
}

void FluidTexture::advect(float dt) {
	advectShader->bind();

	physicsTargets[currentPhysicalTarget]->bindAsTexture("velTexture", advectShader->getProgram());
	obstaclesTargets[currentObstacleTarget]->bindAsTexture("obstaclesTexture", advectShader->getProgram(), 1);

	unsigned int targetIndex = (currentPhysicalTarget + 1) % 2;
	physicsTargets[targetIndex]->bind();
	currentPhysicalTarget = targetIndex;

	glUniform2f(
		glGetUniformLocation(gridSolveShader->getProgram(), "simulationSize"), 
		simulationSize.x + 1, simulationSize.y + 1
	);
	
	RenderTarget::renderQuad();
}

void FluidTexture::update(float dt) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	updateObstacles(dt);
	for (int i = 0; i < 20; i++) {
		updatePressure(dt);
		solve(dt);
	}
	
	//advect(dt);
}

void FluidTexture::render() {
	glDisable(GL_DEPTH_TEST);

	visualShader->bind();

	physicsTargets[currentPhysicalTarget]->bindAsTexture("velTexture", visualShader->getProgram());
	obstaclesTargets[currentObstacleTarget]->bindAsTexture("obstaclesTexture", visualShader->getProgram(), 1);
	pressureTarget->bindAsTexture("pressureTexture", visualShader->getProgram(), 2);

	glUniform2f(
		glGetUniformLocation(visualShader->getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	RenderTarget::renderQuad();
}

FluidTexture::~FluidTexture() {
	
}
