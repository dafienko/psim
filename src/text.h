#pragma once

#include <glm/ext.hpp>
#include <string>
#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

namespace Text {
	FT_Library ft;
	
	void init();
	void renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace);
	void destroy();
}