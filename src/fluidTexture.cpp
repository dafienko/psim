#include "fluidTexture.h"
#include "core.h"
#include <iostream>

FluidTexture::FluidTexture(glm::ivec2 simulationSize) : 
	simulationSize(simulationSize)
{
	fluidShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/fluid.fs");
	gridSolveShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/solve.fs");
	advectVelocityShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/velocityAdvect.fs");
	advectDensityShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/densityAdvect.fs");
	obstaclesShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/obstacles.fs");
	visualShader = std::make_unique<ShaderProgram>("shaders/grid.vsh", "shaders/fluidVisual.fs");

	physicsTargets = std::make_unique<RenderTargetArray>(2, simulationSize.x + 1, simulationSize.y + 1, GL_RGBA32F, GL_NEAREST);
	densityTargets = std::make_unique<RenderTargetArray>(2, simulationSize.x, simulationSize.y, GL_RGB, GL_NEAREST);
	obstaclesTargets = std::make_unique<RenderTargetArray>(2, simulationSize.x, simulationSize.y, GL_RED, GL_NEAREST);
	
	pressureTarget = std::make_unique<RenderTarget>(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST);
	visualTarget = std::make_unique<RenderTarget>(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST);
	
	// initialize obstacles
	obstaclesShader->bind();
	obstaclesTargets->bind();
	
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
	
	obstaclesTargets->bindAsTexture("oldObstacles", obstaclesShader->getProgram());
	obstaclesTargets->bind();
	
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
	
	physicsTargets->bindAsTexture("velTexture", fluidShader->getProgram());
	obstaclesTargets->bindAsTexture("obstaclesTexture", fluidShader->getProgram(), 1);

	pressureTarget->bind();

	glUniform2f(
		glGetUniformLocation(fluidShader->getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	RenderTarget::renderQuad();
}

void FluidTexture::solve(float dt) {
	gridSolveShader->bind();

	physicsTargets->bindAsTexture("velTexture", gridSolveShader->getProgram());
	obstaclesTargets->bindAsTexture("obstaclesTexture", gridSolveShader->getProgram(), 1);
	pressureTarget->bindAsTexture("pressureTexture", gridSolveShader->getProgram(), 2);

	physicsTargets->bind();

	glUniform2f(
		glGetUniformLocation(gridSolveShader->getProgram(), "simulationSize"), 
		simulationSize.x + 1, simulationSize.y + 1
	);
	
	RenderTarget::renderQuad();
}

void FluidTexture::advect(float dt) {
	// advect velocity
	advectVelocityShader->bind();

	physicsTargets->bindAsTexture("velTexture", advectVelocityShader->getProgram());
	obstaclesTargets->bindAsTexture("obstaclesTexture", advectVelocityShader->getProgram(), 1);

	physicsTargets->bind();

	glUniform2f(
		glGetUniformLocation(advectVelocityShader->getProgram(), "simulationSize"), 
		simulationSize.x + 1, simulationSize.y + 1
	);
	
	RenderTarget::renderQuad();

	// advect density
	advectDensityShader->bind();

	physicsTargets->bindAsTexture("velTexture", advectDensityShader->getProgram());
	obstaclesTargets->bindAsTexture("obstaclesTexture", advectDensityShader->getProgram(), 1);
	densityTargets->bindAsTexture("densityTexture", advectDensityShader->getProgram(), 3);

	densityTargets->bind();

	glUniform2f(
		glGetUniformLocation(advectDensityShader->getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);
	
	RenderTarget::renderQuad();
}

void FluidTexture::update(float dt) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	updateObstacles(dt);
	for (int i = 0; i < 40; i++) {
		updatePressure(dt);
		solve(dt);
	}
	
	advect(dt);
	
}

void FluidTexture::render() {
	glDisable(GL_DEPTH_TEST);

	visualShader->bind();

	physicsTargets->bindAsTexture("velTexture", visualShader->getProgram());
	obstaclesTargets->bindAsTexture("obstaclesTexture", visualShader->getProgram(), 1);
	pressureTarget->bindAsTexture("pressureTexture", visualShader->getProgram(), 2);
	densityTargets->bindAsTexture("densityTexture", visualShader->getProgram(), 3);

	glUniform2f(
		glGetUniformLocation(visualShader->getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	RenderTarget::renderQuad();
}

FluidTexture::~FluidTexture() {
	
}
