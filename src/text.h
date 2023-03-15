#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <glm/ext.hpp>
#include <string>
#include "font.h"

namespace Text {	
	void init();
	void renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace, unsigned int fontSize, glm::vec3 textColor);
	glm::ivec2 getTextBounds(const std::string &text, FontFace fontFace, unsigned int fontSize);
	void destroy();
}