#pragma once

#include <glm/ext.hpp>
#include <memory>
#include <string>
#include <map>
#include <GL/glew.h>

enum FontFace {
	Consola
};

class Font {
	private:
		struct Glyph {
			unsigned int id;
			glm::ivec2 glyphSize;	
			glm::ivec2 tlOffset;
			unsigned int advance;
		};

		std::map<char, Glyph> glyphData;
	public:
		const unsigned int fontSize;
		const FontFace fontFace;

		Font(FontFace fontFace, unsigned int fontSize);
		~Font();
};