#pragma once

#include <glm/ext.hpp>
#include <memory>

#include "shader.h"

namespace Quad {
	extern std::unique_ptr<ShaderProgram> quadShaderProgram;

	void init();
	void render();
	void render(glm::vec2 tl, glm::vec2 br);
	void renderAspect(float aspect);
	void destroy();
}