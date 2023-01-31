#include <iostream>

#include "text.h"
#include "core.h"
#include "shader.h"

FT_Library Text::ft;

GLuint textVAO, textVBO;

std::unique_ptr<Font> testFont;
std::unique_ptr<ShaderProgram> glyphShader;

void Text::init() {
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "Freetype failed to initialize" << std::endl;
		exit(EXIT_FAILURE);
	}

	glGenVertexArrays(1, &textVAO);
	glBindVertexArray(textVAO);
	
	glGenBuffers(1, &textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	testFont = std::make_unique<Font>(FontFace::Consola, 40);

	glyphShader = std::make_unique<ShaderProgram>("shaders/textGlyph.vsh", "shaders/textGlyph.fs");
}

void Text::renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace, glm::vec3 textColor) {
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	glyphShader->bind();

	glBindTexture(GL_TEXTURE_2D, testFont->getGlyphTexture());

	glm::mat4x4 ortho = glm::ortho(0.0f, (float)Core::screenWidth, (float)Core::screenHeight, 0.0f);
	GLint projLoc = glGetUniformLocation(glyphShader->getProgram(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &ortho[0][0]);

	GLint tcLoc = glGetUniformLocation(glyphShader->getProgram(), "textColor");
	glUniform3fv(tcLoc, 1, &textColor.r);

	glBindVertexArray(textVAO);
	
	const Font &font = *testFont.get();
	glm::ivec2 textPos = pos;
	for (char c : text) {
		const Glyph glyph = font.getGlyph(c);

		glm::ivec2 tl = textPos + glm::ivec2(glyph.tlOffset.x, -glyph.tlOffset.y);
		glm::ivec2 size = glyph.glyphSize;

		glm::vec2 tlf((float) tl.x, (float) tl.y);
		glm::vec2 brf = tlf + glm::vec2((float) size.x, (float) size.y);

		float positions[4][4] = {
			{tlf.x, tlf.y, glyph.texTL.x, glyph.texTL.y},
			{brf.x, tlf.y, glyph.texBR.x, glyph.texTL.y},
			{tlf.x, brf.y, glyph.texTL.x, glyph.texBR.y},
			{brf.x, brf.y, glyph.texBR.x, glyph.texBR.y},
		};

		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, positions, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		textPos += glm::ivec2(glyph.advance, 0);
	}

	glBindVertexArray(0);
}

void Text::destroy() {
	testFont.release();
	glyphShader.release();
	
	FT_Done_FreeType(ft);
}

