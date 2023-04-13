#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <map>

#include "window.h"
#include "quad.h"
#include "text.h"
#include "core.h"
#include "shader.h"

static FT_Library ft;

std::unique_ptr<std::unordered_map<int, std::map<unsigned int, std::unique_ptr<Font>>>> fonts;
std::unique_ptr<ShaderProgram> glyphShader;

void Text::init() {
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "Freetype failed to initialize" << std::endl;
		exit(EXIT_FAILURE);
	}

	fonts = std::make_unique<std::unordered_map<int, std::map<unsigned int, std::unique_ptr<Font>>>>();

	glyphShader = std::make_unique<ShaderProgram>("shaders/textGlyph.vsh", "shaders/textGlyph.fs");
}

const Font& getFont(FontFace fontFace, unsigned int fontSize) {
	int fontId = static_cast<std::underlying_type<FontFace>::type>(fontFace);
	std::map<unsigned int, std::unique_ptr<Font>> &fontSizes = (*fonts.get())[fontId];
	
	try {
		return *fontSizes.at(fontSize).get();
	} catch (...) {
		fontSizes[fontSize] = std::make_unique<Font>(fontFace, fontSize, ft);
		return *fontSizes[fontSize].get();
	}
}

void Text::renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace, unsigned int fontSize, glm::vec3 textColor) {
	const Font &font = getFont(fontFace, fontSize);
	const glm::vec2 contentScale = Window::getContentScale();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	glyphShader->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font.getGlyphTexture());
	glUniform1i(glGetUniformLocation(glyphShader->getProgram(), "text"), 0);

	glm::ivec2 glyphsheetSize = font.getGlyphsheetSize();
	glUniform2f(
		glGetUniformLocation(glyphShader->getProgram(), "glyphsheetSize"),
		static_cast<float>(glyphsheetSize.x), static_cast<float>(glyphsheetSize.y)
	);

	glm::mat4x4 ortho = glm::ortho(
		0.0f, 
		static_cast<float>(Core::getWindowSize().x), 
		static_cast<float>(Core::getWindowSize().y), 
		0.0f
	);

	GLint projLoc = glGetUniformLocation(glyphShader->getProgram(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &ortho[0][0]);

	GLint tcLoc = glGetUniformLocation(glyphShader->getProgram(), "textColor");
	glUniform3fv(tcLoc, 1, &textColor.r);

	glm::ivec2 textPos = pos;
	for (char c : text) {
		if (c == '\n') {
			textPos.y += fontSize * 1.5 * contentScale.y;
			textPos.x = pos.x;
		} else {
			const Glyph glyph = font.getGlyph(c);

			glm::vec2 tl = glm::vec2(textPos) + glm::vec2(glyph.tlOffset.x, -glyph.tlOffset.y) * contentScale;
			glm::vec2 size = glm::vec2(glyph.glyphSize) * contentScale;

			glm::vec2 tlf(static_cast<float>(tl.x), static_cast<float>(tl.y));
			glm::vec2 brf = tlf + glm::vec2(static_cast<float>(size.x), static_cast<float>(size.y));

			glUniform2f(glGetUniformLocation(glyphShader->getProgram(), "glyphTL"), glyph.texTL.x, glyph.texTL.y);
			glUniform2f(glGetUniformLocation(glyphShader->getProgram(), "glyphBR"), glyph.texBR.x, glyph.texBR.y);

			Quad::render(tlf, brf);

			textPos += glm::ivec2(glyph.advance * contentScale.x, 0);
		}
	}

	glBindVertexArray(0);
}

glm::ivec2 Text::getTextBounds(const std::string &text, FontFace fontFace, unsigned int fontSize) {
	glm::ivec2 bounds(0, static_cast<int>(fontSize));
	const glm::vec2 contentScale = Window::getContentScale();

	const Font &font = getFont(fontFace, fontSize);
	int textPos = 0;
	for (char c : text) {
		if (c == '\n') {
			bounds.y += fontSize * 1.5 * contentScale.y;
			textPos = 0;
		} else {
			const Glyph glyph = font.getGlyph(c);
			bounds.x = std::max(bounds.x, static_cast<int>(textPos + glyph.glyphSize.x * contentScale.x));
			textPos += glyph.advance * contentScale.x;
		}
	}

	return bounds;
}

void Text::destroy() {
	fonts.release();
	glyphShader.release();
	
	FT_Done_FreeType(ft);
}

