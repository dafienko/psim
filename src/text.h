#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <glm/ext.hpp>
#include <string>
#include "font.h"

namespace Text {	
	/**
	 * @brief initialize text functions
	 * 
	 */
	void init();

	/**
	 * @brief render a string of text
	 * 
	 * @param text the text to render
	 * @param pos the baseline position to start rendering text at
	 * @param fontFace the font face to be used
	 * @param fontSize the size of the font 
	 * @param textColor the color of the text
	 */
	void renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace, unsigned int fontSize, glm::vec3 textColor);

	/**
	 * @brief Compute how much space it would take to render a string of text
	 * 
	 * @param text the text to compute bounds for
	 * @param fontFace the font face to be used
	 * @param fontSize the size of the font
	 * @return glm::ivec2 
	 */
	glm::ivec2 getTextBounds(const std::string &text, FontFace fontFace, unsigned int fontSize);
	void destroy();
}