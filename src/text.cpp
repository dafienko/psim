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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	testFont = std::make_unique<Font>(FontFace::Consola, 20);

	glyphShader = std::make_unique<ShaderProgram>("shaders/textGlyph.vsh", "shaders/textGlyph.fs");
}

void Text::renderText(const std::string &text, glm::ivec2 pos, FontFace fontFace) {
	glyphShader->bind();

	glm::mat4x4 ortho = glm::ortho(0, (int)Core::screenWidth, (int)Core::screenHeight, 0);
	GLint projLoc = glGetUniformLocation(glyphShader->getProgram(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &ortho[0][0]);

	glBindVertexArray(textVAO);
	
	const Font &font = *testFont.get();
	glm::ivec2 textPos = pos;
	for (char c : text) {
		const Glyph glyph = font.getGlyph(c);

		glm::ivec2 tl = textPos - glyph.tlOffset;
		glm::ivec2 size = glyph.glyphSize;


		glm::vec2 tlf((float) tl.x, (float) tl.y);
		glm::vec2 brf = tlf + glm::vec2((float) size.x, (float) size.y);

		float positions[6][4] = {
			{tlf.x, tlf.y, 0.0f, 0.0f},
			{tlf.x, brf.y, 0.0f, 1.0f},
			{brf.x, tlf.y, 1.0f, 0.0f},

			{brf.x, brf.y, 1.0f, 1.0f},
			{brf.x, tlf.y, 1.0f, 0.0f},
			{tlf.x, brf.y, 0.0f, 1.0f},
		};

		std::cout << c << "   " << size.x << ", " << size.y << "   ";
		std::cout << tlf.x << ", " << tlf.y << "   " << brf.x << ", " << brf.y << std::endl;

		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, positions, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		pos += glm::ivec2(glyph.advance, 0);
	}

	glBindVertexArray(0);
}

void Text::destroy() {
	testFont.release();
	glyphShader.release();
	
	FT_Done_FreeType(ft);
}

