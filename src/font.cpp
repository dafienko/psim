#include <iostream>

#include "font.h"
#include "text.h"

extern FT_Library Text::ft;

static std::string FONT_FACE_FILENAMES[] = {
	"fonts/MesloLGS NF Regular.ttf"
};

class Glyphsheet { // 16x8 spritesheet for character glyphs
	private:
		unsigned char* buffer;
	public: 
		const static int GLYPH_SHEET_ROWS = 8;
		const static int GLYPH_SHEET_COLS = 16;

		const unsigned int glyphWidth, glyphHeight;
		const unsigned int width, height;

		Glyphsheet(unsigned int glyphWidth, unsigned int glyphHeight) : 
			glyphWidth(glyphWidth), 
			glyphHeight(glyphHeight),
			width(glyphWidth * GLYPH_SHEET_COLS),
			height(glyphHeight * GLYPH_SHEET_ROWS)
		{
			buffer = new unsigned char[width * height]();
		}

		glm::vec4 bufferGlyphBitmap(unsigned char c, unsigned char* bitmapBuffer, int bitmapWidth, int bitmapHeight) {
			assert(bitmapWidth <= glyphWidth);
			assert(bitmapHeight <= glyphHeight);
			assert((int)c < 128);

			glm::ivec2 glyphSheetPos((int)c % GLYPH_SHEET_COLS, (int)c / GLYPH_SHEET_COLS);

			std::cout << (int)c << " " << glyphSheetPos.x << ", " << glyphSheetPos.y << std::endl;
			for (int y = 0; y < bitmapHeight; y++) {
				int bufferRow = glyphSheetPos.y * glyphHeight + y; 
				unsigned char* glyphRowStart = buffer + bufferRow * width + glyphSheetPos.x * glyphWidth;
				unsigned char* bmpRowStart = bitmapBuffer + y * bitmapWidth;
				memcpy(glyphRowStart, bmpRowStart, bitmapWidth * sizeof(unsigned char));
			}
		

			return glm::vec4(
				(float)(glyphSheetPos.x * glyphWidth) / width, (float)(glyphSheetPos.y * glyphHeight) / height, 
				(float)(glyphSheetPos.x * glyphWidth + bitmapWidth + 1) / width, (float)(glyphSheetPos.y * glyphHeight + bitmapHeight + 1) / height
			);
		}

		unsigned char* getBuffer() {
			return buffer;
		}

		~Glyphsheet() {
			delete[] buffer;
		}
};



Font::Font(FontFace fontFace, unsigned int fontSize) : 
	fontSize(fontSize), 
	fontFace(fontFace) 
{
	const std::string &filename = FONT_FACE_FILENAMES[fontFace];

	FT_Face face;
	if (FT_New_Face(Text::ft, filename.c_str(), 0, &face))
	{
		std::cerr << "Freetype failed to load font " << filename << std::endl;  
		exit(EXIT_FAILURE);
	}

	Glyphsheet glyphsheet(50, 50);

	FT_Set_Pixel_Sizes(face, 0, fontSize);  
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "Freetype failed to load glyph " << filename << " " << c << std::endl;
			continue;
		}

		Glyph glyph;
		glyph.advance = (unsigned int)(face->glyph->advance.x >> 6);
		glyph.tlOffset = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		glyph.glyphSize = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);

		FT_Bitmap bmp = face->glyph->bitmap;
		glm::vec4 bounds = glyphsheet.bufferGlyphBitmap(c, bmp.buffer, bmp.width, bmp.rows);
		glyph.texTL = glm::vec2(bounds.x, bounds.y);
		glyph.texBR = glm::vec2(bounds.z, bounds.w);
		
		glyphData[c] = glyph;
	}
	
	FT_Done_Face(face);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &glyphTexture);
	glBindTexture(GL_TEXTURE_2D, glyphTexture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		glyphsheet.width,
		glyphsheet.height,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		glyphsheet.getBuffer()
	);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Glyph Font::getGlyph(char c) const {
	return glyphData.at(c);
}

GLuint Font::getGlyphTexture() const {
	return glyphTexture;
}

Font::~Font() {

}