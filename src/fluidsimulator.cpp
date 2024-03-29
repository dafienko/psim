#include "fluidsimulator.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include "core.h"
#include "quad.h"
#include "uitext.h"

FluidSimulator::FluidSimulator(glm::ivec2 simulationSize) : 
	fluidShader("shaders/grid.vsh", "shaders/fluid/fluid.fs"),
	gridSolveShader("shaders/grid.vsh", "shaders/fluid/solve.fs"),
	advectVelocityShader("shaders/grid.vsh", "shaders/fluid/velocityAdvect.fs"),
	advectDensityShader("shaders/grid.vsh", "shaders/fluid/densityAdvect.fs"),
	visualShader("shaders/grid.vsh", "shaders/fluid/fluidVisual.fs"),
	
	physicsTargets(2, simulationSize.x + 1, simulationSize.y + 1, GL_RGBA32F, GL_NEAREST),
	obstaclesTargets(2, simulationSize.x, simulationSize.y, GL_RED, GL_NEAREST),
	densityTargets(2, simulationSize.x, simulationSize.y, GL_RGB32F, GL_NEAREST),
	
	particleDensityTarget(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST),
	particleVelocityTarget(simulationSize.x + 1, simulationSize.y + 1, GL_RGBA32F, GL_NEAREST),

	pressureTarget(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST),
	visualTarget(simulationSize.x, simulationSize.y, GL_RGBA32F, GL_NEAREST),

	velocityBuffer(new float[(simulationSize.x + 1) * 3 * (simulationSize.y + 1)]),
	
	simulationSize(simulationSize)
{
	Core::keyEvent->bind([&] (int key, int action, int mods) {
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_UP) {
				fluidRenderMode += 1;
				fluidRenderMode %= 3;
			} else if (key == GLFW_KEY_DOWN) {
				fluidRenderMode += 2;
				fluidRenderMode %= 3;
			} else if (key == GLFW_KEY_S) {
				jetStreamsEnabled = !jetStreamsEnabled;
			}

			static const std::string MODE_LABELS[] = {
				"Fluid Render Mode: Density",
				"Fluid Render Mode: Pressure",
				"Fluid Render Mode: Velocity"
			};

			UIText* renderModeLabel = dynamic_cast<UIText*>(Core::mainUI->getChild("renderMode"));
			renderModeLabel->text = MODE_LABELS[fluidRenderMode];
		}
	});
}

float* FluidSimulator::getVelocityBuffer() {
	physicsTargets.bindAsTexture();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, velocityBuffer.get());

	return velocityBuffer.get();
}

GLuint FluidSimulator::getParticleVelocityTexture() {
	return particleVelocityTarget.getTexture();
}

GLuint FluidSimulator::getParticleDensityTexture() {
	return particleDensityTarget.getTexture();
}

void FluidSimulator::bindObstaclesTexture() {
	obstaclesTargets.bind();
	glClearColor(1, 0, 0, 1);
	obstaclesTargets.clear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
}

void FluidSimulator::addParticleTextures() {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	Quad::quadShaderProgram->bind();
	physicsTargets.bindOld();
	particleVelocityTarget.bindAsTexture("screenTexture", Quad::quadShaderProgram->getProgram(), 0);
	Quad::render();

	densityTargets.bindOld();
	particleDensityTarget.bindAsTexture("screenTexture", Quad::quadShaderProgram->getProgram(), 0);
	Quad::render();

	RenderTarget::bindDefault();
}

void FluidSimulator::updatePressure(float dt) {
	fluidShader.bind();
	
	physicsTargets.bindAsTexture("velTexture", fluidShader.getProgram());
	obstaclesTargets.bindAsTexture("obstaclesTexture", fluidShader.getProgram(), 1);

	pressureTarget.bind();

	glUniform1i(
		glGetUniformLocation(fluidShader.getProgram(), "JET_STREAMS_ENABLED"), 
		jetStreamsEnabled
	);

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

	glUniform1i(
		glGetUniformLocation(gridSolveShader.getProgram(), "JET_STREAMS_ENABLED"), 
		jetStreamsEnabled
	);

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

	glUniform1i(
		glGetUniformLocation(advectVelocityShader.getProgram(), "JET_STREAMS_ENABLED"), 
		jetStreamsEnabled
	);

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

	glUniform1i(
		glGetUniformLocation(advectDensityShader.getProgram(), "JET_STREAMS_ENABLED"), 
		jetStreamsEnabled
	);

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
	addParticleTextures();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

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

	glUniform1i(
		glGetUniformLocation(visualShader.getProgram(), "JET_STREAMS_ENABLED"), 
		jetStreamsEnabled
	);

	glUniform2f(
		glGetUniformLocation(visualShader.getProgram(), "gridSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform2f(
		glGetUniformLocation(visualShader.getProgram(), "simulationSize"), 
		simulationSize.x, simulationSize.y
	);

	glUniform1i(
		glGetUniformLocation(visualShader.getProgram(), "renderMode"), 
		fluidRenderMode
	);

	Quad::renderAspect(static_cast<float>(simulationSize.x) / static_cast<float>(simulationSize.y));
}