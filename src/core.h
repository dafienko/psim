#pragma once

namespace Core {	
	extern unsigned int screenWidth, screenHeight;

	void init(unsigned int width, unsigned int height);
	void update(float dt);
	void render();
	void resize(unsigned int widht, unsigned int height);
	void destroy();
};

void checkGLErrors(const std::string &file, int line);

#define CHECK_GL_ERRORS checkGLErrors(std::string(__FILE__), __LINE__);
