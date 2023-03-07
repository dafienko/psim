#pragma once

#include <glm/ext.hpp>

namespace Quad {
	void init();
	void render(glm::vec2 tl, glm::vec2 br);
	void destroy();
}