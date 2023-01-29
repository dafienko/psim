#include <iostream>

#include "font.h"

extern FT_Library Text::ft;

static std::string FONT_FACE_FILENAMES[] = {
	"fonts/consola.ttf"
};

Font::Font(FontFace fontFace, unsigned int fontSize) : 
	fontFace(fontFace), 
	fontSize(fontSize) 
{
	const std::string &filename = FONT_FACE_FILENAMES[fontFace];

	FT_Face face;
	if (FT_New_Face(Text::ft, filename.c_str(), 0, &face))
	{
		std::cerr << "Freetype failed to load font " << filename << std::endl;  
		exit(EXIT_FAILURE);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "Freetype failed to load glyph " << filename << " " << c << std::endl;
			continue;
		}
		
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Glyph glyph = {
			texture, 
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x >> 6
		};
		
		glyphData[c] = glyph;
	}
	
	FT_Done_Face(face);
	FT_Done_FreeType(Text::ft);
}