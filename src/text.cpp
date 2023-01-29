#include <iostream>

#include "text.h"

FT_Library Text::ft;

void Text::init() {
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "Freetype failed to initialize" << std::endl;
		exit(EXIT_FAILURE);
	}

	
}

void Text::renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace) {
	
}

void Text::destroy() {

}