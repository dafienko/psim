#pragma once

#include <glm/ext.hpp>
#include <memory>

#include "shader.h"

namespace Quad {
	extern std::unique_ptr<ShaderProgram> quadShaderProgram;

	void init();
	void render();
	void render(glm::vec2 tl, glm::vec2 br);
	glm::vec2 getAspectTL(float aspect);
	glm::vec2 getAspectSize(float aspect);
	void renderAspect(float aspect);
	void destroy();
}