#pragma once

#include <glm/ext.hpp>

#include "uiobject.h"
#include "font.h"

class UIText: public UIObject {
	public: 
		UIText();
		explicit UIText(json& data);

		std::string text = "";
		glm::vec3 textColor = glm::vec3(1);
		float textTransparency = 0;
		FontFace font = FontFace::Consola;
		unsigned int textSize = 20;
		glm::vec2 textAnchorPoint = glm::vec2(.5);
		glm::vec2 textPadding = glm::vec2(0);

		void render() override;
};