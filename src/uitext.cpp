#include <memory>
#include <iostream>
#include <sstream>

#include "uitext.h"
#include "text.h"

UIText::UIText() :
	UIObject(InstanceClass::IC_UIText, "UIText"),
	textColor(glm::vec3(1, 1, 1)),
	font(FontFace::Consola),
	textSize((unsigned int)20)
{}

UIText::UIText(json& data) : 
	UIObject(InstanceClass::IC_UIText, "UIText", data),
	textColor(glm::vec3(1, 1, 1)),
	font(FontFace::Consola),
	textSize((unsigned int)20)
{
	for (const auto& item : data.items())
    {
        std::string key = item.key();
		const auto& value = item.value();
		
        if (key == "text") {
			text = value;
		} else if (key == "font") {
			if (value == "consola") {
				font = FontFace::Consola;
			} else {
				std::cerr << "invalid font " << value << std::endl;
				exit(-1);
			}
		} else if (key == "textSize") {
			textSize = (unsigned int)value;
		} else if (key == "textColor") {
			textColor = glm::vec3(value[0], value[1], value[2]);
		} else if (key == "textTransparency") {
			textTransparency = value;
		} else if (key == "textAnchorPoint") {
			textAnchorPoint = glm::vec2(value[0], value[1]);
		} else if (key == "textPadding") {
			textPadding = glm::vec2(value[0], value[1]);
		}
    }
}

void UIText::render() {
	UIObject::render();

	glm::vec2 bounds = Text::getTextBounds(text, font, textSize);
	glm::vec2 textTL = getAbsolutePosition() + textPadding + textAnchorPoint * (getAbosluteSize() - textPadding * 2.0f);
	textTL -= textAnchorPoint * bounds;

	std::stringstream ss(text);
	std::string line;
	int i = 0;
	while(std::getline(ss, line, '\n')){
		int lineWidth = Text::getTextBounds(line, font, textSize).x;
		int relX = bounds.x * textAnchorPoint.x - textAnchorPoint.x * lineWidth;
		Text::renderText(
			line, 
			glm::ivec2(textTL.x + relX, textTL.y + textSize + i * (textSize * 1.5)),
			font, 
			textSize,
			textColor
		);

		i++;
	}
}