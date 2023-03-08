#pragma once

#include <glm/ext.hpp>
#include <memory>
#include <string>
#include <map>
#include <GL/glew.h>

enum FontFace {
	Consola,
};

struct Glyph {
	glm::vec2 texTL, texBR;
	glm::ivec2 glyphSize;	
	glm::ivec2 tlOffset;
	unsigned int advance;
};

class Font {
	private:
		std::map<char, Glyph> glyphData;
		GLuint glyphTexture;
	public:
		const unsigned int fontSize;
		const FontFace fontFace;

		Font(FontFace fontFace, unsigned int fontSize);
		~Font();

		Glyph getGlyph(char c) const;
		GLuint getGlyphTexture() const;
};