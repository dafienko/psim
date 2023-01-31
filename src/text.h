#pragma once

#include <glm/ext.hpp>
#include <string>
#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

namespace Text {
	extern FT_Library ft;
	
	void init();
	void renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace, unsigned int fontSize, glm::vec3 textColor);
	void destroy();
}