#pragma once

#include <glm/ext.hpp>

#include "uiobject.h"
#include "font.h"

class UIText: public UIObject {
	public: 
		/**
		 * @brief Construct a new UIText object
		 * 
		 */
		UIText();

		/**
		 * @brief Construct a new UIText object from json data
		 * 
		 * @param data the json data to construct this object from
		 */
		explicit UIText(json& data);

		//! The text to be rendered in this object
		std::string text = "";
		//! The color of the rendered text
		glm::vec3 textColor = glm::vec3(1);
		//! The transparency of the rendered text
		float textTransparency = 0;
		//! The font face to render text with
		FontFace font = FontFace::Consola; 
		//! The text size of the rendered text size
		unsigned int textSize = 20;
		//! Where to anchor the text inside of this object
		glm::vec2 textAnchorPoint = glm::vec2(.5);
		//! How much to pad the edges of this object for text rendering
		glm::vec2 textPadding = glm::vec2(0);

		void render() override;
};