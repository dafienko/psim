#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <glm/ext.hpp>
#include <memory>
#include <string>
#include <map>
#include <GL/glew.h>

enum FontFace {
	Consola,
};

struct Glyph {
	//! The coordinates of the top-left of the glyph bitmap in the glypsheet
	glm::vec2 texTL;
	//! The coordinates of the bottom-right of the glyph bitmap in the glypsheet
	glm::vec2 texBR;
	//! How large the glyph is in pixels
	glm::ivec2 glyphSize;
	//! How to offset the top-left of the glyph when drawing relative to the text baseline
	glm::ivec2 tlOffset;
	//! How many pixels to advance the next glyph after drawing this glyph
	unsigned int advance;
};

class Font {
	private:
		std::map<char, Glyph> glyphData;
		GLuint glyphTexture;
		glm::ivec2 glyphsheetSize;
	public:
		//! the font size used to create this font
		const unsigned int fontSize;

		//! the font face of this font
		const FontFace fontFace;

		/**
		 * @brief Create a new font
		 * 
		 * @param fontFace the font face to use for this font
		 * @param fontSize roughly how tall should each glyph be in pixels
		 * @param ft freetype library reference
		 */
		Font(FontFace fontFace, unsigned int fontSize, FT_Library& ft);

		~Font();

		/**
		 * @brief Get the glyph object for the specified char
		 * 
		 * @param c 
		 * @return the char of the glyph to be returned 
		 */
		Glyph getGlyph(char c) const;

		/**
		 * @brief Get the glyph texture object
		 * 
		 * @return GLuint 
		 */
		GLuint getGlyphTexture() const;

		/**
		 * @brief Get the pixel dimensions of the glyph sheet
		 * 
		 * @return glm::ivec2 
		 */
		glm::ivec2 getGlyphsheetSize() const;
};