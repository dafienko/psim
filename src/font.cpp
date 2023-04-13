#include <iostream>

#include "font.h"
#include "text.h"

static std::string FONT_FACE_FILENAMES[] = {
	"fonts/MesloLGS NF Regular.ttf"
};

class Glyphsheet { // 16x8 spritesheet for character glyphs
	public: 
		//! How many rows of glyphs there are in the glyphsheet texture
		static const int GLYPH_SHEET_ROWS = 8;
		//! How many columns of glyphs there are in the glyphsheet texture
		static const int GLYPH_SHEET_COLS = 16;

		//! How wide in pixels the Glyphsheet is
		const unsigned int width;
		//! How tall in pixels the Glyphsheet is
		const unsigned int height;

		//! How wide in pixels an individual glyph is
		const unsigned int glyphWidth;
		//! How tall in pixels an individual glyph is
		const unsigned int glyphHeight;

	private:
		std::unique_ptr<unsigned char> buffer;

	public:
		/**
		 * @brief Create glyphsheet for storing glyphs with the specified glyph size
		 * 
		 * @param glyphWidth 
		 * @param glyphHeight 
		 */
		Glyphsheet(unsigned int glyphWidth, unsigned int glyphHeight) : 
			width(glyphWidth * GLYPH_SHEET_COLS),
			height(glyphHeight * GLYPH_SHEET_ROWS),
			glyphWidth(glyphWidth), 
			glyphHeight(glyphHeight),
			buffer(new unsigned char[width * height])
		{
			memset(buffer.get(), 0, width * height * sizeof(unsigned char));
		}

		/**
		 * @brief Buffer the glyph bitmap for the specified character
		 * 
		 * @param c The character the bitmap is for
		 * @param bitmapBuffer The bitmap pixels 
		 * @param bitmapWidth The width of the bitmap 
		 * @param bitmapHeight The height of the bitmap
		 * @return glm::vec4 
		 */
		glm::vec4 bufferGlyphBitmap(unsigned char c, unsigned char* bitmapBuffer, unsigned int bitmapWidth, unsigned int bitmapHeight) {
			assert(bitmapWidth <= glyphWidth);
			assert(bitmapHeight <= glyphHeight);
			assert(static_cast<int>(c) < 128);

			glm::ivec2 glyphSheetPos(
				static_cast<int>(c) % GLYPH_SHEET_COLS, 
				static_cast<int>(c) / GLYPH_SHEET_COLS
			);

			for (unsigned int y = 0; y < bitmapHeight; y++) {
				int bufferRow = glyphSheetPos.y * glyphHeight + y; 
				unsigned char* glyphRowStart = buffer.get() + bufferRow * width + glyphSheetPos.x * glyphWidth;
				unsigned char* bmpRowStart = bitmapBuffer + y * bitmapWidth;
				memcpy(glyphRowStart, bmpRowStart, bitmapWidth * sizeof(unsigned char));
			}

			return glm::vec4(
				glyphSheetPos.x * glyphWidth, glyphSheetPos.y * glyphHeight, 
				glyphSheetPos.x * glyphWidth + bitmapWidth + 1, glyphSheetPos.y * glyphHeight + bitmapHeight + 1
			);
		}

		/**
		 * @brief Get the raw glyphsheet buffer
		 * 
		 * @return unsigned* 
		 */
		unsigned char* getBuffer() const {
			return buffer.get();
		}

		/**
		 * @brief Get the size in pixels of the buffer
		 * 
		 * @return glm::ivec2 
		 */
		glm::ivec2 getSize() const {
			return glm::ivec2(width, height);
		}
};


Font::Font(FontFace fontFace, unsigned int fontSize, FT_Library& ft) : 
	fontSize(fontSize), 
	fontFace(fontFace) 
{
	const std::string &filename = FONT_FACE_FILENAMES[fontFace];

	FT_Face face;
	if (FT_New_Face(ft, filename.c_str(), 0, &face))
	{
		std::cerr << "Freetype failed to load font " << filename << std::endl;  
		exit(EXIT_FAILURE);
	}

	glm::ivec2 maxGlyphSize(
		(face->bbox.xMax - face->bbox.xMin) >> 6,
		(face->bbox.yMax - face->bbox.yMin) >> 6
	);

	Glyphsheet glyphsheet(maxGlyphSize.x, maxGlyphSize.y);
	glyphsheetSize = glyphsheet.getSize();

	FT_Set_Pixel_Sizes(face, 0, fontSize);  
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "Freetype failed to load glyph " << filename << " " << c << std::endl;
			continue;
		}

		Glyph glyph = {};
		glyph.advance = (unsigned int)(face->glyph->advance.x >> 6);
		glyph.tlOffset = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		glyph.glyphSize = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);

		FT_Bitmap& bmp = face->glyph->bitmap;
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

glm::ivec2 Font::getGlyphsheetSize() const {
	return glyphsheetSize;
}

Font::~Font() {

}