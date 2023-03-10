#include <iostream>

#include "fluidsimulator.h"
#include "core.h"
#include "quad.h"

FluidSimulator::FluidSimulator(glm::ivec2 simulationSize) : 
	fluidShader("shaders/grid.vsh", "shaders/fluid/fluid.fs"),
	gridSolveShader("shaders/grid.vsh", "shaders/fluid/solve.fs"),
	advectVelocityShader("shaders/grid.vsh", "shaders/fluid/velocityAdvect.fs"),
	advectDensityShader("shaders/grid.vsh", "shaders/fluid/densityAdvect.fs"),
	obstaclesShader("shaders/grid.vsh", "shaders/fluid/obstacles.fs"),
	visualShader("shaders/grid.vsh", "shaders/fluid/fluidVisual.fs"),
	
	physicsTargets(2, simulationSize.x + 1, simulationSize.y + 1, GL_RGBA32F, GL_NEAREST),
	obstaclesTargets(2, simulationSize.x, simulationSize.y, GL_RED, GL_NEAREST),
	densityTargets(2, simulationSize.x, simulationSize.y, GL_RGB, GL_NEAREST),
	
	pressureTarget(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST),
	visualTarget(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST),
	
	simulationSize(simulationSize)
{
	obstaclesShader.bind();
	obstaclesTargets.bind();
	
	glUniform2f(
		glGetUniformLocation(obstaclesShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform2f(
		glGetUniformLocation(obstaclesShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform1i(
		glGetUniformLocation(obstaclesShader.getProgram(), "init"),
		1
	);

	Quad::render();
}

void FluidSimulator::updateObstacles(float dt) {
	obstaclesShader.bind();
	
	obstaclesTargets.bindAsTexture("oldObstacles", obstaclesShader.getProgram());
	obstaclesTargets.bind();
	
	glUniform2f(
		glGetUniformLocation(obstaclesShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform2f(
		glGetUniformLocation(obstaclesShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform1i(
		glGetUniformLocation(obstaclesShader.getProgram(), "init"),
		0
	);

	Quad::render();
}

void FluidSimulator::updatePressure(float dt) {
	fluidShader.bind();
	
	physicsTargets.bindAsTexture("velTexture", fluidShader.getProgram());
	obstaclesTargets.bindAsTexture("obstaclesTexture", fluidShader.getProgram(), 1);

	pressureTarget.bind();

	glUniform2f(
		glGetUniformLocation(fluidShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform2f(
		glGetUniformLocation(fluidShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	Quad::render();
}

void FluidSimulator::solve(float dt) {
	gridSolveShader.bind();

	physicsTargets.bindAsTexture("velTexture", gridSolveShader.getProgram());
	obstaclesTargets.bindAsTexture("obstaclesTexture", gridSolveShader.getProgram(), 1);
	pressureTarget.bindAsTexture("pressureTexture", gridSolveShader.getProgram(), 2);

	physicsTargets.bind();

	glUniform2f(
		glGetUniformLocation(gridSolveShader.getProgram(), "gridSize"), 
		simulationSize.x + 1, simulationSize.y + 1
	);

	glUniform2f(
		glGetUniformLocation(gridSolveShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);
	
	Quad::render();
}

void FluidSimulator::advect(float dt) {
	// advect velocity
	advectVelocityShader.bind();

	physicsTargets.bindAsTexture("velTexture", advectVelocityShader.getProgram());
	obstaclesTargets.bindAsTexture("obstaclesTexture", advectVelocityShader.getProgram(), 1);

	physicsTargets.bind();

	glUniform2f(
		glGetUniformLocation(advectVelocityShader.getProgram(), "gridSize"), 
		simulationSize.x + 1, simulationSize.y + 1
	);

	glUniform2f(
		glGetUniformLocation(advectVelocityShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);
	
	Quad::render();

	// advect density
	advectDensityShader.bind();

	physicsTargets.bindAsTexture("velTexture", advectDensityShader.getProgram());
	obstaclesTargets.bindAsTexture("obstaclesTexture", advectDensityShader.getProgram(), 1);
	densityTargets.bindAsTexture("densityTexture", advectDensityShader.getProgram(), 3);

	densityTargets.bind();

	glUniform2f(
		glGetUniformLocation(advectDensityShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform2f(
		glGetUniformLocation(advectDensityShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);
	
	Quad::render();
}

void FluidSimulator::update(float dt) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	updateObstacles(dt);
	for (int i = 0; i < 40; i++) {
		updatePressure(dt);
		solve(dt);
	}
	
	advect(dt);
	
}

void FluidSimulator::render() {
	glDisable(GL_DEPTH_TEST);

	visualShader.bind();

	physicsTargets.bindAsTexture("velTexture", visualShader.getProgram());
	obstaclesTargets.bindAsTexture("obstaclesTexture", visualShader.getProgram(), 1);
	pressureTarget.bindAsTexture("pressureTexture", visualShader.getProgram(), 2);
	densityTargets.bindAsTexture("densityTexture", visualShader.getProgram(), 3);

	glUniform2f(
		glGetUniformLocation(visualShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform2f(
		glGetUniformLocation(visualShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	Quad::render();
}